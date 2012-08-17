/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the  TIM example
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
This example guides the user to:

- Configure the TIM4 to generate a pulse of 0x1FF0 timer period after a rising 
  edge on the input capture A pin.
- Configure the TIM3 to generate a PWM signal of 0x7FFF timer period and a 
  duty cycle of 50%, this signal is used as external input for the TIM4. 


The system clock(MCLK) is using the internal oscillator ~ 1.17MHz.
The prescaler of the two timers TIM3 and TIM4 are equal to 0x0.

Directory contents
==================
73x_conf.h Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program

How to use it
=============
In order to make the program work, you must do the following :

- Connect the TIM3 OCMPA (GPIO0 Pin 15) to TIM4 ICAPA (GPIO1 Pin 3).
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_eic.c
  + 73x_gpio.c
  + 73x_tim.c
- Link all compiled files & load program into either RAM or Flash 
- Run the example
- Visualise the TIM4's One Pulse Mode Waveform on OCMPA (GPIO1 Pin 0). 


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
