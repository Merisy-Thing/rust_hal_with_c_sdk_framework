/********************************************************************************
  * @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
  * @file     hk32f0301mxxc_spi.h 
  * @version V1.0.0
  * @date    2022-09-07
  * @log  Initial release
 ===============================================================================
                       ##### How to use this driver #####
 ===============================================================================
    [..]
        (#) Enable peripheral clock using RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI1, ENABLE)
            function for SPI1.
  
        (#) Enable SCK, MOSI, MISO and NSS GPIO clocks using 
            RCC_AHBPeriphClockCmd() function. 
  
        (#) Peripherals alternate function: 
            (++) Connect the pin to the desired peripherals' Alternate 
                 Function (AF) using GPIO_PinAFConfig() function.
            (++) Configure the desired pin in alternate function by:
                 GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF.
            (++) Select the type, pull-up/pull-down and output speed via 
                 GPIO_PuPd, GPIO_OType and GPIO_Speed members.
            (++) Call GPIO_Init() function.
  
        (#) Program the Polarity, Phase, First Data, Baud Rate Prescaler, Slave 
            Management, Peripheral Mode and CRC Polynomial values using the SPI_Init()
            function.
  
        (#) Configure the FIFO threshold using SPI_RxFIFOThresholdConfig() to select 
            at which threshold the RXNE event is generated.
            
        (#) Enable the NVIC and the corresponding interrupt using the function 
            SPI_ITConfig() if you need to use interrupt mode. 
   
        (#) Enable the SPI using the SPI_Cmd() function.
  
        (#) Optionally, you can enable/configure the following parameters without
            re-initialization (i.e there is no need to call again SPI_Init() function):
            (++) When bidirectional mode (SPI_Direction_1Line_Rx or SPI_Direction_1Line_Tx)
                 is programmed as Data direction parameter using the SPI_Init() 
                 function it can be possible to switch between SPI_Direction_Tx 
                 or SPI_Direction_Rx using the SPI_BiDirectionalLineConfig() function.
            (++) When SPI_NSS_Soft is selected as Slave Select Management parameter 
                 using the SPI_Init() function it can be possible to manage the 
                 NSS internal signal using the SPI_NSSInternalSoftwareConfig() function.
            (++) Reconfigure the data size using the SPI_DataSizeConfig() function.
            (++) Enable or disable the SS output using the SPI_SSOutputCmd() function.  
  
        (#) To use the CRC Hardware calculation feature refer to the Peripheral 
            CRC hardware Calculation subsection.
  *******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_spi.h"
#include "hk32f0301mxxc_rcc.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver 
  * @{
  */

/** @defgroup SPI SPI
  * @brief SPI driver modules
  * @{
  */

/** @defgroup SPI_Private_Defines SPI Private Defines
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* SPI registers Masks */
#define SPI_CR1_CLEAR_MASK       ((uint16_t)0x3040)
#define SPI_CR1_CLEAR_MASK2      ((uint16_t)0xFFFB)

/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup SPI_Private_Functions SPI Private Functions
  * @{
  */

/** @defgroup SPI_Group1 Initialization and Configuration functions
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
           ##### Initialization and Configuration functions #####
 ===============================================================================
    [..] This section provides a set of functions allowing to initialize the SPI Direction,
         SPI Mode, SPI Data Size, SPI Polarity, SPI Phase, SPI NSS Management, SPI Baud
         Rate Prescaler, SPI First Bit and SPI CRC Polynomial.

    [..] The SPI_Init() function follows the SPI configuration procedures for Master mode
         and Slave mode (details for these procedures are available in reference manual).
         
    [..] When the Software NSS management (SPI_InitStruct->SPI_NSS = SPI_NSS_Soft) is selected,
         use the following function to manage the NSS bit:
         void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft);

    [..] In Master mode, when the Hardware NSS management (SPI_InitStruct->SPI_NSS = SPI_NSS_Hard)
         is selected, use the follwoing function to enable the NSS output feature.
         void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState);

    [..] The NSS pulse mode can be managed by the SPI TI mode when enabling it using the following function:
         void SPI_TIModeCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
         And it can be managed by software in the SPI Motorola mode using this function: 
         void SPI_NSSPulseModeCmd(SPI_TypeDef* SPIx, FunctionalState NewState);
  
@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the SPI1 peripheral registers to their default
  *         reset values.
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.           
  * @retval None
  */
void SPI_DeInit(SPI_TypeDef* SPIx)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));

    /* Enable SPI1 reset state */
    RCC_APBPeriph2ResetCmd(RCC_APBPeriph2_SPI, ENABLE);
    /* Release SPI1 from reset state */
    RCC_APBPeriph2ResetCmd(RCC_APBPeriph2_SPI, DISABLE);
  
}

/**
  * @brief  Fills each SPI_InitStruct member with its default value.
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure which will be initialized.
  * @retval None
  */
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct)
{
/*--------------- Reset SPI init structure parameters values -----------------*/
  /* Initialize the SPI_Direction member */
  SPI_InitStruct->SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  /* Initialize the SPI_Mode member */
  SPI_InitStruct->SPI_Mode = SPI_Mode_Slave;
  /* Initialize the SPI_DataSize member */
  SPI_InitStruct->SPI_DataSize = SPI_DataSize_8b;
  /* Initialize the SPI_CPOL member */
  SPI_InitStruct->SPI_CPOL = SPI_CPOL_Low;
  /* Initialize the SPI_CPHA member */
  SPI_InitStruct->SPI_CPHA = SPI_CPHA_1Edge;
  /* Initialize the SPI_NSS member */
  SPI_InitStruct->SPI_NSS = SPI_NSS_Hard;
  /* Initialize the SPI_BaudRatePrescaler member */
  SPI_InitStruct->SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  /* Initialize the SPI_FirstBit member */
  SPI_InitStruct->SPI_FirstBit = SPI_FirstBit_MSB;
  /* Initialize the SPI_CRCPolynomial member */
  SPI_InitStruct->SPI_CRCPolynomial = 7;
}

/**
  * @brief  Initializes the SPIx peripheral according to the specified 
  *         parameters in the SPI_InitStruct.
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.  
  * @param  SPI_InitStruct: pointer to a SPI_InitTypeDef structure that
  *         contains the configuration information for the specified SPI peripheral.
  * @retval None
  */
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct)
{
  uint16_t tmpreg = 0;

  /* check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));

  /* Check the SPI parameters */
  assert_param(IS_SPI_DIRECTION_MODE(SPI_InitStruct->SPI_Direction));
  assert_param(IS_SPI_MODE(SPI_InitStruct->SPI_Mode));
  assert_param(IS_SPI_DATA_SIZE(SPI_InitStruct->SPI_DataSize));
  assert_param(IS_SPI_CPOL(SPI_InitStruct->SPI_CPOL));
  assert_param(IS_SPI_CPHA(SPI_InitStruct->SPI_CPHA));
  assert_param(IS_SPI_NSS(SPI_InitStruct->SPI_NSS));
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_InitStruct->SPI_BaudRatePrescaler));
  assert_param(IS_SPI_FIRST_BIT(SPI_InitStruct->SPI_FirstBit));
  assert_param(IS_SPI_CRC_POLYNOMIAL(SPI_InitStruct->SPI_CRCPolynomial));

  /*---------------------------- SPIx CR1 Configuration ------------------------*/
  /* Get the SPIx CR1 value */
  tmpreg = SPIx->CR1;
  /* Clear BIDIMode, BIDIOE, RxONLY, SSM, SSI, LSBFirst, BR, CPOL and CPHA bits */
  tmpreg &= SPI_CR1_CLEAR_MASK;
  /* Configure SPIx: direction, NSS management, first transmitted bit, BaudRate prescaler
  master/slave mode, CPOL and CPHA */
  /* Set BIDImode, BIDIOE and RxONLY bits according to SPI_Direction value */
  /* Set SSM, SSI bit according to SPI_NSS values */
  /* Set LSBFirst bit according to SPI_FirstBit value */
  /* Set BR bits according to SPI_BaudRatePrescaler value */
  /* Set CPOL bit according to SPI_CPOL value */
  /* Set CPHA bit according to SPI_CPHA value */
  tmpreg |= (uint16_t)((uint32_t)SPI_InitStruct->SPI_Direction | SPI_InitStruct->SPI_FirstBit |
                      SPI_InitStruct->SPI_CPOL | SPI_InitStruct->SPI_CPHA |
                      SPI_InitStruct->SPI_NSS | SPI_InitStruct->SPI_BaudRatePrescaler);  
  /* Write to SPIx CR1 */
  SPIx->CR1 = tmpreg;
  /*-------------------------Data Size Configuration -----------------------*/
  /* Get the SPIx CR2 value */
  tmpreg = SPIx->CR2;
  /* Clear DS[3:0] bits */
  tmpreg &=(uint16_t)~SPI_CR2_DS;
  /* Configure SPIx: Data Size */
  tmpreg |= (uint16_t)(SPI_InitStruct->SPI_DataSize);
  /* Write to SPIx CR2 */
  SPIx->CR2 = tmpreg;
  
  /*---------------------------- SPIx CRCPOLY Configuration --------------------*/
  /* Write to SPIx CRCPOLY */
  SPIx->CRCPR = SPI_InitStruct->SPI_CRCPolynomial;
  
  /*---------------------------- SPIx CR1 Configuration ------------------------*/
  /* Get the SPIx CR1 value */
  tmpreg = SPIx->CR1;
  /* Clear MSTR bit */
  tmpreg &= SPI_CR1_CLEAR_MASK2;
  /* Configure SPIx: master/slave mode */  
  /* Set MSTR bit according to SPI_Mode */
  tmpreg |= (uint16_t)((uint32_t)SPI_InitStruct->SPI_Mode);  
  /* Write to SPIx CR1 */
  SPIx->CR1 = tmpreg;  
  
}



/**
  * @brief  Enables or disables the specified SPI peripheral.
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.

  * @param  NewState: new state of the SPIx peripheral. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected SPI peripheral */
    SPIx->CR1 |= SPI_CR1_SPE;
  }
  else
  {
    /* Disable the selected SPI peripheral */
    SPIx->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE);
  }
}

/**
  * @brief  Enables or disables the TI Mode.
  *   
  * @note   This function can be called only after the SPI_Init() function has 
  *         been called. 
  * @note   When TI mode is selected, the control bits SSM, SSI, CPOL and CPHA 
  *         are not taken into consideration and are configured by hardware 
  *         respectively to the TI mode requirements.
  *    
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  *
  * @param  NewState: new state of the selected SPI TI communication mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_TIModeCmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the TI mode for the selected SPI peripheral */
    SPIx->CR2 |= SPI_CR2_FRF;
  }
  else
  {
    /* Disable the TI mode for the selected SPI peripheral */
    SPIx->CR2 &= (uint16_t)~((uint16_t)SPI_CR2_FRF);
  }
}


/**
  * @brief  Configures the data size for the selected SPI.
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  *  
  * @param  SPI_DataSize: specifies the SPI data size.
  *         For the SPIx peripheral this parameter can be one of the following values:
  *            @arg SPI_DataSize_4b: Set data size to 4 bits
  *            @arg SPI_DataSize_5b: Set data size to 5 bits
  *            @arg SPI_DataSize_6b: Set data size to 6 bits
  *            @arg SPI_DataSize_7b: Set data size to 7 bits
  *            @arg SPI_DataSize_8b: Set data size to 8 bits
  *            @arg SPI_DataSize_9b: Set data size to 9 bits
  *            @arg SPI_DataSize_10b: Set data size to 10 bits
  *            @arg SPI_DataSize_11b: Set data size to 11 bits
  *            @arg SPI_DataSize_12b: Set data size to 12 bits
  *            @arg SPI_DataSize_13b: Set data size to 13 bits
  *            @arg SPI_DataSize_14b: Set data size to 14 bits
  *            @arg SPI_DataSize_15b: Set data size to 15 bits
  *            @arg SPI_DataSize_16b: Set data size to 16 bits
  * @retval None
  */
void SPI_DataSizeConfig(SPI_TypeDef* SPIx, uint16_t SPI_DataSize)
{
  uint16_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_DATA_SIZE(SPI_DataSize));
  /* Read the CR2 register */
  tmpreg = SPIx->CR2;
  /* Clear DS[3:0] bits */
  tmpreg &= (uint16_t)~SPI_CR2_DS;
  /* Set new DS[3:0] bits value */
  tmpreg |= SPI_DataSize;
  SPIx->CR2 = tmpreg;
}

/**
  * @brief  Configures the FIFO reception threshold for the selected SPI.
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  *  
  * @param  SPI_RxFIFOThreshold: specifies the FIFO reception threshold.
  *          This parameter can be one of the following values:
  *            @arg SPI_RxFIFOThreshold_HF: RXNE event is generated if the FIFO 
  *                                         level is greater or equal to 1/2. 
  *            @arg SPI_RxFIFOThreshold_QF: RXNE event is generated if the FIFO 
  *                                         level is greater or equal to 1/4. 
  * @retval None
  */
void SPI_RxFIFOThresholdConfig(SPI_TypeDef* SPIx, uint16_t SPI_RxFIFOThreshold)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_RX_FIFO_THRESHOLD(SPI_RxFIFOThreshold));

  /* Clear FRXTH bit */
  SPIx->CR2 &= (uint16_t)~((uint16_t)SPI_CR2_FRXTH);

  /* Set new FRXTH bit value */
  SPIx->CR2 |= SPI_RxFIFOThreshold;
}

/**
  * @brief  Selects the data transfer direction in bidirectional mode for the specified SPI.
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  * @note   
  * @param  SPI_Direction: specifies the data transfer direction in bidirectional mode. 
  *          This parameter can be one of the following values:
  *            @arg SPI_Direction_Tx: Selects Tx transmission direction
  *            @arg SPI_Direction_Rx: Selects Rx receive direction
  * @retval None
  */
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, uint16_t SPI_Direction)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_DIRECTION(SPI_Direction));
  if (SPI_Direction == SPI_Direction_Tx)
  {
    /* Set the Tx only mode */
    SPIx->CR1 |= SPI_Direction_Tx;
  }
  else
  {
    /* Set the Rx only mode */
    SPIx->CR1 &= SPI_Direction_Rx;
  }
}

/**
  * @brief  Configures internally by software the NSS pin for the selected SPI.
  * @note   This function can be called only after the SPI_Init() function has 
  *         been called.  
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  * @note 
  * @param  SPI_NSSInternalSoft: specifies the SPI NSS internal state.
  *          This parameter can be one of the following values:
  *            @arg SPI_NSSInternalSoft_Set: Set NSS pin internally
  *            @arg SPI_NSSInternalSoft_Reset: Reset NSS pin internally
  * @retval None
  */
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_NSS_INTERNAL(SPI_NSSInternalSoft));

  if (SPI_NSSInternalSoft != SPI_NSSInternalSoft_Reset)
  {
    /* Set NSS pin internally by software */
    SPIx->CR1 |= SPI_NSSInternalSoft_Set;
  }
  else
  {
    /* Reset NSS pin internally by software */
    SPIx->CR1 &= SPI_NSSInternalSoft_Reset;
  }
}

/**
  * @brief  Enables or disables the SS output for the selected SPI.
  * @note   This function can be called only after the SPI_Init() function has 
  *         been called and the NSS hardware management mode is selected. 
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  * @param  NewState: new state of the SPIx SS output. 
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  if (NewState != DISABLE)
  {
    /* Enable the selected SPI SS output */
    SPIx->CR2 |= SPI_CR2_SSOE;
  }
  else
  {
    /* Disable the selected SPI SS output */
    SPIx->CR2 &= (uint16_t)~((uint16_t)SPI_CR2_SSOE);
  }
}

/**
  * @brief  Enables or disables the NSS pulse management mode.
  * @note   This function can be called only after the SPI_Init() function has 
  *         been called. 
  * @note   When TI mode is selected, the control bits NSSP is not taken into 
  *         consideration and are configured by hardware respectively to the 
  *         TI mode requirements. 
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  * @param  NewState: new state of the NSS pulse management mode.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_NSSPulseModeCmd(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the NSS pulse management mode */
    SPIx->CR2 |= SPI_CR2_NSSP;
  }
  else
  {
    /* Disable the NSS pulse management mode */
    SPIx->CR2 &= (uint16_t)~((uint16_t)SPI_CR2_NSSP);    
  }
}

/**
  * @}
  */

/** @defgroup SPI_Group2 Data transfers functions
 *  @brief   Data transfers functions
 *
@verbatim
 ===============================================================================
                    ##### Data transfers functions #####
 ===============================================================================
    [..] This section provides a set of functions allowing to manage the SPI 
         data transfers.

    [..] In reception, data are received and then stored into an internal Rx buffer while 
         In transmission, data are first stored into an internal Tx buffer before being 
         transmitted.

    [..] The read access of the SPI_DR register can be done using 
         SPI_ReceiveData8() (when data size is equal or inferior than 8bits) and.
         SPI_ReceiveData16() (when data size is superior than 8bits)function
         and returns the Rx buffered value. Whereas a write access to the SPI_DR 
         can be done using SPI_SendData8() (when data size is equal or inferior than 8bits)
         and SPI_SendData16() (when data size is superior than 8bits) function 
         and stores the written data into Tx buffer.

@endverbatim
  * @{
  */

/**
  * @brief  Transmits a Data through the SPIx peripheral.
  * @param  SPIx: where x can be 1 o to select the SPI1 peripheral.
  * @param  Data: Data to be transmitted.
  * @retval None
  */
void SPI_SendData8(SPI_TypeDef* SPIx, uint8_t Data)
{
  uint32_t spixbase = 0x00;

  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));

  spixbase = (uint32_t)SPIx; 
  spixbase += 0x0C;
  
  *(__IO uint8_t *) spixbase = Data;
}

/**
  * @brief  Transmits a Data through the SPIx peripheral.
  * @param  SPIx: where x can be 1 in SPI mode to select 
  *         the SPI peripheral. 
  * @param  Data: Data to be transmitted.
  * @retval None
  */
void SPI_SendData16(SPI_TypeDef* SPIx, uint16_t Data)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  
  SPIx->DR = (uint16_t)Data;
}

/**
  * @brief  Returns the most recent received data by the SPIx peripheral. 
  * @param  SPIx: where x can be 1 in SPI mode to select the SPI peripheral.   
  * @retval The value of the received data.
  */
uint8_t SPI_ReceiveData8(SPI_TypeDef* SPIx)
{
  uint32_t spixbase = 0x00;
  
  spixbase = (uint32_t)SPIx; 
  spixbase += 0x0C;
  
  return *(__IO uint8_t *) spixbase;
}

/**
  * @brief  Returns the most recent received data by the SPIx peripheral. 
  * @param  SPIx: where x can be 1 in SPI mode to select the SPI peripheral.  
  *         the SPI peripheral.  
  * @retval The value of the received data.
  */
uint16_t SPI_ReceiveData16(SPI_TypeDef* SPIx)
{
  return SPIx->DR;
}
/**
  * @}
  */

/** @defgroup SPI_Group3 Hardware CRC Calculation functions
 *  @brief   Hardware CRC Calculation functions
 *
@verbatim   
 ===============================================================================
                ##### Hardware CRC Calculation functions #####
 ===============================================================================
    [..] This section provides a set of functions allowing to manage the SPI CRC hardware 
         calculation.SPI communication using CRC is possible through the following procedure:

         (#) Program the Data direction, Polarity, Phase, First Data, Baud Rate Prescaler,
             Slave Management, Peripheral Mode and CRC Polynomial values using the SPI_Init()
             function.
         (#) Enable the CRC calculation using the SPI_CalculateCRC() function.
         (#) Enable the SPI using the SPI_Cmd() function
         (#) Before writing the last data to the TX buffer, set the CRCNext bit using the 
             SPI_TransmitCRC() function to indicate that after transmission of the last 
             data, the CRC should be transmitted.
         (#) After transmitting the last data, the SPI transmits the CRC. The SPI_CR1_CRCNEXT
             bit is reset. The CRC is also received and compared against the SPI_RXCRCR 
             value. 
             If the value does not match, the SPI_FLAG_CRCERR flag is set and an interrupt
             can be generated when the SPI_IT_ERR interrupt is enabled.

    -@-
       (+@) It is advised to don't read the calculate CRC values during the communication.
       (+@) When the SPI is in slave mode, be careful to enable CRC calculation only
       when the clock is stable, that is, when the clock is in the steady state. 
       If not, a wrong CRC calculation may be done. In fact, the CRC is sensitive 
       to the SCK slave input clock as soon as CRCEN is set, and this, whatever 
       the value of the SPE bit.
       (+@) With high bitrate frequencies, be careful when transmitting the CRC.
       As the number of used CPU cycles has to be as low as possible in the CRC 
       transfer phase, it is forbidden to call software functions in the CRC 
       transmission sequence to avoid errors in the last data and CRC reception. 
       In fact, CRCNEXT bit has to be written before the end of the transmission/reception 
       of the last data.
       (+@) For high bit rate frequencies, it is advised to use the DMA mode to avoid the
       degradation of the SPI speed performance due to CPU accesses impacting the 
       SPI bandwidth.
       (+@) When the hk32aspin02x are configured as slaves and the NSS hardware mode is 
       used, the NSS pin needs to be kept low between the data phase and the CRC 
       phase.
       (+@) When the SPI is configured in slave mode with the CRC feature enabled, CRC
       calculation takes place even if a high level is applied on the NSS pin. 
       This may happen for example in case of a multislave environment where the 
       communication master addresses slaves alternately.
       (+@) Between a slave deselection (high level on NSS) and a new slave selection
       (low level on NSS), the CRC value should be cleared on both master and slave
       sides in order to resynchronize the master and slave for their respective 
       CRC calculation.

    -@- To clear the CRC, follow the procedure below:
       (#@) Disable SPI using the SPI_Cmd() function
       (#@) Disable the CRC calculation using the SPI_CalculateCRC() function.
       (#@) Enable the CRC calculation using the SPI_CalculateCRC() function.
       (#@) Enable SPI using the SPI_Cmd() function.

@endverbatim
  * @{
  */

/**
  * @brief  Configures the CRC calculation length for the selected SPI.
  * @note   This function can be called only after the SPI_Init() function has 
  *         been called.  
  * @param  SPIx: where x can be 1 to select the SPI peripheral. 
  * @param  SPI_CRCLength: specifies the SPI CRC calculation length.
  *          This parameter can be one of the following values:
  *            @arg SPI_CRCLength_8b: Set CRC Calculation to 8 bits
  *            @arg SPI_CRCLength_16b: Set CRC Calculation to 16 bits
  * @retval None
  */
void SPI_CRCLengthConfig(SPI_TypeDef* SPIx, uint16_t SPI_CRCLength)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_CRC_LENGTH(SPI_CRCLength));

  /* Clear CRCL bit */
  SPIx->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_CRCL);

  /* Set new CRCL bit value */
  SPIx->CR1 |= SPI_CRCLength;
}

/**
  * @brief  Enables or disables the CRC value calculation of the transferred bytes.
  * @note   This function can be called only after the SPI_Init() function has 
  *         been called.   
  * @param  SPIx: where x can be 1 to select the SPI peripheral.
  * @param  NewState: new state of the SPIx CRC value calculation.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_CalculateCRC(SPI_TypeDef* SPIx, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Enable the selected SPI CRC calculation */
    SPIx->CR1 |= SPI_CR1_CRCEN;
  }
  else
  {
    /* Disable the selected SPI CRC calculation */
    SPIx->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_CRCEN);
  }
}

/**
  * @brief  Transmit the SPIx CRC value.
  * @param  SPIx: where x can be 1 to select the SPI peripheral.
  * @retval None
  */
void SPI_TransmitCRC(SPI_TypeDef* SPIx)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));

  /* Enable the selected SPI CRC transmission */
  SPIx->CR1 |= SPI_CR1_CRCNEXT;
}

/**
  * @brief  Returns the transmit or the receive CRC register value for the specified SPI.
  * @param  SPIx: where x can be 1 to select the SPI peripheral.
  * @param  SPI_CRC: specifies the CRC register to be read.
  *          This parameter can be one of the following values:
  *            @arg SPI_CRC_Tx: Selects Tx CRC register
  *            @arg SPI_CRC_Rx: Selects Rx CRC register
  * @retval The selected CRC register value..
  */
uint16_t SPI_GetCRC(SPI_TypeDef* SPIx, uint8_t SPI_CRC)
{
  uint16_t crcreg = 0;
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_CRC(SPI_CRC));

  if (SPI_CRC != SPI_CRC_Rx)
  {
    /* Get the Tx CRC register */
    crcreg = SPIx->TXCRCR;
  }
  else
  {
    /* Get the Rx CRC register */
    crcreg = SPIx->RXCRCR;
  }
  /* Return the selected CRC register */
  return crcreg;
}

/**
  * @brief  Returns the CRC Polynomial register value for the specified SPI.
  * @param  SPIx: where x can be 1 to select the SPI peripheral.
  * @retval The CRC Polynomial register value.
  */
uint16_t SPI_GetCRCPolynomial(SPI_TypeDef* SPIx)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));

  /* Return the CRC polynomial register */
  return SPIx->CRCPR;
}

/**
  * @}
  */


/** @defgroup SPI_Group4 Interrupts and flags management functions
 *  @brief   Interrupts and flags management functions
  *
@verbatim   
 ===============================================================================
             ##### Interrupts and flags management functions #####
 ===============================================================================
    [..] This section provides a set of functions allowing to configure the SPI Interrupts 
         sources and check or clear the flags or pending bits status.
         The user should identify which mode will be used in his application to manage 
         the communication: Polling mode, Interrupt mode or DMA mode. 

  *** Polling Mode ***
  ====================
    [..] In Polling Mode, the SPIcommunication can be managed by 9 flags:
        (#) SPI_FLAG_TXE : to indicate the status of the transmit buffer register
        (#) SPI_FLAG_RXNE : to indicate the status of the receive buffer register
        (#) SPI_FLAG_BSY : to indicate the state of the communication layer of the SPI.
        (#) SPI_FLAG_CRCERR : to indicate if a CRC Calculation error occur              
        (#) SPI_FLAG_MODF : to indicate if a Mode Fault error occur
        (#) SPI_FLAG_OVR : to indicate if an Overrun error occur
        (#) SPI_FLAG_FRE: to indicate a Frame Format error occurs.

    [..]
        (@)Do not use the BSY flag to handle each data transmission or reception. It is better 
           to use the TXE and RXNE flags instead.

    [..] In this Mode it is advised to use the following functions:
        (+) FlagStatus SPI_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_FLAG);
        (+) void SPI_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_FLAG);

  *** Interrupt Mode ***
  ======================
    [..] In Interrupt Mode, the SPI communication can be managed by 3 interrupt sources
         and 5 pending bits: 
    [..] Pending Bits:
        (#) SPI_IT_TXE : to indicate the status of the transmit buffer register
        (#) SPI_IT_RXNE : to indicate the status of the receive buffer register
        (#) SPI_IT_OVR : to indicate if an Overrun error occur
        (#) SPI_FLAG_FRE : to indicate a Frame Format error occurs.

    [..] Interrupt Source:
        (#) SPI_IT_TXE: specifies the interrupt source for the Tx buffer empty 
            interrupt.  
        (#) SPI_IT_RXNE : specifies the interrupt source for the Rx buffer not 
            empty interrupt.
        (#) SPI_IT_ERR : specifies the interrupt source for the errors interrupt.

    [..] In this Mode it is advised to use the following functions:
         (+) void SPI_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_IT, FunctionalState NewState);
         (+) ITStatus SPI_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_IT);

  *** FIFO Status ***
  ===================
    [..] It is possible to monitor the FIFO status when a transfer is ongoing using the
         following function:
         (+) uint32_t SPI_GetFIFOStatus(uint8_t SPI_FIFO_Direction); 

  *** DMA Mode ***
  ================
    [..] In DMA Mode, the SPI communication can be managed by 2 DMA Channel 
         requests:
        (#) SPI_DMAReq_Tx: specifies the Tx buffer DMA transfer request.
        (#) SPI_DMAReq_Rx: specifies the Rx buffer DMA transfer request.

    [..] In this Mode it is advised to use the following function:
        (+) void SPI_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_DMAReq, FunctionalState NewState).

@endverbatim
  * @{
  */

/**
  * @brief  Enables or disables the specified SPI interrupts.
  * @param  SPIx: where x can be 1 in SPI mode  to select 
  *         the SPI peripheral.   
  * @param  SPI_IT: specifies the SPI interrupt source to be enabled or disabled. 
  *          This parameter can be one of the following values:
  *            @arg SPI_IT_TXE: Tx buffer empty interrupt mask
  *            @arg SPI_IT_RXNE: Rx buffer not empty interrupt mask
  *            @arg SPI_IT_ERR: Error interrupt mask
  * @param  NewState: new state of the specified SPI interrupt.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SPI_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_IT, FunctionalState NewState)
{
  uint16_t itpos = 0, itmask = 0 ;

  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  assert_param(IS_SPI_CONFIG_IT(SPI_IT));

  /* Get the SPI IT index */
  itpos = SPI_IT >> 4;

  /* Set the IT mask */
  itmask = (uint16_t)1 << (uint16_t)itpos;

  if (NewState != DISABLE)
  {
    /* Enable the selected SPI interrupt */
    SPIx->CR2 |= itmask;
  }
  else
  {
    /* Disable the selected SPI interrupt */
    SPIx->CR2 &= (uint16_t)~itmask;
  }
}

/**
  * @brief  Returns the current SPIx Transmission FIFO filled level.
  * @param  SPIx: where x can be 1 to select the SPI peripheral.
  * @retval The Transmission FIFO filling state.
  *          - SPI_TransmissionFIFOStatus_Empty: when FIFO is empty
  *          - SPI_TransmissionFIFOStatus_1QuarterFull: if more than 1 quarter-full.
  *          - SPI_TransmissionFIFOStatus_HalfFull: if more than 1 half-full.
  *          - SPI_TransmissionFIFOStatus_Full: when FIFO is full.
  */
uint16_t SPI_GetTransmissionFIFOStatus(SPI_TypeDef* SPIx)
{
  /* Get the SPIx Transmission FIFO level bits */
  return (uint16_t)((SPIx->SR & SPI_SR_FTLVL));
}

/**
  * @brief  Returns the current SPIx Reception FIFO filled level.
  * @param  SPIx: where x can be 1 to select the SPI peripheral.
  * @retval The Reception FIFO filling state.
  *          - SPI_ReceptionFIFOStatus_Empty: when FIFO is empty
  *          - SPI_ReceptionFIFOStatus_1QuarterFull: if more than 1 quarter-full.
  *          - SPI_ReceptionFIFOStatus_HalfFull: if more than 1 half-full.
  *          - SPI_ReceptionFIFOStatus_Full: when FIFO is full.
  */
uint16_t SPI_GetReceptionFIFOStatus(SPI_TypeDef* SPIx)
{
  /* Get the SPIx Reception FIFO level bits */
  return (uint16_t)((SPIx->SR & SPI_SR_FRLVL));
}

/**
  * @brief  Checks whether the specified SPI flag is set or not.
  * @param  SPIx: where x can be 1 in SPI mode  to select 
  *         the SPI peripheral.    

  * @param  SPI_FLAG: specifies the SPI flag to check. 
  *          This parameter can be one of the following values:
  *            @arg SPI_FLAG_TXE: Transmit buffer empty flag.
  *            @arg SPI_FLAG_RXNE: Receive buffer not empty flag.
  *            @arg SPI_FLAG_BSY: Busy flag.
  *            @arg SPI_FLAG_OVR: Overrun flag.
  *            @arg SPI_FLAG_MODF: Mode Fault flag.
  *            @arg SPI_FLAG_CRCERR: CRC Error flag.
  *            @arg SPI_FLAG_FRE: TI frame format error flag.
  *            @arg SPI_FLAG_UDR: Underrun Error flag.
  *            @arg SPI_FLAG_CHSIDE: Channel Side flag.   
  * @retval The new state of SPI_FLAG (SET or RESET).
  */
FlagStatus SPI_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_FLAG)
{
  FlagStatus bitstatus = RESET;
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_GET_FLAG(SPI_FLAG));

  /* Check the status of the specified SPI flag */
  if ((SPIx->SR & SPI_FLAG) != (uint16_t)RESET)
  {
    /* SPI_FLAG is set */
    bitstatus = SET;
  }
  else
  {
    /* SPI_FLAG is reset */
    bitstatus = RESET;
  }
  /* Return the SPI_FLAG status */
  return  bitstatus;
}

/**
  * @brief  Clears the SPIx CRC Error (CRCERR) flag.
  * @param  SPIx: where x can be 1 to select the SPI peripheral.
  * @param  SPI_FLAG: specifies the SPI flag to clear. 
  *         This function clears only CRCERR flag.
  * @note   OVR (OverRun error) flag is cleared by software sequence: a read 
  *         operation to SPI_DR register (SPI_ReceiveData()) followed by  
  *         a read operation to SPI_SR register (SPI_GetFlagStatus()).
  * @note   MODF (Mode Fault) flag is cleared by software sequence: a read/write 
  *         operation to SPI_SR register (SPI_GetFlagStatus()) followed by
  *         a write operation to SPI_CR1 register (SPI_Cmd() to enable the SPI).
  * @retval None
  */
void SPI_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_FLAG)
{
  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_CLEAR_FLAG(SPI_FLAG));

  /* Clear the selected SPI CRC Error (CRCERR) flag */
  SPIx->SR = (uint16_t)~SPI_FLAG;
}

/**
  * @brief  Checks whether the specified SPI interrupt has occurred or not.
  * @param  SPIx: where x can be 1 in SPI mode  to select the SPI peripheral.
  * @param  SPI_IT: specifies the SPI interrupt source to check. 
  *          This parameter can be one of the following values:
  *            @arg SPI_IT_TXE: Transmit buffer empty interrupt.
  *            @arg SPI_IT_RXNE: Receive buffer not empty interrupt.
  *            @arg SPI_IT_MODF: Mode Fault interrupt.
  *            @arg SPI_IT_OVR: Overrun interrupt. 
  *            @arg SPI_IT_FRE: Format Error interrupt.  
  * @retval The new state of SPI_IT (SET or RESET).
  */
ITStatus SPI_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_IT)
{
  ITStatus bitstatus = RESET;
  uint16_t itpos = 0, itmask = 0, enablestatus = 0;

  /* Check the parameters */
  assert_param(IS_SPI_ALL_PERIPH(SPIx));
  assert_param(IS_SPI_GET_IT(SPI_IT));

  /* Get the SPI_IT index */
  itpos = 0x01 << (SPI_IT & 0x0F);

  /* Get the SPI_IT IT mask */
  itmask = SPI_IT >> 4;

  /* Set the IT mask */
  itmask = 0x01 << itmask;

  /* Get the SPI_IT enable bit status */
  enablestatus = (SPIx->CR2 & itmask) ;

  /* Check the status of the specified SPI interrupt */
  if (((SPIx->SR & itpos) != (uint16_t)RESET) && enablestatus)
  {
    /* SPI_IT is set */
    bitstatus = SET;
  }
  else
  {
    /* SPI_IT is reset */
    bitstatus = RESET;
  }
  /* Return the SPI_IT status */
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

