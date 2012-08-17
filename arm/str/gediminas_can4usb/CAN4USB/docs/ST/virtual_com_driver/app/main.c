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
 *    1. Date        : August 26, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for the IAR STR750-SK evaluation boards. It implements USB
 * CDC (Communication Device Class) device and install it like a Virtual COM
 * port. The UART1 is used for physical implementation of the RS232 port.
 *
 * Jumpers:
 *  PWR_SEL   - depend of power source
 *  VRE       - Absence
 *  BOOT0     - 0
 *  BOOT1     - 0
 *
 *    $Revision: 18265 $
 *
 **************************************************************************/
#include    "includes.h"
#include    "75x_conf.h"
#include    "75x_lib.h"

#define UART  UART_1

volatile Int32U DlyCount;
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
  // Wait for OSC4M start-up
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

/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description:
 *
 *************************************************************************/
int main (void)
{
Int8U Buffer[100];
pInt8U pBuffer;
Int32U Size,TranSize;
Boolean UpdateLineState = TRUE;
Boolean CdcConfigureStateHold;


#if CDC_DEVICE_SUPPORT_LINE_CODING > 0
CDC_LineCoding_t CDC_LineCoding;
UART_InitTypeDef UartLineCoding;
#endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0

#if CDC_DEVICE_SUPPORT_LINE_STATE > 0

UartLineEvents_t      UartLineEvents;

SerialState_t   SerialState = {0};
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0

  // Set Burst mode
  CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);
  // MCU clock init
  InitClock();
  // EIC init
  EIC_DeInit();
  // Delay timer init
  InitDlyTimer(1);
  // Init UART 0/1
  UartInit(UART,3);

  // Initialize USB
  // Init USB
  USB_Init(5,4,UsbCdcConfigure);
  // Init CDC
  UsbCdcInit(2);
  // Soft connection enable
  USB_ConnectRes(TRUE);

  // Enable the Interrupt controller to manage IRQ channel
  EIC_IRQCmd(ENABLE);
  __enable_interrupt();

  // LCD Powerup init
  HD44780_PowerUpInit();
  // Backlight On
  LCD_LIGHT_ON();
  // Show messages on LCD
  HD44780_StrShow(1, 1,  "  IAR Systems   ");

  SerialState.bRxCarrier = 1;
  SerialState.bTxCarrier = 1;

  CdcConfigureStateHold = !UsbCdcIsCdcConfigure();

  while(1)
  {
    if (UsbCdcIsCdcConfigure())
    {
      if(CdcConfigureStateHold == FALSE)
      {
        HD44780_StrShow(1, 2,  "Virtual COM Port");
        CdcConfigureStateHold = TRUE;
      }
      // Data from USB
      Size = UsbCdcRead(Buffer,sizeof(Buffer)-1);
      if(Size)
      {
#ifdef DATA_LOGGING
        Buffer[Size] = 0;
        printf("> %s\n",Buffer);
#endif // DATA_LOGGING
        TranSize = 0;
        pBuffer = Buffer;
        do
        {
          Size -= TranSize;
          pBuffer += TranSize;
          TranSize = UartWrite(UART,pBuffer,Size);
        }
        while(Size != TranSize);
      }

      // Data from UART
      Size = UartRead(UART,Buffer,sizeof(Buffer)-1);
      if(Size)
      {
#ifdef DATA_LOGGING
        Buffer[Size] = 0;
        printf("< %s\n",Buffer);
#endif  // DATA_LOGGING
        while(!UsbCdcWrite(Buffer,Size));
      }
      // Get line and modem events from UART
#if CDC_DEVICE_SUPPORT_LINE_STATE > 0
      // Get line events - BI, FE, PE, OE
      UartLineEvents = UartGetUartLineEvents(UART);
      if(UartLineEvents.Data | UpdateLineState)
      {
        UpdateLineState = FALSE;
        // Line events report BI, PE, FE and OE
        SerialState.bBreak = UartLineEvents.bBI;
        SerialState.bFraming = UartLineEvents.bFE;
        SerialState.bOverRun = UartLineEvents.bOE;
        SerialState.bParity = UartLineEvents.bPE;
        // Send events
        UsbCdcReportSerialCommState(SerialState);
      }
#endif // CDC_DEVICE_SUPPORT_LINE_STATE > 0

    }
    else
    {
      if(CdcConfigureStateHold == TRUE)
      {
        HD44780_StrShow(1, 2,  " Put USB cable  ");
        CdcConfigureStateHold = FALSE;
      }
    }
    // UART line coding - Baud rate, number of the stop bits,
    // number of bits of the data word and parity type
#if CDC_DEVICE_SUPPORT_LINE_CODING > 0
    if(UsbCdcIsNewLineCodingSettings())
    {
      CDC_LineCoding = UsbCdcGetLineCodingSettings();
      // Update the baud rate
      UartLineCoding.UART_BaudRate = CDC_LineCoding.dwDTERate;
      // Update the stop bits number
      UartLineCoding.UART_StopBits = CDC_LineCoding.bCharFormat?UART_StopBits_2:UART_StopBits_1;
      // Update the parity type
      switch(CDC_LineCoding.bParityType)
      {
      case ODDPARITY:
        UartLineCoding.UART_Parity = UART_Parity_Odd;
        break;
      case EVENPARITY:
        UartLineCoding.UART_Parity = UART_Parity_Even;
        break;
      case MARKPARITY:
        UartLineCoding.UART_Parity = UART_Parity_EvenStick;
        break;
      case SPACEPARITY:
        UartLineCoding.UART_Parity = UART_Parity_OddStick;
        break;
      default:
        UartLineCoding.UART_Parity = UART_Parity_No;
      }
      // Update the word width
      switch( CDC_LineCoding.bDataBits)
      {
      case 5:
        UartLineCoding.UART_WordLength = UART_WordLength_5D;
        break;
      case 6:
        UartLineCoding.UART_WordLength = UART_WordLength_6D;
        break;
      case 7:
        UartLineCoding.UART_WordLength = UART_WordLength_7D;
        break;
      default:
        UartLineCoding.UART_WordLength = UART_WordLength_8D;
      }
      // Set UART line coding
      UartSetLineCoding(UART,UartLineCoding);
    }
#endif // CDC_DEVICE_SUPPORT_LINE_CODING > 0

    // Get line and modem events from USB
#if CDC_DEVICE_SUPPORT_BREAK > 0
    // Break event
    UartSetUartLineState(UART,UsbCdcGetBreakState());
#endif // CDC_DEVICE_SUPPORT_BREAK > 0
  }
}
