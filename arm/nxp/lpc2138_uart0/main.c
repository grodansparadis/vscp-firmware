/******************************************************************************
 *****************************************************************************/

#include "lpc213x.h"
#include "config.h"
#include "uart.h"
#include "armVic.h"

#define BAUD 57600

// Keil MCB2130: one led on P1.16 (active high)
#define LEDPIN  16

///////////////////////////////////////////////////////////////////////////////
// wdtISR
//

void wdtISR( void )
{
	ISR_ENTRY();
/*	
	if ( !( IOPIN1 & PIO1_FAN_BIT ) ) {
		IOSET1 = PIO1_FAN_BIT;
	}
	else {
		IOCLR1 = PIO1_FAN_BIT;
	}
	
	IOSET1 = PIO1_EHEATER_ELEMENTB_BIT;
*/	
	//WDMOD &= ~BIT(WDTOF);
	VICVectAddr = 0x00000000;	// Clear interrupt in 

	ISR_EXIT();					// recover registers and return
}

void systemInit(void)
{
	// --- enable and connect the PLL (Phase Locked Loop) ---
	// a. set multiplier and divider
	PLLCFG = MSEL | (1<<PSEL1) | (0<<PSEL0);
	// b. enable PLL
	PLLCON = (1<<PLLE);
	// c. feed sequence
	PLLFEED = PLL_FEED1;
	PLLFEED = PLL_FEED2;
	
	// d. wait for PLL lock (PLOCK bit is set if locked)
	while ( !( PLLSTAT & (1<<PLOCK) ) );
	
	// e. connect (and enable) PLL
	PLLCON = (1<<PLLE) | (1<<PLLC);
	
	// f. feed sequence
	PLLFEED = PLL_FEED1;
	PLLFEED = PLL_FEED2;
	
	// --- setup and enable the MAM (Memory Accelerator Module) ---
	// a. start change by turning of the MAM (redundant)
	MAMCR = 0;	
	// b. set MAM-Fetch cycle to 3 cclk as recommended for >40MHz
	MAMTIM = MAM_FETCH;
	// c. enable MAM 
	MAMCR = MAM_MODE;
	
	// --- set VPB speed ---
	VPBDIV = VPBDIV_VAL;
	
	// --- map INT-vector ---
	#if defined(RAM_RUN)
	  MEMMAP = MEMMAP_USER_RAM_MODE;
	#elif defined(ROM_RUN)
	  MEMMAP = MEMMAP_USER_FLASH_MODE;
	#else
	#error RUN_MODE not defined!
	#endif
}

void gpioInit(void)
{
	IODIR1 |= (1<<LEDPIN);		// define LED-Pin as output
	IOCLR1  = (1<<LEDPIN);		// set Bit = LED off (active high)
}

void ledToggle(void)
{
	static unsigned char state=0;
	
	state = !state;
	if (state) IOSET1 = (1<<LEDPIN);	// set Bit = LED on
	else IOCLR1 = (1<<LEDPIN);			// set Bit = LED off (active low)
}

int main(void)
{
	int ch;
	
	systemInit();
	gpioInit();
	
	// initialize the watchdog timer	
/*	VICIntSelect &= ~VIC_BIT (VIC_WDT );	// WDT selected as IRQ
	VICIntEnable = VIC_BIT( VIC_WDT );   	// WDT interrupt enabled
	VICVectCntl8 = VIC_ENABLE | VIC_WDT;
	VICVectAddr8 = (uint32_t)wdtISR;    	// address of the ISR
*/  
	WDTC = 150000000;						// One second = 15000000
	WDTV = 0;
	WDMOD = 3;  				// Activate watchdog
	WDFEED = 0xAA; WDFEED = 0x55;
	
	uart0Init( UART_BAUD( BAUD ), UART_8N1, UART_FIFO_8 ); // setup the UART
	
	uart0Putch('M');
	
	uart0Puts("\r\nHello from WinARM!\r\n");
	uart0Puts("Demo based on code from R O Software\r\n");
	
	ledToggle();
	
	while( 1 ) {
	
		if ( ( ch = uart0Getch() ) >= 0 ) {
			uart0Puts("You pressed : ");
			uart0Putch(ch);
			uart0Puts("\r\n");
			ledToggle();
		}
		
		//WDFEED = 0xAA; WDFEED = 0x55;
		
	}
	
	return 0;
}
