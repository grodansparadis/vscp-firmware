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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FLASH_WMS_Mask   0x80000000
#define FLASH_WPG_Mask   0x20000000
#define FLASH_SER_Mask   0x08000000
#define FLASH_SPR_Mask   0x01000000

#define FLASH_CMD_WEN     0x06
#define FLASH_CMD_SE      0xD8
#define FLASH_CMD_BE      0xC7
#define FLASH_CMD_RDID    0x9F

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FLASHR_TypeDef       *FLASHR;
FLASHPR_TypeDef      *FLASHPR;

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

  FLASHR = (FLASHR_TypeDef *)  FLASHR_BASE;
  FLASHPR = (FLASHPR_TypeDef *) FLASHPR_BASE;

  /* MRCC configuration --------------------------------------------------------*/
  MRCC_DeInit();

  /* Enable GPIOs, USB and EXTIT clocks */

   MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO | MRCC_Peripheral_EXTIT | MRCC_Peripheral_USB, ENABLE);


  /* Clear No Clock Detected flag */
  MRCC_ClearFlag(MRCC_FLAG_NCKD);

  /* Enbale No Clock Detected interrupt */
  MRCC_ITConfig(MRCC_IT_NCKD, ENABLE);

  /* Set HCLK to 30 MHz */
  MRCC_HCLKConfig(MRCC_CKSYS_Div2);

  /* Set CKTIM to 30 MHz */
  MRCC_CKTIMConfig(MRCC_HCLK_Div1);

  /* Set PCLK to 30 MHz */
  MRCC_PCLKConfig(MRCC_CKTIM_Div1);

  /* Enable FLASH burst mode */
  CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);

  /* Set CKSYS to 60 MHz */
  MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);

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
  

  GPIO_WriteBit(GPIO0, GPIO_Pin_9, (BitAction)(1));

  SMI_FLASH_Init();

  GPIO_WriteBit(GPIO0, GPIO_Pin_9, (BitAction)(0));


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
* Function Name  : DFU_Button_Config
* Description    : Configures the DFU selector Button to enter DFU Mode
* Input          : None.
* Return         : None.
*******************************************************************************/
void DFU_Button_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure1; 

  /* Enable GPIO Clock */
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
  
  /* Configure P1.5 as input */
  GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIO1, &GPIO_InitStructure1);  
}

/*******************************************************************************
* Function Name  : DFU_Button_Read
* Description    : Reads the DFU selector Button to enter DFU Mode
* Input          : None.
* Return         : Status
*******************************************************************************/

u8 DFU_Button_Read (void)
{
  return GPIO_ReadBit(GPIO1, GPIO_Pin_5);

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
* Function Name  : Reset_Device
* Description    : Reset the device using the watchdog Reset
* Input          : None.
* Return         : None.
*******************************************************************************/

void Reset_Device(void)
{
   WDG_InitTypeDef WDG_InitStructure;
  /* Soft USB disconnection */
  USB_Cable_Config(DISABLE);
  /* Configure the WDG to generate a system reset signal */
  WDG_InitStructure.WDG_Mode = WDG_Mode_WDG;
  WDG_InitStructure.WDG_Preload = 0x7FF;
  WDG_InitStructure.WDG_Prescaler = 0xFF;
  WDG_Init(&WDG_InitStructure);

  /* Enable the WDG */
  WDG_Cmd(ENABLE);

}





/*******************************************************************************
* Function Name  : Internal_FLASH_WordWrite
* Description    : Writes a Word in the Flash (To be executed from RAM)
* Input 1        : Address of the Destination.
* Input 2        : Word to program at Address.
* Return         : None.
*******************************************************************************/
ARM_exe void Internal_FLASH_WordWrite(u32 XtargetAdd, u32 Xdata)
{
  /* Set the Word Programming bit 'WPG' in the CR0 Reg */
  FLASHR->CR0 |= FLASH_WPG_Mask;
  /* Load the destination address in AR */
  FLASHR->AR   = XtargetAdd;
  /* Load DATA to be programmed in DR0 */
  FLASHR->DR0  = Xdata;
  /* Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Write Operation */
  FLASHR->CR0 |= FLASH_WMS_Mask;
  /* Wait until the write operation is completed */
  while((FLASHR->CR0 & 0x16) != 0);
}
/*******************************************************************************
* Function Name  : Internal_FLASH_SectorErase
* Description    : Erases a Flash sector (To be executed from RAM)
* Input 1        : Sectors to be Erased.
* Return         : None.
*******************************************************************************/
ARM_exe void Internal_FLASH_SectorErase(u32 Xsectors)
{
  /* Wait until another operation going on is completed */
  while (FLASHR->CR0&0x16);
  /* Set the Sector Erase flag 'SER' in the CRO reg */
  FLASHR->CR0 |= FLASH_SER_Mask;
  /* Select in the CR1 register the Sectors to be erased  */
  FLASHR->CR1 |= Xsectors;
  /* Set the Write Mode Start bit 'WMS' in the CR0 Reg to Start Erase Operation */
  FLASHR->CR0 |= FLASH_WMS_Mask;
  /* Wait until the erase operation is completed */
  while (FLASHR->CR0&0x16);
}

/*******************************************************************************
* Function Name  : FLASH_WritePrConfig
* Description    : Enable Write protection or Disable temporarily Write
*                  protection of a flash sector. (To be executed from RAM)
* Input 1        : Flash Sector.
* Input 2        : Enable or disable Flash sector Write Protection.
* Return         : None.
*******************************************************************************/
ARM_exe void Internal_FLASH_WritePrConfig(u32 Xsectors, FunctionalState NewState)
{

  u32 TmpProtection;

  TmpProtection = FLASHPR->NVWPAR;
  /* Wait until another operation going on is completed */
   while (FLASHR->CR0&0x16);

  if (NewState == DISABLE) TmpProtection |= Xsectors;
  else TmpProtection &= ~Xsectors;

  /* Set the Set protection Bit */
  FLASHR->CR0 |= FLASH_SPR_Mask;
  /* Set the Register Address   */
  FLASHR->AR  = 0x2010DFB0;
  /* Data To be Programmed to the Protection Register */
  FLASHR->DR0  = TmpProtection;
  /* Set the WMS bit to Start the Sequence */
  FLASHR->CR0 |= FLASH_WMS_Mask;

  /* Wait until another operation going on is completed */
   while (FLASHR->CR0&0x16);

}
/*******************************************************************************
* Function Name  : Internal_FLASH_SectorMask
* Description    : Selects the internal Flash sector mask
* Input          : Address
* Return         : Sector mask
*******************************************************************************/

u32 Internal_FLASH_SectorMask(u32 Address)
{
  u32 ReturnValue = 0;
  switch (Address)
    {
    case 0x20000000: ReturnValue = 0x01;
                     break;
    case 0x20002000: ReturnValue = 0x02;
                     break;
    case 0x20004000: ReturnValue = 0x04;
                     break;
    case 0x20006000: ReturnValue = 0x08;
                     break;
    case 0x20008000: ReturnValue = 0x10;
                     break;
    case 0x20010000: ReturnValue = 0x20;
                     break;
    case 0x20020000: ReturnValue = 0x40;
                     break;
    case 0x20030000: ReturnValue = 0x80;
                     break;
    case 0x200C0000: ReturnValue = 0x10000;
                     break;
    case 0x200C2000: ReturnValue = 0x20000;
                     break;
    default : ReturnValue = 0;
                     break;
    }
  return (ReturnValue);
}

/*******************************************************************************
* Function Name  : SMI_FLASH_Init
* Description    : Configures SMI and GPIOs. This function must be called before
*                  any write/read operation on the FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_Init(void)
{
  SMI_InitTypeDef  SMI_InitStructure;

  SMI_InitStructure.SMI_ClockHold = 0;
  SMI_InitStructure.SMI_Prescaler = 2;
  SMI_InitStructure.SMI_DeselectTime = 1;
  SMI_Init(&SMI_InitStructure);

  /* Enable SMI Alternate Functions */
  GPIO_PinRemapConfig(GPIO_Remap_SMI_EN, ENABLE);

  /* Enable Bank 0 */
  SMI_BankCmd(SMI_Bank_0, ENABLE);

  /* Select Bank 0 */
  SMI_SelectBank(SMI_Bank_0);

  /* Enable SMI Flash Fast Read mode */
  SMI_FastReadConfig(SMI_FastRead_Enable);
}

/*******************************************************************************
* Function Name  : SMI_FLASH_SectorErase
* Description    : Erases the specified FLASH sector.
* Input          : - SectorAddr: FLASH's sector address to erase.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_SectorErase(u32 SectorAddr)
{
  u32 Temp = 0, Cmd = 0;

  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

/* SMI in SW mode ------------------------------------------------------------*/
  SMI_ModeConfig(SMI_Mode_SW);

  /* Transmit one byte to external memory */
  SMI_TxRxLengthConfig(SMI_TxLength_1Byte, SMI_RxLength_0Bytes);


  /* Send Write Enable command */
  SMI_SendCmd(FLASH_CMD_WEN);
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Send Read Status Register command */
  SMI_SendRSRCmd();
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Transmit four bytes to external memory */
  SMI_TxRxLengthConfig(SMI_TxLength_4Bytes, SMI_RxLength_0Bytes);

  /* Send Sector Erase command followed by three address bytes */
  Temp = SectorAddr >> 16;
  Cmd = (Temp << 8) | FLASH_CMD_SE;
  SMI_SendCmd(Cmd);
  while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Wait while the FLASH is busy with Erase cycle */
  do
  {
    SMI_SendRSRCmd();
    while(SMI_GetFlagStatus(SMI_FLAG_TF) == RESET);
    SMI_ClearFlag(SMI_FLAG_TF);
  } while((SMI_ReadMemoryStatusRegister() & 0x01) != RESET);

/* SMI in HW mode ------------------------------------------------------------*/
  SMI_ModeConfig(SMI_Mode_HW);
}

/*******************************************************************************
* Function Name  : SMI_FLASH_WordWrite
* Description    : Writes a word  to the selected memory Bank.
* Input          : - Address : external memory address from which the data will
*                    be written.
*                  - Data : word to write.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_WordWrite(u32 Address, u32 Data )
{   	
  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

  /* Send Write Enable command*/
  SMI_SendWENCmd();
  SMI_ClearFlag(SMI_FLAG_TF);


  /* Transfer data to the memory */
    *(vu32 *) Address = Data ;

  /* Wait while the FLASH is busy with Write cycle */
  while((SMI_GetFlagStatus(SMI_FLAG_WC)) == RESET);
  SMI_ClearFlag(SMI_FLAG_WC);
}

/*******************************************************************************
* Function Name  : SMI_FLASH_PageWrite
* Description    : Writes a Page of 256 to the selected memory Bank.
* Input          : - Address : external memory address from which the data will
*                    be written.
*                  - wBuffer : pointer to buffer.
* Output         : None
* Return         : None
*******************************************************************************/
void SMI_FLASH_PageWrite(u32 Address, u32 * wBuffer )
{   	
  vu32 wCounter;
  /* Clear ERF1 and ERF2 flags */
  SMI_ClearFlag(SMI_FLAG_ERF2 | SMI_FLAG_ERF1);

  /* Send Write Enable command*/
  SMI_SendWENCmd();
  SMI_ClearFlag(SMI_FLAG_TF);

  /* Enable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Enable);

  for(wCounter =64; wCounter != 0; wCounter--) /* while there is data to write */
  {
    /* Transfer data to the memory */
    *(vu32 *) Address = *wBuffer++;

    /* Increment the address*/
    Address += 4;
  }	

  /* Disable Burst Mode */
  SMI_WriteBurstConfig(SMI_WriteBurst_Disable);

  /* Wait while the FLASH is busy with Write cycle */
  while((SMI_GetFlagStatus(SMI_FLAG_WC)) == RESET);
  SMI_ClearFlag(SMI_FLAG_WC);
}

/******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****/
