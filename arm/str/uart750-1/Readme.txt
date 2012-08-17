/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the UART Example1.
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
This example provides a description of how  to use the UART with hardware flow
control and communicate with the Hyperterminal.
First, the UART0 sends the TxBuffer to the hyperterminal and still waiting for
a string from the hyperterminal that you must enter which must end by '\r'
character (keypad ENTER button). Each byte received is retransmitted to the
Hyperterminal. 
The string that you have entered is stored in the RxBuffer array. The receive
buffer have a RxBufferSize bytes as maximum.

The UART0 is configured as follow:
  - Word Length = 7 Bits
  - Two Stop Bit
  - No parity
  - BaudRate = 115200 baud
  - Hardware flow control enabled (RTS and CTS signals)
  - Receive and transmit enabled
  - Receive and transmit FIFOs are enabled
  - Transmit and Receive FIFOs levels have 8 bytes depth 


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - All the UART0 signals (Rx, Tx, RTS and CTS) must be connected to a DB9 
   connector using a RS232 transceiver.

 - Connect a null-modem female/female RS232 cable between the DB9 connector 
   (CN4 on STR75x-EVAL board) and PC serial port.  

 - Hyperterminal configuration:
    - Word Length = 7 Bits
    - Two Stop Bit
    - No parity
    - BaudRate = 115200 baud
    - flow control: Hardware 
  
  
How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 75x_lib.c
  + 75x_uart.c
  + 75x_gpio.c
  + 75x_mrcc.c
  
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
