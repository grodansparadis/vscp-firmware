/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : 71x_it.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Main Interrupt Service Routines
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
#include "71x_lib.h"
#include "usb_lib.h"
#include "usb_pwr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
extern void USB_Istr(void);
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : XTI_IRQHandler
* Description    : This function handles the External interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void XTI_IRQHandler(void)
{
    /*XTI_PendingBitClear(XTI_InterruptLineValue());*/
}


/*******************************************************************************
* Function Name  : USBLP_IRQHandler
* Description    : This function handles USB low priority event interrupt.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBLP_IRQHandler(void)
{
        USB_Istr();
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
