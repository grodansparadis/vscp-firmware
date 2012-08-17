/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the TIM Example2_PWMI.
********************************************************************************
* History:
* 07/17/2006 : V1.0
* 03/10/2006 : V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

Example description
===================
This example shows how to use the TIM peripheral to measure the frequency and 
duty cycle of an external signal. 

In this example two timers are used:
- TIM0 is configured to generate a PWM signal with a frequency equal to 1 kHz
  and a duty cycle of 75%.
- TIM1 is configured in PWM Input Mode and to generate an interrupt on each Input
  Capture1 event. In the TIM1_IC12_IRQHandler routine the frequency and duty cycle  
  of TIM0 are computed. TIM0Frequency holds TIM0 frequency, in Hz, and TIM0DutyCycle
  holds TIM0 duty cycle in %.

The Timer clock(CK_TIM) is set to 30 MHz. The two timers have the same prescalers
and period values.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect TIM0_OC2 pin (P1.00) to TIM1_TI1 pin (P0.29). 
  
  
How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
