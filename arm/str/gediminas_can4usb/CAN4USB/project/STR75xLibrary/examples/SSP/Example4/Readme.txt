/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : Description of the SSP Example4.
********************************************************************************
* History:
* 07/17/2006 : V1.0
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
This example provides a basic example of how to use the SSP software library
and an associate SPI EEPROM driver to communicate with an M95640 EEPROM.
Using this driver the program performs a write of a Tx_Buffer, defined in the 
main.c file, to the memory followed by a read of the written data. Then data read
from the memory stored in the Rx_Buffer is compared with the expected values of 
the Tx_Buffer. The result of this comparison is stored in the "TransferStatus" 
variable.
The SSP0 is configured as Master in motorola frame format with an 8bits data size.
The SSP0 NSS pin is managed by software, this allow to use this as GPIO drive the
SPI EEPROM chip select pin, the clock is set to 5MHz. 
The EEPROM_WriteAddress and the EEPROM_ReadAddress where the program start the write 
and the read operations are defined in the main.c file. 


Directory contents
==================
75x_conf.h   Library Configuration file
75x_it.c     Interrupt handlers
main.c       Main program
spi_ee.c     SPI EEPROM driver
spi_ee.h     Header for the spi_ee.c file


Hardware environment
====================
Connect both SSP0 and SPI EEPROM pins as following:
 - Connect SSP0_NSS(P0.04)  to SPI EEPROM chip select (pin1).
 - Connect SSP0_SCLK(P0.05) to SPI EEPROM serial clock (pin6).
 - Connect SSP0_MISO(P0.06) to SPI EEPROM serial data output (pin2).
 - Connect SSP0_MOSI(P0.07) to SPI EEPROM serial data input (pin5).
 - Connect SPI EEPROM Write Protect (pin3) to Vdd
 - Connect SPI EEPROM Hold (pin7) to Vdd
 - Connect SPI EEPROM Vcc (pin8) to Vdd
 - Connect SPI EEPROM Vss (pin4) to Vss

How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****
