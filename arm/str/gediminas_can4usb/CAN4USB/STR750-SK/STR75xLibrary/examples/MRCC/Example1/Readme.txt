/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the MRCC Example1.
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
This example shows how to enters the system to STANDBY mode and wake-up from this
mode using: external RESET, RTC Alarm or WKP_STDBY pin.
In the associated software, the system clock is set to 60 MHz, the EXTIT line7
is configured to generate an interrupt on falling edge and the RTC is programmed
to generate an interrupt each 1 second. In the RTC interrupt handler, a led 
connected to P2.3 pin is toggled, this is used to indicate whether the MCU is 
in STANDBY or RUN mode.
When a falling edge is detected on EXTIT line7 an interrupt is generated. In the 
EXTIT handler routine the RTC is configured to generate an Alarm event in 4 second
then the system enters STANDBY mode causing the P2.3 pin to stop toggling. 
A rising edge on WKP_STDBY pin or an external RESET will wake-up the system from
STANDBY. If within 4 second neither rising edge on WKP_STDBY nor external RESET
are generated, the RTC Alarm will wake-up the system. After that the P2.3 pin 
restarts toggling.
The system clock is outputted on P0.01 pin, you can monitor this clock using an
oscilloscope.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect the P0.01 pin to an oscilloscope  
 - Connect a led to pin P2.3 (LD6 on STR750-SK board).
 - Connect a push-button to WKP_STDBY pin (P1.15) (Wakeup push-button on
   STR750-SK board).
 - Connect a push-button to EXTIT line7 pin (P1.05) (Key push-button on
   STR750-SK board).
   

How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
