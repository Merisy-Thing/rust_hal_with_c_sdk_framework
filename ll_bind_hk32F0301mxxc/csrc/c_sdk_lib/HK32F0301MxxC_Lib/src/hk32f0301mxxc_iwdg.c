/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: hk32f0301mxxc_iwdg.c
* @brief  : This file provides firmware functions to manage the following 
  *         functionalities of the Independent watchdog (IWDG) peripheral:           
  *           + Prescaler and Counter configuration
  *           + IWDG activation
  *           + Flag management 
* @Author : AE Team  
* @Version:V1.0.0
* 1.³õÊ¼°æ±¾
 ********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_iwdg.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup IWDG IWDG
  * @brief IWDG driver modules
  * @{
    @verbatim
================================================================================
                      ##### How to use this driver #####
================================================================================
	[..]
	(#) Start the IWDG using IWDG_Enable() function, when the IWDG is used
		in software mode (no need to enable the LSI, it will be enabled
		by hardware).    
	(#) Enable write access to IWDG_PR and IWDG_RLR registers using
		IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable) function.
	(#) Configure the IWDG prescaler using IWDG_SetPrescaler() function.
	(#) Configure the IWDG counter value using IWDG_SetReload() function.
		This value will be loaded in the IWDG counter each time the counter
		is reloaded, then the IWDG will start counting down from this value. 
	(#) Wait for the IWDG registers to be updated using IWDG_GetFlagStatus() function.
	(#) Configure the IWDG refresh window using IWDG_SetWindowValue() function.
	@endverbatim       
*/

/** @defgroup IWDG_PERIPH_Constants IWDG_PERIPH_Constants
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* IWDG 0perate Keyword */
#define KR_KEY_RELOAD    ((uint16_t)0xAAAA)
#define KR_KEY_ENABLE    ((uint16_t)0xCCCC)

/**
  * @}
  */

/** @defgroup IWDG_Private_Functions IWDG_Private_Functions
  * @{
  */
/* Private functions ---------------------------------------------------------*/  

/** @defgroup IWDG_Group1 Prescaler and Counter configuration functions
 *  @brief   Prescaler and Counter configuration functions
 *
@verbatim   
  ==============================================================================
            ##### Prescaler and Counter configuration functions #####
  ==============================================================================  
	 [..] This section provides functions allowing to:
        (+) Enables or disables write access to IWDG_PR and IWDG_RLR registers
		(+) Sets IWDG Prescaler value
		(+) Sets IWDG Reload value
		(+) Reloads IWDG counter with value defined in the reload register
		(+) Sets the IWDG window value
@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables write access to IWDG_PR and IWDG_RLR registers.
  * @param  IWDG_WriteAccess: new state of write access to IWDG_PR and IWDG_RLR registers.
  *          This parameter can be one of the following values:
  *            @arg IWDG_WriteAccess_Enable: Enable write access to IWDG_PR and IWDG_RLR registers
  *            @arg IWDG_WriteAccess_Disable: Disable write access to IWDG_PR and IWDG_RLR registers
  * @retval None
  */
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
  /* Check the parameters */
  assert_param(IS_IWDG_WRITE_ACCESS(IWDG_WriteAccess));
  IWDG->KR = IWDG_WriteAccess;
}

/**
  * @brief  Sets IWDG Prescaler value.
  * @param  IWDG_Prescaler: specifies the IWDG Prescaler value.
  *          This parameter can be one of the following values:
  *            @arg IWDG_Prescaler_4: IWDG prescaler set to 4
  *            @arg IWDG_Prescaler_8: IWDG prescaler set to 8
  *            @arg IWDG_Prescaler_16: IWDG prescaler set to 16
  *            @arg IWDG_Prescaler_32: IWDG prescaler set to 32
  *            @arg IWDG_Prescaler_64: IWDG prescaler set to 64
  *            @arg IWDG_Prescaler_128: IWDG prescaler set to 128
  *            @arg IWDG_Prescaler_256: IWDG prescaler set to 256
  * @retval None
  */
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
  /* Check the parameters */
  assert_param(IS_IWDG_PRESCALER(IWDG_Prescaler));
  IWDG->PR = IWDG_Prescaler;
}

/**
  * @brief  Sets IWDG Reload value.
  * @param  Reload: specifies the IWDG Reload value.
  *          This parameter must be a number between 0 and 0x0FFF.
  * @retval None
  */
void IWDG_SetReload(uint16_t Reload)
{
  /* Check the parameters */
  assert_param(IS_IWDG_RELOAD(Reload));
  IWDG->RLR = Reload;
}

/**
  * @brief  Reloads IWDG counter with value defined in the reload register
  *   (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @retval None
  */
void IWDG_ReloadCounter(void)
{
  IWDG->KR = KR_KEY_RELOAD;
}

/**
  * @brief  Sets the IWDG window value.
  * @param  WindowValue: specifies the window value to be compared to the downcounter.
  * @retval None
  */
void IWDG_SetWindowValue(uint16_t WindowValue)
{
  /* Check the parameters */
  assert_param(IS_IWDG_WINDOW_VALUE(WindowValue));
  IWDG->WINR = WindowValue;
}

/**
  * @}
  */

/** @defgroup IWDG_Group2 IWDG activation function
 *  @brief   IWDG activation function 
 *
@verbatim   
 ==============================================================================
                          ##### IWDG activation function #####
 ==============================================================================  
	[..] This section provides functions allowing to:
			(+) Enables IWDG
@endverbatim
  * @{
  */

/**
  * @brief  Enables IWDG (write access to IWDG_PR and IWDG_RLR registers disabled).
  * @retval None
  */
void IWDG_Enable(void)
{
  IWDG->KR = KR_KEY_ENABLE;
}

/**
  * @}
  */

/** @defgroup IWDG_Group3 Flag management function 
 *  @brief  Flag management function  
 *
@verbatim   
 ===============================================================================
                      ##### Flag management function ##### 
 ===============================================================================  
	[..] This section provides functions allowing to:
			(+) Checks whether the specified IWDG flag is set or not
@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the specified IWDG flag is set or not.
  * @param  IWDG_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg IWDG_FLAG_PVU: Prescaler Value Update on going
  *            @arg IWDG_FLAG_RVU: Reload Value Update on going
  *            @arg IWDG_FLAG_WVU: Counter Window Value Update on going
  * @retval The new state of IWDG_FLAG (SET or RESET).
  */
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_IWDG_FLAG(IWDG_FLAG));
  if ((IWDG->SR & IWDG_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the flag status */
  return bitstatus;
}

/**
  * @}
  */

/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

 
