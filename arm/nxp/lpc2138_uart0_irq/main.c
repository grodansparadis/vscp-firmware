/******************************************************************************
 *
 * A simple program which sends a greeting to UART0, then echos
 * characters on UART0 and blinks an LED every 1/2 second.
 * from: Bill Knight, R O SoftWare <BillK@rosw.com>
 * ----------------------------------------------------------------------------
 *
 * - Adapted to the Olimex LPC-P2106 demo-board (Philips LPC2106).
 * - Sends message if button/switch on demo-board is hit.
 * - Slightly modified and extended as WinARM demo-application.
 * by Martin THOMAS <eversmith@heizung-thomas.de>
 *
 * $RCSfile: main.c,v $
 * $Revision: 1.2 $
 *
 *****************************************************************************/
#include "types.h"
#include "lpc213x.h"
#include "config.h"
#include "armVIC.h"
#include "sysTime.h"
#include "uart.h"
#include "config.h"
#include "eeprom.h"

#define HOST_BAUD           (57600)

/******************************************************************************
 *
 * Function Name: lowInit()
 *
 * Description:
 *    This function starts up the PLL then sets up the GPIO pins before
 *    waiting for the PLL to lock.  It finally engages the PLL and
 *    returns
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
static void lowInit(void)
{
  // set PLL multiplier & divisor.
  // values computed from config.h
  PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;

  // enable PLL
  PLLCON = PLLCON_PLLE;
  PLLFEED = 0xAA;                       // Make it happen.  These two updates
  PLLFEED = 0x55;                       // MUST occur in sequence.

  // setup the parallel port pin
  IOCLR1 = PIO0_ZERO_BITS;                // clear the ZEROs output
  IOSET1 = PIO0_ONE_BITS;                 // set the ONEs output
  IODIR1 = PIO0_OUTPUT_BITS;              // set the output bit direction

  // wait for PLL lock
  while (!(PLLSTAT & PLLSTAT_LOCK))
    continue;

  // enable & connect PLL
  PLLCON = PLLCON_PLLE | PLLCON_PLLC;
  PLLFEED = 0xAA;                       // Make it happen.  These two updates
  PLLFEED = 0x55;                       // MUST occur in sequence.

  // setup & enable the MAM
  MAMTIM = MAMTIM_CYCLES;
  MAMCR = MAMCR_FULL;

  // set the peripheral bus speed
  // value computed from config.h
  VPBDIV = VPBDIV_VALUE;                // set the peripheral bus clock speed
}

/******************************************************************************
 *
 * Function Name: sysInit()
 *
 * Description:
 *    This function is responsible for initializing the program
 *    specific hardware
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
static void sysInit(void)
{
  lowInit();                            // setup clocks and processor port pins

  // set the interrupt controller defaults
#if defined(RAM_RUN)
  MEMMAP = MEMMAP_SRAM;                 // map interrupt vectors space into SRAM
#elif defined(ROM_RUN)
  MEMMAP = MEMMAP_FLASH;                // map interrupt vectors space into FLASH
#else
#error RUN_MODE not defined!
#endif
	VICIntEnClr = 0xFFFFFFFF;           	// clear all interrupts
	VICIntSelect = 0x00000000;           	// clear all FIQ selections
	VICDefVectAddr = (uint32_t)reset;   	// point unvectored IRQs to reset()

//  wdtInit();                         	// initialize the watchdog timer
	initSysTime();                       	// initialize the system timer
	uart0Init( UART_BAUD( HOST_BAUD ), UART_8N1, UART_FIFO_8 ); // setup the UART
  
  // Initiate I2C
	i2cInit();	
	
  // initialize the watchdog timer	
/*	VICIntSelect &= ~VIC_BIT (VIC_WDT );	// WDT selected as IRQ
	VICIntEnable = VIC_BIT( VIC_WDT );   	// WDT interrupt enabled
	VICVectCntl8 = VIC_ENABLE | VIC_WDT;
	VICVectAddr8 = (uint32_t)wdtISR;    	// address of the ISR
*/  
	WDTC = 60000000;						// One second = 15000000
	WDTV = 0;
	WDMOD = 3;  				// Activate watchdog
	WDFEED = 0xAA; WDFEED = 0x55;
}

/******************************************************************************
 *
 * Function Name: button_state()
 *
 * Description:
 *    This function checks if a key has been pressed. Assumes
 *    key to be "active low" (PIN-Bit==0 -> pressed). Does
 *    debouncing for given debounce time-difference
 *
 * Calling Sequence: 
 *    GPIO-Initialisation for Inputs
 *
 * Returns:
 *    -1 : key changed or bouncing
 *     0 : key released
 *     1 : key pressed
 *
 *****************************************************************************/
#define KEY_DEBOUNCE FIFTY_MS
static int button_state(void)
{
	static uint32_t lastchangetime;
	static boolean  laststate=FALSE;
	boolean actstate;
	
	actstate = (IOPIN1 & PIO0_BTN_STATE0_BIT) ? FALSE : TRUE; // TRUE if pressed (active low)
	
	if (laststate != actstate) {
		lastchangetime = getSysTICs();
		laststate = actstate;
	}
	else {
		if (getElapsedSysTICs(lastchangetime) > KEY_DEBOUNCE) {
			if (actstate) return 1;
			else return 0;
		}
	}
	return -1; // changed or bouncing
}

/******************************************************************************
 *
 * Function Name: main()
 *
 * Description:
 *    This function is the program entry point.  After initializing the
 *    system, it sends a greeting out UART0 then enters an endless loop
 *    echoing chracters on the UART and blinking an LED every half
 *    second.
 *
 * Calling Sequence: 
 *    void
 *
 * Returns:
 *    void
 *
 *****************************************************************************/
int main(void)
{
	unsigned _cpsr;
	uint32_t startTime;
	boolean lock=FALSE;
  
	sysInit();
#if defined(UART0_TX_INT_MODE) || defined(UART0_RX_INT_MODE)
	enableIRQ();
#endif
	startTime = getSysTICs();
  
	uart0Puts("\r\nHello World!\r\n");
	uart0Puts("(a WinARM Demo-Application based on code from R O Software)\r\n\r\n");

	if ( -1 == readEEPROM( 0 ) ) {
		uart0Puts("EEPROM ERROR1\r\n");
	}
	if ( -1 == readEEPROM( 1 ) ) {
		uart0Puts("EEPROM ERROR2\r\n");
	}
	
	for (;;) {
	
		do {
			int ch;

			if ( (ch = uart0Getch() ) >= 0 ) {
				uart0Puts("the <");
				uart0Putch(ch);
				uart0Puts("> key has been pressed\r\n");
			}
	
			// send button-pressed string only once if hit
			if ( button_state() == 0) lock=FALSE; // release lock if button is released
			if ( ( button_state() == 1 ) && !lock ) {
				uart0Puts("\r\nButton Pressed!\r\n");
				lock=TRUE;
			}
		} 
		while ( getElapsedSysTICs(startTime) < HALF_SEC );

		if ( IOPIN1 & PIO0_LCD_RS_BIT ) {
			IOCLR1 = PIO0_LCD_RS_BIT;
		}
		else {
			IOSET1 = PIO0_LCD_RS_BIT; 
		}
		
		readEEPROM( 1 );
		readEEPROM( 1000 );

		startTime += HALF_SEC;
		_cpsr = disableIRQ();
		WDFEED = 0xAA; WDFEED = 0x55;
		restoreIRQ( _cpsr );
 
   }

  return 0;
  
}
