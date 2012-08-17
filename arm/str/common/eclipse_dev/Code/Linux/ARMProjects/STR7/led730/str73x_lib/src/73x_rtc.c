/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_rtc.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the RTC software functions.
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

/* Standard includes -------------------------------------------------------- */
#include "73x_rtc.h"
#include "73x_cfg.h"

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
* Function Name  : RTC_DeInit                                               
* Description    : Deinitializes the RTC peripheral registers to their default
*                  reset values.
* Input          : None  
* Output         : None                                                    
* Return         : None                                                     
******************************************************************************/
void RTC_DeInit (void)
{
   CFG_PeripheralClockConfig(CFG_CLK_RTC,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_RTC,ENABLE);
 }

/******************************************************************************
* Function Name  : RTC _Init                                                
* Description    : Initializes the RTC  peripheral according to the specified
*                  parameters in the RTC_InitTypeDef structure.
* Input          : RTC_InitStruct: pointer to an RTC_InitTypeDef structure that
                   contains the configuration information for the RTC peripheral.             
* Output         : None                                                      
* Return         : None                                                      
******************************************************************************/
void RTC_Init (RTC_InitTypeDef* RTC_InitStruct)
{
  /* Reset RTC Registers */
  RTC->CRL  = 0x20;
  RTC->PRLH = 0x0;
  RTC->PRLL = 0x8000;
  RTC->DIVH = 0x0;
  RTC->DIVL = 0x8000;
  RTC->CNTH = 0x0;
  RTC->CNTL = 0x0;
  RTC->ALRH = 0xFFFF;
  RTC->ALRL = 0xFFFF;

  /* Alarm config  */
  /* Loop until the Last operation Completion */
  while (!(RTC->CRL & RTC_RTOFF_Mask));
  /* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= RTC_CNF_Mask;
  /* Set the ALARM MSB word */
  RTC->ALRH = (RTC_InitStruct->RTC_Alarm & 0xFFFF0000) >> 16;
  /* Set the ALARM LSB word */
  RTC->ALRL = (RTC_InitStruct->RTC_Alarm & 0x0000FFFF);
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= ~RTC_CNF_Mask;
  /* Loop until the Last operation Completion */
  while (!(RTC->CRL & RTC_RTOFF_Mask));

  /* Prescaler config */
  if ( RTC_GetPrescalerValue() != RTC_InitStruct->RTC_Prescaler )
  {
    /* Loop until the Last operation Completion */
    while (!(RTC->CRL & RTC_RTOFF_Mask));
    /* Set the CNF flag to enter in the Configuration Mode */
    RTC->CRL |= RTC_CNF_Mask;
    /* Set RTC PRESCALER MSB word */
    RTC->PRLH = (RTC_InitStruct->RTC_Prescaler & 0xFFFF0000) >> 16;
    /* Set RTC PRESCALER LSB word */
    RTC->PRLL = (RTC_InitStruct->RTC_Prescaler & 0x0000FFFF);
    /* Reset the CNF flag to exit from the Configuration Mode */
    RTC->CRL &= ~RTC_CNF_Mask;
  }

  /* Counter Config */
  /* Loop until the Last operation Completion */
  while (!(RTC->CRL & RTC_RTOFF_Mask));
  /* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= RTC_CNF_Mask;
  /* SET RTC COUNTER MSB word */
  RTC->CNTH =(RTC_InitStruct->RTC_Counter & 0xFFFF0000) >> 16;
  /* SET RTC COUNTER LSB word */
  RTC->CNTL =(RTC_InitStruct->RTC_Counter & 0x0000FFFF);
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= ~RTC_CNF_Mask;
  /* Loop until the Last operation Completion */
  while (!(RTC->CRL & RTC_RTOFF_Mask));
}

/******************************************************************************
* Function Name  : RTC_StructInit				                              
* Description    : Fills in a RTC_InitTypeDef structure with the reset value of
*                  each parameter.                   
* Input          : RTC_InitStruct: pointer to an RTC_InitTypeDef structure
                   which will be initialized.  
* Output         : None              
* Return         : None.						                              
******************************************************************************/
void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct)
{
  /* Reset RTC init structure parameters values */
  RTC_InitStruct->RTC_Prescaler = 0x8000 ;
  RTC_InitStruct->RTC_Counter = 0x0;
  RTC_InitStruct->RTC_Alarm = 0xFFFFFFFF;
}

/******************************************************************************
* Function Name  : RTC_SetCounter                                           
* Description    : Sets the new RTC counter value.        
* Input          : RTC_Counter: new Counter Value 
* Output         : None                              
* Return         : None                                                     
*******************************************************************************/
void RTC_SetCounter(u32 RTC_Counter)
{
  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();
  /* Enter In Configuration Mode */
  RTC_EnterCfgMode();
  /* SET RTC COUNTER MSB word */
  RTC->CNTH =(RTC_Counter & 0xFFFF0000) >> 16;
  /* SET RTC COUNTER LSB word */
  RTC->CNTL =(RTC_Counter & 0x0000FFFF);
  /* Exit From Configuration Mode */
  RTC_ExitCfgMode ();
  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : RTC_GetCounterValue
* Description    : Gets the RTC Counter value.
* Input          : None
* Output         : None
* Return         : The Counter value
*******************************************************************************/
u32 RTC_GetCounterValue(void)
{
  u16 tmp = RTC->CNTL;

  return ( (u32)RTC->CNTH << 16 ) |tmp ;
}

/******************************************************************************
* Function Name  : RTC_SetPrescaler                                          
* Description    : Set the new Prescaler Value           
* Input          : RTC_prescaler: new Prescaler Value 
* Output         : None                             
* Return         : None                                                      
*******************************************************************************/
void RTC_SetPrescaler(u32 RTC_prescaler)
{
  if(RTC_GetPrescalerValue () != RTC_prescaler)
  {
    /* Wait For Last Task Completion */
    RTC_WaitForLastTask ();
    /* Enter In Configuration Mode */
    RTC_EnterCfgMode ();
    /* Set RTC PRESCALER MSB word */
    RTC->PRLH = (RTC_prescaler & 0x000F0000) >> 16;
    /* Set RTC PRESCALER LSB word */
    RTC->PRLL = (RTC_prescaler & 0x0000FFFF);
    /* Exit From Configuration Mode*/
    RTC_ExitCfgMode ();
    /* Wait For Last Task Completion */
    RTC_WaitForLastTask();
  }
}

/*******************************************************************************
* Function Name  : RTC_GetPrescalerValue
* Description    : Gets the RTC Prescaler value.
* Input          : None
* Output         : None
* Return         : The Prescaler value
*******************************************************************************/
u32 RTC_GetPrescalerValue (void)
{
  u16 tmp = RTC->PRLL;
  return ( (u32)(RTC->PRLH & 0x000F) << 16 ) | tmp ;
}

/******************************************************************************
* Function Name  : RTC_SetAlarm                                              
* Description    : Sets the new RTC alarm Value           
* Input          : RTC_alarm: new Alarm Value  
* Output         : None                                
* Return         : None                                                      
*******************************************************************************/
void RTC_SetAlarm(u32 RTC_alarm)
{
  /* Wait For Last Task Completion */
  RTC_WaitForLastTask ();
  /* Enter In Configuration Mode */
  RTC_EnterCfgMode ();
  /* Set the ALARM MSB word */
  RTC->ALRH = (RTC_alarm & 0xFFFF0000) >> 16;
  /* Set the ALARM LSB word */
  RTC->ALRL = (RTC_alarm & 0x0000FFFF);
  /* Exit From Configuration Mode */
  RTC_ExitCfgMode ();
  /* Wait For Last Task Completion */
  RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : RTC_GetAlarmValue
* Description    : Gets the RTC alarm Value.
* Input          : None
* Output         : None
* Return         : The Alarm value
*******************************************************************************/
u32 RTC_GetAlarmValue(void)
{
   u16 tmp = RTC->ALRL ;
   return ( (u32)RTC->ALRH << 16 ) | tmp ;
}

/*******************************************************************************
* Function Name  : RTC_FlagClear
* Description    : Clears the RTC flags passed as parameters.
* Input          : RTC_Flag: flag to clear. This parameter can be any 
*                  combination of the following values:
*                       - RTC_FLAG_GL:  Global flag
*                       - RTC_FLAG_OV:  Overflow flag
*                       - RTC_FLAG_ALA: Alarm flag
*                       - RTC_FLAG_SEC: Second flag
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_FlagClear(u16 RTC_Flag)
{
  /* Clear the coressponding RTC flag */
  RTC->CRL &= ~RTC_Flag;
}

/*******************************************************************************
* Function Name  : RTC_FlagStatus
* Description    : Checks whether the specified RTC flag is set or not.
* Input          : RTC_Flag: flag to check. This parameter can be one of the
*                  following values:
*                       - RTC_FLAG_GL:  Global flag
*                       - RTC_FLAG_OV:  Overflow flag
*                       - RTC_FLAG_ALA: Alarm flag
*                       - RTC_FLAG_SEC: Second flag
* Output         : None
* Return         : The new state of the RTC_Flag (SET or RESET).
*******************************************************************************/
FlagStatus RTC_FlagStatus(u16 RTC_Flag)
{
  if((RTC->CRL & RTC_Flag) != RESET )
  {
    return SET;
  }
  else
  {
    return RESET;
  }
}

/*******************************************************************************
* Function Name  : RTC_ITConfig
* Description    : This routine is used to configure the RTC interrupts
* Input          : - RTC_IT: interrupt to enable or disable. This parameter can
*                    be any combination of the following values:
*                         - RTC_IT_GL:  Global interrupt
*                         - RTC_IT_OV:  Overflow interrupt
*                         - RTC_IT_ALA: Alarm interrupt
*                         - RTC_IT_SEC: Second interrupt
*                  - NewState: new state of the specified RTC interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void RTC_ITConfig(u16 RTC_IT, FunctionalState NewState)
{
  if(NewState == ENABLE)
  {
    RTC->CRH |= RTC_IT;
  }
  else
  {
    RTC->CRH &= ~RTC_IT;
  }
}

/*******************************************************************************
* Function Name  : RTC_ITStatus
* Description    : Gets the RTC interrupts status
* Input          : - RTC_IT: interrupt to check. This parameter can
*                    be one of the following values:
*                         - RTC_IT_GL:  Global interrupt
*                         - RTC_IT_OV:  Overflow interrupt
*                         - RTC_IT_ALA: Alarm interrupt
*                         - RTC_IT_SEC: Second interrupt
* Output         : None
* Return         : The State of the RTC_IT (ENABLE or DISABLE)
*******************************************************************************/
FunctionalState RTC_ITStatus(u16 RTC_IT)
{
  if(RTC->CRH & RTC_IT)
  {
    return ENABLE;
  }
  else
  {
    return DISABLE;
  }
}

/******************************************************************************
* Function Name  : RTC_EnterCfgMode                                          
* Description    : Enters in the Configuration Mode.   
* Input          : None
* Output         : None                                                       
* Return         : None                                                      
*******************************************************************************/
void RTC_EnterCfgMode(void)
{
  /* Set the CNF flag to enter in the Configuration Mode */
  RTC->CRL |= RTC_CNF_Mask;
}

/******************************************************************************
* Function Name  : RTC_ExitCfgMode                                           
* Description    : Exits from the Configuration Mode.
* Input          : None 
* Output         : None                                                      
* Return         : None                                                      
*******************************************************************************/
void RTC_ExitCfgMode(void)
{
  /* Reset the CNF flag to exit from the Configuration Mode */
  RTC->CRL &= ~RTC_CNF_Mask;
}

/******************************************************************************
* Function Name  : RTC_WaitForLastTask                                       
* Description    : Waits for the last task completion.           
* Input          : None  
* Output         : None                                                     
* Return         : None                                                      
*******************************************************************************/
void RTC_WaitForLastTask(void)
{
  /* Loop until the Last operation Completion */
  while (!(RTC->CRL & RTC_RTOFF_Mask));
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
