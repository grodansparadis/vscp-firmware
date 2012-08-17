/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the DMA Example3.
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
This example provides a description of how to use DMA Stream0 with PWM update
request to transfer data from a constant buffer (RAM or Flash) to the PWM Output
Compare1 register.
PWM is used in PWM mode, run with a frequency of 9,2Hz and Output Compare1 is 
enabled. The DMA Stream0 is configured in Circular mode with transfer size of 
128 and 16 bits data width.

Each PWM update event will trigger a DMA Stream0 request, the DMA will transfer 
16 bits data from the predefined buffer to the PWM_OCR1 register. This will change
the PWM1 output signal duty cycle. The PWM1 output drives a led luminosity
according to the value transferred by the DMA.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect PWM1 pin (P1.09) to a led.
   

How to use it
=============
In order to make the program work, you must do the following :
  
- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
