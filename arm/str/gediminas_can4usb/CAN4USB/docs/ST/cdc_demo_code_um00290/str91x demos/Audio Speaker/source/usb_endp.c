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
#include "USB_lib.h"
#include "usb_rw.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 Stream_Buff[80];
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
   if(GetENDPOINT(ENDP1)&EP_DTOG_TX)
        {
          /*read from ENDP1_BUF0Addr buffer*/
          PMA_Read((u32*)(&Stream_Buff), ENDP1_BUF0Addr , 22);
        }
       else
        {
          /*read from ENDP1_BUF1Addr buffer*/
          PMA_Read((u32*)(&Stream_Buff),ENDP1_BUF1Addr , 22);

        }

        FreeUserBuffer(ENDP1,EP_DBUF_OUT);
        In_Data_Offset += 22;
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
