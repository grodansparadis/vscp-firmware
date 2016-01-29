/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Main application
*******************************************************************************/

#include "digiprobe_gcc.inc"

//somewhat bad behaviour
//#include "digiprobe_memory.c"

int32_t sens_current_raw=0;
int16_t sens_temp_raw=0;
uint8_t samples=0;
uint8_t average=0;

uint8_t adstatus=0;

char txt [20];

#ifdef DIGIPROBE_USE_FLOAT
	float comp;
	float accu;
	double dbl1;
	double dbl2;
#endif

// set the fuses in the ELF file - pay attention!
// 8MHz ext. crystal 1k CK 0ms, noJTAG, noCKDIV, Brownout=3.8V, EESAVE, Bootsz=4096, BOOTRST
FUSES = 
{
	0xEC, // .low
	0xD0, // .high
	0xF7, // .extended
};



///////////////////////////////////////////////////////////////////////////////
// Timer 0 Compare interupt
// We should come to this point once every millisecond

SIGNAL( SIG_OUTPUT_COMPARE0 )
{
	// handle millisecond counter
	vscp_timer++;
	clock_current++;
//	clock_temp++;
}



///////////////////////////////////////////////////////////////////////////////
// main
//

int main( void )
{	

	//init the avr ports
	init_avr_ports();
	
	//22bit adc init
	adstatus = mcp355xs_init(MCP355X_CONTINOUS);
	
	adc_init(ADCREF_256V,ADCPS_128);

	// Initialize the 1ms timer
	initTimer();	

	sei();

	// Init can
	if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) 
		{
		//uart_puts("Failed to open channel!!\n");
		}
		
    // Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() )
		{
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();
		}
		
	vscp_init();			// Initialize the VSCP functionality
	
    while ( 1 ) {  // Loop Forever

		// Check for any valid CAN message
		vscp_imsg.flags = 0;
		vscp_getEvent();

		vscp_protocol_class();		

		//do some timed work
		if ( vscp_timer >= 1000 ) 
			{
			vscp_timer = 0;
			// Do VSCP one second jobs 
			vscp_doOneSecondWork();
			}

		if (vscp_node_state == VSCP_STATE_ACTIVE)
			{

			// do what the application defines
			do_work();

			}
		
    } // end of working while loop
    
} //end of main loop
