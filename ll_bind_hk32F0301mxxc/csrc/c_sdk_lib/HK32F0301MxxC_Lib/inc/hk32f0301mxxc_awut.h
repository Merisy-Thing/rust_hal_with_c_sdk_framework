/********************************************************************
* @copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
* @filename: hk32f0301mxxc_awut.h
* @brief: //本文件功能简要描述
* @author: AE Team
********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  __HK32F0301MXXC_AWUT_H
#define  __HK32F0301MXXC_AWUT_H

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"


/** @addtogroup AWUT
  * @{
  */

/** @defgroup AWUT_Exported_Macros AWUT_Exported_Macros
  * @{
  */


/** @defgroup AWUT_Register_Reset_Val AWUT_Register_Reset_Val
  * @{
  */
#define AWUT_CR_RESET_VALUE 0x00000000U
#define AWUT_SR_RESET_VALUE 0x00000000U
/**
  * @}
  */


/**
  * @brief  AWUT_Clock_Type enumeration
  */
typedef enum
{
  AWUT_CLK_LSI,
  AWUT_CLK_HSE,
}AWUT_CLK_TYPE;

#define IS_AWUT_CLK(AWUT_CLK)   (((AWUT_CLK) == AWUT_CLK_LSI) || \
                                    ((AWUT_CLK) == AWUT_CLK_HSE))

/**
  * @}
  */
/**
  * @}
  */

/* Exported functions ------------------------------------------------------- */

void AWUT_DeInit(void);
void AWUT_CLKConfig(AWUT_CLK_TYPE eAWUT_CLK);
ErrorStatus AWUT_TimerCounterAndStart(uint32_t TimerCounter);
FlagStatus AWUT_GetFlagStatus(void);


#endif /* __HK32F0301MXXC_AWUT_H */

