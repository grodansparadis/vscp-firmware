/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_mem.c
* Author             : MCD Application Team
* Date First Issued  : 10/27/2003 : V1.0
* Description        : Utility functions for memory transfers to/from PMA
********************************************************************************
* History:
* 09/18/2006 : V3.0
* 09/01/2006 : V2.0
* 10/27/2003 : V1.0
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
#include "usb_mem.h"
#include "usb_conf.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : UserToPMABufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input 1        : pbUsrBuf    = pointer to user memory area
* Input 2        : wPMABufAddr = address into PMA
* Input 3        : wNBytes     = no. of bytes to be copied
* Output         : None
* Return         : None
*******************************************************************************/
void UserToPMABufferCopy(u8 *pbUsrBuf,u16 wPMABufAddr, u16 wNBytes)
{
 #ifdef STR7xx             /* STR7xx family    */
  u32 n= (wNBytes+1)>>1;   /* n=(wNBytes+1)/2  */
  u32 i,temp1,temp2;
  u16 *pdwVal;
  pdwVal = (u16 *)(wPMABufAddr*2 + PMAAddr);
  for (i = n; i!= 0; i--)
  {
     temp1 = (u16)*pbUsrBuf;
     pbUsrBuf++;
     temp2 = temp1 | (u16)*pbUsrBuf<<8;
     *pdwVal++ = temp2;
     pdwVal++;
     pbUsrBuf++;
  }
  #endif                  /* End of STR7xx family*/
  
  #ifdef STR91x           /*STR91x family*/
  
    u32 n= (wNBytes+3)>>2;  /*n=(wNBytes+1)/4*/
    u32 i,temp1,temp2,temp3,temp4;
    u32 *pdwVal;
    pdwVal = (u32 *)(PMAAddr+(u32)((wPMABufAddr)));
    for (i=n; i!=0; i--)
    {
      temp1 = (u32)*pbUsrBuf;
      temp2 = temp1 | (u32)*(pbUsrBuf+1)<<8;
      temp3 = temp2 | (u32)*(pbUsrBuf+2)<<16; 
      temp4 = temp3 | (u32)*(pbUsrBuf+3)<<24;
      pbUsrBuf+=4;
      *pdwVal = temp4;

      pdwVal++;
    }
  #endif /* End of STR91x family*/
}
/*******************************************************************************
* Function Name  : PMAToUserBufferCopy
* Description    : Copy a buffer from user memory area to packet memory area (PMA)
* Input 1        : pbUsrBuf    = pointer to user memory area
* Input 2        : wPMABufAddr = address into PMA
* Input 3        : wNBytes     = no. of bytes to be copied
* Output         : pbUsrBuf    = pointer to user memory area with correct data
* Return         : None
*******************************************************************************/
void PMAToUserBufferCopy(u8 *pbUsrBuf,u16 wPMABufAddr, u16 wNBytes)
{
 #ifdef STR7xx /*STR7xx family*/
    u32 n= (wNBytes+1)>>1;/* /2*/
    u32 i;
    u32 *pdwVal;
    pdwVal = (u32 *)(wPMABufAddr*2 + PMAAddr);
    for (i = n; i!= 0; i--)
    {
      *(u16*)pbUsrBuf++ = *pdwVal++;
      pbUsrBuf++;
    }
  #endif /* End of STR7xx family*/
    
  #ifdef STR91x  /*STR91x family*/
    u8 *pbVal;
    u16 wNTrasf=wNBytes;
    if((wNBytes) == 0) return;
    pbVal = (u8 *)(PMAAddr + wPMABufAddr);
    while(1)
    {
      *pbUsrBuf++ = *pbVal++;
      if((--wNTrasf) == 0) return;
      *pbUsrBuf++ = *pbVal++;
      if((--wNTrasf) == 0) return;
    } 
  #endif /* End of STR91x family*/
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

