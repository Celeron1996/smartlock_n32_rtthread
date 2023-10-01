#ifndef COMMON_H
#define COMMON_H


#include <rtthread.h>








struct thread_argv{
	uint8_t run_flag;		/* 运行标志 */
	uint8_t cre_flag;		/* 标志该线程是否创建 */
	void 	*data;
};






#endif
