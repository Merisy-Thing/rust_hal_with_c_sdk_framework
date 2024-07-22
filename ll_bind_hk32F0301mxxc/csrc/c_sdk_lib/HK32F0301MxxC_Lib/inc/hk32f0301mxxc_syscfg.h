/********************************************************************************
  * @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
  * @Filename: hk32f0301mxxc_syscfg.h 
  * @Brief: SYSCFG Initialization and Configuration functions
  * @Author: AE Team 
*********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_SYSCFG_H
#define __HK32F0301MXXC_SYSCFG_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"
 
/** @addtogroup SYSCFG
  * @{
  */ 
  
  /** @defgroup SYSCFG_Exported_Constants SYSCFG_Exported_Constants
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup SYSCFG_EXTI_Port_Sources SYSCFG_EXTI_Port_Sources
  * @{
  */ 
#define EXTI_PortSourceGPIOA       ((uint8_t)0x00)
#define EXTI_PortSourceGPIOB       ((uint8_t)0x01)
#define EXTI_PortSourceGPIOC       ((uint8_t)0x02)
#define EXTI_PortSourceGPIOD       ((uint8_t)0x03)

#define IS_EXTI_PORT_SOURCE(PORTSOURCE) (((PORTSOURCE) == EXTI_PortSourceGPIOA) || \
                                         ((PORTSOURCE) == EXTI_PortSourceGPIOB) || \
                                         ((PORTSOURCE) == EXTI_PortSourceGPIOC) || \
                                         ((PORTSOURCE) == EXTI_PortSourceGPIOD) )
                                         
/**
  * @}
  */ 


/** @defgroup SYSCFG_EXTI_Pin_Sources SYSCFG_EXTI_Pin_Sources
  * @{
  */ 
#define EXTI_PinSource0            ((uint8_t)0x00)
#define EXTI_PinSource1            ((uint8_t)0x01)
#define EXTI_PinSource2            ((uint8_t)0x02)
#define EXTI_PinSource3            ((uint8_t)0x03)
#define EXTI_PinSource4            ((uint8_t)0x04)
#define EXTI_PinSource5            ((uint8_t)0x05)
#define EXTI_PinSource6            ((uint8_t)0x06)
#define EXTI_PinSource7            ((uint8_t)0x07) 
#define IS_EXTI_PIN_SOURCE(PINSOURCE) (((PINSOURCE) == EXTI_PinSource0)  || \
                                       ((PINSOURCE) == EXTI_PinSource1)  || \
                                       ((PINSOURCE) == EXTI_PinSource2)  || \
                                       ((PINSOURCE) == EXTI_PinSource3)  || \
                                       ((PINSOURCE) == EXTI_PinSource4)  || \
                                       ((PINSOURCE) == EXTI_PinSource5)  || \
                                       ((PINSOURCE) == EXTI_PinSource6)  || \
                                       ((PINSOURCE) == EXTI_PinSource7)  )

/**
  * @}
  */ 
  
/** @defgroup SYSCFG_memoryremap SYSCFG_memoryremap
  * @{
  */ 
#define SYSCFG_MemoryRemap_Flash         ((uint8_t)0x00)
#define SYSCFG_MemoryRemap_SRAM           ((uint8_t)0x03)

#define IS_SYSCFG_MEMORY_REMAP_CONFING(REMAP) (((REMAP) == SYSCFG_MemoryRemap_Flash) || ((REMAP) == SYSCFG_MemoryRemap_SRAM))

/**
  * @}
  */ 
  
/** @defgroup SYSCFG_Cortex-M0_LOCKUP_LOCK SYSCFG_Cortex-M0_LOCKUP_LOCK
  * @{
*/ 

#define SYSCFG_Lockup_TIM1Break_OFF       ((uint8_t)0x00)
#define SYSCFG_Lockup_TIM1Break_ON        ((uint8_t)0x01)

#define IS_SYSCFG_LOCKUP_TIM1BREAK_ONOFF(ONOFF) (((ONOFF) == SYSCFG_Lockup_TIM1Break_OFF)       || \
                                              ((ONOFF) == SYSCFG_Lockup_TIM1Break_ON)   )
           
/**
  * @}
  */ 
  
/** @defgroup SYSYCFG_CFGR1_registers_mask SYSYCFG_CFGR1_registers_mask
  * @{
*/ 

#define MEM_REMAP_MASK                    ((uint32_t)0xFFFFFFC)
#define MEM_LOCKUP_OUT_MASK               ((uint32_t)0x7FFFFFF)                      

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
/* Exported functions --------------------------------------------------------*/ 
 
void       SYSCFG_DeInit(void);
void       SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex);
void       SYSCFG_MemoryRemapConfig(uint8_t SYSCFG_MemoryRemap);
void       SYSCFG_Lockup_Tim1BreakConfig(uint8_t Lockup_lockOnOff);

#ifdef __cplusplus
}
#endif

#endif /*__HK32F030MXMXXA_SYSCFG_H */



