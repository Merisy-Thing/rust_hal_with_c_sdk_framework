/********************************************************************
* @copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
* @filename: hk32f0301mxxc_exti.h
* @brief: //本文件功能简要描述
* @author: AE Team
********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_EXTI_H
#define __HK32F0301MXXC_EXTI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"

/** @addtogroup EXTI
	* @{
	*/

/** @defgroup EXTI_Exported_Types EXTI_Exported_Types
  * @{
  */

/**
  * @brief  EXTI mode enumeration
  */
typedef enum
{
  EXTI_Mode_Interrupt = 0x00,
  EXTI_Mode_Event = 0x04
}EXTIMode_TypeDef;

#define IS_EXTI_MODE(MODE) (((MODE) == EXTI_Mode_Interrupt) || ((MODE) == EXTI_Mode_Event))

/**
  * @brief  EXTI Trigger enumeration
  */
typedef enum
{
  EXTI_Trigger_Rising = 0x08,
  EXTI_Trigger_Falling = 0x0C,
  EXTI_Trigger_Rising_Falling = 0x10
}EXTITrigger_TypeDef;

#define IS_EXTI_TRIGGER(TRIGGER) (((TRIGGER) == EXTI_Trigger_Rising) || \
                                  ((TRIGGER) == EXTI_Trigger_Falling) || \
                                  ((TRIGGER) == EXTI_Trigger_Rising_Falling))
/**
  * @brief  EXTI Init Structure definition
  */

typedef struct
{
  uint32_t EXTI_Line;               /*!< Specifies the EXTI lines to be enabled or disabled.
                                         This parameter can be any combination of @ref EXTI_Lines */

  EXTIMode_TypeDef EXTI_Mode;       /*!< Specifies the mode for the EXTI lines.
                                         This parameter can be a value of @ref EXTIMode_TypeDef */

  EXTITrigger_TypeDef EXTI_Trigger; /*!< Specifies the trigger signal active edge for the EXTI lines.
                                         This parameter can be a value of @ref EXTIMode_TypeDef */

  FunctionalState EXTI_LineCmd;     /*!< Specifies the new state of the selected EXTI lines.
                                         This parameter can be set either to ENABLE or DISABLE */
}EXTI_InitTypeDef;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup EXTI_Exported_Constants EXTI_Exported_Constants
  * @{
  */

/** @defgroup EXTI_Lines EXTI_Lines
  * @{
  */

#define EXTI_Line0       ((uint32_t)0x00001)  /*!< External interrupt line 0 */
#define EXTI_Line1       ((uint32_t)0x00002)  /*!< External interrupt line 1 */
#define EXTI_Line2       ((uint32_t)0x00004)  /*!< External interrupt line 2 */
#define EXTI_Line3       ((uint32_t)0x00008)  /*!< External interrupt line 3 */
#define EXTI_Line4       ((uint32_t)0x00010)  /*!< External interrupt line 4 */
#define EXTI_Line5       ((uint32_t)0x00020)  /*!< External interrupt line 5 */
#define EXTI_Line6       ((uint32_t)0x00040)  /*!< External interrupt line 6 */
#define EXTI_Line7       ((uint32_t)0x00080)  /*!< External interrupt line 7 */
#define EXTI_Line8       ((uint32_t)0x00100)  /*!< External interrupt line 8 Connected to the ADC AWD event */
#define EXTI_Line10      ((uint32_t)0x00400)  /*!< External interrupt line 10 Connected to the IIC wakeup event */
#define EXTI_Line11      ((uint32_t)0x00800)  /*!< External interrupt line 11 Connected to the AWU Wakeup event */


/**
  * @}
  */

/** @defgroup EXTI_Lines_check EXTI_Lines_check
  * @{
  */
#define IS_EXTI_LINE(LINE) ((((LINE) & (uint32_t)0xFFFFF200) == 0x00) && ((LINE) != (uint16_t)0x00))
#define IS_GET_EXTI_LINE(LINE) (((LINE) == EXTI_Line0) || ((LINE) == EXTI_Line1) || \
                            ((LINE) == EXTI_Line2) || ((LINE) == EXTI_Line3) || \
                            ((LINE) == EXTI_Line4) || ((LINE) == EXTI_Line5) || \
                            ((LINE) == EXTI_Line6) || ((LINE) == EXTI_Line7) || \
                            ((LINE) == EXTI_Line8) ||  \
                            ((LINE) == EXTI_Line10) || ((LINE) == EXTI_Line11) )

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
/* Function used to set the EXTI configuration to the default reset state *****/
void EXTI_DeInit(void);

/* Initialization and Configuration functions *********************************/
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct);
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);

/* Interrupts and flags management functions **********************************/
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
void EXTI_ClearFlag(uint32_t EXTI_Line);
void EXTI_ClearITPendingBit(uint32_t EXTI_Line);



#ifdef __cplusplus
}
#endif

#endif
