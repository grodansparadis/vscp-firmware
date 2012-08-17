/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : Readme.txt
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Description of the ADC example
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

Example description
===================
This example demonstrates the ADC End Of Conversion interrupt :
- The prescaler configuration
- ADC channels configuration 
- The use of the converter in the scan conversion mode
- ADC Interrupt configuration : End Of Conversion (EOC)
- ADC Analog watchdog configuration
- ADC interrupt handling

It is made of 5 steps :
1) Analog input configuration (in the GPIOx_InitStructure )
         + Configure the used analog input to High impedance Analog input (Channel0)

2) Clock configuration
         + Select the desired ADC clock input: MCLK = 8Mhz

3) EIC configuration
   - ADC channel priority configuration
   - Enable ADC channel interrupt
   - Enable the EIC 
   
4) ADC configuration  (in the ADC_InitStructure )
  - Initialize the converter  
  - Configure the prescaler register: Sampling prescaler = 0x7 and Conversion prescaler = 0x0 
  - Configure the conversion mode: Scan mode conversion 
  - Select the channel to be converted: only channel0
         + First channel = ADC_CHANNEL0 
	 + Channel number = 1
  - Configure Analog watchdog: select Analog Watchdog0 for channel0
         + Analog Watchdog0 Low Thresold set to 0x100
	 + Analog Watchdog0 High Thresold set to 0x250
       
  - Enable EOC interrupt  (using the ADC_ITConfig function )

  - Enable the ADC peripheral (using the ADC_Cmd function )
  
  - Start the conversion (using the ADC_ConversionCmd function )

5) Get the conversion and Analog watchdog0 comparison results and display it in the ADC_IRQHandler
  - Read the conversion result (using the ADC_GetConversionValue function )
  - Display the conversion result on GPIO4 port.
  - Clear EOC flag 

  - Read the analog watchdog comparison result (using the ADC_GetAnalogWatchdogResult function ) 
  - Display the comparison result on GPIO2
  - Clear the last Analog Watchdog0 comparison result in order to get the new value


Directory contents
==================
73x_conf.h  Library Configuration file
73x_it.c    Interrupt handlers
main.c      Main program

How to use it
=============
In order to make the program work, you must do the following :

- Create a project and setup all your toolchain's start-up files
- Compile the directory content files and required Library files:
  + 73x_lib.c
  + 73x_cfg.c
  + 73x_eic.c
  + 73x_prccu.c
  + 73x_gpio.c
  + 73x_cmu.c
  + 73x_adc.c
- Link all compiled files and load your image into either RAM or Flash
- Run the example


******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****
