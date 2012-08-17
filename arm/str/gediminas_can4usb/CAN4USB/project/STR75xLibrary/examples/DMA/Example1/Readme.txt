/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the DMA Example1.
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
This example provides a description of how to use DMA Stream1 with SSP0_TX burst
request to transfer Data from SSP0 to SSP1 in burst mode.
SSP0 is configured as master with a transfer frequency equal to 2.5 MHz while SSP1
is set as slave. Both SSPs are configured in Motorola frame format with a data
size equal to 16 bits and NSS pin managed by software. The SSP1 receive interrupt
is enabled (when the receive FIFO hold four data the interrupt is generated).
The DMA Stream1 is configured to transfer 64 data from a buffer to SSP0 Transmit 
FIFO. The transfer is configured with 4 data burst and 16 bits data size.

Once the DMA is enabled, 4 data are transferred from the buffer to SSP0 Transmit
FIFO through DMA Stream1. Then SSP0 will send these data to SSP1.
As soon as SSP1 receive FIFO level reach four data, the receive interrupt occurs.
In the interrupt handler, four data are read.
The two last actions are repeated until all data are transmitted from SSP0 to 
SSP1(Rx_Idx value equal 64).

At the end of transfer, DMA Stream1 end of transfer interrupt is generated.
The received data are stored in the SSP1_Buffer_Rx which will be compared to the   
SRC_Buffer. The TransferStatus variable status shows if the transfer is 
successful or not.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect SSP0_SCLK (P0.05) to SSP1_SCLK (P0.16).
Connect SSP0_MISO (P0.06) to SSP1_MISO (P0.17).
Connect SSP0_MOSI (P0.07) to SSP1_MOSI (P0.18).
   

How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
