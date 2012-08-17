/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Descriptors for Virtual Com Port Demo
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

#include "configuration.h"

#include "usb_lib.h"
#include "usb_desc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */

const u8 Virtual_Com_Port_DeviceDescriptor[] = {
	0x12,	/* bLength */
	USB_DEVICE_DESCRIPTOR_TYPE,	/* bDescriptorType */
	0x00,
	0x02,	/* bcdUSB = 2.00 */
	0xFF,	/* bDeviceClass */
	0xFF,	/* bDeviceSubClass */
	0x00,	/* bDeviceProtocol */
	0x40,	/* bMaxPacketSize0 */
	0x83,
	0x04,	/* idVendor = 0x0483 */
	0x34,//0x40,
	0x12,//0x57,	/* idProduct = 0x7540 */
	0x00,
	0x01,	/* bcdDevice = 1.00 */
	1,		/* Index of string descriptor describing manufacturer */
	2,		/* Index of string descriptor describing product */
	3,		/* Index of string descriptor describing the device's serial number */
	0x01	/* bNumConfigurations */
};


const u8 Virtual_Com_Port_ConfigDescriptor[] = {
/*Configuation Descriptor*/
	0x09,	/* bLength: Configuation Descriptor size */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType: Configuration */
	VIRTUAL_COM_PORT_SIZ_CONFIG_DESC,	/* wTotalLength:no of returned bytes */
	0x00,
	0x01,	/* bNumInterfaces: 1 interface */
	0x01,	/* bConfigurationValue: Configuration value */
	0x00,	/* iConfiguration: Index of string descriptor describing the configuration */
	0xA0,//0xC0,	/* bmAttributes: self powered */
	0x32,//0x00,	/* MaxPower 0 mA */
	
	

///////////////////// INTERFACE ////////////////////////	
/*Interface Descriptor*/
	0x09,	/* bLength: Interface Descriptor size */
	USB_INTERFACE_DESCRIPTOR_TYPE,	/* bDescriptorType: Interface */
			/* Interface descriptor type */
	0x00,	/* bInterfaceNumber: Number of Interface */
	0x00,	/* bAlternateSetting: Alternate setting */
	0x04,	/* bNumEndpoints: One endpoints used */
	0x11,	/* bInterfaceClass: Communication Interface Class */
	0x22,	/* bInterfaceSubClass: Abstract Control Model */
	0x33,	/* bInterfaceProtocol: Common AT commands */
	0x00,	/* iInterface: */       	
        
/////////////////////////// ENDPOINTS 1 //////////////////////////////	        
/*Endpoint 1 Descriptor*/
	0x07, 	/* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: Endpoint */
	0x81,	/* bEndpointAddress: (IN1) */
	0x02,	/* bmAttributes: Bulk */
	64,		/* wMaxPacketSize: */
	0x00,
	0x00, 	/* bInterval */

/////////////////////////// ENDPOINTS 2 //////////////////////////////		
/*Endpoint 2  Descriptor*/
	0x07, 	/* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: Endpoint */
	0x02,	/* bEndpointAddress: (OUT2) */
	0x02,	/* bmAttributes: Bulk */
	64,		/* wMaxPacketSize: */
	0x00,
	0x00, 	/* bInterval: ignore for Bulk transfer */
/////////////////////////// ENDPOINTS 3 //////////////////////////////	
/*Endpoint 3 Descriptor*/
	0x07, 	/* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: Endpoint */
	0x83,	/* bEndpointAddress: (IN3) */
	0x02,	/* bmAttributes: Bulk */
	25,		/* wMaxPacketSize: */
	0x00,
	0x00, 	/* bInterval */

/////////////////////////// ENDPOINTS 4 //////////////////////////////		
/*Endpoint 4  Descriptor*/
	0x07, 	/* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE ,	/* bDescriptorType: Endpoint */
	0x04,	/* bEndpointAddress: (OUT4) */
	0x02,	/* bmAttributes: Bulk */
	25,		/* wMaxPacketSize: */
	0x00,
	0x00 	/* bInterval: ignore for Bulk transfer */
};


/* USB String Descriptors */
const u8 Virtual_Com_Port_StringLangID[VIRTUAL_COM_PORT_SIZ_STRING_LANGID] = {
	VIRTUAL_COM_PORT_SIZ_STRING_LANGID,
	USB_STRING_DESCRIPTOR_TYPE,
	0x09,
        0x04
};	/* LangID = 0x0409: U.S. English */

const u8 Virtual_Com_Port_StringVendor[VIRTUAL_COM_PORT_SIZ_STRING_VENDOR] = {
	10,	/* Size of Vendor string */
	USB_STRING_DESCRIPTOR_TYPE,		/* bDescriptorType*/
	/* Manufacturer: "STMicroelectronics" */
        'e',0,
        'd',0,
        'e',0,
        'v',0,        
	'i',0,
        'c',0,
        'e',0,
        's',0
};

const u8 Virtual_Com_Port_StringProduct[VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT] = {
        VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT,          /* bLength */
        USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
        /* Product name: "STR75x Virtual COM Port" */
	'U',0,
        'S',0,
        'B',0,
        '2',0,
        'C',0,
        'A',0,
        'N',0,
        ' ',0, 
	'c',0,
        'o',0,
        'n',0,
        'v',0,
        'e',0,
        'r',0,
        't',0,
        'e',0, 
	'r',0
};

/*
const u8 Virtual_Com_Port_StringSerial[VIRTUAL_COM_PORT_SIZ_STRING_SERIAL] = {
        VIRTUAL_COM_PORT_SIZ_STRING_SERIAL,           // bLength 
        USB_STRING_DESCRIPTOR_TYPE,        // bDescriptorType 
        'E',0,
        'D',0,
        '0',0,
        '0',0,
        '0',0,
        '0',0,
        '0',0,
        '2',0          
};
*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
