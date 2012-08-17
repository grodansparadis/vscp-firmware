/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_prccu.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0 
* Description        : This file provides all the PRCCU software functions.
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
#include "73x_prccu.h"
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
* Function Name  : PRCCU_Init
* Description    : Initializes the PRCCU  peripheral according to the specified
*                  parameters in the PRCCU_InitTypeDef structure.
* Input          : PRCCU_InitStruct: pointer to a PRCCU_InitTypeDef structure that
*                  contains the configuration information for the PRCCU peripheral.
* Output         : None
* Return         : None
******************************************************************************/
void PRCCU_Init(PRCCU_InitTypeDef* PRCCU_InitStruct)
{
  u32 Tmp_CLOCK2=0, CLOCK1=0;
 
  PRCCU->CFR |= PRCCU_CFR_Mask;      /* set DIV2 and CK2_16 bits to reset value */
  PRCCU->CFR &= PRCCU_CFR_Mask1;     /* set CSU_CKSEL bit to reset value */
  PRCCU->PLLCR = PRCCU_PLLCR_Mask;   /* set FREEN, FREF_RANGE and MX[1:0] bits to their reset values */ 
                                     /* reset DX[2:0] value to 0x0 */
  
  /* DIV2 part of the PLL-----------------------------------------------------*/
  /* Enable or disable the DIV2 of the Clock unit */
  if(PRCCU_InitStruct->PRCCU_DIV2 == ENABLE)
  {
    /* Enable DIV2 by setting DIV2 bit in PRCCU_CFR register */
    PRCCU->CFR |= PRCCU_DIV2_Enable;
  }
  else
  {
    /* Disable DIV2 by resetting DIV2 bit in PRCCU_CFR register */
    PRCCU->CFR &= PRCCU_DIV2_Disable;
  }

  /* MCLK source Clock selection part ----------------------------------------*/
  switch(PRCCU_InitStruct->PRCCU_MCLKSRC_SRC)
  {
    /* select the CLOCK2 as MCLK Source*/
    case PRCCU_MCLKSRC_CLOCK2 :
    {
      /* Disable PLL by resetting CSU_CKSEL bit in PRCCU_CFR register */
      PRCCU->CFR &= PRCCU_CSU_CKSEL_Disable;
      /* Disable CK2_16 by setting CK2_16 bit in PRCCU_CFR register */
      PRCCU->CFR |= PRCCU_CK2_16_Disable;
      /* switch off the PLL*/
      PRCCU->PLLCR = PRCCU_PLL_SwitchOff;
      break;
    }

    /* select the CLOCK2/16 as MCLK Source*/
    case PRCCU_MCLKSRC_CLOCK2_16 :
    {
      /* Enable CK2_16 by resetting CK2_16 bit in PRCCU_CFR register */
      PRCCU->CFR &= PRCCU_CK2_16_Enable;
      /*  Switch off the PLL */
      PRCCU->PLLCR = PRCCU_PLL_SwitchOff;
      break;
    }

    /* select the PLL output as MCLK Source*/
    case PRCCU_MCLKSRC_PLL :
    {
      if((CMU->CTRL & CMU_CKSEL0_CKOSC) == CMU_CKSEL0_CKOSC)
      {
        CLOCK1 = Main_Oscillator ;
      }
      else
      {
        CLOCK1 = RC_oscillator;
      }
      if((PRCCU->CFR & PRCCU_DIV2_Enable) != 0)
      {
        /* Set CLOCK2 value to half CLOCK1 */
        Tmp_CLOCK2 = CLOCK1 / 2;  
      }
      else /* DIV2_EN bit is reset */
      {
        /* Set CLOCK2 value equal to CLOCK1 */
        Tmp_CLOCK2 =  CLOCK1;
      }
      if(Tmp_CLOCK2 < 3000000)
      {
        /* PLL input in 1.5-3MHz */
        PRCCU->PLLCR &= PRCCU_FREF_RANGE_Low ;
      }
      else if(Tmp_CLOCK2 < 5000000)
      {
        /* PLL input in 3-5MHz  */
        PRCCU->PLLCR |= PRCCU_FREF_RANGE_High ;
      }

      /* configure the PLL Division Factors */
      PRCCU->PLLCR |= PRCCU_InitStruct->PRCCU_PLLDIV - 1;
      /* configure the PLL Multiplication Factor  */
      PRCCU->PLLCR |= PRCCU_InitStruct->PRCCU_PLLMUL;
      /* Enable PLL by setting CSU_CKSEL bit in PRCCU_CFR register */
      PRCCU->CFR |= PRCCU_CSU_CKSEL_Enable;
      /* Disable CK2_16 by setting CK2_16 bit in PRCCU_CFR register */
      PRCCU->CFR |= PRCCU_CK2_16_Disable;
    }
  }
  /* Free running mode for PLL -----------------------------------------------*/
  /* Enable or disable the Free running mode for PLL */
  if(PRCCU_InitStruct->PRCCU_FREEN == ENABLE)
  {
    /* Enable Free running mode for PLL by setting FREEN bit in PRCCU_PLLCR register */
    PRCCU->PLLCR |= PRCCU_FREEN_Enable;
    /* switch off the PLL*/
    PRCCU->PLLCR &= 0xFFFFFFF8;
    PRCCU->PLLCR |= PRCCU_PLL_SwitchOff;
  }
  else
  {
    /* Disable Free running mode for PLL by resetting FREEN bit in PRCCU_PLLCR register */
    PRCCU->PLLCR &= PRCCU_FREEN_Disable;
  }
}

/******************************************************************************
* Function Name  : PRCCU_DeInit
* Description    : Deinitializes the PRCCU peripheral registers to their default
                   reset values.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void PRCCU_DeInit(void)
{
  PRCCU->CCR = 0x0;
  PRCCU->VRCTR = 0x14;
  PRCCU->CFR  = 0x8008;
  PRCCU->PLLCR = 0x07;
  PRCCU->SMR = 0x01;
  PRCCU->RTCPR = 0x0F;
}

/******************************************************************************
* Function Name  : PRCCU_StructInit
* Description    : Fills in a PRCCU_InitTypeDef structure with the reset value of
*                  each parameter.
* Input          : PRCCU_InitStruct : pointer to a PRCCU_InitTypeDef structure
                   which will be initialized.
* Output         : None
* Return         : None
******************************************************************************/
void PRCCU_StructInit(PRCCU_InitTypeDef* PRCCU_InitStruct)
{
  PRCCU_InitStruct->PRCCU_DIV2 = ENABLE;
  PRCCU_InitStruct->PRCCU_MCLKSRC_SRC = PRCCU_MCLKSRC_CLOCK2;
  PRCCU_InitStruct->PRCCU_PLLDIV = 0x7;
  PRCCU_InitStruct->PRCCU_PLLMUL = 0x0;
  PRCCU_InitStruct->PRCCU_FREEN = DISABLE; 
}

/*******************************************************************************
* Function Name  : PRCCU_GetFrequencyValue
* Description    : Retrieves the value of the clock passed as parameter.
* Input          : PRCCU_CLOCK : the Clock to get its value .
*                  This parameter can be one of the following values:
*                   - PRCCU_CLOCK_EXT
*                   - PRCCU_CLOCK_MCLK
* Output         : None
* Return         : The value of the clock passed as parameter in Hz.
*******************************************************************************/
u32 PRCCU_GetFrequencyValue (PRCCU_OUTPUT PRCCU_CLOCK_Out)
{
  u8 MUL_Factor=1, DIV_Factor=1;
  u32 Tmp=0, Tmp_CLOCK2=0, CLOCK1=0;
  
  if((CMU->CTRL & CMU_CKSEL0_CKOSC) == CMU_CKSEL0_CKOSC)
  {
    CLOCK1 = Main_Oscillator ;
  }
  else
  {
    CLOCK1 = RC_oscillator;
  }
  
  /* Depending on the needed value*/
  if(PRCCU_CLOCK_Out == PRCCU_CLOCK_EXT)
  {
    /* Get the value of the RT Clock Frequency (fEXT)*/
    /* Check if the RT Clock Frequency (fEXT) is stopped or not */
    if (PRCCU->RTCPR <= 9)
    {
       /* Calculate the value of the RT Clock Frequency (fEXT) which depends on the RTCPR value  */
       return (Main_Oscillator /(2<<PRCCU->RTCPR));
    }
    else /* PRCCU->RTCPR > 9 */
    {
      /* RT Clock Frequency (fEXT) is stopped  */
       return (0);
    }
  }
  else   /* PRCCU_CLOCK_Out == PRCCU_CLOCK_MCLK */ 
  {
    /* Get the value of the fMCLK to CPU and peripherals */
    /* Depending on the status of the DIV2_EN bit get the CLOCK2 value  */
    if((PRCCU->CFR & PRCCU_DIV2_Enable) != 0)
    {
      Tmp_CLOCK2 = CLOCK1 / 2;  /* Set CLOCK2 value to Half CLOCK1 */
    }
    else /* DIV2_EN bit is reset */
    {
      /* Set CLOCK2 value equal to CLOCK1 */
      Tmp_CLOCK2 =  CLOCK1;
    }
    
    if((PRCCU->CFR & PRCCU_CK2_16_Disable) != 0)
    {
      if ((PRCCU->CFR & PRCCU_CSU_CKSEL_Enable) !=0 )
      {
        /* Get the PLL Multiplication and the Division Factor */
        Tmp = PRCCU->PLLCR;
        MUL_Factor =  ((Tmp & 0x30) >> 4);
        switch (MUL_Factor)
        {
          case 0: MUL_Factor = 20; break;
          case 1: MUL_Factor = 12; break;
          case 2: MUL_Factor = 28; break;
          case 3: MUL_Factor = 16; break;
        }
        
        DIV_Factor = (Tmp& 0x07) +1;
        
        /* return the value of the Main Clock MCLK*/
        return(Tmp_CLOCK2 * MUL_Factor /DIV_Factor) ;
      }
      else /* CSU_CKSEL bit Reset */
      {
        return Tmp_CLOCK2; /* Set the MCLK value to CLOCK2  */
      }
    }
    else /* CK2_16 bit reset */
    {
      /* Set the MCLK value to CLOCK2 / 16  */
      return (Tmp_CLOCK2 /16) ;
    }
  } /* end of if "PRCCU_CLOCK_Out"*/
}

/*******************************************************************************
* Function Name  : PRCCU_SetExtClkDiv
* Description    : Set EXTCLK Divider factor.
* Input          : ExtClkDiv : the value of the divider factor to set the Real
*                  Time Clock prescaling factors. This parameter can be one of 
*                  the following values: 2,4,8,16,32,64,128,256,512,1024.
* Output         : None.
* Return         : None.
*******************************************************************************/
void  PRCCU_SetExtClkDiv(u16 ExtClkDiv)
{
  u8 Rtcp = 0;

  /* Get the n Value of the ExtClkDiv = 2^n */
  while(ExtClkDiv != 2)
  {
    ExtClkDiv = ExtClkDiv >>1;
    Rtcp++ ;
  }
  /* Set the RTCPR[3:0] bits according to Rtpr value */
  PRCCU->RTCPR = Rtcp;
}

/*******************************************************************************
* Function Name  : PRCCU_EnterLPM
* Description    : Enter the selected Low Power Mode.
* Input          : PRCCU_LPM: low power mode to enter. This parameter can be one
*                   of the following values:
*                   - PRCCU_LPM_HALT
*                   - PRCCU_LPM_WFI
*                   - PRCCU_LPM_LPWFI
* Output         : None
* Return         : None
*******************************************************************************/
void PRCCU_EnterLPM (PRCCU_LPM NewLPM)
{
  switch ( NewLPM)
  {
    /* The sequence to enter HALT mode */
    case PRCCU_LPM_HALT :
    {
      /* Set the EN_HALT bit in the PRCCU_CCR register */
      PRCCU->CCR |= 0x800;
      /* Clear the SRESEN bit in the PRCCU_CCR register */
      PRCCU->CCR &= 0xFFF7;
      /* Set the HALT bit in the PRCCU_SMR register */
      PRCCU->SMR |= 0x2;
      break;
    }
    
    /* The sequence to enter Wait For Interrupt mode */
    case PRCCU_LPM_WFI :
    {
      /* Clear the WFI bit in the PRCCU_SMR register*/
      PRCCU->SMR &= 0x0 ;
      break;
    }

    /* The sequence to enter LP Wait For Interrupt mode */
    case PRCCU_LPM_LPWFI :
    {
      /* Set Low Power mode during Wait For Interrupt bit LPOWFI */
      PRCCU->CCR |= 0x01;
      /* Clear WFI_CKSEL bit in PRCCU_CCR register */
      PRCCU->CCR &= 0xFFFFFFFD;
      /* Clear the WFI bit in the PRCCU_SMR register*/
      PRCCU->SMR &= 0x0 ;
      break;
    }
  }
}

/*******************************************************************************
* Function Name  : PRCCU_ITConfig
* Description    : Enables or disables the specified PRCCU interrupts.
* Input          : PRCCU_IT: specifies the PRCCU interrupts sources to be enabled
*                  or disabled. This parameter can be any combination of the
*                  following values:
*                       - PRCCU_IT_LOCK
*                       - PRCCU_IT_STOP
*                       - PRCCU_IT_CLK2_16
*                  - NewState: new state of the specified PRCCU interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void PRCCU_ITConfig(u16 PRCCU_IT, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    /* Set the equivalent interrupt mask bit in the CCR resgister */
    PRCCU->CCR |= PRCCU_IT;
  }
  else
  {
    /* ReSet the equivalent interrupt mask bit in the CCR resgister */
    PRCCU->CCR &= ~PRCCU_IT;
  }
}

/*******************************************************************************
* Function Name  : PRCCU_FlagStatus
* Description    : Checks whether the specified PRCCU flag is set or not.
* Input          : PRCCU_Flag: flag to check. This parameter can be one of the
*                  following values:
*                       - PRCCU_FLAG_LOCK
*                       - PRCCU_FLAG_SOFTRES
*                       - PRCCU_FLAG_WDGRES
*                       - PRCCU_FLAG_LVD_INT
*                       - PRCCU_FLAG_LOCK_I
*                       - PRCCU_FLAG_CK2_16_I
*                       - PRCCU_FLAG_STOP_I
*                       - PRCCU_FLAG_VROK
* Output         : None
* Return         : The new state of the PRCCU_Flag (SET or RESET).
*******************************************************************************/
FlagStatus PRCCU_FlagStatus(u16 PRCCU_Flag)
{
  /* Checks whether PRCCU_Flag is set or reset */
  if (PRCCU_Flag == PRCCU_FLAG_VROK)
  {
    if ((PRCCU->VRCTR & PRCCU_Flag) != RESET)
    {
      return SET;
    }
      else
    {
      return RESET;
    }
  }
  else
  {
    if ((PRCCU->CFR & PRCCU_Flag) != RESET)
    {
      return SET;
    }
    else
    {
      return RESET;
    }
  }
}

/*******************************************************************************
* Function Name  : PRCCU_FlagClear
* Description    : Clears the specified PRCCU flag.
* Input          : PRCCU_Flag: flag to clear. This parameter can be one of the
*                  following values:
*                       - PRCCU_FLAG_LOCK_I
*                       - PRCCU_FLAG_CK2_16_I
*                       - PRCCU_FLAG_STOP_I
* Output         : None
* Return         : None
*******************************************************************************/
void PRCCU_FlagClear(u16 PRCCU_Flag)
{
  /* Clear PRCCU_Flag */
  PRCCU->CFR = (PRCCU->CFR & 0xFFFF816B) | PRCCU_Flag;
}

/*******************************************************************************
* Function Name  : PRCCU_SwResetGenerate
* Description    : Generate a software reset when enabling the Halt bit.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PRCCU_SwResetGenerate ( )
{
  /* Set the EN_HALT bit in the PRCCU_CCR register */
  PRCCU->CCR |= 0x808;

  /* Set the HALT bit in the PRCCU_SMR register */
  PRCCU->SMR |= 0x2;
}

/*******************************************************************************
* Function Name  : PRCCU_VRCmd
* Description    : Enable or Disable the main Voltage regulator in case of
*                  LPWFI low power mode and in the case of run mode.
* Input          : - V_Regulator : the voltage regulator to enable or to disable.
*                    this parameter can be:
*                       - PRCCU_VR_LPWFI : Main VR in LPWFI mode
*                       - PRCCU_VR_Run   : Main VR in Run mode
*                  - NewState: new state of the specified PRCCU interrupts.
*                    This parameter can be: ENABLE or DISABLE.
* Output         : None
* Return         : None
*******************************************************************************/
void PRCCU_VRCmd(u8 V_Regulator, FunctionalState NewState)
{
  if (NewState == ENABLE)
  {
    switch(V_Regulator)
    {
      case PRCCU_VR_LPWFI : PRCCU->VRCTR |= V_Regulator; break;
      case PRCCU_VR_Run   : PRCCU->VRCTR &= V_Regulator; break;
    }
  }
  else
  {
    switch(V_Regulator)
    {
      case PRCCU_VR_LPWFI : PRCCU->VRCTR &= ~V_Regulator; break;
      case PRCCU_VR_Run   : PRCCU->VRCTR |= ~V_Regulator; break;
    }
  }
}

/*******************************************************************************
* Function Name  : PRCCU_LPVRCurrentConfig
* Description    : select the Low-Power Voltage Regulator Current Capability
*                 (the allowed values are 2;4;6)
* Input          : - Current_Capability : the LPVR Output Current to configure
*                    it can be one of the following values:
*                       - PRCCU_LPVR_Current_2
*                       - PRCCU_LPVR_Current_4
*                       - PRCCU_LPVR_Current_6
* Output         : None
* Return         : None
*******************************************************************************/
void PRCCU_LPVRCurrentConfig (u8 Current_Capability)
{
  /* Set the Low Power Voltage Regulator Current Capability bit LPVRCC[1:0]
    depending on the selected LPVR Output Current (mA)*/
  switch(Current_Capability)
  {
    case PRCCU_LPVR_Current_2 :   CFG->R1 |=0x3        ;                 break;
    case PRCCU_LPVR_Current_4 :   CFG->R1 &=0xFFFFFFFC ; CFG->R1 |=0x2 ; break;
    case PRCCU_LPVR_Current_6 :   CFG->R1 &=0xFFFFFFFC ;                 break;
  }
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
