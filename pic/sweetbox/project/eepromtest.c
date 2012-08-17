/******************************************************************************
 * FileName:        I2C_master.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *********************************************************************
 * File Description: I2C Peripheral Lib Demo
 *
 * Change History:
 * Name        Date            Changes
 * A Rafiq     2/7/07          Initial Version
 *
 * $Id: eepromtest.c 154 2008-10-14 20:43:10Z akhe $
 *
 * Demo Code Description:
 * This demo code excersize the i2c channel 1 by interfacing
 * to 24LC256 serial eeprom.  The code writes to a location
 * and then verify the contents of the eeprom.  I2C Channel 1
 * is hooked up to 24LC256.  Address lines of the eeprom are
 * all tied to Vss.
 ********************************************************************/
#include <plib.h>


// Configuration Bit settings
// SYSCLK = 72 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 36 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
//
#pragma config FPLLMUL = MUL_18, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2


#define SYSCLK  (72000000) 
#define PBCLK   (SYSCLK/2)

#define Fsck	50000
#define BRG_VAL ((PBCLK/2/Fsck)-2)

#define Nop() asm( "nop" )


/////////////////////////////////////////////
/////////////////////////////////////////////
void i2c_wait(unsigned int cnt)
{
	while(--cnt)
	{
		Nop();
		Nop();
	}
}

/////////////////////////////////////////////
/////////////////////////////////////////////
int main(void)
{
	unsigned char SlaveAddress;
	char i2cData[ 10 ];
	int  DataSz;

	// Enable channel
	OpenI2C1( I2C_EN, BRG_VAL );

	SlaveAddress = 0x50;	// 0b1010000 Serial EEPROM address

	// Send Data to eeprom to program one location

	i2cData[0] = ( SlaveAddress << 1 ) | 0;	// EEPROM Device Address and WR Command
	i2cData[1] = 0x05;	// eeprom location to program (high address byte)
	i2cData[2] = 0x40;	// eeprom location to program (low address byte)
	i2cData[3] = 0xAA;	// data to write
	DataSz = 4;

	StartI2C1();		// Send the Start Bit
	IdleI2C1();			// Wait to complete

	int Index = 0;
	while( DataSz ) {
    	
		MasterWriteI2C1( i2cData[ Index++ ] );
		IdleI2C1();		// Wait to complete

		DataSz--;

		// ACKSTAT is 0 when slave acknowledge. if 1 then slave has not acknowledge the data.
		if ( I2C1STATbits.ACKSTAT ) {
			break;
		}
	}

	StopI2C1();			// Send the Stop condition
	IdleI2C1();			// Wait to complete

	// wait for eeprom to complete write process. poll the ack status
	while( 1 )
	{
		i2c_wait( 10 );

		StartI2C1();	// Send the Start Bit
		IdleI2C1();		// Wait to complete

		MasterWriteI2C1( i2cData[0] );
		IdleI2C1();		// Wait to complete

		if ( I2C1STATbits.ACKSTAT == 0 )	// eeprom has acknowledged
		{
			StopI2C1();	// Send the Stop condition
			IdleI2C1();	// Wait to complete
			break;
		}

		StopI2C1();		// Send the Stop condition
		IdleI2C1();		// Wait to complete
	}

	// Now Readback the data from the serial eeprom

	i2cData[ 0 ] = (SlaveAddress << 1) | 0;	// EEPROM Device Address and WR Command (to write the address)
	i2cData[ 1 ] = 0x05;	// eeprom location to read (high address byte)
	i2cData[ 2 ] = 0x40;	// eeprom location to read (low address byte)
	DataSz = 3;

	StartI2C1();	// Send the Start Bit
	IdleI2C1();		// Wait to complete

	// send the address to read from the serial eeprom
	Index = 0;
	while( DataSz ) {
    	
		MasterWriteI2C1( i2cData[ Index++ ] );
		IdleI2C1();		//Wait to complete

		DataSz--;

		// ACKSTAT is 0 when slave acknowledge. if 1 then slave has not acknowledge the data.
		if( I2C1STATbits.ACKSTAT ) {
			break;
		}	
	}

	//now send a start sequence again
	RestartI2C1();	// Send the Restart condition
	
	//wait for this bit to go back to zero
	IdleI2C1();	    // Wait to complete

	MasterWriteI2C1( (SlaveAddress << 1) | 1 ); // transmit read command
	IdleI2C1();		// Wait to complete

	unsigned char i2cbyte;
	i2cbyte = MasterReadI2C1();

	StopI2C1();	    // Send the Stop condition
	IdleI2C1();	    // Wait to complete

	if( i2cbyte != 0xAA ){
		while wede(1)    // error: verify failed
		{}
	}

	while (1) // Success
	{}

}


