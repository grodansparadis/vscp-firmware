/******************************************************************************/
/*  lpc_blink_switch_irq                                                      */
/*  LED Flasher - Timer Interrupt example for arm-elf-gcc                     */
/*  Lights LED on Startup and flashs LED when button is pressed.              */
/******************************************************************************/
/*  Inspired by a sample application from Keil Elektronik                     */
/*  A lot of information has been found in a sample from R O Software.        */
/******************************************************************************/
/*  Sample for WinARM by M.Thomas <eversmith@heizung-thomas.de>               */
/******************************************************************************/
/******************************************************************************/
/*  Ported to keil, MCB2130 by Ake Hedman, akhe@eurosource.se                 */
/******************************************************************************/

#include "lpc213x.h"
#include "config.h"
#include "VIClowlevel.h"

#include "timer.h"

// Keil MCB2130 led on P0.16 (active high)
#define LEDPIN  17
#define LEDMASK (1<<LEDPIN);
// Keil MCB2130 INT1 switch on P0.14 (active low)
#define SWPIN 		14

#define WAIT100MS  10
#define WAIT1S     100

static void systemInit(void)
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
	while ( !( PLLSTAT & ( 1 << PLOCK ) ) );
	
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

static void gpioInit(void)
{
	IODIR1 |= 0xff0000;			// define LED-Pins as output
	IODIR0 &= ~(1<<SWPIN);		// define Switch-Pin as input	
	IOCLR1 = (1<<LEDPIN);		// clr Bit = LED off (active high)
}

static void wait (unsigned long delay) 
{
	unsigned long i;

	i = timeval + delay;
	while ( i != timeval);           
}

int main(void) 
{
	gpioInit();  
	
	IOSET1 = LEDMASK; 		// enable LED - "life sign"

	systemInit();			// PLL, MAM etc.
	
	init_timer();
	
	enableIRQ(); 
	
	wait(WAIT1S);
	IOCLR1 = LEDMASK; 		// disable LED
	wait(WAIT1S);

	while(1) {
		if ( IOPIN0 & (1<<SWPIN ) ) {	// true if button released (active low)
			IOSET1 = LEDMASK; 			// enable LED
		} 
		else {
		  IOCLR1 = LEDMASK; 	// disable LED
		  wait(WAIT100MS);
		  IOSET1 = LEDMASK; 	// enable LED
		  wait(WAIT100MS);
		}
	}
}
