/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: hk32f0301mxxc_dbgmcu.c
* @brief  : This file provides firmware functions to manage the following 
  *          functionalities of the Debug MCU (DBGMCU) peripheral:
  *           + Device and Revision ID management
  *           + Peripherals Configuration 
* @Author : AE Team  
* @Version: V1.0.0/2022-09-14
* 1.³õÊ¼°æ±¾
 ********************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_dbgmcu.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup DBGMCU DBGMCU
  * @brief DBGMCU driver modules
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup DBGMCU_IDCODE_DEVID_Exported_Constants DBGMCU_IDCODE_DEVID_Exported_Constants  
  * @{
  */
#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)
/**
  * @}
  */ 
  
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DBGMCU_Private_Functions  DBGMCU_Private_Functions
  * @{
  */
  

/** @defgroup DBGMCU_Group1 DBGMCU_Group1
 *  @brief   Device and Revision ID management functions
 *
@verbatim
  ==============================================================================
            ##### Device and Revision ID management functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Returns the device revision identifier.
  * @retval Device revision identifier
  */
uint16_t DBGMCU_GetREVID(void)
{
   return(DBGMCU->IDCODE >> 16);
}

/**
  * @brief  Returns the device identifier.
  * @retval Device identifier
  */
uint16_t DBGMCU_GetDEVID(void)
{
   return(DBGMCU->IDCODE & IDCODE_DEVID_MASK);
}

/**
  * @brief  Returns the device identifier 2.
  * @retval Device identifier
  */
uint16_t DBGMCU_GetDEVID2(void)
{
   return(DBGMCU->IDCODE >>16);
}

/**
  * @brief  Returns the Chip identifier.
  * @retval Device identifier
  */
uint16_t DBGMCU_GetENGID(void)
{
   return(DBGMCU->IDCODE & 0x0FFF);
}

/**
  * @}
  */
  
/** @defgroup DBGMCU_Group2 DBGMCU_Group2
 *  @brief   Peripherals Configuration
 *
@verbatim
  ==============================================================================
               ##### Peripherals Configuration functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Configures low power mode behavior when the MCU is in Debug mode.
  * @param  DBGMCU_Periph: specifies the low power mode.
  *          This parameter can be any combination of the following values:
  *             @arg DBGMCU_STOP: Keep debugger connection during STOP mode
  *             @arg DBGMCU_STANDBY: Keep debugger connection during STANDBY mode
  * @param  NewState: new state of the specified low power mode in Debug mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DBGMCU_PERIPH(DBGMCU_Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    DBGMCU->CR |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->CR &= ~DBGMCU_Periph;
  }
}


/**
  * @brief  Configures APB1 peripheral behavior when the MCU is in Debug mode.
  * @param  DBGMCU_Periph: specifies the APB1 peripheral.
  *          This parameter can be any combination of the following values:
  *             @arg DBGMCU_TIM1_STOP: TIM1 counter stopped when Core is halted 
  *             @arg DBGMCU_TIM2_STOP: TIM2 counter stopped when Core is halted
  *             @arg DBGMCU_TIM6_STOP: TIM6 counter stopped when Core is halted 
  *             @arg DBGMCU_WWDG_STOP: Debug WWDG stopped when Core is halted
  *             @arg DBGMCU_IWDG_STOP: Debug IWDG stopped when Core is halted
  *             @arg DBGMCU_I2C1_SMBUS_TIMEOUT: I2C1 SMBUS timeout mode stopped 
  *                                             when Core is halted            
  * @param  NewState: new state of the specified APB1 peripheral in Debug mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void DBGMCU_APB1PeriphConfig(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DBGMCU_APB1PERIPH(DBGMCU_Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    DBGMCU->APB_FZ |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->APB_FZ &= ~DBGMCU_Periph;
  }
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

