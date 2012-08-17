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
#include "USB_lib.h"
#include "HW_config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
DMA_InitTypeDef  DMA_InitStruct;
vu32 Linked_List_Descriptor_Table[12];
u8 Stream_Buff0[96];
u8 Stream_Buff1[96];
/* External variables --------------------------------------------------------*/
extern u32 Stream_Buffer_Selector;
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
  /* USB clock = MCLK/2 = 48MHz*/
  SCU_USBCLKConfig(SCU_USBCLK_MCLK2);

  SCU_PLLFactorsConfig(192, 25, 2); /* Configure Factors FPLL = 96MHz */
  SCU_PLLCmd(ENABLE);
  SCU_MCLKSourceConfig(SCU_MCLK_PLL);

  /* Enable USB clock */
  SCU_AHBPeriphClockConfig(__USB, ENABLE);
  SCU_AHBPeriphReset(__USB, DISABLE);

  SCU_AHBPeriphClockConfig(__USB48M, ENABLE);
  /*------------------ Peripheral Clock Enable ------------------*/
  SCU_APBPeriphClockConfig(__TIM01, ENABLE);
  SCU_APBPeriphReset(__TIM01, DISABLE);

  SCU_APBPeriphClockConfig(__TIM23, ENABLE);
  SCU_APBPeriphReset(__TIM23, DISABLE);

  /*Enable VIC clock*/
  SCU_AHBPeriphClockConfig(__VIC,ENABLE);
  SCU_AHBPeriphReset(__VIC,DISABLE);
  /*USB clock = MCLK= 48MHz*/
  SCU_USBCLKConfig(SCU_USBCLK_MCLK2);
  /*Enable USB clock*/
  SCU_AHBPeriphClockConfig(__USB,ENABLE);
  SCU_AHBPeriphReset(__USB,DISABLE);

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

  /* Enable WIU clock */
  SCU_APBPeriphClockConfig(__WIU, ENABLE);
  SCU_APBPeriphReset(__WIU, DISABLE);

  SCU_AHBPeriphClockConfig(__DMA,ENABLE); /* Enable the clock for DMA*/
  SCU_AHBPeriphReset(__DMA, DISABLE);

  DMA_Config(); /*Configure the DMA with the first linked list*/


  SCU_APBPeriphClockConfig(__GPIO9 ,ENABLE);
  SCU_APBPeriphReset(__GPIO9,DISABLE);
  
  
  VIC_DeInit();
  VIC_Config(TIM0_ITLine, VIC_IRQ, 3);
  VIC_ITCmd(TIM0_ITLine, ENABLE);
  VIC_Config(DMA_ITLine, VIC_IRQ, 1);
  VIC_ITCmd(DMA_ITLine, ENABLE);
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
  VIC_Config(USBHP_ITLine, VIC_IRQ, 0);
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
void Speaker_Timer_Config()
{
  TIM_InitTypeDef      TIM_InitStructure;

  GPIO_DeInit(GPIO4);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Direction = GPIO_PinOutput;
  GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull;
  GPIO_InitStructure.GPIO_IPConnected = GPIO_IPConnected_Disable;
  GPIO_InitStructure.GPIO_Alternate = GPIO_OutputAlt2;
  GPIO_Init(GPIO4,&GPIO_InitStructure);

  TIM_StructInit(&TIM_InitStructure);

   /*22.050 KHz generation*/
  TIM_InitStructure.TIM_Mode = TIM_OCM_CHANNEL_1;
  TIM_InitStructure.TIM_OC1_Modes = TIM_TIMING;
  TIM_InitStructure.TIM_Clock_Source = TIM_CLK_APB;
  TIM_InitStructure.TIM_Prescaler = 0x1;
  TIM_InitStructure.TIM_Pulse_Length_1 = TIMER_PULSE_LENGTH_24KHZ;

  /* Initialize the Timer 0 */
  TIM_Init (TIM0, &TIM_InitStructure);

  TIM_StructInit(&TIM_InitStructure);

  /* PWM configuration */
  TIM_InitStructure.TIM_Mode = TIM_PWM;
  TIM_InitStructure.TIM_Clock_Source = TIM_CLK_APB;
  TIM_InitStructure.TIM_Prescaler = 0x0;
  TIM_InitStructure.TIM_Pulse_Level_1 = TIM_HIGH;
  TIM_InitStructure.TIM_Period_Level = TIM_LOW;
  TIM_InitStructure.TIM_Pulse_Length_1 = 0x7F;
  TIM_InitStructure.TIM_Full_Period = 0xFF;

 /* Initialize the Timer 3 */
 TIM_Init (TIM3, &TIM_InitStructure);
 /* Enable the Timer Overflow interrupt */
 TIM_ITConfig(TIM0, TIM_IT_OC1, ENABLE);
 /* Start the Timer counter */
 TIM_CounterCmd(TIM3, TIM_START);
 /* Start the Timer counter */
 TIM_CounterCmd(TIM0, TIM_START);
}
/*******************************************************************************
* Function Name  : DMA_Config
* Description    : Configure the DMA and set the first linked List descriptor
*                 table.
* Input          : None.
* Return         : None.
*******************************************************************************/
void DMA_Config(void)
{
  DMA_DeInit(); /* DMA default configuration : Reset configuration*/
  DMA_Cmd(ENABLE);/* Enable the DMA*/
  DMA_StructInit(&DMA_InitStruct);
  DMA_InitStruct.DMA_Channel_LLstItm=(u32) (&Linked_List_Descriptor_Table);
  DMA_InitStruct.DMA_Channel_SrcAdd=(u32)(PMAAddr+(u32)((GetEPDblBuf0Addr(ENDP1)))); /* source address Endpoint 1 Buffer 0*/
  DMA_InitStruct.DMA_Channel_DesAdd=(u32)(&Stream_Buff0); /* Destination address  */
  DMA_InitStruct.DMA_Channel_SrcWidth= DMA_SrcWidth_Word;/* The source bus width is a word (32bits)*/
  DMA_InitStruct.DMA_Channel_DesWidth= DMA_DesWidth_Word; /* The Destination bus width is a word (32bits)*/
  DMA_InitStruct.DMA_Channel_FlowCntrl=DMA_FlowCntrl_Perip2; /* USB is The flow controller*/
  DMA_InitStruct.DMA_Channel_Src= DMA_SRC_USB_RX;
  DMA_InitStruct.DMA_Channel_TrsfSize =0; /* Transfer size*/
  /* Configure the DMA channel0  */
  DMA_Init(DMA_Channel0,&DMA_InitStruct);/* update the DMA channel0 registers with the LLI structure*/

  DMALinkedModeRxEnable();
  DMASynchEnable();
  DMALinkedModeRxConfig(ENDP1);
  SetDMALLIRxPacketNum(1);
  SetDMALLIRxLength(0x4);
  DMA_ChannelCmd (DMA_Channel0,ENABLE);/*Enable the DMA channel*/
  DMA_ChannelDESIncConfig (DMA_Channel0, ENABLE);
  DMA_ChannelSRCIncConfig (DMA_Channel0, ENABLE);
  DMA_ITMaskConfig(DMA_Channel0, DMA_ITMask_ITC, ENABLE);

  Linked_List_Descriptor_Table[0]= (u32)(PMAAddr+(u32)((GetEPDblBuf1Addr(ENDP1))));
  Linked_List_Descriptor_Table[1]= (u32)(&Stream_Buff0[24]);
  Linked_List_Descriptor_Table[2]= (u32)(&Linked_List_Descriptor_Table[4]);
  Linked_List_Descriptor_Table[3]= 0x0C480000;
  Linked_List_Descriptor_Table[4]= (u32)(PMAAddr+(u32)((GetEPDblBuf0Addr(ENDP1))));
  Linked_List_Descriptor_Table[5]= (u32)(&Stream_Buff0[48]);
  Linked_List_Descriptor_Table[6]= (u32)(&Linked_List_Descriptor_Table[8]);
  Linked_List_Descriptor_Table[7]= 0x0C480000;
  Linked_List_Descriptor_Table[8]= (u32)(PMAAddr+(u32)((GetEPDblBuf1Addr(ENDP1))));
  Linked_List_Descriptor_Table[9]= (u32)(&Stream_Buff0[72]);
  Linked_List_Descriptor_Table[10]= 0;
  Linked_List_Descriptor_Table[11]= 0x8C480000;
}
/*******************************************************************************
* Function Name  : Set_DMA_List
* Description    : Set the DMA Linked List with the Stream_Buffx
*                 x can be 0 or 1 according the used destination buffer
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_DMA_List(void)
{
  DMA_DeInit(); /* DMA default configuration : Reset configuration*/
  DMA_Cmd(ENABLE);/* Enable the DMA*/
  
  
  if(Stream_Buffer_Selector)
  {
    DMA_InitStruct.DMA_Channel_DesAdd =(u32)(&Stream_Buff1); /* Destination address Stream_Buff1 */
    Linked_List_Descriptor_Table[1]= (u32)(&Stream_Buff1[24]);
    Linked_List_Descriptor_Table[5]= (u32)(&Stream_Buff1[48]);
    Linked_List_Descriptor_Table[9]= (u32)(&Stream_Buff1[72]);
  }
  else
  {
    DMA_InitStruct.DMA_Channel_DesAdd=(u32)(&Stream_Buff0); /* Destination address Stream_Buff0 */
    Linked_List_Descriptor_Table[1]= (u32)(&Stream_Buff0[24]);
    Linked_List_Descriptor_Table[5]= (u32)(&Stream_Buff0[48]);
    Linked_List_Descriptor_Table[9]= (u32)(&Stream_Buff0[72]);
  }

  /* Configure the DMA channel0  */
  DMA_Init(DMA_Channel0,&DMA_InitStruct);/* update the DMA channel0 registers with the LLI structure*/

  DMALinkedModeRxEnable();
  DMASynchEnable();
  SetDMALLIRxPacketNum(1);
  SetDMALLIRxLength(0x4);
  DMA_ChannelCmd (DMA_Channel0,ENABLE);/*Enable the DMA channel*/
  DMA_ChannelDESIncConfig (DMA_Channel0, ENABLE);
  DMA_ChannelSRCIncConfig (DMA_Channel0, ENABLE);
  DMA_ITMaskConfig(DMA_Channel0, DMA_ITMask_ITC, ENABLE);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
