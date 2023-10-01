#include "event_manager.h"


static event_t event_root = RT_NULL;


rt_err_t event_register(event_t p_event,
		                    const char  *name,
		                    void        *msgpool,
		                    rt_size_t    size,
		                    rt_uint8_t   flag)
{

	if (rt_mb_init(&p_event->mb, name, msgpool, size, flag) != RT_EOK)
	{
		return RT_ERROR;
	}

	p_event->p_next = event_root;
	event_root = p_event;	

	return RT_EOK;
}


void event_send(rt_uint32_t value)
{
	event_t event_temp = event_root;

	while (event_temp != RT_NULL)
	{
		rt_mb_send(&event_temp->mb, value);
		event_temp = event_temp->p_next;
	}
}


void event_send_special(rt_uint32_t value, const char *name)
{
	rt_object_t obj;
	
	obj = rt_object_find(name, RT_Object_Class_MailBox);

	if (obj != RT_NULL)
	{
		rt_mb_send((rt_mailbox_t)obj, value);
	}
}


rt_err_t event_recv(event_t event, rt_ubase_t *value, rt_int32_t timeout)
{
	return rt_mb_recv(&event->mb , value, timeout);
}

