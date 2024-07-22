/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: hk32f0301mxxc_wwdg.c
* @brief  : This file provides firmware functions to manage the following 
  *          functionalities of the Window watchdog (WWDG) peripheral:
  *           + Prescaler, Refresh window and Counter configuration
  *           + WWDG activation
  *           + Interrupts and flags management
* @Author : AE Team  
* @Version:V1.0.0
* 1.³õÊ¼°æ±¾
 ********************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_wwdg.h"
#include "hk32f0301mxxc_rcc.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup WWDG  WWDG
  * @brief WWDG driver modules
  * @{
    @verbatim
================================================================================
                      ##### How to use this driver #####
================================================================================
    [..]
	(#) Enable WWDG clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE) 
		function.
		  
	(#) Configure the WWDG prescaler using WWDG_SetPrescaler() function.
						 
	(#) Configure the WWDG refresh window using WWDG_SetWindowValue() function.
		  
	(#) Set the WWDG counter value and start it using WWDG_Enable() function.
		When the WWDG is enabled the counter value should be configured to 
		a value greater than 0x40 to prevent generating an immediate reset.
		  
	(#) Optionally you can enable the Early wakeup interrupt which is 
		generated when the counter reach 0x40.
		Once enabled this interrupt cannot be disabled except by a system reset.
			   
	(#) Then the application program must refresh the WWDG counter at regular
		intervals during normal operation to prevent an MCU reset, using
		WWDG_SetCounter() function. This operation must occur only when
		the counter value is lower than the refresh window value, 
		programmed using WWDG_SetWindowValue().
    @endverbatim
  */

/** @defgroup WWDG_PERIPH_Constants WWDG_PERIPH_Constants
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* CFR register bit mask */
#define CFR_WDGTB_MASK    ((uint32_t)0xFFFFFE7F)
#define CFR_W_MASK        ((uint32_t)0xFFFFFF80)
#define BIT_MASK          ((uint8_t) 0x7F)

/**
  * @}
  */

/** @defgroup WWDG_Private_Functions WWDG_Private_Functions
  * @{
  */
/* Private functions ---------------------------------------------------------*/  

/** @defgroup WWDG_Group1 Prescaler, Refresh window and Counter configuration functions
 *  @brief   Prescaler, Refresh window and Counter configuration functions 
 *
@verbatim   
  ==============================================================================
    ##### Prescaler, Refresh window and Counter configuration functions #####
  ==============================================================================  
	[..] This section provides functions allowing to:
        (+) Deinitializes the WWDG peripheral registers to their default reset values
		(+)	Sets the WWDG Prescaler
		(+)	Sets the WWDG window value
		(+)	Enables the WWDG Early Wakeup interrupt(EWI)
		(+)	Sets the WWDG counter value
@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the WWDG peripheral registers to their default reset values.
  * @note  None
  * @retval None
  */
void WWDG_DeInit(void)
{
  RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_WWDG, ENABLE);
  RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_WWDG, DISABLE);
}

/**
  * @brief  Sets the WWDG Prescaler.
  * @param  WWDG_Prescaler: specifies the WWDG Prescaler.
  *          This parameter can be one of the following values:
  *            @arg WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
  *            @arg WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
  *            @arg WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
  *            @arg WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
  * @retval None
  */
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_WWDG_PRESCALER(WWDG_Prescaler));
  /* Clear WDGTB[1:0] bits */
  tmpreg = WWDG->CFR & CFR_WDGTB_MASK;
  /* Set WDGTB[1:0] bits according to WWDG_Prescaler value */
  tmpreg |= WWDG_Prescaler;
  /* Store the new value */
  WWDG->CFR = tmpreg;
}

/**
  * @brief  Sets the WWDG window value.
  * @param  WindowValue: specifies the window value to be compared to the downcounter.
  *          This parameter value must be lower than 0x80.
  * @retval None
  */
void WWDG_SetWindowValue(uint8_t WindowValue)
{
  __IO uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_WWDG_WINDOW_VALUE(WindowValue));
  /* Clear W[6:0] bits */

  tmpreg = WWDG->CFR & CFR_W_MASK;

  /* Set W[6:0] bits according to WindowValue value */
  tmpreg |= WindowValue & (uint32_t) BIT_MASK;

  /* Store the new value */
  WWDG->CFR = tmpreg;
}

/**
  * @brief  Enables the WWDG Early Wakeup interrupt(EWI).
  * @note   Once enabled this interrupt cannot be disabled except by a system reset. 
  * @note  None
  * @retval None
  */
void WWDG_EnableIT(void)
{
  WWDG->CFR |= WWDG_CFR_EWI;
}

/**
  * @brief  Sets the WWDG counter value.
  * @param  Counter: specifies the watchdog counter value.
  *          This parameter must be a number between 0x40 and 0x7F (to prevent 
  *          generating an immediate reset).
  * @retval None
  */
void WWDG_SetCounter(uint8_t Counter)
{
  /* Check the parameters */
  assert_param(IS_WWDG_COUNTER(Counter));
  /* Write to T[6:0] bits to configure the counter value, no need to do
     a read-modify-write; writing a 0 to WDGA bit does nothing */
  WWDG->CR = Counter & BIT_MASK;
}

/**
  * @}
  */

/** @defgroup WWDG_Group2 WWDG activation functions
 *  @brief   WWDG activation functions 
 *
@verbatim   
  ==============================================================================
                     ##### WWDG activation function #####
  ==============================================================================  
    [..] This section provides functions allowing to:
        (+) Enables WWDG and load the counter value
@endverbatim
  * @{
  */

/**
  * @brief  Enables WWDG and load the counter value.                  
  * @param  Counter: specifies the watchdog counter value.
  *          This parameter must be a number between 0x40 and 0x7F (to prevent 
  *          generating an immediate reset).
  * @retval None
  */
void WWDG_Enable(uint8_t Counter)
{
  /* Check the parameters */
  assert_param(IS_WWDG_COUNTER(Counter));
  WWDG->CR = WWDG_CR_WDGA | Counter;
}

/**
  * @}
  */

/** @defgroup WWDG_Group3 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
  ==============================================================================
                ##### Interrupts and flags management functions #####
  ==============================================================================  
    [..] This section provides functions allowing to:
        (+) Checks whether the Early Wakeup interrupt flag is set or not
		(+)	Clears Early Wakeup interrupt flag
@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the Early Wakeup interrupt flag is set or not.
  * @note  None
  * @retval The new state of the Early Wakeup interrupt flag (SET or RESET).
  */
FlagStatus WWDG_GetFlagStatus(void)
{
  FlagStatus bitstatus = RESET;
    
  if ((WWDG->SR) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  return bitstatus;
}

/**
  * @brief  Clears Early Wakeup interrupt flag.
  * @note  None
  * @retval None
  */
void WWDG_ClearFlag(void)
{
  WWDG->SR = (uint32_t)RESET;
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


 
