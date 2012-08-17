/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the CAN example
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
This example demonstrates the following features of the 3 CAN cells:
- bus initialization (setting baudrate)
- loopback and silent test modes
- configuration of message objects
- emission and reception of CAN messages
- standard and extended identifiers
- polling mode and interrupt mode

The program runs in loopback mode combined with silent mode (i.e. self-test mode), to be
independant from any true CAN network.

It is made of 2 parts :
1) one message exchange in polling mode
2) one message exchange in interrupt mode

Directory contents
==================
73x_conf.h Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program

How to use it
=============
In order to make the program work, you must do the following:
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_eic.c
  + 73x_prccu.c
  + 73x_cmu.c
  + 73x_gpio.c
  + 73x_can.c
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
