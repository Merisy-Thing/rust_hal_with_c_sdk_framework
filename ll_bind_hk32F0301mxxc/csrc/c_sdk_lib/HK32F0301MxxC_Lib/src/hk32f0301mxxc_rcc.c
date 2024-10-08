/********************************************************************
* @copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @filename: hk32f0301mxxc_rcc.c
* @brief  : RCC initialization and configuration  
* @author : AE Team  
* @version: V1.0.0/2022-09-12 
*         1.初始版本
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_rcc.h"
#include "hk32f0301mxxc_conf.h"
/** @addtogroup HK32F0301MxxC_StdPeriph_Driver 
  * @{
  */
  
/** @defgroup RCC  RCC
  * @brief RCC driver modules
  *@{
  @verbatim
===============================================================================
                        ##### RCC specific features #####
===============================================================================
    [..] After reset the device is running from HSI with Flash 0 WS, 
         all peripherals are off except internal SRAM, Flash and SWD.
         (#) There is a prescaler on High speed (AHB) and Low speed (APB) busses;
             all peripherals mapped on these busses are running at HSI speed.
         (#) The clock for all peripherals is switched off, except the SRAM and FLASH.
         (#) All GPIOs are in input floating state, except the SWD pins which
             are assigned to be used for debug purpose.
    [..] Once the device started from reset, the user application has to:
         (#) Configure the clock source to be used to drive the System clock
             (if the application needs higher frequency/performance)
         (#) Configure the System clock frequency and Flash settings
         (#) Configure the AHB and APB busses prescalers
         (#) Enable the clock for the peripheral(s) to be used
         (#) Configure the clock source(s) for peripherals which clocks are not
             derived from the System clock (ADC, I2C, UART1/2/,and IWDG)
    [..]复位后，设备运行从HSI与Flash 0 WS。除了内部SRAM, Flash和SWD外，所有外设都关闭。 
	(#)在高速(AHB)和低速(APB)总线上有一个分频器;映射在这些总线上的所有外设都以HSI速度运行。
	(#)除了SRAM和FLASH外，所有外设的时钟都关闭了。
	(#)所有的GPIO都处于输入浮动状态，除了SWD引脚被分配用于调试目的。
	[..]一旦设备重新启动，用户应用程序必须: 
	    (#)配置驱动系统时钟的时钟源 (如果用户程序需要更高的频率/性能) 
		(#)配置系统时钟频率和Flash设置
		(#)配置AHB和APB总线预分配器
		(#)启用外围设备的时钟
		(#)对于一些外设的时钟不是来自于System Clock的，需要配置时钟源(ADC, I2C, UART1/2，和IWDG)。
   @endverbatim
  */ 

  
/** @defgroup RCC_Private_Macro_Define RCC_Private_Macro_Define
  * @brief   RCC Macro Define table
  * @{
  */

/* RCC Flag Mask */
#define FLAG_MASK                 ((uint8_t)0x1F)

/* CIR register byte 1 (Bits[15:8]) base address */
#define CIR_BYTE1_ADDRESS         ((uint32_t)0x40021009)

/* CIR register byte 2 (Bits[23:16]) base address */
#define CIR_BYTE2_ADDRESS         ((uint32_t)0x4002100A)



const uint16_t  AHBPrescTable[16] = {1, 6, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 64, 128, 256, 512}; 
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};


/**
  * @}
  */
  
/** @defgroup RCC_Private_Functions RCC_Private_Functions
  * @{
  */ 

/** @defgroup  RCC_Group1 External clocks, HSI, LSI , CSS and MCO configuration functions
 *  brief   External clocks, HSI, LSI , CSS and MCO configuration functions
 *
@verbatim
 ===============================================================================
 ##### External clocks, HSI, LSI , CSS and MCO configuration functions #####
 ===============================================================================
    [..] This section provides functions allowing to configure the external clocks,
         HSI, LSI ,CSS and MCO.
         (#) HSI (high-speed internal), 48 MHz factory-trimmed RC used directly .
             The HSI clock can be used also to clock the I2C , ADC , UART1/2
         (#) LSI (low-speed internal), 40 KHz low consumption RC used as IWDG clock source.
         (#) External CLK, 4 to 48 MHz used directly 
         (#) CSS (Clock security system), once enabled and if a EXT clock failure occurs, 
             the System clock is automatically switched to HSI and an interrupt is generated if enabled. 
             The interrupt is linked to the Cortex-M0 NMI (Non-Maskable Interrupt) 
             exception vector.   
         (#) MCO (microcontroller clock output), used to output SYSCLK, HSI, LSI, clock on mco pin.
@endverbatim
 *@{
 */

/**
  * @brief  Resets the RCC clock configuration to the default reset state.
  * @note   The default reset state of the clock configuration is given below:
  * @note   HSI ON and used as system clock source 
  * @note   AHB, APB prescaler set to 1.
  * @note   MCO OFF
  * @note   All interrupts disabled
  * @note   However, this function doesn't modify the configuration of the Peripheral clocks and LSI CLK.
  * @retval None
  */
void RCC_DeInit(void)
{
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0] ,MCO[2:0] and MCOPRE[2:0] bits */
  RCC->CFGR &= (uint32_t)0x80FFF80C;
  
  /* Reset UART1SW[1:0], I2CSW,UART2SW[1:0],UART3SW[1:0] and UART4SW[1:0] bits */
  RCC->CFGR3 &= (uint32_t)0xFFC0FFEC;
  
  /* Disable all interrupts */
  RCC->CIR = 0x00000000;
}


/**
  * @brief  Waits for start-up.
  * @note   This function waits on HSIRDY/LSIRDY/EXTRDY flag to be set and return SUCCESS if 
  *         this flag is set, otherwise returns ERROR if the timeout is reached 
  *         and this flag is not set. The timeout value is defined by the constant
  *         STARTUP_TIMEOUT in hk32f0301mxxc.h file. 
  * @note   is stopped by hardware when entering STOP modes.
  * @param  RCC_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *             @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready  
  *             @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
  *             @arg RCC_FLAG_EXTRDY: EXT clock ready
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS:   ready to use
  *          - ERROR: not yet ready
  */
ErrorStatus RCC_WaitForStartUp(uint8_t RCC_FLAG)
{
  __IO uint32_t StartUpCounter = 0;
  ErrorStatus status = ERROR;
  FlagStatus OSCStatus = RESET;
  
  /* Wait till OSC is ready and if timeout is reached exit */
  do
  {
    OSCStatus = RCC_GetFlagStatus(RCC_FLAG);
    StartUpCounter++;  
  } while((StartUpCounter != STARTUP_TIMEOUT) && (OSCStatus == RESET));
  
  if(RCC_GetFlagStatus(RCC_FLAG) != RESET)
  {
    status = SUCCESS;
  }
  else
  {
    status = ERROR;
  }  
  return (status);
}

/**
  * @brief  Adjusts the Internal High Speed oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.
  * @param  HSITrimValue: specifies the HSI calibration trimming value.
  *         This parameter must be a number between 0 and 0x3F.
  * @retval None
  */
void RCC_TrimHSIValue(uint8_t HSITrimValue)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_HSI_CALIBRATION_VALUE(HSITrimValue));
  
  tmpreg = RCC->CR;
  
  /* Clear HSITRIM[4:0] bits */
  tmpreg &= ~RCC_CR_HSITRIM;
  
  /* Set the HSITRIM[4:0] bits according to HSICalibrationValue value */
  tmpreg |= (uint32_t)HSITrimValue << RCC_CR_HSITRIM_Pos;

  /* Store the new value */
  RCC->CR = tmpreg;
}

/**
  * @brief  Adjusts the Internal High Speed oscillator (HSI) calibration value.
  * @note   The calibration is used to compensate for the variations in voltage
  *         and temperature that influence the frequency of the internal HSI RC.
  * @param  HSICalibrationValue: specifies the HSI calibration trimming value.
  *         This parameter must be a number between 0 and 0x3F.
  * @retval None
  */
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_HSI_CALIBRATION_VALUE(HSICalibrationValue));
  
  tmpreg = RCC->CR;
  
  /* Clear HSITRIM[4:0] bits */
  tmpreg &= ~RCC_CR_HSICAL;
  
  /* Set the HSITRIM[4:0] bits according to HSICalibrationValue value */
  tmpreg |= (uint32_t)HSICalibrationValue << RCC_CR_HSICAL_Pos;
  ;

  /* Store the new value */
  RCC->CR = tmpreg;
}

/**
  * @brief  Enables or disables the Internal High Speed oscillator (HSI).
  * @note   After enabling the HSI, the application software should wait on 
  *         HSIRDY flag to be set indicating that HSI clock is stable and can
  *         be used to clock the system clock.
  * @note   HSI can not be stopped if it is used directly.
  			In this case, you have to select another source 
  *         of the system clock then stop the HSI.
  * @note   The HSI is stopped by hardware when entering STOP modes.
  * @param  NewState: new state of the HSI.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When the HSI is stopped, HSIRDY flag goes low after 6 HSI oscillator
  *         clock cycles.
  * @retval None
  */
void RCC_HSICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    RCC->CR |= RCC_CR_HSION;
  }
  else
  {
    RCC->CR &= ~RCC_CR_HSION;
  }
}


/**
  * @brief  Enables or disables the Internal Low Speed oscillator (LSI).
  * @note   After enabling the LSI, the application software should wait on 
  *         LSIRDY flag to be set indicating that LSI clock is stable and can
  *         be used to clock the IWDG.
  * @note   LSI can not be disabled if the IWDG is running.
  * @param  NewState: new state of the LSI.
  *          This parameter can be: ENABLE or DISABLE.
  * @note   When the LSI is stopped, LSIRDY flag goes low after 3 LSI oscillator
  *         clock cycles.
  * @retval None
  */
void RCC_LSICmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    RCC->CSR |= RCC_CSR_LSION;
  }
  else
  {
    RCC->CSR &= ~RCC_CSR_LSION;
  }
}


/**
  * @brief	Enables or disables the EXTCLK.
  * @param	NewState: new state of the EXTCLK.
  * 		 This parameter can be: ENABLE or DISABLE.
  * @param	EXTCKL_SEL: before RCC_EXTConfig，user should config Corresponding IO 
			@arg RCC_CFGR4_EXTCLK_SEL_PA1:to PA1
			@arg RCC_CFGR4_EXTCLK_SEL_PD7:to PD7
			@arg RCC_CFGR4_EXTCLK_SEL_PB5:to PB5
			@arg RCC_CFGR4_EXTCLK_SEL_PC5:to PC5
  * @retval None
  */
void RCC_EXTConfig(FunctionalState NewState, uint32_t EXTCKL_SEL)
{
	 /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
	
	assert_param(IS_RCC_EXTCLK_GPIO(EXTCKL_SEL));
	
	/*CLOCK select */
	RCC->CFGR4 &= (uint32_t)~(RCC_CFGR4_EXTCLK_SEL);
	
	RCC->CFGR4 |= (uint32_t)EXTCKL_SEL;  
	  
	if (NewState != DISABLE)
	{
	  RCC->CR |= RCC_CR_EXTCLKON;
	}
	else
	{
	  RCC->CR &= ~RCC_CR_EXTCLKON;
	}
}



/**
  * @brief  Enables or disables the Clock Security System.
  * @note   If a failure is detected on the EXT clock, this CLK
  *         is automatically disabled and an interrupt is generated to inform the
  *         software about the failure (Clock Security System Interrupt, CSSI),
  *         allowing the MCU to perform rescue operations. The CSSI is linked to 
  *         the Cortex-M0 NMI (Non-Maskable Interrupt) exception vector.
  * @param  NewState: new state of the Clock Security System.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ClockSecuritySystemCmd(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    RCC->CR |= RCC_CR_CSSON;
  }
  else
  {
    RCC->CR &= ~RCC_CR_CSSON;
  }
}



/**
  * @brief  Selects the clock source to output on MCO pinx  and the corresponding
  *         prescsaler.
  * @note   Pinx should be configured in alternate function mode.
  * @param  RCC_MCOSource: specifies the clock source to output.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCOSource_NoClock: No clock selected.
  *            @arg RCC_MCOSource_LSI: LSI oscillator clock selected.
  *            @arg RCC_MCOSource_SYSCLK: System clock selected.
  *            @arg RCC_MCOSource_HSI: HSI oscillator clock selected.
  * @param  RCC_MCOPrescaler: specifies the prescaler on MCO pin.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCOPrescaler_1: MCO clock is divided by 1.
  *            @arg RCC_MCOPrescaler_2: MCO clock is divided by 2.
  *            @arg RCC_MCOPrescaler_4: MCO clock is divided by 4.
  *            @arg RCC_MCOPrescaler_8: MCO clock is divided by 8.
  *            @arg RCC_MCOPrescaler_16: MCO clock is divided by 16.
  *            @arg RCC_MCOPrescaler_32: MCO clock is divided by 32.
  *            @arg RCC_MCOPrescaler_64: MCO clock is divided by 64.
  *            @arg RCC_MCOPrescaler_128: MCO clock is divided by 128.    
  * @retval None
  */
void RCC_MCOConfig(uint8_t RCC_MCOSource, uint32_t RCC_MCOPrescaler)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_MCO_SOURCE(RCC_MCOSource));
  assert_param(IS_RCC_MCO_PRESCALER(RCC_MCOPrescaler));
    
  /* Get CFGR value */  
  tmpreg = RCC->CFGR;
	
  /* Clear MCOPRE[2:0] bits */
  tmpreg &= ~(RCC_CFGR_MCOPRE | RCC_CFGR_MCO );
	
  /* Set the RCC_MCOSource and RCC_MCOPrescaler */
  tmpreg |= (RCC_MCOPrescaler | ((uint32_t)RCC_MCOSource<<RCC_CFGR_MCO_Pos));
	
  /* Store the new value */
  RCC->CFGR = tmpreg;
}
/**
  * @}
  */

/** @defgroup RCC_Group2 System AHB and APB busses clocks configuration functions
  * @brief   System, AHB and APB busses clocks configuration functions
  *
@verbatim  
 ===============================================================================
     ##### System, AHB and APB busses clocks configuration functions #####
 ===============================================================================

    [..] This section provide functions allowing to configure the System, AHB and 
         APB busses clocks.
         (#) Several clock sources can be used to drive the System clock (SYSCLK): HSI,
             EXT and LSI.
             The AHB clock (HCLK) is derived from System clock through configurable prescaler
             and used to clock the CPU, memory and peripherals mapped on AHB bus .
             and APB (PCLK) clocks are derived from AHB clock through 
             configurable prescalers and used to clock the peripherals mapped on these busses.
             You can use "RCC_GetClocksFreq()" function to retrieve the frequencies of these clocks.

         (#) All the peripheral clocks are derived from the System clock (SYSCLK) except:
             (+@) The ADC clock which is derived from HSI or APB (APB divided by a
                  programmable prescaler: 2 or 4).
             (+@) The I2C clock which is derived from HSI or system clock (SYSCLK).
             (+@) The USART clock which is derived from HSI, system clock (SYSCLK), APB.
             (+@) IWDG clock which is always the LSI clock.
			 (+@) The UART1/2 clock which is derived from HSI, system clock (SYSCLK), APB.
       
         (#) The maximum frequency of the SYSCLK, HCLK and PCLK is 48 MHz for HK32F0301mxxc.
             Depending on the maximum frequency, the FLASH wait states (WS) should be 
             adapted accordingly:
        +--------------------------------------------- +
        |  Wait states  |   HCLK clock frequency (MHz) |
        |---------------|------------------------------|
        |0WS(1CPU cycle)|       0 < HCLK <= 20         |
        |---------------|------------------------------|
        |1WS(2CPU cycle)|       20 < HCLK <= 40        |
        |----------------------------------------------|
		|2WS(2CPU cycle)|       40 < HCLK <= 48        |
		+--------------------------------------------- +

         (#) After reset, the System clock source is the HSI/6 with 0 WS and 
             prefetch is disabled.
  
    [..] It is recommended to use the following software sequences to tune the number
         of wait states needed to access the Flash memory with the CPU frequency (HCLK).
         (+) Increasing the CPU frequency
         (++) Program Flash WS to 1, using "FLASH_SetLatency(FLASH_Latency_1)" function
         (++) Check that the new number of WS is taken into account by reading FLASH_ACR
         (++) Modify the CPU clock source, using "RCC_SYSCLKConfig()" function
         (++) If needed, modify the CPU clock prescaler by using "RCC_HCLKConfig()" function
         (++) Check that the new CPU clock source is taken into account by reading 
              the clock source status, using "RCC_GetSYSCLKSource()" function 
         (+) Decreasing the CPU frequency
         (++) Modify the CPU clock source, using "RCC_SYSCLKConfig()" function
         (++) If needed, modify the CPU clock prescaler by using "RCC_HCLKConfig()" function
         (++) Check that the new CPU clock source is taken into account by reading 
              the clock source status, using "RCC_GetSYSCLKSource()" function
         (++) Program the new number of WS, using "FLASH_SetLatency()" function
         (++) Check that the new number of WS is taken into account by reading FLASH_ACR
         (++) Disable the Flash Prefetch buffer using "FLASH_PrefetchBufferCmd(DISABLE)" 
              function
         (++) Check that Flash Prefetch buffer deactivation is taken into account by reading FLASH_ACR
              using the FLASH_GetPrefetchBufferStatus() function.
  @endverbatim
  *@{
  */

/**
  * @brief  Configures the system clock (SYSCLK).
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         startup from Reset, wake-up from STOP mode.
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after startup delay or PLL locked). 
  *         If a clock source which is not yet ready is selected, the switch will
  *         occur when the clock source will be ready. 
  *         You can use RCC_GetSYSCLKSource() function to know which clock is
  *         currently used as system clock source.  
  * @param  RCC_SYSCLKSource: specifies the clock source used as system clock source 
  *          This parameter can be one of the following values:
  *            @arg RCC_SYSCLKSource_HSI:    HSI selected as system clock source
  *            @arg RCC_SYSCLKSource_EXTCLK:    EXTCLK selected as system clock source
  *            @arg RCC_SYSCLKSource_LSI: LSI selected as system clock source
  * @retval None
  */
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource)
{
  uint32_t tmpreg = 0; 
	uint32_t tmp = 0;
  uint32_t StartUpCounter = 0 ;
	
  
  /* Check the parameters */
  assert_param(IS_RCC_SYSCLK_SOURCE(RCC_SYSCLKSource));
	
	 /* wait for CLK Ready*/
  do{
    switch(RCC_SYSCLKSource)
    {
      case RCC_SYSCLKSource_HSI:
            tmp = RCC->CR & RCC_CR_HSIRDY;
            break;
      case RCC_SYSCLKSource_LSI:
            tmp = RCC->CSR & RCC_CSR_LSIRDY;
            break;
      case RCC_SYSCLKSource_EXTCLK:
            tmp = RCC->CR & RCC_CR_EXTCLKRDY;
            break;
    }
    StartUpCounter++;
  }while((tmp == 0) && (StartUpCounter != 0xFFFF));
  
  tmpreg = RCC->CFGR;
  
  /* Clear SW[1:0] bits */
  tmpreg &= ~RCC_CFGR_SW;
  
  /* Set SW[1:0] bits according to RCC_SYSCLKSource value */
  tmpreg |= RCC_SYSCLKSource;
  
  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Returns the clock source used as system clock.
  * @retval The clock source used as system clock. The returned value can be one 
  *         of the following values:
  *           - 0x00: HSI used as system clock
  *           - 0x04: EXTCLK used as system clock  
  *           - 0x0C: LSI used as system clock
  */
uint8_t RCC_GetSYSCLKSource(void)
{
  return ((uint8_t)(RCC->CFGR & RCC_CFGR_SWS));
}

/**
  * @brief  Configures the AHB clock (HCLK).
  * @param  RCC_SYSCLK: defines the AHB clock divider. This clock is derived from 
  *         the system clock (SYSCLK).
  *          This parameter can be one of the following values:
  *            @arg RCC_SYSCLK_Div1:   AHB clock = SYSCLK
  *            @arg RCC_SYSCLK_Div2:   AHB clock = SYSCLK/2
  *            @arg RCC_SYSCLK_Div4:   AHB clock = SYSCLK/4
  *            @arg RCC_SYSCLK_Div6:   AHB clock = SYSCLK/6  
  *            @arg RCC_SYSCLK_Div8:   AHB clock = SYSCLK/8
  *            @arg RCC_SYSCLK_Div16:  AHB clock = SYSCLK/16
  *            @arg RCC_SYSCLK_Div64:  AHB clock = SYSCLK/64
  *            @arg RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
  *            @arg RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
  *            @arg RCC_SYSCLK_Div512: AHB clock = SYSCLK/512
  * @retval None
  */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_HCLK(RCC_SYSCLK));
  
  tmpreg = RCC->CFGR;
  
  /* Clear HPRE[3:0] bits */
  tmpreg &= ~RCC_CFGR_HPRE;
  
  /* Set HPRE[3:0] bits according to RCC_SYSCLK value */
  tmpreg |= RCC_SYSCLK;
  
  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Configures the APB clock (PCLK).
  * @param  RCC_HCLK: defines the APB clock divider. This clock is derived from 
  *         the AHB clock (HCLK).
  *          This parameter can be one of the following values:
  *            @arg RCC_HCLK_Div1: APB clock = HCLK
  *            @arg RCC_HCLK_Div2: APB clock = HCLK/2
  *            @arg RCC_HCLK_Div4: APB clock = HCLK/4
  *            @arg RCC_HCLK_Div8: APB clock = HCLK/8
  *            @arg RCC_HCLK_Div16: APB clock = HCLK/16
  * @retval None
  */
void RCC_PCLKConfig(uint32_t RCC_HCLK)
{
  uint32_t tmpreg = 0;
  
  /* Check the parameters */
  assert_param(IS_RCC_PCLK(RCC_HCLK));
  
  tmpreg = RCC->CFGR;
  
  /* Clear PPRE[2:0] bits */
  tmpreg &= ~RCC_CFGR_PPRE;
  
  /* Set PPRE[2:0] bits according to RCC_HCLK value */
  tmpreg |= RCC_HCLK;
  
  /* Store the new value */
  RCC->CFGR = tmpreg;
}

/**
  * @brief  Configures the ADC clock (ADCCLK).
  * @param  RCC_ADCCLK: defines the ADC clock source. This clock is derived 
  *         from the HSI or APB clock (PCLK).
  *         This parameter can be one of the following values:
  *            @arg  RCC_ADCCLK_HSI_Div1      
	*			@arg  RCC_ADCCLK_HSI_Div1_5    
	*			@arg  RCC_ADCCLK_HSI_Div2      
	*			@arg  RCC_ADCCLK_HSI_Div2_5    
	*			@arg  RCC_ADCCLK_HSI_Div3      
	*			@arg  RCC_ADCCLK_HSI_Div3_5    
	*			@arg  RCC_ADCCLK_HSI_Div4      
	*			@arg  RCC_ADCCLK_HSI_Div4_5    
	*			@arg  RCC_ADCCLK_HSI_Div5      
	*			@arg  RCC_ADCCLK_HSI_Div5_5    
	*			@arg  RCC_ADCCLK_HSI_Div6      
	*			@arg  RCC_ADCCLK_HSI_Div6_5    
	*			@arg  RCC_ADCCLK_HSI_Div7      
	*			@arg  RCC_ADCCLK_HSI_Div7_5    
	*			@arg  RCC_ADCCLK_HSI_Div8      
	*			@arg  RCC_ADCCLK_HSI_Div8_5    
	*			@arg  RCC_ADCCLK_HSI_Div9      
	*			@arg  RCC_ADCCLK_HSI_Div9_5    
	*			@arg  RCC_ADCCLK_HSI_Div10     
	*			@arg  RCC_ADCCLK_HSI_Div10_5   
	*			@arg  RCC_ADCCLK_HSI_Div11     
	*			@arg  RCC_ADCCLK_HSI_Div11_5   
	*			@arg  RCC_ADCCLK_HSI_Div12     
	*			@arg  RCC_ADCCLK_HSI_Div12_5   
	*			@arg  RCC_ADCCLK_HSI_Div13     
	*			@arg  RCC_ADCCLK_HSI_Div13_5   
	*			@arg  RCC_ADCCLK_HSI_Div14     
	*			@arg  RCC_ADCCLK_HSI_Div14_5   
	*			@arg  RCC_ADCCLK_HSI_Div15     
	*			@arg  RCC_ADCCLK_HSI_Div15_5   
	*			@arg  RCC_ADCCLK_HSI_Div16     
	*			@arg  RCC_ADCCLK_HSI_Div16_5   
  *     @arg  RCC_ADCCLK_PCLK_DIV2
  *     @arg  RCC_ADCCLK_PCLK_DIV4  
  * @retval None
  */
void RCC_ADCCLKConfig(uint32_t RCC_ADCCLK)
{ 
	ADC->CFGR2&= ~ADC_CFGR2_CKMODE;

	if((RCC_ADCCLK==RCC_ADCCLK_PCLK_DIV2)||(RCC_ADCCLK==RCC_ADCCLK_PCLK_DIV4))
	{
		ADC->CFGR2|=RCC_ADCCLK;
	}
	else
	{
		/* Clear ADCPRE bit */
		RCC->CFGR4 &= ~RCC_CFGR4_ADCHSIPRE;
		/* Set ADCPRE bits according to RCC_PCLK value */
		RCC->CFGR4 |= (RCC_ADCCLK & RCC_CFGR4_ADCHSIPRE);
	}
}


/**
  * @brief  Configures the I2C clock (I2CCLK).
  * @param  RCC_I2CCLK: defines the I2C1 clock source. This clock is derived 
  *         from the HSI or System clock.
  *          This parameter can be one of the following values:
  *             @arg RCC_I2C1CLK_HSI: I2C1 clock = HSI
  *             @arg RCC_I2C1CLK_SYSCLK: I2C1 clock = System Clock
  * @retval None
  */
void RCC_I2CCLKConfig(uint32_t RCC_I2CCLK)
{ 
  /* Check the parameters */
  assert_param(IS_RCC_I2CCLK(RCC_I2CCLK));

  /* Clear I2CSW bit */
  RCC->CFGR3 &= ~RCC_CFGR3_I2CSW;
	
  /* Set I2CSW bits according to RCC_I2CCLK value */
  RCC->CFGR3 |= RCC_I2CCLK;
}

/**
  * @brief  Configures the UART1 clock (UART1CLK).
  * @param  RCC_USARTCLK: defines the USART clock source. This clock is derived 
  *         from the HSI or System clock.
  *          This parameter can be one of the following values:
  *             @arg RCC_UART1CLK_PCLK: UART1 clock = APB Clock (PCLK)
  *             @arg RCC_UART1CLK_SYSCLK: UART1 clock = System Clock
  *             @arg RCC_UART1CLK_HSI: UART1 clock = HSI Clock
  * @retval None
  */
void RCC_UART1CLKConfig(uint32_t RCC_USARTCLK)
{ 
	/* Check the parameters */
	assert_param(IS_RCC_UARTCLK(RCC_USARTCLK));

	/* Clear UART1SW[1:0] bit */  
	RCC->CFGR3 &= ~RCC_CFGR3_UART1SW;

	/* Set USARTxSW bits according to RCC_USARTCLK value */
	RCC->CFGR3 |= RCC_USARTCLK;
}

/**
  * @brief  Configures the UART2 clock (UART2CLK).
  * @param  RCC_UART2CLK: defines the UART2 clock source. This clock is derived 
  *         from the HSI or System clock or  PCLK.
  *          This parameter can be one of the following values:
  *             @arg RCC_UART2CLK_PCLK: UART2 clock = APB Clock (PCLK)
  *             @arg RCC_UART2CLK_SYSCLK: UART2 clock = System Clock
  *             @arg RCC_UART2CLK_HSI: UART2 clock = HSI Clock
  * @retval None
  */
void RCC_UART2CLKConfig(uint32_t RCC_UART2CLK)
{ 
	/* Check the parameters */
	assert_param(IS_RCC_UART2CLK(RCC_UART2CLK));

	/* Clear UART1SW[1:0] bit */  
	RCC->CFGR3 &= ~RCC_CFGR3_UART2SW;

	/* Set USARTxSW bits according to RCC_USARTCLK value */
	RCC->CFGR3 |= RCC_UART2CLK;
}

/**
  * @brief  Returns the frequencies of the System, AHB and APB busses clocks.
  * @note    The frequency returned by this function is not the real frequency
  *           in the chip. It is calculated based on the predefined constant and
  *           the source selected by RCC_SYSCLKConfig():
  *                                   
  *             
  * @param  RCC_Clocks: pointer to a RCC_ClocksTypeDef structure which will hold 
  *         the clocks frequencies. 
  *     
  * @note   This function can be used by the user application to compute the 
  *         baudrate for the communication peripherals or configure other parameters.
  * @note   Each time SYSCLK, HCLK and/or PCLK clock changes, this function
  *         must be called to update the structure's field. Otherwise, any
  *         configuration based on this function will be incorrect.
  *    
  * @retval None
  */
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks)
{
	uint32_t tmp = 0, presc = 0 , temp = 0;

	/* Get SYSCLK source -------------------------------------------------------*/
	tmp = RCC->CFGR & RCC_CFGR_SWS;

	switch (tmp)
	{
		case 0x00:  /* HSI used as system clock */
			RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
			break;

		case 0x04:  /* EXTCLK used as system clock */
			RCC_Clocks->SYSCLK_Frequency = EXTCLK_VALUE;
			break;

		case 0x0C:  /* LSI used as system clock */
			RCC_Clocks->SYSCLK_Frequency = LSI_VALUE;
			break;
		default: /* HSI used as system clock */
			RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
			break;
	}
	/* Compute HCLK, PCLK clocks frequencies -----------------------------------*/
	
	/* Get HCLK prescaler */
	tmp = RCC->CFGR & RCC_CFGR_HPRE;
	
	tmp = tmp >> 4;
	
	presc = AHBPrescTable[tmp]; 
	/* HCLK clock frequency */
	RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency/presc;

	/* Get PCLK prescaler */
	tmp = RCC->CFGR & RCC_CFGR_PPRE;
	tmp = tmp >> 8;
	presc = APBPrescTable[tmp];
	/* PCLK clock frequency */
	RCC_Clocks->PCLK_Frequency = RCC_Clocks->HCLK_Frequency >> presc;

	/* ADCCLK clock frequency */
	if((ADC->CFGR2 & ADC_CFGR2_CKMODE) == 0x0)
	{
		/* ADC Clock is HSI Osc. */
	  	tmp = RCC->CFGR4 & RCC_CFGR4_ADCHSIPRE;
	  	tmp = tmp >> 26;
	  	RCC_Clocks->ADCCLK_Frequency = (HSI_VALUE<<1)/(tmp+2);
	}
	else if((ADC->CFGR2 & ADC_CFGR2_CKMODE) == ADC_CFGR2_CKMODE_0)
	{
		/* ADC Clock is derived from PCLK/2 */
		RCC_Clocks->ADCCLK_Frequency = RCC_Clocks->PCLK_Frequency >> 1;
	}
    else if((ADC->CFGR2 & ADC_CFGR2_CKMODE) == ADC_CFGR2_CKMODE_1)
    {
		/* ADC Clock is derived from PCLK/4 */
		RCC_Clocks->ADCCLK_Frequency = RCC_Clocks->PCLK_Frequency >> 2;
    }

	/* I2CCLK clock frequency */
	if((RCC->CFGR4 & RCC_CFGR4_I2CCLK_SEL) == RCC_CFGR4_I2CCLK_SEL)
	{
		/* I2C Clock is PCLK Osc. */
		RCC_Clocks->I2CCLK_Frequency = RCC_Clocks->PCLK_Frequency;
	}
	else if((RCC->CFGR3 & RCC_CFGR3_I2CSW) != RCC_CFGR3_I2CSW)
	{
		/* I2C Clock is HSI Osc. */
		tmp = RCC->CFGR4 & RCC_CFGR4_I2CHSIPRE;
	  RCC_Clocks->I2CCLK_Frequency = (HSI_VALUE<<1)/(tmp+2);;
	}	
	else 
	{
		/* I2C Clock is System Clock */
		RCC_Clocks->I2CCLK_Frequency = RCC_Clocks->SYSCLK_Frequency;
	}
	
  /* UART1CLK clock frequency */
	temp = (RCC->CFGR3 & RCC_CFGR3_UART1SW);

	switch(temp)
  {
    case 0x00:      /* UART1CLK Clock is PCLK Osc. */
      RCC_Clocks->UART1CLK_Frequency = RCC_Clocks->PCLK_Frequency;
      break;
    case 0x01:      /* UART1CLK Clock is SYSCLK Osc. */
      RCC_Clocks->UART1CLK_Frequency = RCC_Clocks->SYSCLK_Frequency;
      break;
    case 0x03:      /* UART1CLK Clock is USARTHSIPRE Clock */
      tmp = RCC->CFGR4 & RCC_CFGR4_UARTHSIPRE;
	    RCC_Clocks->UART1CLK_Frequency = (HSI_VALUE<<1)/(tmp+2);;
      break;
    default:
      RCC_Clocks->UART1CLK_Frequency =  RCC_Clocks->PCLK_Frequency;
      break;
  }
	
	/* UART2CLK clock frequency */
	temp = (RCC->CFGR3 & RCC_CFGR3_UART2SW) >> RCC_CFGR3_UART2SW_Pos;

	switch(temp)
  {
    case 0x00:      /* I2C2 Clock is PCLK Osc. */
      RCC_Clocks->UART2CLK_Frequency = RCC_Clocks->PCLK_Frequency;
      break;
    case 0x01:      /* I2C2 Clock is SYSCLK Osc. */
      RCC_Clocks->UART2CLK_Frequency = RCC_Clocks->SYSCLK_Frequency;
      break;
    case 0x03:      /* I2C2 Clock is USARTHSIPRE Clock */
      tmp = RCC->CFGR4 & RCC_CFGR4_UARTHSIPRE;
	    RCC_Clocks->UART2CLK_Frequency = (HSI_VALUE<<1)/(tmp+2);;
      break;
    default:
      RCC_Clocks->UART2CLK_Frequency =  RCC_Clocks->PCLK_Frequency;
      break;
  }
}

/**
  * @}
  */


/**  @defgroup RCC_Group3 Peripheral clocks configuration functions
 *   @brief Peripheral clocks configuration functions 
 *
 @verbatim
 ===============================================================================
             #####Peripheral clocks configuration functions #####
 ===============================================================================  

    [..] This section provide functions allowing to configure the Peripheral clocks. 
         (#) After restart from Reset , all peripherals are off
             except internal SRAM, Flash and SWD. Before to start using a peripheral you
             have to enable its interface clock. You can do this using RCC_AHBPeriphClockCmd(),
             RCC_APBPeriph2ClockCmd() and RCC_APBPeriph1ClockCmd() functions.
         (#) To reset the peripherals configuration (to the default state after device reset)
             you can use RCC_AHBPeriphResetCmd(), RCC_APBPeriph2ResetCmd() and 
             RCC_APBPeriph1ResetCmd() functions.
 @endverbatim
 * @{
  */

/**
  * @brief  Enables or disables the AHB peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.    
  * @param  RCC_AHBPeriph: specifies the AHB peripheral to gates its clock.
  *          This parameter can be any combination of the following values:
  *             @arg RCC_AHBPeriph_GPIOA: GPIOA clock
  *             @arg RCC_AHBPeriph_GPIOB: GPIOB clock
  *             @arg RCC_AHBPeriph_GPIOC: GPIOC clock
  *             @arg RCC_AHBPeriph_GPIOD: GPIOD clock
  *             @arg RCC_AHBPeriph_CRC:   CRC clock
  *             @arg RCC_AHBPeriph_FLITF: (has effect only when the Flash memory is in power down mode)  
  *             @arg RCC_AHBPeriph_SRAM:  SRAM clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB_PERIPH(RCC_AHBPeriph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    RCC->AHBENR |= RCC_AHBPeriph;
  }
  else
  {
    RCC->AHBENR &= ~RCC_AHBPeriph;
  }
}

/**
  * @brief  Enables or disables the APBPeriph2 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  * @param  RCC_APBPeriph2: specifies the APB periphera2 to gates its clock.
  *          This parameter can be any combination of the following values:
  *             @arg RCC_APBPeriph2_SYSCFG: SYSCFG clock
  *             @arg RCC_APBPeriph2_ADC:    ADC clock
  *             @arg RCC_APBPeriph2_TIM1:   TIM1 clock
  *             @arg RCC_APBPeriph2_SPI:   SPI1 clock
  *             @arg RCC_APBPeriph2_UART1: UART1 clock   
  *             @arg RCC_APBPeriph2_DBGMCU: DBGMCU clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APBPeriph2ClockCmd(uint32_t RCC_APBPeriph2, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB_PERIPH2(RCC_APBPeriph2));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APBENR2 |= RCC_APBPeriph2;
  }
  else
  {
    RCC->APBENR2 &= ~RCC_APBPeriph2;
  }
}

/**
  * @brief  Enables or disables the APBPeriph1 peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.
  * @param  RCC_APBPeriph1: specifies the APBperipheral1 to gates its clock.
  *          This parameter can be any combination of the following values:
  *           @arg RCC_APBPeriph1_TIM2:   TIM2 clock  
  *           @arg RCC_APBPeriph1_TIM6:   TIM6 clock
  *           @arg RCC_APBPeriph1_WWDG:   WWDG clock
  *           @arg RCC_APBPeriph1_AWU:    AWU clock    
  *           @arg RCC_APBPeriph1_UART2:  UART2 clock
  *			      @arg RCC_APBPeriph1_UART3:  UART3 clock
  *			      @arg RCC_APBPeriph1_UART4:  UART4 clock	
  *           @arg RCC_APBPeriph1_I2C:   I2C1 clock
  *           @arg RCC_APBPeriph1_PWR:    PWR clock
  *           @arg RCC_APBPeriph1_BEEPER: BEEPER clock
  *           @arg RCC_APBPeriph1_IOMUX:  IOMUX  clock                  
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APBPeriph1ClockCmd(uint32_t RCC_APBPeriph1, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB_PERIPH1(RCC_APBPeriph1));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APBENR1 |= RCC_APBPeriph1;
  }
  else
  {
    RCC->APBENR1 &= ~RCC_APBPeriph1;
  }
}

/**
  * @brief  Forces or releases AHB peripheral reset.
  * @param  RCC_AHBPeriph: specifies the AHB peripheral to reset.
  *          This parameter can be any combination of the following values:
  *             @arg RCC_AHBRSTR_GPIOARST: GPIOA clock
  *             @arg RCC_AHBRSTR_GPIOBRST: GPIOB clock
  *             @arg RCC_AHBRSTR_GPIOCRST: GPIOC clock
  *             @arg RCC_AHBRSTR_GPIODRST: GPIOD clock
  *             @arg RCC_AHBRSTR_CRCRST:    CRC clock
  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_AHBPeriphResetCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_AHB_RST_PERIPH(RCC_AHBPeriph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->AHBRSTR |= RCC_AHBPeriph;
  }
  else
  {
    RCC->AHBRSTR &= ~RCC_AHBPeriph;
  }
}

/**
  * @brief  Forces or releases APB peripheral2 reset.
  * @param  RCC_APBPeriph2: specifies the APB peripheral2 to reset.
  *          This parameter can be any combination of the following values:
  *             @arg RCC_APBPeriph2_SYSCFG: SYSCFG clock
  *             @arg RCC_APBPeriph2_ADC:    ADC clock
  *             @arg RCC_APBPeriph2_TIM1:   TIM1 clock
  *             @arg RCC_APBPeriph2_SPI:   SPI1 clock
  *             @arg RCC_APBPeriph2_UART1: UART1 clock   
  *             @arg RCC_APBPeriph2_DBGMCU: DBGMCU clock

  * @param  NewState: new state of the specified peripheral reset.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APBPeriph2ResetCmd(uint32_t RCC_APBPeriph2, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB_PERIPH2(RCC_APBPeriph2));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APBRSTR2 |= RCC_APBPeriph2;
  }
  else
  {
    RCC->APBRSTR2 &= ~RCC_APBPeriph2;
  }
}

/**
  * @brief  Forces or releases APB peripheral1 reset.
  * @param  RCC_APBPeriph1: specifies the APB peripheral1 to reset.
  *          This parameter can be any combination of the following values:
  *           @arg RCC_APBPeriph1_TIM2:   TIM2 clock  
  *           @arg RCC_APBPeriph1_TIM6:   TIM6 clock
  *           @arg RCC_APBPeriph1_WWDG:   WWDG clock
  *           @arg RCC_APBPeriph1_AWU:   AWU clock  
  *           @arg RCC_APBPeriph1_UART2: UART2 clock  
  *           @arg RCC_APBPeriph1_UART2: UART3 clock 
  *           @arg RCC_APBPeriph1_UART2: UART4 clock 
  *           @arg RCC_APBPeriph1_I2C:   I2C1 clock
  *           @arg RCC_APBPeriph1_PWR:    PWR clock
  *           @arg RCC_APBPeriph1_BEEPER:   BEEPER clock
  *           @arg RCC_APBPeriph1_IOMUX:     IOMUX  clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_APBPeriph1ResetCmd(uint32_t RCC_APBPeriph1, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_RCC_APB_PERIPH1(RCC_APBPeriph1));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    RCC->APBRSTR1 |= RCC_APBPeriph1;
  }
  else
  {
    RCC->APBRSTR1 &= ~RCC_APBPeriph1;
  }
}
/**
  * @}
  */

/**@defgroup  RCC_Group4 Interrupts and flags management functions
 * @brief    Interrupts and flags management functions 
 *
@verbatim
 ===============================================================================
             ##### Interrupts and flags management functions #####
 ===============================================================================
 @endverbatim
  * @{
 */

/**
  * @brief  Enables or disables the specified RCC interrupts.
  * @note   The CSS interrupt doesn't have an enable bit; once the CSS is enabled
  *         and if the EXT clock fails, the CSS interrupt occurs and an NMI is
  *         automatically generated. The NMI will be executed indefinitely, and 
  *         since NMI has higher priority than any other IRQ (and main program)
  *         the application will be stacked in the NMI ISR unless the CSS interrupt
  *         pending bit is cleared.
  * @param  RCC_IT: specifies the RCC interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *              @arg RCC_IT_LSIRDY: LSI ready interrupt
  *              @arg RCC_IT_HSIRDY: HSI ready interrupt
  *              @arg RCC_IT_EXTRDY: EXT ready interrupt
  *              @arg RCC_IT_CSS: CSS interrupt
  * @param  NewState: new state of the specified RCC interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));
  
  if (NewState != DISABLE)
  {
    /* Perform Byte access to RCC_CIR[13:8] bits to enable the selected interrupts */
    *(__IO uint8_t *) CIR_BYTE1_ADDRESS |= RCC_IT;
  }
  else
  {
    /* Perform Byte access to RCC_CIR[13:8] bits to disable the selected interrupts */
    *(__IO uint8_t *) CIR_BYTE1_ADDRESS &= (uint8_t)~RCC_IT;
  }
}

/**
  * @brief  Checks whether the specified RCC flag is set or not.
  * @param  RCC_FLAG: specifies the flag to check.
  *          This parameter can be one of the following values:
  *            ** CR **
  *           RCC_FLAG_HSIRDY 
  *			      RCC_FLAG_EXTCLKRDY
  *           ** CSR **
  *		        RCC_FLAG_LSIRDY
  *		        ** CSR **
   *          RCC_FLAG_PINRST	
   *          RCC_FLAG_PORRST	
   *          RCC_FLAG_SFTRST	
   *          RCC_FLAG_IWDGRST	
   *          RCC_FLAG_WWDGRST	
   *          RCC_FLAG_LPWRRST	
  * @retval The new state of RCC_FLAG (SET or RESET).
  */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
  uint32_t tmp = 0;
  uint32_t statusreg = 0;
  FlagStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_RCC_FLAG(RCC_FLAG));

  /* Get the RCC register index */
  tmp = RCC_FLAG >> 5;

  if (tmp == 1)               /* The flag to check is in CR register */
  {
    statusreg = RCC->CR;
  }
  else if (tmp == 3)          /* The flag to check is in CSR register */
  {
    statusreg = RCC->CSR;
  }
  
  /* Get the flag position */
  tmp = RCC_FLAG & FLAG_MASK;

  if ((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
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
  * @brief  Clears the RCC reset flags.
  *         The reset flags are: 
            RCC_FLAG_OBLRST, 
			RCC_FLAG_PINRST, 
			RCC_FLAG_V18PWRRSTF,
  *         RCC_FLAG_PORRST, 
            RCC_FLAG_SFTRST, 
			RCC_FLAG_IWDGRST, 
			RCC_FLAG_WWDGRST,
  *         RCC_FLAG_LPWRRST. 
  * @retval None
  */
void RCC_ClearFlag(void)
{
  /* Set RMVF bit to clear the reset flags */
  RCC->CSR |= RCC_CSR_RMVF;
}

/**
  * @brief  Checks whether the specified RCC interrupt has occurred or not.
  * @param  RCC_IT: specifies the RCC interrupt source to check.
  *          This parameter can be one of the following values:
  *              @arg RCC_IT_LSIRDY: LSI ready interrupt
  *              @arg RCC_IT_HSIRDY: HSI ready interrupt
  *              @arg RCC_IT_EXTRDY: EXT ready interrupt
  *              @arg RCC_IT_CSS: CSS interrupt
  * @retval The new state of RCC_IT (SET or RESET).
  */
ITStatus RCC_GetITStatus(uint8_t RCC_IT)
{
  ITStatus bitstatus = RESET;
  
  /* Check the parameters */
  assert_param(IS_RCC_GET_IT(RCC_IT));
  
  /* Check the status of the specified RCC interrupt */
  if ((RCC->CIR & RCC_IT) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the RCC_IT status */
  return  bitstatus;
}

/**
  * @brief  Clears the RCC's interrupt pending bits.
  * @param  RCC_IT: specifies the interrupt pending bit to clear.
  *          This parameter can be any combination of the following values:
  *              @arg RCC_IT_LSIRDY: LSI ready interrupt
  *              @arg RCC_IT_HSIRDY: HSI ready interrupt
  *              @arg RCC_IT_EXTRDY: EXT ready interrupt
  *              @arg RCC_IT_CSS: CSS interrupt
  * @retval None
  */
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
  /* Check the parameters */
  assert_param(IS_RCC_CLEAR_IT(RCC_IT));
  
  /* Perform Byte access to RCC_CIR[23:16] bits to clear the selected interrupt
     pending bits */
  *(__IO uint8_t *) CIR_BYTE2_ADDRESS = RCC_IT;
}

/**
  * @brief  Adjusts the CSS counter threshold control.
  * @note   After the CSS function is enabled, RCC uses the HSI clock to sample the EXTCLK frequency division waveform.
	*					If the FREQUENCY of EXTCLK input is very low, CSS outages can be triggered even when the EXTCLK is still working.
  *         You can adjust the value through CSS_THRESHOLD[6:0] to avoid. When different thresholds are configured, 
  *         the lowest EXTCLK frequency determined by the CSS is about 16M/ CSS_THRESHOLD[6:0].  
  * @param  CSS_THRESHOLD_Value: specifies the CSS counter threshold control value.
  *         This parameter must be a number between 0 and 0x3F.
  * @retval None
  */
void RCC_CSS_THRESHOLD_Config(uint8_t CSS_THRESHOLD_Value)
{
	uint32_t tmpreg = 0;
	
	assert_param(IS_CSS_THRESHOLD_Value(CSS_THRESHOLD_Value));
	
	tmpreg = RCC->CSS ;
	
	tmpreg &= ~RCC_CSS_CSS_THRESHOLD;
	
	
  tmpreg |= (uint32_t)CSS_THRESHOLD_Value << 25 ;
	
	RCC->CSS = tmpreg;

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
