/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_pwm.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the PWM software functions.
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
#include "73x_pwm.h"
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
* Function Name  : PWM_Init
* Description    : Initializes PWM  peripheral according to the specified
*                  parameters in the PWM_InitTypeDef structure.
* Input          : - PWMx: where x can be 0,..,5 to select the PWM peripheral.
*                  - PWM_InitStruct: pointer to a PWM_InitTypeDef structure that
                   contains the configuration information for the specified PWM
                   peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void PWM_Init( PWM_TypeDef *PWMx ,PWM_InitTypeDef  *PWM_InitStruct)
{
  PWMx->PRS0 = PWM_InitStruct->PWM_Prescaler0;
  PWMx->PRS1 = PWM_InitStruct->PWM_Prescaler1;
  PWMx->PER = PWM_InitStruct->PWM_Period;
  PWMx->DUT = PWM_InitStruct->PWM_DutyCycle;
  PWMx->PLS = (PWMx->PLS & PWM_POLARITY_Reset) | PWM_InitStruct->PWM_Polarity_Level;
}

/******************************************************************************
* Function Name  : PWM_DeInit
* Description    : Deinitializes PWM peripheral registers to their default reset
                   values.
* Input          : PWMx: where x can be 0,..,5 to select the PWM peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void PWM_DeInit(PWM_TypeDef *PWMx)
{  

 if( PWMx == PWM0)
  {
   CFG_PeripheralClockConfig(CFG_CLK_PWM0,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_PWM0,ENABLE);
  }
  else if  ( PWMx == PWM1)
  {
  
   CFG_PeripheralClockConfig(CFG_CLK_PWM1,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_PWM1,ENABLE);
  }
  else if  ( PWMx == PWM2)
  {
   CFG_PeripheralClockConfig(CFG_CLK_PWM2,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_PWM2,ENABLE);
  }
  else if  ( PWMx == PWM3)
  {
   CFG_PeripheralClockConfig(CFG_CLK_PWM3,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_PWM3,ENABLE);
  }
  else if  ( PWMx == PWM4)
  {
   CFG_PeripheralClockConfig(CFG_CLK_PWM4,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_PWM4,ENABLE);
  }
  else if  ( PWMx == PWM5)
  {
   CFG_PeripheralClockConfig(CFG_CLK_PWM5,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_PWM5,ENABLE);
  }

}

/******************************************************************************
* Function Name  : PWM_StructInit
* Description    : Fills in a PWM_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : PWM_InitStruct : pointer to a PWM_InitTypeDef structure which 
                   will be initialized.
* Output         : None                        
* Return         : None.
******************************************************************************/
void PWM_StructInit(PWM_InitTypeDef *PWM_InitStruct)
{
  PWM_InitStruct->PWM_Prescaler0 = 0x00;
  PWM_InitStruct->PWM_Prescaler1 = 0x00;
  PWM_InitStruct->PWM_DutyCycle = 0x0000;
  PWM_InitStruct->PWM_Period = 0x0000;
  PWM_InitStruct->PWM_Polarity_Level = 0x00;
}

/*******************************************************************************
* Function Name  : PWM_PolarityConfig
* Description    : Sets the polarity level
* Input          : - PWMx: where x can be 0,..,5 to select the PWM peripheral.
                   - PWM_Polarity_Mode: can be PWM_POLARITY_Low or 
                     PWM_POLARITY_High
* Output         : None                      
* Return         : None
*******************************************************************************/
void PWM_PolarityConfig(PWM_TypeDef *PWMx, u16 PWM_Polarity_Mode)
{
  if (PWM_Polarity_Mode == PWM_POLARITY_Low)
  {
    PWMx->PLS |= PWM_POLARITY_Set;
  }
  else
  {  
    PWMx->PLS &= PWM_POLARITY_Reset;
  }
}

/*******************************************************************************
* Function Name  : PWM_ITConfig
* Description    : Enables or disables the CP interrupt.
* Input         : - PWMx: where x can be 0,..,5 to select the PWM peripheral.
                  - NewState: new state of the CP interrupt that can be: ENABLE 
                    or DISABLE.
* Return         : None
*******************************************************************************/
void PWM_ITConfig (PWM_TypeDef *PWMx, FunctionalState Newstate)
 {
  if (Newstate == ENABLE)
  {
    PWMx->IM = PWM_ITCPI_Enable;
  }
  else
  { 
    PWMx->IM = PWM_ITCPI_Disable;
  }
}

/*******************************************************************************
* Function Name  : PWM_Cmd
* Description    : Enables or disables PWMx peripheral.
* Input          : - PWMx: where x can be 0,..,5 to select the PWM peripheral.
*                  - NewState: new state of the  PWMx peripheral (NewState can be
                   ENABLE or DISABLE)
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_Cmd(PWM_TypeDef* PWMx, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* Enable the selected PWM by setting the PE bit on the PEN register*/
    PWMx->PEN |= PWM_Enable_Mask;
  }
  else
  {
    /* Disable the selected PWM by clearing the PE bit on the PEN register*/
    PWMx->PEN &= PWM_Disable_Mask;
  }
}

/*******************************************************************************
* Function Name  : PWM_SetPeriod
* Description    : This routine is used to set the PWM period
* Input          : - PWMx: where x can be 0,..,5 to select the PWM peripheral.
*                  - PWM_Period: period value
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_SetPeriod(PWM_TypeDef *PWMx, u16 PWM_Period)
{
  PWMx->PER = PWM_Period;
}

/*******************************************************************************
* Function Name  : PWM_GetPeriodValue
* Description    : This routine is used to get the PWM period
* Input          : PWMx: where x can be 0,..,5 to select the PWM peripheral.
* Output         : None
* Return         : Period value
*******************************************************************************/
u16 PWM_GetPeriodValue(PWM_TypeDef *PWMx)
{
  return PWMx->PER;
}

/*******************************************************************************
* Function Name  : PWM_SetDutyCycle
* Description    : This routine is used to set the PWM duty cycle
* Input          : - PWMx: where x can be 0,..,5 to select the PWM peripheral.
*                : - PWM_Duty: duty cycle value
* Output         : None
* Return         : None
*******************************************************************************/
void PWM_SetDutyCycle(PWM_TypeDef *PWMx, u16 PWM_Duty)
{
  PWMx->DUT = PWM_Duty;
}

/*******************************************************************************
* Function Name  : PWM_GetDutyCycle
* Description    : This routine is used to get the PWM period
* Input          : PWMx: where x can be 0,..,5 to select the PWM peripheral.
* Output         : None
* Return         : Duty cycle value
*******************************************************************************/
u16 PWM_GetDutyCycleValue(PWM_TypeDef *PWMx)
{
 return PWMx->DUT;
}

/*******************************************************************************
* Function Name  : PWM_FlagStatus
* Description    : Checks whether the specified PWM flag is set or not.
* Input          : PWMx: where x can be 0,..,5 to select the PWM peripheral.               
* Output         : None
* Return         : the new state of the PWM_Flag (SET or RESET).
*******************************************************************************/
FlagStatus PWM_FlagStatus (PWM_TypeDef *PWMx)
{
  /* Checks whether PWM_CPFlag is set or not */ 
  if((PWMx->CPI & PWM_Enable_Mask) != RESET)
  {
   return SET;
  }
  else
  {
   return RESET;
  }
}

/*******************************************************************************
* Function Name  : PWM_FlagClear
* Description    : Clears the PWM Flag passed as a parameter
* Input          : PWMx: where x can be 0,..,5 to select the PWM peripheral.
* Output         : None
* Return         : None.
*******************************************************************************/
void PWM_FlagClear (PWM_TypeDef *PWMx)
{
  /* Clear CPI Flag */
  PWMx->CPI &= PWM_Disable_Mask ;
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

