/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_conf.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Virtual Com Port Demo configuration file
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

#ifndef __USB_CONF_H
#define __USB_CONF_H

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*-------------------------------------------------------------*/
/* define the µC and the Bufferd Mode                          */
/*-------------------------------------------------------------*/
//#define STR7xx  /*STR7 family*/ 
//#define STR71x  /* STR71x µCs*/
#define STR91x   /*STR91x Family*/
#define STR91x_USB_BUFFERED /* Buffered Mode*/
//#define STR91x_USB_NON_BUFFERED /*Non buffered mode*/
/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/
#define EP_NUM  			(4)

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x118)
#define ENDP0_TXADDR        (0x158)

/* EP1  */
/* tx buffer base address */
#define ENDP1_TXADDR        (0x200)
#define ENDP2_TXADDR        (0x240)
#define ENDP3_RXADDR        (0x250)

/* ISTR events */
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
//#define  EP1_IN_Callback   NOP_Process
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
//#define  EP3_OUT_Callback  NOP_Process
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
