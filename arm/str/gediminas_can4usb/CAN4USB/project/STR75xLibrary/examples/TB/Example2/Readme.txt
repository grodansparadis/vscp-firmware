/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the TB Example2.
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
This example demonstrates and explains how to use the TB peripheral to calibrate
the LPOSC Oscillator.
As an application example, it demonstrates how to select the LPOSC as RTC clock
source then measure the LPOSC frequency with TB input capture. 
The RTC prescaler is adjusted according to the measured value to have a time 
base of 1 second. Each second, in the RTC interrupt handler, a led connected to 
P2.4 is toggled and after 4 second the RTC is configured to generate an Alarm 
event in 5 second then the system enters STANDBY mode. When the system has entered 
STANDBY mode the led stops toggling and the RTC Alarm will wake-up the system 
in 5 second then the led starts toggling. This behavior is repeated indefinitely.

Note: as the period of the LPOSC shifts around +30% in STANDBY mode (see STR75x
refrence manual), the Alarm period will be set to 3s.

Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect a led to pin P2.4(LD7 on STR750-SK board).
    
     
How to use it
=============
In order to make the program work, you must do the following :
  
- Load your image into Flash
- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
