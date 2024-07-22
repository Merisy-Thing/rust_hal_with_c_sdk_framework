/**
  ******************************************************************************
  * @copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
  * @file: hk32f0301mxxc_adc.h 
  * @brief: ADC Initialization and Configuration functions
  * @author: AE Team 
  * @version: V1.0.0/2022-09-09 
  ****************************************************************************** 
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_ADC_H
#define __HK32F0301MXXC_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"
 

/** @addtogroup ADC  
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup ADC_Exported_Types ADC_Exported_Types
  * @{
  */
/** 
  * @brief  ADC Init structure definition
  */
  
typedef struct
{
  FunctionalState ADC_ContinuousConvMode;   /*!< Specifies whether the conversion is performed in
                                                 Continuous or Single mode.
                                                 This parameter can be set to ENABLE or DISABLE. */

  uint32_t ADC_ExternalTrigConvEdge;        /*!< Selects the external trigger Edge and enables the
                                                 trigger of a regular group. This parameter can be a value
                                                 of @ref ADC_external_trigger_edge_conversion */

  uint32_t ADC_ExternalTrigConv;            /*!< Defines the external trigger used to start the analog
                                                 to digital conversion of regular channels. This parameter
                                                 can be a value of @ref ADC_external_trigger_sources_for_channels_conversion */

  uint32_t ADC_DataAlign;                   /*!< Specifies whether the ADC data alignment is left or right.
                                                 This parameter can be a value of @ref ADC_data_align */

  uint32_t  ADC_ScanDirection;              /*!< Specifies in which direction the channels will be scanned
                                                 in the sequence. 
                                                 This parameter can be a value of @ref ADC_Scan_Direction */
}ADC_InitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup ADC_Exported_Constants ADC_Exported_Constants
  * @{
  */ 
#define IS_ADC_ALL_PERIPH(PERIPH)                  ((PERIPH) == ADC)

/** @defgroup ADC_JitterOff ADC_JitterOff
  * @{
  */ 
/* These defines are obsolete and maintained for legacy purpose only. They are replaced  by the ADC_ClockMode */  
#define ADC_JitterOff_PCLKDiv2                    ADC_CFGR2_JITOFFDIV2
#define ADC_JitterOff_PCLKDiv4                    ADC_CFGR2_JITOFFDIV4

#define IS_ADC_JITTEROFF(JITTEROFF) (((JITTEROFF) & 0x3FFFFFFF) == (uint32_t)RESET)

/**
  * @}
  */
  
/** @defgroup ADC_ClockMode ADC_ClockMode
  * @{
  */ 
#define ADC_ClockMode_AsynClk                  ((uint32_t)0x00000000)   /*!< ADC Asynchronous clock mode */
#define ADC_ClockMode_SynClkDiv2               ADC_CFGR2_CKMODE_0   /*!<  Synchronous clock mode divided by 2 */
#define ADC_ClockMode_SynClkDiv4               ADC_CFGR2_CKMODE_1   /*!<  Synchronous clock mode divided by 4 */
#define IS_ADC_CLOCKMODE(CLOCK) (((CLOCK) == ADC_ClockMode_AsynClk) ||\
				                        ((CLOCK) == ADC_ClockMode_SynClkDiv2) ||\
				                        ((CLOCK) == ADC_ClockMode_SynClkDiv4))

/**
  * @}
  */ 

/** @defgroup ADC_external_trigger_edge_conversion  ADC_external_trigger_edge_conversion
  * @{
  */ 
#define ADC_ExternalTrigConvEdge_None              ((uint32_t)0x00000000)
#define ADC_ExternalTrigConvEdge_Rising            ADC_CFGR1_EXTEN_0
#define ADC_ExternalTrigConvEdge_Falling           ADC_CFGR1_EXTEN_1
#define ADC_ExternalTrigConvEdge_RisingFalling     ADC_CFGR1_EXTEN

#define IS_ADC_EXT_TRIG_EDGE(EDGE) (((EDGE) == ADC_ExternalTrigConvEdge_None) || \
                                    ((EDGE) == ADC_ExternalTrigConvEdge_Rising) || \
                                    ((EDGE) == ADC_ExternalTrigConvEdge_Falling) || \
                                    ((EDGE) == ADC_ExternalTrigConvEdge_RisingFalling))
/**
  * @}
  */ 

/** @defgroup ADC_external_trigger_sources_for_channels_conversion ADC_external_trigger_sources_for_channels_conversion
  * @{
  */ 

/* TIM1 */
#define ADC_ExternalTrigConv_T1_TRGO               ((uint32_t)0x00000000)	//0
#define ADC_ExternalTrigConv_T1_CC4                ADC_CFGR1_EXTSEL_0		//1
#define ADC_ExternalTrigConv_T1_CC1                ADC_CFGR1_EXTSEL_2		//4
#define ADC_ExternalTrigConv_T1_CC2                ((uint32_t)ADC_CFGR1_EXTSEL_2 | ADC_CFGR1_EXTSEL_0)		//5
#define ADC_ExternalTrigConv_T1_CC3                ((uint32_t)ADC_CFGR1_EXTSEL_2 | ADC_CFGR1_EXTSEL_1)		//6

/* TIM2 */
#define ADC_ExternalTrigConv_T2_TRGO               ADC_CFGR1_EXTSEL_1		//2

/* TIM6 */
#define ADC_ExternalTrigConv_T6_TRGO               ((uint32_t)(ADC_CFGR1_EXTSEL_0 | ADC_CFGR1_EXTSEL_1))	//3

/* IO Trig */
#define ADC_ExternalTrigConv_IO_TRGO               ((uint32_t)(ADC_CFGR1_EXTSEL_0 | ADC_CFGR1_EXTSEL_1)| ADC_CFGR1_EXTSEL_2)		//7

#define IS_ADC_EXTERNAL_TRIG_CONV(CONV) (((CONV) == ADC_ExternalTrigConv_T1_TRGO) || \
                                         ((CONV) == ADC_ExternalTrigConv_T1_CC4)   || \
                                         ((CONV) == ADC_ExternalTrigConv_T1_CC1)   || \
                                         ((CONV) == ADC_ExternalTrigConv_T1_CC2)   || \
                                         ((CONV) == ADC_ExternalTrigConv_T1_CC3)   || \
                                         ((CONV) == ADC_ExternalTrigConv_T2_TRGO)  || \
                                         ((CONV) == ADC_ExternalTrigConv_T6_TRGO)  || \
                                         ((CONV) == ADC_ExternalTrigConv_IO_TRGO)) 
/**
  * @}
  */ 

/** @defgroup ADC_data_align  ADC_data_align
  * @{
  */ 
  
#define ADC_DataAlign_Right                        ((uint32_t)0x00000000)
#define ADC_DataAlign_Left                         ADC_CFGR1_ALIGN

#define IS_ADC_DATA_ALIGN(ALIGN) (((ALIGN) == ADC_DataAlign_Right) || \
                                  ((ALIGN) == ADC_DataAlign_Left))
/**
  * @}
  */

/** @defgroup ADC_Scan_Direction ADC_Scan_Direction
  * @{
  */ 
  
#define ADC_ScanDirection_Upward                   ((uint32_t)0x00000000)
#define ADC_ScanDirection_Backward                 ADC_CFGR1_SCANDIR

#define IS_ADC_SCAN_DIRECTION(DIRECTION) (((DIRECTION) == ADC_ScanDirection_Upward) || \
                                          ((DIRECTION) == ADC_ScanDirection_Backward))
/**
  * @}
  */ 
    
/** @defgroup ADC_analog_watchdog_selection ADC_analog_watchdog_selection
  * @{
  */ 
  
#define ADC_AnalogWatchdog_Channel_0                 ((uint32_t)0x00000000)
#define ADC_AnalogWatchdog_Channel_1                 ((uint32_t)0x04000000)
#define ADC_AnalogWatchdog_Channel_2                 ((uint32_t)0x08000000)
#define ADC_AnalogWatchdog_Channel_3                 ((uint32_t)0x0C000000)
#define ADC_AnalogWatchdog_Channel_4                 ((uint32_t)0x10000000)
#define ADC_AnalogWatchdog_Channel_5                 ((uint32_t)0x14000000)
#define ADC_AnalogWatchdog_Channel_6                 ((uint32_t)0x18000000)
#define ADC_AnalogWatchdog_Channel_7                 ((uint32_t)0x1C000000)
#define ADC_AnalogWatchdog_Channel_8                 ((uint32_t)0x20000000)


#define IS_ADC_ANALOG_WATCHDOG_CHANNEL(CHANNEL) (((CHANNEL) == ADC_AnalogWatchdog_Channel_0)  || \
                                                 ((CHANNEL) == ADC_AnalogWatchdog_Channel_1)  || \
                                                 ((CHANNEL) == ADC_AnalogWatchdog_Channel_2)  || \
                                                 ((CHANNEL) == ADC_AnalogWatchdog_Channel_3)  || \
                                                 ((CHANNEL) == ADC_AnalogWatchdog_Channel_4)  || \
												 ((CHANNEL) == ADC_AnalogWatchdog_Channel_5)  || \
												 ((CHANNEL) == ADC_AnalogWatchdog_Channel_6)  || \
												 ((CHANNEL) == ADC_AnalogWatchdog_Channel_7)  || \
                                                 ((CHANNEL) == ADC_AnalogWatchdog_Channel_8))
/**
  * @}
  */ 
  
/** @defgroup ADC_sampling_times ADC_sampling_times
  * @{
  */ 

#define ADC_SampleTime_1_5Cycles                     ((uint32_t)0x00000000)
#define ADC_SampleTime_7_5Cycles                     ((uint32_t)0x00000001)
#define ADC_SampleTime_13_5Cycles                    ((uint32_t)0x00000002)
#define ADC_SampleTime_28_5Cycles                    ((uint32_t)0x00000003)
#define ADC_SampleTime_41_5Cycles                    ((uint32_t)0x00000004)
#define ADC_SampleTime_55_5Cycles                    ((uint32_t)0x00000005)
#define ADC_SampleTime_71_5Cycles                    ((uint32_t)0x00000006)
#define ADC_SampleTime_239_5Cycles                   ((uint32_t)0x00000007)

#define IS_ADC_SAMPLE_TIME(TIME) (((TIME) == ADC_SampleTime_1_5Cycles)   || \
                                  ((TIME) == ADC_SampleTime_7_5Cycles)   || \
                                  ((TIME) == ADC_SampleTime_13_5Cycles)  || \
                                  ((TIME) == ADC_SampleTime_28_5Cycles)  || \
                                  ((TIME) == ADC_SampleTime_41_5Cycles)  || \
                                  ((TIME) == ADC_SampleTime_55_5Cycles)  || \
                                  ((TIME) == ADC_SampleTime_71_5Cycles)  || \
                                  ((TIME) == ADC_SampleTime_239_5Cycles))
/**
  * @}
  */ 

/** @defgroup ADC_thresholds ADC_thresholds
  * @{
  */ 
  
#define IS_ADC_THRESHOLD(THRESHOLD) ((THRESHOLD) <= 0xFFF)

/**
  * @}
  */ 

/** @defgroup ADC_Channel ADC_Channel
  * @{
  */ 
  
#define ADC_Channel_0                              ADC_CHSELR_CHSEL0
#define ADC_Channel_1                              ADC_CHSELR_CHSEL1
#define ADC_Channel_2                              ADC_CHSELR_CHSEL2
#define ADC_Channel_3                              ADC_CHSELR_CHSEL3
#define ADC_Channel_4                              ADC_CHSELR_CHSEL4
#define ADC_Channel_5                              ADC_CHSELR_CHSEL5
#define ADC_Channel_6                              ADC_CHSELR_CHSEL6
#define ADC_Channel_7                              ADC_CHSELR_CHSEL7
#define ADC_Channel_8                              ADC_CHSELR_CHSEL8

#define ADC_Channel_Vrefint                        ((uint32_t)ADC_Channel_8)
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF124))

#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) != (uint32_t)RESET) && (((CHANNEL) & 0xFFFFFE00) == (uint32_t)RESET))

/**
  * @}
  */ 
  
/** @defgroup ADC_interrupts_definition ADC_interrupts_definition
  * @{
  */ 
  
#define ADC_IT_ADRDY                               ADC_IER_ADRDYIE
#define ADC_IT_EOSMP                               ADC_IER_EOSMPIE
#define ADC_IT_EOC                                 ADC_IER_EOCIE
#define ADC_IT_EOSEQ                               ADC_IER_EOSIE
#define ADC_IT_OVR                                 ADC_IER_OVRIE
#define ADC_IT_AWD                                 ADC_IER_AWDIE
 
#define IS_ADC_CONFIG_IT(IT) (((IT) != (uint32_t)RESET) && (((IT) & 0xFFFFFF60) == (uint32_t)RESET))

#define IS_ADC_GET_IT(IT) (((IT) == ADC_IT_ADRDY) || ((IT) == ADC_IT_EOSMP) || \
                           ((IT) == ADC_IT_EOC)   || ((IT) == ADC_IT_EOSEQ) || \
                           ((IT) == ADC_IT_OVR)   || ((IT) == ADC_IT_AWD))

#define IS_ADC_CLEAR_IT(IT) (((IT) != (uint32_t)RESET) && (((IT) & 0xFFFFFF60) == (uint32_t)RESET))

/**
  * @}
  */ 

/** @defgroup ADC_flags_definition ADC_flags_definition
  * @{
  */ 
  
#define ADC_FLAG_ADRDY                             ADC_ISR_ADRDY
#define ADC_FLAG_EOSMP                             ADC_ISR_EOSMP
#define ADC_FLAG_EOC                               ADC_ISR_EOC
#define ADC_FLAG_EOSEQ                             ADC_ISR_EOS
#define ADC_FLAG_OVR                               ADC_ISR_OVR
#define ADC_FLAG_AWD                               ADC_ISR_AWD

#define ADC_FLAG_ADEN                              ((uint32_t)0x01000001)
#define ADC_FLAG_ADDIS                             ((uint32_t)0x01000002)
#define ADC_FLAG_ADSTART                           ((uint32_t)0x01000004)
#define ADC_FLAG_ADSTP                             ((uint32_t)0x01000010)
#define ADC_FLAG_ADCAL                             ((uint32_t)0x81000000) 

#define IS_ADC_CLEAR_FLAG(FLAG) (((FLAG) != (uint32_t)RESET) && (((FLAG) & 0xFFFFFF60) == (uint32_t)RESET))

#define IS_ADC_GET_FLAG(FLAG) (((FLAG) == ADC_FLAG_ADRDY)   || ((FLAG) == ADC_FLAG_EOSMP) || \
                               ((FLAG) == ADC_FLAG_EOC)     || ((FLAG) == ADC_FLAG_EOSEQ) || \
                               ((FLAG) == ADC_FLAG_AWD)     || ((FLAG) == ADC_FLAG_OVR)   || \
                               ((FLAG) == ADC_FLAG_ADEN)    || ((FLAG) == ADC_FLAG_ADDIS) || \
                               ((FLAG) == ADC_FLAG_ADSTART) || ((FLAG) == ADC_FLAG_ADSTP) || \
                               ((FLAG) == ADC_FLAG_ADCAL))
 

/**
  * @}
  */
  
/** @defgroup ADC_ADCGAIN_definition ADC_ADCGAIN_definition
* @{
*/
#define ADC_ADCGAIN_SEL1_0                             ((uint32_t)0x00000000U)
#define ADC_ADCGAIN_SEL1_1                             ADC_GAIN_GAIN_SEL1_0
#define ADC_ADCGAIN_SEL1_2                             ADC_GAIN_GAIN_SEL1_1
#define ADC_ADCGAIN_SEL1_3                             (ADC_GAIN_GAIN_SEL1_0 | ADC_GAIN_GAIN_SEL1_1)
#define ADC_ADCGAIN_SEL1_4                             ADC_GAIN_GAIN_SEL1_2
#define ADC_ADCGAIN_SEL1_5                             (ADC_GAIN_GAIN_SEL1_2 | ADC_GAIN_GAIN_SEL1_0)

#define IS_ADC_ADCGAIN_SEL1(GAIN_SEL)  (((GAIN_SEL) == ADC_ADCGAIN_SEL1_0)   || \
                                        ((GAIN_SEL) == ADC_ADCGAIN_SEL1_1 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL1_2 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL1_3 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL1_4 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL1_5 )) 
																	
#define ADC_ADCGAIN_SEL2_0                             ((uint32_t)0x00000000U)
#define ADC_ADCGAIN_SEL2_1                             ADC_GAIN_GAIN_SEL2_0
#define ADC_ADCGAIN_SEL2_2                             ADC_GAIN_GAIN_SEL2_1
#define ADC_ADCGAIN_SEL2_3                             (ADC_GAIN_GAIN_SEL2_0 | ADC_GAIN_GAIN_SEL2_1)
#define ADC_ADCGAIN_SEL2_4                             ADC_GAIN_GAIN_SEL2_2
#define ADC_ADCGAIN_SEL2_5                             (ADC_GAIN_GAIN_SEL1_2 | ADC_GAIN_GAIN_SEL1_0)

#define IS_ADC_ADCGAIN_SEL2(GAIN_SEL)  (((GAIN_SEL) == ADC_ADCGAIN_SEL2_0)   || \
                                        ((GAIN_SEL) == ADC_ADCGAIN_SEL2_1 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL2_2 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL2_3 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL2_4 )  || \
																				((GAIN_SEL) == ADC_ADCGAIN_SEL2_5 ))
/**
  * @}
  */ 
/**
  * @}
  */ 

/**
  * @}
  */ 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

/*  Function used to set the ADC configuration to the default reset state *****/
void ADC_DeInit(ADC_TypeDef* ADCx);

/* Initialization and Configuration functions *********************************/ 
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct);
void ADC_ClockModeConfig(ADC_TypeDef* ADCx, uint32_t ADC_ClockMode);
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
/* This Function is obsolete and maintained for legacy purpose only. 
   ADC_ClockModeConfig() function should be used instead */
void ADC_JitterCmd(ADC_TypeDef* ADCx, uint32_t ADC_JitterOff, FunctionalState NewState);

/* Power saving functions *****************************************************/
void ADC_AutoPowerOffCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_WaitModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/* Analog Watchdog configuration functions ************************************/
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold,uint16_t LowThreshold);
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog_Channel);
void ADC_AnalogWatchdogSingleChannelCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/* Temperature Sensor , Vrefint and Vbat management function ******************/
void ADC_VrefintCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

/* Channels Configuration functions *******************************************/
void ADC_ChannelConfig(ADC_TypeDef* ADCx, uint32_t ADC_Channel, uint32_t ADC_SampleTime);
void ADC_ContinuousModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_DiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);
void ADC_OverrunModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState);

void ADC_StopOfConversion(ADC_TypeDef* ADCx);
void ADC_StartOfConversion(ADC_TypeDef* ADCx);
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);
uint16_t ADC_GetChannelConvertValue(ADC_TypeDef* ADCx, uint32_t ADC_Channel);

/* Interrupts and flags management functions **********************************/
void ADC_ITConfig(ADC_TypeDef* ADCx, uint32_t ADC_IT, FunctionalState NewState);
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint32_t ADC_FLAG);
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint32_t ADC_FLAG);
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint32_t ADC_IT);
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint32_t ADC_IT);

void ADC_Diff_Func(ADC_TypeDef* ADCx, FunctionalState NewState);

/*set the ADC GAIN_SEL configuration***************************************************/
void ADC_ConfigADCGAIN_SEL(ADC_TypeDef *ADCx, uint32_t GAIN_SEL1, uint32_t GAIN_SEL2);

/*ADC Calibration Offset configuration***************************************************/
void ADC_OFFSET_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
uint8_t ADC_Get_CAL_OFFSETValue(ADC_TypeDef* ADCx);
void ADC_Set_CAL_OFFSETValue(ADC_TypeDef* ADCx, uint8_t CAL_Value);
/*Get real VDDA when the power supply voltage changes*****************************/
uint32_t ADC_GETVDDA(uint32_t VrefADbuff);

#ifdef __cplusplus
}
#endif

#endif /*__HK32F0301MXXC_ADC_H */


 
/***************************** (C) COPYRIGHT HKMicroChip  *******************************/
