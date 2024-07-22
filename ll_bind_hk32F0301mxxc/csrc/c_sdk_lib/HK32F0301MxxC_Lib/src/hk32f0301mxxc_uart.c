/********************************************************************************
  * @file    hk32f0301mxxc_uart.c 
  * @author  AE Team
  * @version V1.0.0/2022-09-07
  *     1.初始版本
  * @log  V1.0.0
 ===============================================================================
                       ##### How to use this driver #####
 ===============================================================================
    [..]
        (#) Enable peripheral clock using RCC_APB2PeriphClockCmd(RCC_APBPeriph2_UART1, ENABLE)
            function for UART1 or using RCC_APB1PeriphClockCmd(RCC_APBPeriph1_UART2, ENABLE)
            function for UART2.
        (#) According to the UART mode, enable the GPIO clocks using 
            RCC_AHBPeriphClockCmd() function. (The I/O can be TX, RX, CTS, 
            or and SCLK). 
        (#) Peripheral's alternate function: 
            (++) Connect the pin to the desired peripherals' Alternate 
                 Function (AF) using GPIO_PinAFConfig() function.
            (++) Configure the desired pin in alternate function by:
                 GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF.
            (++) Select the type, pull-up/pull-down and output speed via 
                 GPIO_PuPd, GPIO_OType and GPIO_Speed members.
            (++) Call GPIO_Init() function.        
        (#) Program the Baud Rate, Word Length , Stop Bit, Parity, Hardware 
            flow control and Mode(Receiver/Transmitter) using the SPI_Init()
            function.  
        (#) For synchronous mode, enable the clock and program the polarity,
            phase and last bit using the UART_ClockInit() function.  
        (#) Enable the NVIC and the corresponding interrupt using the function 
            UART_ITConfig() if you need to use interrupt mode.   
        (#) Enable the UART using the UART_Cmd() function.   
            
    [..]       
    		(#) 用RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE)使能UART1或使用
              RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART2, ENABLE)使能UART2。

        (#) 根据UART模式，使能GPIO时钟使用RCC_AHBPeriphClockCmd()函数（IO可以是TX、RX）。  

        (#) 外设复用功能： 
        		(++)连接引脚到所需外设使用复用函数RCC_AHBPeriphClockCmd()。
        		(++)通过GPIO_PuPd，GPIO_OType和GPIO_Speed选择上下拉，输出类型和输出速度。
        		(++)调用GPIO_Init()函数。
        (#) 使用UART_Init()函数配置波特率，字长，停止位，奇偶校验，硬件流控和模式（接收/发送）。
        (#) 要使能同步模式，需要使用UART_ClockInit()函数使能时钟，并配置极性，相位和最后一个时钟脉冲位。
        (#) 如果需要使用中断，使用UART_ITConfig()使能NVIC和相应的中断。

        (#) 使用UART_Cmd()函数使能UART。       
  *******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_uart.h"
#include "hk32f0301mxxc_rcc.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup UART  UART
  * @brief UART driver modules
  * @{
  */
  
/** @defgroup UART_Private_Defines UART_Private_Defines
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*!< UART CR1 register clear Mask ((~(uint32_t)0xFFFFE6F3)) */
#define UART_CR1_CLEAR_MASK       ((uint32_t)(UART_CR1_M0 | UART_CR1_PCE | \
                                              UART_CR1_PS | UART_CR1_TE | \
                                              UART_CR1_RE))
																							

/*!< UART Interrupts mask */
#define IT_MASK                   ((uint32_t)0x000000FF)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @}
  */
/** @defgroup UART_Private_Functions UART_Private_Functions
  * @{
  */

/** @defgroup UART_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
          ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]
        This subsection provides a set of functions allowing to initialize the UART 
        in asynchronous and in synchronous modes.
        (+) For the asynchronous mode only these parameters can be configured: 
          (++) Baud Rate.
          (++) Word Length.
          (++) Stop Bit.
          (++) Parity: If the parity is enabled, then the MSB bit of the data written
               in the data register is transmitted but is changed by the parity bit.
               Depending on the frame length defined by the M bit (8-bits or 9-bits),
               the possible UART frame formats are as listed in the following table:

   +-------------------------------------------------------------+     
   |   M bit |  PCE bit  |            UART frame                |
   |---------------------|---------------------------------------|             
   |    0    |    0      |    | SB | 8 bit data | STB |          |
   |---------|-----------|---------------------------------------|  
   |    0    |    1      |    | SB | 7 bit data | PB | STB |     |
   |---------|-----------|---------------------------------------|  
   |    1    |    0      |    | SB | 9 bit data | STB |          |
   |---------|-----------|---------------------------------------|  
   |    1    |    1      |    | SB | 8 bit data | PB | STB |     |
   +-------------------------------------------------------------+            

          (++) Hardware flow control.
          (++) Receiver/transmitter modes.
    [..] The UART_Init() function follows the UART  asynchronous configuration 
         procedure(details for the procedure are available in reference manual.
        (+) For the synchronous mode in addition to the asynchronous mode parameters
            these parameters should be also configured:
            (++) UART Clock Enabled.
            (++) UART polarity.
            (++) UART phase.
            (++) UART LastBit.
    [..] These parameters can be configured using the UART_ClockInit() function.

@endverbatim
  * @{
  */
  
/**
  * @brief  Deinitializes the UARTx peripheral registers to their default reset values.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @retval None
  */
void UART_DeInit(UART_TypeDef* UARTx)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));

  if (UARTx == UART1)
  {
    RCC_APBPeriph2ResetCmd(RCC_APBPeriph2_UART1, ENABLE);
    RCC_APBPeriph2ResetCmd(RCC_APBPeriph2_UART1, DISABLE);
  }  
  else
  {
    RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_UART2, ENABLE);
    RCC_APBPeriph1ResetCmd(RCC_APBPeriph1_UART2, DISABLE);
  }   
	
	
}

/**
  * @brief  Initializes the UARTx peripheral according to the specified
  *         parameters in the UART_InitStruct .
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_InitStruct: pointer to a UART_InitTypeDef structure that contains
  *         the configuration information for the specified UART peripheral.
  * @retval None
  */
void UART_Init(UART_TypeDef* UARTx, UART_InitTypeDef* UART_InitStruct)
{
  uint32_t divider = 0, apbclock = 0, tmpreg = 0;
  RCC_ClocksTypeDef RCC_ClocksStatus;
  
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_BAUDRATE(UART_InitStruct->UART_BaudRate));  
  assert_param(IS_UART_WORD_LENGTH(UART_InitStruct->UART_WordLength));
  assert_param(IS_UART_STOPBITS(UART_InitStruct->UART_StopBits));
  assert_param(IS_UART_PARITY(UART_InitStruct->UART_Parity));
  assert_param(IS_UART_MODE(UART_InitStruct->UART_Mode));
  
  /* Disable UART */
  UARTx->CR1 &= (uint32_t)~((uint32_t)UART_CR1_UE);
  
  /*---------------------------- UART CR2 Configuration -----------------------*/
  tmpreg = UARTx->CR2;
  /* Clear STOP[13:12] bits */
  tmpreg &= (uint32_t)~((uint32_t)UART_CR2_STOP);
  
  /* Configure the UART Stop Bits, Clock, CPOL, CPHA and LastBit ------------*/
  /* Set STOP[13:12] bits according to UART_StopBits value */
  tmpreg |= (uint32_t)UART_InitStruct->UART_StopBits;
  
  /* Write to UART CR2 */
  UARTx->CR2 = tmpreg;
  
  /*---------------------------- UART CR1 Configuration -----------------------*/
  tmpreg = UARTx->CR1;
  /* Clear M, PCE, PS, TE and RE bits */
  tmpreg &= (uint32_t)~((uint32_t)UART_CR1_CLEAR_MASK);
  
  /* Configure the UART Word Length, Parity and mode ----------------------- */
  /* Set the M bits according to UART_WordLength value */
  /* Set PCE and PS bits according to UART_Parity value */
  /* Set TE and RE bits according to UART_Mode value */
  tmpreg |= (uint32_t)UART_InitStruct->UART_WordLength | UART_InitStruct->UART_Parity |
    UART_InitStruct->UART_Mode;
  
  /* Write to UART CR1 */
  UARTx->CR1 = tmpreg;
  
  
  /*---------------------------- UART BRR Configuration -----------------------*/
  /* Configure the UART Baud Rate -------------------------------------------*/
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  
  if (UARTx == UART1)
  {
    apbclock = RCC_ClocksStatus.UART1CLK_Frequency;
  }
  else if (UARTx == UART2)
  {
    apbclock = RCC_ClocksStatus.UART2CLK_Frequency;
  }
  else
  {
    apbclock = RCC_ClocksStatus.PCLK_Frequency;
  }
  
  /* Determine the integer part */
  if ((UARTx->CR1 & UART_CR1_OVER8) != 0)
  {
    /* (divider * 10) computing in case Oversampling mode is 8 Samples */
    divider = (uint32_t)((2 * apbclock) / (UART_InitStruct->UART_BaudRate));
    tmpreg  = (uint32_t)((2 * apbclock) % (UART_InitStruct->UART_BaudRate));
  }
  else /* if ((UARTx->CR1 & CR1_OVER8_Set) == 0) */
  {
    /* (divider * 10) computing in case Oversampling mode is 16 Samples */
    divider = (uint32_t)((apbclock) / (UART_InitStruct->UART_BaudRate));
    tmpreg  = (uint32_t)((apbclock) % (UART_InitStruct->UART_BaudRate));
  }
  
  /* round the divider : if fractional part i greater than 0.5 increment divider */
  if (tmpreg >=  (UART_InitStruct->UART_BaudRate) / 2)
  {
    divider++;
  } 
  
  /* Implement the divider in case Oversampling mode is 8 Samples */
  if ((UARTx->CR1 & UART_CR1_OVER8) != 0)
  {
    /* get the LSB of divider and shift it to the right by 1 bit */
    tmpreg = (divider & (uint16_t)0x000F) >> 1;
    
    /* update the divider value */
    divider = (divider & (uint16_t)0xFFF0) | tmpreg;
  }
  
  /* Write to UART BRR */
  UARTx->BRR = (uint16_t)divider;
}

/**
  * @brief  Fills each UART_InitStruct member with its default value.
  * @param  UART_InitStruct: pointer to a UART_InitTypeDef structure
  *         which will be initialized.
  * @retval None
  */
void UART_StructInit(UART_InitTypeDef* UART_InitStruct)
{
  /* UART_InitStruct members default value */
  UART_InitStruct->UART_BaudRate = 9600;
  UART_InitStruct->UART_WordLength = UART_WordLength_8b;
  UART_InitStruct->UART_StopBits = UART_StopBits_1;
  UART_InitStruct->UART_Parity = UART_Parity_No ;
  UART_InitStruct->UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
}


/**
  * @brief  Enables or disables the specified UART peripheral.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.    
  * @param  NewState: new state of the UARTx peripheral.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void UART_Cmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the selected UART by setting the UE bit in the CR1 register */
    UARTx->CR1 |= UART_CR1_UE;
  }
  else
  {
    /* Disable the selected UART by clearing the UE bit in the CR1 register */
    UARTx->CR1 &= (uint32_t)~((uint32_t)UART_CR1_UE);
  }
}

/**
  * @brief  Enables or disables the UART's transmitter or receiver.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_DirectionMode: specifies the UART direction.
  *          This parameter can be any combination of the following values:
  *            @arg UART_Mode_Tx: UART Transmitter
  *            @arg UART_Mode_Rx: UART Receiver
  * @param  NewState: new state of the UART transfer direction.
  *          This parameter can be: ENABLE or DISABLE.  
  * @retval None
  */
void UART_DirectionModeCmd(UART_TypeDef* UARTx, uint32_t UART_DirectionMode, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_MODE(UART_DirectionMode));
  assert_param(IS_FUNCTIONAL_STATE(NewState)); 

  if (NewState != DISABLE)
  {
    /* Enable the UART's transfer interface by setting the TE and/or RE bits 
       in the UART CR1 register */
    UARTx->CR1 |= UART_DirectionMode;
  }
  else
  {
    /* Disable the UART's transfer interface by clearing the TE and/or RE bits
       in the UART CR3 register */
    UARTx->CR1 &= (uint32_t)~UART_DirectionMode;
  }
}

/**
  * @brief  Enables or disables the UART's 8x oversampling mode.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  NewState: new state of the UART 8x oversampling mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   This function has to be called before calling UART_Init() function
  *         in order to have correct baudrate Divider value.
  * @retval None
  */
void UART_OverSampling8Cmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the 8x Oversampling mode by setting the OVER8 bit in the CR1 register */
    UARTx->CR1 |= UART_CR1_OVER8;
  }
  else
  {
    /* Disable the 8x Oversampling mode by clearing the OVER8 bit in the CR1 register */
    UARTx->CR1 &= (uint32_t)~((uint32_t)UART_CR1_OVER8);
  }
}  

/**
  * @brief  Enables or disables the UART's one bit sampling method.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  NewState: new state of the UART one bit sampling method.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   This function has to be called before calling UART_Cmd() function.  
  * @retval None
  */
void UART_OneBitMethodCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the one bit method by setting the ONEBITE bit in the CR3 register */
    UARTx->CR3 |= UART_CR3_ONEBIT;
  }
  else
  {
    /* Disable the one bit method by clearing the ONEBITE bit in the CR3 register */
    UARTx->CR3 &= (uint32_t)~((uint32_t)UART_CR3_ONEBIT);
  }
}

/**
  * @brief  Enables or disables the UART's most significant bit first 
  *         transmitted/received following the start bit.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  NewState: new state of the UART most significant bit first
  *         transmitted/received following the start bit.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   This function has to be called before calling UART_Cmd() function.  
  * @retval None
  */
void UART_MSBFirstCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the most significant bit first transmitted/received following the 
       start bit by setting the MSBFIRST bit in the CR2 register */
    UARTx->CR2 |= UART_CR2_MSBFIRST;
  }
  else
  {
    /* Disable the most significant bit first transmitted/received following the 
       start bit by clearing the MSBFIRST bit in the CR2 register */
    UARTx->CR2 &= (uint32_t)~((uint32_t)UART_CR2_MSBFIRST);
  }
}

/**
  * @brief  Enables or disables the binary data inversion.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  NewState: new defined levels for the UART data.
  *          This parameter can be:
  *            @arg ENABLE: Logical data from the data register are send/received in negative
  *                          logic (1=L, 0=H). The parity bit is also inverted.
  *            @arg DISABLE: Logical data from the data register are send/received in positive
  *                          logic (1=H, 0=L) 
  * @note   This function has to be called before calling UART_Cmd() function.  
  * @retval None
  */
void UART_DataInvCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the binary data inversion feature by setting the DATAINV bit in 
       the CR2 register */
    UARTx->CR2 |= UART_CR2_DATAINV;
  }
  else
  {
    /* Disable the binary data inversion feature by clearing the DATAINV bit in 
       the CR2 register */
    UARTx->CR2 &= (uint32_t)~((uint32_t)UART_CR2_DATAINV);
  }
}

/**
  * @brief  Enables or disables the Pin(s) active level inversion.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_InvPin: specifies the UART pin(s) to invert.
  *          This parameter can be any combination of the following values:
  *            @arg UART_InvPin_Tx: UART Tx pin active level inversion.
  *            @arg UART_InvPin_Rx: UART Rx pin active level inversion.
  * @param  NewState: new active level status for the UART pin(s).
  *          This parameter can be:
  *            @arg ENABLE: pin(s) signal values are inverted (Vdd =0, Gnd =1).
  *            @arg DISABLE: pin(s) signal works using the standard logic levels (Vdd =1, Gnd =0).
  * @note   This function has to be called before calling UART_Cmd() function.  
  * @retval None
  */
void UART_InvPinCmd(UART_TypeDef* UARTx, uint32_t UART_InvPin, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_INVERSTION_PIN(UART_InvPin));  
  assert_param(IS_FUNCTIONAL_STATE(NewState)); 

  if (NewState != DISABLE)
  {
    /* Enable the active level inversion for selected pins by setting the TXINV 
       and/or RXINV bits in the UART CR2 register */
    UARTx->CR2 |= UART_InvPin;
  }
  else
  {
    /* Disable the active level inversion for selected requests by clearing the 
       TXINV and/or RXINV bits in the UART CR2 register */
    UARTx->CR2 &= (uint32_t)~UART_InvPin;
  }
}

/**
  * @}
  */





/** @defgroup UART_Group2 Data transfers functions
 *  @brief   Data transfers functions 
 *
@verbatim   
 ===============================================================================
                    ##### Data transfers functions #####
 ===============================================================================
    [..] This subsection provides a set of functions allowing to manage 
         the UART data transfers.
    [..] During an UART reception, data shifts in least significant bit first 
         through the RX pin. When a transmission is taking place, a write instruction to 
         the UART_TDR register stores the data in the shift register.
    [..] The read access of the UART_RDR register can be done using 
         the UART_ReceiveData() function and returns the RDR value.
         Whereas a write access to the UART_TDR can be done using UART_SendData()
         function and stores the written data into TDR.

@endverbatim
  * @{
  */

/**
  * @brief  Transmits single data through the UARTx peripheral.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  Data: the data to transmit.
  * @retval None
  */
void UART_SendData(UART_TypeDef* UARTx, uint16_t Data)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_DATA(Data)); 
    
  /* Transmit Data */
  UARTx->TDR = (Data & (uint16_t)0x01FF);
}

/**
  * @brief  Returns the most recent received data by the UARTx peripheral.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @retval The received data.
  */
uint16_t UART_ReceiveData(UART_TypeDef* UARTx)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  
  /* Receive Data */
  return (uint16_t)(UARTx->RDR & (uint16_t)0x01FF);
}

/**
  * @}
  */

/** @defgroup UART_Group3 MultiProcessor Communication functions
 *  @brief   Multi-Processor Communication functions 
 *
@verbatim   
 ===============================================================================
             ##### Multi-Processor Communication functions #####
 ===============================================================================
    [..] This subsection provides a set of functions allowing to manage the UART
         multiprocessor communication.
    [..] For instance one of the UARTs can be the master, its TX output is
         connected to the RX input of the other UART. The others are slaves,
         their respective TX outputs are logically ANDed together and connected 
         to the RX input of the master. UART multiprocessor communication is 
         possible through the following procedure:
         (#) Program the Baud rate, Word length = 9 bits, Stop bits, Parity, 
             Mode transmitter or Mode receiver and hardware flow control values 
             using the UART_Init() function.
         (#) Configures the UART address using the UART_SetAddress() function.
         (#) Configures the wake up methode (UART_WakeUp_IdleLine or 
             UART_WakeUp_AddressMark) using UART_WakeUpConfig() function only 
             for the slaves.
         (#) Enable the UART using the UART_Cmd() function.
         (#) Enter the UART slaves in mute mode using UART_ReceiverWakeUpCmd() 
             function.
    [..] The UART Slave exit from mute mode when receive the wake up condition.

@endverbatim
  * @{
  */

/**
  * @brief  Sets the address of the UART node.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_Address: Indicates the address of the UART node.
  * @retval None
  */
void UART_SetAddress(UART_TypeDef* UARTx, uint8_t UART_Address)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  
  /* Clear the UART address */
  UARTx->CR2 &= (uint32_t)~((uint32_t)UART_CR2_ADD);
  /* Set the UART address node */
  UARTx->CR2 |=((uint32_t)UART_Address << (uint32_t)0x18);
}

/**
  * @brief  Enables or disables the UART's mute mode.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  NewState: new state of the UART mute mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void UART_MuteModeCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_FUNCTIONAL_STATE(NewState)); 
  
  if (NewState != DISABLE)
  {
    /* Enable the UART mute mode by setting the MME bit in the CR1 register */
    UARTx->CR1 |= UART_CR1_MME;
  }
  else
  {
    /* Disable the UART mute mode by clearing the MME bit in the CR1 register */
    UARTx->CR1 &= (uint32_t)~((uint32_t)UART_CR1_MME);
  }
}

/**
  * @brief  Selects the UART WakeUp method from mute mode.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.   
  * @param  UART_WakeUp: specifies the UART wakeup method.
  *          This parameter can be one of the following values:
  *            @arg UART_WakeUp_IdleLine: WakeUp by an idle line detection
  *            @arg UART_WakeUp_AddressMark: WakeUp by an address mark
  * @retval None
  */
void UART_MuteModeWakeUpConfig(UART_TypeDef* UARTx, uint32_t UART_WakeUp)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_MUTEMODE_WAKEUP(UART_WakeUp));

  UARTx->CR1 &= (uint32_t)~((uint32_t)UART_CR1_WAKE);
  UARTx->CR1 |= UART_WakeUp;
}

/**
  * @brief  Configure the the UART Address detection length.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_AddressLength: specifies the UART address length detection.
  *          This parameter can be one of the following values:
  *            @arg UART_AddressLength_4b: 4-bit address length detection 
  *            @arg UART_AddressLength_7b: 7-bit address length detection 
  * @retval None
  */
void UART_AddressDetectionConfig(UART_TypeDef* UARTx, uint32_t UART_AddressLength)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_ADDRESS_DETECTION(UART_AddressLength));

  UARTx->CR2 &= (uint32_t)~((uint32_t)UART_CR2_ADDM7);
  UARTx->CR2 |= UART_AddressLength;
}

/**
  * @}
  */


/** @defgroup UART_Group4 Halfduplex mode function
 *  @brief   Half-duplex mode function 
 *
@verbatim   
 ===============================================================================
                   ##### Half-duplex mode function #####
 ===============================================================================
    [..] This subsection provides a set of functions allowing to manage the UART
         Half-duplex communication.
    [..] The UART can be configured to follow a single-wire half-duplex protocol 
         where the TX and RX lines are internally connected.
    [..] UART Half duplex communication is possible through the following procedure:
         (#) Program the Baud rate, Word length, Stop bits, Parity, Mode transmitter 
             or Mode receiver and hardware flow control values using the UART_Init()
            function.
         (#) Configures the UART address using the UART_SetAddress() function.
         (#) Enable the half duplex mode using UART_HalfDuplexCmd() function.
         (#) Enable the UART using the UART_Cmd() function.
         -@- The RX pin is no longer used.
         -@- In Half-duplex mode the following bits must be kept cleared:
             (+@) LINEN and CLKEN bits in the UART_CR2 register.
             (+@) SCEN and IREN bits in the UART_CR3 register.

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the UART's Half Duplex communication.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.   
  * @param  NewState: new state of the UART Communication.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void UART_HalfDuplexCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Enable the Half-Duplex mode by setting the HDSEL bit in the CR3 register */
    UARTx->CR3 |= UART_CR3_HDSEL;
  }
  else
  {
    /* Disable the Half-Duplex mode by clearing the HDSEL bit in the CR3 register */
    UARTx->CR3 &= (uint32_t)~((uint32_t)UART_CR3_HDSEL);
  }
}

/**
  * @}
  */

/** @defgroup UART_Group5 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions 
 *
@verbatim   
 ===============================================================================
            ##### Interrupts and flags management functions #####
 ===============================================================================
    [..] This subsection provides a set of functions allowing to configure the 
         UART Interrupts sources, Requests and check or clear the flags or pending bits status. 
         The user should identify which mode will be used in his application to 
         manage the communication: Polling mode, Interrupt mode.

 *** Polling Mode ***
 ====================
    [..] In Polling Mode, the SPI communication can be managed by these flags:
         (#) UART_FLAG_REACK: to indicate the status of the Receive Enable 
             acknowledge flag
         (#) UART_FLAG_TEACK: to indicate the status of the Transmit Enable 
             acknowledge flag.
         (#) UART_FLAG_RWU: to indicate the status of the Receive Wake up flag.
         (#) UART_FLAG_SBK: to indicate the status of the Send Break flag.
         (#) UART_FLAG_CM: to indicate the status of the Character match flag.
         (#) UART_FLAG_BUSY: to indicate the status of the Busy flag.
         (#) UART_FLAG_nCTSS: to indicate the status of the Inverted nCTS input 
             bit status.
         (#) UART_FLAG_TXE: to indicate the status of the transmit buffer register.
         (#) UART_FLAG_RXNE: to indicate the status of the receive buffer register.
         (#) UART_FLAG_TC: to indicate the status of the transmit operation.
         (#) UART_FLAG_IDLE: to indicate the status of the Idle Line.
         (#) UART_FLAG_CTS: to indicate the status of the nCTS input.
         (#) UART_FLAG_NE: to indicate if a noise error occur.
         (#) UART_FLAG_FE: to indicate if a frame error occur.
         (#) UART_FLAG_PE: to indicate if a parity error occur.
         (#) UART_FLAG_ORE: to indicate if an Overrun error occur.
    [..] In this Mode it is advised to use the following functions:
         (+) FlagStatus UART_GetFlagStatus(UART_TypeDef* UARTx, uint16_t UART_FLAG).
         (+) void UART_ClearFlag(UART_TypeDef* UARTx, uint16_t UART_FLAG).

 *** Interrupt Mode ***
 ======================
    [..] In Interrupt Mode, the UART communication can be managed by 8 interrupt 
         sources and 10 pending bits:
         (+) Pending Bits:
             (##) UART_IT_WU: to indicate the status of the Wake up interrupt.
             (##) UART_IT_CM: to indicate the status of Character match interrupt.
             (##) UART_IT_TC: to indicate the status of Transmission complete interrupt.
             (##) UART_IT_IDLE: to indicate the status of IDLE line detected interrupt.
             (##) UART_IT_ORE: to indicate the status of OverRun Error interrupt.
             (##) UART_IT_NE: to indicate the status of Noise Error interrupt.
             (##) UART_IT_FE: to indicate the status of Framing Error interrupt.
             (##) UART_IT_PE: to indicate the status of Parity Error interrupt.  

         (+) Interrupt Source:
             (##) UART_IT_WU: specifies the interrupt source for Wake up interrupt.
             (##) UART_IT_CM: specifies the interrupt source for Character match 
                  interrupt.
             (##) UART_IT_TXE: specifies the interrupt source for Tansmit Data 
                  Register empty interrupt.
             (##) UART_IT_TC: specifies the interrupt source for Transmission 
                  complete interrupt.
             (##) UART_IT_RXNE: specifies the interrupt source for Receive Data 
                  register not empty interrupt.
             (##) UART_IT_IDLE: specifies the interrupt source for Idle line 
                  detection interrupt.
             (##) UART_IT_PE: specifies the interrupt source for Parity Error interrupt.
             (##) UART_IT_ERR: specifies the interrupt source for Error interrupt
                  (Frame error, noise error, overrun error)
             -@@- Some parameters are coded in order to use them as interrupt 
                 source or as pending bits.
    [..] In this Mode it is advised to use the following functions:
         (+) void UART_ITConfig(UART_TypeDef* UARTx, uint16_t UART_IT, FunctionalState NewState).
         (+) ITStatus UART_GetITStatus(UART_TypeDef* UARTx, uint16_t UART_IT).
         (+) void UART_ClearITPendingBit(UART_TypeDef* UARTx, uint16_t UART_IT).

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified UART interrupts.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_IT: specifies the UART interrupt sources to be enabled or disabled.
  *          This parameter can be one of the following values:
  *            @arg UART_IT_TXE:  Tansmit Data Register empty interrupt.
  *            @arg UART_IT_TC:  Transmission complete interrupt.
  *            @arg UART_IT_RXNE:  Receive Data register not empty interrupt.
  *            @arg UART_IT_IDLE:  Idle line detection interrupt.
  *            @arg UART_IT_PE:  Parity Error interrupt.
  *            @arg UART_IT_ERR:  Error interrupt(Frame error, noise error, overrun error)
  * @param  NewState: new state of the specified UARTx interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void UART_ITConfig(UART_TypeDef* UARTx, uint32_t UART_IT, FunctionalState NewState)
{
  uint32_t uartreg = 0, itpos = 0, itmask = 0;
  uint32_t uartxbase = 0;
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_CONFIG_IT(UART_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  uartxbase = (uint32_t)UARTx;
  
  /* Get the UART register index */
  uartreg = (((uint16_t)UART_IT) >> 0x08);
  
  /* Get the interrupt position */
  itpos = UART_IT & IT_MASK;
  itmask = (((uint32_t)0x01) << itpos);
  
  if (uartreg == 0x02) /* The IT is in CR2 register */
  {
    uartxbase += 0x04;
  }
  else if (uartreg == 0x03) /* The IT is in CR3 register */
  {
    uartxbase += 0x08;
  }
  else /* The IT is in CR1 register */
  {
  }
  if (NewState != DISABLE)
  {
    *(__IO uint32_t*)uartxbase  |= itmask;
  }
  else
  {
    *(__IO uint32_t*)uartxbase &= ~itmask;
  }
}

/**
  * @brief  Enables the specified UART's Request.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_Request: specifies the UART request.
  *          This parameter can be any combination of the following values:
  *            @arg UART_Request_RXFRQ: Receive data flush ReQuest
  *            @arg UART_Request_MMRQ: Mute Mode ReQuest
  *            @arg UART_Request_SBKRQ: Send Break ReQuest
  * @param  NewState: new state of the DMA interface when reception error occurs.
  *          This parameter can be: ENABLE or DISABLE.  
  * @retval None
  */
void UART_RequestCmd(UART_TypeDef* UARTx, uint32_t UART_Request, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_REQUEST(UART_Request));
  assert_param(IS_FUNCTIONAL_STATE(NewState)); 

  if (NewState != DISABLE)
  {
    /* Enable the UART ReQuest by setting the dedicated request bit in the RQR
       register.*/
      UARTx->RQR |= UART_Request;
  }
  else
  {
    /* Disable the UART ReQuest by clearing the dedicated request bit in the RQR
       register.*/
    UARTx->RQR &= (uint32_t)~UART_Request;
  }
}

/**
  * @brief  Enables or disables the UART's Overrun detection.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_OVRDetection: specifies the OVR detection status in case of OVR error.
  *          This parameter can be any combination of the following values:
  *            @arg UART_OVRDetection_Enable: OVR error detection enabled when
  *                                            the UART OVR error is asserted.
  *            @arg UART_OVRDetection_Disable: OVR error detection disabled when
  *                                             the UART OVR error is asserted.
  * @retval None
  */
void UART_OverrunDetectionConfig(UART_TypeDef* UARTx, uint32_t UART_OVRDetection)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_OVRDETECTION(UART_OVRDetection));
  
  /* Clear the OVR detection bit */
  UARTx->CR3 &= (uint32_t)~((uint32_t)UART_CR3_OVERDIS);
  /* Set the new value for the OVR detection bit */
  UARTx->CR3 |= UART_OVRDetection;
}

/**
  * @brief  Checks whether the specified UART flag is set or not.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            @arg UART_FLAG_REACK:  Receive Enable acknowledge flag.
  *            @arg UART_FLAG_TEACK:  Transmit Enable acknowledge flag.
  *            @arg UART_FLAG_SBK:  Send Break flag.
  *            @arg UART_FLAG_BUSY:  Busy flag.
  *            @arg UART_FLAG_TXE:  Transmit data register empty flag.
  *            @arg UART_FLAG_TC:  Transmission Complete flag.
  *            @arg UART_FLAG_RXNE:  Receive data register not empty flag.
  *            @arg UART_FLAG_IDLE:  Idle Line detection flag.
  *            @arg UART_FLAG_ORE:  OverRun Error flag.
  *            @arg UART_FLAG_NE:  Noise Error flag.
  *            @arg UART_FLAG_FE:  Framing Error flag.
  *            @arg UART_FLAG_PE:  Parity Error flag.
  * @retval The new state of UART_FLAG (SET or RESET).
  */
FlagStatus UART_GetFlagStatus(UART_TypeDef* UARTx, uint32_t UART_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_FLAG(UART_FLAG));
  
  if ((UARTx->ISR & UART_FLAG) != (uint16_t)RESET)
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
  * @brief  Clears the UARTx's pending flags.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_FLAG: specifies the flag to clear.
  *          This parameter can be any combination of the following values:
  *            @arg UART_FLAG_TC:  Transmission Complete flag.
  *            @arg UART_FLAG_IDLE:  IDLE line detected flag.
  *            @arg UART_FLAG_ORE:  OverRun Error flag.
  *            @arg UART_FLAG_NE: Noise Error flag.
  *            @arg UART_FLAG_FE: Framing Error flag.
  *            @arg UART_FLAG_PE:   Parity Errorflag.
  *   
  * @note     RXNE pending bit is cleared by a read to the UART_RDR register 
  *           (UART_ReceiveData()) or by writing 1 to the RXFRQ in the register
  *           UART_RQR (UART_RequestCmd()).
  * @note     TC flag can be also cleared by software sequence: a read operation
  *           to UART_SR register (UART_GetFlagStatus()) followed by a write 
  *           operation to UART_TDR register (UART_SendData()).
  * @note     TXE flag is cleared by a write to the UART_TDR register (UART_SendData())
  *           or by writing 1 to the TXFRQ in the register UART_RQR (UART_RequestCmd()).
  * @note     SBKF flag is cleared by 1 to the SBKRQ in the register UART_RQR
  *           (UART_RequestCmd()).
  * @retval None
  */
void UART_ClearFlag(UART_TypeDef* UARTx, uint32_t UART_FLAG)
{
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_CLEAR_FLAG(UART_FLAG));
     
  UARTx->ICR = UART_FLAG;
}

/**
  * @brief  Checks whether the specified UART interrupt has occurred or not.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_IT: specifies the UART interrupt source to check.
  *          This parameter can be one of the following values:
  *            @arg UART_IT_TXE:  Tansmit Data Register empty interrupt.
  *            @arg UART_IT_TC:  Transmission complete interrupt.
  *            @arg UART_IT_RXNE:  Receive Data register not empty interrupt.
  *            @arg UART_IT_IDLE:  Idle line detection interrupt.
  *            @arg UART_IT_ORE:  OverRun Error interrupt.
  *            @arg UART_IT_NE:  Noise Error interrupt.
  *            @arg UART_IT_FE:  Framing Error interrupt.
  *            @arg UART_IT_PE:  Parity Error interrupt.
  * @retval The new state of UART_IT (SET or RESET).
  */
ITStatus UART_GetITStatus(UART_TypeDef* UARTx, uint32_t UART_IT)
{
  uint32_t bitpos = 0, itmask = 0, uartreg = 0;
  ITStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_GET_IT(UART_IT)); 
  
  /* Get the UART register index */
  uartreg = (((uint16_t)UART_IT) >> 0x08);
  /* Get the interrupt position */
  itmask = UART_IT & IT_MASK;
  itmask = (uint32_t)0x01 << itmask;
  
  if (uartreg == 0x01) /* The IT  is in CR1 register */
  {
    itmask &= UARTx->CR1;
  }
  else if (uartreg == 0x02) /* The IT  is in CR2 register */
  {
    itmask &= UARTx->CR2;
  }
  else /* The IT  is in CR3 register */
  {
    itmask &= UARTx->CR3;
  }
  
  bitpos = UART_IT >> 0x10;
  bitpos = (uint32_t)0x01 << bitpos;
  bitpos &= UARTx->ISR;
  if ((itmask != (uint16_t)RESET)&&(bitpos != (uint16_t)RESET))
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
  * @brief  Clears the UARTx's interrupt pending bits.
  * @param  UARTx: Select the UART or the UART peripheral.  
	*   This parameter can be one of the following values: 
  *      UART1, UART2.  
  * @param  UART_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be one of the following values:
  *            @arg UART_IT_TC:  Transmission complete interrupt.
  *            @arg UART_IT_IDLE:  IDLE line detected interrupt.
  *            @arg UART_IT_ORE:  OverRun Error interrupt.
  *            @arg UART_IT_NE:  Noise Error interrupt.
  *            @arg UART_IT_FE:  Framing Error interrupt.
  *            @arg UART_IT_PE:  Parity Error interrupt.
  *
  * @note     RXNE pending bit is cleared by a read to the UART_RDR register 
  *           (UART_ReceiveData()) or by writing 1 to the RXFRQ in the register 
  *           UART_RQR (UART_RequestCmd()).
  * @note     TC pending bit can be also cleared by software sequence: a read 
  *           operation to UART_SR register (UART_GetITStatus()) followed by  
  *           a write operation to UART_TDR register (UART_SendData()).
  * @note     TXE pending bit is cleared by a write to the UART_TDR register 
  *           (UART_SendData()) or by writing 1 to the TXFRQ in the register 
  *           UART_RQR (UART_RequestCmd()).
  * @retval None
  */
void UART_ClearITPendingBit(UART_TypeDef* UARTx, uint32_t UART_IT)
{
  uint32_t bitpos = 0, itmask = 0;
  /* Check the parameters */
  assert_param(IS_UART_ALL_PERIPH(UARTx));
  assert_param(IS_UART_CLEAR_IT(UART_IT)); 
  
  bitpos = UART_IT >> 0x10;
  itmask = ((uint32_t)0x01 << (uint32_t)bitpos);
  UARTx->ICR = (uint32_t)itmask;
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

