#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "gpio.h"
#include "wrapper.h"

const GPIO_TypeDef* GPIO_LIST[] = { GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG };

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
		GPIO_InitStruct.GPIO_Pin   = 1<<pin;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		
		uint32_t af_flag = flags & GPIO_FLAG_AF_MASK;
		if(af_flag) {
			if(af_flag == GPIO_FLAG_AF_OD) {
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
			} else if(af_flag == GPIO_FLAG_AF_PP){
				GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
			}
		} else {
			if(flags & GPIO_FLAG_OUT_MASK) {
				if((flags & GPIO_FLAG_OUT_MASK) == GPIO_FLAG_OUT_OD) {
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
				} else {
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
				}
			} else {
				uint32_t in_flag = flags & GPIO_FLAG_IN_MASK;
				if(in_flag == GPIO_FLAG_AIN) {
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
				} else if(in_flag == GPIO_FLAG_IN_FLOATING) {
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
				} else if(in_flag == GPIO_FLAG_IN_PU) {
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
				} else if(in_flag == GPIO_FLAG_IN_PD) {
					GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
				}
			}
		}
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

int gpio_get_port_reg(uint32_t port, struct PortReg* p_port_reg)
{
	GPIO_TypeDef* GPIOx = get_GPIOx(port);
	if(GPIOx) {
		p_port_reg->idr = (uint16_t*)&GPIOx->INDR;
		p_port_reg->odr = (uint16_t*)&GPIOx->OUTDR;
		p_port_reg->bsr = (uint16_t*)&GPIOx->BSHR;
		p_port_reg->bcr = (uint16_t*)&GPIOx->BCR;

		return 0;
	}

	return -1;
}