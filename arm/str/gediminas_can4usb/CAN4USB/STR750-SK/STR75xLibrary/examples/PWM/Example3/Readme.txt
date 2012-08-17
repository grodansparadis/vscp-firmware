/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : Description of the PWM Example3. 
********************************************************************************
* History:
* 07/17/2006 : V1.0
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
This example shows how to synchronize the TIM0 peripheral and the PWM peripheral.

In this example one timer and PWM are used:
- TIM0 used as Master for the PWM. It is used in PWM mode.
- PWM is used as Slave in External clock Mode: the rising edge of selected trigger
  (TRG) clocks the counter. In our case the trigger is the TIM0 OC1. The PWM is 
  configured to generate two complementary signals on channel 1.
The Timer clock(CK_TIM) is set to 30 MHz. The TIM0 generates a PWM signal with a 
frequency equal to 625 KHz. 
The PWM generates a complementary signal with a frequency equal to:
freq(TIM0_OC1)/(PWM_PSC + 1)(PWM_ARR + 1) = 1.085 KHz. 

Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect the three timers pins to an oscilloscope:
- TIM0_OC1 pin (P0.00)  
- PWM_OC1 pin (P1.09)  
- PWM_OC1N pin (P1.08) 
 
  
How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
