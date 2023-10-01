#include "mypm.h"



/* 功耗管理器初始化，创建功耗管理线程、创建软件定时器 */
int mypm_init(void)
{
	return RT_EOK;
}
INIT_COMPONENT_EXPORT(mypm_init);



/* 注册功耗敏感设备         */
rt_err_t mypm_register(struct mypm_device *device, const char *name, rt_err_t (*voter)(void *parameter), rt_err_t (*sleep)(void *parameter))
{
	return RT_EOK;
}


/* 功耗管理线程 */
void mypm_thread_entry(void *parameter)
{
	
}


/* 复位软件定时器 */
void mypm_timer_reset(struct mypm_device *device)
{

}

