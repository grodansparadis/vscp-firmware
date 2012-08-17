/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the RTC example
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
This example guides the user to :
- configure the prescaler, counter and alarm values
- enable Second, Alarm and overflow interrupts

it is made of 6 steps :
1) GPIO4 configuration in output push-pull mode to use leds

2) Set MCLK Clock to 8Mhz 

3) Set EXTCLK Clock to 1Mhz (using PRCCU_SetExtClkDiv() function)

4) EIC configuration
   - RTC channel priority configuration
   - Enable RTC channel interrupt
   - Enable Interrupt
   
5) RTC configuration  (in the RTC_InitStructure )
   - Initialize the RTC using the RTC_DeInit function
   - Enable RTC interrupts: Second, Alarm and Overflow interrupts
   - Configure Alarm register value to 0xFFFFFFFF
   - Configure Counter register value to 0xFFFFFFFA
   - Configure Prescaler register value to 0x3D09

6) Toggle leds in the RTC_IRQHandler (73x_it.c)
  - Toggle GPIO4 pin 0 when global interrupt request happen
  - Toggle GPIO4 pin 2 when Second interrupt request happen
  - Toggle GPIO4 pin 4 when Alarm interrupt request happen, then set counter value to 0xFFFFFFFA
  - Toggle GPIO4 pin 6 when Overflow interrupt request happen, then set counter value to 0xFFFFFFFA
  - Clear the flag of each generated interrupt request

Directory contents
==================
73x_conf.h  Library Configuration file
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
  + 73x_prccu.c
  + 73x_gpio.c
  + 73x_cmu.c
  + 73x_rtc.c
- Link all compiled files & load program into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
