/**
	******************************************************************************
	* @file    system_hk32f0301mxxc.h
	* @author  AE Team
  * @version V1.0.0/2022-04-14
  *     1.初始版本
  * @log  V1.0.0
	******************************************************************************
*/ 


#ifndef __SYSTEM_HK32F0301MXXC_H
#define __SYSTEM_HK32F0301MXXC_H

#ifdef __cplusplus
 extern "C" {
#endif 

 

/** hk32f0301mxxC_System_Exported_types  */

extern uint32_t SystemCoreClock;          /*!< System Clock Frequency (Core Clock) */
extern const uint16_t AHBPrescTable[16];   /*!< AHB prescalers table values */
extern const uint8_t APBPrescTable[8];    /*!< APB prescalers table values */


extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
#ifdef __cplusplus
}
#endif

#endif /*__SYSTEM_HK32F0301MXXC_H */


