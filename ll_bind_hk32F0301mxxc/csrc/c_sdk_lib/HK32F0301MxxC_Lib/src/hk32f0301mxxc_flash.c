/********************************************************************************
  * @file    hk32f0301mxxc_flash.c
  * @author  AE Team
  * @version V1.0.0/2022-09-14
  *     1.初始版本
  * @log  V1.0.0
  *******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "hk32f0301mxxc_flash.h"


/** @addtogroup HK32F0301MxxC_StdPeriph_Driver
  * @{
  */

/** @defgroup FLASH FLASH
  * @brief FLASH driver modules
  * @{
  */  
 
/** @defgroup FLASH_Private_Functions FLASH_Private_Functions
  * @{
  */ 

/** @defgroup FLASH_Group1 FLASH Interface configuration functions
  *  @brief   FLASH Interface configuration functions 
 *
 
 ===============================================================================
               ##### FLASH Interface configuration functions #####
 ===============================================================================

    [..] FLASH_Interface configuration_Functions, includes the following functions:
       (+) void FLASH_SetLatency(uint32_t FLASH_Latency):
    [..] To correctly read data from Flash memory, the number of wait states (LATENCY)
     must be correctly programmed according to the frequency of the CPU clock (HCLK)
    [..]
        +--------------------------------------------- +
        |  Wait states  |   HCLK clock frequency (MHz) |
        |---------------|------------------------------|
        |0WS(1CPU cycle)|       0 < HCLK <= 20         |
        |---------------|------------------------------|
        |1WS(2CPU cycle)|       20 < HCLK <= 40        |
        |---------------|------------------------------|
        |2WS(3CPU cycle)|       40 < HCLK <= 48        |
        +----------------------------------------------+
    [..]
     All these functions don't need the unlock sequence.

*/

/**
  * @brief  Sets the code latency value.
  * @param  FLASH_Latency: specifies the FLASH Latency value.
  *          This parameter can be one of the following values:
  *             @arg FLASH_Latency_0: FLASH Zero Latency cycle
  *             @arg FLASH_Latency_1: FLASH One Latency cycle
  *             @arg FLASH_Latency_2: FLASH two Latency cycle
  * @retval None
  */
void FLASH_SetLatency(uint32_t FLASH_Latency)
{
   uint32_t tmpreg = 0;

  /* Check the parameters */
  assert_param(IS_FLASH_LATENCY(FLASH_Latency));

  /* Read the ACR register */
  tmpreg = FLASH->ACR;

  /* Sets the Latency value */
  tmpreg &= (uint32_t) (~((uint32_t)FLASH_ACR_LATENCY));
  tmpreg |= FLASH_Latency;

  /* Write the ACR register */
  FLASH->ACR = tmpreg;
}

/** @defgroup FLASH_Memory_Programming_functions FLASH_Memory_Programming_functions
  * @{
  */

 /** @defgroup Flash_Group1 Flash_Group1
 *  @brief   Initialization and Configuration functions 
 *
@verbatim   
 ===============================================================================
                ##### FLASH Memory Programming functions #####
 ===============================================================================

    [..] The FLASH Memory Programming functions, includes the following functions:
       (+) void FLASH_Unlock(void);
       (+) void FLASH_Lock(void);
       (+) FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
       (+) FLASH_Status FLASH_EraseAllPages(void);
       (+) FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);

    [..] Any operation of erase or program should follow these steps:

       (#) Call the FLASH_Unlock() function to enable the flash control register and
           program memory access
       (#) Call the desired function to erase page or program data
       (#) Call the FLASH_Lock() to disable the flash program memory access
      (recommended to protect the FLASH memory against possible unwanted operation)

  @endverbatim
  * @{
  */

/**
  * @brief  Unlocks the FLASH control register and program memory access.
  * @retval None
  */
void FLASH_Unlock(void)
{
  if((FLASH->CR & FLASH_CR_LOCK) != RESET)
  {
    /* Unlocking the program memory access */
    FLASH->KEYR = FLASH_FKEY1;
    FLASH->KEYR = FLASH_FKEY2;
  }
}

/**
  * @brief  Locks the Program memory access.
  * @retval None
  */
void FLASH_Lock(void)
{
  /* Set the LOCK Bit to lock the FLASH control register and program memory access */
  FLASH->CR |= FLASH_CR_LOCK;
}

/**
  * @brief  Erases a specified page in program memory.
  * @note   To correctly run this function, the FLASH_Unlock() function must be called before.
  * @note   Call the FLASH_Lock() to disable the flash memory access (recommended
  *         to protect the FLASH memory against possible unwanted operation)
  * @param  Page_Address: The page address in program memory to be erased.
  * @note   A Page is erased in the Program memory only if the address to load
  *         is the start address of a page (multiple of 128 bytes).
  * @retval FLASH Status: The returned value can be:
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_FLASH_PROGRAM_ADDRESS(Page_Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to erase the page */
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR  = Page_Address;
    FLASH->CR |= FLASH_CR_STRT;
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    /* Disable the PER Bit */
    FLASH->CR &= ~FLASH_CR_PER;
  }

  /* Return the Erase Status */
  return status;
}

/**
  * @brief  Erases all FLASH pages.
  * @note   To correctly run this function, the FLASH_Unlock() function must be called before.
  * @note   Call the FLASH_Lock() to disable the flash memory access (recommended
  *         to protect the FLASH memory against possible unwanted operation)
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_EraseAllPages(void)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
         /* Disable the MER Bit */
    FLASH->CR &= ~FLASH_CR_MER;
    /* if the previous operation is completed, proceed to erase all pages */
     FLASH->CR |= FLASH_CR_MER;
     FLASH->CR |= FLASH_CR_STRT;

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    /* Disable the MER Bit */
    FLASH->CR &= ~FLASH_CR_MER;
  }

  /* Return the Erase Status */
  return status;
}


/**
  * @brief  Programs a word at a specified address.
  * @note   To correctly run this function, the FLASH_Unlock() function must be called before.
  * @note   Call the FLASH_Lock() to disable the flash memory access (recommended
  *         to protect the FLASH memory against possible unwanted operation)
  * @param  Address: specifies the address to be programmed.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_FLASH_PROGRAM_ADDRESS(Address));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
    /* If the previous operation is completed, proceed to program the new data */
    FLASH->CR |= FLASH_CR_PG;

    *(__IO uint32_t*)Address = Data;

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    /* Disable the PG Bit */
    FLASH->CR &= ~FLASH_CR_PG;
  }

  /* Return the Program Status */
  return status;
}


/**
  * @brief  Unlocks the option bytes block access.
  * @retval None
  */
void FLASH_OB_Unlock(void)
{
  if((FLASH->CR & FLASH_CR_OPTWRE) == RESET)
  {
    /* Unlocking the option bytes block access */
    FLASH->OPTKEYR = FLASH_OPTKEY1;
    FLASH->OPTKEYR = FLASH_OPTKEY2;
  }
}

/**
  * @brief  Locks the option bytes block access.
  * @retval None
  */
void FLASH_OB_Lock(void)
{
  /* Set the OPTWREN Bit to lock the option bytes block access */
  FLASH->CR &= ~FLASH_CR_OPTWRE;
}


/**
  * @brief  Erases the FLASH option bytes.
  * @note   To correctly run this function, the FLASH_OB_Unlock() function must be called before.
  * @note   Call the FLASH_OB_Lock() to disable the flash control register access and the option
  *         bytes (recommended to protect the FLASH memory against possible unwanted operation)
  * @note   This functions erases all option bytes except the Read protection (RDP).
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_OB_Erase(void)
{ 
  	FLASH_Status status = FLASH_COMPLETE; 

    FLASH->CR |= FLASH_CR_OPTER;
    FLASH->AR = 0x1FFFF000;
    FLASH->CR |= FLASH_CR_STRT;   
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
    
    if(status == FLASH_COMPLETE)
    {
      /* If the erase operation is completed, disable the OPTER Bit */
      FLASH->CR &= ~FLASH_CR_OPTER;
      
      /* Enable the Option Bytes Programming operation */
      FLASH->CR |= FLASH_CR_OPTPG;
         
      OB->USER_RDP = 0x00FF55AA;
    }  
     FLASH->CR &= ~FLASH_CR_OPTPG;
    
  	return status;
}

/**
  * @brief  Write protects the desired pages
  * @note   To correctly run this function, the FLASH_OB_Unlock() function must be called before.
  * @note   Call the FLASH_OB_Lock() to disable the flash control register access and the option
  *         bytes (recommended to protect the FLASH memory against possible unwanted operation)
  * @param  OB_WRP: specifies the address of the pages to be write protected.
  *          This parameter can be:
  *             @arg OB_WRP_Pages0to1..OB_WRP_Pages62to63, When there are multiple pages, required or manipulated 
  *             @arg OB_WRP_AllPages
  *             @arg OB_WRP_None
  * @retval FLASH Status: The returned value can be:
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_OB_WRPConfig(uint32_t OB_WRP)
{ 
 uint32_t WRP01_Data = 0xFFFFFFFF, WRP23_Data = 0xFFFFFFFF;

  FLASH_Status status = FLASH_COMPLETE;

  /* Check the parameters */
  assert_param(IS_OB_WRP(OB_WRP));

  OB_WRP = (uint32_t)(~OB_WRP);
  WRP01_Data = (uint32_t)((OB_WRP & OB_WRP0_WRP0)|(((OB_WRP >> 8) & OB_WRP0_WRP0)<<16));
  WRP23_Data = (uint32_t)(((OB_WRP >> 16) & OB_WRP0_WRP0)|(((OB_WRP >> 24) & OB_WRP0_WRP0)<<16));
   
  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

  if(status == FLASH_COMPLETE)
  {
    FLASH->CR |= FLASH_CR_OPTPG;

    if(WRP01_Data != 0x00FF00FF)
    {
      OB->WRP01 = WRP01_Data;

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
    }
    if((status == FLASH_COMPLETE) && (WRP23_Data != 0x00FF00FF))
    {
      OB->WRP23 = WRP23_Data;

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
    }

    if(status != FLASH_TIMEOUT)
    {
      /* if the program operation is completed, disable the OPTPG Bit */
      FLASH->CR &= ~FLASH_CR_OPTPG;
    }
  }
	return status;
}

/**
  * @brief  Enables or disables the read out protection.
  * @note   To correctly run this function, the FLASH_OB_Unlock() function must be called before.
  * @note   Call the FLASH_OB_Lock() to disable the flash control register access and the option
  *         bytes (recommended to protect the FLASH memory against possible unwanted operation)
  * @param  OB_RDP: specifies the read protection level.
  *          This parameter can be:
  *             @arg OB_RDP_Level_0: No protection
  *             @arg OB_RDP_Level_1: Read protection of the memory
  *             @arg OB_RDP_Level_2: Chip protection
  * @note   When enabling OB_RDP level 2 it's no more possible to go back to level 1 or 0
  * @retval FLASH Status: The returned value can be:
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_OB_RDPConfig(uint16_t OB_RDP)
{ 
    uint32_t user_rdp = 0x00FF55AA;
    user_rdp = OB->USER_RDP;
   FLASH_Status status = FLASH_COMPLETE;  
  /* Check the parameters */
  assert_param(IS_OB_RDP(OB_RDP));
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
 
  if(status == FLASH_COMPLETE)
  {
    FLASH->CR |= FLASH_CR_OPTER;
	FLASH->AR = 0x1FFFF000;
    FLASH->CR |= FLASH_CR_STRT;
    
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
    
    if(status == FLASH_COMPLETE)
    {
      /* If the erase operation is completed, disable the OPTER Bit */
      FLASH->CR &= ~FLASH_CR_OPTER;
      
      /* Enable the Option Bytes Programming operation */
      FLASH->CR |= FLASH_CR_OPTPG;
        
      user_rdp &= 0xFFFF0000;
      user_rdp |= OB_RDP;
      OB->USER_RDP = user_rdp;

      /* Wait for last operation to be completed */
      status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT); 
    
      if(status != FLASH_TIMEOUT)
      {
        /* if the program operation is completed, disable the OPTPG Bit */
        FLASH->CR &= ~FLASH_CR_OPTPG;
      }
    }
    else 
    {
      if(status != FLASH_TIMEOUT)
      {
        /* Disable the OPTER Bit */
        FLASH->CR &= ~FLASH_CR_OPTER;
      }
    }
  } 
  
  /* Return the protection operation Status */
  return status;
}

/**
  * @brief  Programs the FLASH User Option Byte: IWDG_SW / RST_STOP.
  * @note   To correctly run this function, the FLASH_OB_Unlock() function must be called before.
  * @note   Call the FLASH_OB_Lock() to disable the flash control register access and the option
  *         bytes (recommended to protect the FLASH memory against possible unwanted operation)
  * @param  OB_IWDG: Selects the WDG mode
  *          This parameter can be one of the following values:
  *             @arg OB_IWDG_SW: Software WDG selected
  *             @arg OB_IWDG_HW: Hardware WDG selected
  * @param  OB_STOP: Reset event when entering STOP mode.
  *          This parameter can be one of the following values:
  *             @arg OB_STOP_NoRST: No reset generated when entering in STOP
  *             @arg OB_STOP_RST: Reset generated when entering in STOP
  * @retval FLASH Status: The returned value can be:
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_OB_UserConfig(uint8_t OB_IWDG, uint8_t OB_STOP)
{
  FLASH_Status status = FLASH_COMPLETE; 
 uint32_t user_rdp = 0x00FF55AA;
  user_rdp = OB->USER_RDP;
  /* Check the parameters */
  assert_param(IS_OB_IWDG_SOURCE(OB_IWDG));
  assert_param(IS_OB_STOP_SOURCE(OB_STOP));

  /* Wait for last operation to be completed */
  status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);
  
  if(status == FLASH_COMPLETE)
  {
    /* Enable the Option Bytes Programming operation */
    FLASH->CR |= FLASH_CR_OPTPG; 
	
    user_rdp &= 0x0000FFFF;
    user_rdp |= (uint32_t)((uint16_t)(OB_IWDG | OB_STOP)<<16);
    OB->USER_RDP = user_rdp; 
  
    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    if(status != FLASH_TIMEOUT)
    {
      /* If the program operation is completed, disable the OPTPG Bit */
      FLASH->CR &= ~FLASH_CR_OPTPG;
    }
  } 
  /* Return the Option Byte program Status */
  return status;
}
 

/**
  * @brief  Programs a half word at a specified Option Byte Data address.
  * @note   To correctly run this function, the FLASH_OB_Unlock() function must be called before.
  * @note   Call the FLASH_OB_Lock() to disable the flash control register access and the option
  *         bytes (recommended to protect the FLASH memory against possible unwanted operation)
  * @param  Address: specifies the address to be programmed.
  *          This parameter can be 0x1FFFF004 to  0x1FFFF017.
  * @param  Data: specifies the data to be programmed.
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_OB_ProgramData(uint32_t Address, uint32_t Data)
{
  FLASH_Status status = FLASH_COMPLETE;
  /* Check the parameters */
  assert_param(IS_OB_DATA_ADDRESS(Address)); 

  if(status == FLASH_COMPLETE)
  {
    /* Enables the Option Bytes Programming operation */
    FLASH->CR |= FLASH_CR_OPTPG;
    *(__IO uint32_t*)Address = Data;

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    if(status != FLASH_TIMEOUT)
    {
      /* If the program operation is completed, disable the OPTPG Bit */
      FLASH->CR &= ~FLASH_CR_OPTPG;
    }
  }
  /* Return the Option Byte Data Program Status */
  return status;
}

/**
  * @brief  Programs a  word at a specified Option Byte Data address.
  * @param  BOR_LEVEL: BOR Level
  *          This parameter can be  OB_BOR_LEVEL_DISABLE
    *        @arg OB_BOR_LEVEL_1          
    *        @arg OB_BOR_LEVEL_2          
    *        @arg OB_BOR_LEVEL_3          
    *        @arg OB_BOR_LEVEL_4          
    *        @arg OB_BOR_LEVEL_5          
    *        @arg OB_BOR_LEVEL_6          
    *        @arg OB_BOR_LEVEL_7           
  * @retval FLASH Status: The returned value can be: FLASH_ERROR_PG,
  *         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_OB_BOR_LEVEL(uint8_t BOR_LEVEL)
{    
    uint16_t nborlevel = ~(BOR_LEVEL & 0xFFFF);
    uint16_t borlevel = BOR_LEVEL|(nborlevel<<8);
    uint32_t bor = *(uint32_t *)0x1ffff018;
    
  FLASH_Status status = FLASH_COMPLETE;  
   status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);    
  if(status == FLASH_COMPLETE)
  {
      bor &= 0xFFFF0000;      
      bor |= borlevel;
    /* Enables the Option Bytes Programming operation */
    FLASH->CR |= FLASH_CR_OPTPG;
    *(__IO uint32_t*)0x1ffff018 = bor;

    /* Wait for last operation to be completed */
    status = FLASH_WaitForLastOperation(FLASH_ER_PRG_TIMEOUT);

    if(status != FLASH_TIMEOUT)
    {
      /* If the program operation is completed, disable the OPTPG Bit */
      FLASH->CR &= ~FLASH_CR_OPTPG;
    }
  }
  /* Return the Option Byte Data Program Status */
  return status;
}

/**
  * @brief  Returns the FLASH User Option Bytes values.
  * @retval The FLASH User Option Bytes .
  */
uint8_t FLASH_OB_GetUser(void)
{
  /* Return the User Option Byte */
  return (uint8_t)(FLASH->OBR >> 8);
}

/**
  * @brief  Returns the FLASH Write Protection Option Bytes value.
  * @retval The FLASH Write Protection Option Bytes value
  */
uint32_t FLASH_OB_GetWRP(void)
{
  /* Return the FLASH write protection Register value */
  return (uint32_t)(FLASH->WRPR);
}

/**
  * @brief  Checks whether the FLASH Read out Protection Status is set or not.
  * @retval FLASH ReadOut Protection Status(SET or RESET)
  */
FlagStatus FLASH_OB_GetRDP(void)
{
  FlagStatus readstatus = RESET;

  if ((uint8_t)(FLASH->OBR & (FLASH_OBR_RDPRT_1 | FLASH_OBR_RDPRT)) != RESET)
  {
    readstatus = SET;
  }
  else
  {
    readstatus = RESET;
  }
  return readstatus;
}


/**  FLASH_Group4 Interrupts and flags management functions
 *   Interrupts and flags management functions
 *

 ===============================================================================
             ##### Interrupts and flags management functions #####
 ===============================================================================
 */

/**
  * @brief  Enables or disables the specified FLASH interrupts.
  * @param  FLASH_IT: specifies the FLASH interrupt sources to be enabled or
  *         disabled.
  *          This parameter can be any combination of the following values:
  *             @arg FLASH_IT_EOP: FLASH end of programming Interrupt
  *             @arg FLASH_IT_ERR: FLASH Error Interrupt
  * @param  NewState: specifies the FLASH interrupt sources to be enabled or
  * @retval None
  */
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FLASH_IT(FLASH_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if(NewState != DISABLE)
  {
    /* Enable the interrupt sources */
    FLASH->CR |= FLASH_IT;
  }
  else
  {
    /* Disable the interrupt sources */
    FLASH->CR &= ~(uint32_t)FLASH_IT;
  }
}

/**
  * @brief  Checks whether the specified FLASH flag is set or not.
  * @param  FLASH_FLAG: specifies the FLASH flag to check.
  *          This parameter can be one of the following values:
  *             @arg FLASH_FLAG_BSY: FLASH write/erase operations in progress flag
  *             @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag
  *             @arg FLASH_FLAG_EOP: FLASH End of Programming flag
  * @retval The new state of FLASH_FLAG (SET or RESET).
  */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG)
{
  FlagStatus bitstatus = RESET;

  /* Check the parameters */
  assert_param(IS_FLASH_GET_FLAG(FLASH_FLAG));

  if((FLASH->SR & FLASH_FLAG) != (uint32_t)RESET)
  {
    bitstatus = SET;
  }
  else
  {
    bitstatus = RESET;
  }
  /* Return the new state of FLASH_FLAG (SET or RESET) */
  return bitstatus;
}

/**
  * @brief  Clears the FLASH's pending flags.
  * @param  FLASH_FLAG: specifies the FLASH flags to clear.
  *          This parameter can be any combination of the following values:
  *             @arg FLASH_FLAG_WRPERR: FLASH Write protected error flag
  *             @arg FLASH_FLAG_EOP: FLASH End of Programming flag
  *             @arg FLASH_ERROR_PGERR:
  * @retval None
  */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
  /* Check the parameters */
  assert_param(IS_FLASH_CLEAR_FLAG(FLASH_FLAG));

  /* Clear the flags */
  FLASH->SR |= FLASH_FLAG;
}

/**
  * @brief  Returns the FLASH Status.
  * @retval FLASH Status: The returned value can be:
  *         FLASH_BUSY,  FLASH_ERROR_WRP FLASH_ERROR_PGERR or FLASH_COMPLETE.
  */
FLASH_Status FLASH_GetStatus(void)
{
  FLASH_Status FLASHstatus = FLASH_COMPLETE;

  if((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY)
  {
    FLASHstatus = FLASH_BUSY;
  }
  else
  {
    if((FLASH->SR & (uint32_t)FLASH_FLAG_WRPERR) == FLASH_FLAG_WRPERR)
    {
      FLASHstatus = FLASH_ERROR_WRP;
    }
    else  if((FLASH->SR & (uint32_t)FLASH_FLAG_PGERR) == FLASH_FLAG_PGERR)
    {
      FLASHstatus = FLASH_ERROR_PROGRAM;
    } 
    else
    {
      FLASHstatus = FLASH_COMPLETE;
    }
  }
  /* Return the FLASH Status */
  return FLASHstatus;
}


/**
  * @brief  Waits for a FLASH operation to complete or a TIMEOUT to occur.
  * @param  Timeout: FLASH programming Timeout
  * @retval FLASH Status: The returned value can be: FLASH_BUSY,
  *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
  */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{
  FLASH_Status status = FLASH_COMPLETE;

  /* Check for the FLASH Status */
  status = FLASH_GetStatus();

  /* Wait for a FLASH operation to complete or a TIMEOUT to occur */
  while((status == FLASH_BUSY) && (Timeout != 0x00))
  {
    status = FLASH_GetStatus();
    Timeout--;
  }

  if(Timeout == 0x00 )
  {
    status = FLASH_TIMEOUT;
  }
  /* Return the operation status */
  return status;
}

/**
  * @brief Return the unique device identifier (UID based on 96 bits)
  * @param UID: pointer to 3 words array.
  * @retval void
  */
void Sys_GetDevice96BitUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
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
