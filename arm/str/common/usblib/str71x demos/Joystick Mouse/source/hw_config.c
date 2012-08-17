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
#include "71x_lib.h"
#include "usb_lib.h"
#include "hw_config.h"
#include "usb_mem.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
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
  u32 Tmp=0 ;

     RCCU_MCLKConfig ( RCCU_DEFAULT );
     RCCU_FCLKConfig ( RCCU_RCLK_2 );
     RCCU_PCLKConfig ( RCCU_RCLK_2 );



     /* Configure the PLL1 ( * 16 , / 2 )*/
     /*RCCU_PLL1Config ( RCCU_PLL1_Mul_12 , RCCU_Div_2 ) ;*/

     Tmp = ( RCCU->PLL1CR & ~0x30 ) | ( RCCU_PLL1_Mul_12 << 0x04 );
     RCCU->PLL1CR = ( Tmp & ~0x7 ) | RCCU_Div_2 | 0x40;

     for ( Tmp = 0x7f ; Tmp> 0; Tmp --);

    /* Select PLL1_Output as RCLK clock */

    /*RCCU_RCLKSourceConfig ( RCCU_PLL1_Output ) ;*/
     RCCU->CFR = RCCU->CFR | RCCU_CK2_16_Mask;
     if (( RCCU->PLL1CR & 0x0007 ) != 7)
     while(!(RCCU->CFR & RCCU_LOCK_Mask));
     /*  Deselect The CKAF */
     RCCU->CCR  &= ~RCCU_CKAF_SEL_Mask;
     /*  Select The CSU_CKSEL */
     RCCU->CFR |= RCCU_CSU_CKSEL_Mask;

    /* at this step the MCLK clock should be equal to 48Mhz */
}
/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Configure the PLL2 ( * 12 , / 1 ) assuming HCLK=4MHz */
  /* RCCU_PLL2Config (RCCU_PLL2_Mul_12,RCCU_Div_1)*/
  /* PCU->PLL2CR  = 0x81D0; */
}
/*******************************************************************************
* Function Name  : Enter_LowPowerMode
* Description    : Power-off system clocks and power while entering suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{

}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{

}
/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : Configures the USB interrupts
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  EIC_IRQChannelPriorityConfig(USBLP_IRQChannel,2);
  EIC_IRQChannelConfig(USBLP_IRQChannel,ENABLE);
  EIC_IRQConfig(ENABLE);
}
/*******************************************************************************
* Function Name  : Keys_Config.
* Description    : configures the Joytick Keys.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Keys_Config (void)
{
  GPIO_Config (GPIO1, 0x0300, GPIO_IN_TRI_TTL);
}

/*******************************************************************************
* Function Name  : Joystick_Send.
* Description    : Moves Joystick Cursor Position.
* Input          : Key Movement.
* Return         : None.
*******************************************************************************/
void Joystick_Send(u8 Keys)
{
  u8 Mouse_Buffer[3] = {0,0,0};
  s8 X = 0, Y = 0;

  switch (Keys)
  {
    case KEY_LEFT:
      X -= CURSOR_STEP;
      break;

    case KEY_RIGHT:
      X += CURSOR_STEP;
      break;

    case KEY_UP:
      Y -= CURSOR_STEP;
      break;

    case KEY_DOWN:
      Y += CURSOR_STEP;
      break;

      default:
	  return;
  }

  /* prepare buffer to send */
  Mouse_Buffer[1] = X;
  Mouse_Buffer[2] = Y;
  UserToPMABufferCopy(Mouse_Buffer,GetEPTxAddr(ENDP1), 3);
  /* enable endpoint for transmission */
  SetEPTxValid(ENDP1);

}
/*******************************************************************************
* Function Name  : ReadUSBKeys.
* Description    : Reads the Pushed Key (Here only Up & Down keys are used)
* Input          : None.
* Return         : Pushed key.
*******************************************************************************/
u8 ReadUSBKeys (void )
{
  /* "RIGHT" key is pressed */
  /* implement your code to manage the right direction */
  /* "LEFT" key is pressed */
  /* implement your code to manage the left direction*/
  /* "UP" key is pressed */
  if(!GPIO_BitRead(GPIO1, 9))
  {
    return KEY_UP;
  }
  /* "DOWN" key is pressed */
  if(!GPIO_BitRead(GPIO1, 8))
  {
    return KEY_DOWN;
  }
  /* No key is pressed */
  else
  {
    return KEY_NOKEY;
  }
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
