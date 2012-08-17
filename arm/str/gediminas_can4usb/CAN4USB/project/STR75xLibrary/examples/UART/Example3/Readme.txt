/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the UART Example 3.
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
This example provides a basic communication between UART0 and UART2 using flags.
First, the UART0 sends TxBuffer to UART2. The UART2 reads the received data and
store it into RxBuffer.
The received data is then compared with the send ones and the result of this 
comparison is stored in the "TransferStatus" variable.   

UART0 and UART2 configured as follow:
  - Word Length = 8 Bits
  - One Stop Bit
  - Even parity
  - BaudRate = 230400 baud
  - Hardware flow control disabled (RTS and CTS signals)
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
Connect UART0_Tx(P0.11) to UART2_Rx(P0.24) and UART0_Rx(P0.10) to UART2_Tx(P0.25)
(in the STR750-SK board, connect a null-modem male/male RS232 cable between
RS232_0 and RS232_2).
  
  
How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
