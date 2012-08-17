/******************** (C) COPYRIGHT 2005 STMicroelectronics **********************
* File Name          : 73x_adc.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the ADC software functions.
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

/* Standard includes -----------------------------------------------------------*/
#include "73x_adc.h"
/* Include of other module interface headers -----------------------------------*/
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/
/* Private function prototypes -------------------------------------------------*/
/* Interface functions ---------------------------------------------------------*/
/* Private functions -----------------------------------------------------------*/

/********************************************************************************
* Function Name  : ADC_DeInit                                           
* Description    : Deinitializes the ADC peripheral registers to their default
*                  reset values.
* Input          : None.  
* Output         : None                                              
* Return         : None.                                                
********************************************************************************/
void ADC_DeInit(void)
{
  
  /*Reset ADC peripheral */
  CFG_PeripheralClockConfig(CFG_CLK_ADC,DISABLE);
  CFG_PeripheralClockConfig(CFG_CLK_ADC,ENABLE); 

}

/*******************************************************************************
* Function Name  : ADC_Init                                                 
* Description    : Initializes the ADC  peripheral according to the specified
*                  parameters in the ADC_InitTypeDef structure.
* Input          : - ADC_InitStruct: pointer to an ADC_InitTypeDef structure that
                   contains the configuration information for the ADC peripheral.                
* Output         : None                                                      
* Return         : None                                                      
********************************************************************************/
void ADC_Init (ADC_InitTypeDef* ADC_InitStruct)
{
  /* Initiate ADC Control Logic registers */
  ADC->CLR0 = 0x0000;
  ADC->CLR1 = 0x0000;
  ADC->CLR2 = 0x0000;
  ADC->CLR4 &= ADC_CLR4_Mask;
  /*Initiate ADC Pending Bit register*/
  ADC->PBR = 0xFFFF;

  /* Calibration */
  if (ADC_InitStruct->ADC_Calibration==ADC_Calibration_ON)
  {
    ADC->CLR4 |= ADC_InitStruct->ADC_CalibAverage;
    ADC->CLR0 |= ADC_InitStruct->ADC_Calibration;
  }

  /* Auto Clock Off */
  ADC->CLR4 |= ADC_InitStruct->ADC_AutoClockOff;

  /* Conversion Mode */
  ADC->CLR2 |= ADC_InitStruct->ADC_ConversionMode;

  /* Sampling and Conversion Prescaler */
  /* Reset SPEN bit and set the Conversion and sampling prescalers values */
  /* write only in LSB bit[7:5] of Sampling_Conversion_Prescaler register */
  ADC->CLR1 = (ADC->CLR1 & 0x7F1F) | (ADC_InitStruct->ADC_ConversionPrescaler<<5);
  /* Set the SPEN bit and set the sampling prescaler value */
  /* write only in LSB bit[2:0] of Sampling_Conversion_Prescaler register */
  if(ADC_InitStruct->ADC_SamplingPrescaler!=0)
  {
    /* Set the sampling prescaler value */
    ADC->CLR1 = (ADC->CLR1 & 0xFFF8) | (ADC_InitStruct->ADC_SamplingPrescaler);
    /* Set SPEN bit */
    ADC->CLR1 = (ADC->CLR1 | 0x8000);
  }

  /* Channels to convert */
  ADC->CLR2 = (ADC->CLR2 | ADC_InitStruct->ADC_FirstChannel) | ((ADC_InitStruct->ADC_ChannelNumber)-1<<6);
}

/********************************************************************************
* Function Name  : ADC_StructInit                                       
* Description    : Initializes the ADC Init Structure parameters                    
* Input          : - ADC_InitStruct: pointer to an ADC_InitTypeDef structure
                     which will be initialized.  
* Output         : None        
* Return         : None.                                                
********************************************************************************/
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct)
{
  /* Reset ADC init structure parameters values */
  ADC_InitStruct->ADC_Calibration = ADC_Calibration_OFF;
  ADC_InitStruct->ADC_SamplingPrescaler = 0;
  ADC_InitStruct->ADC_ConversionPrescaler = 0;
  ADC_InitStruct->ADC_FirstChannel = ADC_CHANNEL0;
  ADC_InitStruct->ADC_ChannelNumber = 0;
  ADC_InitStruct->ADC_CalibAverage = ADC_CalibAverage_Enable;
  ADC_InitStruct->ADC_AutoClockOff = ADC_AutoClockOff_Disable;
  ADC_InitStruct->ADC_ConversionMode = ADC_ConversionMode_OneShot;
 }

/***********************************************************************************
* Function Name  : ADC_CalibrationStart                                       
* Description    : Start ADC Calibration.                                         
* Input          : - ADC_Calib: Enables or disables ADC calibration average.This 
*                    parameter can be one of the following values:
*                         - ADC_CalibAverage_Enable:  enable calibration average 
*                         - ADC_CalibAverage_Disable: disable calibration average  
* Output         : None 
* Return         : None                                                       
************************************************************************************/
void ADC_CalibrationStart (u16 ADC_Calib)
{
  if (ADC_Calib == ADC_CalibAverage_Enable)
  {
    /* Enable ADC Calibration Average */
    ADC->CLR4 &= ~ADC_CalibAverage_Disable;
  }
  else
  {
    /* Disable ADC Calibration Average */
    ADC->CLR4 |= ADC_CalibAverage_Disable;
  }

  /* Start Calibration */
  ADC->CLR0 |= ADC_Calibration_ON;
}

/*********************************************************************************
* Function Name  : ADC_ConversionCmd                                      
* Description    : Start/Stop the Conversion.                             
* Input          : - ADC_Cmd: Start or stop ADC conversion.This parameter can 
*                    be one of the following values:
*                         - ADC_ConversionStart: start conversion 
*                         - ADC_ConversionStop:  stop conversion 
* Output         : None    
* Return         : None                                                   
**********************************************************************************/
void ADC_ConversionCmd (u16 ADC_Cmd)
{
  if (ADC_Cmd == 1)
  {
    /* Start the ADC Conversion */
    ADC->CLR0 |= ADC_Cmd;
  }
  else
  {
    /* Stop the ADC Conversion */
    ADC->CLR0 &= ADC_Cmd;
  }
}

/********************************************************************************
* Function Name  : ADC_Cmd                                               
* Description    : Enables or disables the ADC peripheral                         
*                  - NewState: new state of the ADC peripheral. This parameter
*                    can be: ENABLE or DISABLE.
* Output         : None  
* Return         : None.                                                 
********************************************************************************/
void ADC_Cmd(FunctionalState NewState)
{
  if (NewState == DISABLE)
  {
    /* Enable ADC Power Down Mode */
    ADC->CLR4 |= ADC_PowerDown_Enable;
  }
  else
  {
    /* Disable ADC Power Down Mode */
    ADC->CLR4 &= ADC_PowerDown_Disable;
  }
}

/********************************************************************************
* Function Name  : ADC_AutoClockOffConfig                                
* Description    : Enables or disables the Auto clock off feature for ADC                        
*                  - NewState: new state of the Auto clock off feature. This 
*                    parameter can be: ENABLE or DISABLE.  
* Output         : None   
* Return         : None.                                                 
********************************************************************************/
void ADC_AutoClockOffConfig(FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* Enable ADC Auto Clock Off */
    ADC->CLR4 |= ADC_AutoClockOff_Enable;
  }
  else
  {
    /* Disable ADC Auto Clock Off */
    ADC->CLR4 &= ~ADC_AutoClockOff_Enable;
  }
}

/**********************************************************************************
* Function Name  : ADC_ChannelsSelect                                          
* Description    : Select the channels passed as parameters to be converted.     
* Input          : - FirstChannel: first converted channel. This parameter can be 
*                     ADC_CHANNELx where x can be (0..15). 
*                  - ChannelNumber: number of converted channels. This parameter 
*                     can be a value from 1 to 16.    
* Output         : None          
* Return         : None                                                        
**********************************************************************************/
void ADC_ChannelsSelect(u8 FirstChannel,u8 ChannelNumber )
{
  /* Set the First channel to convert */
  ADC->CLR2 = (ADC->CLR2 & 0xFFF0) | FirstChannel;
  /* Set the Last channel to convert */
  ADC->CLR2 = (ADC->CLR2 & 0xFC3F) | ((ChannelNumber-1)<<6);
}

/**********************************************************************************
* Function Name  : ADC_PrescalersConfig                                   
* Description    : Configure the Sampling and Conversion prescalers       
* Input          : - ADC_Selection: Sampling or Conversion prescaler or both selected.
*                    This parameter can be one of the following values:
*                         - ADC_Both_Prescalers: both sampling and conversion prescalers
*                         - ADC_Sampling_Prescaler: sampling prescaler
*                         - ADC_Conversion_Prescaler: conversion prescaler
*                  - ADC_Prescaler: the prescaler value. 
* Output         : None    
* Return         : None.                                                  
***********************************************************************************/
void ADC_PrescalersConfig(u8 ADC_Selection, u8 ADC_Prescaler)
{
  switch(ADC_Selection)
  {
    case ADC_Both_Prescalers:
      /* Reset SPEN bit and set the Conversion and sampling prescalers values */
      /* write only in LSB bit[2:0] of Sampling_Conversion_Prescaler register */
      ADC->CLR1 = (ADC->CLR1 & 0x7F1F) | (ADC_Prescaler<<5);
      break;

    case ADC_Sampling_Prescaler:
      /* Set the sampling prescaler value */
      /* write only in LSB bit[2:0] of Sampling_Conversion_Prescaler register */
      ADC->CLR1 = (ADC->CLR1 & 0xFFF8) | (ADC_Prescaler);
      /* Set SPEN bit */
      ADC->CLR1 = (ADC->CLR1 | 0x8000);
      break;

    case ADC_Conversion_Prescaler:
      /* Reset the SPEN bit and set the conversion prescaler value */
      /* write only in LSB bit[2:0] of Sampling_Conversion_Prescaler register */
      ADC->CLR1 = (ADC->CLR1 & 0xFF1F) | (ADC_Prescaler<<5);
      break;
  }
}

/***********************************************************************************
* Function Name  : ADC_AnalogWatchdogConfig                                       
* Description    : Configure the High and Low Watchdogs Thresholds for the selected channel                
* Input          : - ADC_Watchdog: the selected analog watchdog. This parameter can be 
*                    one of the following values: 
*                         - ADC_Watchdog0: select analog watchdog 0                       
*		          - ADC_Watchdog1: select analog watchdog 1                       
*         	          - ADC_Watchdog2: select analog watchdog 2                       
*                         - ADC_Watchdog3: select analog watchdog 3                       
*                  - ADC_Channel: the selected channel. This parameter can be
*                    ADC_CHANNELx where x can be (0..15)                    
* 	           - LowThreshold: Low Threshold for the selected watchdog   
* 	           - HighThreshold: High Threshold for the selected watchdog  
* Output         : None
* Return         : None.                                                          
*************************************************************************************/
void ADC_AnalogWatchdogConfig(u16 ADC_Watchdog, u8 ADC_Channel, u16 LowThreshold, u16 HighThreshold)
{
  switch (ADC_Watchdog)
  {
    case ADC_AnalogWatchdog0 :
      ADC->TRA0 = (ADC->TRA0 & 0xC3FF) | ((u16) ADC_Channel<<10);
      ADC->TRA0 = (ADC->TRA0 & 0xFC00) |  HighThreshold;
      ADC->TRB0 = (ADC->TRB0 & 0xFC00) |  LowThreshold;
      break;

    case ADC_AnalogWatchdog1 :
      ADC->TRA1 = (ADC->TRA1 & 0xC3FF) | ((u16) ADC_Channel<<10);
      ADC->TRA1 = (ADC->TRA1 & 0xFC00) |  HighThreshold;
      ADC->TRB1 = (ADC->TRB1 & 0xFC00) |  LowThreshold;
      break;

    case ADC_AnalogWatchdog2 :
      ADC->TRA2 = (ADC->TRA2 & 0xC3FF) | ((u16) ADC_Channel<<10);
      ADC->TRA2 = (ADC->TRA2 & 0xFC00) |  HighThreshold;
      ADC->TRB2 = (ADC->TRB2 & 0xFC00) |  LowThreshold;
      break;

    case ADC_AnalogWatchdog3 :
      ADC->TRA3 = (ADC->TRA3 & 0xC3FF) | ((u16) ADC_Channel<<10);
      ADC->TRA3 = (ADC->TRA3 & 0xFC00) |  HighThreshold;
      ADC->TRB3 = (ADC->TRB3 & 0xFC00) |  LowThreshold;
      break;
  }
}

/*******************************************************************************
* Function Name  : ADC_AnalogWatchdogCmd                                   
* Description    : Enable/Disable the selected analog Watchdog                      
* Input          : - ADC_Watchdog: the selected analog watchdog. This parameter can be 
*                    one of the following values: 
*                         - ADC_Watchdog0: select analog watchdog 0                       
*		          - ADC_Watchdog1: select analog watchdog 1                       
*         	          - ADC_Watchdog2: select analog watchdog 2                       
*                         - ADC_Watchdog3: select analog watchdog 3                                       
*                  - NewState: new state of the specified analog watchdog.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None.                                                   
********************************************************************************/
void ADC_AnalogWatchdogCmd(u16 ADC_Watchdog, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* Enable ADC Thresholds */
    switch (ADC_Watchdog)
    {
      case ADC_AnalogWatchdog0 : ADC->TRB0 |= ADC_AnalogWatchdog_Enable;  break;
      case ADC_AnalogWatchdog1 : ADC->TRB1 |= ADC_AnalogWatchdog_Enable;  break;
      case ADC_AnalogWatchdog2 : ADC->TRB2 |= ADC_AnalogWatchdog_Enable;  break;
      case ADC_AnalogWatchdog3 : ADC->TRB3 |= ADC_AnalogWatchdog_Enable;  break;
    }
  }
  else
  {
    /* Disable ADC Thresholds */
    switch (ADC_Watchdog)
    {
      case ADC_AnalogWatchdog0 : ADC->TRB0 &= ADC_AnalogWatchdog_Disable;  break;
      case ADC_AnalogWatchdog1 : ADC->TRB1 &= ADC_AnalogWatchdog_Disable;  break;
      case ADC_AnalogWatchdog2 : ADC->TRB2 &= ADC_AnalogWatchdog_Disable;  break;
      case ADC_AnalogWatchdog3 : ADC->TRB3 &= ADC_AnalogWatchdog_Disable;  break;
    }
  }
}

/*******************************************************************************
* Function Name  : ADC_GetAnalogWatchdogResult                               
* Description    : Return the analog watchdog comparaison result          
* Input          : - ADC_Watchdog: the selected analog watchdog. This parameter can be 
*                    one of the following values: 
*                         - ADC_Watchdog0: select analog watchdog 0                       
*		          - ADC_Watchdog1: select analog watchdog 1                       
*         	          - ADC_Watchdog2: select analog watchdog 2                       
*                         - ADC_Watchdog3: select analog watchdog 3                                       
* Output         : None              
* Return         : The analog watchdog comparaison result value                 
********************************************************************************/
u16 ADC_GetAnalogWatchdogResult (u16 ADC_Watchdog)
{
  /* Return the AnalogWatchdog comparaison result */
  switch(ADC_Watchdog)
  {
    case ADC_AnalogWatchdog0 :
      return ((ADC->PBR & ADC_Watchdog)>>4);

    case ADC_AnalogWatchdog1 :
      return ((ADC->PBR & ADC_Watchdog)>>6);

    case ADC_AnalogWatchdog2 :
      return ((ADC->PBR & ADC_Watchdog)>>8);

    case ADC_AnalogWatchdog3 :
      return ((ADC->PBR & ADC_Watchdog)>>10);

    default : return (0xFF);  /* if a wrong value of ADC_Watchdog is selected */
  }
}

/*******************************************************************************
* Function Name  : ADC_InjectedConversionStart
* Description    : Start the Injected Conversion.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_InjectedConversionStart (void)
{
  /* Start the injected ADC Conversion */
  ADC->CLR3 |= ADC_Injec_ConversionStart;
}

/*******************************************************************************
* Function Name  : ADC_InjectedChannelsSelect
* Description    : Select injected channels passed as parameter to be converted.
* Input          : - FirstChannel: first converted channel. This parameter can be 
*                     ADC_CHANNELx where x can be (0..15). 
*                  - ChannelNumber: number of converted channels. This parameter 
*                     can be a value from 1 to 16.    
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_InjectedChannelsSelect(u8 FirstChannel,u8 ChannelNumber)
{
  /* Set the First channel to convert */
  ADC->CLR3 = (ADC->CLR3 & 0xFFF0) | FirstChannel;
  /* Set the Last channel to convert */
  ADC->CLR3 = (ADC->CLR3 & 0xFC3F) | (ChannelNumber-1<<6);
}

/*******************************************************************************
* Function Name  : ADC_GetConversionValue
* Description    : Read the conversion result from the data register.
* Input          : - ADC_Channel :ADC channel register value to beread.
*                    This parameter can be ADC_CHANNELx where x can 
*                    be (0..15) to select channelx  
* Output         : None
* Return         : The register value of the converted channel 
*******************************************************************************/
u16 ADC_GetConversionValue( u16 ADC_Channel)
{
  return *((u16 *)(ADC_BASE + ((ADC_Channel<<2) +0x0050)));
}

/*******************************************************************************
* Function Name  : ADC_ITConfig
* Description    : Enables or disables the specified ADC interrupts.
* Input          : - ADC_IT: specifies the ADC interrupts sources to be enabled
*                    or disabled. This parameter can be any combination of the
*                    following values:
*                         - ADC_IT_ECH:  End of chain conversion interrupt mask
*                         - ADC_IT_EOC:  End of channel conversion interrupt mask
*                         - ADC_IT_JECH: End of injected chain conversion interrupt mask
*                         - ADC_IT_JEOC: End of injected channel conversion interrupt mask
*                         - ADC_IT_AnalogWatchdog0_LowThresold:  Analog Watchdog0 LowThresold interrupt mask       
*                         - ADC_IT_AnalogWatchdog0_HighThresold: Analog Watchdog0 HighThresold interrupt mask 	
*                         - ADC_IT_AnalogWatchdog1_LowThresold:  Analog Watchdog1 LowThresold interrupt mask  	
*                         - ADC_IT_AnalogWatchdog1_HighThresold: Analog Watchdog1 HighThresold interrupt mask  	
*                         - ADC_IT_AnalogWatchdog2_LowThresold:  Analog Watchdog2 LowThresold interrupt mask  	
*                         - ADC_IT_AnalogWatchdog2_HighThresold: Analog Watchdog2 HighThresold interrupt mask 	
*                         - ADC_IT_AnalogWatchdog3_LowThresold:  Analog Watchdog3 LowThresold interrupt mask  	
*                         - ADC_IT_AnalogWatchdog3_HighThresold: Analog Watchdog3 HighThresold interrupt mask 	
*                  - NewState: new state of the specified ADC interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ITConfig(u16 ADC_IT, FunctionalState NewState )
{
  if (NewState == ENABLE)
  {
    ADC->IMR |= ADC_IT;
  }
  else
  {
    ADC->IMR &= ~ADC_IT;
  }
}

/*******************************************************************************
* Function Name  : ADC_FlagClear
* Description    : Clear the ADC flags passed as parameters 
* Input          : - ADC_Flag: flag to clear. This parameter can be can be any 
*                    combination of the following values:
*                         - ADC_FLAG_ECH:  End of chain conversion
*                         - ADC_FLAG_EOC:  End of channel conversion
*                         - ADC_FLAG_JECH: End of injected chain conversion
*                         - ADC_FLAG_JEOC: End of injected channel conversion
*                         - ADC_FLAG_AnalogWatchdog0_LowThresold:  Analog Watchdog 0 LowThresold Flag        
*                         - ADC_FLAG_AnalogWatchdog0_HighThresold: Analog Watchdog 0 HighThresold Flag 	
*                         - ADC_FLAG_AnalogWatchdog1_LowThresold:  Analog Watchdog 1 LowThresold Flag  	
*                         - ADC_FLAG_AnalogWatchdog1_HighThresold: Analog Watchdog 1 HighThresold Flag  	
*                         - ADC_FLAG_AnalogWatchdog2_LowThresold:  Analog Watchdog 2 LowThresold Flag  	
*                         - ADC_FLAG_AnalogWatchdog2_HighThresold: Analog Watchdog 2 HighThresold Flag 	
*                         - ADC_FLAG_AnalogWatchdog3_LowThresold:  Analog Watchdog 3 LowThresold Flag  	
*                         - ADC_FLAG_AnalogWatchdog3_HighThresold: Analog Watchdog 3 HighThresold Flag 	
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_FlagClear (u16 ADC_Flag)
{
  /* Clear flags by setting their corresponding bits in PBR register */
  ADC->PBR = ADC_Flag;
}

/*******************************************************************************
* Function Name  : ADC_FlagStatus
* Description    : Checks whether the specified ADC flag is set or not.
* Input          : - ADC_Flag: flag to check. This parameter can be one of the
*                    following values:
*                         - ADC_FLAG_ECHECH :End of chain conversion
*                         - ADC_FLAG_EOC :End of channel conversion
*                         - ADC_FLAG_JECH :End of injected chain conversion
*                         - ADC_FLAG_JEOC :End of injected channel conversion 
* Output         : None	
* Return         : The NewState of the ADC_Flag (SET or RESET).
*******************************************************************************/
FlagStatus ADC_FlagStatus (u8 ADC_Flag)
{
  /* Test on the flag status and return set or RESET */
  if(ADC->PBR & ADC_Flag)
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*******************************************************************************
* Function Name  : ADC_DMAConfig
* Description    : Enables or disables the ADC channels for DMA transfert.
* Input          : - ADC_DMAChannel: channels selected to be enabled or disabled 
*                    for DMA transfert. This parameter can be any combination of 
*                    ADC_DMA_CHANNELx where x can be (0..15). 
*                  - NewState: new state of the specified ADC DMA channels.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_DMAConfig(u16 ADC_DMAChannel, FunctionalState NewState )
{
  if (NewState == ENABLE)
  {
    /* select the DMA channel to be enabled */
    ADC->DMAR |= ADC_DMAChannel ;
  }
  else
  {
    ADC->DMAR &= ~ADC_DMAChannel;
  }
}

/*******************************************************************************
* Function Name  : ADC_DMACmd
* Description    : Enable or disable global DMA capability for the ADC.
* Input          : - NewState: new state of the ADC DMA capability.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_DMACmd (FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* Enable global DMA capability */
    ADC->DMAE |= ADC_DMA_Enable ;
  }
  else
  {
    ADC->DMAE &= ADC_DMA_Disable;
  }
}

/********************************************************************************
* Function Name  : ADC_DMAFirstEnabledChannel
* Description    : Return the first DMA enabled channel
* Input          : None
* Output         : None
* Return         : The first DMA enabled channel
********************************************************************************/
u16 ADC_DMAFirstEnabledChannel ( void )
{
  /* Return the DMA first enabled channel */
  return (ADC->DMAE & 0x000F);
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE*****/

