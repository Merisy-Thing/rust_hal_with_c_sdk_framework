/**
  ******************************************************************************
  * @file    hk32f0301mxxc_conf_Template.h
  * @brief   configuration file.
  ******************************************************************************
  * @attention 
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_CONF_H
#define __HK32F0301MXXC_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/*system clock source*/
#define SYSCLK_SRC_HSI   			0x4
#define SYSCLK_SRC_LSI      	    0x5
#define SYSCLK_SCR_EXTCLK_IO         0x6

#define SYSCLK_SOURCE SYSCLK_SRC_HSI

// #define VECT_TAB_SRAM
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field. This value must be a multiple of 0x200. */
 

/* ########################## HSE/HSI Values adaptation ##################### */
/**
  * @brief Adjust the value of External High Speed oscillator (HSE) used in your application.
  *        This value is used by the RCC  module to compute the system frequency
  *        (when HSE is used as system clock source, directly or through the PLL).  
  */
 


#define EXTCLK_VALUE	((uint32_t)32000000) /*!< Value of the Internal oscillator in Hz*/

 
#define HSI_VALUE    ((uint32_t)48000000) /*!< Value of the Internal oscillator in Hz*/



/**
  * @brief In the following line adjust the Internal High Speed oscillator (HSI) Startup 
  *        Timeout value 
  */

 #define STARTUP_TIMEOUT   ((uint32_t)0xFFFF) /*!< Time out for start up */
 

/**
  * @brief Internal Low Speed oscillator (LSI) value.
  */

 #define LSI_VALUE  ((uint32_t)60000)     
  /*!< Value of the Internal Low Speed oscillator in Hz
    The real value may vary depending on the variations*/


/* Includes ------------------------------------------------------------------*/
/**
  * @brief Include module's header file 
  */

 #include "hk32f0301mxxc_rcc.h"

 #include "hk32f0301mxxc_crc.h"

 #include "hk32f0301mxxc_exti.h"

 #include "hk32f0301mxxc_flash.h"

 #include "hk32f0301mxxc_gpio.h"

 #include "hk32f0301mxxc_misc.h"

 #include "hk32f0301mxxc_adc.h"

 #include "hk32f0301mxxc_syscfg.h"

 #include "hk32f0301mxxc_def.h"

 #include "hk32f0301mxxc_i2c.h"

 #include "hk32f0301mxxc_iwdg.h"

 #include "hk32f0301mxxc_pwr.h"

 #include "hk32f0301mxxc_spi.h"

 #include "hk32f0301mxxc_tim.h"

 #include "hk32f0301mxxc_uart.h"

 #include "hk32f0301mxxc_wwdg.h"

 #include "hk32f0301mxxc_awut.h"
   
 #include "hk32f0301mxxc_dbgmcu.h"
/* Exported macro ------------------------------------------------------------*/
/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *         drivers code
  */

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed. 
  *         If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0U : assert_failed((char *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(char* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */    
    
#ifdef __cplusplus
}
#endif

#endif /* __HK32F0301MXXC_CONF_H */

/************************ (C) COPYRIGHT MKMcircoChuip *****END OF FILE****/
