/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the DMA example
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
This example demonstrates how to copy a block of data from a specified RAM address
to another address using memory-to-memory DMA capability. The DMA3 stream3 is 
configured to transfer data from "SRC_Buffer" to "DST_Buffer" and to generate an
interrupt(transfer end interrupt, SI3) at the transfer end.
At the end of the transfer, the SI3 interrupt occurs. In the interrupt handler
routine the INT3 flag is cleared and DMA transfer on AHB bus is disabled. Then the
data transfered by the DMA is compared with the expected values for correctness.
The result of this comparison is stored in the "TransferStatus" variable.

Directory contents
==================

73x_conf.h  Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program
main.h      header for "main.c" file

How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_eic.c
  + 73x_dma.c
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
