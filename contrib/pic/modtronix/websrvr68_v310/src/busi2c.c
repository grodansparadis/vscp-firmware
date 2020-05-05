 /**
 * @brief           Interrupt driven Serial receive and transmit handler.
 * @file            busi2c.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_serint
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

#define THIS_IS_BUSI2C

#include "projdefs.h"
#include "busi2c.h"
#include "busser1.h"	// TEST TEST
#include "net\helpers.h"
#include "net\i2c.h"
#include "net\tick.h"
#include "net\delay.h"


//Get our bus id. This is defined in buses.h. It needed to access TX and RX buffers that are
//defined in the buses module
#define BUSID BUSID_I2C1


/**
 * Initialize all I2C modules
 */
void i2cBusInit(void) {
	//I2C bus is always enabled
	busEnable(BUSID);
}


/**
 * Service routine. Must be regulary called by main program.
 * This fuction checks if the transmit buffer contains any data that has to be transmitted. If it does,
 * it will decode and transmit it via the I2C bus. Any data that has to be read from I2C, will be read
 * and added to the receive buffer.
 */
void i2cBusService(void) {
	BYTE adr;	//I2C address
	BYTE c;
	BYTE i;
    BYTE lsb;
    BYTE msb;

	//Message must start with a 'S' character (Start command), remove all other data
	while (busTxBufHasData(BUSID) && (busPeekByteTxBuf(BUSID) != 'S') ) {
		busRemoveByteTxBuf(BUSID);
	}

	//Buffer empty, Nothing to do
	if (busIsTxBufEmpty(BUSID))
		return;

	//Peek in transmit buffer, and only start I2C communication once a whole I2C message is available. That is, [Start][Adr][Data][Stop].
	//We thus have to peek for 'P' command = end of I2C message
	adr = 0;
	for (i=0; i<busGetTxBufCount(BUSID); i++) {
		if (busPeekByteTxBufAt(BUSID, i) == 'P') {
			adr = 1;	//Indicate that 'P' character was found
			//Found a stop character
			break;
		}
	}
	//If 'P' not found, return
	if (adr==0)
		return;

	//At this stage, first character will be 'S', remove it
	busRemoveByteTxBuf(BUSID);

	//Next two characters should be address, get them (in 2 character ASCII HEX format), and convert to byte
	if (busGetTxBufCount(BUSID) >= 2) {
	    msb = getHexNibble(busGetByteTxBuf(BUSID));
	    lsb = getHexNibble(busGetByteTxBuf(BUSID));

		//If both bytes were not ASCII hex, return
	    if ((lsb == 0xff) || (msb == 0xff)) {	
	        return;   //Return error
		}

	    adr = ((msb << 4) | lsb);
	}
	
    i2cWaitForIdle();

    i2cPutStartAndWait();       //Write I2C Start Condition, and wait for it to complete

    //Write module address
    i2cPutByteAndWait(adr);

    //Only continue if slave sent ACK. If no slave present, no ACK is sent. If no slave with address sent is present, no ACK is sent.
    if(i2cWasAckReceived()==0) {
        i2cPutStopAndWait();    //Reset I2C bus

		//Return "NAA" to bus we are connected to, where AA is uppercase ASCII hex of address. Indicates no slave replied at this address
		busPutByteRxBuf(BUSID, 'N');
		busPutByteRxBuf(BUSID, btohexa_high(adr));
		busPutByteRxBuf(BUSID, btohexa_low(adr));
		busPutByteRxBuf(BUSID, 0x0A);
		return;
    }

	//R/W bit set in I2C address byte. We have to read from I2C bus
	if (adr & 0x01) {
		i=0;	//Count how many bytes we read from slave

		// TODO
		// Add code for reading from I2C node. See lcdGetByte() function in lcd2s.c file to details how to do this.

		//Return "RAA,NN,DD" to bus we are connected to, where AA is uppercase ASCII hex of address, NN is uppercase ASCII hex indicating
		//how many bytes we read, and following pairs of DD are uppercase ASCII hex of bytes read.
		busPutByteRxBuf(BUSID, 'R');
		busPutByteRxBuf(BUSID, btohexa_high(adr));
		busPutByteRxBuf(BUSID, btohexa_low(adr));
		busPutByteRxBuf(BUSID, ',');
		busPutByteRxBuf(BUSID, btohexa_high(i));
		busPutByteRxBuf(BUSID, btohexa_low(i));
		busPutByteRxBuf(BUSID, ',');
		busPutByteRxBuf(BUSID, btohexa_high(0));
		busPutByteRxBuf(BUSID, btohexa_low(0));
		busPutByteRxBuf(BUSID, 0x0A);

		DelayMs(10);
	}
	//R/W bit clear in I2C address byte. We have to write to I2C bus
	else {
		i = 0;	//Count how many bytes we write to the I2C bus
		//All following characters, up to 'P' character is data. Buffer must still contain at least 3 characters = 2 char ASCII hex, and 'P'
		while (busGetTxBufCount(BUSID) > 2) {
			i++;
		    msb = getHexNibble(busGetByteTxBuf(BUSID));
		    lsb = getHexNibble(busGetByteTxBuf(BUSID));

			//If both bytes were not ASCII hex, return
		    if ((lsb == 0xff) || (msb == 0xff)) {
				i2cPutStopAndWait();    //Reset I2C bus
		        return;   //Return error
			}

			i2cPutByteAndWait((msb << 4) | lsb);
		}

		//Return "WAA,NN" to bus we are connected to, where AA is uppercase ASCII hex of address, and NN is uppercase ASCII hex indicating
		//how many bytes we wrote to the node.
		busPutByteRxBuf(BUSID, 'W');
		busPutByteRxBuf(BUSID, btohexa_high(adr));
		busPutByteRxBuf(BUSID, btohexa_low(adr));
		busPutByteRxBuf(BUSID, ',');
		busPutByteRxBuf(BUSID, btohexa_high(i));
		busPutByteRxBuf(BUSID, btohexa_low(i));
		busPutByteRxBuf(BUSID, 0x0A);
	}

	//At this stage, 'P' should be the only remaining one in buffer, remove it
	busRemoveByteTxBuf(BUSID);

    i2cPutStopAndWait();    //Write I2C Stop condition, and wait for it to complete
}


/**
 * Resets this module, and empties all buffers.
 */
void i2c1BusReset(void) {
    //Set transmit buffer to empty
    busEmptyTxBuf(BUSID);

    //Set receive buffer to empty
    busEmptyRxBuf(BUSID);
}


/**
 * Get the next byte in the RX buffer. Before calling this function, the
 * i2c1BusIsGetReady() function should be called to check if there is any
 * data available.
 *
 * @return Returns next byte in receive buffer.
 */
BYTE i2c1BusGetByte(void) {
    BYTE c;

    if (busRxBufHasData(BUSID)) {
        c = busPeekByteRxBuf(BUSID);
        busRemoveByteRxBuf(BUSID);
    }
    
    return c;
}


/**
 * Add a byte to the TX buffer.
 *
 * @param c     Byte to write out on the serial port
 */
void i2c1BusPutByte(BYTE c) {
    //Add byte to TX buffer, and update buffer pointers
    busPutByteTxBuf(BUSID, c);
}


/**
 * Send the ASCII hex value of the given byte to the BUS. It is added to the transmit buffer, and
 * asynchronously transmitted. For example, if c=11, then "0B" will be sent to the Bus
 *
 * @param c     Byte to write out on the bus
 */
void i2c1BusPutByteHex(BYTE c) {
    i2c1BusPutByte( (c > 0x9f) ? ((c>>4)&0x0f)+0x57 : ((c>>4)&0x0f)+'0' );
    i2c1BusPutByte( ((c&0x0f)>9) ? (c&0x0f)+0x57 : (c&0x0f)+'0' );
}


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param s     Null terminated string to write out on the bus.
 */
void i2c1BusPutString(BYTE* s) {
    char c;

    while((c = *s++)) {
        i2c1BusPutByte(c);
        FAST_USER_PROCESS();
    }
}


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param str   Null terminated string to write out on the bus.
 */
void i2c1BusPutRomString(ROM char* s) {
    char c;

    while((c = *s++)) {
        i2c1BusPutByte(c);
        FAST_USER_PROCESS();
    }
}
