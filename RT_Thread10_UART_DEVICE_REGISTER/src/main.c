/*****************************************************************************
 * Copyright (c) 2022, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file main.c
 * @author Nations
 * @version v1.2.0
 *
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 */

#include <rtthread.h>
#include "drv_gpio.h"
#include "pin.h"

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "finger.h"

#include "event_manager.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t test0_stack[ 256 ], test1_stack[ 2048 ], test0_finger_stack[256], test1_card_stack[256], test2_pass_stack[256], recv_stack[256];

static struct rt_thread test0_thread;
static struct rt_thread test1_thread;
static struct rt_thread test0_finger_thread;
static struct rt_thread test1_card_thread;
static struct rt_thread test2_pass_thread;
static struct rt_thread recv_thread;



struct rt_event system_event;

#define SYS_EVENT_UART_RX_FINISH    0x00000001 /* UART receive data finish event */

typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;

TestStatus Buffercmp8bit(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength);
TestStatus Buffercmp16bit(uint16_t* pBuffer, uint16_t* pBuffer1, uint16_t BufferLength);
TestStatus Buffercmp32bit(uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);
TestStatus TransferStatus1 = FAILED, TransferStatus2 = PASSED;;

#define LED1_PIN        GET_PIN(A,  8)
#define LED2_PIN        GET_PIN(B,  4)


/* Receive data callback function 
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    rt_event_send(&system_event, SYS_EVENT_UART_RX_FINISH);
    return RT_EOK;
}*/

/**
 * @brief  test0 thread entry
 */
static void test0_thread_entry(void* parameter)
{
    while(1)
    {
        rt_thread_delay(100);    //delay 250ms
        rt_pin_write(LED1_PIN, PIN_HIGH);
        rt_thread_delay(100);    //delay 250ms
        rt_pin_write(LED1_PIN, PIN_LOW);
    }
}

//static event event0;
//static char test0_pool[MB_BUFFER_SIZE];
static event_value test0_value;
static void test0_finger_thread_entry(void *parameter)
{
	//event_register(&event0, "test_finger", (void *)&test0_pool[0], sizeof(test0_pool) / 4, RT_IPC_FLAG_FIFO);
	test0_value.type = event_type_input_finger;
	test0_value.data.verify.result = 0;
	test0_value.data.verify.user_id = 0;
	while (1)
	{
		event_send((rt_uint32_t)&test0_value);
		rt_thread_mdelay(1000);
		test0_value.data.verify.user_id++;
	}
}


//static event event1;
//static char test1_pool[MB_BUFFER_SIZE];
static event_value test1_value;
static void test1_card_thread_entry(void *parameter)
{
	//event_register(&event1, "test_card", (void *)&test1_pool[0], sizeof(test1_pool) / 4, RT_IPC_FLAG_FIFO);
	test1_value.type = event_type_input_card;
	test1_value.data.verify.result = 0;
	test1_value.data.verify.user_id = 0;
	while (1)
	{
		event_send((rt_uint32_t)&test1_value);
		rt_thread_mdelay(1000);
		test1_value.data.verify.user_id++;
	}
}


//static event event2;
//static char test2_pool[MB_BUFFER_SIZE];
static event_value test2_value;
static void test2_pass_thread_entry(void *parameter)
{
	//event_register(&event2, "test_finger", (void *)&test2_pool[0], sizeof(test2_pool) / 4, RT_IPC_FLAG_FIFO);
	test2_value.type = event_type_input_touch;
	test2_value.data.verify.result = 0;
	test2_value.data.verify.user_id = 0;
	while (1)
	{
		event_send((rt_uint32_t)&test2_value);
		rt_thread_mdelay(1000);
		test2_value.data.verify.user_id++;
	}
}

static event event_recv_;
static char event_recv_pool[MB_BUFFER_SIZE];
static event_value_t event_recv_value_t;
static void recv_thread_entry(void *parameter)
{
	rt_err_t err;
	event_register(&event_recv_, "recv", (void *)&event_recv_pool[0], sizeof(event_recv_pool) / 4, RT_IPC_FLAG_FIFO);
	
	while (1)
	{
		err = event_recv(&event_recv_, (rt_ubase_t *)&event_recv_value_t, RT_UINT32_MAX);
		if (err == RT_EOK)
		{
			rt_kprintf("recv event ! type:%d, result:%d, user_id:%d\n", event_recv_value_t->type, event_recv_value_t->data.verify.result, event_recv_value_t->data.verify.user_id);
		}
		
	}
}


/**
 * @brief  test1 thread entry
 */
static void test1_thread_entry(void* parameter)
{

	#if 0
    uint8_t data1[30] = {"secv:"};
    uint16_t data_length = 0;
    rt_uint32_t sys_event_recv = 0;
    
    //if(RT_EOK == rt_device_open(uart_dev, RT_DEVICE_FLAG_INT_RX))
    //{
       rt_device_write(uart_dev, RT_NULL, "\n\n\n\nHello rt-rhread!!!!!!!!!!!!!!", sizeof("\n\n\n\nHello rt-rhread!!!!!!!!!!!!!!")-1); 
    //}
    while(1)
    {
        //rt_thread_delay(50);
        /* wait uart get data event */
        if(RT_EOK == rt_event_recv(&system_event, SYS_EVENT_UART_RX_FINISH, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 10, &sys_event_recv))
        {
            data_length = rt_device_read(uart_dev, RT_NULL, data1+5, RT_SERIAL_RB_BUFSZ);
            rt_device_write(uart_dev, RT_NULL, data1, data_length+5); 
        }   
    }
	#endif

    rt_device_t finger_;
    rt_err_t err;
	struct rt_finger_info *info;
	
    finger_ = rt_device_find("finger");

    if (finger_ == RT_NULL)
    {
    	rt_kprintf("finger_ not fined");
    	while(1)rt_thread_delay(500*RT_TICK_PER_SECOND/1000);
    }
    
    err = rt_device_open(finger_, RT_DEVICE_FLAG_RDWR);

    if (err != RT_EOK)
    {
    	rt_kprintf("rt_device_open not fined");
    	while(1)rt_thread_delay(500*RT_TICK_PER_SECOND/1000);
    }    

    

    rt_device_control(finger_, FINGER_CMD_READ_INFO, (void *)&info);

    rt_kprintf("\n\nuser total:%d, enroll cnt:%d, vendor:%s\n\n", info->user_total, info->enroll_cnt, info->vendor);


    while(1)
    {	
    	/*  rt_thread_mdelay(100);  */

    	 err = rt_device_control(finger_, FINGER_CMD_GET_IMAGE, RT_NULL);
    	//err = rt_device_control(finger_, FINGER_CMD_READ_INFO, (void *)&info);

    	if (err != RT_EOK)
    	{
    		rt_kprintf("\nfinger get image error!\n");
    		rt_pin_write(LED1_PIN, PIN_HIGH);
    	}
    	else
    	{
    		rt_kprintf("\nfinger get image ok!\n");
    		rt_pin_write(LED1_PIN, PIN_LOW);
    	}
    	
    	rt_thread_mdelay(200);
    	
    }
}

#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */

/**
 * @brief  Main program
 */
int main(void)
{
    rt_err_t result;

    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    
    /* init uart_rx_event */
    result = rt_event_init(&system_event, "event", RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        rt_kprintf("init event failed.\n");
    }  

    /* init test0 thread */
    result = rt_thread_init(&test0_thread, "test0", test0_thread_entry, RT_NULL, (rt_uint8_t*)&test0_stack[0], sizeof(test0_stack), 4, 5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&test0_thread);
    }
    /* init test1 thread */
    result = rt_thread_init(&test1_thread, "test1", test1_thread_entry, RT_NULL, (rt_uint8_t*)&test1_stack[0], sizeof(test1_stack), 4, 5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&test1_thread);
    }

    result = rt_thread_init(&recv_thread, "recv", recv_thread_entry, RT_NULL, (rt_uint8_t*)&recv_stack[0], sizeof(recv_stack), 4, 5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&recv_thread);
    }
    
	result = rt_thread_init(&test0_finger_thread, "finger", test0_finger_thread_entry, RT_NULL, (rt_uint8_t*)&test0_finger_stack[0], sizeof(test0_finger_stack), 4, 5);
	if (result == RT_EOK)
	{
		rt_thread_startup(&test0_finger_thread);
	}
	
    result = rt_thread_init(&test1_card_thread, "card", test1_card_thread_entry, RT_NULL, (rt_uint8_t*)&test1_card_stack[0], sizeof(test1_card_stack), 4, 5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&test1_card_thread);
    }
    
    result = rt_thread_init(&test2_pass_thread, "pass", test2_pass_thread_entry, RT_NULL, (rt_uint8_t*)&test2_pass_stack[0], sizeof(test2_pass_stack), 4, 5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&test2_pass_thread);
    }


   // while(1)
  //  {
   //     rt_thread_delay(100);
   // }
}

#if 0
int my_finger_test_init(void)
{
	uart_dev = rt_device_find("usart3");
	rt_device_open(uart_dev, RT_DEVICE_FLAG_INT_RX);
	/* Set the interrupt callback function */
	rt_device_set_rx_indicate(uart_dev, uart_input);

	return 0;
}

INIT_DEVICE_EXPORT(my_finger_test_init);
#endif


/**
 * @brief  Compares two buffers.
 * @param  pBuffer, pBuffer1: buffers to be compared.
 * @param BufferLength buffer's length
 * @return PASSED: pBuffer identical to pBuffer1
 *         FAILED: pBuffer differs from pBuffer1
 */
TestStatus Buffercmp8bit(uint8_t* pBuffer, uint8_t* pBuffer1, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer != *pBuffer1)
        {
            return FAILED;
        }

        pBuffer++;
        pBuffer1++;
    }

    return PASSED;
}

/**
 * @brief  Compares two buffers.
 * @param  pBuffer, pBuffer1: buffers to be compared.
 * @param BufferLength buffer's length
 * @return PASSED: pBuffer identical to pBuffer1
 *         FAILED: pBuffer differs from pBuffer1
 */
TestStatus Buffercmp16bit(uint16_t* pBuffer, uint16_t* pBuffer1, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer != *pBuffer1)
        {
            return FAILED;
        }

        pBuffer++;
        pBuffer1++;
    }

    return PASSED;
}

/**
 * @brief  Compares two buffers.
 * @param  pBuffer, pBuffer1: buffers to be compared.
 * @param BufferLength buffer's length
 * @return PASSED: pBuffer identical to pBuffer1
 *         FAILED: pBuffer differs from pBuffer1
 */
TestStatus Buffercmp32bit(uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
    while (BufferLength--)
    {
        if (*pBuffer != *pBuffer1)
        {
            return FAILED;
        }

        pBuffer++;
        pBuffer1++;
    }

    return PASSED;
}



/*@}*/
