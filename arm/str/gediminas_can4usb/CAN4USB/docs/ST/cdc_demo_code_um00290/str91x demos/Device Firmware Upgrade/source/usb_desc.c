/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_desc.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Descriptors for Device Firmware Upgrade (DFU)
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
#include "usb_desc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


const u8 DFU_DeviceDescriptor[DFU_SIZ_DEVICE_DESC] ={
        0x12,	/* bLength */
	0x01,	/* bDescriptorType */
	0x00,	/* bcdUSB, version 1.00 */
	0x01,
	0x00,	/* bDeviceClass : See interface */
        0x00,	/* bDeviceSubClass : See interface*/
        0x00,	/* bDeviceProtocol : See interface */
	bMaxPacketSize0, /* bMaxPacketSize0 0x40 = 64 */
	0x83,	/* idVendor	(0483) */
	0x04,
	0x11,	/* idProduct (0xDF11) DFU PiD*/
	0xDF,
	0x1A,	/* bcdDevice*/
	0x01,

	0x01,   /* iManufacturer : index of string Manufacturer  */	
	0x02,	/* iProduct      : index of string descriptor of product*/		
	0x03,   /* iSerialNumber : index of string serial number*/		
		
	0x01	/*bNumConfigurations */
};

const u8 DFU_ConfigDescriptor[DFU_SIZ_CONFIG_DESC] = {
	0x09,	/* bLength: Configuation Descriptor size */
	0x02,	/* bDescriptorType: Configuration */
	DFU_SIZ_CONFIG_DESC,
			/* wTotalLength: Bytes returned */
	0x00,
	0x01,	/* bNumInterfaces: 1 interface */
	0x01,	/* bConfigurationValue: */
			/*	Configuration value */
	0x00,	/* iConfiguration: */
			/*	Index of string descriptor */
			/*	describing the configuration */
	0x80,	/* bmAttributes: */
			/*	bus powered */
	0x52,	/* MaxPower 100 mA: this current is used for detecting Vbus */
/* 09 */

/******************** Descriptor of DFU interface 0 Alternate setting 0 ********************/
	0x09,	/* bLength: Interface Descriptor size */
	0x04,	/* bDescriptorType: */
		/*	Interface descriptor type */
	0x00,	/* bInterfaceNumber: Number of Interface */
	0x00,	/* bAlternateSetting: Alternate setting */
	0x00,	/* bNumEndpoints*/
	0xFE,	/* bInterfaceClass: DFU */
	0x01,	/* bInterfaceSubClass */
	0x00,	/* nInterfaceProtocol */
	0x04,	/* iInterface: */
		/* Index of string descriptor */
/* 18 */

/******************** Descriptor of DFU interface 0 Alternate setting 1  ********************/


	0x09,	/* bLength: Interface Descriptor size */
	0x04,	/* bDescriptorType: */
		/*	Interface descriptor type */
	0x00,	/* bInterfaceNumber: Number of Interface */
	0x01,	/* bAlternateSetting: Alternate setting */
	0x00,	/* bNumEndpoints*/
	0xFE,	/* bInterfaceClass: DFU */
	0x01,	/* bInterfaceSubClass */
	0x00,	/* nInterfaceProtocol */
	0x05,	/* iInterface: */
		/* Index of string descriptor */

/* 27 */

#ifdef _STR710_EVAL
/******************** Descriptor of DFU interface 0 Alternate setting 2  ********************/

	0x09,	/* bLength: Interface Descriptor size */
	0x04,	/* bDescriptorType: */
		/*	Interface descriptor type */
	0x00,	/* bInterfaceNumber: Number of Interface */
	0x02,	/* bAlternateSetting: Alternate setting */
	0x00,	/* bNumEndpoints*/
	0xFE,	/* bInterfaceClass: DFU */
	0x01,	/* bInterfaceSubClass */
	0x00,	/* nInterfaceProtocol */
	0x06,	/* iInterface: */
		/* Index of string descriptor */
#endif
/* 36 */


/******************** DFU Functional Descriptor********************/
	0x09,   /*blength = 7 Bytes*/
	0x21,   /* DFU Functional Descriptor*/
	0x0B,   /*bmAttribute
	
                     bitCanDnload             = 1      (bit 0)
                     bitCanUpload             = 1      (bit 1)
                     bitManifestationTolerant = 0      (bit 2)
                     bitWillDetach            = 1      (bit 3)
                     Reserved                          (bit4-6)
                     bitAcceleratedST         = 0      (bit 7)*/
	0xFF,   /*DetachTimeOut= 255 ms*/
	0x00,
    	wTransferSizeB0,
	wTransferSizeB1,          /* TransferSize = 1024 Byte*/
        0x1A,                     /* bcdDFUVersion*/
        0x01
/***********************************************************/
/*43*/

}; /* DFU_ConfigDescriptor */



const u8 DFU_StringLangId[DFU_SIZ_STRING_LANGID] = {
	DFU_SIZ_STRING_LANGID,
	0x03,
	0x09,
	0x04	/* LangID = 0x0409: U.S. English */
};


const u8 DFU_StringVendor[DFU_SIZ_STRING_VENDOR] = {
	DFU_SIZ_STRING_VENDOR,
	0x03,
/* Manufacturer: "STMicroelectronics" */
	'S',0, 'T',0, 'M',0, 'i',0, 'c',0, 'r',0, 'o',0, 'e',0,
	'l',0, 'e',0, 'c',0, 't',0, 'r',0, 'o',0, 'n',0, 'i',0,
	'c',0, 's',0
};


const u8 DFU_StringProduct[DFU_SIZ_STRING_PRODUCT] = {
        DFU_SIZ_STRING_PRODUCT,
	0x03,
        /* Product name: "STR91x-DFU" */
	'S',0, 'T',0, 'R',0, '9',0, '1',0 , 'x',0,'-',0, 'D',0, 'F',0,'U',0
};


const u8 DFU_StringSerial[DFU_SIZ_STRING_SERIAL] = {
	DFU_SIZ_STRING_SERIAL,
	0x03,
	/* Serial number: "001" */
	'0',0,'0',0,'1',0,
};

/* Interface String */

const u8 DFU_StringInterface0[DFU_SIZ_STRING_INTERFACE0] = {
	DFU_SIZ_STRING_INTERFACE0,
	0x03,
	// Interface 0: "@Internal Flash 0 /0x00000000/8*064Kg"
	'@',0, 'I',0, 'n',0, 't',0, 'e',0, 'r',0, 'n',0, 'a',0, 'l',0,
        ' ',0, 'F',0, 'l',0, 'a',0, 's',0, 'h',0, ' ',0, '0',0,
        '/',0, '0',0, 'x',0, '0',0, '0',0, '0',0, '0',0, '0',0, '0',0, '0',0, '0',0,
        '/',0, '8',0, '*',0, '0',0, '6',0, '4',0, 'K',0, 'g',0
};

const u8 DFU_StringInterface1[DFU_SIZ_STRING_INTERFACE1] = {
	DFU_SIZ_STRING_INTERFACE1,
	0x03,
        // Interface 1: "@Internal Flash 1 /0x00080000/4*008Ka"
	'@',0, 'I',0, 'n',0, 't',0, 'e',0, 'r',0, 'n',0, 'a',0, 'l',0,
        ' ',0, 'F',0, 'l',0, 'a',0, 's',0, 'h',0, ' ',0, '1',0,
        '/',0, '0',0, 'x',0, '0',0, '0',0, '0',0, '8',0, '0',0, '0',0, '0',0, '0',0,
        '/',0, '4',0, '*',0, '0',0, '0',0, '8',0, 'K',0, 'a',0
};

#ifdef _STR910_EVAL

const u8 DFU_StringInterface2[DFU_SIZ_STRING_INTERFACE2] = {
	DFU_SIZ_STRING_INTERFACE2,
	0x03,
        // Interface 2: "@External Flash  /0x60000000/8*008Kg,63*064Kg"
	'@',0, 'E',0, 'x',0, 't',0, 'e',0, 'r',0, 'n',0, 'a',0, 'l',0,
        ' ',0, 'F',0, 'l',0, 'a',0, 's',0, 'h',0, ' ',0, ' ',0,
        '/',0, '0',0, 'x',0, '6',0, '0',0, '0',0, '0',0, '0',0, '0',0, '0',0, '0',0,
        '/',0, '8',0, '*',0, '0',0, '0',0, '8',0, 'K',0, 'g',0,
        ',',0, '6',0, '3',0, '*',0, '0',0, '6',0, '4',0, 'K',0, 'g',0
};
#endif

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

