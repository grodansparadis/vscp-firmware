/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the BSPI example1
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
In this example BSPI0 and BSPI1 are connected together on the same bus. BSPI0 is
configured as Slave and BSPI1 as Master, both with a word of 8 bits and Tx/Rx
FIFO of 16 words. The BSPI1 Slave Select (SS) pin is not used, it is managed by
software. The BSPI clock frequency is fixed to 5.3Mb/s.

A BSPI1 Transmit FIFO Empty (TFE) interrupt based function, performs the transfer
of data from a predefined buffer to the BSPI1 transmit FIFO. Then BSPI1 transmits
these data to BSPI0. Each data received by BSPI0 asserts a DMA0 stream2 request.
The DMA0 stream2 is programmed to perform a transfer from BSPI0 to a receive
buffer block. At the end of this transfer, DMA0 stream2 Transfer End Interrupt
is generated. In the correspondant interrupt service routine BSPI0 and DMA transfer
on AHB bus are disabled. Then the data received by the BSPI0 is compared with the
expected values for correctness. The result of this comparison is stored in the
"TransferStatus" variable.

The system clock(MCLK) is set to 32MHz, using an external 8MHz quartz.

Directory contents
==================
73x_conf.h  Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program
main.h      Main header file

How to use it
=============
In order to make the program work, you must do the following :

- Connect BSPI0 MISO pin (P6.11) to BSPI1 MISO pin (P5.1)
- Connect BSPI0 MOSI pin (P6.12) to BSPI1 MOSI pin(P5.0)
- Connect BSPI0 SCK pin (P6.13) to BSPI1 SCK pin(P4.15)
- Connect BSPI0 SS pin (P6.14) to a low level (Vss)
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_eic.c
  + 73x_prccu.c
  + 73x_cmu.c
  + 73x_gpio.c
  + 73x_bspi.c
  + 73x_dma.c
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
