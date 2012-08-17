/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the UART Example 5.
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
This example provides a basic communication between UART0 and UART2 using 
UART0 DMA capability.
First, UART2 sends the TxBuffer2 buffer to UART0, data received by UART0 is
transfered by DMA to RxBuffer0 then compared with the send ones and the result 
of this comparison is stored in the "TransferStatus2" variable. 
 
Then, the DMA transfers data from TxBuffer0 buffer to UART0 Tx FIFO, then this 
data is sent to UART2. Data received by UART2 is stored in RxBuffer2 then compared
with the send ones and the result of this comparison is stored in the 
"TransferStatus0" variable. 

UART0 and UART2 configured as follow:
  - Word Length = 8 Bits
  - One Stop Bit
  - Odd parity
  - BaudRate = 9600 baud
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  - Receive and transmit FIFOs are disabled


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
