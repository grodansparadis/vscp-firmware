/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : 73x_gpio.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : This file provides all the GPIO software functions.
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
#include "73x_gpio.h"
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
/*****************************************************************************
* Function Name  : GPIO_DeInit                                               
* Description    : Deinitializes the GPIOx peripheral registers to their default
*                  reset values.      
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.           
* Output         : None                                                      
* Return         : None                                                      
******************************************************************************/
void GPIO_DeInit (GPIO_TypeDef* GPIOx)
{
  /* All GPIO Port pin is configured in Input Tristate TTL mode */
  
  if( GPIOx == GPIO0)
  {
   CFG_PeripheralClockConfig(CFG_CLK_GPIO0,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_GPIO0,ENABLE);
  }
  else if  ( GPIOx == GPIO1)
  {
  
   CFG_PeripheralClockConfig(CFG_CLK_GPIO1,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_GPIO1,ENABLE);
  }
  else if  ( GPIOx == GPIO2)
  {
   CFG_PeripheralClockConfig(CFG_CLK_GPIO2,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_GPIO2,ENABLE);
  }
  else if  ( GPIOx == GPIO3)
  {
   CFG_PeripheralClockConfig(CFG_CLK_GPIO3,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_GPIO3,ENABLE);
  }
  else if  ( GPIOx == GPIO4)
  {
   CFG_PeripheralClockConfig(CFG_CLK_GPIO4,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_GPIO4,ENABLE);
  }
  else if  ( GPIOx == GPIO5)
  {
   CFG_PeripheralClockConfig(CFG_CLK_GPIO5,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_GPIO5,ENABLE);
  }
  else if  ( GPIOx == GPIO6)
  {
   CFG_PeripheralClockConfig(CFG_CLK_GPIO6,DISABLE);
   CFG_PeripheralClockConfig(CFG_CLK_GPIO6,ENABLE);
  }
}

/******************************************************************************
* Function Name  : GPIO_Init                                                 
* Description    : Initializes the GPIOx  peripheral according to the specified
*                  parameters in the GPIO_InitTypeDef structure.
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.
*                  - GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
*                    contains the configuration information for the specified GPIO
*                    peripheral.              
* Output         : None                                                      
* Return         : None                                                      
******************************************************************************/
void GPIO_Init (GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct)
{
  /* GPIOx Mode and Pins Set */
  switch (GPIO_InitStruct->GPIO_Mode)
  {
    case GPIO_Mode_HI_AIN_TRI:
      GPIOx->PC0&=~GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC1&=~GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC2&=~GPIO_InitStruct->GPIO_Pins;
      break;

    case GPIO_Mode_IN_TRI_TTL:
      GPIOx->PC0|=GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC1&=~GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC2&=~GPIO_InitStruct->GPIO_Pins;
      break;

    case GPIO_Mode_INOUT_WP:
      GPIOx->PC0|=GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC1|=GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC2&=~GPIO_InitStruct->GPIO_Pins;
      break;

    case GPIO_Mode_OUT_OD:
      GPIOx->PC0&=~GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC1&=~GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC2|=GPIO_InitStruct->GPIO_Pins;
      break;

    case GPIO_Mode_OUT_PP:
      GPIOx->PC0|=GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC1&=~GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC2|=GPIO_InitStruct->GPIO_Pins;
      break;

    case GPIO_Mode_AF_OD:
      GPIOx->PC0&=~GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC1|=GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC2|=GPIO_InitStruct->GPIO_Pins;
      break;

    case GPIO_Mode_AF_PP:
      GPIOx->PC0|=GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC1|=GPIO_InitStruct->GPIO_Pins;
      GPIOx->PC2|=GPIO_InitStruct->GPIO_Pins;
      break;
  }
}

/******************************************************************************
* Function Name  : GPIO_StructInit                                          
* Description    : Initialize the GPIO Init Structure parameters                    
* Input          : - GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure
*                    which will be initialized.            
* Return         : None.                                                    
******************************************************************************/
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct)
{
  /* Reset GPIO init structure parameters values */
  GPIO_InitStruct->GPIO_Pins = GPIO_PIN_ALL;
  GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN_TRI_TTL;
}

/*****************************************************************************
* Function Name  : GPIO_BitWrite                                             
* Description    : Set or reset the selected GPIO port pin                   
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.           
*                  - GPIO_Pin: the Pin number. This parameter can be GPIO_PIN_x 
*                    where x can be (0..15) 
*                  - Bit_Val: A BitAction enumuration value:
*                         - Bit_SET: set the selected port pin                           
*                         - Bit_RESET: reset the selected port pin                           
* Output         : None                                                      
* Return         : None                                                      
******************************************************************************/
void GPIO_BitWrite(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, BitAction Bit_Val)
{
  if ((Bit_Val & Bit_SET) != 0 )
  {
    GPIOx->PD |= GPIO_Pin;
  }
  else
  {
    GPIOx->PD &= ~GPIO_Pin;
  }
}

/*******************************************************************************
* Function Name  : GPIO_BitRead					       
* Description    : Read the desired port pin value                            
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.          
*                : - GPIO_Pin: the Pin number. This parameter can be GPIO_PIN_x 
*                    where x can be (0..15)                                      
* Output         : None                                                       
* Return         : The selected pin value of the selected GPIO port           
*******************************************************************************/
u8 GPIO_BitRead(GPIO_TypeDef* GPIOx, u16 GPIO_Pin)
{
  if ((GPIOx->PD & GPIO_Pin) != 0 )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/******************************************************************************
* Function Name  : GPIO_ByteWrite                                            
* Description    : Write byte value to the selected GPIO port                
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.           
*                  - GPIO_Byte: High or low byte
*                    This parameter can be: GPIO_MSB or GPIO_LSB                          
*                  - Byte_Val: Byte value                                     
* Output         : None                                                      
* Return         : None                                                      
******************************************************************************/
void GPIO_ByteWrite(GPIO_TypeDef* GPIOx, u8 GPIO_Byte, u8 Byte_Val)
{
  if (GPIO_Byte == GPIO_MSB)
  {
    GPIOx->PD = (GPIOx->PD&0x00FF) | ((u16)Byte_Val<<8);
  }
  else
  {
    GPIOx->PD = (GPIOx->PD&0xFF00) | Byte_Val;
  }
}

/*******************************************************************************
* Function Name  : GPIO_ByteRead                                              
* Description    : Read the desired port Byte value                           
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.           
*                : - GPIO_Byte: High or low byte
*                    This parameter can be: GPIO_MSB or GPIO_LSB                           
* Output         : None                                                       
* Return         : The GPIO MSB or LSB Byte value of the selected GPIO port   
*******************************************************************************/
u8 GPIO_ByteRead(GPIO_TypeDef* GPIOx, u8 GPIO_Byte)
{
  return (u8)(GPIOx->PD >> GPIO_Byte);
}

/*******************************************************************************
* Function Name  : GPIO_WordWrite                                            
* Description    : Write word value to the selected GPIO port                 
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.           
*                : - Port_Val: Word value to output                                      
* Output         : None                                                      
* Return         : None                                                       
*******************************************************************************/
void GPIO_WordWrite(GPIO_TypeDef* GPIOx, u16 Port_Val)
{
  GPIOx->PD = Port_Val;
}

/*******************************************************************************
* Function Name  : GPIO_WordRead                                              
* Description    : Read the desired port word value                           
* Input          : - GPIOx: where x can be (0..6) to select the GPIO peripheral.          
* Output         : None                                                       
* Return         : The GPIO Word value of the selected GPIO port              
*******************************************************************************/
u16 GPIO_WordRead(GPIO_TypeDef* GPIOx)
{
  return GPIOx->PD;
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/
