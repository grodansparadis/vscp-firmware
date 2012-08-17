/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the  EIC example
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
*********************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************


Example description
===================
This example demontrates the use of the Enhanced Interrupt Controller (EIC): 

- Configuration of 8 TIM (TIM0..TIM7)timers to generate an interrupt
  on each counter overflow event.
- TIM0 is linked to the first fast FIQ channel while all others timers
  to their correspondant IRQ channel.
- Assignment of a descendant IRQ priority for each IRQ channel :
  TIM1 has a priority of 7  and TIM7 has a priority of 1.
- In each interrupt routine, a pin of GPIO4 is chosen to toggle
  the IRQ or FIQ (entry/exit) status.

The system clock(MCLK) is using the internal oscillator ~ 1.17MHz.
The prescaler of each timer is equal to the timer number. For example,
TIM7 has a prescaler = 7 .

Directory contents
==================
73x_conf.h Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program

How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_eic.c
  + 73x_gpio.c
  + 73x_tim.c
- Link all compiled files and load your image into either RAM or Flash
- Run the example. 


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
