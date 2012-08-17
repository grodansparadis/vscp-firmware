/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : usb_istr.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : ISTR events interrupt service routines
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
#include "91x_lib.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_pwr.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile u16 wIstr;  /* ISTR register last read value */
/* Private function prototypes -----------------------------------------------*/
void USB_Istr(void);
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* function prototypes */
/* automatically built defining related macros */
#ifdef CTR_Callback
	void CTR_Callback(void);
#endif
#ifdef DOVR_Callback
	void DOVR_Callback(void);
#endif
#ifdef ERR_Callback
	void ERR_Callback(void);
#endif
#ifdef WKUP_Callback
	void WKUP_Callback(void);
#endif
#ifdef SUSP_Callback
	void SUSP_Callback(void);
#endif
#ifdef RESET_Callback
	void RESET_Callback(void);
#endif
#ifdef SOF_Callback
	void SOF_Callback(void);
#endif
#ifdef ESOF_Callback
	void ESOF_Callback(void);
#endif

extern void EP1_IN_Callback(void);
extern void EP2_IN_Callback(void);
extern void EP3_IN_Callback(void);
extern void EP4_IN_Callback(void);
extern void EP5_IN_Callback(void);
extern void EP6_IN_Callback(void);
extern void EP7_IN_Callback(void);
extern void EP8_IN_Callback(void);
extern void EP9_IN_Callback(void);
extern void EP10_IN_Callback(void);
extern void EP11_IN_Callback(void);
extern void EP12_IN_Callback(void);
extern void EP13_IN_Callback(void);
extern void EP14_IN_Callback(void);
extern void EP15_IN_Callback(void);


extern void EP1_OUT_Callback(void);
extern void EP2_OUT_Callback(void);
extern void EP3_OUT_Callback(void);
extern void EP4_OUT_Callback(void);
extern void EP5_OUT_Callback(void);
extern void EP6_OUT_Callback(void);
extern void EP7_OUT_Callback(void);
extern void EP8_OUT_Callback(void);
extern void EP9_OUT_Callback(void);
extern void EP10_OUT_Callback(void);
extern void EP11_OUT_Callback(void);
extern void EP12_OUT_Callback(void);
extern void EP13_OUT_Callback(void);
extern void EP14_OUT_Callback(void);
extern void EP15_OUT_Callback(void);

/* function pointers to non-control endpoints service routines */
void (*pEpInt_IN[15])(void)={
	EP1_IN_Callback,
	EP2_IN_Callback,
	EP3_IN_Callback,
	EP4_IN_Callback,
	EP5_IN_Callback,
	EP6_IN_Callback,
	EP7_IN_Callback,
	EP8_IN_Callback,
	EP9_IN_Callback,// only 9 endpoint in the STR9
        EP10_IN_Callback,
	EP11_IN_Callback,
	EP12_IN_Callback,
	EP13_IN_Callback,
	EP14_IN_Callback,
	EP15_IN_Callback
};

void (*pEpInt_OUT[15])(void)={
	EP1_OUT_Callback,
	EP2_OUT_Callback,
	EP3_OUT_Callback,
	EP4_OUT_Callback,
	EP5_OUT_Callback,
	EP6_OUT_Callback,
	EP7_OUT_Callback,
	EP8_OUT_Callback,
	EP9_OUT_Callback,
        EP10_OUT_Callback,// only 9 endpoint in the STR9
	EP11_OUT_Callback,
	EP12_OUT_Callback,
	EP13_OUT_Callback,
	EP14_OUT_Callback,
	EP15_OUT_Callback
};
/*******************************************************************************
* Function Name  : USB_Istr
* Description    : handle the usb interrupt routines
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Istr(void)
{
	wIstr = _GetISTR();
	#if (IMR_MSK & ISTR_RESET)
	if (wIstr & ISTR_RESET & wInterrupt_Mask)
	{
		_SetISTR((u16)CLR_RESET);
		Device_Property.Reset();
		#ifdef RESET_Callback
		RESET_Callback();
		#endif
	}
	#endif
	
	#if	(IMR_MSK & ISTR_DOVR)
	if (wIstr & ISTR_DOVR & wInterrupt_Mask)
	{
		_SetISTR((u16)CLR_DOVR);
		#ifdef DOVR_Callback
		DOVR_Callback();
		#endif
	}
	#endif
	
	#if	(IMR_MSK & ISTR_ERR)
	if (wIstr & ISTR_ERR & wInterrupt_Mask)
	{
		_SetISTR((u16)CLR_ERR);
		#ifdef ERR_Callback
		ERR_Callback();
		#endif
	}
	#endif
	
	#if	(IMR_MSK & ISTR_WKUP)
	if (wIstr & ISTR_WKUP & wInterrupt_Mask)
	{
		_SetISTR((u16)CLR_WKUP);
		Resume(RESUME_EXTERNAL);
		#ifdef WKUP_Callback
		WKUP_Callback();
		#endif
	}
	#endif
	
	#if	(IMR_MSK & ISTR_SUSP)
	if (wIstr & ISTR_SUSP & wInterrupt_Mask)
	{
		/* check if SUSPEND is possible */
		if(fSuspendEnabled)
		{
			Suspend();
		}
		else
		{
			/* if not possible then resume after xx ms */
			Resume(RESUME_LATER);
		}
		/* clear of the ISTR bit must be done after setting of CNTR_FSUSP */
		_SetISTR((u16)CLR_SUSP);
		#ifdef SUSP_Callback
		SUSP_Callback();
		#endif
	}
	#endif
	
	#if (IMR_MSK & ISTR_SOF)
	if (wIstr & ISTR_SOF & wInterrupt_Mask)
	{
		_SetISTR((u16)CLR_SOF);
		#ifdef SOF_Callback
		SOF_Callback();
		#endif
	}
	#endif
	
	#if (IMR_MSK & ISTR_ESOF)
	if (wIstr & ISTR_ESOF & wInterrupt_Mask)
	{
		_SetISTR((u16)CLR_ESOF);
		/* resume handling timing is made with ESOFs */
		Resume(RESUME_ESOF); /* request without change of the machine state */
		#ifdef ESOF_Callback
		ESOF_Callback();
		#endif
	}
	#endif
	
	#if	(IMR_MSK & ISTR_CTR)
	if (wIstr & ISTR_CTR & wInterrupt_Mask)
	{
		/* servicing of the endpoint correct transfer interrupt */
		/* clear of the CTR flag into the sub */
		CTR_LP();
		#ifdef CTR_Callback
		CTR_Callback();
		#endif
	}
	#endif
}/* USB_Istr */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/



