/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : Description of the SMI Example2.
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
This example demonstrates how to boot on the external Serial Flash connected to
SMI Bank0.
The system clock is set to 48MHz and the SMI runs at 48MHz (SMI_PRESC = 1) with
Fast Read mode enabled.
The executed program toggles four led connected to P1.00, P1.01, P1.02 and P1.03
pins.

      
Directory contents
==================
75x_conf.h     Library Configuration file
75x_it.c       Interrupt handlers
main.c         Main program


Hardware environment
====================
- This example runs on the STR750-SK board and use the M25P64 SPI FLASH.
- Configure the 2 jumpers BOOT0 & BOOT1 in order to boot from External SMI
     
How to use it
=============
In order to make the program work, you must do the following :

- In the 75x_init.s file enable the 'SMI_Bank0_EN' define          
- Load your image into the external Flash using the External_SMI flash loader
- Run the example

******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****
