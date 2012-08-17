/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_conf.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Device Firmware Upgrade (DFU) configuration file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

#define STR7xx
#define STR75x
/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/
#define EP_NUM  			(1)

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0 */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x10)
#define ENDP0_TXADDR        (0x50)


/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
#define IMR_MSK (CNTR_CTRM  | \
				 CNTR_WKUPM | \
				 CNTR_SUSPM | \
				 CNTR_ERRM  | \
				 CNTR_SOFM  | \
				 CNTR_ESOFM | \
		 		 CNTR_RESETM  \
		 		)

/* CTR service routines */
/* associated to defined endpoints */
#define  EP1_IN_Callback   NOP_Process
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process
#define  EP8_IN_Callback   NOP_Process
#define  EP9_IN_Callback   NOP_Process
#define  EP10_IN_Callback  NOP_Process
#define  EP11_IN_Callback  NOP_Process
#define  EP12_IN_Callback  NOP_Process
#define  EP13_IN_Callback  NOP_Process
#define  EP14_IN_Callback  NOP_Process
#define  EP15_IN_Callback  NOP_Process


#define  EP1_OUT_Callback   NOP_Process
#define  EP2_OUT_Callback   NOP_Process
#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process
#define  EP8_OUT_Callback   NOP_Process
#define  EP9_OUT_Callback   NOP_Process
#define  EP10_OUT_Callback  NOP_Process
#define  EP11_OUT_Callback  NOP_Process
#define  EP12_OUT_Callback  NOP_Process
#define  EP13_OUT_Callback  NOP_Process
#define  EP14_OUT_Callback  NOP_Process
#define  EP15_OUT_Callback  NOP_Process

#endif /*__USB_CONF_H*/

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/




