/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the TIM Example1_OC: Toggle_Mode.
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
This example shows how to configure the TIM peripheral to generate two different 
signals with two different frequencies.
The CK_TIM frequency is set to 60 MHz, and used in Output Compare Toggle Mode. 
The TIM OC1 register value is equal to 15000, so the TIM0 Channel 1 generates a 
periodic signal with an update rate equal to 250 us.
The TIM OC2 register is equal to 6000, so the TIM0 channel 2 generates a periodic 
signal with an update rate equal to 100us.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect an oscilloscope on P0.00 and P1.00 to show the two different signals.
  
  
How to use it
=============
In order to make the program work, you must do the following:

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
