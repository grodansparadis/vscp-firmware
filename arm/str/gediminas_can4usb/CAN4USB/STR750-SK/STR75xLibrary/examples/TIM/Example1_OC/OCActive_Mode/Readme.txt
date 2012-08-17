/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the TIM Example1_OC: OCActive_Mode.
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
This example shows how to configure the TIM peripheral in Output Compare Active
mode.
The CK_TIM frequency is set to 30 MHz, the TIM is running at 7.324 KHz and the
Output Pulse period is equal to 5.6 s.

While the counter is lower than the Output compare2 register value, which 
determines the Output Pulse period, a led connected to P2.4 pin is turned on. 
When the counter value reachs the Output compare2 register value, the Output 
Compare2 interrupt is generated and in the handler routine this led is turned off.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect a led to P2.4 pin (LD4 on STR750-SK board).

  
How to use it
=============
In order to make the program work, you must do the following :
- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
