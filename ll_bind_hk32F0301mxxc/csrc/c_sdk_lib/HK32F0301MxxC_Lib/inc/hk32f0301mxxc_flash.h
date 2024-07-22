/********************************************************************************
  * @file    hk32f0301mxxc_flash.h
  * @author  AE Team
  * @version V1.0.0/2022-09-14
  *     1.初始版本
  * @log  V1.0.0 
  *******************************************************************************/ 


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_FLASH_H
#define __HK32F0301MXXC_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc.h"


/** @addtogroup FLASH
	* @{
	*/

/** @defgroup FLASH_Exported_Types FLASH_Exported_Types
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/** FLASH Status  */ 
typedef enum
{
  FLASH_BUSY = 1,
  FLASH_ERROR_WRP,
  FLASH_ERROR_PROGRAM,
  FLASH_COMPLETE,
  FLASH_TIMEOUT
}FLASH_Status;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup FLASH_Exported_Constants FLASH_Exported_Constants
  * @{
  */

/** @defgroup FLASH_Latency FLASH_Latency
  * @{
  */

/** FLASH_Latency   */ 
#define FLASH_Latency_0                ((uint32_t)0x00000000)      /*HCLK=16Mhz*/
#define FLASH_Latency_1                ((uint32_t)0x00000001)      /*16Mhz<HCLK<=32Mhz*/
#define FLASH_Latency_2                ((uint32_t)0x00000002)      /*32Mhz<HCLK<=48Mhz*/
#define FLASH_Latency_3                ((uint32_t)0x00000003)     


#define IS_FLASH_LATENCY(LATENCY) (((LATENCY) == FLASH_Latency_0) || \
								   ((LATENCY) == FLASH_Latency_1) || \
								   ((LATENCY) == FLASH_Latency_2) || \
                                   ((LATENCY) == FLASH_Latency_3))

/**
  * @}
  */
/** @defgroup FLASH_Interrupts FLASH_Interrupts
  * @{
  */
/**  FLASH_Interrupts   */   
#define FLASH_IT_EOP                   FLASH_CR_EOPIE  /*!< End of programming interrupt source */
#define FLASH_IT_ERR                   FLASH_CR_ERRIE  /*!< Error interrupt source */
#define IS_FLASH_IT(IT) ((((IT) & (uint32_t)0xFFFFEBFF) == 0x00000000) && (((IT) != 0x00000000)))
/**
  * @}
  */
  
/** @defgroup FLASH_Address FLASH_Address
  * @{
  */
/*  FLASH_Address  16K devices */
#define IS_FLASH_PROGRAM_ADDRESS(ADDRESS) (((ADDRESS) >= 0x08000000) && ((ADDRESS) <= 0x08003FFF))

#define FLASH_OB_RDP_ADDRESS 		    0x1FFFF000
#define FLASH_OB_USER_ADDRESS 		    0x1FFFF002
#define FLASH_OB_DATA0_ADDRESS 		    0x1FFFF004
#define FLASH_OB_DATA1_ADDRESS 		    0x1FFFF006
#define FLASH_OB_WRP_ADDRESS 		    0x1FFFF008
#define FLASH_OB_IWDG_ADDRESS 		    0x1FFFF010
#define FLASH_OB_LSI_LP_ADDRESS 	    0x1FFFF014
#define FLASH_OB_DBG_CLK_ADDRESS 	    0x1FFFF016
#define FLASH_OB_BOR_LVL_ADDRESS 	    0x1FFFF018
#define FLASH_OB_NRST_SEL_KEY_ADDRESS 	0x1FFFF01A

/**
  * @}
  */
  
/** @defgroup FLASH_Option_Bytes_Write_Protection FLASH_Option_Bytes_Write_Protection
  * @{
  */
/**  FLASH_Option_Bytes_Write_Protection   */
#define OB_WRP_Pages0to1               ((uint32_t)0x00000001) /* Write protection of page 0 to 1 */ 
#define OB_WRP_Pages2to3               ((uint32_t)0x00000002) /* Write protection of page 2 to 3 */ 
#define OB_WRP_Pages4to5               ((uint32_t)0x00000004) /* Write protection of page 4 to 5 */ 
#define OB_WRP_Pages6to7               ((uint32_t)0x00000008) /* Write protection of page 6 to 7 */ 
#define OB_WRP_Pages8to9               ((uint32_t)0x00000010) /* Write protection of page 8 to 9 */ 
#define OB_WRP_Pages10to11             ((uint32_t)0x00000020) /* Write protection of page 10 to 11 */ 
#define OB_WRP_Pages12to13             ((uint32_t)0x00000040) /* Write protection of page 12 to 13 */ 
#define OB_WRP_Pages14to15             ((uint32_t)0x00000080) /* Write protection of page 14 to 15 */ 
#define OB_WRP_Pages16to17             ((uint32_t)0x00000100) /* Write protection of page 16 to 17 */ 
#define OB_WRP_Pages18to19             ((uint32_t)0x00000200) /* Write protection of page 18 to 19 */ 
#define OB_WRP_Pages20to21             ((uint32_t)0x00000400) /* Write protection of page 20 to 21 */ 
#define OB_WRP_Pages22to23             ((uint32_t)0x00000800) /* Write protection of page 22 to 23 */ 
#define OB_WRP_Pages24to25             ((uint32_t)0x00001000) /* Write protection of page 24 to 25 */ 
#define OB_WRP_Pages26to27             ((uint32_t)0x00002000) /* Write protection of page 26 to 27*/ 
#define OB_WRP_Pages28to29             ((uint32_t)0x00004000) /* Write protection of page 28 to 29 */ 
#define OB_WRP_Pages30to31             ((uint32_t)0x00008000) /* Write protection of page 30 to 31 */  
#define OB_WRP_Pages32to33             ((uint32_t)0x00010000) /* Write protection of page 32 to 33*/ 
#define OB_WRP_Pages34to35             ((uint32_t)0x00020000) /* Write protection of page 34 to 35 */ 
#define OB_WRP_Pages36to37             ((uint32_t)0x00040000) /* Write protection of page 36 to 37 */ 
#define OB_WRP_Pages38to39             ((uint32_t)0x00080000) /* Write protection of page 38 to 39 */ 
#define OB_WRP_Pages40to41             ((uint32_t)0x00100000) /* Write protection of page 40 to 41 */ 
#define OB_WRP_Pages42to43             ((uint32_t)0x00200000) /* Write protection of page 42 to 43 */ 
#define OB_WRP_Pages44to45             ((uint32_t)0x00400000) /* Write protection of page 44 to 45 */ 
#define OB_WRP_Pages46to47             ((uint32_t)0x00800000) /* Write protection of page 46 to 47 */ 
#define OB_WRP_Pages48to49             ((uint32_t)0x01000000) /* Write protection of page 48 to 49 */ 
#define OB_WRP_Pages50to51             ((uint32_t)0x02000000) /* Write protection of page 50 to 51 */ 
#define OB_WRP_Pages52to53             ((uint32_t)0x04000000) /* Write protection of page 52 to 53 */ 
#define OB_WRP_Pages54to55             ((uint32_t)0x08000000) /* Write protection of page 54 to 55 */ 
#define OB_WRP_Pages56to57             ((uint32_t)0x10000000) /* Write protection of page 56 to 57 */ 
#define OB_WRP_Pages58to59             ((uint32_t)0x20000000) /* Write protection of page 58 to 59*/ 
#define OB_WRP_Pages60to61             ((uint32_t)0x40000000) /* Write protection of page 60 to 61 */ 
#define OB_WRP_Pages62to63             ((uint32_t)0x80000000) /* Write protection of page 62 to 63 */  

#define OB_WRP_AllPages                ((uint32_t)0xFFFFFFFF) /*!< Write protection of all Sectors */
 
 #define IS_OB_WRP(PAGE) (((PAGE) != 0x0000000))
/**
  * @}
  */
 
/** @defgroup FLASH_Option_Bytes_Read_Protection FLASH_Option_Bytes_Read_Protection
  * @{
  */
/**  FLASH_Option_Bytes_Read_Protection   */ 

/**  FLASH_Read Protection Level   */ 
#define OB_RDP_Level_0   ((uint16_t)0x55aa)
#define OB_RDP_Level_1   ((uint16_t)0x44bb)
/*#define OB_RDP_Level_2   ((uint16_t)0x33cc)*/ /* Warning: When enabling read protection level 2 
                                                it's no more possible to go back to level 1 or 0 */

#define IS_OB_RDP(LEVEL) (((LEVEL) == OB_RDP_Level_0)||\
                          ((LEVEL) == OB_RDP_Level_1))/*||\
                          ((LEVEL) == OB_RDP_Level_2))*/				  
/**
  * @}
  */
  
/** @defgroup FLASH_Option_Bytes_IWatchdog FLASH_Option_Bytes_IWatchdog
  * @{
  */
/**  FLASH_Option_Bytes_IWatchdog   */

#define OB_IWDG_SW                     ((uint8_t)0x01)  /*!< Software IWDG selected */
#define OB_IWDG_HW                     ((uint8_t)0x00)  /*!< Hardware IWDG selected */
#define IS_OB_IWDG_SOURCE(SOURCE) (((SOURCE) == OB_IWDG_SW) || ((SOURCE) == OB_IWDG_HW))

/**
  * @}
  */
/** @defgroup FLASH_Option_Bytes_nRST_STOP FLASH_Option_Bytes_nRST_STOP
  * @{
  */
/** defgroup FLASH_Option_Bytes_nRST_STOP   */

#define OB_STOP_NoRST                  ((uint8_t)0x02) /*!< No reset generated when entering in STOP */
#define OB_STOP_RST                    ((uint8_t)0x00) /*!< Reset generated when entering in STOP */
#define IS_OB_STOP_SOURCE(SOURCE) (((SOURCE) == OB_STOP_NoRST) || ((SOURCE) == OB_STOP_RST))

/**
  * @}
  */
  
  /**
  * @}
  */
 
#define OB_BOR_LEVEL_DISABLE    (0x00)
#define OB_BOR_LEVEL_1          (0x09)
#define OB_BOR_LEVEL_2          (0x0A)
#define OB_BOR_LEVEL_3          (0x0B)
#define OB_BOR_LEVEL_4          (0x0C)
#define OB_BOR_LEVEL_5          (0x0D)
#define OB_BOR_LEVEL_6          (0x0E)
#define OB_BOR_LEVEL_7          (0x0F) 
  /** @defgroup FLASH_ADDRESS FLASH_ADDRESS
  * @{
  */
 /**
  * @brief  Macro used by the assert function in order to check the different 
  *         sensitivity values for the option bytes Address
  */

#define OPTION_BYTE_START_DATA1_ADDRESS  ((uint32_t)0x1FFFF000)
#define OPTION_BYTE_END_DATA1_ADDRESS  ((uint32_t)0x1FFFF01C)

#define IS_OB_DATA_ADDRESS(ADDRESS)  (((ADDRESS) >= OPTION_BYTE_START_DATA1_ADDRESS) && \
    ((ADDRESS) <= OPTION_BYTE_END_DATA1_ADDRESS))
  /**
  * @}
  */
 
  /** @defgroup FLASH_Flags FLASH_Flags
  * @{
  */
/**  FLASH_Flags   */ 

#define FLASH_FLAG_BSY                 FLASH_SR_BSY        /*!< FLASH Busy flag */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPRTERR  /*!< FLASH Write protected error flag */
#define FLASH_FLAG_PGERR               FLASH_SR_PGERR     /*!< FLASH program error flag */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP       /*!< FLASH End of Programming flag */
 
#define IS_FLASH_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0xFFFFFFCA) == 0x00000000) && ((FLAG) != 0x00000000))
#define IS_FLASH_GET_FLAG(FLAG)  (((FLAG) == FLASH_FLAG_BSY) || ((FLAG) == FLASH_FLAG_WRPERR) || ((FLAG) == FLASH_FLAG_EOP)||((FLAG) == FLASH_FLAG_PGERR))
/**
  * @}
  */
  
/** @defgroup FLASH_Timeout_definition FLASH_Timeout_definition
  * @{
  */
/** FLASH_Timeout_definition   */ 
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x000FFFFF)
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
  
 
/* FLASH Interface configuration functions ************************************/
void FLASH_SetLatency(uint32_t FLASH_Latency);


/* FLASH Memory Programming functions *****************************************/
void FLASH_Unlock(void);
void FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);
/* FLASH Option Bytes Programming functions *****************************************/
void FLASH_OB_Unlock(void);
void FLASH_OB_Lock(void);
FLASH_Status FLASH_OB_Erase(void);

FLASH_Status FLASH_OB_WRPConfig(uint32_t OB_WRP);
FLASH_Status FLASH_OB_RDPConfig(uint16_t OB_RDP);
FLASH_Status FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP);
FLASH_Status FLASH_OB_WriteUser(uint16_t OB_USER);
FLASH_Status FLASH_OB_ProgramData(uint32_t Address, uint32_t Data);
uint8_t FLASH_OB_GetUser(void);
uint32_t FLASH_OB_GetWRP(void);
FlagStatus FLASH_OB_GetRDP(void);

/* FLASH Interrupts and flags management functions **********************************/
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);


void Sys_GetDevice96BitUID(uint32_t *UID);

#ifdef __cplusplus
}
#endif

#endif /* __HK32F0301MXXC_FLASH_H */


