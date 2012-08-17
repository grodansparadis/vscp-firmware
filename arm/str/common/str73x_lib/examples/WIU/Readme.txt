/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the WIU example
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
This example provides a basic example of how to use an external wake-up line to
exit the MCU from Stop mode.
In the associated software, the TB1 is programmed to toggle the P4.4 pin each
250 ms, this pin is used to indicate whether the MCU is in Stop or RUN mode.
The WIU is configured to wake-up the system from Stop mode when a falling edge
occurs on P2.10 pin.

In an infinite loop, the MCU enters and exit from Stop mode as the following:
The P4.4 pin starts toggling then after a time delay the MCU enters Stop mode 
causing the P4.4 pin to stop toggling. Once a falling edge is detected on P2.10
pin, the MCU wake up from Stop mode and an interrupt is generated. In the
associated interrupt handler routine the wake-up line16 pending bit is cleared.
Then the PRCCU STOP_I flag is cleared. After that the P4.4 pin restarts toggling.

The system clock(MCLK) is set to 4MHz using a 8 MHz external quartz.

Note: The P2.10 pin is connected to WAKE-UP push button on STR73x-EVAL board.

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
  + 73x_wiu.c
- Link all compiled files & load program into either RAM or Flash 
- Run the example



******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
