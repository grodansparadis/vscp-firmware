/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Date First Issued  : 10/01/2006 : V1.0
* Description        : Hardware Configuration & Setup
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
#include "hw_config.h"
u8 Mouse_Buffer[4] = {0,0,0,0};
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Set_System.
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
  SCU_MCLKSourceConfig(SCU_MCLK_OSC);
  FMI_Config(FMI_READ_WAIT_STATE_2,FMI_WRITE_WAIT_STATE_0, FMI_PWD_ENABLE,\
             FMI_LVD_ENABLE,FMI_FREQ_HIGH);
  SCU_PLLFactorsConfig(192,25,2);
  SCU_PLLCmd(ENABLE);
  SCU_MCLKSourceConfig(SCU_MCLK_PLL);
  
  SCU_AHBPeriphClockConfig(__VIC,ENABLE);
  SCU_AHBPeriphReset(__VIC,DISABLE);
  /*USB clock = MCLK/2= 48MHz*/
  SCU_USBCLKConfig(SCU_USBCLK_MCLK2);
  /*Enable USB clock*/
  SCU_AHBPeriphClockConfig(__USB,ENABLE);
  SCU_AHBPeriphReset(__USB,DISABLE);

  SCU_AHBPeriphClockConfig(__USB48M,ENABLE);
  
    /*Configure GPIO0 (D+ Pull-Up on P0.1)*/

  SCU_APBPeriphClockConfig(__GPIO0 ,ENABLE);
  SCU_APBPeriphReset(__GPIO0,DISABLE);

  GPIO_DeInit(GPIO0);
  GPIO_InitStructure.GPIO_Direction = GPIO_PinOutput;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull ;
  GPIO_InitStructure.GPIO_IPConnected=GPIO_IPConnected_Enable;
  GPIO_InitStructure.GPIO_Alternate=GPIO_OutputAlt1;
  GPIO_Init (GPIO0, &GPIO_InitStructure);
  
  /* Enable and configure the priority of the USB_LP IRQ Channel*/
  VIC_DeInit();
}
/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{

     if (NewState == ENABLE)
     GPIO_WriteBit(GPIO0, GPIO_Pin_1, Bit_RESET);
     else
     GPIO_WriteBit(GPIO0, GPIO_Pin_1, Bit_SET);

}
/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
}
/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : configure and enable the USB interrupt Lines.
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  VIC_Config(USBLP_ITLine, VIC_IRQ, 0);
  VIC_ITCmd(USBLP_ITLine, ENABLE);
}
/*******************************************************************************
* Function Name  : Keys_Config. 
* Description    : configure the keys of the joystick.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Keys_Config (void)
{
 /* GPIO 7 clock source enable */
  SCU_APBPeriphClockConfig(__GPIO7,ENABLE);
  SCU_APBPeriphReset(__GPIO7,DISABLE);

 /*Configure GPIO7 */
  GPIO_DeInit(GPIO7);
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Direction = GPIO_PinInput;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull ;
  GPIO_Init (GPIO7, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name : JoyState
* Description   : Decodes the Joystick direction
* Output        : None
* Return value  : the direction value
*******************************************************************************/
u8 JoyState(void)
{
  u8 port;
  port =GPIO_Read(GPIO7);
  if ((port&0xE0)==0x80) return(UP);
  if ((port&0xE0)==0x20) return(DOWN);
  if ((port&0xE0)==0x60) return(RIGHT);
  if ((port&0xE0)==0x40) return(LEFT);
  else return(0);

}
/*******************************************************************************
* Function Name : Joystick_Send
* Description   : prepares buffer to be sent containing Joystick event infos
* Input         : Keys: keys received from terminal
* Output        : None
* Return value  : None
*******************************************************************************/
void Joystick_Send(u8 Keys)
{
 
  s8 X = 0, Y = 0;

  switch (Keys)
  {
	case LEFT:
	  X -= CURSOR_STEP;
	  break;
	case RIGHT:

	  X += CURSOR_STEP;
	  break;
	case UP:
	  Y -= CURSOR_STEP;
	  break;
	case DOWN:
	  Y += CURSOR_STEP;
	  break;
	default:
	  return;
  }

  /* prepare buffer to send */
  Mouse_Buffer[1] = X;
  Mouse_Buffer[2] = Y;
  /*copy mouse position info in ENDP1 Tx Packet Memory Area*/
  UserToPMABufferCopy(Mouse_Buffer,GetEPTxAddr(ENDP1),4);
  /* enable endpoint for transmission */
  SetEPTxValid(ENDP1);

} /* Joystick_Send */
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/

