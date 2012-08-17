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
u32 Mass_Memory_Size;
u32 Mass_Block_Size;
u32 Mass_Block_Count;
GPIO_InitTypeDef GPIO_InitStructure;
/* External variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
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
  SCU_MCLKSourceConfig(SCU_MCLK_OSC);
  FMI_Config(FMI_READ_WAIT_STATE_2,FMI_WRITE_WAIT_STATE_0, FMI_PWD_ENABLE,\
             FMI_LVD_ENABLE,FMI_FREQ_HIGH);
  SCU_PLLFactorsConfig(192,25,2);
  SCU_PLLCmd(ENABLE);
  SCU_MCLKSourceConfig(SCU_MCLK_PLL);

  /*Enable VIC clock*/
  SCU_AHBPeriphClockConfig(__VIC,ENABLE);
  SCU_AHBPeriphReset(__VIC,DISABLE);
  /*USB clock = MCLK= 48MHz*/
  SCU_USBCLKConfig(SCU_USBCLK_MCLK2);
  /*Enable USB clock*/
  SCU_AHBPeriphClockConfig(__USB,ENABLE);
  SCU_AHBPeriphReset(__USB,DISABLE);

  SCU_AHBPeriphClockConfig(__USB48M,ENABLE);

    /*Enable RTC clock*/
  SCU_APBPeriphClockConfig(__RTC,ENABLE);
  SCU_APBPeriphReset(__RTC,DISABLE);

  /* GPIO 9 clock source enable */
  SCU_APBPeriphClockConfig(__GPIO9, ENABLE);

  RTC_SRAMBattPowerCmd(ENABLE);

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

  /*initialize the VIC*/
  VIC_DeInit();
}
/*******************************************************************************
* Function Name  : Get_Medium_Characteristics
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_Medium_Characteristics(void)
{
Mass_Memory_Size = 0x00015800;      /* 86KBytes */
Mass_Block_Size =  0x00000200;
Mass_Block_Count   =   (Mass_Memory_Size / Mass_Block_Size);
}
/*******************************************************************************
* Function Name  : Led_Config
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Led_Config(void)
{
  GPIO_DeInit(GPIO9);
  /* GPIO9 pin configuration */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Direction = GPIO_PinOutput;
  GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull;
  GPIO_Init(GPIO9, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : Power_LED_ON
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Power_LED_ON(void)
{
}
/*******************************************************************************
* Function Name  : Power_LED_OFF
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Power_LED_OFF(void)
{
}
/*******************************************************************************
* Function Name  : Led_RW_ON
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Led_RW_ON(void)
{
  GPIO_Write(GPIO9,LED_ON);
}
/*******************************************************************************
* Function Name  : Led_RW_OFF
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Led_RW_OFF(void)
{
  GPIO_Write(GPIO9,LED_OFF);
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
* Function Name  : Config_USB_Interrupt
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Config_USB_Interrupt(void)
{
  VIC_DeInit();
  VIC_Config(USBHP_ITLine, VIC_IRQ, 0);
  VIC_ITCmd(USBHP_ITLine, ENABLE);
  VIC_Config(USBLP_ITLine, VIC_IRQ, 1);
  VIC_ITCmd(USBLP_ITLine, ENABLE);
}
/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


