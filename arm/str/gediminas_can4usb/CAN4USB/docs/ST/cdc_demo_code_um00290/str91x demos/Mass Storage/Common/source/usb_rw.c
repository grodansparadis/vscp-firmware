/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_rw.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : PMA read & write file
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
#include "usb_lib.h"
#include "usb_rw.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants----------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : PMA_Write
* Description    : Write a buffer to Packet Memory Area (PMA).
* Input 1        : pbUsrBuf    = pointer to the buffer to Write in PMA.
* Input 2        : PMAOffset   = offset in the PMA.
* Input 3        : wNBytes     = number of bytes to write.
* Output         : None
* Return         : None
*******************************************************************************/
void PMA_Write(u32 *pbUsrBuf,u16 PMAOffset, u32 wNBytes)
{
  u32 n= (wNBytes+3)>>2;
  u32 i;
  u32 *pbVal;
  
  pbVal = (u32 *)(PMAAddr + PMAOffset);
  for (i = n; i!= 0; i--)
  {
    *pbVal++ = *pbUsrBuf++;
  }
}
/*******************************************************************************
* Function Name  : PMA_Read
* Description    : Read a buffer from Packet Memory Area (PMA)
* Input 1        : pbUsrBuf    = Pointer to the buffer to fill with data from 
*                  PMA.
* Input 2        : PMAOffset   = Offset in the PMA.
* Input 3        : wNBytes     = Number of bytes to Read.
* Output         : None
* Return         : None
*******************************************************************************/
void PMA_Read(u32 *pbUsrBuf,u16 PMAOffset, u32 wNBytes)
{
  u32 n= (wNBytes+3)>>2;  /* n=(wNBytes+3)/4 */
  u32 i;
  u32 *pbVal;
    
  pbVal = (u32 *)(PMAAddr + PMAOffset);
  for (i = n; i!= 0; i--)
  {
    *pbUsrBuf++=*pbVal++;
  }  
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
