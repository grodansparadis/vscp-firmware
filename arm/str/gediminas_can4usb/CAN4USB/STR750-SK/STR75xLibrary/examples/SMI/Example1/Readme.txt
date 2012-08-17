/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the SMI Example 1.
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
This example demonstrates how to use the SMI controller and an associate SMI FLASH
driver to perform write, read and erase operations on an SPI based FLASH. 
The SMI hardware mode is used for write/read operations, while the software mode
is used for sector erase, bulk erase and to read the FLASH ID.
This example erases several FLASH sectors, writes data to these sectors then the
written data are read and checked for correctness. bWriteStatus, hWriteStatus and
wWriteStatus variables reports if the write operations was successful or not.
Finally, the FLASH ID is read and stored in FALSHID variable.
The SMI clock is set to 30 MHz.

      
Directory contents
==================
75x_conf.h     Library Configuration file
75x_it.c       Interrupt handlers
main.c         Main program
smi_flash.c    SMI Flash driver
smi_flash.h    Header for smi_flash.c module 


Hardware environment
====================
This example runs on the STR750-SK IAR board and use the M25P64 SPI FLASH.
   
     
How to use it
=============
In order to make the program work, you must do the following :
       

- Run the example

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
