/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: hk32f0301mxxc_dbgmcu.h
* @brief  : This file contains all the functions prototypes for the DBGMCU firmware 
  *          library.
* @Author : AE Team  
********************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_DBGMCU_H
#define __HK32F0301MXXC_DBGMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"
 
/** @addtogroup DBGMCU 
  * @{
  */ 
/* Exported types ------------------------------------------------------------*/ 
/* Exported constants --------------------------------------------------------*/


/** @defgroup DBGMCU_Exported_Constants DBGMCU_Exported_Constants
  * @{ 
  */

#define DBGMCU_STOP                  DBGMCU_CR_DBG_STOP
#define DBGMCU_STANDBY               DBGMCU_CR_DBG_STANDBY
#define IS_DBGMCU_PERIPH(PERIPH) ((((PERIPH) & 0xFFFFFFF9) == 0x00) && ((PERIPH) != 0x00))

#define DBGMCU_TIM1_STOP             DBGMCU_APB1_FZ_DBG_TIM1_STOP  
#define DBGMCU_TIM2_STOP             DBGMCU_APB1_FZ_DBG_TIM2_STOP
#define DBGMCU_TIM6_STOP             DBGMCU_APB1_FZ_DBG_TIM6_STOP 
#define DBGMCU_WWDG_STOP             DBGMCU_APB1_FZ_DBG_WWDG_STOP
#define DBGMCU_IWDG_STOP             DBGMCU_APB1_FZ_DBG_IWDG_STOP
#define DBGMCU_I2C1_SMBUS_TIMEOUT    DBGMCU_APB1_FZ_DBG_I2C1_SMBUS_TIMEOUT
#define IS_DBGMCU_APB1PERIPH(PERIPH) ((((PERIPH) & 0xFFDFE7EC) == 0x00) && ((PERIPH) != 0x00))


/**
  * @}
  */ 
  
/**
  * @}
  */ 

 
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

/* Device and Revision ID management functions ********************************/ 
uint16_t DBGMCU_GetREVID(void);
uint16_t DBGMCU_GetDEVID(void);
uint16_t DBGMCU_GetENGID(void);
uint16_t DBGMCU_GetDEVID2(void);
/* Peripherals Configuration functions ****************************************/ 
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState);
void DBGMCU_APB1PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F0301MXXC_DBGMCU_H */
 
 
