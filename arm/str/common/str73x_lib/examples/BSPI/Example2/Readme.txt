/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the BSPI example2
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
This example provides a basic example of how to use the BSPI software library
and an associate SPI EEPROM driver to communicate with an M95080 SPI bus EEPROM
(mounted on the STR73x-EVAL board). The M95080 EEPROM is connected to BPSI2.
The BPSI2 is configured as Master, with a word of 8 bits, Tx/Rx FIFO of 1 word,
and (CPOL,CPHA)=(0,0) and clock frequency fixed to 5.3Mb/s. The Slave Select(SS)
pin is not used, it is managed by software.
The associated software performs a write to the memory followed by a read of the
written data. Then the data read from the memory is compared with the expected 
values for correctness. The result of this comparison is stored in the
"TransferStatus" variable.

The system clock(MCLK) is set to 32MHz, using an external 8MHz quartz.

Directory contents
==================
73x_conf.h  Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program
spi_ee.c    SPI EEPROM driver
spi_ee.h    Header for the spi_ee.c file

How to use it
=============
In order to make the program work, you must do the following :
- check  that SW4 and SW5 switches (mounted on the STR73x-Eval board)
  are in position 2-3.
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_prccu.c
  + 73x_cmu.c
  + 73x_gpio.c
  + 73x_bspi.c
  + 73x_dma.c
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
