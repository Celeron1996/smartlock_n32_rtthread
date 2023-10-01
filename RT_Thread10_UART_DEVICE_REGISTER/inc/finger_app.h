#ifndef FINGER_APP_H
#define FINGER_APP_H

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
#include "finger.h"
#include "event_manager.h"
#include "common.h"
#include "mypm.h"

rt_err_t finger_empty(void);
void finger_enroll_thread(void *parameter);
rt_err_t finger_delete_number(uint16_t user_id);
void finger_delete_thread(void *parameter);
void finger_verify_thread(void *parameter);
void finger_thread_entry(void *parameter);
rt_err_t finger_voter(void *parameter);
rt_err_t finger_sleep(void *parameter);


#endif

