/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the MRCC Example2.
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
This example shows how to enter the system to STOP mode and wake-up from this
mode using: external Interrupt, RTC Alarm or NCKD(no clock detected) interrupt.

In the associated software, the system clock is set to 60 MHz and an interrupt 
will be generated if no clock is present on OSC4M.
The EXTIT line7(P1.05) and EXTIT line15 are configured to generate interrupts on
rising edge and the RTC is programmed to generate an interrupt each 1 second. 
The EXTIT line15 is shared between the RTC_Alarm event and the WKP_STDBY pin 
(P1.15). 
In the RTC interrupt handler, a led connected to P2.3 pin is toggled, this is 
used to indicate whether the MCU is in STOP or RUN mode.

The system enters and exits STOP mode as following:
After system power on, a led connected to P2.3 pin is toggled each 1 second and 
after 4 second the RTC is configured to generate an Alarm event in 5 second
then the system enters STOP mode causing the P2.3 pin to stop toggling. 
To wake-up from STOP mode you can either apply a rising edge on EXTIT line15,
EXTIT line7 or disconnect the 4MHz external Quartz oscillator.
If within 5 second none of those actions are performed the RTC Alarm will wake-up
the system. After that the P2.3 pin restarts toggling and in 4 second the system
will enter again in STOP mode then exit in the way described above. This behavior
is repeated in an infinite loop.

When the 4MHz external Quartz oscillator is disconnected, the VCO of the PLL (FREEOSC)
supplies the system clock. Once the 4MHz clock has recovered (by connecting the 
4MHz Quartz oscillator) the system clock is reconfigured to 60 MHz.

If the system fails to enter STOP mode, a led connected to P2.4 pin is turned on.

The system clock is outputted on P0.01 pin, you can monitor this clock using an 
oscilloscope.


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect the P0.01 pin to an oscilloscope .
 - Connect two leds to P2.3 and P2.4 pins (respectively LD6 and LD7 on 
   STR750-SK board).
 - Connect a push-button to WKP_STDBY pin (P1.15) (Wakeup push-button on
   STR750-SK board).
 - Connect a push-button to EXTIT line7 pin (P1.05) (Key push-button(B1)) on
   STR750-SK board).
 - on STR750-SK board the 4MHz Quartz oscillator is mounted on socket so it is 
   easy to disconnect it.
   

How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
