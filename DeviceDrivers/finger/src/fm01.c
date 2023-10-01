#include "fm01.h"


#define UART_NAME	"usart3"
#define FM01_VENDOR	"zhusl_test"
#define FM01_NAME	"finger"

static rt_device_t uart_dev;


rt_err_t fm01_control(struct rt_finger_device *finger, int cmd, void *arg);



const struct rt_finger_info		info = {
	FM01_VENDOR,
	100,
	12
};
const struct rt_finger_ops		ops = {fm01_control};


static struct rt_finger_device finger;



rt_err_t fm01_control(struct rt_finger_device *finger, int cmd, void *arg)
{
	switch (cmd)
	{
		case FINGER_CMD_WORK:
			uart_dev = rt_device_find(UART_NAME);
			if (uart_dev != RT_NULL)
			{
				if (rt_device_open(uart_dev, RT_DEVICE_FLAG_RDWR) != RT_EOK)
				{
					rt_kprintf("uart open error");
				}
			}
			else
			{
				rt_kprintf("rt_device_find not fined");
			}
			break;
		case FINGER_CMD_VERIFY:
			rt_device_write(uart_dev, RT_NULL, "\n\n>FINGER_CMD_VERIFY\n\n", sizeof("\n\n>FINGER_CMD_VERIFY\n\n")-1);
			break;
		case FINGER_CMD_DELETE:
			rt_device_write(uart_dev, RT_NULL, "\n\n>FINGER_CMD_DELETE\n\n", sizeof("\n\n>FINGER_CMD_DELETE\n\n")-1);
			break;
		case FINGER_CMD_ENROLL:
			rt_device_write(uart_dev, RT_NULL, "\n\n>FINGER_CMD_ENROLL\n\n", sizeof("\n\n>FINGER_CMD_ENROLL\n\n")-1);
			break;
		case FINGER_CMD_INFO:
			*(const struct rt_finger_info **)arg = &info;
			break;
		default:
			break;
	}
	
	return RT_EOK;
}







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


	/* step 1 init GPIO£ºdsp power¡¢touch int¡¢touch power*/

	/* step 2 init uart */

	/* step 3 enable */
}
INIT_DEVICE_EXPORT(fm01_init);






