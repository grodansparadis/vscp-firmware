/**
 * @brief           Part of the Modtronix Configurable Buses. This modules uses buffers from
 *                  the buses module for it's transmit and receive buffers.
 * @file            busi2c.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v3.21 or higher
 * @ingroup         mod_buses
 *
 *
 * @section description Description
 **********************************
 * This module makes the I2C bus available for writing and reading data to
 * and from. The bus is configured at 400kbits/sec by default.
 *
 * All data written to the transmit buffer is decoded by this module, and will
 * generally instruct data to be written, or read from a I2C slave node. An ASCII
 * text based message protocol is used. The protocol format is:
 *
 * @subsection busi2c1_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- BusI2C1 Configuration --------------------
 //*********************************************************************

 @endcode
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
 * 2008-07-18, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

/////////////////////////////////////////////////
//  Events - documentation module
/** @defgroup mod_events Events
@section mod_events_intro Introduction

/////////////////////////////////////////////////
//  I2C Bus - documentation module
/** @defgroup mod_buses_busi2c I2C Bus
@ingroup mod_buses

@section mod_buses_busi2c_intro Introduction
This module makes the I2C bus available for writing and reading data to
and from. The bus is configured at 400kbits/sec by default.

All data written to the transmit buffer is decoded by this module, and will
generally instruct data to be written, or read from a I2C slave node. An ASCII
text based message protocol is used.


@section mod_buses_busi2c_protocol Message Protocol
Currently the following commands are defined:
@htmlonly
<table class=compactTable cellpadding=2 cellspacing=0 border=1>
<tr width="100%"><td align="center"><i><b>Command</b></i></td><td align="center"><i><b>Format</b></i></td><td align="center"><i><b>Description</b></i></td></tr>
<tr><td><b>0-9<br>A-F</b></td> <td><code>ASCII HEX</code></td>
    <td>A 8 bit value represented in ASCII HEX format.</td>
</tr>
<tr><td><b>S</b></td> <td><code>S</code></td>
    <td>Put a I2C Start condition on the bus.</td>
</tr>
<tr><td><b>P</b></td> <td><code>P</code></td>
    <td>Put a I2C Stop condition on the bus. This command returns the I2C bus to the idle state. You should always leave the bus 
    in this state when finished with a command.</td>
</tr>
<tr><td><b>R</b></td> <td><code>Rnn</code></td>
    <td>Reads given number of bytes from I2C slave node. Where 'nn' gives number of bytes to read in a 2 digit value.
    For example, <i>R05</i> will read 5 bytes from I2C bus.
  	The I2C bus can not be idle when issuing this command, and must be preceded by a S (I2C Start Condition) command.
    </td>
</tr>
<tr><td><b>W</b></td> <td><code>Wxx</code></td>
    <td>Writes a single byte to the I2C bus. Where "xx" represents a byte in two-digit upper case hex format.
    For example, <i>W1F</i> will write 0x1F (31 decimal) to the I2C bus.
  	The I2C bus can not be idle when issuing this command, and must be preceded by a S (I2C Start Condition) command.
    </td>
</tr>
</table>
@endhtmlonly

@section mod_buses_busi2c_read Reading from I2C bus
Each I2C node actually has two addresses. The even address is used to write to the node, and the odd address to read from it.
Bit 0 (R/W bit) of the address byte is used to determine if a read or write command has to be executed. If 1, a I2C read is executed.
To read, set bit 0 of the address. For example, accessing address 0x20 will write to the node, and address 0x21 will read from it.


@section mod_buses_busi2c_examples Examples

Following are some examples for communicating with nodes on the I2C bus.
<br><b>Writing 0x10 to an I2C node with address 5A</b>
<br><code><b>S5A10P</b></code>
<br>[I2C Start][Address:0x5A][Data:0x10][I2C Stop]
<br>This commands sends and I2C message, beginning with a I2C Start condition, and ending with an
I2C stop condition.

<br><b>Writing "Hello" to an I2C node with address 25</b>
<br><code><b>S2548656C6C6FP</b></code>
<br>[I2C Start][Address:0x5A][Data:0x48, 0x65, 0x6C, 0x6C, ox6F][I2C Stop]
<br>This commands sends and I2C message, beginning with a I2C Start condition, and ending with an
I2C stop condition. The ASCII string "Hello" is sent. Each character has to be sent with it's ASCII code.

<br><b>Reading two bytes from an I2C node with address 51</b>
<br><code><b>S5102P</b></code>
<br>[I2C Start][Address:0x51, R/W = Read][Number of bytes to read: 02][I2C Stop]
<br>This commands sends and I2C message, beginning with a I2C Start condition, and ending with an
I2C stop condition. Two bytes are read from the I2C node with address 0x51 (or 0x50 for reading).

<br><b>Repeated Start, write 1 byte (0x15), reading 3 bytes from an I2C node with address 2A (or 2B for writing)</b>
<br><code><b>S2A15 S2B03P</b></code>
<br>[I2C Start][Address:0x2A, R/W = Write][Data: 0x15] [I2C Repeated Start][Address:0x2B, R/W = Read][Number of bytes to read: 03][I2C Stop]
<br>This commands uses the I2C repeated start feature. If a second start command is sent, without sending a stop command, it will cause a repeated start condition to be put on the I2C bus.
A single byte is written to the I2C node with address 0x2A, and 3 bytes are read from the I2C node with address 0x2B.
*/


#ifndef _BUSI2C_H_
#define _BUSI2C_H_

#include "buses.h"

/////////////////////////////////////////////////
//Global defines


/////////////////////////////////////////////////
//i2c1Stat defines
#define I2C1_TXBUF_OVERRUN    0x01    //The transmit buffer had overrun. Must be cleared by the user
#define I2C1_RXBUF_OVERRUN    0x10    //The receive buffer had overrun. Must be cleared by the user


/////////////////////////////////////////////////
//Global data
#if !defined(THIS_IS_BUSI2C)
extern BYTE i2c1Stat;
#endif

/////////////////////////////////////////////////
//Function prototypes


/**
 * Initialize all I2C modules
 */
void i2cBusInit(void);


/**
 * Service all I2C modules
 */
void i2cBusService(void);


/**
 * Resets this module, and empties all buffers.
 */
void i2c1BusReset(void);


/**
 * Are there any bytes in the receive buffer.
 * @return 0 if not empty, 1 if empty
 */
#define i2c1BusRxBufEmpty() busIsRxBufEmpty(BUSID_I2C1)


/**
 * Are there any bytes in the I2C1 Bus receive buffer.
 * @return 1 if true, else 0
 */
#define i2c1BusIsGetReady() busRxBufHasData(BUSID_I2C1)

/**
 * Get the next byte in the RX buffer. Before calling this function, the
 * i2c1BusIsGetReady() function should be called to check if there is any
 * data available.
 *
 * @return Returns next byte in receive buffer.
 */
BYTE i2c1BusGetByte(void);


/**
 * Add a byte to the TX buffer.
 *
 * @param c     Byte to write out on the serial port
 */
void i2c1BusPutByte(BYTE c);


/**
 * Send the ASCII hex value of the given byte to the BUS. It is added to the transmit buffer, and
 * asynchronously transmitted. For example, if c=11, then "0B" will be sent to the Bus
 *
 * @param c     Byte to write out on the bus
 */
void i2c1BusPutByteHex(BYTE c);


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param s     Null terminated string to write out on the bus.
 */
void i2c1BusPutString(BYTE* s);


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted. The NULL is NOT sent!
 *
 * @param str   Null terminated string to write out on the bus.
 */
void i2c1BusPutRomString(ROM char* str);


#endif    //_BUSI2C_H_




