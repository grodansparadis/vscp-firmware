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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
ADC_InitTypeDef      ADC_InitStructure;
TIM_InitTypeDef TIM_InitStructure;
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
  /*-------------------- System Clock Config --------------------*/
  FMI_BankRemapConfig(4, 2, 0, 0x80000); /* Set Flash banks size & address */
  FMI_Config(FMI_READ_WAIT_STATE_2, FMI_WRITE_WAIT_STATE_0, FMI_PWD_ENABLE,\
             FMI_LVD_ENABLE, FMI_FREQ_HIGH); /* FMI Waite States */

  /* Set the PCLK Clock to MCLK/2 */
  SCU_PCLKDivisorConfig(SCU_PCLK_Div2);
  /* USB clock = MCLK/2 = 48MHz */
  SCU_USBCLKConfig(SCU_USBCLK_MCLK2);

  SCU_PLLFactorsConfig(192, 25, 2); /* Configure Factors FPLL = 96MHz */
  SCU_PLLCmd(ENABLE);
  SCU_MCLKSourceConfig(SCU_MCLK_PLL);

  /* Enable USB clock */
  SCU_AHBPeriphClockConfig(__USB, ENABLE);
  SCU_AHBPeriphReset(__USB, DISABLE);

  SetCNTR(0);
  SetISTR(0);
  SCU_AHBPeriphClockConfig(__USB48M, ENABLE);

  /*------------------ Peripheral Clock Enable ------------------*/

  SCU_APBPeriphClockConfig(__TIM01, ENABLE);
  SCU_APBPeriphReset(__TIM01, DISABLE);

  SCU_APBPeriphClockConfig(__TIM23, ENABLE);
  SCU_APBPeriphReset(__TIM23, DISABLE);

  SCU_APBPeriphClockConfig(__ADC, ENABLE);
  SCU_APBPeriphReset(__ADC, DISABLE);
  
  /*Enable VIC clock*/
  SCU_AHBPeriphClockConfig(__VIC,ENABLE);
  SCU_AHBPeriphReset(__VIC,DISABLE);
  /*USB clock = 48MHz*/
  SCU_USBCLKConfig(SCU_USBCLK_MCLK2);
  /*Enable USB clock*/
  SCU_AHBPeriphClockConfig(__USB,ENABLE);
  SCU_AHBPeriphReset(__USB,DISABLE);
  
  /* Enable WIU clock */
  SCU_APBPeriphClockConfig(__WIU, ENABLE);
  SCU_APBPeriphReset(__WIU, DISABLE);
    
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
  VIC_DeInit();
  VIC_Config(TIM1_ITLine, VIC_IRQ, 0);
  VIC_ITCmd(TIM1_ITLine, ENABLE);
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
* Function Name  : Enter_LowPowerMode
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{

}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode
* Description    : 
* Input          : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{

}
/*******************************************************************************
* Function Name  : USB_Interrupts_Config
* Description    : configure and enable the USB interrupt Lines
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  VIC_Config(USBHP_ITLine, VIC_IRQ, 1);
  VIC_ITCmd(USBHP_ITLine, ENABLE);
  VIC_Config(USBLP_ITLine, VIC_IRQ, 2);
  VIC_ITCmd(USBLP_ITLine, ENABLE);

}  
/*******************************************************************************
* Function Name  : Speaker_Timer_Config
* Description    : configure and enable the timer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Micro_Config(void)
{
  /* ADC Channel 7 pin configuration */
  GPIO_ANAPinConfig(GPIO_ANAChannel7, ENABLE);

  TIM_StructInit(&TIM_InitStructure);

  /* 22.050 KHz generation */
  TIM_InitStructure.TIM_Mode = TIM_OCM_CHANNEL_1;
  TIM_InitStructure.TIM_OC1_Modes = TIM_TIMING;
  TIM_InitStructure.TIM_Clock_Source = TIM_CLK_APB;
  TIM_InitStructure.TIM_Prescaler = 0x1;
  TIM_InitStructure.TIM_Pulse_Length_1 = TIMER_PULSE_LENGTH_22KHZ;

  /* Initialize the Timer 1 */
  TIM_Init (TIM1, &TIM_InitStructure);

  /* ADC configuration */
  /* ADC Structure Initialization */
  ADC_StructInit(&ADC_InitStructure);

  /* Configure the ADC in continuous mode conversion */
  ADC_InitStructure.ADC_Channel_7_Mode = ADC_NoThreshold_Conversion;
  ADC_InitStructure.ADC_Select_Channel = ADC_Channel_7;
  ADC_InitStructure.ADC_Scan_Mode = DISABLE;
  ADC_InitStructure.ADC_Conversion_Mode = ADC_Continuous_Mode;

  /* Enable the ADC */
  ADC_Cmd(ENABLE);

  /* Prescaler config */
  ADC_PrescalerConfig(0x0);

  /* Configure the ADC */
  ADC_Init(&ADC_InitStructure);
}
/*******************************************************************************
* Function Name  : Voice_Micro_Start
* Description    : start the micro
* Input          : None.
* Return         : None.
*******************************************************************************/
void Voice_Micro_Start(void)
{
  /* Start the conversion */
  ADC_ConversionCmd(ADC_Conversion_Start);
  /* Enable TIM0 update interrupt */
  TIM_ITConfig(TIM1, TIM_IT_OC1, ENABLE);
  /* Start the Timer counter */
  TIM_CounterCmd(TIM1, TIM_START);
}
/*******************************************************************************
* Function Name  : Voice_Micro_Stop
* Description    : stop the micro
* Input          : None.
* Return         : None.
*******************************************************************************/
void Voice_Micro_Stop(void)
{
  /* Disable TIM0 update interrupt */
  TIM_ITConfig(TIM1, TIM_IT_OC1, DISABLE);
  /* Stop the Timer counter */
  TIM_CounterCmd(TIM1, TIM_STOP);
  /* Stop conversion */
  ADC_ConversionCmd(ADC_Conversion_Stop);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
