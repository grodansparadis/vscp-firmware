/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_endp.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Endpoint routines
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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 Stream_Buff[24];
u16 In_Data_Offset;
u16 Out_Data_Offset;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
u16 Data_Len;       /* data length*/
   if(GetENDPOINT(ENDP1)&EP_DTOG_TX)
        {
          /*read from ENDP1_BUF0Addr buffer*/
          Data_Len = GetEPDblBuf0Count(ENDP1);
          PMAToUserBufferCopy(Stream_Buff,ENDP1_BUF0Addr, Data_Len);
        }
       else
        {
          /*read from ENDP1_BUF1Addr buffer*/
          Data_Len = GetEPDblBuf1Count(ENDP1);
          PMAToUserBufferCopy(Stream_Buff,ENDP1_BUF1Addr, Data_Len);
        }

        FreeUserBuffer(ENDP1,EP_DBUF_OUT);
        In_Data_Offset += Data_Len;
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
