/********************************************************************************
  * @file    hk32f0301mxxc_gpio.c 
  * @author  AE Team
  * @version V1.0.0/2022-09-14
  *     1.初始版本
  * @log  V1.0.0
  *******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_gpio.h"
#include "hk32f0301mxxc_rcc.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup GPIO GPIO
  * @brief GPIO driver modules
  * @{
  */

/** @defgroup GPIO_Private_Defines GPIO_Private_Defines
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup GPIO_Private_Functions  GPIO_Private_Functions
  * @{
  */

/** @defgroup GPIO_Group1 Initialization and Configuration
 *  @brief   Initialization and Configuration
 *
@verbatim
 ===============================================================================
                    ##### Initialization and Configuration #####
 ===============================================================================

@endverbatim
  * @{
  */

/**
  * @brief  Deinitializes the GPIOx peripheral registers to their default reset 
  *         values.
  * @param  GPIOx: where x can be (A, B, C, D, F) to select the GPIO peripheral.  
  * @retval None
  */
void GPIO_DeInit(GPIO_TypeDef* GPIOx)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  if(GPIOx == GPIOA)
  {
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOA, DISABLE);
  }
  else if(GPIOx == GPIOB)
  {
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOB, DISABLE);
  }
  else if(GPIOx == GPIOC)
  {
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOC, DISABLE);
  }
  else
  {
    if(GPIOx == GPIOD)
    {
      RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOD, ENABLE);
      RCC_AHBPeriphResetCmd(RCC_AHBPeriph_GPIOD, DISABLE);
    }
  }
}

/**
  * @brief  Initializes the GPIOx peripheral according to the specified 
  *         parameters in the GPIO_InitStruct.
  * @param  GPIOx: where x can be (A, B, C, D, F) to select the GPIO peripheral. 
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that contains
  *         the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
  uint32_t pinpos = 0x00, pos = 0x00 , currentpin = 0x00;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_Pin));
  assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
  assert_param(IS_GPIO_PUPD(GPIO_InitStruct->GPIO_PuPd));

  /*-------------------------- Configure the port pins -----------------------*/
  /*-- GPIO Mode Configuration --*/
  for (pinpos = 0x00; pinpos < 0x10; pinpos++)
  {
    pos = ((uint32_t)0x01) << pinpos;

    /* Get the port pins position */
    currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

    if (currentpin == pos)
    {
      if ((GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT) || (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_AF))
      {
        /* Check Speed mode parameters */
        assert_param(IS_GPIO_SPEED(GPIO_InitStruct->GPIO_Speed));

        /* Speed mode configuration */
        GPIOx->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR0 << (pinpos * 2));
        GPIOx->OSPEEDR |= ((uint32_t)(GPIO_InitStruct->GPIO_Speed) << (pinpos * 2));

        /* Check Output mode parameters */
        assert_param(IS_GPIO_OTYPE(GPIO_InitStruct->GPIO_OType));

        /* Output mode configuration */
        GPIOx->OTYPER &= ~((GPIO_OTYPER_OT0) << ((uint16_t)pinpos));
        GPIOx->OTYPER |= (uint16_t)(((uint16_t)GPIO_InitStruct->GPIO_OType) << ((uint16_t)pinpos));
      }

      GPIOx->MODER  &= ~(GPIO_MODER_MODER0 << (pinpos * 2));

      GPIOx->MODER |= (((uint32_t)GPIO_InitStruct->GPIO_Mode) << (pinpos * 2));

      /* Pull-up Pull down resistor configuration */
      GPIOx->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << ((uint16_t)pinpos * 2));
      GPIOx->PUPDR |= (((uint32_t)GPIO_InitStruct->GPIO_PuPd) << (pinpos * 2));
    }
  }

  /* config schmit*/
	if (GPIO_InitStruct->GPIO_Schmit == GPIO_Schmit_Disable)
	{
		GPIOx->IOSR |= (GPIO_InitStruct->GPIO_Pin);
	}
	else
	{
		GPIOx->IOSR &= ~(GPIO_InitStruct->GPIO_Pin);
	}

}

/**
  * @brief  Fills each GPIO_InitStruct member with its default value.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure which will 
  *         be initialized.
  * @retval None
  */
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
  /* Reset GPIO init structure parameters values */
  GPIO_InitStruct->GPIO_Pin  = GPIO_Pin_All;
  GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct->GPIO_Speed = GPIO_Speed_Level_2;
  GPIO_InitStruct->GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct->GPIO_PuPd = GPIO_PuPd_NOPULL;
}

/**
  * @brief  Locks GPIO Pins configuration registers.
  * @note   The locked registers are GPIOx_MODER, GPIOx_OTYPER, GPIOx_OSPEEDR,
  *         GPIOx_PUPDR, GPIOx_AFRL and GPIOx_AFRH.
  * @note   The configuration of the locked GPIO pins can no longer be modified
  *         until the next device reset.
  * @param  GPIOx: where x can be (A or B) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bit to be written.
  *          This parameter can be any combination of GPIO_Pin_x where x can be (0..7).
  * @retval None
  */
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  __IO uint32_t tmp = 0x00010000;

  /* Check the parameters */
  assert_param(IS_GPIO_LIST_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  tmp |= GPIO_Pin;
  /* Set LCKK bit */
  GPIOx->LCKR = tmp;
  /* Reset LCKK bit */
  GPIOx->LCKR =  GPIO_Pin;
  /* Set LCKK bit */
  GPIOx->LCKR = tmp;
  /* Read LCKK bit */
  tmp = GPIOx->LCKR;
  /* Read LCKK bit */
  tmp = GPIOx->LCKR;
}

/**
  * @}
  */

/** @defgroup GPIO_Group2 GPIO Read and Write
 *  @brief   GPIO Read and Write
 *
@verbatim   
 ===============================================================================
                      ##### GPIO Read and Write #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  Reads the specified input port pin bit.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bit to read.
  * @note   This parameter can be GPIO_Pin_x where x can be:
  *          (0..7) for GPIOA, GPIOB, GPIOC, for GPIOD 
  * @retval The input port pin value.
  */
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  uint8_t bitstatus = 0x00;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

  if ((GPIOx->IDR & GPIO_Pin) != (uint32_t)Bit_RESET)
  {
    bitstatus = (uint8_t)Bit_SET;
  }
  else
  {
    bitstatus = (uint8_t)Bit_RESET;
  }
  return bitstatus;
}

/**
  * @brief  Reads the specified input port pin.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.   
  * @retval The input port pin value.
  */
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  return ((uint16_t)GPIOx->IDR);
}

/**
  * @brief  Reads the specified output data port bit.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral. 
  * @param  GPIO_Pin: Specifies the port bit to read.
  * @note   This parameter can be GPIO_Pin_x where x can be:
  *          (0..7) for GPIOA, GPIOB, GPIOC, for GPIOD 
  * @retval The output port pin value.
  */
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  uint8_t bitstatus = 0x00;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));

  if ((GPIOx->ODR & GPIO_Pin) != (uint32_t)Bit_RESET)
  {
    bitstatus = (uint8_t)Bit_SET;
  }
  else
  {
    bitstatus = (uint8_t)Bit_RESET;
  }
  return bitstatus;
}

/**
  * @brief  Reads the specified GPIO output data port.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
    
  * @retval GPIO output data port value.
  */
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  return ((uint16_t)GPIOx->ODR);
}

/**
  * @brief  Sets the selected data port bits.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral. 
  * @param  GPIO_Pin: specifies the port bits to be written.
  * @note   This parameter can be GPIO_Pin_x where x can be:
  *          (0..7) for GPIOA, GPIOB, GPIOC, for GPIOD 
  * @retval None
  */
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  GPIOx->BSRR = GPIO_Pin;
}

/**
  * @brief  Clears the selected data port bits.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bits to be written.
  * @note   This parameter can be GPIO_Pin_x where x can be:
  *          (0..7) for GPIOA, GPIOB, GPIOC, for GPIOD 
  * @retval None
  */
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN(GPIO_Pin));

  GPIOx->BRR = GPIO_Pin;
}

/**
  * @brief  Writes the selected data port bit.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
  * @param  GPIO_Pin: specifies the port bit to be written.
  * @param  BitVal: specifies the value to be written to the selected bit.
  *          This parameter can be one of the BitAction enumeration values:
  *            @arg Bit_RESET: to clear the port pin
  *            @arg Bit_SET: to set the port pin
  * @note   This parameter can be GPIO_Pin_x where x can be:
  *          (0..7) for GPIOA, GPIOB, GPIOC, for GPIOD 
  * @retval None
  */
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GET_GPIO_PIN(GPIO_Pin));
  assert_param(IS_GPIO_BIT_ACTION(BitVal));

  if (BitVal != Bit_RESET)
  {
    GPIOx->BSRR = GPIO_Pin;
  }
  else
  {
    GPIOx->BRR = GPIO_Pin ;
  }
}

/**
  * @brief  Writes data to the specified GPIO data port.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
  * @param  PortVal: specifies the value to be written to the port output data register.
  * @retval None
  */
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal)
{
  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));

  GPIOx->ODR = PortVal;
}

/**
  * @brief  Toggle the GPIO port.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
  * @param  GPIO_Pin: sGPIO_pins_define
  *         GPIO_Pin_0
  *         GPIO_Pin_1
  *         GPIO_Pin_2
  *         GPIO_Pin_3
  *         GPIO_Pin_4
  *         GPIO_Pin_5
  *         GPIO_Pin_6
  *         GPIO_Pin_7 
  * @retval None
  */
void GPIO_Toggle(GPIO_TypeDef* GPIOx , uint16_t GPIO_Pin)
{
	GPIOx->ODR ^= GPIO_Pin;
}

/** @defgroup GPIO_Group3 GPIO Alternate functions configuration functions
 *  @brief   GPIO Alternate functions configuration functions
 *
@verbatim   
 ===============================================================================
          ##### GPIO Alternate functions configuration functions #####
 ===============================================================================  

@endverbatim
  * @{
  */

/**
  * @brief  selects the pin to used as Alternate function.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
  * @param  GPIO_PinSource: specifies the pin for the Alternate function.
  *          This parameter can be GPIO_PinSourcex where x can be (0..15) for GPIOA, GPIOB,GPIOB, GPIOD  
  * @param  GPIO_AF: selects the pin to used as Alternate function.
  *          This parameter can be one of the following value:
  *            @arg GPIO_AF_0: (I2C1_WD)I2C1_SMBA, I2C1_SCL, SWCLK_I2C1_SDA, I2C1_SDA, SWDIO
  *            @arg GPIO_AF_1: (USART1)USART1_TX, USART1_RX, USART1_CK 
  *            @arg GPIO_AF_2: (SPI)SPI1_SCK, SPI1_NSS, SPI1_MISO, SPI1_NSS, SPI1_MOSI
  *            @arg GPIO_AF_3: (TIM1)TIM1_BKIN, TIM1_CH1N, TIM1_CH2N, TIM1_CH3N,TIM1_CH3_CH1N,TIM1_CH4_CH2N, TIM1_ETR,TIM1_CH1,TIM1_CH2,TIM1_CH4,
  *                           TIM1_ETR
  *            @arg GPIO_AF_4: (TIM2)TIM2_CH3, TIM2_ETR, TIM2_CH4, TIM2_CH2, TIM2_CH1
  *            @arg GPIO_AF_5: (RCC)RCC_MCO
  *            @arg GPIO_AF_6: (BEEPER)BEEP
  *            @arg GPIO_AF_7: (ADC1)ADC1_ETR
  * @note   The pin should already been configured in Alternate Function mode(AF)
  *         using GPIO_InitStruct->GPIO_Mode = GPIO_Mode_AF
  * @note   Refer to the Alternate function mapping table in the device datasheet 
  *         for the detailed mapping of the system and peripherals'alternate 
  *         function I/O pins.
  * @retval None
  */

void GPIO_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t GPIO_AF)
{
  uint32_t temp = 0x00;
  uint32_t temp_2 = 0x00;

  /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource));
  assert_param(IS_GPIO_AF(GPIO_AF));

  temp = ((uint32_t)(GPIO_AF) << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4));
  GPIOx->AFR &= ~((uint32_t)0xF << ((uint32_t)((uint32_t)GPIO_PinSource & (uint32_t)0x07) * 4));
  temp_2 = GPIOx->AFR | temp;
  GPIOx->AFR = temp_2;
	
	

}

/**
  * @brief  selects the IOMUX pin to used as Alternate function.
  * @param  GPIOx: where x can be (A, B, C, D) to select the GPIO peripheral.
  * @param  GPIO_PinSource: specifies the pin for the Alternate function.
  *          This parameter can be GPIO_PinSourcex where x can be (0..15) for GPIOA, GPIOB,GPIOB, GPIOD  
  * @param  IOMUX_AF: selects the pin to used as Alternate function.
  *          This parameter can be one of the following value:
  *           GPIOMUX_AF_3_TIM1CH3 
              GPIOMUX_AF_3_TIM1CH1N
              GPIOMUX_AF_3_TIM1CH4 
              GPIOMUX_AF_3_TIM1CH2N
			  GPIOMUX_AF0_SWCLK         
			  GPIOMUX_AF0_I2C_SDA   
              GPIOMUX_AF3_TIM1BKIN                
  * @retval None
  */

void GPIO_IOMUX_PinAFConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_PinSource, uint8_t IOMUX_AF)
{
   /* Check the parameters */
  assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
  assert_param(IS_GPIO_PIN_SOURCE(GPIO_PinSource));
  assert_param(GPIO_IOMUX_AF(IOMUX_AF));
  /* IOMUX function*/  
   // if PD7 as TIM1_CH3 or TIM1_BKIN
  if((GPIOx == GPIOD)&&(GPIO_PinSource == GPIO_PinSource7))
  {
    if(IOMUX_AF == GPIOMUX_AF3_TIM1CH3)
    { 
      IOMUX->PIN_FUNC_SEL &= IOMUX_PD7_TIM1_CH3;
    } 
    else if(IOMUX_AF == GPIOMUX_AF3_TIM1BKIN)
    {
      IOMUX->PIN_FUNC_SEL |= IOMUX_PD7_TIM1_BKIN;
    }
  }
  // if PB5 as SWCLK or I2C_SDA
  if((GPIOx == GPIOB)&&(GPIO_PinSource == GPIO_PinSource5))
  {
    if(IOMUX_AF == GPIOMUX_AF0_SWCLK)
    { 
      IOMUX->PIN_FUNC_SEL &= IOMUX_PB5_SWCLK;
    } 
    else if(IOMUX_AF == GPIOMUX_AF0_I2C_SDA)
    {
      IOMUX->PIN_FUNC_SEL |= IOMUX_PB5_I2C_SDA;
    }
  }
  // select PC3 as TIM1 CH3 or CHIN
  else if((GPIOx == GPIOC)&&(GPIO_PinSource == GPIO_PinSource3))
  {
    
    if(IOMUX_AF == GPIOMUX_AF3_TIM1CH3)
    {
      IOMUX->PIN_FUNC_SEL &= IOMUX_PC3_TIM1CH3;
    }
    else if(IOMUX_AF == GPIOMUX_AF3_TIM1CH1N)
    {
      IOMUX->PIN_FUNC_SEL |= IOMUX_PC3_TIM1CH1N;
    }    
  }
  // select PC4 as TIM1 CH4 or CH2N
  else if((GPIOx == GPIOC)&&(GPIO_PinSource == GPIO_PinSource4))
  {
    if(IOMUX_AF == GPIOMUX_AF3_TIM1CH4)
    {
      IOMUX->PIN_FUNC_SEL &= IOMUX_PC4_TIM1CH4;
    }
    else if(IOMUX_AF == GPIOMUX_AF3_TIM1CH2N)
    {
      IOMUX->PIN_FUNC_SEL |= IOMUX_PC4_TIM1CH2N;
    }
  }

}


/**
  * @brief change the pin to others IOMUX pin.
  * @param  eIOMUX_Pinx: where x can be (1 5 6 7 8) to select the GPIO pin.
  * @param  IOMUX_FuncPin: specifies the pin for the Alternate function.
  *          This parameter can be :
					IOMUX_PD6_SEL_PD6  
					IOMUX_PD6_SEL_PA1  			
					IOMUX_PD6_SEL_PD4  			
					IOMUX_PD6_SEL_PA2  	
							
					IOMUX_PB5_SEL_PB5  			
					IOMUX_PB5_SEL_PA3  			
					IOMUX_PB5_SEL_PD2  			
							
					IOMUX_NRST_SEL_NRST			
					IOMUX_NRST_SEL_PA0 			
					IOMUX_NRST_SEL_PB4 			
							
					IOMUX_PC4_SEL_PC4  			
					IOMUX_PC4_SEL_PC5  			
					IOMUX_PC4_SEL_PC3  			
					IOMUX_PC4_SEL_PC7  

					IOMUX_PD5_SEL_PD5 
					IOMUX_PD5_SEL_PD3
					IOMUX_PD5_SEL_PD1
					IOMUX_PD5_SEL_PC6
			 
  * @note   The IOMUX_FuncPin parameter should from the list of IOMUX_FuncPin 
  *          1.nrst_pa0_sel can only be powered on reset 
  *          2. only when nrst_pin_key [15:0] = 0x5ae1  can be written by nrst_pa0_sel
  *         3. When the cpu changes the value of pkg_pin_sel.nrstpa0_pin_sel or nrst_pa0_sel, the nrst_pin_key [15:0] is reset by the system hardware.
  * @retval None
  */  
void GPIO_IOMUX_ChangePin(IOMUX_PIN eIOMUX_Pinx, uint32_t IOMUX_FuncPin)
{
     /* Check the parameters */
  assert_param(IS_IOMUX_PIN(eIOMUX_Pinx));
  assert_param(IS_IOMUX_PINFNC(IOMUX_FuncPin));

  switch (eIOMUX_Pinx)
  {
    case IOMUX_PIN1:
      #if defined HK32F0301MJ4M7C  //SOP 8 pin
        IOMUX->PKG_PIN_SEL &= IOMUX_PD6_SEL_MASK;//clear select bits(7 8)
        IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
		  #elif defined HK32F0301MD4P7C   //SOP 16 pin
        if((IOMUX_FuncPin == IOMUX_PD6_SEL_PD6)||(IOMUX_FuncPin == IOMUX_PD6_SEL_PD4))
        {
          IOMUX->PKG_PIN_SEL &= IOMUX_PD6_SEL_MASK;//clear select bits(7 8)
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
        }
      #elif defined HK32F0301MF4U7C  //QFN 20 pin
        if((IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)||(IOMUX_FuncPin == IOMUX_NRST_SEL_NRST))
         {
            IOMUX->NRST_PIN_KEY = NRST_PINKEY;
						IOMUX->PKG_PIN_SEL &= IOMUX_NRST_SEL_MASK;//clear select bits(0)
						IOMUX->NRST_PIN_KEY = NRST_PINKEY;
						IOMUX->PKG_PIN_SEL |= IOMUX_NRST_SEL_PA0;
            
            if(IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)
            {
              IOMUX->NRST_PIN_KEY = NRST_PINKEY;
              IOMUX->NRST_PA0_SEL |= (uint32_t)(0x00000001);
            }          
            else  
            {
              IOMUX->NRST_PIN_KEY = NRST_PINKEY;
              IOMUX->NRST_PA0_SEL &= (uint32_t)(0x00000000);
            } 
         }
      #endif
    break;
    case IOMUX_PIN2://TSSOP16 
      #if defined (HK32F0301MD4P7C) 
        if((IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)||(IOMUX_FuncPin == IOMUX_NRST_SEL_NRST))
        {
			IOMUX->NRST_PIN_KEY = NRST_PINKEY;
			IOMUX->PKG_PIN_SEL &= IOMUX_NRST_SEL_MASK;//clear select bits(0)
			IOMUX->NRST_PIN_KEY = NRST_PINKEY;
			IOMUX->PKG_PIN_SEL |= IOMUX_NRST_SEL_PA0;

            if(IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)
            {
              IOMUX->NRST_PIN_KEY = NRST_PINKEY;
              IOMUX->NRST_PA0_SEL |= (uint32_t)(0x00000001);
            }          
            else  
            {
              IOMUX->NRST_PIN_KEY = NRST_PINKEY;
              IOMUX->NRST_PA0_SEL &= (uint32_t)(0x00000000);
            }           
        }
       #elif  defined (HK32F0301MF4P7C) 
         if(IOMUX_FuncPin == IOMUX_PD5_SEL_PD5)
          {
            IOMUX->PKG_PIN_SEL &= IOMUX_PD5_SEL_MASK;//clear select bits(bit 6 5)
            IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
          }          
      #endif
    break; 
    case IOMUX_PIN4:
       #if defined  (HK32F0301MF4P7C) 
        if((IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)||(IOMUX_FuncPin == IOMUX_NRST_SEL_NRST))
        {		 
						IOMUX->NRST_PIN_KEY = NRST_PINKEY;
						IOMUX->PKG_PIN_SEL &= IOMUX_NRST_SEL_MASK;//clear select bits(0)
						IOMUX->NRST_PIN_KEY = NRST_PINKEY;
						IOMUX->PKG_PIN_SEL |= IOMUX_NRST_SEL_PA0;
						if(IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)
						{
							IOMUX->NRST_PIN_KEY = NRST_PINKEY;
							IOMUX->NRST_PA0_SEL |= (uint32_t)(0x00000001); 
						}
						else
						{
							IOMUX->NRST_PIN_KEY = NRST_PINKEY;
							IOMUX->NRST_PA0_SEL &= (uint32_t)(0x00000000);
						} 
				}
    
      #endif
    break;
    case IOMUX_PIN5:
       #if defined  HK32F0301MJ4M7C
        IOMUX->PKG_PIN_SEL &= IOMUX_PB5_SEL_MASK;//clear select bits(2 1)
        IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;     
      #endif
    break;
    case IOMUX_PIN6:
      #if defined  HK32F0301MJ4M7C
        IOMUX->PKG_PIN_SEL &= IOMUX_PC4_SEL_MASK;//clear select bits(4 3 )
        IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
      #endif
    break;
    case IOMUX_PIN7:
    #if defined  HK32F0301MJ4M7C    
			if((IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)||(IOMUX_FuncPin == IOMUX_NRST_SEL_NRST))
        {				
						IOMUX->NRST_PIN_KEY = NRST_PINKEY;
						IOMUX->PKG_PIN_SEL &= IOMUX_NRST_SEL_MASK;//clear select bits(0)
						IOMUX->NRST_PIN_KEY = NRST_PINKEY;
						IOMUX->PKG_PIN_SEL |= IOMUX_NRST_SEL_PA0;
						if(IOMUX_FuncPin == IOMUX_NRST_SEL_PA0)
						{
							IOMUX->NRST_PIN_KEY = NRST_PINKEY;
							IOMUX->NRST_PA0_SEL |= (uint32_t)(0x00000001);
						}
						else
						{
							IOMUX->NRST_PIN_KEY = NRST_PINKEY;
							IOMUX->NRST_PA0_SEL &= (uint32_t)(0x00000000);
						} 
				}
        else if(IOMUX_FuncPin == IOMUX_NRST_SEL_PB4)
        {
					  IOMUX->NRST_PIN_KEY = NRST_PINKEY;
            IOMUX->PKG_PIN_SEL &= IOMUX_NRST_SEL_MASK;//clear select bits(0)
					
					  IOMUX->NRST_PIN_KEY = NRST_PINKEY;
            IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
        }        
    #endif

    break;
    case IOMUX_PIN8:
       #if defined  HK32F0301MJ4M7C
          IOMUX->PKG_PIN_SEL &= IOMUX_PD5_SEL_MASK;//clear select bits (6 5)
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
      #elif defined HK32F030MF4U6  //QFN 20 pin
          IOMUX->PKG_PIN_SEL &= IOMUX_PB5_SEL_MASK;//clear select bits (2 1 )
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
      #endif  
    break;
    case IOMUX_PIN9:
      #if defined  HK32F0301MD4P7C
        if((IOMUX_FuncPin == IOMUX_PB5_SEL_PB5)||(IOMUX_FuncPin == IOMUX_PB5_SEL_PD2))
        {
          IOMUX->PKG_PIN_SEL &= IOMUX_PB5_SEL_MASK;//clear select bits (2 1 )
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
        }
      #endif
    break;
    case IOMUX_PIN11:
      #if defined HK32F0301MF4P7C  
          IOMUX->PKG_PIN_SEL &= IOMUX_PB5_SEL_MASK;//clear select bits (2 1 )
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;    
      #endif
    break;
    case IOMUX_PIN12:
       #if defined  HK32F0301MD4P7C
        if((IOMUX_FuncPin == IOMUX_PC4_SEL_PC4)||(IOMUX_FuncPin == IOMUX_PC4_SEL_PC7))
        {
          IOMUX->PKG_PIN_SEL &= IOMUX_PC4_SEL_MASK;//clear select bits (4 3 )
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
        }
      #endif
    break;
    case IOMUX_PIN15:
      #if defined  HK32F0301MD4P7C 
        if((IOMUX_FuncPin == IOMUX_PD5_SEL_PD5)||(IOMUX_FuncPin == IOMUX_PD5_SEL_PD1))
        {
          IOMUX->PKG_PIN_SEL &= IOMUX_PD5_SEL_MASK;//clear select bits (6 5)
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
        }
      #endif
    break;
    case IOMUX_PIN19:
      #if defined  HK32F0301MF4U7C 
        if(IOMUX_FuncPin == IOMUX_PD5_SEL_PD5)
        {
          IOMUX->PKG_PIN_SEL &= IOMUX_PD5_SEL_MASK;//clear select bits (6 5)
          IOMUX->PKG_PIN_SEL |= IOMUX_FuncPin;
        }
      #endif
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

/**
  * @}
  */
  
/**
  * @}
*/
 
