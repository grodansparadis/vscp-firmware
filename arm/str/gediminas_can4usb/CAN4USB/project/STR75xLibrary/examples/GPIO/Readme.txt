/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the GPIO Example.
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
This example provides a short description of how to use the GPIO peripheral and
gives a configuration sequence. It shows also how to use the bit mask feature.
16 leds connected to P1.0.. P1.3, P1.14,P2.3,P2.4,P0.0,P0.1 and P0.23..31 pins are 
toggled in an infinite loop.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect four leds to pins P1.0.. P1.3, P1.14,P2.3,P2.4,P0.0,P0.1 and P0.23..31
 (respectively LD1.. LD16 on STR750-SK board).
   
     
How to use it
=============
In order to make the program work, you must do the following :
- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****
