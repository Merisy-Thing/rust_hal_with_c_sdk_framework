/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: hk32f0301mxxc_iwdg.h
* @brief  : This file contains all the functions prototypes for the IWDG 
  *          firmware library.
* @Author : AE Team  
********************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_IWDG_H
#define __HK32F0301MXXC_IWDG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"
 

/** @addtogroup IWDG
  * @{
  */

/** @defgroup IWDG_Exported_Constants IWDG_Exported_Constants
  * @{
  */
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup IWDG_WriteAccess IWDG_WriteAccess
  * @{
  */
#define IWDG_WriteAccess_Enable     ((uint16_t)0x5555)
#define IWDG_WriteAccess_Disable    ((uint16_t)0x0000)
#define IS_IWDG_WRITE_ACCESS(ACCESS) (((ACCESS) == IWDG_WriteAccess_Enable) || \
                                      ((ACCESS) == IWDG_WriteAccess_Disable))
/**
  * @}
  */

/** @defgroup IWDG_prescaler  IWDG_prescaler
  * @{
  */
#define IWDG_Prescaler_4            ((uint8_t)0x00)
#define IWDG_Prescaler_8            ((uint8_t)0x01)
#define IWDG_Prescaler_16           ((uint8_t)0x02)
#define IWDG_Prescaler_32           ((uint8_t)0x03)
#define IWDG_Prescaler_64           ((uint8_t)0x04)
#define IWDG_Prescaler_128          ((uint8_t)0x05)
#define IWDG_Prescaler_256          ((uint8_t)0x06)
#define IS_IWDG_PRESCALER(PRESCALER) (((PRESCALER) == IWDG_Prescaler_4)  || \
                                      ((PRESCALER) == IWDG_Prescaler_8)  || \
                                      ((PRESCALER) == IWDG_Prescaler_16) || \
                                      ((PRESCALER) == IWDG_Prescaler_32) || \
                                      ((PRESCALER) == IWDG_Prescaler_64) || \
                                      ((PRESCALER) == IWDG_Prescaler_128)|| \
                                      ((PRESCALER) == IWDG_Prescaler_256))
/**
  * @}
  */

/** @defgroup IWDG_Flag  IWDG_Flag
  * @{
  */
#define IWDG_FLAG_PVU               IWDG_SR_PVU
#define IWDG_FLAG_RVU               IWDG_SR_RVU
#define IWDG_FLAG_WVU               IWDG_SR_WVU
#define IS_IWDG_FLAG(FLAG) (((FLAG) == IWDG_FLAG_PVU) || ((FLAG) == IWDG_FLAG_RVU)  || \
                            ((FLAG) == IWDG_FLAG_WVU))
							
#define IS_IWDG_RELOAD(RELOAD) ((RELOAD) <= 0xFFF)
#define IS_IWDG_WINDOW_VALUE(VALUE) ((VALUE) <= 0xFFF)
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

/* Prescaler and Counter configuration functions ******************************/
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess);
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler);
void IWDG_SetReload(uint16_t Reload);
void IWDG_ReloadCounter(void);
void IWDG_SetWindowValue(uint16_t WindowValue);

/* IWDG activation function ***************************************************/
void IWDG_Enable(void);

/* Flag management function ***************************************************/
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F0301MXXC_IWDG_H */




