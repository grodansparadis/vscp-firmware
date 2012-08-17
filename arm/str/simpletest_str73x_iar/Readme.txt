/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the LCD example
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
*********************************************************************************
 THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
 CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
 OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**********************************************************************************


This example contains absolute paths to the include folders and this probaly has to be changed
if installed in a new environment.



Example description
===================
This example allows displaying "STR730 IAR BOARD" message and the ADC End Of 
Conversion interrupt result on the LCD display.

It is made of 6 steps :
1) Analog input configuration (in the GPIOx_InitStructure )
         + Configure the used analog input to High impedance Analog input (Channel0)


2) EIC configuration
   - ADC channel priority configuration
   - Enable ADC channel interrupt
   - Enable the EIC 

3) Clock configuration
         + Select the desired ADC clock input: MCLK = 4Mhz

   
4) ADC configuration  (in the ADC_InitStructure )
  - Initialize the converter  
  - Configure the prescaler register: Sampling prescaler = 0x2 and Conversion prescaler = 0x4 
  - Configure the conversion mode: Scan mode conversion 
  - Select the channel to be converted: only channel0
         + First channel = ADC_CHANNEL0 
	 + Channel number = 1
         
  - Enable EOC interrupt  (using the ADC_ITConfig function )
  - Enable the ADC peripheral (using the ADC_Cmd function )  

5) LCD configuration
  - Initialize the LCD
  - Clear Line 1 & 2
  - Set cursor position:Line 1, column 1
  - Display "STR730 IAR BOARD" message on LCD: Line 1
  - Set cursor position:Line 2, column 1
  - Display "ADC:" message on LCD: Line 2


6) Get the conversion results and display it in the ADC_IRQHandler
  - Start the conversion (using the ADC_ConversionCmd function )
  - Read the conversion result (using the ADC_GetConversionValue function )
  - Display the conversion result on LCD display.
  - Clear EOC flag 

  
Directory contents
==================
73x_conf.h  Library Configuration file
73x_it.c    Interrupt handlers
73x_lcd.h   Functions prototypes for the LCD software library
73x_lcd.c   LCD software functions
main.c      Main program


How to use it
=============
In order to make the program work, you must do the following :

- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
