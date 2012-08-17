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
#include "usb_pwr.h"
/* Private typedef -----------------------------------------------------------*/
typedef  void (*pFunction)(void);
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
/* External variables --------------------------------------------------------*/
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
  SCU_MCLKSourceConfig(SCU_MCLK_OSC);
  SCU_PLLFactorsConfig(192,25,3);
  SCU_PLLCmd(ENABLE);
  SCU_MCLKSourceConfig(SCU_MCLK_PLL);


  SCU_AHBPeriphClockConfig(__VIC,ENABLE);
  SCU_AHBPeriphReset(__VIC,DISABLE);
  /*USB clock = MCLK= 48MHz*/
  SCU_USBCLKConfig(SCU_USBCLK_MCLK);
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
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz)
* Input          : None.
* Return         : None.
*******************************************************************************/

void Set_USBClock(void)
{
  /*USB clock = MCLK= 48MHz*/
  SCU_USBCLKConfig(SCU_USBCLK_MCLK);
  /*Enable USB clock*/
  SCU_AHBPeriphClockConfig(__USB,ENABLE);
  SCU_AHBPeriphReset(__USB,DISABLE);

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
* Function Name  : DFU_Button_Config
* Description    : Configures the DFU selector Button to enter DFU Mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void DFU_Button_Config(void)
{
  /* GPIO 7 clock source enable */
  SCU_APBPeriphClockConfig(__GPIO7,ENABLE);
  SCU_APBPeriphReset(__GPIO7,DISABLE);


 /*Configure GPIO7*/
  GPIO_DeInit(GPIO7);
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Direction = GPIO_PinInput;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Type = GPIO_Type_PushPull ;
  GPIO_Init (GPIO7, &GPIO_InitStructure);


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
* Function Name  : DFU_Button_Read
* Description    : Reads the DFU selector Button to enter DFU Mode
* Input          : None.
* Return         : Status
*******************************************************************************/

u32 DFU_Button_Read (void)
{
   /* Return the status of the P7.0 pin button on the STR910-EVAL*/

   return (GPIO_Read(GPIO7) & 0xE0);


}
/*******************************************************************************
* Function Name  : Reset_Device
* Description    : Reset the device using the watchdog Reset
* Input          : None.
* Return         : None.
*******************************************************************************/

ARM_exe RAM_exe void Reset_Device(void)
{
      pFunction Jump_To_Application;
    
      PowerOff();
     
      FMI->BBSR   = 0x1;
      FMI->NBBSR  = 0x6;
      FMI->BBADR  = 0x80000 >> 2;
      FMI->NBBADR = 0 ;
      



      Jump_To_Application = (pFunction) ApplicationAddress;     
      Jump_To_Application();
     
    

}
/*******************************************************************************
* Function Name  : Internal_FLASH_SectorErase
* Description    : Erases a sector in the internal Flash
* Input 1        : Address of the Destination.
* Return         : None.
*******************************************************************************/

RAM_exe void Internal_FLASH_SectorErase(u32 Address)
{


          * (vu16 *) (Address)=0x60;
          * (vu16 *) (Address)=0xD0;

          * (vu16 *) (Address)=0x20;
          * (vu16 *) (Address)=0xD0;
          * (vu16 *) (Address)=0x70;
            while (((*(vu16 *)Address) & 0x80) != 0x80);
          * (vu16 *) (Address ) = 0x50;
          * (vu16 *) (Address)= 0xFF;
}

/*******************************************************************************
* Function Name  : Internal_FLASH_WordWrite
* Description    : Writes a Word in the internal Flash
* Input 1        : Address of the Destination.
* Input 2        : Word to program at Address.
* Return         : None.
*******************************************************************************/
RAM_exe void Internal_FLASH_WordWrite(u32 Address,u32 Data)
{

  vu16 temp;
  temp = (u16)(Data&0xFFFF);
  Internal_FLASH_HalfWordWrite(Address,temp);
  temp= (u16)(Data>>16);
  Internal_FLASH_HalfWordWrite(Address+2,temp);

}
/*******************************************************************************
* Function Name  : Internal_FLASH_HalfWordWrite
* Description    : Writes a Half Word in the internal Flash
* Input 1        : Address of the Destination.
* Input 2        : Word to program at Address.
* Return         : None.
*******************************************************************************/
RAM_exe void Internal_FLASH_HalfWordWrite(u32 Address, u16 Data)
{
  /* Write the program command to the secor to be written */
  *(vu16 *)(Address & 0xFFFFFFFC) = 0x40;

  /* Write the half word to the destination address */
  *(vu16 *)Address = Data;

  /* Status register command */
  *(vu16 *)(Address & 0xFFFFFFFC) = 0x70;

  /* Wait until operation compeletion */
  while(!((*(vu16 *)(Address & 0xFFFFFFFC)) & 0x80));

  /* Reset the status register */
  *(vu16 *)(Address & 0xFFFFFFFC) = 0x50;

  /* Write the read array command */
  *(vu16 *)(Address & 0xFFFFFFFC) = 0xFF;
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/


