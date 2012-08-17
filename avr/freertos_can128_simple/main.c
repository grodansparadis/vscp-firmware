///////////////////////////////////////////////////////////////////////////////
// AT90CAN128 RTOS test code
//
// This simple code sends data on UART0 and blinks some LED's as
// two different tasks.
//
// ----------------------------------------------------------------------------
// Configuration for FREERTOS and this application (baudrates etc) is done in
// the FreeRTOSConfig.h file.
// ----------------------------------------------------------------------------
//

#include <stdlib.h>
#include <string.h>

#ifdef GCC_MEGA_AVR
	/* EEPROM routines used only with the WinAVR compiler. */
	#include <avr/eeprom.h> 
#endif

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#include "../common/avr_macros.h"
#include "../common/freertos_serial.h"

// General prototypes
void LED_On( int i );

// Prototypes for tasks
static void task1( void *pvParameters );
static void task2( void *pvParameters );


///////////////////////////////////////////////////////////////////////////////
// LED_On
//

void LED_On( int i )
{
	PORTB = ~BIT(i);	/* low output to turn LED on */
	vTaskDelay( 500 );
}


///////////////////////////////////////////////////////////////////////////////
// main
//

portSHORT main( void )
{
    DDRB = 0xFF;	/* output */
	PORTB = 0xFF;	/* all off */
    
    // Initialize UART
	initSerialPort( USE_BAUDRATE, USE_RX_QUEUE_SIZE, USE_TX_QUEUE_SIZE );
	
	xTaskCreate( task1, "led", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 8, NULL );
    xTaskCreate( task2, "uart", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

	/* In this port, to use preemptive scheduler define configUSE_PREEMPTION 
	as 1 in portmacro.h.  To use the cooperative scheduler define 
	configUSE_PREEMPTION as 0. */
	vTaskStartScheduler();
	
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// task1
//

static void task1( void *pvParameters )
{
	static int i;

	/* The parameters are not used. */
	( void ) pvParameters;

	/* Cycle for ever, delaying then checking all the other tasks are still
	operating without error. */
	for( ;; ) {
		/* forward march */
		for (i = 0; i < 8; i++)
			LED_On(i);
		/* backward march */
		for (i = 8; i > 0; i--)
			LED_On(i);
		/* skip */
		for (i = 0; i < 8; i += 2)
			LED_On(i);
		for (i = 7; i > 0; i -= 2)
			LED_On(i);
		
	}
}

///////////////////////////////////////////////////////////////////////////////
// task2
//

static void task2( void *pvParameters )
{
	// The parameters are not used.
	( void ) pvParameters;

	for( ;; ) {
		
		serialPutChar( 'H', 10 );
		serialPutChar( 'e', 10 );
		serialPutChar( 'l', 10 );
		serialPutChar( 'l', 10 );
		serialPutChar( 'o', 10 );
		serialPutChar( 0x0d, 10 );
		serialPutChar( 0x0a, 10 );
		vTaskDelay( 50 );
		
	}
}

