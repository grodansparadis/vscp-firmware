/***********************************************************************************
*	STR71x Example Application 
*	by Martin Thomas, Kaiserlautern, Germany
*	http://www.siwawi.arubi.uni-kl.de/avr_projects/arm_projects
*
*	Target: STR710 on STR710 eval-board (MB393B)
*	        using the internal flash memory (verify boot-jumpers)
*	Compiler/Toolchain: GNU with gcc 4.1.1 (WinARM 6/06)
*
*	Partly based on code from:
*	- Anglia Design (www.anglia-designs.com, Author: Spencer Oliver)
*	- STMicroelectronics (www.st.com)
*	- Yuri Tiomkin (www.tnkernel.org)
*
*	This demo code and associated components are provided as is and has no warranty,
*	implied or otherwise. You are free to use/modify any of the provided
*	code at your own risk in your applications with the expressed limitation
*	of liability (see below)
* 
*	LIMITATION OF LIABILITY: THE AUTHORS OF THIS EXAMPLE SHALL NOT BE LIABLE FOR ANY
*	LOSS OF PROFITS, LOSS OF USE, LOSS OF DATA, INTERRUPTION OF BUSINESS, NOR FOR
*	INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES OF ANY KIND WHETHER UNDER
*	THIS AGREEMENT OR OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
*
***********************************************************************************/

// just don't define if your port does not have a text-mode LCD
// (memory-mapped)
//#define LCD_ENABLED

#include <stdio.h>
#include "71x_lib.h"
#include "lcd.h"
#include "vectors.h"

#define UART0_Rx_Pin (0x0001<<8)   /*  TQFP 64: pin N° 63 , TQFP 144 pin N° 143 */
#define UART0_Tx_Pin (0x0001<<9)   /*  TQFP 64: pin N° 64 , TQFP 144 pin N° 144 */

u8 characT[]={0x0E, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00};
u8 characM[]={0x1B, 0x15, 0x11, 0x00, 0x00, 0x00, 0x00};
char szBuffer[80];

static void delay(void)
{
	volatile int i, j;
	j = 0;
	
	while(j<3) {
		for(i=0; i<0x1FFFF;i++);
		j++;
	}
}

// gets called every millisecond
static void systick_callback(void)
{
	static int cnt = 0;
	
	if ( cnt++ >= 1000 ) {
		// Blink LED at 1 Hz
		GPIO_BitWrite( GPIO0, 2, !GPIO_BitRead( GPIO0, 2 ));
		cnt = 0;
	}
}

// init a 1ms system-tick using timer 3
static void systick_init(void)
{
	//  Set the Timer3 prescaler and enabled output compare A interrupt (OCAIE set)
	TIM3->CR2  = TIM_OCAIE_mask | TIMER3_PRESCALER;  
	// Timer3 - enable in Output Compare mode (EN set, OCAE set)
	TIM3->CR1  = TIM_EN_Mask | TIM_OCAE_Mask;
	// Timer3 compare-match-value
	TIM3->OCAR = TIMER3_LOAD_VAL;
	// Reset the counter & run it
	TIM3->CNTR = 0xFFFC;
	
	Timer3_set_callback( systick_callback );
	
	EIC_IRQChannelConfig( T3TIMI_IRQChannel, ENABLE );
	EIC_IRQChannelPriorityConfig( T3TIMI_IRQChannel, 1 );

}

static void my_UART_StringSend(UART_TypeDef *UARTx, char *String)
{
	u8 *Data = (u8*)String;
	
	while( *Data != '\0' ) {
		UART_ByteSend( UARTx, Data++ );
	}
}

int main (void)
{
	#ifdef DEBUG
		libdebug();
	#endif
#if 0
	// all clocks at 8MHz (with 16MHz XTAL) - no longer used:
	RCCU_Div2Config( ENABLE );                    // DIV2
	RCCU_RCLKSourceConfig( RCCU_CLOCK2 );         // see startup.S
#else
	// use the PLL and configure the various clocks:
	/* RCCU peripheral configuration ------------------------------------------*/
	RCCU_Div2Config( ENABLE );                    // DIV2
	// RCCU_Div2Config ( DISABLE );
	/*  Configure FCLK = RCLK /2 */
	RCCU_FCLKConfig ( RCCU_RCLK_2 );
	
	/*  Configure PCLK = RCLK /4 */
	RCCU_PCLKConfig ( RCCU_RCLK_4 );
	
	/*  Configure MCLK clock for the CPU, RCCU_DEFAULT = RCLK /1 */
	RCCU_MCLKConfig ( RCCU_DEFAULT );
	
	/*  Configure the PLL1 ( * 12 , / 2 ) */
	RCCU_PLL1Config ( RCCU_PLL1_Mul_12, RCCU_Div_2 ) ;
	
	/*  Wait PLL to lock */
	while( RCCU_FlagStatus( RCCU_PLL1_LOCK ) == RESET ) {
		;
	}
	
	/*  Select PLL1_Output as RCLK clock */
	RCCU_RCLKSourceConfig (RCCU_PLL1_Output) ;
#endif
	
	// P0.3 as output PP
	GPIO_Config( GPIO0, (1<<3), GPIO_OUT_PP );
	
	// P0.0, P0.1 and P0.2 outputs
	GPIO_Config( GPIO0, (1<<0)|(1<<1)|(1<<2), GPIO_OUT_PP );
	
	/* UART peripheral configuration -------------------------------------------*/

	/* Configure the UART0 pins */
	GPIO_Config(GPIO0, UART0_Tx_Pin, GPIO_AF_PP);
	GPIO_Config(GPIO0, UART0_Rx_Pin, GPIO_IN_TRI_CMOS);

	/*  Configure the UART X */
	UART_OnOffConfig( UART0, ENABLE );       /*  Turn UART0 on */
	UART_FifoConfig ( UART0, DISABLE );      /*  Disable FIFOs */
	UART_FifoReset  ( UART0 , UART_RxFIFO ); /*  Reset the UART_RxFIFO */
	UART_FifoReset  ( UART0 , UART_TxFIFO ); /*  Reset the UART_TxFIFO */
	UART_LoopBackConfig(UART0 , DISABLE );   /*  Disable Loop Back */
	/* Configure the UART0 as following:
	- Baudrate = 9600 Bps
	- No parity
	- 8 data bits
	- 1 stop bit */
	UART_Config( UART0, 9600, UART_NO_PARITY, UART_1_StopBits ,UARTM_8D );
	UART_RxConfig( UART0, ENABLE );          /*  Enable Rx */
	
	/* EIC peripheral configuration --------------------------------------------*/
	
	/* Configure the EIC channel interrupt for the UART0 */
	EIC_IRQChannelPriorityConfig( UART0_IRQChannel, 1 );
	EIC_IRQChannelConfig( UART0_IRQChannel, ENABLE );

	UART_ItConfig( UART0,UART_RxBufFull, ENABLE );

	systick_init();
	
	EIC_IRQConfig( ENABLE );

#ifdef LCD_ENABLED
	/* LCD-Init */
	LCD_Init();
	LCD_SetCursorHome();
	LCD_UnderlineCursorOn();
	LCD_BlockCursorOn();
	LCD_SetCursorPosition(8,2);
	LCD_CursorLeft();
	LCD_SetCursorHome();
	LCD_CursorRight();
	LCD_BlockCursorOff();
	LCD_CreateCharacter(0x1, characT);
	LCD_CreateCharacter(0x2, characM);
	
	// print "Welcome" to LCD
	LCD_String("STR710 DemoBoard", 1, 1);
	// siprintf(szBuffer, " ARM7%c%c Powered ", 1, 2);
	siprintf(szBuffer, "ARM7%c%c1-M.Thomas", 2, 1);
	LCD_String(szBuffer, 1, 2);
#endif
	
	// printf "Welcome" to UART0 and give frequency information:
	my_UART_StringSend( UART0, "\r\n\r\nSTR710 example by Martin Thomas\r\n" );
	my_UART_StringSend( UART0, "partly based on code from Anglia Design and STMicro.\r\n\r\n" );
	
	my_UART_StringSend( UART0, "Frequency-Settings:\r\n");
	siprintf(szBuffer, "Ext CK         : % 9lu Hz (CK)\r\n", RCCU_Main_Osc );
	my_UART_StringSend( UART0, szBuffer );
	siprintf(szBuffer, "RCCU_CLK2      : % 9lu Hz (PLL ref. input)\r\n", RCCU_FrequencyValue( RCCU_CLK2 ) );
	my_UART_StringSend( UART0, szBuffer );
	siprintf(szBuffer, "RCCU_RCLK      : % 9lu Hz (RCCU output)\r\n", RCCU_FrequencyValue( RCCU_RCLK ) );
	my_UART_StringSend( UART0, szBuffer );
	siprintf(szBuffer, "RCCU_MCLK      : % 9lu Hz (ARM and memory)\r\n", RCCU_FrequencyValue( RCCU_MCLK ) );
	my_UART_StringSend( UART0, szBuffer );
	siprintf(szBuffer, "RCCU_PCLK      : % 9lu Hz (peripheral ABP)\r\n", RCCU_FrequencyValue( RCCU_PCLK ) );
	my_UART_StringSend( UART0, szBuffer );
	siprintf(szBuffer, "RCCU_FCLK      : % 9lu Hz (fast peripherals)\r\n", RCCU_FrequencyValue( RCCU_FCLK ) );
	my_UART_StringSend( UART0, szBuffer );
	siprintf(szBuffer, "RCCU_RTC_CLOCK : % 9lu Hz\r\n", RCCU_FrequencyValue( RCCU_RTC_CLOCK ) );
	my_UART_StringSend( UART0, szBuffer );

	
	while(1) {
	
		// test Button NXT: P1.9 SEL: P1.8
		GPIO_BitWrite( GPIO0, 0, !GPIO_BitRead( GPIO1, 8 ) );
		GPIO_BitWrite( GPIO0, 1, !GPIO_BitRead( GPIO1, 9 ) );
		
		// toggle output
		GPIO_BitWrite( GPIO0, 3, !GPIO_BitRead( GPIO0, 3 ));
		siprintf(szBuffer, "Loop T3-CNT %d ", TIM_CounterValue(TIM3));
		// my_UART_StringSend( UART0, szBuffer );
		delay();
		
	}
}
