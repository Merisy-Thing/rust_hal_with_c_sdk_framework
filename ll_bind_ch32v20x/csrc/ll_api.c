#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "debug.h"
#include "wrapper.h"
#include "gpio.h"
#include "spi_bus.h"
#include "i2c.h"
#include "pwm.h"
#include "usart.h"
#include "adc.h"

extern void sys_tick_inc(void);

void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick_Handler( void )
{
    SysTick->SR=0;
    sys_tick_inc();
}

void SysTick_Config(void)
{
    SysTick->CTLR= 0x00000000;
    SysTick->SR  = 0x00000000;
    SysTick->CNT = 0x00000000;
    SysTick->CMP = SystemCoreClock/1000 - 1;;
    
    NVIC_SetPriority(SysTicK_IRQn, 15);
    SysTick->CTLR |= 0x0F;
    NVIC_EnableIRQ(SysTicK_IRQn);
}

static void hw_config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE );
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	SysTick_Config();
	
	//启用的事件和所有中断（包括未开启中断）都能唤醒系统；
	//将 WFI 指令当成是 WFE 执行。
	NVIC->SCTLR |= (1<<3) | (1<<4); 
}


void hal_hw_init(void)
{
	hw_config();
	USART_Printf_Init(115200);
}

void delay_ns(uint32_t ns)
{
    uint32_t start = SysTick->CNT;
    uint32_t target = (start + ns * SystemCoreClock / 1000000) / 1000;
    uint32_t curr;

    if(target > start) {
        while(1) {
            curr = SysTick->CNT;
            if((curr >= target) || (curr < start)) {
                break;
            }
        }
    } else {
        while(1) {
            curr = SysTick->CNT;
            if((curr >= target) && (curr < start)) {
                break;
            }
        }
    }
}


int ll_invoke(enum INVOKE invoke_id, ...)
{
	int result = 0;
    va_list args;
    va_start(args, invoke_id);

	switch (invoke_id)
	{
	case ID_SYSTEM_INIT:
		//system inited in start asm.
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
		USART_TypeDef *USARTx;
#if(DEBUG == DEBUG_UART1)
        USARTx = USART1;
#elif(DEBUG == DEBUG_UART2)
        USARTx = USART2;
#elif(DEBUG == DEBUG_UART3)
        USARTx = USART3;
#endif
		while (len--) {
			USART_SendData(USARTx, *p_str++);
        	while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == 0);
		}
	}
	break;
	case ID_GPIO_INIT:
	{
		uint32_t port = va_arg(args, uint32_t);
		uint32_t pin = va_arg(args, uint32_t);
		uint32_t flags = va_arg(args, uint32_t);

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
	case ID_GPIO_GET_PORT_REG:
	{
		uint32_t port = va_arg(args, uint32_t);
		struct PortReg* p_port_reg = va_arg(args, struct PortReg*);

		result = gpio_get_port_reg(port, p_port_reg);
	}
	break;
	case ID_SPI_BUS_INIT:
	{
		uint32_t bus = va_arg(args, uint32_t);
		uint32_t mode = va_arg(args, uint32_t);
		uint32_t flags = va_arg(args, uint32_t);
		uint32_t baud_rate = va_arg(args, uint32_t);

		result = spi_bus_init(bus, mode, flags, baud_rate);
	}
	break;
	case ID_SPI_BUS_DEINIT:
	{
		//TODO
	}
	break;
	case ID_SPI_BLOCKING_RW:
	{
		uint32_t bus = va_arg(args, uint32_t);
		uint8_t* p_wr = va_arg(args, uint8_t*);
		uint8_t* p_rd = va_arg(args, uint8_t*);
		uint32_t size = va_arg(args, uint32_t);

		result = spi_bus_blocking_transfer(bus, p_wr, p_rd, size);
	}
	break;
	case ID_USART_INIT:
	{
		uint32_t usart_id = va_arg(args, uint32_t);
		uint32_t flags = va_arg(args, uint32_t);
		uint32_t baud_rate = va_arg(args, uint32_t);

		result = usart_init(usart_id, flags, baud_rate);
	}
	break;
	case ID_USART_DEINIT:
	{
		//TODO
	}
	break;
	case ID_USART_WRITE:
	{
		uint32_t usart_id = va_arg(args, uint32_t);
		uint8_t* p_buff = va_arg(args, uint8_t*);
		uint32_t size = va_arg(args, uint32_t);
		result = usart_blocking_write(usart_id, p_buff, size);
	}
	break;
	case ID_ADC_INIT:
	{
		uint32_t adc_ch = va_arg(args, uint32_t);
		uint32_t flags = va_arg(args, uint32_t);

		result = adc_init(adc_ch, flags);
	}
	break;
	case ID_ADC_DEINIT:
	{
		//todo
	}
	break;
	case ID_ADC_CTRL:
	{
		uint32_t adc_ch = va_arg(args, uint32_t);
		uint32_t ctrl = va_arg(args, uint32_t);
		if(adc_ch == 0) {//ch0 map to temp sensor
			adc_ch = ADC_Channel_TempSensor;
		} else if(adc_ch == 1) {//ch1 map to vref
			adc_ch = ADC_Channel_Vrefint;
		} else {
			result = -1;
			break;
		}

		if(ctrl == ADC_CTRL_CONVERT) {
			uint16_t* p_buf = va_arg(args, uint16_t*);
			uint32_t  size  = va_arg(args, uint32_t);
			for(uint32_t idx=0; idx<size; idx++) {
				p_buf[idx] = Get_ConversionVal(adc_ch);
			}
		} else if(ctrl == ADC_CTRL_START) {
			adc_buffered_init(adc_ch);
		} else if(ctrl == ADC_CTRL_STOP) {
			adc_buffered_deinit(adc_ch);
		}
	}
	break;
	case ID_PWM_INIT:
	{
		uint32_t pwm_ch = va_arg(args, uint32_t);
		uint32_t freq = va_arg(args, uint32_t);

		result = pwm_init(pwm_ch, freq);
	}
	break;
	case ID_PWM_DEINIT:
	{
		uint32_t pwm_ch = va_arg(args, uint32_t);
		result = pwm_deinit(pwm_ch);
	}
	break;
	case ID_PWM_CTRL:
	{
		uint32_t pwm_ch = va_arg(args, uint32_t);
		uint32_t ctrl = va_arg(args, uint32_t);
		uint32_t param = va_arg(args, uint32_t);

		result = pwm_ctrl(pwm_ch, ctrl, param);
	}
	break;
	default:
		result = -1000;
	break;
	}
	va_end(args);

	return result;
}

