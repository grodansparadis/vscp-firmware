/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : STR75x Flash Library example.
********************************************************************************
* History:
* 10/01/2006 : V1.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "7xx_flash.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 SectorSize;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{
  
/*----------------------------------------------------------------------------*/
/*                      Flash default initialization                          */
/*----------------------------------------------------------------------------*/

  FLASH_DeInit();     

/*----------------------------------------------------------------------------*/
/*               Disable write protection of the sectors 0..3                 */
/*----------------------------------------------------------------------------*/

  FLASH_WriteProtectionCmd(FLASH_BANK0_SECTOR0 | FLASH_BANK0_SECTOR1 | 
                           FLASH_BANK0_SECTOR2 | FLASH_BANK0_SECTOR3, DISABLE);
  FLASH_WaitForLastOperation();

/*----------------------------------------------------------------------------*/
/*                           Erase sectors 1 & 2                              */
/*----------------------------------------------------------------------------*/

  FLASH_EraseSector(FLASH_BANK0_SECTOR1|FLASH_BANK0_SECTOR2);
  FLASH_WaitForLastOperation();

/*----------------------------------------------------------------------------*/
/*          Word program of data 0x11111111 at address 0x03554                */
/*----------------------------------------------------------------------------*/

  FLASH_WriteWord(0x03554, 0x11111111);
  FLASH_WaitForLastOperation();

/*----------------------------------------------------------------------------*/
/*           Double word program of data 0x22222222 at address 0x03558        */
/*                     and data 0x33333333 at address 0x0555C                 */
/*----------------------------------------------------------------------------*/

  FLASH_WriteDoubleWord(0x03558, 0x22222222, 0x33333333);
  FLASH_WaitForLastOperation();

/*----------------------------------------------------------------------------*/
/*                         Copy sector 1 to sector 2                          */
/*----------------------------------------------------------------------------*/
  
  /* Initialize the sector size */
  SectorSize = 0x2000;

/* Copy sector1 to sector2 ---------------------------------------------------*/
  while (SectorSize)
  {
    FLASH_WriteDoubleWord(0x06000 - SectorSize, *(u32*)(0x20004000 - SectorSize),
                          *(u32*)(0x20004000 + 4 - SectorSize));
    FLASH_WaitForLastOperation();

    /* Update the sector size */
    SectorSize = SectorSize - 8;
  }


/*----------------------------------------------------------------------------*/
/*               Enable write protection of the sectors 0-3                   */
/* Important Note : Write protection can be disabled only in a temporary way  */
/*----------------------------------------------------------------------------*/
  //FLASH_WriteProtectionCmd(FLASH_BANK0_SECTOR0 | FLASH_BANK0_SECTOR1 | 
  //                         FLASH_BANK0_SECTOR2 | FLASH_BANK0_SECTOR3, ENABLE);
  //FLASH_WaitForLastOperation();

  /* Infinite loop */
  while(1);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
