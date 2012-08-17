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
 *    1. Date        : August 10, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench
 * for ARM to develop code for the IAR STR750-SK board. It shows basic
 * use of parallel I/O, timer, interrupt controller, ADC and interface to
 * a LCD HD44780 compatible module.
 *
 * It show the position of the potentiometer in percents.
 *
 * Jumpers:
 *  PWR_SEL   - depend of power source
 *  VRE       - Absence
 *  BOOT0     - 0
 *  BOOT1     - 0
 *
 *
 *    $Revision: 15135 $
 *
 **************************************************************************/
#include    "includes.h"
volatile Int32U DlyCount;
volatile Int32U TrimVal;

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
 * Function Name: AdcHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: ADC interrupt handler
 *
 *************************************************************************/
void AdcHandler(void)
{
  TrimVal = ADC_GetConversionValue(ADC_CHANNEL14);
  // Clear EOC Interrupt pending bit
  ADC_ClearITPendingBit(ADC_IT_EOC);
}

/*************************************************************************
 * Function Name: Tim1Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 0 interrupt handler
 *
 *************************************************************************/
void Dly100us(void *arg)
{
  DlyCount = (Int32U)arg;

  // Clear TIM0 flags
  TIM_ClearFlag(TIM0, TIM_FLAG_OC1| TIM_FLAG_OC2| TIM_FLAG_Update);

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
TIM_InitTypeDef TIM_InitStructure;
EIC_IRQInitTypeDef EIC_IRQInitStructure;
GPIO_InitTypeDef  GPIO_InitStructure;
MRCC_ClocksTypeDef MRCC_Clocks;
ADC_InitTypeDef  ADC_InitStructure;
char Str[17];
Int32U Pos;

  // Set Burst mode
  CFG_FLASHBurstConfig(CFG_FLASHBurst_Enable);
  // Clock
  // MRCC system reset(for debug purpose)
  MRCC_DeInit();
  // Wait for OSC4M start-up
  MRCC_WaitForOSC4MStartUp();
  // Set HCLK
  MRCC_HCLKConfig(MRCC_CKSYS_Div2);
  // Set CKTIM
  MRCC_CKTIMConfig(MRCC_HCLK_Div1);
  // Set PCLK
  MRCC_PCLKConfig(MRCC_CKTIM_Div2);
  // Set CKSYS
  MRCC_CKSYSConfig(MRCC_CKSYS_OSC4MPLL, MRCC_PLL_Mul_15);
  // GPIO pins optimized for 3V3 operation
  MRCC_IOVoltageRangeConfig(MRCC_IOVoltageRange_3V3);

  // EIC Init
  EIC_DeInit();

  // Enable TIM0 clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM0, ENABLE);
  // Release TIM0 reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM0,DISABLE);

  // GPIO 2.18 (STAR LED)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_18;
  GPIO_Init(GPIO2, &GPIO_InitStructure);
  GPIO_Write(GPIO2,0);

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

  // Enable and configure the priority of the TIM0 Update IRQ Channel
  EIC_IRQInitStructure.EIC_IRQChannel = TIM0_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  // Adc IN14
  // Enable ADC and GPIO clock
  MRCC_PeripheralClockConfig(MRCC_Peripheral_ADC | MRCC_Peripheral_GPIO, ENABLE);
  // Release ADC and GPIO reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_ADC | MRCC_Peripheral_GPIO,DISABLE);

  // ADC Channel 14 pin configuration
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ;
  GPIO_Init(GPIO1, &GPIO_InitStructure);

  // ADC IRQ Channel configuration */
  EIC_IRQInitStructure.EIC_IRQChannel = ADC_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 2;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  ADC_InitStructure.ADC_ConversionMode = ADC_ConversionMode_OneShot;
  ADC_InitStructure.ADC_ExtTrigger = ADC_ExtTrigger_Disable;
  ADC_InitStructure.ADC_AutoClockOff = ADC_AutoClockOff_Disable;
  ADC_InitStructure.ADC_SamplingPrescaler = 5;
  ADC_InitStructure.ADC_ConversionPrescaler = 1;
  ADC_InitStructure.ADC_FirstChannel = ADC_CHANNEL14;
  ADC_InitStructure.ADC_ChannelNumber = 1;
  ADC_Init(&ADC_InitStructure);

  // Enable EOC interrupt
  ADC_ITConfig(ADC_IT_EOC, ENABLE);
  // Enable ADC
  ADC_Cmd(ENABLE);
  // Start calibration
  ADC_StartCalibration(ADC_CalibAverage_Enable);

  // Enable the Interrupt controller to manage IRQ channel
  EIC_IRQCmd(ENABLE);
  __enable_interrupt();

  // LCD Init
  HD44780_PowerUpInit();
  // Show message on the LCD
  HD44780_StrShow(1, 1,  "  IAR Systems   ");
  HD44780_StrShow(1, 2,  "    STR750F     ");
  LCD_LIGHT_ON();
  Dly100us((void*)30000);

  while(1)
  {
    // Start conversion
    ADC_ConversionCmd(ADC_Conversion_Start);
    Dly100us((void*)2000);
    // Calculate position of the potentiometer in percents
    Pos = (TrimVal*100)/1023;
    sprintf(Str,"AN_TR:%3d%%      ",Pos);
    HD44780_StrShow(1, 2,(pInt8S)Str);
    // Delay 1sec
  }
}
