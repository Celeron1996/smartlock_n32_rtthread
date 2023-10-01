/*
 * finger.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-09-16     zhusl        first version
 */

#include <rthw.h>
#include <rtthread.h>
#include <finger.h>

#define DBG_TAG    "FINGER"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>


/* RT-Thread Device Interface */
/*
 * This function initializes finger device.
 */
static rt_err_t rt_finger_init(struct rt_device *dev)
{
    return RT_EOK;
}

static rt_err_t rt_finger_open(struct rt_device *dev, rt_uint16_t oflag)
{
	struct rt_finger_device *finger;

	finger = (struct rt_finger_device *)dev;

	return finger->ops->control(finger, FINGER_CMD_OPEN, NULL);
}

static rt_err_t rt_finger_close(struct rt_device *dev)
{
	struct rt_finger_device *finger;

	finger = (struct rt_finger_device *)dev;

	return finger->ops->control(finger, FINGER_CMD_CLOSE, NULL);
}


static rt_err_t rt_finger_control(struct rt_device *dev,
                                  int              cmd,
                                  void             *args)
{
	struct rt_finger_device *finger;

	finger = (struct rt_finger_device *)dev;

	return finger->ops->control(finger, cmd, args);
}


/*
 * finger register
 */
rt_err_t rt_finger_register(struct rt_finger_device *finger,
                               const char              *name,
                               rt_uint32_t              flag,
                               void                    *data)
{
    rt_err_t ret;
    struct rt_device *device;
    RT_ASSERT(finger != RT_NULL);

    device = &(finger->parent);

    device->type        = RT_Device_Class_Sensor;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;

    device->init        = rt_finger_init;
    device->open        = rt_finger_open;
    device->close       = rt_finger_close;
    device->read        = RT_NULL;
    device->write       = RT_NULL;
    device->control     = rt_finger_control;

    device->user_data   = data;

    /* register a character device */
    ret = rt_device_register(device, name, flag);

    return ret;
}

/* ISR for finger interrupt */
void rt_hw_finger_isr(struct rt_finger_device *finger, int event)
{

}

