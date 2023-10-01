#ifndef FM01_BSP_H
#define FM01_BSP_H


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
#include "n32l40x.h"


/* gpio define : dsp power enable */
#define FM01_DSP_VCC_GPIO_PORT				GPIOA
#define FM01_DSP_VCC_GPIO_PIN				GPIO_PIN_1
#define FM01_DSP_VCC_GPIO_CLK_ENABLE()		do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);}while(0)
#define FM01_DSP_VCC_GPIO_CLK_DISABLE()		do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE);}while(0)


/* gpio define : touch vcc enable */
#define FM01_TOUCH_VCC_GPIO_PORT			GPIOA
#define FM01_TOUCH_VCC_GPIO_PIN				GPIO_PIN_1
#define FM01_TOUCH_VCC_GPIO_CLK_ENABLE()	do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);}while(0)
#define FM01_TOUCH_VCC_GPIO_CLK_DISABLE()	do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE);}while(0)


/* gpio define : touch int enable */
#define FM01_TOUCH_INT_GPIO_PORT			GPIOA
#define FM01_TOUCH_INT_GPIO_PIN				GPIO_PIN_1
#define FM01_TOUCH_INT_GPIO_CLK_ENABLE()	do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);}while(0)
#define FM01_TOUCH_INT_GPIO_CLK_DISABLE()	do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE);}while(0)
/* touch int EXIT */
#define FM01_TOUCH_INT_EXIT_SOURCE_PORT		GPIOA_PORT_SOURCE
#define FM01_TOUCH_INT_EXIT_SOURCE_PIN		GPIO_PIN_SOURCE1
#define FM01_TOUCH_INT_EXIT_LINE			EXTI_LINE1
/* touch int : Configure the NVIC Preemption Priority Bits */
#define FM01_TOUCH_INT_NVIC_IRQ_CHANNEL		EXTI1_IRQn
/* touch gpio exti call define */
#define fm01_touch_irq_call						exti0_irqhandler_call

/* gpio define : uart rx */
#define FM01_RX_GPIO_PORT					GPIOA
#define FM01_RX_GPIO_PIN					GPIO_PIN_10
#define FM01_RX_GPIO_CLK_ENABLE()			do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);}while(0)
#define FM01_RX_GPIO_CLK_DISABLE()			do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE);}while(0)
#define FM01_RX_GPIO_AF						GPIO_AF4_USART1

/* gpio define : uart tx */
#define FM01_TX_GPIO_PORT					GPIOA
#define FM01_TX_GPIO_PIN					GPIO_PIN_9
#define FM01_TX_GPIO_CLK_ENABLE()			do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);}while(0)
#define FM01_TX_GPIO_CLK_DISABLE()			do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, DISABLE);}while(0)
#define FM01_TX_GPIO_AF						GPIO_AF4_USART1

/* uart config define */
#define FM01_UART							USART1
#define FM01_UART_BAUDRATE					57600u
#define FM01_UART_CLK_ENABLE()				do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);}while(0)
#define FM01_UART_CLK_DISABLE()				do{RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, DISABLE);}while(0)
/* uart interrupt config */
#define FM01_UART_NVIC_IRQ_CHANNEL			USART1_IRQn

/* uart interrupt handler */
#define FM01_UART_IRQHANDLER				USART1_IRQHandler

/* dsp vcc and touch vcc control */
#define fm01_dsp_vcc_control(state)			do {GPIO_WriteBit(FM01_DSP_VCC_GPIO_PORT, FM01_DSP_VCC_GPIO_PIN, (state == ENABLE)?(Bit_SET):(Bit_RESET));}while(0)
#define fm01_touch_vcc_control(state)		do {GPIO_WriteBit(FM01_TOUCH_VCC_GPIO_PORT, FM01_TOUCH_VCC_GPIO_PIN, (state == ENABLE)?(Bit_SET):(Bit_RESET));}while(0)


/* uart transmitter byte data */
#define fm01_uart_putc(uart, data)			do {USART_SendData(uart, (uint8_t)data);while (USART_GetFlagStatus(uart, USART_FLAG_TXDE) == RESET);}while(0)




void fm01_gpio_init(void);
void fm01_uart_init(void);
void fm01_uart_deinit(void);
void fm01_exit_config(FunctionalState cmd);




#endif

