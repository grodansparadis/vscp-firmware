/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the CMU example
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

This example guides the user to configure the CMU to
Switch source clocks between the external Quartz and the internal RC.

The example returns the info about the current used oscillator in the STDIO debug interface.
The user can note the change in the toggling frequency of all leds connected to the GPIO4.


Directory contents
==================
73x_conf.h  Library Configuration file
main.c      Main program
73x_it.c    Interrupt handlers

How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_cmu.c
  + 73x_prccu.c
  + 73x_gpio.c
- Link all compiled files and load your image into Flash
- Run the example

******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****

