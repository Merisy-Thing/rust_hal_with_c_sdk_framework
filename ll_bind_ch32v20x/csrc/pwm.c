#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "pwm.h"
#include "wrapper.h"

static void pwm_enable(uint32_t ch, bool en)
{
	TIM_OCInitTypeDef TIM_OCInitStructure={0};

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = en ? TIM_OutputState_Enable : TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	if(ch == 0) {
		TIM_OC1Init( TIM1, &TIM_OCInitStructure );
	}
}

int pwm_deinit(uint32_t ch)
{
	if(ch == 0) {
		pwm_enable(ch, false);
	}
	TIM_Cmd( TIM1, DISABLE );
	return 0;
}

int pwm_init(uint32_t ch, uint32_t freq)
{
	(void)ch;
	if(freq == 0) {
		return -1;
	}

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};
	uint32_t prescaler = SystemCoreClock/freq - 1;

	TIM_TimeBaseInitStructure.TIM_Period = 1000;
	TIM_TimeBaseInitStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( TIM1, &TIM_TimeBaseInitStructure);

	pwm_enable(ch, false);

	TIM_CtrlPWMOutputs(TIM1, ENABLE );
	if(ch == 0) {
		TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
	}
	TIM_ARRPreloadConfig( TIM1, ENABLE );
	TIM_Cmd( TIM1, ENABLE );

	return 0;
}


int pwm_ctrl(uint32_t ch, uint32_t ctrl, uint32_t param)
{
	switch (ctrl)
	{
	case PWM_CTRL_ON:
		pwm_enable(ch, true);
	break;
	case PWM_CTRL_OFF:
		pwm_enable(ch, false);
	break;
	case PWM_CTRL_SET_DUTY:
	{
		if(ch == 0) {
			TIM_SetCompare1(TIM1, param);
		}
	}
	break;
	case PWM_CTRL_GET_DUTY:
	{
		if(ch == 0) {
			return TIM_GetCapture1(TIM1);
		}
	}
	break;
	case PWM_CTRL_SET_PERIOD://param: unit:tick
	{
		TIM1->ATRLR = param - 1;
	}
	break;
	case PWM_CTRL_GET_PERIOD:
	{
		uint32_t arr = TIM1->ATRLR + 1;

		return arr;
	}
	break;
	case PWM_CTRL_GET_MAXDUTY:
	{
		return TIM1->ATRLR + 1;
	}
	break;
	case PWM_CTRL_ACTIVE_HIGH:
		TIM_OC1PolarityConfig(TIM1, TIM_OCPolarity_High);
	break;
	case PWM_CTRL_ACTIVE_LOW:
		TIM_OC1PolarityConfig(TIM1, TIM_OCPolarity_Low);
	break;
	default:
	break;
	}

	return 0;
}