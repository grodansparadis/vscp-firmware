/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Provides a basic example of how to perform write, read and 
*                      erase operations on an SPI based FLASH connected to SMI. 
*                      The M25P64 FLASH mounted on the STR75x-EVAL board is used
*                      as example.
********************************************************************************
* History:
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "smi_flash.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/
#define PageSize       0x100

#define bBufferLength    PageSize
#define hBufferLength    PageSize/2
#define wBufferLength    PageSize/4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 bTxBuffer[bBufferLength], bRxBuffer[bBufferLength];
u16 hTxBuffer[hBufferLength], hRxBuffer[hBufferLength];
u32 wTxBuffer[wBufferLength], wRxBuffer[wBufferLength];
u32 WriteAddr, ReadAddr;
TestStatus bWriteStatus = PASSED, hWriteStatus = PASSED, wWriteStatus = PASSED;
u32 FALSHID, nCounter;
ErrorStatus OSC4MStartUpStatus;
  
/* Private function prototypes -----------------------------------------------*/
void MRCC_Configuration(void);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main()
{
  #ifdef DEBUG
    debug();
  #endif

/* Configure the system clocks -----------------------------------------------*/
  MRCC_Configuration();

/* Fill the transmit buffers -------------------------------------------------*/
  for(nCounter = 0; nCounter < bBufferLength; nCounter++)
  {
    bTxBuffer[nCounter] = nCounter;
  }

  for(nCounter = 0; nCounter < hBufferLength; nCounter++)
  {
    hTxBuffer[nCounter] = nCounter*2;
  }    
  
  for(nCounter = 0; nCounter < wBufferLength; nCounter++)
  {
    wTxBuffer[nCounter] = nCounter*4;
  }    
  

/* Configure SMI and GPIOs ---------------------------------------------------*/
  SMI_FLASH_Init(); /* SMI clock: HCLK/SMI_Prescaler = 60 MHz/2 = 30 MHz */

  
/* Write Byte buffer to the FLASH then check the written data ----------------*/  
  /* Erase sector 2 of the FLASH */
  SMI_FLASH_SectorErase(0x20000);

  ReadAddr = WriteAddr = 0x80020000;

  SMI_FLASH_WritebBuffer(bTxBuffer, WriteAddr, bBufferLength);

  SMI_FLASH_ReadbBuffer(bRxBuffer, ReadAddr, bBufferLength);
  
  for(nCounter = 0; (nCounter < bBufferLength) && (bWriteStatus == PASSED); nCounter++)
  {
    if(bRxBuffer[nCounter] != bTxBuffer[nCounter]) 
    {
      bWriteStatus = FAILED;   
    }
  }

  
/* Write Half Word buffer to the FLASH then check the written data ------------*/  
  /* Erase sector 3 of the FLASH */
  SMI_FLASH_SectorErase(0x30000);

  ReadAddr = WriteAddr = 0x80030000;

  SMI_FLASH_WritehBuffer(hTxBuffer, WriteAddr, hBufferLength);
  SMI_FLASH_ReadhBuffer(hRxBuffer, ReadAddr, hBufferLength);

  for(nCounter = 0; (nCounter < hBufferLength) && (hWriteStatus == PASSED); nCounter++)
  {
    if(hRxBuffer[nCounter] != hTxBuffer[nCounter])
    {
      hWriteStatus = FAILED;   
    }       
  }  
 
/* Write Word buffer to the FLASH then check the written data ----------------*/     
  /* Erase sector 4 of the FLASH */
  SMI_FLASH_SectorErase(0x40000);

  ReadAddr = WriteAddr = 0x80040000;

  SMI_FLASH_WritewBuffer(wTxBuffer, WriteAddr, wBufferLength);
  SMI_FLASH_ReadwBuffer(wRxBuffer, ReadAddr, wBufferLength);
  
  for(nCounter = 0; (nCounter < wBufferLength) && (wWriteStatus == PASSED); nCounter++)
  {
    if(wRxBuffer[nCounter] != wTxBuffer[nCounter]) 
    {
      wWriteStatus = FAILED;   
    }       
  }    

/* Bulk Erase ----------------------------------------------------------------*/  
  //SMI_FLASH_BulkErase(); /* This will erase the whole FLASH */
 
/* Read FLASH ID -------------------------------------------------------------*/
  FALSHID = SMI_FLASH_ReadID(); /* The ID should be 0x202017 for M25P64 FLASH */

 
  while(1);
}

/*******************************************************************************
* Function Name  : MRCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MRCC_Configuration(void)
{
  /* MRCC system reset(for debug purpose) */
  MRCC_DeInit();

  /* Wait for OSC4M start-up */
  OSC4MStartUpStatus = MRCC_WaitForOSC4MStartUp();

  if(OSC4MStartUpStatus == SUCCESS)
  {
    /* Set HCLK to 60MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div2);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div1);

    /* FLASH in burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

    /* Set CKSYS to 60MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* GPIO pins optimized for 3V3 operation */
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3); 
  
  /* Enable GPIOs clocks */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
}
  
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
