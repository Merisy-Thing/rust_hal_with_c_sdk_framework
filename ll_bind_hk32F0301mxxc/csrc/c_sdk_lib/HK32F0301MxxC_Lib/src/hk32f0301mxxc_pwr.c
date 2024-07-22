/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: k32f0301mxxc_pwr.c
* @brief  : This file provides firmware functions to manage the following 
*          functionalities of the PWR peripheral:
*           + lower power mode management
*           + flags management
* @Author : AE Team  
* @Version: V1.0.0
*********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_pwr.h"
#include "hk32f0301mxxc.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup PWR  PWR
  * @brief PWR driver modules
  * @{
  */
  
/** @defgroup PWR_Private_Defines PWR_Private_Defines
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* ------------------ PWR registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_DS_MASK               ((uint32_t)0xFFFFFFFE)

/**
  * @}
  */
	
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PWR_Private_Functions PWR_Private_Functions
  * @{
  */

/** @defgroup PWR_Group1 PWR DeInit function 
 *  @brief   PWR DeInit function
 *
@verbatim
  ==============================================================================
                   ##### PWR DeInit function #####
  ==============================================================================

    [..] use PWR_DeInit to reset PWR peripheral.

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the PWR peripheral registers to their default reset values.
  * @retval None
  */
	
void PWR_DeInit(void)
{
  RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_PWR, ENABLE);
  RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_PWR, DISABLE);
}
 /**
  * @}
  */

/** @defgroup PWR_Group2 Low Power modes configuration functions
 *  @brief   Low Power modes configuration functions 
 *
@verbatim
  ==============================================================================
              ##### Low Power modes configuration functions #####
  ==============================================================================

    [..] The devices feature three low-power modes:
    (+) Sleep mode: Cortex-M0 core stopped, peripherals kept running.
    (+) Stop mode: all clocks are stopped, regulator running, regulator in low power mode
    (+) Standby mode: VCORE domain powered off

  *** Sleep mode *** 
  ==================
  [..] 
    (+) Entry:
        (++) The Sleep mode is entered by executing the WFE() or WFI() instructions.
    (+) Exit:
        (++) Any peripheral interrupt acknowledged by the nested vectored interrupt 
             controller (NVIC) can wake up the device from Sleep mode.

  *** Stop mode *** 
  =================
  [..] In Stop mode, all clocks in the VCORE domain are stopped, the HSI are disabled. 
	        Internal SRAM and register contents are preserved.
       The voltage regulator can be configured either in normal or low-power mode.

    (+) Entry:
        (++) The Stop mode is entered using the PWR_EnterSTOPMode(PWR_Regulator_LowPower,) 
             function with regulator in LowPower or with Regulator ON.
    (+) Exit:
        (++) Any EXTI Line (Internal or External) configured in Interrupt/Event mode
             or any internal IPs (I2C, UASRT or CEC) wakeup event.


  *** Auto-wakeup (AWUT) from low-power mode *** 
  =============================================
  [..] The MCU can be woken up from a comparator event, without depending on an 
       external interrupt (Auto-wakeup mode).

    (+) Comparator auto-wakeup (AWUT) from the Stop mode
        (++) To wake up from the Stop mode with an comparator wakeup
             event, it is necessary to:
             (+++) Configure the EXTI Line 11 for comparator to be sensitive to to the selected 
						 edges (falling, rising or falling and rising) (Interrupt or Event modes) 
						 using the EXTI_Init() function.
             (+++) Configure the comparator to generate the event.

@endverbatim
  * @{
  */

/**
  * @brief  Enters Sleep mode.
  * @note   In Sleep mode, all I/O pins keep the same state as in Run mode.
  * @param  PWR_Entry: specifies if SLEEP mode in entered with WFI or WFE instruction.
  *          This parameter can be one of the following values:
  *             @arg PWR_Entry_WFI: enter SLEEP mode with WFI instruction
  *             @arg PWR_Entry_WFE: enter SLEEP mode with WFE instruction
	*             @arg PWR_Entry_SLEEPONEXIT: enter SLEEP mode with SLEEPONEXIT instruction
  * @retval None
  */

void PWR_EnterSleepMode(uint8_t PWR_Entry)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_PWR_ENTRY(PWR_Entry));
  
  /* Select the regulator state in Sleep mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_MASK;
  /* Store the new value */
  PWR->CR = tmpreg;

  /* Select sleep mode entry --------------------------------------------------*/
  if(PWR_Entry == PWR_Entry_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
  }
  else if(PWR_Entry == PWR_Entry_WFE)
  {
    /* Request Wait For Event */
  	__SEV();
    __WFE();
	  __WFE();
  }  
	else
  {
    /* Set SLEEP on exit bit of Cortex-M0 System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
  }
}

/**
  * @brief  Enters STOP mode.
  * @note   In Stop mode, all I/O pins keep the same state as in Run mode.
  * @note   When exiting Stop mode by issuing an interrupt or a wakeup event, 
  *         the HSI RC oscillator is selected as system clock.
  * @note   When the voltage regulator operates in low power mode, an additional 
  *         startup delay is incurred when waking up from Stop mode. 
  *         By keeping the internal regulator ON during Stop mode, the consumption 
  *         is higher although the startup time is reduced.
  * @param  PWR_Regulator: specifies the regulator state in STOP mode.
  *         This parameter can be one of the following values:
  *             @arg PWR_Regulator_ON: STOP mode with regulator ON
  *             @arg PWR_Regulator_LowPower: STOP mode with regulator in low power mode
  * @param  PWR_Entry: specifies if STOP mode in entered with WFI or WFE instruction.
  *         This parameter can be one of the following values:
  *             @arg PWR_Entry_WFI: enter STOP mode with WFI instruction
  *             @arg PWR_Entry_WFE: enter STOP mode with WFE instruction
  *             @arg PWR_Entry_SLEEPONEXIT: enter STOP mode with SLEEPONEXIT instruction
  * @retval None
  */
void PWR_EnterStopMode(uint32_t PWR_Regulator, uint8_t PWR_Entry)
{
  uint32_t tmpreg = 0;
  /* Check the parameters */
  assert_param(IS_PWR_REGULATOR(PWR_Regulator));
  assert_param(IS_PWR_ENTRY(PWR_Entry));

  /* Select the regulator state in Stop mode ---------------------------------*/
  tmpreg = PWR->CR;
  /* Clear PDDS and LPDS bits */
  tmpreg &= CR_DS_MASK;
  /* Set LPDS bit according to PWR_Regulator value */
  tmpreg |= PWR_Regulator;
  /* Store the new value */
  PWR->CR = tmpreg;
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR |= SCB_SCR_SLEEPDEEP;
  
  /* Select Stop mode entry --------------------------------------------------*/
  if(PWR_Entry == PWR_Entry_WFI)
  {   
    /* Request Wait For Interrupt */
    __WFI();
		
		
  }
  else if (PWR_Entry == PWR_Entry_WFE)
  {
    /* Request Wait For Event */
  	__SEV();
    __WFE();
	  __WFE();
  }
	else
	{
	/* Set SLEEP on exit bit of Cortex-M0 System Control Register */
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;
	}
  
  /* Reset SLEEPDEEP bit of Cortex System Control Register */
  SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP);  
}
/**
  * @}
  */

/** @defgroup PWR_Group3 Flags management functions
 *  @brief   Flags management functions 
 *
@verbatim
  ==============================================================================
                       ##### Flags management functions #####
  ==============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Checks whether the specified PWR flag is set or not.
  * @param  PWR_FLAG: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg PWR_FLAG_LDORDY: Internal CPU LDO ready flag
  * @retval The new state of PWR_FLAG (SET or RESET).
  */

FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_PWR_GET_FLAG(PWR_FLAG));
  
  if ((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
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
  

  
