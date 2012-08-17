/******************************************************************************
 * ARM7 - LPC2000 DS18x20 interface
 * ----------------------------------------------------------------------------
 *
 * - Adapted to the Olimex LPC-P2106 demo-board (Philips LPC2106).
 * - Sends message if button/switch on demo-board is hit.
 * - Slightly modified and extended as WinARM demo-application.
 * by Martin THOMAS <eversmith@heizung-thomas.de>
 *
 * LPC-headers and UART from Bill Knight, R O SoftWare <BillK@rosw.com>
 *
 *****************************************************************************/
#include "types.h"
#include "lpc213x.h"
#include "config.h"
#include "armVIC.h"
#include "sysTime.h"
#include "uart.h"

#include "mtutil.h"
#include "onewire.h"
#include "ds18x20.h"

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
  IOCLR1 = PIO_ZERO_BITS;                // clear the ZEROs output
  IOSET1 = PIO_ONE_BITS;                 // set the ONEs output
  IODIR1 = PIO_OUTPUT_BITS;              // set the output bit direction

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
  VICIntEnClr = 0xFFFFFFFF;           		// clear all interrupts
  VICIntSelect = 0x00000000;            	// clear all FIQ selections
  VICDefVectAddr = (uint32_t)reset;     	// point unvectored IRQs to reset()

//  wdtInit();                            	// initialize the watchdog timer
  initSysTime();                        	// initialize the system timer
  uart0Init( UART_BAUD(HOST_BAUD), UART_8N1, UART_FIFO_8 ); // setup the UART
}



#define MAXSENSORS 5

uint8_t gSensorIDs[MAXSENSORS][OW_ROMCODE_SIZE];

static uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;
	
	uart0Puts( "\r\nScanning Bus for DS18X20\r\n" );
	
	nSensors = 0;
	
	for( diff = OW_SEARCH_FIRST; 
		diff != OW_LAST_DEVICE && nSensors < MAXSENSORS ; )
	{
		DS18X20_find_sensor( &diff, &id[0] );
		
		if( diff == OW_PRESENCE_ERR ) {
			uart0Puts( "No Sensor found\r\n" );
			break;
		}
		
		if( diff == OW_DATA_ERR ) {
			uart0Puts( "Bus Error\r\n" );
			break;
		}
		
		for (i=0;i<OW_ROMCODE_SIZE;i++) {
			gSensorIDs[nSensors][i]=id[i];
		}
		
		if ( 237 == gSensorIDs[0][1] ) {
			uart0Puts( "247\r\n" );
		}
		
		if ( 36 == gSensorIDs[0][1] ) {
			uart0Puts( "36\r\n" );
		}
		
		nSensors++;
	}
	
	return nSensors;
}

static void uart_put_temp(const uint8_t subzero, uint8_t cel, 
	uint8_t cel_frac_bits)
{
	// uint8_t buffer[sizeof(int)*8+1];
	// uint8_t i, j;
	
	uint16_t decicelsius;
	
	uart0Putch((subzero)?'-':'+');
	uart0Puts(itoa(cel));
	/*uart0Puts(".");
	itoa((cel_frac_bits*DS18X20_FRACCONV),buffer,10);
	j=4-strlen(buffer);
	for (i=0;i<j;i++) uart0_puts("0");
	uart0Puts(buffer);
	*/
	uart0Puts("°C [");
	// "rounding"
	uart0Putch((subzero)?'-':'+');
	decicelsius = DS18X20_temp_to_decicel(subzero, cel, cel_frac_bits);
	uart0Puts( itoa(decicelsius/10) );
	uart0Puts(".");
	uart0Putch( (decicelsius%10) + '0');
	uart0Puts("°C]");
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
	uint32_t startTime;
	
	uint8_t nSensors, i, timecnt;
	uint8_t subzero, cel, cel_frac_bits;
	
	sysInit();
	#if defined(UART0_TX_INT_MODE) || defined(UART0_RX_INT_MODE)
	enableIRQ();
	#endif
	startTime = getSysTICs();
	timecnt = 0;
	
	uart0Puts("\r\nLPC2106 - DS18x20 Temp.-Sensor interface\r\n");
	uart0Puts("a WinARM Demo-Application by Martin Thomas\r\n\r\n");
	
/*	
	while( 1 ) {
		if ( ( IOPIN0 & OW_BIT ) ) {
			uart0Puts("ON\r\n");
		}
		else {
			uart0Puts("OFF\r\n");
		}
	}
*/
	
	nSensors = search_sensors();
	uart0Puts( itoa(nSensors) );
	uart0Puts( " DS18X20 Sensors available:\r\n" );
	for (i=0; i<nSensors; i++) {
		uart0Puts("Sensor# ");
		uart0Puts(itoa(i+1));
		uart0Puts(" is a ");
		if ( gSensorIDs[i][0] == DS18S20_ID)
			uart0Puts("DS18S20/DS1820");
		else 
			uart0Puts("DS18B20");
		uart0Puts(" which is ");
		if ( DS18X20_get_power_status( &gSensorIDs[i][0] ) ==
			DS18X20_POWER_PARASITE ) 
			uart0Puts( "parasite" );
		else uart0Puts( "externally" ); 
		uart0Puts( " powered\r" );
	}

	for (;;) {
		do {
			int ch;
			
			if ((ch = uart0Getch()) >= 0) {
				uart0Puts("the <");
				uart0Putch(ch);
				uart0Puts("> key has been pressed\r\n");
				if (ch=='p') ow_delay_debug();
			}
		} while (getElapsedSysTICs(startTime) < HALF_SEC);
	
		if (IOPIN1 & LED1_BIT) IOCLR1 = LED1_BIT;
		else IOSET1 = LED1_BIT; 	
	
		startTime += HALF_SEC;
		
		timecnt++;
		if (timecnt==2) {
			uart0Puts("Start conversion all sensors ");
			if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, 0 ) 
				== DS18X20_OK) 
				uart0Puts("OK ");
			else 
				uart0Puts("failed (short circuit?)");
			uart0Puts("\r\n");
		}
		else if (timecnt==6) {
			for ( i=0; i<nSensors; i++ ) {
				uart0Puts("Sensor# ");
				uart0Puts(itoa(i+1));
				uart0Puts(" = ");
				if ( DS18X20_read_meas( &gSensorIDs[i][0], &subzero,
						&cel, &cel_frac_bits) == DS18X20_OK ) {
					uart_put_temp(subzero, cel, cel_frac_bits);
				}
				else uart0Puts("CRC Error (lost connection?)");
				uart0Puts("\r\n");
			}
		}
		else if (timecnt>10) timecnt=0;
	}
	
	return 0;	
}
