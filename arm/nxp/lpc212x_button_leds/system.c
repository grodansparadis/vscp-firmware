//system.c
#include <lpc21xx.h>
#include "config.h"
#include "system.h"

//#define VIC_TIMER0_bit (1 << VIC_TIMER0)

//oscillator frequency
//IMPORTANT - if you use oscillator with different frequency,
//please change this value, becàuse timer not work correctly
//#define OSCILLATOR_CLOCK_FREQUENCY  14745600      //in MHz

/*
unsigned int GetCclk(void)
{
  //return real processor clock speed
  return OSCILLATOR_CLOCK_FREQUENCY * (PLLCON & 1 ? (PLLCFG & 0xF) + 1 : 1);
}

unsigned int GetPclk(void)
{
  //VPBDIV - determines the relationship between the processor clock (cclk)
  //and the clock used by peripheral devices (pclk).
  unsigned int divider;
  switch (VPBDIV & 3)
    {
      case 0: divider = 4;  break;
      case 1: divider = 1;  break;
      case 2: divider = 2;  break;
    }
  return GetCclk() / divider;
}


void FrecInit(void)
{
  //devide or multiplier
  //here is calculate frecuence
  PLLCFG_bit.MSEL = 0x2;  //M - multiplier
  PLLCFG_bit.PSEL = 0x1;  //P - devider
  //set changes (require from architecture)
  PLLFEED_bit.FEED = 0xAA;
  PLLFEED_bit.FEED = 0x55;


  //enable or connect PLL
  //enable PLL
  PLLCON_bit.PLLE = 1;
  //set changes (require from architecture)
  PLLFEED_bit.FEED = 0xAA;
  PLLFEED_bit.FEED = 0x55;

  //wait for PLOK (correct freq)
  while(PLLSTAT_bit.PLOCK == 0);

  //connect PLL
  PLLCON_bit.PLLC = 1;
  //set changes (require from architecture)
  PLLFEED_bit.FEED = 0xAA;
  PLLFEED_bit.FEED = 0x55;
}
*/


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
void lowInit( void )
{
	// set PLL multiplier & divisor.
	// values computed from config.h
	PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;

	// enable PLL
	PLLCON = PLLCON_PLLE;
	PLLFEED = 0xAA;      		// Make it happen.  These two updates
	PLLFEED = 0x55;             // MUST occur in sequence.
	
	// setup the PIO0 port pin
	IOCLR0 = PIO0_ZERO_BITS;                // clear the ZEROs output
	IOSET0 = PIO0_ONE_BITS;                 // set the ONEs output
	IODIR0 = PIO0_OUTPUT_BITS;              // set the output bit direction

	// wait for PLL lock
	while ( !(PLLSTAT & PLLSTAT_LOCK ) ) {
		continue;
	}

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
void sysInit( void )
{
	lowInit();                            	// setup clocks and processor port pins

	// set the interrupt controller defaults
#if defined(RAM_RUN)
	MEMMAP = MEMMAP_SRAM;                 	// map interrupt vectors space into SRAM
#elif defined(ROM_RUN)
	MEMMAP = MEMMAP_FLASH;                	// map interrupt vectors space into FLASH
#else
#error RUN_MODE not defined!
#endif
	VICIntEnClr = 0xFFFFFFFF;  				// clear all interrupts
	VICIntSelect = 0x00000000;           	// clear all FIQ selections
	VICDefVectAddr = (uint32_t)reset;   	// point unvectored IRQs to reset

	//WDTC = 15000000;						// One second = 15000000
	//WDTV = 0;
	//WDMOD = WDEN | WDRESET;  				// Activate watchdog
	//WDFEED = 0xAA; WDFEED = 0x55;
	
}





