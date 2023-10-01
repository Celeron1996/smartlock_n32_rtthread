#ifndef FINGER_H
#define FINGER_H

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


/**
 * finger status
 */
enum rt_finger_status {
	FINGER_STATUS_STANDBY,
	FINGER_STATUS_SLEEP,
	FINGER_STATUS_BUSY,
	FINGER_STATUS_DEINIT
};


/**
 * finger device
 */
struct rt_finger_device {
	struct rt_device								parent;
	enum rt_finger_status						status;
	const struct rt_finger_info *		info;
	const struct rt_finger_ops *		ops;
};


/**
 * finger operators
 */
struct rt_finger_ops
{
    rt_err_t (*control)(struct rt_finger_device *finger, int cmd, void *arg);
};


/**
 * finger info
 */
struct rt_finger_info {
	const char *			vendor;
	const uint16_t			user_total;
	const uint8_t				enroll_cnt;
};


/**
 * finger command
 */
enum rt_finger_cmd {
	FINGER_CMD_OPEN,
	FINGER_CMD_CLOSE,
	FINGER_CMD_SLEEP,
	FINGER_CMD_READ_INFO,
	FINGER_CMD_GET_IMAGE,
	FINGER_CMD_GEN_CHAR,
	FINGER_CMD_SEARCH,
	FINGER_CMD_GET_ENROLL_IMAGE,
	FINGER_CMD_REG_MODEL,
	FINGER_CMD_STORE_MODEL,
	FINGER_CMD_DELETE_CHAR,
	FINGER_CMD_EMPTY,
	FINGER_CMD_SET_TOUCH_IRQ
};


/**
 * finger arg
 */
typedef union {
	struct rt_finger_verify_arg {
		uint16_t num;
	} verify;
	struct rt_finger_delet_arg {
		uint16_t num;
	} delet;
	struct rt_finger_enroll_arg {
		uint16_t num;
		uint8_t step;
	} enroll;
} rt_finger_arg;


/*
 * finger register
 */
rt_err_t rt_finger_register(struct rt_finger_device *finger,
                               const char              *name,
                               rt_uint32_t              flag,
                               void                    *data);


#endif
