#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

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



#define MB_BUFFER_SIZE	16u

typedef enum {
	event_type_null							= 0,
	event_type_input_finger			= (0x1 << 1),
	event_type_input_touch			= (0x1 << 2),
	event_type_input_card			= (0x1 << 3),
	event_type_input_face			= (0x1 << 4),
	event_type_input_vena			= (0x1 << 5),
	event_type_input_button			= (0x1 << 6),
	event_type_input_infrared		= (0x1 << 7),
	event_type_verify_finger		= (0x1 << 8),
	event_type_verify_pass			= (0x1 << 9),
	event_type_verify_card			= (0x1 << 10),
	event_type_verify_face			= (0x1 << 11),
	event_type_verify_vena			= (0x1 << 12),
	event_type_enroll_finger		= (0x1 << 13),
	event_type_enroll_pass			= (0x1 << 14),
	event_type_enroll_card			= (0x1 << 15),
	event_type_enroll_face			= (0x1 << 16),
	event_type_enroll_vena			= (0x1 << 17),	
	event_type_delete_finger		= (0x1 << 18),
	event_type_delete_pass			= (0x1 << 19),
	event_type_delete_card			= (0x1 << 20),
	event_type_delete_face			= (0x1 << 21),
	event_type_delete_vena			= (0x1 << 22),

	event_type_into_verify			= (0x1 << 23),	/* 进入验证流程 */
	event_type_into_enroll			= (0x1 << 24),	/* 进入注册流程 */
	event_type_into_delete			= (0x1 << 25),	/* 进入删除流程 */
	event_type_into_config			= (0x1 << 26),	/* 进入配置流程，即用户菜单操作 */
} event_type;






typedef struct {
	uint8_t result;
	uint16_t user_id;
} event_data_verify, event_data_delete, *event_data_verify_t, *event_data_delete_t;


typedef struct {
	uint8_t result;
	uint16_t enroll_id;
	uint8_t enroll_step;
	uint8_t now_step;
} event_data_enroll, *event_data_enroll_t;



typedef struct str_event_value{
	uint32_t	type;
	union {
	event_data_verify verify;
	event_data_enroll enroll;
	event_data_delete delete;
	} data;
} event_value, *event_value_t;




typedef struct str_event{
	struct rt_mailbox	mb;
	struct str_event 	*p_next;
} event, *event_t;




rt_err_t event_register(event_t p_event,
		                    const char  *name,
		                    void        *msgpool,
		                    rt_size_t    size,
		                    rt_uint8_t   flag);
void event_send(rt_uint32_t value);
void event_send_special(rt_uint32_t value, const char *name);
rt_err_t event_recv(event_t event, rt_ubase_t *value, rt_int32_t timeout);


#endif

