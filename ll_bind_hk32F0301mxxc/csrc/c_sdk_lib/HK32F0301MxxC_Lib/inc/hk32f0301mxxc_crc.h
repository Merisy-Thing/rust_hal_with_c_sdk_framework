/********************************************************************
* @Copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd 
* @Filename: hk32f0301mxxc_crc.h
* @brief  : This file contains all the functions prototypes for the CRC firmware 
*          library  
* @Author : AE Team
********************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HK32F0301MXXC_CRC_H
#define __HK32F0301MXXC_CRC_H

#ifdef __cplusplus
 extern "C" {
#endif

/*!< Includes ----------------------------------------------------------------*/
#include "hk32f0301mxxc.h"


/** @addtogroup CRC
  * @{
  */
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup CRC_Exported_Constants CRC_Exported_Constants
  * @{
  */ 


/** @defgroup CRC_ReverseInputData CRC_ReverseInputData
  * @{
  */
 
#define CRC_ReverseInputData_No             ((uint32_t)0x00000000) /*!< No reverse operation of Input Data */
#define CRC_ReverseInputData_8bits          CRC_CR_REV_IN_0        /*!< Reverse operation of Input Data on 8 bits */
#define CRC_ReverseInputData_16bits         CRC_CR_REV_IN_1        /*!< Reverse operation of Input Data on 16 bits */
#define CRC_ReverseInputData_32bits         CRC_CR_REV_IN          /*!< Reverse operation of Input Data on 32 bits */

#define IS_CRC_REVERSE_INPUT_DATA(DATA) (((DATA) == CRC_ReverseInputData_No)     || \
                                         ((DATA) == CRC_ReverseInputData_8bits)  || \
                                         ((DATA) == CRC_ReverseInputData_16bits) || \
                                         ((DATA) == CRC_ReverseInputData_32bits))
/**
  * @}
  */


/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* Configuration of the CRC computation unit **********************************/
void CRC_DeInit(void);
void CRC_ResetDR(void);
void CRC_ReverseInputDataSelect(uint32_t CRC_ReverseInputData);
void CRC_ReverseOutputDataCmd(FunctionalState NewState);
void CRC_SetInitRegister(uint32_t CRC_InitValue); 

/* CRC computation ************************************************************/
uint32_t CRC_CalcCRC(uint32_t CRC_Data);
uint32_t CRC_CalcCRC16bits(uint16_t CRC_Data);
uint32_t CRC_CalcCRC8bits(uint8_t CRC_Data);
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength);
uint32_t CRC_GetCRC(void);

/* Independent register (IDR) access (write/read) *****************************/
void CRC_SetIDRegister(uint8_t CRC_IDValue);
uint8_t CRC_GetIDRegister(void);

#ifdef __cplusplus
}
#endif

#endif /* __HK32S003C_CRC_H */

/**
  * @}
  */ 

/**
  * @}
  */
/**
  * @}
  */ 



