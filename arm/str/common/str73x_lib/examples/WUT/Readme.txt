/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the WUT example
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
This example provides a basic example of how to enter the MCU in Stop mode and
to wake-up by the WUT End of Count(EC) event.
In the associated software, the TB0 is programmed to toggle the P4.4 pin each
250 ms and the WUT is configured to wake-up the MCU from Stop mode in 10s.
The P4.4 pin is used to indicate whether the MCU is in Stop or RUN mode.

In an infinite loop, the MCU enters and exit from Stop mode as the following:
The P4.4 pin starts toggling then after a time delay the MCU enters Stop mode 
causing the P4.4 pin to stop toggling. However, the WUT is still working and
when the counter reaches the End of Count condition (10s elapsed) it generates
an internal wake-up causing the MCU to exit from Stop mode. As result, the
P4.4 pin restarts toggling.

The system clock(MCLK) is set to 4MHz using a 8 MHz external quartz.

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
  + 73x_cmu.c
  + 73x_gpio.c
  + 73x_tb.c
  + 73x_wut.c
  + 73x_wiu.c
- Link all compiled files & load program into either RAM or Flash 
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
