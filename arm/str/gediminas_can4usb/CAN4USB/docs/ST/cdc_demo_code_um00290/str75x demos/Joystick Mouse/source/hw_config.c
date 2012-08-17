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
#include "75x_lib.h"
#include "hw_config.h"
#include "usb_lib.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus OSC4MStartUpStatus;
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{

  GPIO_InitTypeDef    GPIO_InitStructure;

  EXTIT_InitTypeDef   EXTIT_InitStructure;

 
  /* MRCC configuration --------------------------------------------------------*/
  MRCC_DeInit();

  /* Clear No Clock Detected flag */
  MRCC_ClearFlag(MRCC_FLAG_NCKD);

  /* Enbale No Clock Detected interrupt */
  MRCC_ITConfig(MRCC_IT_NCKD, ENABLE);

  /* Wait for OSC4M start-up */
  OSC4MStartUpStatus = MRCC_WaitForOSC4MStartUp();
  
   if(OSC4MStartUpStatus == SUCCESS)
  {  
  /* Set HCLK to 30 MHz */
  MRCC_HCLKConfig(MRCC_CKSYS_Div2);

  /* Set CKTIM to 30 MHz */
  MRCC_CKTIMConfig(MRCC_HCLK_Div1);

  /* Set PCLK to 30 MHz */
  MRCC_PCLKConfig(MRCC_CKTIM_Div1);
   
  /* Set CKSYS to 60 MHz */
  MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

 

  /* Enable GPIOs, USB and EXTIT clocks */
   MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_EXTIT | MRCC_Peripheral_USB, ENABLE);


  
  EXTIT_ClearITPendingBit(EXTIT_ITLine14);
  EXTIT_InitStructure.EXTIT_ITLine = EXTIT_ITLine14;
  EXTIT_InitStructure.EXTIT_ITTrigger = EXTIT_ITTrigger_Rising;
  EXTIT_InitStructure.EXTIT_ITLineCmd = ENABLE;
  EXTIT_Init(&EXTIT_InitStructure);


  /* Configure P2.18 ( USB Suspend/Resume ) as Output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_19;
  GPIO_Init(GPIO2, &GPIO_InitStructure);

  GPIO_WriteBit(GPIO2, GPIO_Pin_19, (BitAction)(0));

   /* Configure P0.9 (D+ Pull up)  as Output push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin =   GPIO_Pin_9;
  GPIO_Init(GPIO0, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/

void Set_USBClock(void)
{
  /* Set USB kernel clock to 48 MHz */
  MRCC_CKUSBConfig (MRCC_CKUSB_Internal);
  CFG_USBFilterConfig(CFG_USBFilter_Enable);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
 /*    GPIO_WriteBit(GPIO2, GPIO_Pin_19, (BitAction)(1));

     EXTIT_ClearITPendingBit(EXTIT_ITLine14);


     // Enter STOP mode with OSC4M off, FLASH off and MVREG off
      MRCC_EnterSTOPMode(MRCC_STOPParam_MVREGOff);

     //Clear LP_DONE flag
      MRCC_ClearFlag(MRCC_FLAG_LPDONE);*/
}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  /* EXTIT_ClearITPendingBit(EXTIT_ITLine14);

   // wake up and enable all clocks in STR75x
   //WaitForOSC4MStartUp();

    // Clear No Clock Detected flag
    MRCC_ClearFlag(MRCC_FLAG_NCKD);
    // Set CKSYS to 60MHz
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);

    Set_USBClock();*/
}
/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  EIC_IRQInitTypeDef  EIC_IRQInitStructure;

  /* Enable and configure the priority of the USB_LP IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = USB_LP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 2;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable the Interrupt controller to manage IRQ channel*/
  EIC_IRQCmd(ENABLE);
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
     GPIO_WriteBit(GPIO0, GPIO_Pin_9, (BitAction)(0));
     else
     GPIO_WriteBit(GPIO0, GPIO_Pin_9, (BitAction)(1));
}
/*******************************************************************************
* Function Name : JoyState
* Description   : Decodes the Joystick direction
* Output        : None
* Return value  : the direction value
*******************************************************************************/
u8 JoyState(void)
{
  /* "RIGHT" key is pressed */
  if(!GPIO_ReadBit(GPIO1, GPIO_Pin_13))
  {
    return RIGHT; 
  }
  /* "LEFT" key is pressed */
  if(!GPIO_ReadBit(GPIO1, GPIO_Pin_12))
  {
    return LEFT; 
  }
  /* "UP" key is pressed */
  if(!GPIO_ReadBit(GPIO0, GPIO_Pin_8))
  {
    return UP; 
  }
  /* "DOWN" key is pressed */
  if(!GPIO_ReadBit(GPIO1, GPIO_Pin_7))
  {
    return DOWN; 
  }
  /* No key is pressed */
  else 
  {
    return 0;
  }
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
  u8 Mouse_Buffer[4] = {0,0,0,0};
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

}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
