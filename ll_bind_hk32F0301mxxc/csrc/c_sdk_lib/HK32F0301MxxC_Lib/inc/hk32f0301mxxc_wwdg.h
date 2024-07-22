/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: hk32f0301mxxc_wwdg.h
* @brief  :  This file contains all the functions prototypes for the WWDG 
  *          firmware library.
* @Author : AE Team  
********************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_WWDG_H
#define __HK32F0301MXXC_WWDG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"
 

/** @addtogroup WWDG
  * @{
  */ 

/** @defgroup WWDG_Exported_Constants WWDG_Exported_Constants
  * @{
  */ 
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
  
/** @defgroup WWDG_Prescaler WWDG_Prescaler
  * @{
  */ 
  
#define WWDG_Prescaler_1    ((uint32_t)0x00000000)
#define WWDG_Prescaler_2    ((uint32_t)0x00000080)
#define WWDG_Prescaler_4    ((uint32_t)0x00000100)
#define WWDG_Prescaler_8    ((uint32_t)0x00000180)
#define IS_WWDG_PRESCALER(PRESCALER) (((PRESCALER) == WWDG_Prescaler_1) || \
                                      ((PRESCALER) == WWDG_Prescaler_2) || \
                                      ((PRESCALER) == WWDG_Prescaler_4) || \
                                      ((PRESCALER) == WWDG_Prescaler_8))
#define IS_WWDG_WINDOW_VALUE(VALUE) ((VALUE) <= 0x7F)
#define IS_WWDG_COUNTER(COUNTER) (((COUNTER) >= 0x40) && ((COUNTER) <= 0x7F))

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
/*  Function used to set the WWDG configuration to the default reset state ****/  
void WWDG_DeInit(void);

/* Prescaler, Refresh window and Counter configuration functions **************/
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler);
void WWDG_SetWindowValue(uint8_t WindowValue);
void WWDG_EnableIT(void);
void WWDG_SetCounter(uint8_t Counter);

/* WWDG activation functions **************************************************/
void WWDG_Enable(uint8_t Counter);

/* Interrupts and flags management functions **********************************/
FlagStatus WWDG_GetFlagStatus(void);
void WWDG_ClearFlag(void);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F0301MXXC_WWDG_H */



