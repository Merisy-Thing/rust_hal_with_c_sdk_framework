/********************************************************************
* @copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
* @filename: hk32f0301mxxc_awut.c
* @brief: //本文件功能简要描述
* @author: AE Team
* @version: v1.0.0 2022-9-9 //首个版本从V1.0.0开始完成日期
* @log: //修改历史记录列表，修改记录包括修改日期、及修改内容简述。
********************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_awut.h"

/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

  /** @defgroup AWUT  AWUT
  * @brief AWUTS driver modules
  * @{
  */

/* Public functions ----------------------------------------------------------*/

/**
  * @defgroup AWUT_Public_Functions AWUT_Public_Functions
  * @{
  */

/**
  * @brief  Deinitializes the AWUT peripheral registers to their default reset
  * values.
  * @note   None
  * @retval None
  */
void AWUT_DeInit(void)
{
    AWUT->CR = AWUT_CR_RESET_VALUE;
}

/**
  * @brief  config the AWUT clock
  * @param   eAWUT_CLK :
  *  arg AWUT_CLK_LSI128
  *  arg WUT_CLK_HSE
  * @retval None
  * @par Required preconditions:
  * The LS RC calibration must be performed before calling this function.
  */
void AWUT_CLKConfig(AWUT_CLK_TYPE eAWUT_CLK)
{
    uint32_t temp = 0;
    /* Check parameter */
    assert_param(IS_AWUT_CLK(eAWUT_CLK));

    temp =  AWUT->CR;
    /*clear Bit AWUT_CKSEL*/
    temp &= 0xFFFFFFFE;
    /* config AWUT timer clk*/
    temp |= eAWUT_CLK;
    /*set the register*/
    AWUT->CR |= temp;
}

/**
  * @brief  loade the AWUT timer counter,This load value will be automatically loaded into the 22-bit timer inside the AWUT
  *  when the mcu enters stop mode and start timing.
  * @param   TimerCounter : the AWUT timer counter
  * @note When AWUT_rlr [22:1] is '0' or is '1' , the loading behavior will not occur and AWUT will not start working .
  *       when AWUT_wbusy =1 ,the write operation on the AWUT-rlr register will be invalid.
  * @return ErrorStatus: the AWUT result
  *       SUCCESS:AWUT timer start success
  *       ERROR拢潞AWUT timer start error
  */
ErrorStatus AWUT_TimerCounterAndStart(uint32_t TimerCounter)
{
    uint32_t temp = 0;
    uint32_t TimeoutCnt = 0;
    while (TimeoutCnt ++ <= 0x0fff)
    {
      //  AWUT_APB bus is idle
      if ((AWUT->CR & 0x80000000) == 0x00000000)
      {
        temp = AWUT->CR;
        temp &= 0xFF800001;
        temp |= ( TimerCounter << 1);
        AWUT->CR |= temp;
        return SUCCESS;
      }
      else
      {
        /* when AWUT_wbusy =1 ,the write operation on the AWUT-rlr register will be invalid.*/
      }
    }

    return ERROR;

}

/**
  * @brief  Returns status of the AWUT peripheral flag.
  * @note  None
  * @retval FlagStatus : Status of the AWUT flag.
  */
FlagStatus AWUT_GetFlagStatus(void)
{
    return((FlagStatus)(((uint8_t)(AWUT->SR & AWUT_SR_BUSY) == (uint8_t)0x00) ? RESET : SET));
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
