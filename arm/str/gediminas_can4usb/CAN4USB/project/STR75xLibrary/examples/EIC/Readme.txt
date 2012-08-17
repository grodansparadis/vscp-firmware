/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the EIC Example.
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
This example demontrates the use of the Enhanced Interrupt Controller (EIC): 

- Configuration of 3 TIM (TIM0..TIM2)timers to generate an interrupt on each 
  counter update event.
- The three timers are linked to their correspondant Update IRQ channel.
- Assignment of a ascendant IRQ priority for each IRQ channel :
  TIM0 has a priority of 1 and TIM2 has a priority of 3.
- In each interrupt routine: 
   - TIM0 toggles a led connected to P1.0 pin each 1s 
   - TIM1 toggles a led connected to P1.1 pin each 2s 
   - TIM2 toggles a led connected to P1.2 pin each 3s 

      
Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
Connect three leds to pins P1.0, P1.1 and P1.2(respectively LD1, LD2 and LD3
on STR750-SK board).
   
     
How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
