/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the I2C Example.
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
This example provides a basic example of how to use the I2C software library
and an associate I2C EEPROM driver to communicate with an M24C08 EEPROM.
Using this driver the program performs a write of a Tx_Buffer, defined in the 
main.c file, to the memory followed by a read of the written data. Then data read
from the memory stored in the Rx_Buffer is compared with the expected values of 
the Tx_Buffer. The result of this comparison is stored in the "TransferStatus" 
variable.
The I2C is configured in Master transmitter during write operation and in Master 
receiver during read operation from I2C EEPROM. 
The Own I2C address is defined to 0xA0 in the I2c_ee.c file.
The speed is set to the maximum frequency of 400kHz. This value is defined in 
the I2c_ee.c file, which could be in the range of 0 to 400KHZ.
One I2C EEPROM Block address where the program will write the buffer have to be
selected from the four address available and defined in the I2c_ee.c file. 
The EEPROM address where the program start the write and the read operations is
defined in the main.c file. 


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program
i2c_ee.c    I2C EEPROM driver
i2c_ee.h    Header for the i2c_ee.c file


Hardware environment
====================
An I2C EEPROM have to be connected to the STR750 microcontroller as following:
 - Connect I2C_SCL(P0.08) to EEPROM_SCL(Pin6).
 - Connect I2C_SDA(P0.09) to EEPROM_SDA(Pin5).
 - Connect pin(P2.00) to EEPROM_E2(Pin3).
 - Connect EEPROM_Vcc(Pin8) to VDD.
 - Connect EEPROM_Vss(Pin4) to VSS.


How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
