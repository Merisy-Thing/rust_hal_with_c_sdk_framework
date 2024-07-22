/********************************************************************************
  * @file    hk32f0301mxxc_uart.h 
  * @author  AE Team
  * @version V1.0.0/2022-09-07
  *     1.初始版本
  * @log  V1.0.0
  *******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F030XMXXC_UART_H
#define __HK32F030XMXXC_UART_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"
 

/** @addtogroup UART
  * @{
  */ 

/* Exported types ------------------------------------------------------------*/

/** @defgroup UART_Exported_Types UART_Exported_Types
  * @{
  */
   
/** 
  * @brief  UART Init Structure definition  
  */ 

typedef struct
{
  uint32_t UART_BaudRate;            /*!< This member configures the UART communication baud rate.
                                           The baud rate is computed using the following formula:
                                            - IntegerDivider = ((PCLKx) / (16 * (UART_InitStruct->UART_BaudRate)))
                                            - FractionalDivider = ((IntegerDivider - ((uint32_t) IntegerDivider)) * 16) + 0.5 */

  uint32_t UART_WordLength;          /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref UART_Word_Length */

  uint32_t UART_StopBits;            /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref UART_Stop_Bits */

  uint32_t UART_Parity;              /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref UART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */
 
  uint32_t UART_Mode;                /*!< Specifies wether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Mode */
} UART_InitTypeDef;



/* Exported constants --------------------------------------------------------*/

/** @defgroup UART_Exported_Constants UART_Exported_Constants
  * @{
  */ 									

#define IS_UART_1_PERIPH(PERIPH)   (((PERIPH) == UART1))

#define IS_UART_ALL_PERIPH(PERIPH) (((PERIPH) == UART1) ||\
																		((PERIPH) == UART2))
/**
  * @}
  */
/** @defgroup UART_Word_Length  UART_Word_Length
  * @{
  */ 

#define UART_WordLength_8b                  ((uint32_t)0x00000000)
#define UART_WordLength_9b                  UART_CR1_M0 /* should be ((uint32_t)0x00001000) */
#define UART_WordLength_7b                  ((uint32_t)0x10001000) 
#define IS_UART_WORD_LENGTH(LENGTH) (((LENGTH) == UART_WordLength_8b) || \
                                      ((LENGTH) == UART_WordLength_9b) || \
                                      ((LENGTH) == UART_WordLength_7b))
/**
  * @}
  */ 

/** @defgroup UART_Stop_Bits UART_Stop_Bits
  * @{
  */ 

#define UART_StopBits_1                     ((uint32_t)0x00000000)
#define UART_StopBits_2                     UART_CR2_STOP_1
#define UART_StopBits_1_5                   (UART_CR2_STOP_0 | UART_CR2_STOP_1)
#define IS_UART_STOPBITS(STOPBITS) (((STOPBITS) == UART_StopBits_1) || \
                                     ((STOPBITS) == UART_StopBits_2) || \
                                     ((STOPBITS) == UART_StopBits_1_5))
/**
  * @}
  */ 

/** @defgroup UART_Parity UART_Parity
  * @{
  */ 

#define UART_Parity_No                      ((uint32_t)0x00000000)
#define UART_Parity_Even                    UART_CR1_PCE
#define UART_Parity_Odd                     (UART_CR1_PCE | UART_CR1_PS) 
#define IS_UART_PARITY(PARITY) (((PARITY) == UART_Parity_No) || \
                                 ((PARITY) == UART_Parity_Even) || \
                                 ((PARITY) == UART_Parity_Odd))
/**
  * @}
  */ 

/** @defgroup  UART_DirectionMode UART_DirectionMode
  * @{
  */ 

#define UART_Mode_Rx                        UART_CR1_RE
#define UART_Mode_Tx                        UART_CR1_TE
#define IS_UART_MODE(MODE) ((((MODE) & (uint32_t)0xFFFFFFF3) == 0x00) && \
                              ((MODE) != (uint32_t)0x00))
/**
  * @}
  */ 

/** @defgroup UART_Hardware_Flow_Control UART_Hardware_Flow_Control
  * @{
  */ 

#define UART_HardwareFlowControl_None       ((uint32_t)0x00000000)
#define UART_HardwareFlowControl_RTS        UART_CR3_RTSE
#define UART_HardwareFlowControl_CTS        UART_CR3_CTSE
#define UART_HardwareFlowControl_RTS_CTS    (UART_CR3_RTSE | UART_CR3_CTSE)
#define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              (((CONTROL) == UART_HardwareFlowControl_None) || \
                               ((CONTROL) == UART_HardwareFlowControl_RTS) || \
                               ((CONTROL) == UART_HardwareFlowControl_CTS) || \
                               ((CONTROL) == UART_HardwareFlowControl_RTS_CTS))
/**
  * @}
  */ 


  


/** @defgroup UART_MuteMode_WakeUp_methods UART_MuteMode_WakeUp_methods
  * @{
  */

#define UART_WakeUp_IdleLine                ((uint32_t)0x00000000)
#define UART_WakeUp_AddressMark             UART_CR1_WAKE
#define IS_UART_MUTEMODE_WAKEUP(WAKEUP) (((WAKEUP) == UART_WakeUp_IdleLine) || \
                                          ((WAKEUP) == UART_WakeUp_AddressMark))
/**
  * @}
  */

/** @defgroup UART_Address_Detection UART_Address_Detection
  * @{
  */ 

#define UART_AddressLength_4b               ((uint32_t)0x00000000)
#define UART_AddressLength_7b               UART_CR2_ADDM7
#define IS_UART_ADDRESS_DETECTION(ADDRESS) (((ADDRESS) == UART_AddressLength_4b) || \
                                             ((ADDRESS) == UART_AddressLength_7b))
/**
  * @}
  */ 



/** @defgroup UART_DE_Polarity UART_DE_Polarity
  * @{
  */

#define UART_DEPolarity_High                ((uint32_t)0x00000000)
#define UART_DEPolarity_Low                 UART_CR3_DEP
#define IS_UART_DE_POLARITY(POLARITY) (((POLARITY) == UART_DEPolarity_Low) || \
                                        ((POLARITY) == UART_DEPolarity_High))
/**
  * @}
  */ 

/** @defgroup UART_Inversion_Pins UART_Inversion_Pins
  * @{
  */

#define UART_InvPin_Tx                      UART_CR2_TXINV
#define UART_InvPin_Rx                      UART_CR2_RXINV
#define IS_UART_INVERSTION_PIN(PIN) ((((PIN) & (uint32_t)0xFFFCFFFF) == 0x00) && \
                                       ((PIN) != (uint32_t)0x00))

/**
  * @}
  */ 


/** @defgroup UART_OVR_DETECTION UART_OVR_DETECTION
  * @{
  */

#define UART_OVRDetection_Enable            ((uint32_t)0x00000000)
#define UART_OVRDetection_Disable           UART_CR3_OVERDIS
#define IS_UART_OVRDETECTION(OVR) (((OVR) == UART_OVRDetection_Enable)|| \
                                    ((OVR) == UART_OVRDetection_Disable))
/**
  * @}
  */ 
/** @defgroup UART_Request UART_Request
  * @{
  */

#define UART_Request_SBKRQ                  UART_RQR_SBKRQ
#define UART_Request_MMRQ                   UART_RQR_MMRQ
#define UART_Request_RXFRQ                  UART_RQR_RXFRQ

#define IS_UART_REQUEST(REQUEST) (((REQUEST) == UART_Request_RXFRQ) || \
                                   ((REQUEST) == UART_Request_MMRQ) || \
                                   ((REQUEST) == UART_Request_SBKRQ))
/**
  * @}
  */ 

/** @defgroup UART_Flags UART_Flags
  * @{
  */
#define UART_FLAG_REACK                     UART_ISR_REACK
#define UART_FLAG_TEACK                     UART_ISR_TEACK
#define UART_FLAG_RWU                       UART_ISR_RWU 
#define UART_FLAG_SBK                       UART_ISR_SBKF
#define UART_FLAG_BUSY                      UART_ISR_BUSY
#define UART_FLAG_TXE                       UART_ISR_TXE
#define UART_FLAG_TC                        UART_ISR_TC
#define UART_FLAG_RXNE                      UART_ISR_RXNE
#define UART_FLAG_IDLE                      UART_ISR_IDLE
#define UART_FLAG_ORE                       UART_ISR_ORE
#define UART_FLAG_NE                        UART_ISR_NF
#define UART_FLAG_FE                        UART_ISR_FE
#define UART_FLAG_PE                        UART_ISR_PE
#define IS_UART_FLAG(FLAG) (((FLAG) == UART_FLAG_PE) || ((FLAG) == UART_FLAG_TXE) || \
                             ((FLAG) == UART_FLAG_TC) || ((FLAG) == UART_FLAG_RXNE) || \
                             ((FLAG) == UART_FLAG_IDLE) || ((FLAG) == UART_FLAG_ORE) || \
                             ((FLAG) == UART_FLAG_NE) || ((FLAG) == UART_FLAG_FE) || \
                             ((FLAG) == UART_FLAG_BUSY) || ((FLAG) == UART_FLAG_SBK))

#define IS_UART_CLEAR_FLAG(FLAG) (((FLAG) == UART_FLAG_PE) || ((FLAG) == UART_FLAG_TXE) || \
                                  ((FLAG) == UART_FLAG_TC) || ((FLAG) == UART_FLAG_RXNE) || \
                                  ((FLAG) == UART_FLAG_IDLE) || ((FLAG) == UART_FLAG_ORE) || \
                                  ((FLAG) == UART_FLAG_NE) || ((FLAG) == UART_FLAG_FE) || \
                                  ((FLAG) == UART_FLAG_BUSY) || ((FLAG) == UART_FLAG_SBK))
/**
  * @}
  */ 

/** @defgroup UART_Interrupt_definition UART_Interrupt_definition
  * @brief UART Interrupt definition
  * UART_IT possible values
  * Elements values convention: 0xZZZZYYXX
  *   XX: Position of the corresponding Interrupt
  *   YY: Register index
  *   ZZZZ: Flag position
  * @{
  */

#define UART_IT_WU                          ((uint32_t)0x00140316) 
#define UART_IT_PE                          ((uint32_t)0x00000108)
#define UART_IT_TXE                         ((uint32_t)0x00070107)
#define UART_IT_TC                          ((uint32_t)0x00060106)
#define UART_IT_RXNE                        ((uint32_t)0x00050105)
#define UART_IT_IDLE                        ((uint32_t)0x00040104)
#define UART_IT_ERR                         ((uint32_t)0x00000300)
#define UART_IT_ORE                         ((uint32_t)0x00030300)
#define UART_IT_NE                          ((uint32_t)0x00020300)
#define UART_IT_FE                          ((uint32_t)0x00010300)

#define IS_UART_CONFIG_IT(IT) (((IT) == UART_IT_PE) || ((IT) == UART_IT_TXE) || \
                                ((IT) == UART_IT_TC) || ((IT) == UART_IT_RXNE) || \
                                ((IT) == UART_IT_IDLE) || ((IT) == UART_IT_ERR))

#define IS_UART_GET_IT(IT) (((IT) == UART_IT_PE) || ((IT) == UART_IT_TXE) || \
                             ((IT) == UART_IT_TC) || ((IT) == UART_IT_RXNE) || \
                             ((IT) == UART_IT_IDLE) || ((IT) == UART_IT_ORE) || \
                             ((IT) == UART_IT_NE) || ((IT) == UART_IT_FE))

#define IS_UART_CLEAR_IT(IT) (((IT) == UART_IT_PE) || ((IT) == UART_IT_TXE) || \
                             ((IT) == UART_IT_TC) || ((IT) == UART_IT_RXNE) || \
                             ((IT) == UART_IT_IDLE) || ((IT) == UART_IT_ORE) || \
                             ((IT) == UART_IT_NE) || ((IT) == UART_IT_FE))
/**
  * @}
  */

/** @defgroup IS_UART_DE_ASSERTION_DEASSERTION_TIME IS_UART_DE_ASSERTION_DEASSERTION_TIME
  * @{
  */

#define IS_UART_BAUDRATE(BAUDRATE) (((BAUDRATE) > 0) && ((BAUDRATE) < 0x005B8D81))
#define IS_UART_DE_ASSERTION_DEASSERTION_TIME(TIME) ((TIME) <= 0x1F)
#define IS_UART_AUTO_RETRY_COUNTER(COUNTER) ((COUNTER) <= 0x7)
#define IS_UART_TIMEOUT(TIMEOUT) ((TIMEOUT) <= 0x00FFFFFF)
#define IS_UART_DATA(DATA) ((DATA) <= 0x1FF)

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

/* Initialization and Configuration functions *********************************/
void UART_DeInit(UART_TypeDef* UARTx);
void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef* UART_InitStruct);
void UART_StructInit(UART_InitTypeDef* UART_InitStruct);
void UART_Cmd(UART_TypeDef* UARTx, FunctionalState NewState);
void UART_DirectionModeCmd(UART_TypeDef* UARTx, uint32_t UART_DirectionMode, FunctionalState NewState);
void UART_SetPrescaler(UART_TypeDef* UARTx, uint8_t UART_Prescaler);
void UART_OverSampling8Cmd(UART_TypeDef* UARTx, FunctionalState NewState);
void UART_OneBitMethodCmd(UART_TypeDef* UARTx, FunctionalState NewState);
void UART_MSBFirstCmd(UART_TypeDef* UARTx, FunctionalState NewState);
void UART_DataInvCmd(UART_TypeDef* UARTx, FunctionalState NewState);
void UART_InvPinCmd(UART_TypeDef* UARTx, uint32_t UART_InvPin, FunctionalState NewState);

/* Data transfers functions ***************************************************/
void UART_SendData(UART_TypeDef* UARTx, uint16_t Data);
uint16_t UART_ReceiveData(UART_TypeDef* UARTx);

/* Multi-Processor Communication functions ************************************/
void UART_SetAddress(UART_TypeDef* UARTx, uint8_t UART_Address);
void UART_MuteModeWakeUpConfig(UART_TypeDef* UARTx, uint32_t UART_WakeUp);
void UART_MuteModeCmd(UART_TypeDef* UARTx, FunctionalState NewState);
void UART_AddressDetectionConfig(UART_TypeDef* UARTx, uint32_t UART_AddressLength);

/* Half-duplex mode function **************************************************/
void UART_HalfDuplexCmd(UART_TypeDef* UARTx, FunctionalState NewState);

/* RS485 mode functions *******************************************************/
void UART_DECmd(UART_TypeDef* UARTx, FunctionalState NewState);
void UART_DEPolarityConfig(UART_TypeDef* UARTx, uint32_t UART_DEPolarity);
void UART_SetDEAssertionTime(UART_TypeDef* UARTx, uint32_t UART_DEAssertionTime);
void UART_SetDEDeassertionTime(UART_TypeDef* UARTx, uint32_t UART_DEDeassertionTime);


/* Interrupts and flags management functions **********************************/
void UART_ITConfig(UART_TypeDef* UARTx, uint32_t UART_IT, FunctionalState NewState);
void UART_RequestCmd(UART_TypeDef* UARTx, uint32_t UART_Request, FunctionalState NewState);
void UART_OverrunDetectionConfig(UART_TypeDef* UARTx, uint32_t UART_OVRDetection);
FlagStatus UART_GetFlagStatus(UART_TypeDef* UARTx, uint32_t UART_FLAG);
void UART_ClearFlag(UART_TypeDef* UARTx, uint32_t UART_FLAG);
ITStatus UART_GetITStatus(UART_TypeDef* UARTx, uint32_t UART_IT);
void UART_ClearITPendingBit(UART_TypeDef* UARTx, uint32_t UART_IT);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F030XMXXC_UART_H */


 
