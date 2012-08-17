/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_cmu.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the CMU software functions.
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

/* Standard include ----------------------------------------------------------*/
#include "73x_cmu.h"
/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
 
/******************************************************************************
* Function Name  : CMU_DeInit
* Description    : Deinitializes the CMU peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void CMU_DeInit(void)
{ 
  CMU->WE = CMU_KeyValue1;
  CMU->WE = CMU_KeyValue2;
  /*Initialize CRF bit in CMU_STAT register*/
  CMU->CTRL  = 0x0200; 
  
  /*Deinitializes RC oscillator control bits*/
  CMU->RCCTL = 0x0008;
  
  /*Deinitializes Frequency reference high bits*/
  CMU->FRH   = 0x0FFF;
  
  /*Deinitializes Frequency reference low bits*/
  CMU->FRL   = 0x0000;
  
  /*Deinitializes CTRL regsiter*/
  CMU->CTRL  = 0x0000;
  
  /*Deinitializes Interrupt Status register*/
  CMU->IS    = 0x0000;
  
  /*Deinitializes Interrupt Mask register*/
  CMU->IM    = 0x0000;
  
  /*Disable access to all CMU register*/
  CMU->WE    = 0x0000;
}

/******************************************************************************
* Function Name  : CMU_StructInit
* Description    : Fills in a CMU_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : CMU_InitStruct : pointer to a CMU_InitTypeDef structure
*                  which will be initialized.
* Output         : None                        
* Return         : None
******************************************************************************/
void CMU_StructInit(CMU_InitTypeDef* CMU_InitStruct)
{
/* Reset CMU init structure parameters values */

  /*Initializes RC oscillator control bits*/
  CMU_InitStruct->CMU_RCOscControl = 0x0008;

  /*Initializes end of count value*/
  CMU_InitStruct->CMU_EndCountValue = 0x0000;

  /*Initializes Frequency reference high bits*/
  CMU_InitStruct->CMU_FreqRef_High = 0x0FFF;

  /*Initializes Frequency reference low bits*/
  CMU_InitStruct->CMU_FreqRef_Low = 0x0000;

  /*Initializes the clock source for CKOUT */
  CMU_InitStruct->CMU_CKSEL0 = CMU_CKSEL0_CKRC;

  /*Initializes the clock source for Frequency Meter*/
  CMU_InitStruct->CMU_CKSEL1 = CMU_CKSEL1_CKOSC;

  /*Initializes the clock source for driving the CMU logic*/
  CMU_InitStruct->CMU_CKSEL2 = CMU_CKSEL2_CKRC;
}

/******************************************************************************
* Function Name  : CMU_Init
* Description    : Initializes the CMU  peripheral according to the specified
*                  parameters in the CMU_InitTypeDef structure.
* Input          : CMU_InitStruct: pointer to a CMU_InitTypeDef structure that
*                  contains the configuration information for the CMU peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void CMU_Init(CMU_InitTypeDef* CMU_InitStruct)
{
  /* enable write access to all CMU registers */
  CMU_Lock(DISABLE);

  /*Initialize register*/
  CMU->RCCTL = CMU_RCCTL_MASK;
  CMU->FRH   = CMU_FRH_MASK;
  CMU->FRL   = CMU_FRL_MASK;
  CMU->CTRL &= CMU_CTRL_MASK;

  /*Initializes RC oscillator control bits*/
  CMU->RCCTL = CMU_InitStruct->CMU_RCOscControl;

  /*Configure end of count value*/
  CMU->EOCV = CMU_InitStruct->CMU_EndCountValue;

  /*Configure Frequency reference high bits*/
  CMU->FRH = CMU_InitStruct->CMU_FreqRef_High;

  /*configure Frequency reference low bits*/
  CMU->FRL = CMU_InitStruct->CMU_FreqRef_Low;
 
  /*Select the clock source for CKOUT = Main Oscillator or Backup oscillator*/
  CMU->CTRL |= CMU_InitStruct->CMU_CKSEL0 ;
  
  /*Select the clock source for Frequency Meter = PLL output from PRCCU or Main Oscillator */
  CMU->CTRL |= CMU_InitStruct->CMU_CKSEL1;

  /*Select the clock source for driving the CMU logic = Main Oscillator or Backup oscillator*/
  CMU->CTRL |= CMU_InitStruct->CMU_CKSEL2;

  /* disable write access to all CMU registers */
  CMU_Lock(ENABLE);

}

/*******************************************************************************
* Function Name  : CMU_GetOSCFrequency
* Description    : Gets the current value of frequency measure register
* Input          : None
* Output         : None
* Return         : Value of FDISP register
*******************************************************************************/
u16 CMU_GetOSCFrequency(void)
{
  /*Start a clock frequency measurement*/
  CMU->CTRL |= CMU_StartFreqMeas;  
  
  /*Wait until the measure is ready in FDISP register*/
  while((CMU->CTRL & CMU_SFMbit)!=0x000);
  
  /*Return Measured frequency bits*/
  return(CMU->FDISP);
}

/*******************************************************************************
* Function Name  : CMU_Lock
* Description    : Enables or disables access to all CMU register.
* Input          : NewState: new state of the CMU peripheral. This parameter
*                  can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void CMU_Lock(FunctionalState NewState)
{
  if(NewState == DISABLE)
  {
    /*Enable access to all CMU register.*/
    CMU->WE = CMU_KeyValue1;
    CMU->WE = CMU_KeyValue2;
  }
  else
  {
    /*Disable access to all CMU register.*/
    CMU->WE = 0x0000;
  }
}

/*******************************************************************************
* Function Name  : CMU_ModeOscConfig
* Description    : Configures the CMU Mode frequency.
* Input          : CMU_OSCMode: Choose the mode of oscillator.
*                  This member can be one of the following values:
*                      - CMU_Stop_Low: CMU RC oscillateur Frequency in Stop mode 
*                                      is low.
*                      - CMU_Stop_High: CMU RC oscillateur Frequency in Stop mode
*                                       is high.
*                      - CMU_Run_High: CMU RC oscillateur Frequency in Run mode
*                                      is high .
*		       - CMU_Run_Low: CMU RC oscillateur Frequency in Run mode
*                                     is low.
* Output         : None
* Return         : None
*******************************************************************************/
void CMU_ModeOscConfig(u16 CMU_OSCMode)
{
  switch(CMU_OSCMode)
  {
    case CMU_Stop_Low: 
      CMU->CTRL |= CMU_Stop_Low; 
    break;
    
    case CMU_Stop_High: 
      CMU->CTRL &= CMU_Stop_High; 
    break;

    case CMU_Run_Low: 
      CMU->CTRL |= CMU_Run_Low; 
    break;
    
    case CMU_Run_High: 
      CMU->CTRL &= CMU_Run_High; 
    break; 
  }       
} 

/*******************************************************************************
* Function Name  : CMU_StopOscConfig
* Description    : Stops the specific source of clock.
* Input          : CMU_StopOsc : Source of clock will be stoped. This parameter 
*                  can be any combination of the following values:
*                       - CMU_RCoscSoftStop : Stop RC Oscillator in Run mode.
*                       - CMU_RCoscHardStop : Stop RC Oscillator when Stop mode occurs
*                       - CMU_MainoscStop : Stop Quartz Oscillator.                               
* Output         : None
* Return         : None
*******************************************************************************/
void CMU_StopOscConfig(u16 CMU_StopOsc )
{
  CMU->CTRL |= CMU_StopOsc;
}

/*******************************************************************************
* Function Name  : CMU_ResetConfig
* Description    : Enables or disables Reset generation.
* Input          : NewState: new state of the CMU peripheral. This parameter
*                  can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void CMU_ResetConfig(FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    /* Enable Reset generation by setting the REN bit on the CTRL register*/
    CMU->CTRL |= CMU_Reset_Enable;
  }
  else
  {
    /* Disable Reset generation by clearing the REN bit on the CTRL register*/
    CMU->CTRL &= CMU_Reset_Disable;
  }
}

/*******************************************************************************
* Function Name  : CMU_ITConfig
* Description    : Enables or disables the specified CMU interrupts.
* Input          : - CMU_IT: specifies the CMU interrupts sources to be enabled
*                    or disabled. This parameter can be any combination of the
*                    following values:
*                        - CMU_IT_OSCLessRC      : Oscillator frequency Less than
*                                                  RC frequency interrupt Mask bit
*                        - CMU_IT_EndCounter     : End of Counter interrupt Mask bit
*                        - CMU_IT_FreqLessLowRef : Clock Frequency Less than Low 
*                                                 reference interrupt Mask bit
*                        - CMU_IT_Reset          : Reset ON Interrupt Mask bit
*                  - NewState: new state of the specified CMU interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void CMU_ITConfig(u8 CMU_IT, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    CMU->IM |= CMU_IT;
  }
  else
  { 
    CMU->IM &= ~CMU_IT;
  }
}
/*******************************************************************************
* Function Name  : CMU_FlagStatus
* Description    : Checks whether the specified CMU flag is set or not.
* Input          : - CMU_Flag: flag to check. This parameter can be one of the
*                    following values:
*                          - CMU_FLAG_CKON0bit : It indicates which clock drives CKOUT.
*                          - CMU_FLAG_CKON1bit : It indicates which clock drives the Frequency Meter.
*                          - CMU_FLAG_CKON2bit : It indicates which clock drives fCMU.
*                          - CMU_FLAG_RONbit   : CMU Reset condition ON status.
*                          - CMU_FLAG_CRFbit   : CMU Reset Flag bit.
* Output         : None
* Return         : The new state of CMU_Flag (SET or RESET).
*******************************************************************************/
FlagStatus CMU_FlagStatus(u8 CMU_Flag)
{ 
  /* Checks whether CMU_Flag is set or not */
  if((CMU->STAT & CMU_Flag) != RESET)
  {
     return SET;
  }
  else
  {
     return RESET;
  }
}

/*******************************************************************************
* Function Name  : CMU_ITClear
* Description    : Clears the specified interrupt.
* Input          : - ITClear: specifies the CMU interrupts sources to be cleared
*                     This parameter can be any combination of the following values:
*                          - CMU_IT_OSCLessRC      : Oscillator frequency Less than
                                                     RC frequency interrupt
*                          - CMU_IT_EndCounter     : End of Counter interrupt 
*                          - CMU_IT_FreqLessLowRef : Clock Frequency Less than Low
                                                     reference interrupt
*                          - CMU_IT_Reset          : Reset ON Interrupt
* Output         : None
* Return         : None
*******************************************************************************/
void CMU_ITClear(u16 CMU_ITClear)
{
  CMU->IS &= ~CMU_ITClear;
}

/*******************************************************************************
* Function Name  : CMU_FlagClear
* Description    : Clears the CRF bit in stat register.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CMU_FlagClear(void)
{ 
  /*Clear CRF bit in STAT register by setting CRFR bit in CTRL register*/
  CMU->CTRL |= CMU_ClearCRFbit;
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/









