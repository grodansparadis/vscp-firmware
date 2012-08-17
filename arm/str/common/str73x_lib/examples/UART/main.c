/******************** (C) COPYRIGHT 2005 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 09/27/2005 :  V1.0
* Description        : Main program body
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
#include "73x_lib.h"
/* Include of other module interface headers -----------------------------------*/
/* Local includes --------------------------------------------------------------*/
/* Private typedef -------------------------------------------------------------*/
/* Private define --------------------------------------------------------------*/
#define UART0_Rx_Pin 0x0100   
#define UART0_Tx_Pin 0x0200   

/* Private macro ---------------------------------------------------------------*/
/* Private variables -----------------------------------------------------------*/ 
UART_InitTypeDef UART_InitStruct;
CMU_InitTypeDef CMU_InitStruct;
GPIO_InitTypeDef GPIO_InitStruct;
u16 Delay;


int main(void)
{
  #ifdef DEBUG
  debug();
  #endif

/*Enable Clock for UART0*/
  CFG_PeripheralClockConfig(CFG_CLK_UART0 , ENABLE);
/*Enable Clock for GPIO6*/
  CFG_PeripheralClockConfig(CFG_CLK_GPIO6 , ENABLE);

/*Configure CMU and switch to the external 8MHz Quartz*/
  CMU_InitStruct.CMU_CKSEL0 = CMU_CKSEL0_CKOSC;
  CMU_Init (&CMU_InitStruct);

/* Configure the GPIO pins as alternate function push pull*/
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStruct.GPIO_Pins = UART0_Tx_Pin ;
  GPIO_Init (GPIO6, &GPIO_InitStruct);

/* UART Configuration */
  UART_StructInit(&UART_InitStruct);             /*Initialize Structure with default values*/
  UART_InitStruct.UART_BaudRate  = 9600;         /* Baud rate value */ 
  UART_InitStruct.UART_Mode = UART_Mode_8D_P ;   /*Set the UARTx Mode  8 bit Data + Parity */
  UART_InitStruct.UART_Loop_Standard = UART_Standard; /*Disable LoopBack*/
  UART_InitStruct.UART_StopBits = UART_StopBits_1; /* STOP bits number */
  UART_Cmd(UART0, ENABLE);
  UART_Init(UART0, &UART_InitStruct);   /*Configure Uart0*/
  while(1)
  {
    
    UART_ByteSend(UART0, 'A');   /*Send "A" character*/
   }
}

/******************* (C) COPYRIGHT 2005 STMicroelectronics *****END OF FILE****/

