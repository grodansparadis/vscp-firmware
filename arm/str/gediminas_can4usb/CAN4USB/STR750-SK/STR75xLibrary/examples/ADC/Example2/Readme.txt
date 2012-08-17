/******************** (C) COPYRIGHT 2006 STMicroelectronics ********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 03/10/2006
* Description        : Description of the ADC Example2.
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
This example provides a description of how to use the PWM TRGO signal as ADC 
injected conversion start trigger.
ADC Channel11 to Channel13 are converted in scan mode while ADC Channel14 is configured 
as injected channel which its start of conversion is driven by the PWM TRGO signal.
The PWM is cofigured to provide the TRGO signal which is synchronized with the 
Output compare OC with a frequency of 10Hz. 
On each rising edge of this signal a conversion of the injected ADC Channel14
starts. 
In an infinite loop the program check the end of injected chain conversion and 
toggle a led connected to P2.3 pin, if this flag is set.
The Channel14 converted value can be checked using your toolchain watch window. 

Directory contents
==================
75x_conf.h  Library Configuration file
75x_it.c    Interrupt handlers
main.c      Main program


Hardware environment
====================
 - Connect a led to pin P2.3 (LD6 on STR750-SK board).
 - Connect a variable power supply 0-3.3V to ADC Channel14 mapped on pin P1.13
   (potentiometer AN_TR on STR750-SK board) 

How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2006 STMicroelectronics *****END OF FILE****
