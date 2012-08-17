/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 05/18/2006 : Version 1.0
* Description        : Description of the GPIO Example.
********************************************************************************
* History:
* 05/24/2006 : Version 1.1
* 05/18/2006 : Version 1.0
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
This example provides a short description of how to use the GPIO peripheral and
gives a configuration sequence. 
Four leds connected to P9.0, P9.1, P9.2 and P9.3 pins are toggled in an 
infinite loop.


Directory contents
==================
91x_conf.h  Library Configuration file
91x_it.c    Interrupt handlers
main.c      Main program
Readme.txt   This file

Hardware environment
====================
 - Connect four leds to pins P9.0, P9.1, P9.2 and P9.3(respectively LD2,LD3,
   LD4 and LD5 on STR91x-EVAL board).
   
     
How to use it
=============
In order to make the program work, you must do the following :
- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files :
  + 91x_lib.c
  + 91x_gpio.c
  + 91x_scu.c
      
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
