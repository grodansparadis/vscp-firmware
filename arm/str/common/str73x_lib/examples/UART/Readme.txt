/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the UART example
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
This example gives an example of the UART peripheral use :

Send 'A' character  from UART0 to a Windows PC Hyperterminal.


Directory contents
==================
73x_conf.h Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program

How to use it
=============
In order to make the program work, you must do the following :

- Connect UART0 to your Windows PC  via a RS232 cable 
- Configure Windows Hyperterminal application : 9600bps,8 data, Parity even, stop bits 1
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_cmu.c
  + 73x_prccu.c
  + 73x_gpio.c
  + 73x_uart.c
- Link all compiled files & load program into either RAM or Flash 
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
