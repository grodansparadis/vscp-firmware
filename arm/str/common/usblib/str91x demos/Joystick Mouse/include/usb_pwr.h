/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_pwr.h
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Connection/disconnection & power management header
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
#ifndef __USB_PWR_H
#define __USB_PWR_H
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
typedef enum _RESUME_STATE{
	RESUME_EXTERNAL,
	RESUME_INTERNAL,
	RESUME_LATER,
	RESUME_WAIT,
	RESUME_START,
	RESUME_ON,
	RESUME_OFF,
	RESUME_ESOF
} RESUME_STATE;

typedef enum _DEVICE_STATE{
	UNCONNECTED,
	ATTACHED,
	POWERED,
	DEFAULT,
	ADDRESSED,
	CONFIGURED
} DEVICE_STATE;
/* Extern variables ----------------------------------------------------------*/
extern volatile u8 bDeviceState; /* USB device status */
extern volatile bool fSuspendEnabled;  /* true when suspend is possible */
/* Exported functions ------------------------------------------------------- */
void Suspend(void);
void Resume_Init(void);
void Resume(RESUME_STATE eResumeSetVal);
RESULT PowerOn(void);
RESULT PowerOff(void);
#endif /* __USB_PWR_H */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
