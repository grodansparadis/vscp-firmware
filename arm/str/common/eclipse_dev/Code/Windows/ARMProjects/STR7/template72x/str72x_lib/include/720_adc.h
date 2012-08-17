/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_adc.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the ADC software functions headers
**********************************************************************************
* History:
* 12/20/2004 :  V1.1
* 08/02/2004 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH  CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/

#ifndef __720_ADC_H
#define __720_ADC_H

#include "720_lib.h"

#ifndef _RCCU
  #error _RCCU Must be Defined in "720_conf.h"
#endif




//--------------------ADC Conversion modes-----------------------------------
typedef enum
{
  ADC_SINGLE,
  ADC_ROUND
} ADC_Modes;

//--------------------ADC Channels-------------------------------------------
typedef enum
{
  ADC_CHANNEL0 = 0x00,
  ADC_CHANNEL1 = 0x10,
  ADC_CHANNEL2 = 0x20,
  ADC_CHANNEL3 = 0x30
} ADC_Channels;

//--------------------ADC control status register flag-----------------------

#define  ADC_DA0    0x01
#define  ADC_DA1    0x02
#define  ADC_DA2    0x04
#define  ADC_DA3    0x08
#define  ADC_OR     0x2000
#define  ADC_DEC    0x4000

// Masks for the update of interrupt bit for channel n
#define ADC_IT0_Mask   0x0100
#define ADC_IT1_Mask   0x0200
#define ADC_IT2_Mask   0x0400
#define ADC_IT3_Mask   0x0800

//Mask for the update of all the interrupt bit in the CSR
#define ADC_IT_Mask    0x0F00

//Mask for Selecting mode
#define ADC_Mode_Mask  0x0040

//Mask for configuring the converter
#define ADC_Start_Mask        0x0000
#define ADC_Stop_Mask         0x0004
#define ADC_Decimation_Mask   0x4000

/*******************************************************************************
* Function Name      : ADC_Init
* Description        : Intialize the ADC
* Input              : None.
* Return             : None.
*******************************************************************************/
void ADC_Init(void);

/*******************************************************************************
* Function Name      : ADC_DecimationMode
* Description        : Select the decimation mode .
* Input              : None
* Return             : None
*******************************************************************************/
static inline void ADC_DecimationMode (void)
{
  // Set the ADC_DEC bit of the ADCCSR register
  ADC->CSR |= ADC_Decimation_Mask;
}

/*******************************************************************************
* Function Name      : ADC_ConvertsionStart
* Description        : Declenche the Conversion.
* Input              : None
* Return             : None
*******************************************************************************/
static inline void ADC_ConvertionStart(void)
{
  // Clear the ADC_STOP bit of the AGCR1 register
  AGCR->CR1 &= ADC_Start_Mask;
}

/*******************************************************************************
* Function Name      : ADC_ConversionStop
* Description        : Stop the ADC
* Input              : None.
* Return             : None.
*******************************************************************************/
static inline void ADC_ConversionStop(void)
{
  // Set the ADC_STOP bit of the AGCR1 register
  AGCR->CR1 |= ADC_Stop_Mask;
}

/*******************************************************************************
* Function Name      : ADC_ModeConfig
* Description        : Configure the mode of conversion
* Input              : ADC_SINGLE: single channel mode
*                      ADC_ROUND : round robin mode
* Return             : None
*******************************************************************************/
static inline void ADC_ModeConfig (ADC_Modes Mode)
{
  // Select the mode of conversion and update the CSR[6]
  ADC->CSR= Mode == ADC_SINGLE ? ADC->CSR | ADC_Mode_Mask : ADC->CSR & ~ADC_Mode_Mask;
}

/*******************************************************************************
* Function Name      : ADC_PrescalerConfig
* Description        : Configure the prescaler
* Input              : ADC_clk: Sampling frequency.
* Return             : None.
*******************************************************************************/
void ADC_PrescalerConfig(vu32 ADC_clk);

/*******************************************************************************
* Function Name      : ADC_ChannelSelect
* Description        : select the channel passed as parameter to be converted.
* Input              : ADC_Channel: channel selected to be converted it may be
*                      ADC_CHANNEL0 : select channel 0
*                      ADC_CHANNEL1 : select channel 1
*                      ADC_CHANNEL2 : select channel 2
*                      ADC_CHANNEL3 : select channel 3
* Return             : None
*******************************************************************************/
static inline void ADC_ChannelSelect(ADC_Channels ADC_Channel)
{
  // Update the CSR by the value of the selected channel
  ADC->CSR |= ADC_Channel;
}

/*******************************************************************************
* Function Name      : ADC_GetStatus
* Description        : test if the flag passed in parameter is set or not
* Input              : ADC_DA0 :Data Available on Channel 0
*                      ADC_DA1 :Data Available on Channel 1
*                      ADC_DA2 :Data Available on Channel 2
*                      ADC_DA3 :Data Available on Channel 3
*                      ADC_OR  :Overrun
*                      ADC_DEC :Decimation
* Return             : SET: if the flag is set
*                      RESET: if the flag is clear
*******************************************************************************/
static inline u16  ADC_GetStatus (vu16 ADC_Flag)
{
  // Test on the flag status and return set or RESET
  return ADC->CSR & ADC_Flag ? SET : RESET;
}

/*******************************************************************************
* Function Name      : ADC_ConversionValue
* Description        : Read the conversion result from the data register.
* Input              : ADC_Channel :number of the register to read
*                      ADC_CHANNEL0 : read the DATA0 register
*                      ADC_CHANNEL1 : read the DATA1 register
*                      ADC_CHANNEL2 : read the DATA2 register
*                      ADC_CHANNEL3 : read the DATA3 register
* Return             : the register value of the channel converted
*******************************************************************************/
static inline u16 ADC_ConversionValue( ADC_Channels ADC_Channel)
{
  // Only the 12 MSB of the DATAn Register are taken
  return *(u16 *)(ADC_BASE + (ADC_Channel >> 1)) >> 4;
}

/*******************************************************************************
* Function Name      : ADC_ITConfig
* Description        : enable or disable the interruption
* Input              : NewState=ENABLE=>enable interrupt
*                      NewState=DISABLE=>disable interrupt
* Return             : None
*******************************************************************************/
void ADC_ITConfig (FunctionalState NewState);

#endif // __720_ADC_H

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/
