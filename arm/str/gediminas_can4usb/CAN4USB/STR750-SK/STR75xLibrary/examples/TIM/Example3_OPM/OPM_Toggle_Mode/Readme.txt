/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the TIM Example3_OPM: OPM_Toggle_Mode.
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
This example show how to use the TIM peripheral to generate a One pulse Mode after
an edge received in Timer Input pin.

In this example two timers are used:
- TIM0 is configured to generate a One Pulse in Output Toggle Mode after a delay
  of 2.73 ms. The External pulse is received on TIM0_TI1 pin.
- TIM1 is configured in Output Compare Toggle Mode to generate a signal used as
  an external input signal for the first timer.

The Timer clock (CK_TIM) is set to 30 MHz.
The two timers waveforms can be displayed using an oscilloscope. 


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect TIM0_TI1 pin (P0.01) to TIM1_OC1 pin (P0.28). 
 - Connect the two timers pins to an oscilloscope:
     - TIM0_OC1 pin (P0.00)  
     - TIM1_OC1 pin (P0.28)   

  
How to use it
=============
In order to make the program work, you must do the following:

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
