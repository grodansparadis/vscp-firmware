/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the TIM Example4_Sync_Parallel.
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
This example shows how to synchronize TIM peripherals in parallel mode.
In this example three timers are used:
- TIM0 used as Master in Enable Mode for TIM1 and TIM2. It is configured in PWM mode.
- TIM2 and TIM1 are used as Slaves in Gated Mode: both  the start and the stop  
  are controlled by the Master. These two timers are configured in PWM mode.

The Timer clock(CK_TIM) is set to 30 MHz. The three timers have the same period
and prescalers values. 
The three timers waveforms can be displayed using an oscilloscope. 


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program
main.h      Main header file


Hardware environment
====================
Connect the three timers pins to an oscilloscope:
- TIM0_OC1 pin (P0.00)  
- TIM1_OC1 pin (P0.28)  
- TIM2_OC1 pin (P0.02) 
  
  
How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 75x_lib.c
  + 75x_tim.c
  + 75x_mrcc.c
  + 75x_gpio.c
  
- Link all compiled files and load your image into either RAM or Flash
- Run the example

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
