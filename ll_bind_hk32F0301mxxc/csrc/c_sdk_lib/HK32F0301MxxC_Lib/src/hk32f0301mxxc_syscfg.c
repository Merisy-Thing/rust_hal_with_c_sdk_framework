/********************************************************************************
  * @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
  * @Filename: hk32f0301mxxc_syscfg.c
  * @Brief: SYSCFG Initialization and Configuration functions
  * @Author: AE Team 
  * @Version: V1.0.0/2022-09-09
  * 1.初始版本
*********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_syscfg.h"
#include "hk32f0301mxxc_rcc.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup SYSCFG  SYSCFG
  * @brief SYSCFG driver modules
  * @{
  */ 

/** @defgroup SYSCFG_Private_Functions SYSCFG_Private_Functions
  * @{ 
  */ 

/** @defgroup SYSCFG_Group1 SYSCFG Initialization and Configuration functions
 *  @brief   SYSCFG Initialization and Configuration functions 
 *
@verbatim
 ===============================================================================
        ##### SYSCFG Initialization and Configuration functions #####
 ===============================================================================

@endverbatim
  * @{
  */
  
/**
  * @brief  Deinitializes the Alternate Functions (remap and EXTI configuration)
  *   registers to their default reset values.
  * @retval None
  */
void SYSCFG_DeInit(void)
{
    RCC_APBPeriph2ResetCmd(RCC_APBPeriph2_SYSCFG, ENABLE);
    RCC_APBPeriph2ResetCmd(RCC_APBPeriph2_SYSCFG, DISABLE);
}

/**
  * @brief  Cortex-m0 lockup output to tim1 break input connected or disconnected.
  * @param  Lockup_lockOnOff: To TIM1 break input onoff.
  *         This parameter can be one of the following values:
  *            @arg SYSCFG_Lockup_TIM1Break_ON: Cortex-m0 lockup output to tim1 break input connected  
  *            @arg SYSCFG_Lockup_TIM1Break_OFF:Cortex-m0 lockup output to tim1 break input disconnected
  * @retval None
  */
void SYSCFG_Lockup_Tim1BreakConfig(uint8_t Lockup_lockOnOff)
{
   uint32_t temp = 0;
  /* Check the parameters */
  assert_param(IS_SYSCFG_LOCKUP_TIM1BREAK_ONOFF(Lockup_lockOnOff));
  /*select SYSYCFG CFGR1 register*/
  temp = SYSCFG->CFGR1;
  /* clear mem_mode */
  temp &= MEM_LOCKUP_OUT_MASK;
  /* set memoryRemap value*/
  temp |= Lockup_lockOnOff;

  SYSCFG->CFGR1 |= temp;
}

/**
  * @brief  Changes the mapping of the specified pin.
  * @param  SYSCFG_MemoryRemap: selects the memory remapping.
  *         This parameter can be one of the following values:
  *            @arg SYSCFG_MemoryRemap_Flash:       Main Flash memory mapped at 0x00000000   
  *            @arg SYSCFG_MemoryRemap_SRAM:        Embedded SRAM (2kB) mapped at 0x00000000
  * @retval None
  */
void SYSCFG_MemoryRemapConfig(uint8_t SYSCFG_MemoryRemap)
{
  uint32_t temp = 0;
  /* Check the parameters */
  assert_param(IS_SYSCFG_MEMORY_REMAP_CONFING(SYSCFG_MemoryRemap));
  /*select SYSYCFG CFGR1 register*/
  temp = SYSCFG->CFGR1;
  /* clear mem_mode */
  temp &= MEM_REMAP_MASK;
  /* set memoryRemap value*/
  temp |= SYSCFG_MemoryRemap;

  SYSCFG->CFGR1 |= temp;
}
/**
  * @brief  Selects the GPIO pin used as EXTI Line.
  * @param  EXTI_PortSourceGPIOx : selects the GPIO port to be used as source for
  *          EXTI lines where x can be (A..D) 
  *            
  * @param  EXTI_PinSourcex: specifies the EXTI line to be configured.
  *           This parameter can be EXTI_PinSourcex where x can be (0..15) 
  *             
  * @retval None
  */
void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
{
      uint32_t tmp = 0x00;

  /* Check the parameters */
  assert_param(IS_EXTI_PORT_SOURCE(EXTI_PortSourceGPIOx));
  assert_param(IS_EXTI_PIN_SOURCE(EXTI_PinSourcex));

      tmp = ((uint32_t)0x0F) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03));
      switch(EXTI_PinSourcex >> 0x02)
	{
        case (uint8_t)0x00:
            SYSCFG->EXTICR1 &= ~tmp;
            SYSCFG->EXTICR1 |= (((uint32_t)EXTI_PortSourceGPIOx) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03)));
            break;
        case (uint8_t)0x01:
            SYSCFG->EXTICR2 &= ~tmp;
            SYSCFG->EXTICR2 |= (((uint32_t)EXTI_PortSourceGPIOx) << (0x04 * (EXTI_PinSourcex & (uint8_t)0x03)));
            break;
        default:
            break;
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
