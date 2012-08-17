/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the DMA Example2.
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
This example provides a description of how to use DMA Stream3 with ADC request
to transfer Data from an ADC Data register to a TIM0 Output Compare register.
ADC Channel 14 is converted continuously with a frequency equal to 333.33KHz 
DMA capability is enabled for Channel 14. TIM0 used in PWM mode, run with a 
frequency of 70.75KHz and Output Compare2 is enabled.
The DMA Stream3 is configured in Circular mode to transfer 16 bits data from
ADC_D14 register to TIM0_OCR2 register continuously.
Once the DMA Stream3 is enabled, each time a conversion of Channel 14 is finished 
a transfer of the last converted value is done to fill the TIM0_OCR2 register. 
This will change the TIM0 output compare2 signal duty cycle. The TIM0_OC2 output
drives a led luminosity according to the value converted by the ADC.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect a variable power supply 0-3.3V to ADC Channel 14 mapped on pin P1.13
   (potentiometer AN_TR on STR750-SK board) 
 - Connect TIM0_OC2 pin (P1.00) to a led.
 
 
How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
