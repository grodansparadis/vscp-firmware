/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : All processing related to Voice Speaker demo (Endp=0)
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
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 MUTE_DATA=0;

DEVICE Device_Table = {
	EP_NUM,
	1
};

DEVICE_PROP Device_Property = {
	Speaker_init,
	Speaker_Reset,
	Speaker_Status_In,
	Speaker_Status_Out,
	Speaker_Data_Setup,
	Speaker_NoData_Setup,
	Speaker_Get_Interface_Setting,
	Speaker_GetDeviceDescriptor,
	Speaker_GetConfigDescriptor,
	Speaker_GetStringDescriptor,
	0,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
        Speaker_GetConfiguration,
        Speaker_SetConfiguration,
        Speaker_GetInterface,
        Speaker_SetInterface,
        Speaker_GetStatus,
        Speaker_ClearFeature,
        Speaker_SetEndPointFeature,
        Speaker_SetDeviceFeature,
        Speaker_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor = {
	(u8*)Speaker_DeviceDescriptor,
	SPEAKER_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor = {
	(u8*)Speaker_ConfigDescriptor,
	SPEAKER_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[5] = {
  {(u8*)Speaker_StringLangID, SPEAKER_SIZ_STRING_LANGID},
  {(u8*)Speaker_StringVendor, SPEAKER_SIZ_STRING_VENDOR},
  {(u8*)Speaker_StringProduct, SPEAKER_SIZ_STRING_PRODUCT},
  {(u8*)Speaker_StringSerial, SPEAKER_SIZ_STRING_SERIAL},
};
/* Extern variables ----------------------------------------------------------*/
extern u16 In_Data_Offset;
extern u16 Out_Data_Offset;
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Speaker_init
* Description    : Speaker init routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Speaker_init()
{
  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  _SetISTR(0);               /* clear pending interrupts */
  wInterrupt_Mask = IMR_MSK;
  _SetCNTR(wInterrupt_Mask); /* set interrupts mask */
  pInformation->Current_Feature = Speaker_ConfigDescriptor[7];
  /* Wait until device is configured */
  while (pInformation->Current_Configuration == 0) NOP_Process();
  bDeviceState = CONFIGURED;
}
/*******************************************************************************
* Function Name  : Speaker_Reset
* Description    : Speaker reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Speaker_Reset()
{
  /* Set Speaker device as not configured state */
  pInformation->Current_Configuration = 0;

  SetBTABLE(BTABLE_ADDRESS);

  /* Initialize Endpoint 0 */
  SetEPType(ENDP0, EP_CONTROL);
  SetEPTxStatus(ENDP0, EP_TX_NAK);
  SetEPRxAddr(ENDP0, ENDP0_RXADDR);
  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPTxAddr(ENDP0, ENDP0_TXADDR);
  Clear_Status_Out(ENDP0);
  SetEPRxValid(ENDP0);
	
  /* Initialize Endpoint 1 */
  SetEPType(ENDP1, EP_ISOCHRONOUS);
  SetEPDblBuffAddr(ENDP1,ENDP1_BUF0Addr,ENDP1_BUF1Addr);
  SetEPDblBuffCount(ENDP1, EP_DBUF_OUT, 0xC0);
  ClearDTOG_RX(ENDP1);
  ClearDTOG_TX(ENDP1);
  ToggleDTOG_TX(ENDP1);
  SetEPRxStatus(ENDP1, EP_RX_VALID);
  SetEPTxStatus(ENDP1, EP_TX_DIS);

  SetEPRxValid(ENDP0);
  /* Set this device to response on default address */
  SetDeviceAddress(0);
  Speaker_Timer_Config();
  In_Data_Offset = 0;
  Out_Data_Offset = 0;
}
/*******************************************************************************
* Function Name  : Speaker_Status_In
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Speaker_Status_In(void)
{
	
}
/*******************************************************************************
* Function Name  : Speaker_Status_Out
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Speaker_Status_Out (void)
{

}
/*******************************************************************************
* Function Name  : Speaker_Data_Setup
* Description    : handle the data class specific requests
* Input          : None.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Speaker_Data_Setup(u8 RequestNo)
{
  u8	*(*CopyRoutine)(u16);
  CopyRoutine = NULL;

  if ((RequestNo==GET_CUR)||(RequestNo==SET_CUR))  CopyRoutine = Mute_Command;

  else return USB_UNSUPPORT;

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);
  return USB_SUCCESS;
}
/*******************************************************************************
* Function Name  : Speaker_NoData_Setup
* Description    : handle the no data class specific requests
* Input          : None.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Speaker_NoData_Setup(u8 RequestNo)
{
 return USB_UNSUPPORT;
}
/*******************************************************************************
* Function Name  : Speaker_GetDeviceDescriptor
* Description    : get the device descriptor
* Input          : Length : u16.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
u8 *Speaker_GetDeviceDescriptor(u16 Length)
{
  return Standard_GetDescriptorData( Length, &Device_Descriptor );
}
/*******************************************************************************
* Function Name  : Speaker_GetConfigDescriptor
* Description    : get the configuration descriptor
* Input          : Length : u16.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *Speaker_GetConfigDescriptor(u16 Length)
{
  return Standard_GetDescriptorData( Length, &Config_Descriptor );
}
/*******************************************************************************
* Function Name  : Speaker_GetStringDescriptor
* Description    : Get the string descriptors according to the needed index
* Input          : Length : u16.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *Speaker_GetStringDescriptor(u16 Length)
{
  u8 wValue0 = pInformation->USBwValue0;
  return Standard_GetDescriptorData( Length, &String_Descriptor[wValue0] );
}
/*******************************************************************************
* Function Name  : Speaker_Get_Interface_Setting
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : u8: Interface : interface number.
* Input2         : u8: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT Speaker_Get_Interface_Setting(u8 Interface,u8 AlternateSetting)
{
  if(AlternateSetting >1) return USB_UNSUPPORT;
  else if(Interface > 1) return USB_UNSUPPORT;
  return USB_SUCCESS;
}
/*******************************************************************************
* Function Name  : Mute_Command
* Description    : handle the GET MUTE and SET MUTE command
* Input          : Length : u16.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *Mute_Command(u16 Length)
{

  if(Length==0)
  {
    pInformation->Ctrl_Info.Usb_wLength = pInformation->USBwLengths.w;
    return NULL;
  }
  else
    return((u8*)(&MUTE_DATA));
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
