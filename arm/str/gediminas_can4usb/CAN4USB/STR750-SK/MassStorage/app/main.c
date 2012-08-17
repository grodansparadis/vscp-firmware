/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : main.c
 *    Description : main module
 *
 *    History :
 *    1. Date        : September, 20 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 * Jumpers:
 *  PWR_SEL         - depend of power source
 *  VRE             - Absence
 *  BOOT0           - 0
 *  BOOT1           - 0
 *  SPI1_NSS/AIN5   - SPI1_NSS
 *  Jumper array J5 - 2-3 (near prototype area)
 *  Jumper array J6 - 2-3 (near prototype area)
 *
 *   This example project shows how to use the IAR Embedded Workbench for ARM
 *  to develop code for the IAR-STR750-SK evaluation board. It implements a
 *  MMC/SD card drive. The first free drive letters will be used. For example,
 *  if your PC configuration includes two hard disk partitions (in C:\ and D:\)
 *  and a CD-ROM drive (in E:\), the memory card drive will appear as F:\.
 *   The MassStorage example project works standalone on the IAR-STR750-SK board.
 *   The LCD backlight will indicate drive activity.
 *
 *    $Revision: 18265 $
 **************************************************************************/
#include "includes.h"
#define TICK_PER_SECOND       2
#define UPDATE_SHOW_DLY       ((Int32U)(1*TICK_PER_SECOND))
#define STARTUP_SHOW_DLY      ((Int32U)(5 *TICK_PER_SECOND))

const Int8U HexToCharStr [] = "0123456789ABCDEF";
volatile Boolean TickSysFlag;
volatile Int32U DlyCount;

#pragma data_alignment=4
__no_init Int8U Lun0Buffer[2048];

#ifndef DEBUG_KS
/*************************************************************************
 * Function Name: Tim0Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 0 interrupt handler
 *
 *************************************************************************/
void Tim0Handler (void)
{
  if(DlyCount)
  {
    --DlyCount;
  }
  // Clear the Update pending Bit
  TIM_ClearITPendingBit(TIM0, TIM_IT_Update);
}
#endif // DEBUG_KS

/*************************************************************************
 * Function Name: Tim1Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 1 interrupt handler
 *
 *************************************************************************/
void Tim1Handler (void)
{
  TickSysFlag = TRUE;
  // Clear the Update pending Bit
  TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
}

/*************************************************************************
 * Function Name: InitClock
 * Parameters: none
 *
 * Return: none
 *
 * Description: Init MCU clock
 *
 *************************************************************************/
void InitClock (void)
{
  // Clock
  // MRCC system reset(for debug purpose)
  MRCC_DeInit();
  // Wait for OSC4M startup
  MRCC_WaitForOSC4MStartUp();
  // Set HCLK
  MRCC_HCLKConfig(MRCC_CKSYS_Div1);
  // Set CKTIM
  MRCC_CKTIMConfig(MRCC_HCLK_Div1);
  // Set PCLK
  MRCC_PCLKConfig(MRCC_CKTIM_Div2);
  // Set CKSYS
  MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  // GPIO pins optimized for 3V3 operation
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);
}

#ifndef DEBUG_KS
/*************************************************************************
 * Function Name: Dly100us
 * Parameters: void *arg
 *
 * Return: none
 *
 * Description: Delay 100us * arg
 *
 *************************************************************************/
void Dly100us(void *arg)
{
  DlyCount = (Int32U)arg;

  // Clear TIM0 flags
  TIM_ClearFlag(TIM0, TIM_FLAG_Update);

  // Enable TIM0 Update interrupt
  TIM_ITConfig(TIM0, TIM_IT_Update, ENABLE);

  // Enable TIM0 counter
  TIM_Cmd(TIM0, ENABLE);
  while(DlyCount);

  // Disable TIM0 Update interrupt
  TIM_ITConfig(TIM0, TIM_IT_Update, DISABLE);

  // Disable TIM0 counter
  TIM_Cmd(TIM0, DISABLE);
}

/*************************************************************************
 * Function Name: InitDlyTimer
 * Parameters: Int32U IntrPriority
 *
 * Return: none
 *
 * Description: Init Delay Timer (TIM 0)
 *
 *************************************************************************/
void InitDlyTimer (Int32U IntrPriority)
{
MRCC_ClocksTypeDef MRCC_Clocks;
TIM_InitTypeDef TIM_InitStructure;
EIC_IRQInitTypeDef EIC_IRQInitStructure;

  // Enable TIM0 clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM0, ENABLE);
  // Release TIM0 reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM0,DISABLE);

  // Timer 0
  // TIM Configuration in Output Compare Timing Mode period 100us
  // Calculate counter value
  MRCC_GetClocksStatus(&MRCC_Clocks);

  TIM_InitStructure.TIM_Period = (MRCC_Clocks.CKTIM_Frequency / 10000);
  TIM_InitStructure.TIM_Prescaler = 0;
  TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;
  TIM_Init(TIM0, &TIM_InitStructure);

  // Enable and configure the priority of the TIM0 Output compare IRQ Channel
  EIC_IRQInitStructure.EIC_IRQChannel = TIM0_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrPriority;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);
}
#endif // DEBUG_KS

/*************************************************************************
 * Function Name: InitSysTimer
 * Parameters: Int32U IntrPriority
 *
 * Return: none
 *
 * Description: Init Delay Timer (TIM 1)
 *
 *************************************************************************/
void InitSysTimer (Int32U IntrPriority)
{
MRCC_ClocksTypeDef MRCC_Clocks;
TIM_InitTypeDef TIM_InitStructure;
EIC_IRQInitTypeDef EIC_IRQInitStructure;

  // Enable TIM1 clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM1, ENABLE);
  // Release TIM1 reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM1,DISABLE);

  // Timer 1
  // TIM Configuration in Output Compare Timing Mode period 200 msec
  // Calculate counter value
  MRCC_GetClocksStatus(&MRCC_Clocks);

  TIM_InitStructure.TIM_Period = MRCC_Clocks.CKTIM_Frequency / (TICK_PER_SECOND*200);
  TIM_InitStructure.TIM_Prescaler = 200-1;
  TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;
  TIM_Init(TIM1, &TIM_InitStructure);

  // Enable and configure the priority of the TIM1 Output compare IRQ Channel
  EIC_IRQInitStructure.EIC_IRQChannel = TIM1_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = IntrPriority;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  // Clear TIM1 flags
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);
  // Enable TIM1 Update interrupt
  TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
  // Enable TIM1 counter
  TIM_Cmd(TIM1, ENABLE);
}

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
int main()
{
#ifndef DEBUG_KS
Boolean flag;  // flag is TRUE if the conversion has started
Int32U Dly;
Int8U Message[17];
Int32U Tmp, Tmp1;
DiskStatusCode_t StatusHold = (DiskStatusCode_t) -1;
Int32U USB_StateHold = (Int32U)-1;
#else
GPIO_InitTypeDef  GPIO_InitStructure;
#endif // DEBUG_KS

  // Set Burst mode
  CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);
  // MCU clock init
  InitClock();
  // EIC init
  EIC_DeInit();
#ifndef DEBUG_KS
  // Delay timer init
  InitDlyTimer(1);
#endif // DEBUG_KS
  // Delay timer init
  InitSysTimer(2);
  // Init USB
  USB_Init(5,4,UsbClassBotConfigure);
  // LUNs Init
  LunInit(MMC_DISK_LUN,MmcDiskInit,MmcDiskInfo,MmcDiskStatus,MmcDiskIO);
  // Init SCSI module
  ScsiInit();
  // Soft connection enable
  USB_ConnectRes(TRUE);
  // Enable interrupts
  EIC_IRQCmd(ENABLE);
  __enable_interrupt();

#ifndef DEBUG_KS
  // LCD Powerup init
  HD44780_PowerUpInit();
  // Show message on LCD
  HD44780_StrShow(1, 1,  "IAR Systems ARM ");
  HD44780_StrShow(1, 2,  "USB Mass Storage");
  // Set the startup message time out
  Dly = STARTUP_SHOW_DLY;
#else
  // Enable GPIO clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
  // Release GPIO reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_GPIO,DISABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = LCD_LIGHT;
  GPIO_Init(LCD_LIGHT_PORT, &GPIO_InitStructure);
#endif // DEBUG_KS

  while(1)
  {

    for(Int32U i = 0; i < SCSI_LUN_NUMB; i++)
    {
      // Implement LUNs messages
      if(LunImp(i))
      {
        LCD_LIGHT_ON();
      }
      else
      {
        LCD_LIGHT_OFF();
      }
    }
    if (TickSysFlag)
    {
      TickSysFlag = 0;
      // Update MMC/SD card status
      MmcStatusUpdate();
    #ifndef DEBUG_KS
      if(Dly-- == 0)
      {
        // LCD show
        Dly = UPDATE_SHOW_DLY;
        Tmp = UsbCoreReq(UsbCoreReqDevState);
        if(USB_StateHold != Tmp)
        {
          // Current state of USB show
          if((USB_StateHold = Tmp) == UsbDevStatusConfigured)
          {
            if(UsbCoreReq(UsbCoreReqDevSusState))
            {
              strcpy((char*)Message,"USB - Suspend   ");
            }
            else
            {
              strcpy((char*)Message,"USB - Connect   ");
            }
          }
          else
          {
            strcpy((char*)Message,"Pls, Insert USB ");
          }
          HD44780_StrShow(1, 1, (pInt8S)Message);
        }
        // Current state of MMC/SD show
        pDiskCtrlBlk_t pMMCDiskStatus = MmcDiskStatus();
        if(StatusHold != pMMCDiskStatus->DiskStatus)
        {
          flag=FALSE;
          StatusHold = pMMCDiskStatus->DiskStatus;
          switch (pMMCDiskStatus->DiskStatus)
          {
          case DiskCommandPass:
            switch(pMMCDiskStatus->DiskType)
            {
            case DiskMMC:
              strcpy((char*)Message,"MMC Card - ");
              break;
            case DiskSD:
              strcpy((char*)Message,"SD Card - ");
              break;
            default:
              strcpy((char*)Message,"Card - ");
            }
            // Calculate MMC/SD size [MB]
            Tmp  = pMMCDiskStatus->BlockNumb * pMMCDiskStatus->BlockSize;
            Tmp  = Tmp/1000000;
            Tmp1 = Tmp/1000;
            if(Tmp1)
            {
              Message[strlen((char*)Message)+1] = 0;
              Message[strlen((char*)Message)]   = HexToCharStr[Tmp1];
              Tmp %= 1000;
              flag = TRUE;
            }
            Tmp1 = Tmp/100;
            if(Tmp1 || flag)
            {
              Message[strlen((char*)Message)+1] = 0;
              Message[strlen((char*)Message)]   = HexToCharStr[Tmp1];
              Tmp %= 100;
              flag = TRUE;
            }
            Tmp1 = Tmp/10;
            if(Tmp1 || flag)
            {
              Message[strlen((char*)Message)+1] = 0;
              Message[strlen((char*)Message)]   = HexToCharStr[Tmp1];
              Tmp %= 10;
              flag = TRUE;
            }
            if(Tmp || flag)
            {
              Message[strlen((char*)Message)+1] = 0;
              Message[strlen((char*)Message)]   = HexToCharStr[Tmp];
            }
            strcat((char*)Message,"MB");
            for(Int32U i = strlen((char*)Message); i < 16; ++i)
            {
              Message[i] = ' ';
            }
            Message[strlen((char*)Message)+1] = 0;
            break;
          default:
            strcpy((char*)Message,"Pls, Insert Card");
          }
          HD44780_StrShow(1, 2, (pInt8S)Message);
        }
      }
    #endif // DEBUG_KS
    }
  }
}
