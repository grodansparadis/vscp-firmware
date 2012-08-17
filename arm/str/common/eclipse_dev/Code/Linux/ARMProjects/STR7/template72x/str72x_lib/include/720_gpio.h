/******************** (C) COPYRIGHT 2004 STMicroelectronics ********************
* File Name          : 720_gpio.h
* Author             : MCD Application Team
* Date First Issued  : 08/02/2004
* Description        : This file provides all the GPIO software functions headers
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

/* Standard include ----------------------------------------------------------*/
/* Include of other module interface headers ---------------------------------*/
/* Local includes ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Interface functions -------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __720_GPIO_H
#define __720_GPIO_H
/* Includes ------------------------------------------------------------------*/
#include "720_lib.h"
/* Exported types ------------------------------------------------------------*/

typedef enum 
{
  IN_TRI_CMOS,
  INOUT_WP_CMOS,
  OUT_OD_CMOS,
  OUT_PP_CMOS,
  AF_OD_CMOS,
  AF_PP_CMOS
}GpioPinMode_TypeDef;
             
#define LSB  0x00
#define MSB  0x01

/*******************************************************************************
* Function Name  : GPIO_Config
* Description    : This routine is used to configure the selected GPIO ports 
		   pins according to the chosen mode

* Input1         : GPIOx: selects the port to be configured, x can be 2, 3 or 4
				   
* Input2         : Port_Pins: this parameter specifies the port pin number
				   
* Input3         : GPIO_Mode : this parameter specifies the pin mode
	           This must take one of the following values :
		     IN_TRI_CMOS     :  Standard CMOS Input
		     INOUT_WP_CMOS   :  Bidirectional Weak Push-Pull
		     OUT_OD_CMOS     :  Open Drain, Output
		     OUT_PP_CMOS     :  Push-Pull Output
		     AF_OD_CMOS      :  Open Drain Output Alternate-Function
		     AF_PP_CMOS      :  Push-Pull Output Alternate-Function

* Return         : None.
*******************************************************************************/
void GPIO_Config (GPIO_TypeDef *GPIOx,vu16 Port_Pins,GpioPinMode_TypeDef GPIO_Mode);

/*******************************************************************************
* Function Name  : GPIO_BitRead
* Description    : This routine is used to read the specified data port bit 
		   and return its value.

* Input1         : GPIOx: this parameter specifies the port.  x can be 2, 3 or 4

* Input2         : Port_Pin: this parameter specifies the bit to be read

* Return         : The port pin value.
*******************************************************************************/
u8 GPIO_BitRead (GPIO_TypeDef *GPIOx,vu8 Port_Pin);

/*******************************************************************************
* Function Name  : GPIO_ByteRead
* Description    : This routine is used to read the specified data port byte 
		   and return its value to user.

* Input1         : GPIOx: this parameter specifies the port.  x can be 2, 3 or 4.

* Input2         : Port_Byte: specifies which byte to be  read. 
		   It must be either MSB or LSB
		     MSB corresponds to the upper byte.
		     LSB corresponds to the lower byte.

* Return         : The specified port byte value.
*******************************************************************************/
u8 GPIO_ByteRead (GPIO_TypeDef *GPIOx,vu8 Port_Byte);

/*******************************************************************************
* Function Name  : GPIO_WordRead
* Description    : This routine is used to return the value of the specified
		   data port register GPIOx.PD.

* Input          : GPIOx: this parameter specifies the port. x can be 2, 3 or 4

* Return         : The specified port word value
*******************************************************************************/
u16 GPIO_WordRead(GPIO_TypeDef *GPIOx);

/*******************************************************************************
* Function Name  : GPIO_BitWrite
* Description    : This routine is used to set or clear the data 
	           port bit (*GPIOx).PD[Port_Pin]

* Input1         : GPIOx: this parameter specifies the port. x can be 2, 3 or 4.

* Input2         : Port_Pin: specifies the port pin number
		   Pin number values must be in the interval [0,15]
		     0 corresponds to the first pin
	            15 corresponds to the last pin

* Input3         : Port_Val: this parameter specifies the value to be written 
		   to the selected bit.
		   Port_Val must be one of the following values:
		     0: to clear the port pin
		     1: to set the port pin

* Return         : None.
*******************************************************************************/
void GPIO_BitWrite(GPIO_TypeDef *GPIOx,vu8 Port_Pin,vu8 Port_Val);

/*******************************************************************************
* Function Name  : GPIO_ByteWrite
* Description    : This routine is used to write the passed value in to 
		   the selected GPIOx.PD byte

* Input1         : GPIOx: this parameter specifies the port. x can be 2, 3 or 4.

* Input2         : Port_Byte: this parameter selects the byte to be written. 
		   Port_Byte must be either MSB or LSB.
		     MSB corresponds to the upper byte.
		     LSB corresponds to the lower byte.

* Input3         : Port_Val: The value to be written to the selected byte

* Return         : None.
*******************************************************************************/
void GPIO_ByteWrite(GPIO_TypeDef *GPIOx,vu8 Port_Byte,vu8 Port_Val);

/*******************************************************************************
* Function Name  : GPIO_WordWrite
* Description    : This routine is used to write the passed value in to 
		   the selected data port register GPIOx.PD

* Input1         : GPIOx: this parameter specifies the port. x can be 2, 3 or 4.
				   
* Input2         : Port_Val: The value to be written to the data port register.

* Return         : None.
*******************************************************************************/
void GPIO_WordWrite(GPIO_TypeDef *GPIOx,vu16 Port_Val);

#endif /* __GPIO_H */

/******************* (C) COPYRIGHT 2004 STMicroelectronics *****END OF FILE****/



