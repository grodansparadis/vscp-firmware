/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the SSP Example2.
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
This example provides a description of how to set a communication between the two
SSPs and performs a transfer from Master to Slave in Motorola frame format and 
using Slave receive FIFO interrupt.
SSP0 is configured as Master in Motorola frame format and with an 16bits data size 
to communicate with the Slave SSP1 configured in motorola frame format and with the
same data size as SSP0. The clock set to the master is equal to 2,5MHz. The NSS 
pin is managed by software.
The Master send the SSP0_Buffer_Tx to be received by the Slave one by one.
Each time the Slave receive FIFO contain four data an interrupt is generated and 
the program read those data and store them into the SSP1_Buffer_Rx buffer
The data read by the SSP1 peripheral are compared with the expected values of 
the transmitted buffer. The result of this comparison is stored in the 
"TransferStatus" variable.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect both SSP0 and SSP1 pins as following:
 - Connect SSP0_SCLK(P0.05) to SSP1_SCLK(P0.16).
 - Connect SSP0_MISO(P0.06) to SSP1_MISO(P0.17).
 - Connect SSP0_MOSI(P0.07) to SSP1_MOSI(P0.18).
 

How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
