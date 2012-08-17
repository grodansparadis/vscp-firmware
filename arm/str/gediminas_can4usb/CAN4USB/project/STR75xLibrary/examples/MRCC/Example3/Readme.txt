/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the MRCC Example3.
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
This example describes how to configure the different system clocks. It shows 
how to use, for debug purpose, the MRCC_GetClocksStatus function to retrieve the 
current status and frequencies of different on chip clocks. 
It handles also the 4MHz external Quartz oscillator failure detection: when the 
clock disappears on the XT1 pin (broken or disconnected external 4MHz Quartz), 
the system automatically switches to the FREEOSC oscillator and an interrupt 
(NCKD) is generated. 
In this example, NCKD interrupt is generated indefinitely while there is no clock
detected on OSC4M. Once this clock is recovered the system clock is reconfigured
to its previous state.
A led connected to P2.3 pin is toggled with a frequency depending on the system
clock. If no clock is present on OSC4M this led stops toggling.

The system clock is outputted on P0.01 pin, you can monitor this clock using an 
oscilloscope.

You can see the MRCC_Clocks structure content, which hold the status and 
frequencies of different on chip clocks, using your toolchain debugger.

Note: When fetching from the embedded FLASH with HCLK clock higher than 32MHz,
the FLASH burst mode must be enabled. 


Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect the MCO pin (P0.01) to an oscilloscope . 
 - Connect a led to pin P2.3 (LD4 on STR750-SK board).
 - On STR750-SK board the 4MHz external Quartz oscillator is mounted on socket 
   so it is easy to disconnect it.
   

How to use it
=============
In order to make the program work, you must do the following :
- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE******
