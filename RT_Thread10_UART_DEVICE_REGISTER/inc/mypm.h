#ifndef MYPM_H
#define MYPM_H


#include <rtthread.h>

#include "ipc/ringbuffer.h"
#include "ipc/completion.h"
#include "ipc/dataqueue.h"
#include "ipc/workqueue.h"
#include "ipc/waitqueue.h"
#include "ipc/pipe.h"
#include "ipc/poll.h"
#include "ipc/ringblk_buf.h"
#include "rtdef.h"
#include "common.h"


struct mypm_device {
	const char *name;
	rt_err_t (*voter)(void *parameter);
	void (*sleep)(void *parameter);
	struct mypm_device *next;
};


/* 注册功耗敏感设备         */
rt_err_t mypm_register(struct mypm_device *device, const char *name, rt_err_t (*voter)(void *parameter), rt_err_t (*sleep)(void *parameter));
/* 功耗管理线程 */
void mypm_thread_entry(void *parameter);
/* 复位软件定时器 */
void mypm_timer_reset(struct mypm_device *device);


#endif

