/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the PWM Example1.
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
complementary PWM signals, to insert a defined dead time value and to lock the 
desired parameters.
The CK_TIM frequency is set to 60 MHz, the PWM is running at 300 KHz, the 
Pulse1, Pulse2 and Pulse3 duty cycles are fixed respectively to 25%, 50% and 75%.
The dead time is equal to 4.25 microsecond, and the Lock level 3 is selected. 
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
- PWM1N pin (P1.08)  
- PWM2  pin (P1.07)  
- PWM2N pin (P1.06)  
- PWM3  pin (P1.05)  
- PWM3N pin (P1.04)  
  
  
How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****
