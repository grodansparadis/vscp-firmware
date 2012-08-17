/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the TIM Example5_DMA.
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
This example provides a description of how to use DMA Stream0 with TIM0_Update
request to transfer Data from memory to TIM0 Output compare Register2.

The TIM0 Channel2 is configured to generate a PWM signal with a frequency equal 
to 13.8 Hz and a variable duty cycle that is transfered from memory to the 
corresponding register after each Update DMA request. The TIM0 waveform can be 
displayed using an oscilloscope. 


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect an oscilloscope to TIM0_OC2 pin (P1.00).  
  
  
How to use it
=============
In order to make the program work, you must do the following :
- Run the example

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
