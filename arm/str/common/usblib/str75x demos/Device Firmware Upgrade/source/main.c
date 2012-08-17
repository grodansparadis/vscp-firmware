/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Device Firmware Upgrade(DFU) demo main file
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
#include "usb_pwr.h"
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
u8 DeviceState ;
u8 DeviceStatus[6];
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


int main(void)
{
   pFunction Jump_To_Application;



        DFU_Button_Config();
	if(DFU_Button_Read())
        {
                  Jump_To_Application = (pFunction) ApplicationAddress;
                  Jump_To_Application();	
	}


        DeviceState = STATE_dfuERROR;
	DeviceStatus[0]=STATUS_ERRFIRMWARE;
        DeviceStatus[4]=DeviceState;

        Set_System();
        Set_USBClock();
	USB_Init();

	while(1); /* Main loop */
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
