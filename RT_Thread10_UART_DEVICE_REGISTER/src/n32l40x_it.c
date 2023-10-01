/*****************************************************************************
 * Copyright (c) 2022, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file n32l40x_it.c
 * @author Nations
 * @version v1.2.0
 *
 * @copyright Copyright (c) 2022, Nations Technologies Inc. All rights reserved.
 */
#include "n32l40x_it.h"
#include "n32l40x.h"
#include "main.h"


__WEAK void exti0_irqhandler_call(void){}
__WEAK void exti1_irqhandler_call(void){}
__WEAK void exti2_irqhandler_call(void){}
__WEAK void exti3_irqhandler_call(void){}
__WEAK void exti4_irqhandler_call(void){}
__WEAK void exti5_irqhandler_call(void){}
__WEAK void exti6_irqhandler_call(void){}
__WEAK void exti7_irqhandler_call(void){}
__WEAK void exti8_irqhandler_call(void){}
__WEAK void exti9_irqhandler_call(void){}
__WEAK void exti10_irqhandler_call(void){}
__WEAK void exti11_irqhandler_call(void){}
__WEAK void exti12_irqhandler_call(void){}
__WEAK void exti13_irqhandler_call(void){}
__WEAK void exti14_irqhandler_call(void){}
__WEAK void exti15_irqhandler_call(void){}


/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 */
void NMI_Handler(void)
{
}


/**
 * @brief  This function handles Memory Manage exception.
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Bus Fault exception.
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles Usage Fault exception.
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/**
 * @brief  This function handles SVCall exception.
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 */
void DebugMon_Handler(void)
{
}


/**
 * @brief  This function handles DMA interrupt request defined in main.h .
 */
void DMA_IRQ_HANDLER(void)
{
}

/******************************************************************************/
/*                 N32l40x Peripherals Interrupt Handlers                     */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_n32l40x.s).                                                 */
/******************************************************************************/



void EXTI0_IRQHandler(void)
{
	rt_interrupt_enter();
	exti0_irqhandler_call();
	rt_interrupt_leave();
}

void EXTI1_IRQHandler(void)
{
	rt_interrupt_enter();
	exti1_irqhandler_call();
	rt_interrupt_leave();
}

void EXTI2_IRQHandler(void)
{
	rt_interrupt_enter();
	exti2_irqhandler_call();
	rt_interrupt_leave();
}

void EXTI3_IRQHandler(void)
{
	rt_interrupt_enter();
	exti3_irqhandler_call();
	rt_interrupt_leave();
}

void EXTI4_IRQHandler(void)
{
	rt_interrupt_enter();
	exti4_irqhandler_call();
	rt_interrupt_leave();
}

void EXTI9_5_IRQHandler(void)
{
	rt_interrupt_enter();
	exti5_irqhandler_call();
	exti6_irqhandler_call();
	exti7_irqhandler_call();
	exti8_irqhandler_call();
	exti9_irqhandler_call();
	rt_interrupt_leave();
}

void EXTI15_10_IRQHandler(void)
{
	rt_interrupt_enter();
	exti10_irqhandler_call();
	exti11_irqhandler_call();
	exti12_irqhandler_call();
	exti13_irqhandler_call();
	exti14_irqhandler_call();
	exti15_irqhandler_call();
	rt_interrupt_leave();
}


