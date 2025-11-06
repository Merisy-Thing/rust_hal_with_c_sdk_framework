#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "hk32f0301mxxc.h"
#include "wrapper.h"
#include "print.h"

static void uart_config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	UART_InitTypeDef UART_InitStructure;

	UART_InitStructure.UART_BaudRate = 115200;
	UART_InitStructure.UART_WordLength = UART_WordLength_8b;
	UART_InitStructure.UART_StopBits = UART_StopBits_1;
	UART_InitStructure.UART_Parity = UART_Parity_No;
	UART_InitStructure.UART_Mode = UART_Mode_Tx;

	/* UART configuration */
	UART_Init(UART1, &UART_InitStructure);

	/* Enable UART */
	UART_Cmd(UART1, ENABLE);

	GPIO_StructInit(&GPIO_InitStructure);
	//PD2 TX
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_1);
}

static void hw_config(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA ,  ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB ,  ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC ,  ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD ,  ENABLE);
	RCC_APBPeriph2ClockCmd(RCC_APBPeriph2_UART1, ENABLE);
    RCC_APBPeriph2ClockCmd(RCC_APBPeriph2_SYSCFG, ENABLE);
	RCC_APBPeriph1ClockCmd(RCC_APBPeriph1_PWR, ENABLE);

	RCC_APBPeriph1ClockCmd(RCC_APBPeriph1_IOMUX,ENABLE);
	//SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource4);

	SysTick_Config(SystemCoreClock/1000);
}

void hal_hw_init(void)
{
	hw_config();
	uart_config();
}

void delay_ns(uint32_t ns)
{
	uint32_t delay = (ns * (SystemCoreClock / 1000000)) / 1000;
	uint32_t start = SysTick->VAL;
	
	if(start > delay){
		while((start - SysTick->VAL) < delay);
	} else {
		uint32_t end_val = SysTick->LOAD - (delay - start);
		volatile uint32_t curr_val;
		while(1) {
			curr_val = SysTick->VAL;
			if((curr_val > start) && (curr_val < end_val)){
				break;
			}
		}
	}
}

const GPIO_TypeDef* GPIO_LIST[] = { GPIOA, GPIOB, GPIOC, GPIOD };

static GPIO_TypeDef* get_GPIOx(uint32_t port)
{
	if(port < sizeof(GPIO_LIST)/sizeof(GPIO_LIST[0])) {
		return (GPIO_TypeDef*)GPIO_LIST[port];
	}
	return NULL;
}

//flags: bit0: input/out, bit1: output type, bit2: pullup/nopull
int gpio_init(uint32_t port, uint32_t pin, uint32_t flags)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_TypeDef* GPIOx = get_GPIOx(port);

	if(GPIOx) {
		if(flags & GPIO_FLAG_AF_MASK) {
			GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
			
			GPIO_PinAFConfig(GPIOx, pin, flags & 0xFF);
		} else {
			if(flags == GPIO_FLAG_OUT_PP || flags == GPIO_FLAG_OUT_OD) {
				if(flags == GPIO_FLAG_OUT_OD) {
					GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
				} else {
					GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
				}
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
				GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
			} else if(flags == GPIO_FLAG_AIN) {
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
				GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
			} else if(flags == GPIO_FLAG_IN_FLOATING) {
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
				GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
			} else if(flags == GPIO_FLAG_IN_PU) {
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
				GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
			} else if(flags == GPIO_FLAG_IN_PD) {
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
				GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
			}
		}
		GPIO_InitStruct.GPIO_Pin   = 1<<pin;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;

		GPIO_Init(GPIOx, &GPIO_InitStruct);

		return 0;
	}

	return -1;
}

void gpio_set(uint32_t port, uint32_t pin, bool level)
{
	GPIO_TypeDef* GPIOx = get_GPIOx(port);
	if(GPIOx) {
		if(level) {
			GPIO_SetBits(GPIOx, 1 << pin);
		} else {
			GPIO_ResetBits(GPIOx, 1 << pin);
		}
	}
}

bool gpio_get_input(uint32_t port, uint32_t pin)
{
	GPIO_TypeDef* GPIOx = get_GPIOx(port);
	if(GPIOx) {
		if(GPIO_ReadInputDataBit(GPIOx, 1 << pin)) {
			return true;
		}
	}
	return false;
}

bool gpio_get_output(uint32_t port, uint32_t pin)
{
	GPIO_TypeDef* GPIOx = get_GPIOx(port);
	if(GPIOx) {
		if(GPIO_ReadOutputDataBit(GPIOx, 1 << pin)) {
			return true;
		}
	}
	return false;
}

const uint8_t EXTI_IRQ_LIST[] = { 
	EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, 
	EXTI4_IRQn, EXTI5_IRQn, EXTI6_IRQn, EXTI7_IRQn, 
 };
static int get_ExtiIRQ(uint32_t pin)
{
	if(pin < sizeof(EXTI_IRQ_LIST)/sizeof(EXTI_IRQ_LIST[0])) {
		return EXTI_IRQ_LIST[pin];
	}
	return -1;
}
int gpio_exti(uint32_t port, uint32_t pin, uint32_t flags)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	int irq = get_ExtiIRQ(pin);
	if(irq < 0) {
		return -1;
	}

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Line = 1 << pin;
	if(flags == GPIO_FLAG_EXTI_RISING) {
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	} else if(flags == GPIO_FLAG_EXTI_FALLING) {
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	} else {
    	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	}

	FunctionalState new_state = (flags != GPIO_FLAG_EXTI_DISABLE) ? ENABLE : DISABLE;
	
	EXTI_InitStructure.EXTI_LineCmd = new_state;
    EXTI_Init(&EXTI_InitStructure);

    SYSCFG_EXTILineConfig(port & 0x03, pin);
    EXTI_ClearITPendingBit(1 << pin);

    NVIC_InitStructure.NVIC_IRQChannel = irq;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = new_state;
    NVIC_Init(&NVIC_InitStructure);

	return 0;
}
void ll_putc(char c)
{
	UART_SendData(UART1, (uint8_t)c);
	while(UART_GetFlagStatus(UART1, UART_FLAG_TXE) == 0);
}
int ll_invoke(enum INVOKE invoke_id, ...)
{
	int result = 0;
    va_list args;
    va_start(args, invoke_id);

	switch (invoke_id)
	{
	case ID_SYSTEM_INIT:
		SystemInit();
	break;
	case ID_LL_DRV_INIT:
		hal_hw_init();
	break;
	case ID_DELAY_NANO:
	{
		uint32_t ns = va_arg(args, uint32_t);
		while(ns > 500000) { //max 500us
			delay_ns(500000);
			ns-=500000;
		}
		delay_ns(ns);
	}
	break;
	case ID_LOG_PUTS:
	{
		uint8_t* p_str = va_arg(args, uint8_t*);
		uint32_t len = va_arg(args, uint32_t);
		while (len--) {
			ll_putc(*p_str++);
		}
	}
	break;
	case ID_LOG_PRINT:
	{
		const char* fmt = va_arg(args, const char*);
		ll_vprintf(fmt, args);
	}
	break;
	case ID_GPIO_INIT:
	{
		uint32_t port = va_arg(args, uint32_t);
		uint32_t pin = va_arg(args, uint32_t);
		uint32_t flags = va_arg(args, uint32_t);
		println("port = %d", port);

		result = gpio_init(port, pin, flags);
	}
	break;
	case ID_GPIO_SET:
	{
		uint32_t port = va_arg(args, uint32_t);
		uint32_t pin = va_arg(args, uint32_t);
		uint32_t level = va_arg(args, uint32_t);

		gpio_set(port, pin, level);
	}
	break;
	case ID_GPIO_GET_INPUT:
	{
		uint32_t port = va_arg(args, uint32_t);
		uint32_t pin = va_arg(args, uint32_t);

		result = gpio_get_input(port, pin) ? 1 : 0;
	}
	break;
	case ID_GPIO_GET_OUTPUT:
	{
		uint32_t port = va_arg(args, uint32_t);
		uint32_t pin = va_arg(args, uint32_t);

		result = gpio_get_output(port, pin) ? 1 : 0;
	}
	break;
	case ID_GPIO_EXTI:
	{
		uint32_t port = va_arg(args, uint32_t);
		uint32_t pin = va_arg(args, uint32_t);
		uint32_t flags = va_arg(args, uint32_t);

		gpio_exti(port, pin, flags);

	}
	break;
	default:
		result = -1000;
	break;
	}
	va_end(args);

	return result;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char* file, uint32_t line)
{
	(void)file;
	(void)line;

	/* Infinite loop */
	while (1)
	{
	}
}
#endif
