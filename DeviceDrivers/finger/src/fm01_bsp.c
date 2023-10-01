#include "fm01_bsp.h"




static USART_InitType usart_init;



/**
  * gpio dsp power enable config : output pull push mode, not pull up / not pull down
  * gpio touch power config : output pull push mode, not pull up / not pull down
  * gpio touch int config : input mode, pull up, exit mode, falling trigger
  **/
void fm01_gpio_init(void)
{
	GPIO_InitType gpio_init;
	EXTI_InitType exti_init;
	NVIC_InitType nvic_init;

	/* dsp power gpio init */
	FM01_DSP_VCC_GPIO_CLK_ENABLE();
	GPIO_InitStruct(&gpio_init);
    gpio_init.Pin			= FM01_DSP_VCC_GPIO_PIN;
    gpio_init.GPIO_Current	= GPIO_DC_2mA;
    gpio_init.GPIO_Pull		= GPIO_No_Pull;
    gpio_init.GPIO_Mode		= GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(FM01_DSP_VCC_GPIO_PORT, &gpio_init);
    
	/* touch power gpio init */
	FM01_TOUCH_VCC_GPIO_CLK_ENABLE();
	GPIO_InitStruct(&gpio_init);
    gpio_init.Pin			= FM01_TOUCH_VCC_GPIO_PIN;
    gpio_init.GPIO_Current	= GPIO_DC_2mA;
    gpio_init.GPIO_Pull		= GPIO_No_Pull;
    gpio_init.GPIO_Mode		= GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(FM01_TOUCH_VCC_GPIO_PORT, &gpio_init);

    /* touch int config */
    FM01_TOUCH_INT_GPIO_CLK_ENABLE();
    GPIO_InitStruct(&gpio_init);
	gpio_init.Pin			= FM01_TOUCH_INT_GPIO_PIN;
	gpio_init.GPIO_Pull		= GPIO_Pull_Up;
	gpio_init.GPIO_Mode		= GPIO_Mode_IT_Falling;
	GPIO_InitPeripheral(FM01_TOUCH_INT_GPIO_PORT, &gpio_init);
	GPIO_ConfigEXTILine(FM01_TOUCH_INT_EXIT_SOURCE_PORT, FM01_TOUCH_INT_EXIT_SOURCE_PIN);
    
    /*Configure touch int EXTI line*/
    EXTI_InitStruct(&exti_init);
    exti_init.EXTI_Line    = FM01_TOUCH_INT_EXIT_LINE;
    exti_init.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_InitPeripheral(&exti_init);

    /*Set touch int interrupt priority*/
    nvic_init.NVIC_IRQChannel                   = FM01_TOUCH_INT_NVIC_IRQ_CHANNEL;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0x05;
    nvic_init.NVIC_IRQChannelSubPriority        = 0x0F;
    nvic_init.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic_init);


    /* disable dsp vcc and touch vcc */
    fm01_dsp_vcc_control(DISABLE);
    fm01_touch_vcc_control(DISABLE);
    
}




void fm01_uart_init(void)
{
	NVIC_InitType nvic_init;
	GPIO_InitType gpio_init;
	
	/* step 1 enable gpio rx / tx clk */
	FM01_RX_GPIO_CLK_ENABLE();
	FM01_TX_GPIO_CLK_ENABLE();
	
	/* step 2 enable clk */
	FM01_UART_CLK_ENABLE();

    /* step 3 Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

    /* step 4 Enable the nvic config */
    nvic_init.NVIC_IRQChannel					= FM01_UART_NVIC_IRQ_CHANNEL;
    //nvic_init.NVIC_IRQChannelPreemptionPriority	= 0;
    nvic_init.NVIC_IRQChannelSubPriority		= 0;
    nvic_init.NVIC_IRQChannelCmd				= ENABLE;
    NVIC_Init(&nvic_init);	

    /* step 5 gpio config */
    /* Initialize gpio_init */
    GPIO_InitStruct(&gpio_init);

    /* Configure USARTy Tx as alternate function push-pull */
    gpio_init.Pin            = FM01_TX_GPIO_PIN;    
    gpio_init.GPIO_Mode      = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Alternate = FM01_TX_GPIO_AF;
    GPIO_InitPeripheral(FM01_TX_GPIO_PORT, &gpio_init);

    /* Configure USARTz Tx as alternate function push-pull */
    gpio_init.Pin            = FM01_RX_GPIO_PIN;
    gpio_init.GPIO_Alternate = FM01_RX_GPIO_AF;
    GPIO_InitPeripheral(FM01_RX_GPIO_PORT, &gpio_init);

    /* step 6 uart config */
    USART_StructInit(&usart_init);
    usart_init.BaudRate            = FM01_UART_BAUDRATE;
    usart_init.WordLength          = USART_WL_8B;
    usart_init.StopBits            = USART_STPB_1;
    usart_init.Parity              = USART_PE_NO;
    usart_init.HardwareFlowControl = USART_HFCTRL_NONE;
    usart_init.Mode                = USART_MODE_RX | USART_MODE_TX;

    USART_Init(FM01_UART, &usart_init);

    /* step 6.1 uart interrupt config */
    USART_ConfigInt(FM01_UART, USART_INT_RXDNE, ENABLE);

    /* step 6.2 uart enable */
    USART_Enable(FM01_UART, ENABLE);
}

void fm01_uart_deinit(void)
{
	GPIO_InitType gpio_init;
	NVIC_InitType nvic_init;
	
	/* step 1 uart disable */
	USART_Enable(FM01_UART, DISABLE);
	
	/* step 2 uart interrupt disable */
	USART_ConfigInt(FM01_UART, USART_INT_RXDNE, DISABLE);
	
	/* step 3 uart reset config */
	USART_DeInit(FM01_UART);
	
	/* step 4 gpio reset config */
	GPIO_InitStruct(&gpio_init);
    gpio_init.Pin			= FM01_RX_GPIO_PIN;
    gpio_init.GPIO_Current	= GPIO_DC_2mA;
    gpio_init.GPIO_Pull		= GPIO_No_Pull;
    gpio_init.GPIO_Mode		= GPIO_Mode_Out_PP;
    GPIO_InitPeripheral(FM01_RX_GPIO_PORT, &gpio_init);
    
	gpio_init.Pin			= FM01_TX_GPIO_PIN;
	GPIO_InitPeripheral(FM01_TX_GPIO_PORT, &gpio_init);

	GPIO_WriteBit(FM01_RX_GPIO_PORT, FM01_RX_GPIO_PIN, Bit_RESET);
	GPIO_WriteBit(FM01_TX_GPIO_PORT, FM01_TX_GPIO_PIN, Bit_RESET);
	
	/* step 5 uart nvic reset config */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic_init.NVIC_IRQChannel					= FM01_UART_NVIC_IRQ_CHANNEL;
    //nvic_init.NVIC_IRQChannelPreemptionPriority	= 0;
    nvic_init.NVIC_IRQChannelSubPriority		= 0;
    nvic_init.NVIC_IRQChannelCmd				= DISABLE;
    NVIC_Init(&nvic_init);	
    
	/* step 6 uart clk disable */
	FM01_UART_CLK_DISABLE();
}


void fm01_exit_config(FunctionalState cmd)
{
	GPIO_InitType gpio_init;
	EXTI_InitType exti_init;
	NVIC_InitType nvic_init;

    GPIO_InitStruct(&gpio_init);
	gpio_init.Pin			= FM01_TOUCH_INT_GPIO_PIN;
	gpio_init.GPIO_Pull		= GPIO_Pull_Up;
	if (cmd == ENABLE)
	{
		gpio_init.GPIO_Mode		= GPIO_Mode_IT_Falling;
	}
	else
	{
		gpio_init.GPIO_Mode		= GPIO_Mode_Input;
	}
	GPIO_InitPeripheral(FM01_TOUCH_INT_GPIO_PORT, &gpio_init);
	GPIO_ConfigEXTILine(FM01_TOUCH_INT_EXIT_SOURCE_PORT, FM01_TOUCH_INT_EXIT_SOURCE_PIN);
    
    /*Configure touch int EXTI line*/
    EXTI_InitStruct(&exti_init);
    exti_init.EXTI_Line    = FM01_TOUCH_INT_EXIT_LINE;
    exti_init.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    exti_init.EXTI_LineCmd = cmd;
    EXTI_InitPeripheral(&exti_init);

    /*Set touch int interrupt priority*/
    nvic_init.NVIC_IRQChannel                   = FM01_TOUCH_INT_NVIC_IRQ_CHANNEL;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0x05;
    nvic_init.NVIC_IRQChannelSubPriority        = 0x0F;
    nvic_init.NVIC_IRQChannelCmd                = cmd;
    NVIC_Init(&nvic_init);	

}



void fm01_exit_handler(void)
{
	
}



__WEAK void fm01_uart_int_getc(uint8_t data){}


void FM01_UART_IRQHANDLER(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

    if (USART_GetIntStatus(FM01_UART, USART_INT_RXDNE) != RESET && 
        USART_GetFlagStatus(FM01_UART, USART_FLAG_RXDNE) != RESET)
    {
        fm01_uart_int_getc(USART_ReceiveData(FM01_UART));
    }	

	/* leave interrupt */
	rt_interrupt_leave();
}





