/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This program provides a basic example of how to use the
*                      FLASH software library.
********************************************************************************
* History:
* 09/27/2005 :  V1.0
********************************************************************************
THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A
RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH
SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN
IN CONNECTION WITH THEIR PRODUCTS.
********************************************************************************/

/* Standard include ----------------------------------------------------------*/
#include "73x_lib.h"

/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum { FAIL = 0, PASS = !FAIL } Status;

/* Private define ------------------------------------------------------------*/
#define BufferSize 256
#define AddrToWriteTo  0x2000
#define AddrToReadFrom 0x2000

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 Index;
u32 SRC_Buffer[BufferSize], DST_Buffer[BufferSize];
Status  TransferStatus;

/* Private function prototypes -----------------------------------------------*/
Status Buffercmp(u32* pBuffer, u32* pBuffer1, u16 BufferLength);

/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : main
* Description    : main function, where program start. 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{

#ifdef DEBUG
    debug();
#endif

  /* fill in the SRC_Buffer */
  for(Index = 0; Index < BufferSize; Index++)
  {
    SRC_Buffer[Index] = Index; 
  }
  
  /* deinitialize Flash registers */
  FLASH_DeInit();
  
  /* erase Flash sector1 */
  FLASH_SectorErase(FLASH_Sector1);

  /* write the SRC_Buffer contents to the Flash starting from address 0x2000 */
  FLASH_BlockWrite((u32)&SRC_Buffer, AddrToWriteTo, BufferSize);

  /* read the first 256 words from Flash sector1 and store them in DST_Buffer */
  FLASH_BlockRead(AddrToReadFrom, (u32)&DST_Buffer, BufferSize);

/* check if the written data is correct --------------------------------------*/
  TransferStatus = Buffercmp(DST_Buffer, SRC_Buffer, BufferSize);
  /* TransferStatus = PASS, if the written data is correct */
  /* TransferStatus = FAIL, if the written data is corrupted */

  while(1);
}

/*******************************************************************************
* Function Name  : Buffercmp
* Description    : Compares two buffers.
* Input          : - pBuffer, pBuffer1: buffers to be compared.
*                : - BufferLength: buffer's length
* Output         : None
* Return         : PASS: pBuffer identical to pBuffer1
*                  FAIL: pBuffer differs from pBuffer1
*******************************************************************************/
Status Buffercmp(u32* pBuffer, u32* pBuffer1, u16 BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAIL;
    }
    
    pBuffer++;
    pBuffer1++;
  }
  return PASS;  
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
