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
#include "usb_lib.h"
#include "hw_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus OSC4MStartUpStatus;
GPIO_InitTypeDef    GPIO_InitStructure;
EIC_IRQInitTypeDef  EIC_IRQInitStructure;
ADC_InitTypeDef ADC_InitStructure;
TB_InitTypeDef      TB_InitStructure;
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
GPIO_InitTypeDef    GPIO_InitStructure;

  EXTIT_InitTypeDef   EXTIT_InitStructure;

 
/* MRCC configuration --------------------------------------------------------*/
  /* MRCC system reset(for debug purpose) */
  MRCC_DeInit();

  /* Wait for OSC4M start-up */
  OSC4MStartUpStatus = MRCC_WaitForOSC4MStartUp();

  if(OSC4MStartUpStatus == SUCCESS)
  {  
    /* Set HCLK to 60MHz */
    MRCC_HCLKConfig(MRCC_CKSYS_Div1);

    /* Set CKTIM to 30MHz */
    MRCC_CKTIMConfig(MRCC_HCLK_Div1);

    /* Set PCLK to 30MHz */
    MRCC_PCLKConfig(MRCC_CKTIM_Div2);

    /* Enable Flash burst mode */
    CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);
  
    /* Set CKSYS to 60MHz */
    MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  }

  /* Enbale No Clock Detected interrupt */
  MRCC_ITConfig(MRCC_IT_NCKD, ENABLE);
  
  /* Enable GPIOs, USB and EXTIT clocks */
   MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_EXTIT | MRCC_Peripheral_USB
                              | MRCC_Peripheral_ADC | MRCC_Peripheral_TB, ENABLE);

  /* Enable and configure the priority of the TB IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = TB_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 3;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
  
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
* Description    : 
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
* Description    : 
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
* Description    : configure and enable the USB interrupt Lines
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{

/* Enable and configure the priority of the USB_LP IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = USB_LP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 5;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  /* Enable and configure the priority of the USB_HP IRQ Channel*/
  EIC_IRQInitStructure.EIC_IRQChannel = USB_HP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 7;
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
* Function Name  : Micro_Config
* Description    : Microphone configuration.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Micro_Config(void)
{
  /* ADC Channel 15 pin configuration */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 ;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  /* ADC configuration */
  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_Scan;
  ADC_InitStructure.ADC_ExtTrigger = ADC_ExtTrigger_Disable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable;
  ADC_InitStructure.ADC_SamplingPrescaler = 4;
  ADC_InitStructure.ADC_ConversionPrescaler = 2;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL15;
  ADC_InitStructure.ADC_ChannelNumber = 1;
  ADC_Init(&ADC_InitStructure);

  /* Enable ADC */
  ADC_Cmd(ENABLE);
  /* Start calibration */
  ADC_StartCalibration(ADC_CalibAverage_Enable);
  
   /* TB configuration in Timing mode (CKTIM  = 60 MHz) */
  /* TB is configured to generate an update every 22 KHz */
  TB_InitStructure.TB_Mode = TB_Mode_Timing;        
  TB_InitStructure.TB_Prescaler = 2; /* TB clock = 60MHz / (2+1) = 20 MHz */
  TB_InitStructure.TB_CounterMode = TB_CounterMode_Up; 
  TB_InitStructure.TB_AutoReload = 907; /* update each:  907~ 22050Hz */        
  TB_Init(&TB_InitStructure);
  
  ADC_ConversionCmd(ADC_Conversion_Start);
  while(!ADC_GetFlagStatus(ADC_FLAG_EOC));
}
/*******************************************************************************
* Function Name  : voise_Start
* Description    : Start voice playing
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Voice_Micro_Start(void)
{
  /* Enable TB update interrupt */
  TB_ITConfig(TB_IT_Update, ENABLE);
  /* Enable TB counter */
  TB_Cmd(ENABLE);  
}
/*******************************************************************************
* Function Name  : WavePlayer_Stop
* Description    : Stop wave playing: Osiris or recorded waves
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Voice_Micro_Stop(void)
{
  /* Disable TB update interrupt */
  TB_ITConfig(TB_IT_Update, DISABLE);
  /* Disable TB counter */
  TB_Cmd(DISABLE);    
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
