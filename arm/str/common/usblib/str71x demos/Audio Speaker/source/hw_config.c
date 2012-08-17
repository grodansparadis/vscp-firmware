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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : Set_System
* Description    : Set System clock.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{
 u32 Tmp;

 /* Configure CLK = CLK2 : Disable Div2*/
  RCCU_Div2Config (ENABLE);

  RCCU_MCLKConfig ( RCCU_DEFAULT );/* MCLK = 64 MHz*/

  /* Configure FCLK = RCLK /2*/
  /* FCLK = PCLK = 32MHz*/
   RCCU_FCLKConfig ( RCCU_RCLK_2);  /* APB1 Clock (USB) & TIMERs*/
   RCCU_PCLKConfig ( RCCU_RCLK_2 );

  /* Configure the PLL1 ( * 16 , / 2 )*/

   RCCU_PLL1Config ( RCCU_PLL1_Mul_16 , RCCU_Div_2) ;


  /* Wait PLL to lock*/

   for ( Tmp = 0x7f ; Tmp> 0; Tmp--)

  /* Select PLL1_Output as RCLK clock*/
  RCCU_RCLKSourceConfig ( RCCU_PLL1_Output ) ;

  /* End Single USB Clock */
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
      /*  RCCU->CCR  |= RCCU_CKAF_SEL_Mask;
          RCCU->CFR    &=~0x1;                  //PLL not sys clock
          RCCU->PLL1CR &=~0x80;                 //Free running off
          GPIO_Config(GPIO0,0x4010,GPIO_OUT_PP);
          GPIO_Config(GPIO1,0x8400,GPIO_OUT_PP);
          GPIO_WordWrite(GPIO0,0x0);

          FLASHR->CR0|=0x8000;                  //disable the flash during the LPWFI
          PCU->PWRCR  |=0x8000;                 //disable MVR during WFI mode
          PCU->PWRCR  |=0x0010;

          RCCU->CCR  |=0x2;                     //select the 32KHz as peripherl clock during LPWFI
          RCCU->CCR  |=0x1;

          GPIO_Config (GPIO0, 0x1000,GPIO_HI_AIN_TRI);    //Disable External oscillator P0.12

          RCCU->SMR &=0xFE;                               // enter LPWFI*/


}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : Restores system clocks and power while exiting suspend mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  /* GPIO_BitWrite(GPIO0,12,0);   */             /*Enable External oscillator*/
  /* GPIO_Config(GPIO0,0x1000,GPIO_OUT_PP);

   Set_System();
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
        EIC_IRQChannelPriorityConfig(USBHP_IRQChannel,4);
        EIC_IRQChannelPriorityConfig(USBLP_IRQChannel,5);
        EIC_IRQChannelConfig(USBHP_IRQChannel,ENABLE);
        EIC_IRQChannelConfig(USBLP_IRQChannel,ENABLE);
        EIC_IRQConfig(ENABLE);
}
/*******************************************************************************
* Function Name  : Speaker_Timer_Config
* Description    : configure and enable the timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Speaker_Timer_Config(void)
{
/* TIM0 used as system timer -------------------------------------------------*/
  /* Initialize the Timer */
  TIM_Init(TIM0);

  /* Timer clock: 32MHz/(0+1) = 32MHz */
  TIM_PrescalerConfig(TIM0, 0x00);
  TIM_ClockSourceConfig(TIM0, TIM_INTERNAL);

  /* TIM0 frequency */
  TIM_OCMPModeConfig(TIM0, TIM_CHANNEL_A, SampleRate_22050-5, TIM_TIMING, TIM_LOW);

  /* Enable TIM0 Output Compare A interrupt */
  TIM_ITConfig(TIM0, TIM_OCA_IT, ENABLE);

  /* Enable TIM0 IRQ channel */
   EIC_IRQChannelConfig(T0OC1_IRQChannel, ENABLE);
   EIC_IRQChannelPriorityConfig(T0OC1_IRQChannel,1);

/* TIM3 used in PWM mode -----------------------------------------------------*/
  /* Configure P1.02 as alternate function (T3_OCMPA) */
  GPIO_Config(GPIO1, T3_OCMPA, GPIO_AF_PP);

  /* Initialize the Timer */
  TIM_Init(TIM3);

  /* Timer clock: 32MHz/(0+1) = 32MHz */
  TIM_PrescalerConfig(TIM3, 0x0);

  /* Generate a PWM Signal: freq = 32MHz/255 = ~125.5KHz */
  TIM_PWMOModeConfig(TIM3, 128, TIM_HIGH, 255, TIM_LOW);

  /* Start TIM3 Counter */
  TIM_CounterConfig(TIM3, TIM_START);

  /* Start TIM0 Counter */
  TIM_CounterConfig(TIM0, TIM_START);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
