#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include "ch32v20x.h"
#include "wrapper.h"
#include "adc.h"

//static int16_t Calibrattion_Val = 0;
extern void ADC_CH0_EOC_hook_rs(uint16_t val);

int adc_init(uint32_t adc_ch, uint32_t flags)
{
	(void)adc_ch;
	(void)flags;
    ADC_InitTypeDef  ADC_InitStructure = {0};

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_Cmd(ADC1, ENABLE);

	ADC_BufferCmd(ADC1, DISABLE); //disable buffer
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	//Calibrattion_Val = Get_CalibrationValue(ADC1);

	ADC_BufferCmd(ADC1, ENABLE); //enable buffer

	ADC_TempSensorVrefintCmd(ENABLE);

	return 0;
}

// static uint16_t Cali_ConversionVal(int16_t val)
// {
// 	val = val + Calibrattion_Val;

//     if(val < 0)
//         return 0;
//     if(val > 4095)
//         return 4095;
//     return val;
// }

uint16_t Get_ConversionVal(uint8_t ch)
{
    uint16_t val;

    ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    val = ADC_GetConversionValue(ADC1);

    return val;
}

int adc_buffered_deinit(uint32_t adc_ch)
{
	(void)adc_ch;
	ADC_Cmd(ADC1, DISABLE);

    NVIC_InitTypeDef NVIC_InitStructure = {0};
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

	return 0;
}

void adc_buffered_init(uint32_t adc_ch)
{
	(void)adc_ch;
    ADC_InitTypeDef  ADC_InitStructure = {0};

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_RegularChannelConfig(ADC1, adc_ch, 1, ADC_SampleTime_13Cycles5);

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	ADC_BufferCmd(ADC1, DISABLE); //disable buffer
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	//Calibrattion_Val = Get_CalibrationValue(ADC1);

	ADC_BufferCmd(ADC1, ENABLE); //enable buffer

	ADC_TempSensorVrefintCmd(ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure = {0};
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void ADC1_2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void ADC1_2_IRQHandler()
{
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC))
    {
		uint16_t val = ADC_GetConversionValue(ADC1);
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
		ADC_CH0_EOC_hook_rs(val);
	}
}