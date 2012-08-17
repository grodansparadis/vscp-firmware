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
 * use of parallel I/O, timer and interrupt controller.
 *
 *  It starts by blinking the red STAT LED with period of 1 second.
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

#include <intrinsics.h>
#include    "75x_conf.h"
#include    "75x_lib.h"

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
  GPIO_WriteBit( GPIO2,
                 GPIO_Pin_18,
                (GPIO_ReadBit(GPIO2,GPIO_Pin_18) == Bit_RESET)?Bit_SET:Bit_RESET);
  // Clear the Update pending Bit
  TIM_ClearITPendingBit(TIM0, TIM_IT_Update);
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

  // Enable TIM0 and GPIO clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_TIM0 | MRCC_Peripheral_GPIO, ENABLE);
  // Release TIM0 and GPIO reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_TIM0 | MRCC_Peripheral_GPIO,DISABLE);

  // GPIO 2.18 (STAR LED)
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_18;
  GPIO_Init(GPIO2, &GPIO_InitStructure);
  GPIO_Write(GPIO2,0);

  // Timer 0
  // TIM Configuration in Output Compare Timing Mode period 0.5 sec
  // Calculate counter value
  MRCC_GetClocksStatus(&MRCC_Clocks);

  TIM_InitStructure.TIM_Period = (MRCC_Clocks.CKTIM_Frequency / 277)*0.5;
  TIM_InitStructure.TIM_Prescaler = 277-1;
  TIM_InitStructure.TIM_Mode = TIM_Mode_OCTiming;
  TIM_InitStructure.TIM_ClockSource = TIM_ClockSource_Internal;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_InitStructure.TIM_Channel = TIM_Channel_2;
  TIM_Init(TIM0, &TIM_InitStructure);

  // Clear TIM0 flags
  TIM_ClearFlag(TIM0, TIM_FLAG_OC1| TIM_FLAG_OC2| TIM_FLAG_Update);

  // Enable TIM0 Update interrupt
  TIM_ITConfig(TIM0, TIM_IT_Update, ENABLE);

  // Enable TIM0 counter
  TIM_Cmd(TIM0, ENABLE);

  // Enable and configure the priority of the TIM0 Update IRQ Channel
  EIC_IRQInitStructure.EIC_IRQChannel = TIM0_UP_IRQChannel;
  EIC_IRQInitStructure.EIC_IRQChannelPriority = 1;
  EIC_IRQInitStructure.EIC_IRQChannelCmd = ENABLE;
  EIC_IRQInit(&EIC_IRQInitStructure);

  // Enable the Interrupt controller to manage IRQ channel
  EIC_IRQCmd(ENABLE);
  __enable_interrupt();

  while(1)
  {
  }
}
