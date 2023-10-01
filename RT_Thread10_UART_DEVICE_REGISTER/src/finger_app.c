#include "finger_app.h"


#define NAME		"finger"

#define finger_get_image()				rt_device_control(finger, FINGER_CMD_GET_IMAGE, RT_NULL)
#define finger_gen_char(buffer_id)		rt_device_control(finger, FINGER_CMD_GEN_CHAR, buffer_id)
#define finger_search(user_id)			rt_device_control(finger, FINGER_CMD_SEARCH, user_id)
#define finger_delete(user_id)			rt_device_control(finger, FINGER_CMD_DELETE_CHAR, user_id)
#define finger_get_enroll_image()		rt_device_control(finger, FINGER_CMD_GET_ENROLL_IMAGE, RT_NULL)
#define finger_reg_model()				rt_device_control(finger, FINGER_CMD_REG_MODEL, RT_NULL)
#define finger_store_model(user_id)		rt_device_control(finger, FINGER_CMD_STORE_MODEL, user_id)


/*

rt_err_t rt_mb_init(rt_mailbox_t mb,
				  const char* name,
				  void* msgpool,
				  rt_size_t size,
				  rt_uint8_t flag)
rt_err_t rt_mb_send (rt_mailbox_t mb, rt_uint32_t value);

*/
ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t finger_task[128];
static struct rt_thread finger_thread;

#define FINGER_THREAD_PRIORITY         6
#define FINGER_THREAD_STACK_SIZE       256
#define FINGER_THREAD_TIMESLICE        5
static volatile struct thread_argv verify_arg, enroll_arg, delete_arg;
static rt_thread_t thread = RT_NULL;
static rt_thread_t verify_thread = RT_NULL;
static rt_thread_t enroll_thread = RT_NULL;
static rt_thread_t delete_thread = RT_NULL;

/* 指纹设备及设备信息 */
static rt_device_t finger;
static struct rt_finger_info finger_info;

/* 事件定义 */
static char mb_pool[MB_BUFFER_SIZE];
static event_value finger_event_value;
static event finger_event;

/* 功耗敏感设备定义 */
static struct mypm_device finger_pm_device = {};

/* 触摸中断标志位定义 */
static volatile uint8_t finger_touch_flag = 0;

int finger_app_init(void)
{
	rt_err_t err;
	
	finger = rt_device_find(NAME);
	if (finger == RT_NULL)
	{
		return RT_ERROR;
	}

	err = rt_device_open(finger, RT_DEVICE_FLAG_RDWR);
	if (err != RT_EOK)
	{
		return err;
	}

	/* 注册事件管理器，用于接收事件消息 */
	err = event_register(&finger_event,
	                    NAME,              
	                    &mb_pool[0],           
	                    sizeof(mb_pool) / 4,   
	                    RT_IPC_FLAG_FIFO);

	/* 设置触摸中断回调函数 */
	err = rt_device_control(finger, FINGER_CMD_SET_TOUCH_IRQ, (void *)finger_touch_irq_call);

	/* 注册功耗敏感设备 */
	err = mypm_register(&finger_pm_device,
											NAME,
											finger_voter,
											finger_sleep);

	/* 开启finger主线程 */
  err = rt_thread_init(&finger_thread,
  											NAME,
  											finger_thread_entry,
  											RT_NULL,
  											(rt_uint8_t *)&finger_task[0],
  											sizeof(finger_task),
  											FINGER_THREAD_PRIORITY - 1,
  											FINGER_THREAD_TIMESLICE);
  if (err == RT_EOK)
  {
      rt_thread_startup(&finger_thread);
  }
    
	return err;
}
INIT_APP_EXPORT(finger_app_init);


void finger_verify_thread(void *parameter)
{
	uint8_t buffer_id = 1;
	uint16_t user_id;
	struct thread_argv *argv = (struct thread_argv *)parameter;

	while (argv->run_flag)
	{
		if (finger_get_image() == RT_EOK)
		{
			mypm_timer_reset(&finger_pm_device);
			
			finger_event_value.type = event_type_input_finger;
			event_send((rt_uint32_t)&finger_event_value);
			if (finger_gen_char((void *)&buffer_id) == RT_EOK)
			{
				finger_event_value.data.verify.user_id = user_id;
				finger_event_value.type = event_type_verify_finger;			
				if (finger_search((void *)&user_id) == RT_EOK)
				{
					finger_event_value.data.verify.result = RT_EOK;
					event_send((rt_uint32_t)&finger_event_value);

					return;
				}
			}

			finger_event_value.data.verify.result = RT_ERROR;
			event_send((rt_uint32_t)&finger_event_value);
			rt_thread_mdelay(2000);
		}
		else
		{
			rt_thread_mdelay(250);
		}
	}
}


void finger_delete_thread(void *parameter)
{
	uint8_t buffer_id = 1;
	uint16_t user_id;
	struct thread_argv *argv = (struct thread_argv *)parameter;

	while (argv->run_flag)
	{
		if (finger_get_image() == RT_EOK)
		{
			mypm_timer_reset(&finger_pm_device);
			
			finger_event_value.type = event_type_input_finger;
			event_send((rt_uint32_t)&finger_event_value);
			if (finger_gen_char((void *)&buffer_id) == RT_EOK)
			{	
				if (finger_search((void *)&user_id) == RT_EOK)
				{
					if (finger_delete((void *)&user_id) == RT_EOK)
					{
						finger_event_value.type = event_type_delete_finger;
						finger_event_value.data.delete.result = RT_EOK;
						finger_event_value.data.delete.user_id = user_id;
						event_send((rt_uint32_t)&finger_event_value);
					}
					return;
				}
			}

			finger_event_value.type = event_type_delete_finger;
			finger_event_value.data.delete.result = RT_ERROR;
			event_send((rt_uint32_t)&finger_event_value);
			rt_thread_mdelay(2000);
		}
		else
		{
			rt_thread_mdelay(250);
		}
	}	
}


rt_err_t finger_delete_number(uint16_t user_id)
{
	if( finger_delete((void *)&user_id) == RT_EOK)
	{
		finger_event_value.type = event_type_delete_finger;
		finger_event_value.data.delete.result = RT_EOK;
		finger_event_value.data.delete.user_id = user_id;
		event_send((rt_uint32_t)&finger_event_value);
		return RT_EOK;
	}
	
	return RT_ERROR;
}



void finger_enroll_thread(void *parameter)
{
	struct thread_argv *argv = (struct thread_argv *)parameter;
	
	uint16_t enroll_id = *((uint16_t *)(argv->data));

	uint8_t step_total = finger_info.enroll_cnt;
	uint8_t step = 1;
	
	finger_event_value.data.enroll.enroll_id = enroll_id;
	finger_event_value.data.enroll.enroll_step = step_total;

	while (argv->run_flag)
	{
		if (finger_get_enroll_image() == RT_EOK)
		{
			mypm_timer_reset(&finger_pm_device);
			
			finger_event_value.type = event_type_input_finger;
			event_send((rt_uint32_t)&finger_event_value);
			
			if (finger_gen_char((void *)&step) == RT_EOK)
			{
				finger_event_value.type = event_type_enroll_finger;
				finger_event_value.data.enroll.result = RT_EOK;
				finger_event_value.data.enroll.now_step = step;
				event_send((rt_uint32_t)&finger_event_value);

				step++;
				if (step >= step_total)
				{
					if (finger_reg_model() == RT_EOK)
					{
						if (finger_store_model((void *)&enroll_id) == RT_EOK)
						{
							finger_event_value.data.enroll.result = RT_EOK;
							finger_event_value.data.enroll.now_step = step;
							event_send((rt_uint32_t)&finger_event_value);					
							return;
						}
					}

					// failed
					finger_event_value.data.enroll.result = RT_ERROR;
					finger_event_value.data.enroll.now_step = step;
					event_send((rt_uint32_t)&finger_event_value);
					step = 1;
				}

				
				if (step < step_total)
				{
					// not enough counter, waiting for finger leave and continue
					while (finger_get_enroll_image() == RT_EOK){rt_thread_mdelay(250);}
				}
			}
			else
			{
				finger_event_value.type = event_type_enroll_finger;
				finger_event_value.data.enroll.result = RT_ERROR;
				finger_event_value.data.enroll.now_step = step;
				event_send((rt_uint32_t)&finger_event_value);
			}
		}
		else
		{
			rt_thread_mdelay(250);
		}
	}
}



rt_err_t finger_empty(void)
{
	return rt_device_control(finger, FINGER_CMD_EMPTY, RT_NULL);
}


void finger_thread_entry(void *parameter)
{
	rt_err_t err;

	while (1)
	{
		err = event_recv(&finger_event, (rt_ubase_t *)&finger_event_value, UINT32_MAX);
		
		if (err != RT_NULL)
		{
			switch (finger_event_value.type)
			{
				case event_type_into_verify:	/* 系统进入验证流程，调起验证线程 */
				{
					/* 创建验证线程前先结束注册和删除线程，如果还在运行的话 */
					enroll_arg.run_flag = 0;
					delete_arg.run_flag = 0;
					enroll_thread = RT_NULL;
					delete_thread = RT_NULL;

					/* 创建线程 */
					if (verify_thread == RT_NULL)
					{
						verify_arg.run_flag = 1;
						verify_thread = rt_thread_create(NAME,
											                        finger_verify_thread, (void *)&verify_arg,
											                        FINGER_THREAD_STACK_SIZE,
											                        FINGER_THREAD_PRIORITY, FINGER_THREAD_TIMESLICE);
					}
					break;
				}
				case event_type_into_enroll:	/* 系统进入指纹注册流程，调起注册线程 */
				{					
					/* 创建注册线程前先结束验证和删除线程，如果还在运行的话 */
					verify_arg.run_flag = 0;
					delete_arg.run_flag = 0;
					verify_thread = RT_NULL;
					delete_thread = RT_NULL;

					/* 创建线程 */
					if (enroll_thread == RT_NULL)
					{
						enroll_arg.run_flag = 1;
						enroll_thread = rt_thread_create(NAME,
											                        finger_enroll_thread, (void *)&enroll_arg,
											                        FINGER_THREAD_STACK_SIZE,
											                        FINGER_THREAD_PRIORITY, FINGER_THREAD_TIMESLICE);
					}
					break;
				}
				case event_type_into_delete:	/* 系统进入指纹删除流程，调起删除流程 */
				{				
					/* 创建删除线程前先结束注册和验证线程，如果还在运行的话 */
					enroll_arg.run_flag = 0;
					verify_arg.run_flag = 0;
					enroll_thread = RT_NULL;
					verify_thread = RT_NULL;

					/* 创建线程 */
					if (delete_thread == RT_NULL)
					{
						delete_arg.run_flag = 1;
						delete_thread = rt_thread_create(NAME,
											                        finger_delete_thread, (void *)&delete_arg,
											                        FINGER_THREAD_STACK_SIZE,
											                        FINGER_THREAD_PRIORITY, FINGER_THREAD_TIMESLICE);
					}
					break;
				}
				case event_type_into_config:	/* 系统进入配置流程，即用户操作菜单 */
				{					
					/* 关闭验证删除注册线程 */
					verify_arg.run_flag = 0;
					enroll_arg.run_flag = 0;
					delete_arg.run_flag = 0;

					verify_thread = RT_NULL;
					enroll_thread = RT_NULL;
					delete_thread = RT_NULL;
					break;
				}
				default:
					break;
			}
		}
	}
}


rt_err_t finger_voter(void *parameter)
{
	return RT_EOK;
}


rt_err_t finger_sleep(void *parameter)
{
	return RT_EOK;
}



void finger_touch_irq_call(void)
{
	finger_touch_flag = 1;
}
