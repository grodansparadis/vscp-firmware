/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the PWM Example2.
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
This example shows how to configure the PWM peripheral to generate three 
independants PWM signals.
The CK_TIM frequency is set to 60 MHz, the PWM is running at 300 KHz, the 
Pulse1, Pulse2 and Pulse3 duty cycles are fixed respectively to 25%, 50% and 75%.
The Lock level 3 is selected. The PWM is configured to generate an interrupt on 
each update.
On each PWM update interrupt Pulse1, Pulse2 and Pulse3 values are modified which 
changes the PWM channel1, PWM channel2 and PWM channel3 duty cycles.
The PWM waveform can be displayed using an oscilloscope. 


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect the PWM pins to an oscilloscope:
- PWM1  pin (P1.09)  
- PWM2  pin (P1.07)  
- PWM3  pin (P1.05)  

 
How to use it
=============
In order to make the program work, you must do the following :

- Run the example

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******

