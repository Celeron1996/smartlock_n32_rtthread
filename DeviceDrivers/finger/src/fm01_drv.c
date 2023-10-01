#include "fm01_drv.h"
#include "finger.h"


#define UART_NAME	"usart3"
#define FM01_VENDOR	"zhusl_test"
#define FM01_NAME	"finger"


static rt_err_t fm01_rx_err_check(rt_err_t err);
static void fm01_tx_gen_sum(uint8_t *data);
static rt_err_t fm01_rx_check_sum(uint8_t *data);
static rt_err_t fm01_tx_rx_data(uint8_t *tx_data, uint8_t tx_size, uint8_t rx_size, uint16_t rx_timeout_ms);
static void fm01_memset(uint8_t *data, uint8_t size, uint8_t value);
static void fm01_send_data(uint8_t *data, uint8_t length);
static rt_err_t fm01_open(void);
static rt_err_t fm01_close(void);



rt_err_t fm01_control(struct rt_finger_device *finger, int cmd, void *arg);


static volatile uint8_t fm01_rx_buffer[FM01_RX_BUFFER_SIZE];
static volatile uint8_t fm01_rx_counter = 0;

const struct rt_finger_info		info = {
	FM01_VENDOR,
	100,
	12
};

const struct rt_finger_ops		ops = {fm01_control};

static struct rt_finger_device finger;



int fm01_init(void)
{
	finger.ops = &ops;
	finger.info = &info;
	
    /* register UART device */
    rt_finger_register(&finger,
                          FM01_NAME,
                          RT_DEVICE_FLAG_RDWR,
                          RT_NULL);

	return RT_EOK;                          
}
INIT_DEVICE_EXPORT(fm01_init);


rt_err_t fm01_control(struct rt_finger_device *finger, int cmd, void *arg)
{
	switch (cmd)
	{
		case FINGER_CMD_OPEN:
		{
			return fm01_open();
		}
		case FINGER_CMD_CLOSE:
		{
			return fm01_close();
		}
		case FINGER_CMD_READ_INFO:
		{
			*(const struct rt_finger_info **)arg = &info;
			return RT_EOK;
		}
		case FINGER_CMD_GET_IMAGE:
		{
			return fm01_get_image();
		}
		case FINGER_CMD_GEN_CHAR:
		{
			return fm01_gen_char(*((uint8_t *)arg));
		}
		case FINGER_CMD_SEARCH:
		{
			return fm01_search(1, 0, info.user_total, (uint16_t *)arg);
		}
		case FINGER_CMD_GET_ENROLL_IMAGE:
		{
			return fm01_get_enroll_image();
		}
		case FINGER_CMD_REG_MODEL:
		{
			return fm01_reg_model();
		}
		case FINGER_CMD_STORE_MODEL:
		{
			return fm01_store_model(1, *((uint16_t *)arg));
		}
		case FINGER_CMD_DELETE_CHAR:
		{
			return fm01_delete_char(*((uint16_t *)arg), 1);
		}
		case FINGER_CMD_EMPTY:
		{
			return fm01_empty();
		}
		case FINGER_CMD_SET_TOUCH_IRQ:
		{
			
		}
		default:
		{
			return RT_ERROR;
		}
	}
}


static rt_err_t fm01_open(void)
{
	/* hardware init */
	fm01_gpio_init();
	rt_thread_mdelay(10);
	
	fm01_uart_init();
	rt_thread_mdelay(10);
	
	fm01_dsp_vcc_control(ENABLE);

	rt_thread_mdelay(100);

	return RT_EOK;
}


static rt_err_t fm01_close(void)
{
	return RT_EOK;
}


static void fm01_send_data(uint8_t *data, uint8_t length)
{
	uint8_t *temp = data;

	rt_enter_critical();
	
	while (length--)
	{
		fm01_uart_putc(FM01_UART, *temp++);
	}

	rt_exit_critical();
}




static void fm01_memset(uint8_t *data, uint8_t size, uint8_t value)
{
	uint8_t *temp = data;

	while (size--)
	{
		*temp++ = value;
	}
}


static rt_err_t fm01_tx_rx_data(uint8_t *tx_data, uint8_t tx_size, uint8_t rx_size, uint16_t rx_timeout_ms)
{	
	fm01_tx_gen_sum(tx_data);
	
	fm01_memset((uint8_t *)fm01_rx_buffer, sizeof(fm01_rx_buffer), 0);
	fm01_rx_counter = 0;

	fm01_send_data(tx_data, tx_size);

	while (rx_timeout_ms--)
	{
		if (fm01_rx_counter >=rx_size)
		{
			break;
		}
		rt_thread_delay(1);
	}

	if (fm01_rx_counter < rx_size)
	{
		return RT_ERROR;
	}
	else
	{
		return fm01_rx_check_sum((uint8_t *)fm01_rx_buffer);
	}
}


static rt_err_t fm01_rx_check_sum(uint8_t *data)
{
	uint16_t sum = 0, length, i;
	
	//包标识是否为应答包标识
	if(data[6] == 0x07)
	{
		length = (((uint16_t)data[7]) << 8) | (data[8]) + 1;
		if((length + 8) <= FM01_RX_BUFFER_SIZE)
		{
		  for(i = 0; i < length; i++)
		  {
			sum += data[6 + i];
		  }
		  if(sum == ((((uint16_t)data[length + 6]) << 8) | (data[length + 7])))
			return RT_EOK;
		}		
	}
	return RT_ERROR;
}


static void fm01_tx_gen_sum(uint8_t *data)
{
	uint16_t sum = 0, length, i;
	
	length = (((uint16_t)data[7]) << 8) | (data[8]) - 2;	//减去检验和的两个字节
	
	sum += (data[6] + data[7] + data[8]);		//包标识加包长度
	
	for(i = 0; i < length; i++)
	{
		sum += data[9 + i];
	}
	
	data[9 + length] = (u8)(sum >> 8);		//高位
	data[9 + length + 1] = (u8)sum;				//低位
}


static rt_err_t fm01_rx_err_check(rt_err_t err)
{
	if (err != RT_EOK)
	{
		return RT_ERROR;
	}
	else
	{
		if (RS_SUCCESS != fm01_rx_buffer[9])
		{
			return RT_ERROR;
		}
		else
		{
			return RT_EOK;
		}
	}
}


rt_err_t fm01_get_image(void)
{
	uint8_t dat_buf[] = {	//包头
							0xEF, 0x01, 
							//芯片地址
							0xFF, 0xFF, 0xFF, 0xFF, 
							//包标识
							0x01, 
							//包长度
							0x00, 0x03, 
							//指令码
							CM_GetImage, 
							//校验和
							0x00, 0x05};
	rt_err_t err;

	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_GetImage_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;
}



rt_err_t fm01_get_enroll_image(void)
{
	uint8_t dat_buf[] = {	//包头
							0xEF, 0x01, 
							//芯片地址
							0xFF, 0xFF, 0xFF, 0xFF, 
							//包标识
							0x01, 
							//包长度
							0x00, 0x03, 
							//指令码
							CM_GetEnrollImage, 
							//校验和
							0x00, 0x05};
	rt_err_t err;

	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_GetEnrollImage_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;
}



rt_err_t fm01_sleep(void)
{
	uint8_t dat_buf[] = {	//包头
							0xEF, 0x01, 
							//芯片地址
							0xFF, 0xFF, 0xFF, 0xFF, 
							//包标识
							0x01, 
							//包长度
							0x00, 0x03, 
							//指令码
							CM_Sleep, 
							//校验和
							0x00, 0x37};
	rt_err_t err;

	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_Sleep_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;	
}



rt_err_t fm01_gen_char(uint8_t buffer_id)
{
	uint8_t dat_buf[] = {	//包头
						0xEF, 0x01, 
						//芯片地址
						0xFF, 0xFF, 0xFF, 0xFF, 
						//包标识
						0x01, 
						//包长度
						0x00, 0x04, 
						//指令码
						CM_GenChar,
						//缓冲区号
						0x00,
						//校验和
						0x00, 0x00};	
	rt_err_t err;
						
	dat_buf[10] = buffer_id;

	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_GenChar_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;							
}



rt_err_t fm01_search(uint8_t buffer_id, uint16_t start_page, uint16_t page_num, uint16_t *user_num)
{
	uint8_t dat_buf[] = {	//包头
							0xEF, 0x01, 
							//芯片地址
							0xFF, 0xFF, 0xFF, 0xFF, 
							//包标识
							0x01, 
							//包长度
							0x00, 0x08, 
							//指令码
							CM_Search,
							//缓冲区号
							0x00,
							//起始页(start_page)
							0x00, 0x00,
							//页数(page_num)
							0x00, 0x00,
							//校验和
							0x00, 0x00};
	rt_err_t err;

	dat_buf[10] = buffer_id;
	//起始页赋值
	dat_buf[11] = (uint8_t)(start_page >> 8);
	dat_buf[12] = (uint8_t)start_page;
	//页数赋值
	dat_buf[13] = (uint8_t)(page_num >> 8);
	dat_buf[14] = (uint8_t)page_num;	
	
	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_Search_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)	//搜索的指纹存在，获取其ID号
	{
		*user_num = (((uint16_t)(fm01_rx_buffer[10])) << 8) | ((uint16_t)(fm01_rx_buffer[11]));
		return RT_EOK;
	}

	return RT_ERROR;
}


rt_err_t fm01_reg_model(void)
{
	uint8_t dat_buf[] = {	//包头
							0xEF, 0x01, 
							//芯片地址
							0xFF, 0xFF, 0xFF, 0xFF, 
							//包标识
							0x01, 
							//包长度
							0x00, 0x03, 
							//指令码
							CM_RegModel, 
							//校验和
							0x00, 0x09};
	rt_err_t err;							

	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_RegModel_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;	
}


rt_err_t fm01_store_model(uint8_t buffer_id, uint16_t page_id)
{
	uint8_t dat_buf[] = {//包头
									0xEF, 0x01, 
									//芯片地址
									0xFF, 0xFF, 0xFF, 0xFF, 
									//包标识
									0x01, 
									//包长度
									0x00, 0x06, 
									//指令码
									CM_StoreChar,
									//缓冲区号
									0x00,
									//位置号(page_id)
									0x00, 0x00,
									//校验和
									0x00, 0x00};
	rt_err_t err;	
	
	dat_buf[10] = buffer_id;
	//位置号赋值
	dat_buf[11] = (uint8_t)(page_id >> 8);
	dat_buf[12] = (uint8_t)page_id;
	
	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_StoreChar_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;	
}


rt_err_t fm01_delete_char(uint16_t page_id, uint16_t num)
{
	uint8_t dat_buf[] = {//包头
									0xEF, 0x01, 
									//芯片地址
									0xFF, 0xFF, 0xFF, 0xFF, 
									//包标识
									0x01, 
									//包长度
									0x00, 0x07, 
									//指令码
									CM_DeletChar,
									//模板号(page_id)
									0x00, 0x00,
									//删除个数(num)
									0x00, 0x00,
									//校验和
									0x00, 0x00};
	rt_err_t err;
	
	//起始页赋值
	dat_buf[10] = (uint8_t)(page_id >> 8);
	dat_buf[11] = (uint8_t)page_id;
	//页数赋值
	dat_buf[12] = (uint8_t)(num >> 8);
	dat_buf[13] = (uint8_t)num;	

	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_StoreChar_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;	
}


rt_err_t fm01_empty(void)
{
	uint8_t dat_buf[] = {	//包头
							0xEF, 0x01, 
							//芯片地址
							0xFF, 0xFF, 0xFF, 0xFF, 
							//包标识
							0x01, 
							//包长度
							0x00, 0x03, 
							//指令码
							CM_Empty, 
							//校验和
							0x00, 0x11};
	rt_err_t err;
	
	err = fm01_tx_rx_data((uint8_t *)dat_buf, sizeof(dat_buf), CM_Empty_RxSize, FM01_RX_TIMEOUT_MS);

	if (fm01_rx_err_check(err) == RT_EOK)
	{
		return RT_EOK;
	}

	return RT_ERROR;	
}


void fm01_uart_int_getc(uint8_t data)
{
	fm01_rx_buffer[fm01_rx_counter++] = data;
	if (fm01_rx_counter >= sizeof(fm01_rx_buffer))
	{
		fm01_rx_counter = sizeof(fm01_rx_buffer) - 1;
	}
}


