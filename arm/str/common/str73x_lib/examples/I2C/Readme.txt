/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the  I²C example
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
This example provides a basic example of how to use the I2C software library
and an associate I2C EEPROM driver to communicate with an M24C08 I2C bus EEPROM
(mounted on the STR73x-EVAL board). 
- Start a communication between I2C0 interface and the on board I2C EEPROM 
- The I2C EEPROM is connected to I2C0.
- I2C0 is configured in Master transmitter during write operation and in Master receiver 
during read operation from I2C EEPROM. 
- The Own I2C0 address is defined to 0xA0 in the I2c_ee.c file.
- The speed is set to 10kHz in the I2c_ee.c file.
- The EEPROM address where the program start the write and the read operations, is defined
in the main.c file. 

The system clock(MCLK) is set to 32MHz, using an external 8MHz oscillator.

Directory contents
==================
73x_conf.h  Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program
i2c_ee.c    I2C EEPROM driver
i2c_ee.h    Header for the i2c_ee.c file

How to use it
=============
In order to make the program work, you must do the following :
- check  that SW7 switch (mounted on the STR73x-Eval board) is in position 1-2.
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_eic.c
  + 73x_prccu.c
  + 73x_gpio.c
  + 73x_cmu.c
  + 73x_i2c.c
- Link all compiled files and load your image into either RAM or Flash
- Run the example
- The associated software performs a write to the memory followed by a read of the
  written data. Then data read from the memory stored in the Rx_Buffer
  is compared with the expected values of the Tx_Buffer.
  The result of this comparison is stored in the "TransferStatus" variable.

******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
