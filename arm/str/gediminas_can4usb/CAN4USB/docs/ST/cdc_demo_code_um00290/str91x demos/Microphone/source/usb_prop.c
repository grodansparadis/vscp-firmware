/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : All processing related to Microphone demo (Endp=0)
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
#include "usb_rw.h"
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
DEVICE Device_Table = {
	EP_NUM,
	1
};

DEVICE_PROP Device_Property = {
	Micro_init,
	Micro_Reset,
	Micro_Status_In,
	Micro_Status_Out,
	Micro_Data_Setup,
	Micro_NoData_Setup,
	Micro_Get_Interface_Setting,
	Micro_GetDeviceDescriptor,
	Micro_GetConfigDescriptor,
	Micro_GetStringDescriptor,
	0,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
        Micro_GetConfiguration,
        Micro_SetConfiguration,
        Micro_GetInterface,
        Micro_SetInterface,
        Micro_GetStatus,
        Micro_ClearFeature,
        Micro_SetEndPointFeature,
        Micro_SetDeviceFeature,
        Micro_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor = {
	(u8*)Micro_DeviceDescriptor,
	MICRO_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor = {
	(u8*)Micro_ConfigDescriptor,
	MICRO_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[5] = {
  {(u8*)Micro_StringLangID, MICRO_SIZ_STRING_LANGID},
  {(u8*)Micro_StringVendor, MICRO_SIZ_STRING_VENDOR},
  {(u8*)Micro_StringProduct, MICRO_SIZ_STRING_PRODUCT},
  {(u8*)Micro_StringSerial, MICRO_SIZ_STRING_SERIAL},
};

/* Extern variables ----------------------------------------------------------*/
extern u8 Stream_Buff[22];
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Micro_init
* Description    : Microphone init routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Micro_init()
{
  pInformation->Current_Configuration = 0;
  /* Connect the device */
  PowerOn();
  /* USB interrupts initialization */
  _SetISTR(0);               /* clear pending interrupts */
  wInterrupt_Mask = IMR_MSK;
  _SetCNTR(wInterrupt_Mask); /* set interrupts mask */
  pInformation->Current_Feature = Micro_ConfigDescriptor[7];
  /* Wait until device is configured */
  while (pInformation->Current_Configuration == 0) NOP_Process();
  bDeviceState = CONFIGURED;
}
/*******************************************************************************
* Function Name  : Micro_Reset
* Description    : Microphone reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Micro_Reset()
{
  /* Set MOUSE_DEVICE as not configured */
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
  SetEPDblBuffCount(ENDP1, EP_DBUF_IN,22);
  PMA_Write((u32*)(&Stream_Buff),ENDP1_BUF0Addr, 22);
  ClearDTOG_RX(ENDP1);
  ToggleDTOG_RX(ENDP1); 
  ClearDTOG_TX(ENDP1);  
  SetEPTxStatus(ENDP1, EP_TX_VALID);
  SetEPRxStatus(ENDP1, EP_RX_DIS);

  SetEPRxValid(ENDP0);
  Micro_Config();

  /* Set this device to response on default address */
  SetDeviceAddress(0);
} 
/*******************************************************************************
* Function Name  : Micro_Status_In
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Micro_Status_In(void)
{
  
}
/*******************************************************************************
* Function Name  : Micro_Status_Out
* Description    : 
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Micro_Status_Out (void)
{
  
}
/*******************************************************************************
* Function Name  : Micro_Data_Setup
* Description    : handle the data class specific requests
* Input          : None.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Micro_Data_Setup(u8 RequestNo)
{
  return USB_UNSUPPORT;
}
/*******************************************************************************
* Function Name  : Micro_NoData_Setup
* Description    : handle the no data class specific requests
* Input          : None.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Micro_NoData_Setup(u8 RequestNo)
{
  return USB_UNSUPPORT;
}
/*******************************************************************************
* Function Name  : Micro_GetDeviceDescriptor
* Description    : get the device descriptor
* Input          : Length : u16.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
u8 *Micro_GetDeviceDescriptor(u16 Length)
{
  return Standard_GetDescriptorData( Length, &Device_Descriptor );
}
/*******************************************************************************
* Function Name  : Micro_GetConfigDescriptor
* Description    : get the configuration descriptor
* Input          : Length : u16.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
u8 *Micro_GetConfigDescriptor(u16 Length)
{
  return Standard_GetDescriptorData( Length, &Config_Descriptor );
}
/*******************************************************************************
* Function Name  : Micro_GetStringDescriptor
* Description    : Get the string descriptors according to the needed index
* Input          : Length : u16.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
u8 *Micro_GetStringDescriptor(u16 Length)
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
RESULT Micro_Get_Interface_Setting(u8 Interface,u8 AlternateSetting)
{
  if(AlternateSetting >1) return USB_UNSUPPORT;
  else if(Interface > 1) return USB_UNSUPPORT;
  return USB_SUCCESS;
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


