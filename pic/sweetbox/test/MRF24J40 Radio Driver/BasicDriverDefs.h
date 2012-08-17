/*****************************************************************************
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the "Company") is intended and supplied to you, the Company's
 * customer, for use solely and exclusively with products manufactured
 * by the Company.
 *
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *****************************************************************************/

// application specific (for demo purposes only)
#define BAUD_RATE 19200
//#define ENABLE_CONSOLE
#define SEASOLVE

// IEEE EUI - globally unique number
#define EUI_7 0x00
#define EUI_6 0x01
#define EUI_5 0x02
#define EUI_4 0x03
#define EUI_3 0x04
#define EUI_2 0x05
#define EUI_1 0x06
#define EUI_0 0x07

// SPI Open Config setting (see spi.h)
#define SPI_OPEN_CFG_1			( PRI_PRESCAL_4_1 | SEC_PRESCAL_8_1 | MASTER_ENABLE_ON | SPI_CKE_ON | SPI_SMP_ON | SPI_MODE8_ON | ENABLE_SDO_PIN )
#define SPI_OPEN_CFG_2			( SPI_ENABLE )

// SPI baud rate.  Make sure that the baud rate is within spec. (recommended < 20MHz)).
#define IP_SPI_BRG      		( 10 )

//Interrupt Definition Macros
#define ISR(PRIOR,VECT) void __attribute__ ((interrupt(PRIOR))) __attribute__ ((vector(VECT))) interrupt ## VECT (void)
#define INTERRUPT(PRIOR,VECT,NAME) void __attribute__ ((interrupt(PRIOR))) __attribute__ ((vector(VECT))) NAME (void)

// Do nothing
#define Reset()					SoftReset()

#define CLOCK_FREQ      		80000000

// INT1
// Set up the direction for the interrupt input 
#define mSetupINTDirection()	{   mPORTESetPinsDigitalIn( BIT_8 );  }


// Chip Select
// -----------

// Direction
#define mSetupCSDirection()     {   mPORTBSetBits( BIT_2 ); mPORTBSetPinsDigitalOut( BIT_2 );  }

// CS on, active low
#define mSetCSOn()              {   mPORTBClearBits( BIT_2 ); }

// CS off, active low
#define mSetCSOff()             {   mPORTBSetBits( BIT_2 ); }

// Reset
// -----

// Direction
#define mSetupResetDirection()  {   mPORTGSetBits( BIT_2 ); mPORTGSetPinsDigitalOut( BIT_2 );  }

// Reset on, active low
#define mSetResetOn()           {   mPORTGClearBits( BIT_2 ); }

// Reset off, active low
#define mSetResetOff()          {   mPORTGSetBits( BIT_2 );   }

// Wake
// ----

// Set up the direction for Wake 
#define mSetupWakeDirection()	{  mPORTGSetBits( BIT_3 ); mPORTGSetPinsDigitalOut( BIT_3 );  }  

// Wake on
#define mSetWakeOn()           	{   mPORTGClearBits( BIT_3 ); }

// Wake off
#define mSetWakeOff()          	{   mPORTGSetBits( BIT_3 );   }

 

#define PA_LEVEL 			0x00  	// -0.00 dBm
#define FREQUENCY_BAND 		2400
