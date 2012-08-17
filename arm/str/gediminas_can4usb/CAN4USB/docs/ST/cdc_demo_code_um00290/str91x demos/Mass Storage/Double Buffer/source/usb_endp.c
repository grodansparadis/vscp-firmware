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
#include "usb_bot.h"
#include "usb_rw.h"
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  u8 Bulk_Data_Buff[BULK_MAX_PACKET_SIZE];  /* data buffer*/
  u32 Data_Len;           /* data length*/
/* Private function prototypes -----------------------------------------------*/
  void Send_Data (u32* Data_Address,u16 Data_Length);
  void Send_Data_Request (u8* Data_Address,u16 Data_Length);
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
   Mass_Storage_In ();
}
/*******************************************************************************
* Function Name  : EP2_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP2_OUT_Callback(void)
{
  if(GetENDPOINT(ENDP2)&EP_DTOG_TX)
        {
          /*read from buffer 0*/
          Data_Len = GetEPDblBuf0Count(ENDP2);
          PMA_Read((u32 *)(&Bulk_Data_Buff),ENDP2_BUFF0ADDR, Data_Len);
        }
        else
        {
          /*read from buffer 1 */
          Data_Len = GetEPDblBuf1Count(ENDP2);
          PMA_Read((u32 *)(&Bulk_Data_Buff),ENDP2_BUFF1ADDR, Data_Len);
        }
        FreeUserBuffer(ENDP2,EP_DBUF_OUT);
      
  Mass_Storage_Out ();
}
/*******************************************************************************
* Function Name  : Send_Data
* Description    :
* Input          :
*      u8* Data_Address : point to the data to send.
*      u16 Data_Length : the nember of Bytes to send.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Send_Data(u32* Data_Address,u16 Data_Length)
{
  u32 Endpoint_DTOG_Status = _GetENDPOINT(ENDP1) & EP_DTOG_RX;
  
  SetEPDblBuffCount(ENDP1, EP_DBUF_IN, Data_Length);
  if (!Endpoint_DTOG_Status)
  {
    PMA_Write(Data_Address,ENDP1_BUFF0ADDR, Data_Length);
  }
  else 
  {
    PMA_Write(Data_Address,ENDP1_BUFF1ADDR, Data_Length);
  }
  FreeUserBuffer(ENDP1, EP_DBUF_IN);
}
/*******************************************************************************
* Function Name  : Send_Data_Request
* Description    :
* Input          :
*      u8* Data_Address : point to the data to send.
*      u16 Data_Length : the nember of Bytes to send.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Send_Data_Request(u8* Data_Address,u16 Data_Length)
{
  u32 Endpoint_DTOG_Status = _GetENDPOINT(ENDP1) & EP_DTOG_RX;
  
  SetEPDblBuffCount(ENDP1, EP_DBUF_IN, Data_Length);
  if (!Endpoint_DTOG_Status)
  {
    UserToPMABufferCopy(Data_Address,ENDP1_BUFF0ADDR,Data_Length);
  }
  else
  {
    UserToPMABufferCopy(Data_Address,ENDP1_BUFF1ADDR,Data_Length);
  }
  FreeUserBuffer(ENDP1, EP_DBUF_IN);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

