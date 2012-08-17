/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the UART Example 2.
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
This example provides a description of how  to use the UART0 interrupts to 
communicate with the hyperterminal.
The UART0 sends a predefined buffer to the hyperterminal and still waiting 
for a string from the hyperterminal which length is defined by RxBufferSize. 
The communication is managed by Transmit and Receive interrupts. 

First 16 data bytes, from TxBuffer, are transmitted to the UART0 transmit FIFO.
Then when the  UART0 is enabled it starts sending the transmit FIFO content to
the hyperterminal.
As soon as UART0 transmit FIFO level falls below eight data, the Transmit interrupt
is generated. In the handler routine eight more data are written to the UART0. 
This procedure is repeated until all data are written to the UART0 transmit FIFO.
Once all data are transmitted the Transmit interrupt is disabled.

The bytes transmitted from hyperterminal are managed as follow: each time UART0 
receive FIFO level reach 2 bytes, the receive interrupt is asserted. In the handler
routine two bytes are read from FIFO. When the maximum receive buffer length is 
reached, the receive interrupt is disabled. The string that you have entered is 
stored in the RxBuffer array.

The UART0 is configured as follow:
  - Word Length = 8 Bits
  - One Stop Bit
  - Odd parity
  - BaudRate = 9600 baud
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  - Receive and transmit FIFOs are enabled
  - Transmit FIFO level have 8 bytes depth 
  - Receive FIFO level have 2 bytes depth 
  - UART Transmit and Receive interrupts are enabled

  
Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - The UART0 signals (Rx and Tx) must be connected to a DB9 connector using
   a RS232 transceiver.

 - Connect a null-modem female/female RS232 cable between the DB9 connector 
   (CN4 on STR75x-EVAL board) and PC serial port.  

 - Hyperterminal configuration:
    - Word Length = 8 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
    - flow control: None 
      
How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 75x_lib.c
  + 75x_uart.c
  + 75x_gpio.c
  + 75x_mrcc.c
  + 75x_eic.c
  
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
