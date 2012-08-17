/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_adc.h
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file contains all the functions prototypes for the
*                      ADC software library.
**********************************************************************************
* History:
* 09/27/2005 :  V1.0
**********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH
* CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT
* OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
* CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*********************************************************************************/


/* Define to prevent recursive inclusion ---------------------------------------*/
#ifndef __73x_ADC_H
#define __73x_ADC_H

/* Includes --------------------------------------------------------------------*/
#include "73x_map.h"
#include "73x_cfg.h"


/* Exported types --------------------------------------------------------------*/

/* ADC Init structure definition */
typedef struct
{
    u8  ADC_Calibration;
    u8  ADC_SamplingPrescaler;
    u8  ADC_ConversionPrescaler;
    u8  ADC_FirstChannel;
    u8  ADC_ChannelNumber;
    u16 ADC_CalibAverage;
    u16 ADC_AutoClockOff;
    u16 ADC_ConversionMode;
 }ADC_InitTypeDef;

/* ADC control status flags */
#define    ADC_FLAG_ECH             	   	        0x0001
#define    ADC_FLAG_EOC             	   		0x0002
#define    ADC_FLAG_JECH            	   		0x0004
#define    ADC_FLAG_JEOC            	   		0x0008
#define    ADC_FLAG_AnalogWatchdog0_LowThresold         0x0010
#define    ADC_FLAG_AnalogWatchdog0_HighThresold  	0x0020
#define    ADC_FLAG_AnalogWatchdog1_LowThresold   	0x0040
#define    ADC_FLAG_AnalogWatchdog1_HighThresold  	0x0080
#define    ADC_FLAG_AnalogWatchdog2_LowThresold   	0x0100
#define    ADC_FLAG_AnalogWatchdog2_HighThresold  	0x0200
#define    ADC_FLAG_AnalogWatchdog3_LowThresold   	0x0400
#define    ADC_FLAG_AnalogWatchdog3_HighThresold  	0x0800

/* ADC Interrupt sources */
#define    ADC_IT_None             	   		0x0000
#define    ADC_IT_ECH             	   		0x0001
#define    ADC_IT_EOC             	   		0x0002
#define    ADC_IT_JECH            	   		0x0004
#define    ADC_IT_JEOC            	   		0x0008
#define    ADC_IT_AnalogWatchdog0_LowThresold           0x0010
#define    ADC_IT_AnalogWatchdog0_HighThresold  	0x0020
#define    ADC_IT_AnalogWatchdog1_LowThresold   	0x0040
#define    ADC_IT_AnalogWatchdog1_HighThresold  	0x0080
#define    ADC_IT_AnalogWatchdog2_LowThresold   	0x0100
#define    ADC_IT_AnalogWatchdog2_HighThresold  	0x0200
#define    ADC_IT_AnalogWatchdog3_LowThresold   	0x0400
#define    ADC_IT_AnalogWatchdog3_HighThresold  	0x0800

/* ADC Watchdogs Thresholds */
#define  ADC_AnalogWatchdog0   0x0030
#define  ADC_AnalogWatchdog1   0x00C0
#define  ADC_AnalogWatchdog2   0x0300
#define  ADC_AnalogWatchdog3   0x0C00

/* ADC Channels */
#define  ADC_CHANNEL0   0x0
#define  ADC_CHANNEL1   0x1
#define  ADC_CHANNEL2   0x2
#define  ADC_CHANNEL3   0x3
#define  ADC_CHANNEL4   0x4
#define  ADC_CHANNEL5   0x5
#define  ADC_CHANNEL6   0x6
#define  ADC_CHANNEL7   0x7
#define  ADC_CHANNEL8   0x8
#define  ADC_CHANNEL9   0x9
#define  ADC_CHANNEL10  0xA
#define  ADC_CHANNEL11  0xB
#define  ADC_CHANNEL12  0xC
#define  ADC_CHANNEL13  0xD
#define  ADC_CHANNEL14  0xE
#define  ADC_CHANNEL15  0xF

/* ADC DMA Channels */
#define  ADC_DMA_CHANNEL0   0x0001
#define  ADC_DMA_CHANNEL1   0x0002
#define  ADC_DMA_CHANNEL2   0x0004
#define  ADC_DMA_CHANNEL3   0x0008
#define  ADC_DMA_CHANNEL4   0x0010
#define  ADC_DMA_CHANNEL5   0x0020
#define  ADC_DMA_CHANNEL6   0x0040
#define  ADC_DMA_CHANNEL7   0x0080
#define  ADC_DMA_CHANNEL8   0x0100
#define  ADC_DMA_CHANNEL9   0x0200
#define  ADC_DMA_CHANNEL10  0x0400
#define  ADC_DMA_CHANNEL11  0x0800
#define  ADC_DMA_CHANNEL12  0x1000
#define  ADC_DMA_CHANNEL13  0x2000
#define  ADC_DMA_CHANNEL14  0x4000
#define  ADC_DMA_CHANNEL15  0x8000

/* Mask for DMA enable*/
#define ADC_DMA_Enable       0x8000
#define ADC_DMA_Disable      0x000F

/* Mask for Injected conversion start */
#define ADC_Injec_ConversionStart  0x8000

/* Mask for Watchdog Thresholds Enable */
#define ADC_AnalogWatchdog_Enable  0x8000
#define ADC_AnalogWatchdog_Disable 0x7FFF

/* Mask for Power Down Mode */
#define ADC_PowerDown_Enable  0x8000
#define ADC_PowerDown_Disable 0x7FFF

/* Mask for Start Conversion */
#define ADC_ConversionStart   0x0001
#define ADC_ConversionStop    0xFFFE

/* Mask for Prescaler selection */
#define ADC_Sampling_Prescaler    0x1
#define ADC_Conversion_Prescaler  0x2
#define ADC_Both_Prescalers       0x3

/* ADC Conversion Modes */
#define ADC_ConversionMode_Scan     0x8000
#define ADC_ConversionMode_OneShot  0x0000

/* Mask for Auto Clock Off */
#define ADC_AutoClockOff_Enable  0x4000
#define ADC_AutoClockOff_Disable 0x0000

/* Mask for Calibration */
#define ADC_Calibration_ON       0x0002
#define ADC_Calibration_OFF      0x0000
#define ADC_CalibAverage_Disable 0x0020
#define ADC_CalibAverage_Enable  0x0000

/* ADC Init Mask */
#define ADC_CLR4_Mask 0x8000

/* Module private variables ----------------------------------------------------*/

/* Exported macro --------------------------------------------------------------*/

/* Private functions -----------------------------------------------------------*/

/* Exported functions ----------------------------------------------------------*/

void ADC_DeInit (void);
void ADC_StructInit (ADC_InitTypeDef *ADC_InitStruct);
void ADC_Init (ADC_InitTypeDef *ADC_InitStruct);
void ADC_CalibrationStart (u16 ADC_Calib);
void ADC_ConversionCmd (u16 ADC_Cmd);
void ADC_Cmd (FunctionalState NewState);
void ADC_AutoClockOffConfig (FunctionalState NewState);
void ADC_ChannelsSelect (u8 FirstChannel,u8 ChannelNumber );
void ADC_PrescalersConfig (u8 ADC_Selection, u8 ADC_Prescaler);
void ADC_AnalogWatchdogConfig(u16 ADC_Watchdog, u8 ADC_Channel,
                              u16 LowThreshold, u16 HighThreshold);
void ADC_AnalogWatchdogCmd(u16 ADC_Watchdog, FunctionalState NewState);
u16 ADC_GetAnalogWatchdogResult (u16 ADC_Watchdog);
void ADC_InjectedConversionStart (void);
void ADC_InjectedChannelsSelect (u8 FirstChannel,u8 ChannelNumber );
u16 ADC_GetConversionValue( u16 ADC_Channel);
void ADC_ITConfig (u16 ADC_IT, FunctionalState NewState );
void ADC_FlagClear (u16 ADC_Flag);
FlagStatus ADC_FlagStatus (u8 ADC_Flag);
void ADC_DMAConfig(u16 ADC_DMAChannel, FunctionalState NewState);
void ADC_DMACmd (FunctionalState NewState);
u16 ADC_DMAFirstEnabledChannel (void);
#endif /*__73x_ADC_H */

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE******/
