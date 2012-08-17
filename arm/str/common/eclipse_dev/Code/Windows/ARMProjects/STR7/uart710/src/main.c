/******************** (C) COPYRIGHT 2003 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Date First Issued  : 22/07/2003
* Description        : This program demonstrates how to use the UART with the
*                      STR71x software library.
********************************************************************************
* History:
*  22/07/2003 : Created
*  12/07/2004 : New GPIO pins configuration, Rx Always enabled
*******************************************************************************/

#include "71x_lib.h"

#define UART0_Rx_Pin (0x0001<<8)   // TQFP 64: pin N° 63 , TQFP 144 pin N° 143
#define UART0_Tx_Pin (0x0001<<9)   // TQFP 64: pin N° 64 , TQFP 144 pin N° 144

#define UART1_Rx_Pin (0x0001<<10)  // TQFP 64: pin N° 1  , TQFP 144 pin N° 1
#define UART1_Tx_Pin (0x0001<<11)  // TQFP 64: pin N° 2  , TQFP 144 pin N° 3

#define UART2_Rx_Pin (0x0001<<13)  // TQFP 64: pin N° 5  , TQFP 144 pin N° 9
#define UART2_Tx_Pin (0x0001<<14)  // TQFP 64: pin N° 6  , TQFP 144 pin N° 10

#define UART3_Rx_Pin (0x0001<<1)   // TQFP 64: pin N° 52 , TQFP 144 pin N° 123
#define UART3_Tx_Pin (0x0001<<0)   // TQFP 64: pin N° 53 , TQFP 144 pin N° 124

#define Use_UART0
//#define Use_UART1
//#define Use_UART2
//#define Use_UART3

   #ifdef Use_UART0
     #define  UARTX  UART0
     #define  UARTX_Rx_Pin  UART0_Rx_Pin
     #define  UARTX_Tx_Pin  UART0_Tx_Pin
     #define  UARTX_Periph  UART0_Periph
   #endif /* Use_UART0 */

   #ifdef Use_UART1
     #define  UARTX  UART1
     #define  UARTX_Rx_Pin  UART1_Rx_Pin
     #define  UARTX_Tx_Pin  UART1_Tx_Pin
     #define  UARTX_Periph  UART1_Periph
   #endif /* Use_UART1 */

   #ifdef Use_UART2
     #define  UARTX  UART2
     #define  UARTX_Rx_Pin  UART2_Rx_Pin
     #define  UARTX_Tx_Pin  UART2_Tx_Pin
     #define  UARTX_Periph  UART2_Periph
   #endif /* Use_UART2 */

   #ifdef Use_UART3
     #define  UARTX  UART3
     #define  UARTX_Rx_Pin  UART3_Rx_Pin
     #define  UARTX_Tx_Pin  UART3_Tx_Pin
     #define  UARTX_Periph  UART3_Periph
   #endif /* Use_UART3 */


u16 i;
u16 UARTStatus;
u8 bBuffer[4]={'S','T','R','7'};

static void delay(void)
{
	volatile int i, j;
	j = 0;
	
	while(j<3) {
		for(i=0; i<0x1FFFF;i++);
		j++;
	}
}

int main(void)
{
  #ifdef DEBUG
    libdebug();
  #endif
  // Configure the GPIO pins
  GPIO_Config(GPIO0, UARTX_Tx_Pin, GPIO_AF_PP);
  GPIO_Config(GPIO0, UARTX_Rx_Pin, GPIO_IN_TRI_CMOS);

  // Configure the UART X
  UART_OnOffConfig(UARTX, ENABLE);       // Turn UARTX on
  UART_FifoConfig (UARTX, DISABLE);      // Disable FIFOs
  UART_FifoReset  (UARTX , UART_RxFIFO); // Reset the UART_RxFIFO
  UART_FifoReset  (UARTX , UART_TxFIFO); // Reset the UART_TxFIFO
  UART_LoopBackConfig(UARTX , DISABLE);  // Disable Loop Back
                                         /* Configure the UARTX as following:
                                            - Baudrate = 9600 Bps
                                            - No parity
                                            - 8 data bits
                                            - 1 stop bit */
  UART_Config(UARTX,9600,UART_NO_PARITY,UART_1_StopBits,UARTM_8D);
  UART_RxConfig(UARTX ,ENABLE);          // Enable Rx

  while(1)
  {
      UART_StringSend(UARTX, "Hello world\r\n");
	  delay();
   }
}

/******************* (C) COPYRIGHT 2003 STMicroelectronics *****END OF FILE****/
