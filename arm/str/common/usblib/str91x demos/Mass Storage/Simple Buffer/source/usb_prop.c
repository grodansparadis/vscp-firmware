/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_prop.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : All processing related to Mass Storage Demo
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
#include "usb_bot.h"
#include "hw_config.h"
#include "memory.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u32 Max_Lun = 0;

DEVICE Device_Table = {
	EP_NUM,
	1
};

DEVICE_PROP Device_Property = {
	MASS_init,
	MASS_Reset,
	MASS_Status_In,
	MASS_Status_Out,
	MASS_Data_Setup,
	MASS_NoData_Setup,
	MASS_Get_Interface_Setting,
	MASS_GetDeviceDescriptor,
	MASS_GetConfigDescriptor,
	MASS_GetStringDescriptor,
	0,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests = {
        Mass_Storage_GetConfiguration,
        Mass_Storage_SetConfiguration,
        Mass_Storage_GetInterface,
        Mass_Storage_SetInterface,
        Mass_Storage_GetStatus,
        Mass_Storage_ClearFeature,
        Mass_Storage_SetEndPointFeature,
        Mass_Storage_SetDeviceFeature,
        Mass_Storage_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor = {
	(u8*)MASS_DeviceDescriptor,
	MASS_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor = {
	(u8*)MASS_ConfigDescriptor,
	MASS_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[5] = {
  {(u8*)MASS_StringLangID, MASS_SIZ_STRING_LANGID},
  {(u8*)MASS_StringVendor, MASS_SIZ_STRING_VENDOR},
  {(u8*)MASS_StringProduct, MASS_SIZ_STRING_PRODUCT},
  {(u8*)MASS_StringSerial, MASS_SIZ_STRING_SERIAL},
  {(u8*)MASS_StringInterface, MASS_SIZ_STRING_INTERFACE},
};
/* Extern variables ----------------------------------------------------------*/
extern unsigned char Bot_State;
extern Bulk_Only_CBW CBW;
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Mass_Storage_SetConfiguration
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Mass_Storage_SetConfiguration(void)
{
  if (pInformation->Current_Configuration)
    {
        ClearDTOG_TX(ENDP1);
        ClearDTOG_RX(ENDP2);
       Bot_State = BOT_IDLE; /* set the Bot state machine to the IDLE state */
    }
}
/*******************************************************************************
* Function Name  : Mass_Storage_ClearFeature
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Mass_Storage_ClearFeature(void)
{
  /* when the host send a CBW with invalid signature or invalid length the two
     Endpoints (IN & OUT) shall stall until receiving a Mass Storage Reset     */
  if(CBW.dSignature != BOT_CBW_SIGNATURE)
  Bot_Abort(BOTH_DIR);
}
/*******************************************************************************
* Function Name  : MASS_init
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MASS_init()
{
    pInformation->Current_Configuration = 0;

    /* Connect the device */
     PowerOn();
    /* USB interrupts initialization */
    _SetISTR(0);               /* clear pending interrupts */
    wInterrupt_Mask = IMR_MSK;
    _SetCNTR(wInterrupt_Mask); /* set interrupts mask */
    pInformation->Current_Feature = MASS_ConfigDescriptor[7];
    /* Wait until device is configured */
    while (pInformation->Current_Configuration == 0) NOP_Process();
    bDeviceState = CONFIGURED;
} /* MASS_init() */
/*******************************************************************************
* Function Name  : MASS_Reset
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MASS_Reset()
{
  /* Set the device as not configured */
  Device_Info.Current_Configuration = 0;

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
  SetEPType(ENDP1, EP_BULK);
  SetEPTxAddr(ENDP1, ENDP1_TXADDR);
  SetEPTxStatus(ENDP1, EP_TX_NAK);
  SetEPRxStatus(ENDP1, EP_RX_DIS);
	
  /* Initialize Endpoint 2 */
  SetEPType(ENDP2, EP_BULK);
  SetEPRxAddr(ENDP2, ENDP2_RXADDR);
  SetEPRxCount(ENDP2, Device_Property.MaxPacketSize);
  SetEPRxStatus(ENDP2, EP_RX_VALID);
  SetEPTxStatus(ENDP2, EP_TX_DIS);


  SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
  SetEPRxValid(ENDP0);
	
  /* Set this device to response on default address */
  SetDeviceAddress(0);
  Get_Medium_Characteristics();
  CBW.dSignature = BOT_CBW_SIGNATURE;
  Bot_State = BOT_IDLE;
} /* MASS_Reset() */
/*******************************************************************************
* Function Name  : MASS_Status_In
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MASS_Status_In(void)
{
}/* End of MASS_Status_Out*/
/*******************************************************************************
* Function Name  : MASS_Status_Out
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void MASS_Status_Out (void)
{
}/* End of MASS_Status_Out*/
/*******************************************************************************
* Function Name  : MASS_Data_Setup
* Description    :
* Input          : RequestNo.
* Output         : None.
* Return         : RESULT.
*******************************************************************************/
RESULT MASS_Data_Setup(u8 RequestNo)
{	
  u8	*(*CopyRoutine)(u16);

  CopyRoutine = NULL;
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) && (RequestNo == GET_MAX_LUN) &&
     (pInformation->USBwValue==0)&&(pInformation->USBwIndex==0)&&(pInformation->USBwLength==0x01))
    {
      CopyRoutine = Get_Max_Lun;
    }
  else return USB_UNSUPPORT;
	
  if (CopyRoutine == NULL) return USB_UNSUPPORT;

  pInformation->Ctrl_Info.CopyData = CopyRoutine;
  pInformation->Ctrl_Info.Usb_wOffset = 0;
  (*CopyRoutine)(0);

  return USB_SUCCESS;

}  
/*******************************************************************************
* Function Name  : MASS_NoData_Setup
* Description    :
* Input          : RequestNo.
* Output         : None.
* Return         : RESULT.
*******************************************************************************/
RESULT MASS_NoData_Setup(u8 RequestNo)
{
  if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) && (RequestNo == MASS_STORAGE_RESET)
     &&(pInformation->USBwValue==0)&&(pInformation->USBwIndex==0)&&(pInformation->USBwLength==0x00))
    {
      /* Initialize Endpoint 1 */
      ClearDTOG_TX(ENDP1);
      /* Initialize Endpoint 2 */
      ClearDTOG_RX(ENDP2);
      CBW.dSignature = BOT_CBW_SIGNATURE; /*intialise the CBW signature to enable the clear feature*/
      Bot_State = BOT_IDLE;
      return USB_SUCCESS;
    }
  return USB_UNSUPPORT;
}  
/*******************************************************************************
* Function Name  : MASS_Get_Interface_Setting
* Description    :
* Input          : u8 Interface, u8 AlternateSetting.
* Output         : None.
* Return         : RESULT.
*******************************************************************************/
RESULT MASS_Get_Interface_Setting(u8 Interface,u8 AlternateSetting)
{
  if(AlternateSetting >0) return USB_UNSUPPORT;// in this application we don't have AlternateSetting
  else if(Interface > 0) return USB_UNSUPPORT;//in this application we have only 1 interfaces
  return USB_SUCCESS;
}
/*******************************************************************************
* Function Name  : MASS_GetDeviceDescriptor
* Description    :
* Input          : u16 Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 *MASS_GetDeviceDescriptor(u16 Length)
{
  return Standard_GetDescriptorData( Length, &Device_Descriptor );
}
/*******************************************************************************
* Function Name  : MASS_GetConfigDescriptor
* Description    :
* Input          : u16 Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 *MASS_GetConfigDescriptor(u16 Length)
{
  return Standard_GetDescriptorData( Length, &Config_Descriptor );
}
/*******************************************************************************
* Function Name  : MASS_GetStringDescriptor
* Description    :
* Input          : u16 Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 *MASS_GetStringDescriptor(u16 Length)
{
  u8 wValue0 = pInformation->USBwValue0;
  return Standard_GetDescriptorData( Length, &String_Descriptor[wValue0] );
}
/*******************************************************************************
* Function Name  : Get_Max_Lun
* Description    :
* Input          : u16 Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 *Get_Max_Lun(u16 Length)
{
  if(Length==0)
  {
    pInformation->Ctrl_Info.Usb_wLength = LUN_DATA_LENGTH;
    return 0;
  }
  else
    return((u8*)(&Max_Lun));
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

