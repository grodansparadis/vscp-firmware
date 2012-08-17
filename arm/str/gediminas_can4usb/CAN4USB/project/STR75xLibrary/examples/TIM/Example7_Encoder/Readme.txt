/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 07/17/2006
* Description        : Description of the TIM Example7: Encoder Mode.
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
This example shows how to use the TIM sw driver to configure the TIM peripheral
in Encoder3 mode: Counter counts up/down on both IC1 and IC2 edges. 
The IC1 is configured to be connected to TI1 and IC2 is configured to be connected
to TI2. The polarities for both inputs are selected as falling edge.
The CK_TIM frequency is set to 60 MHz.

Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 
  
How to use it
=============
In order to make the program work, you must do the following:

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****
