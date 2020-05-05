 /**
 * @brief           Serial Buses
 * @file            buses.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_buses
 *
 *
 * @section description Description
 **********************************
 * This module contains code for implementing different serial buses.
 *
 * @section buses_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //--------------------  Buses Configuration --------------------
 //*********************************************************************
 //Default "UDP Command Port"
 //#define DEFAULT_CMD_UDPPORT (54123)

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
 * 2006-12-08, David Hosken (DH):
 *    - Initial version
 *********************************************************************/


/////////////////////////////////////////////////
//  Serial Buses - documentation module
 /**
 * @defgroup mod_buses Serial Buses
 *
 * <b>Serial Buses have not been implemented yet! Will be implemented in a future version! </b>
 *
 * The @appname implements many different serial buses that can be linked together via the web
 * based interface. For example, USART 1 (RS232 port) can be linked to a UDP port to implement a
 * USART to UDP interface converters.
 * The following buses are currently implemented:
 * - Two software SPI buses, SWSPI1 and SWSPI2
 * - One master I2C buses.
 * - Two USARTS, USART1 and USART2. They use the actual USART1 and 2 hardware peripherals of
 *   the CPU
 * - Two UDP ports
 * - One TCP port
 *
 * By default, there are 8 buffers that can be used for the buses. Each bus can be
 * configured to have a receive and/or transmit buffer. When configured to have a receive buffer,
 * all received data will be stored to it. When configured to have a transmit buffer,
 * all data contained in this buffer will be transmitted.
 * The following buffers are defined:
 * - Buffer 0: 128 bytes
 * - Buffer 1: 128 bytes
 * - Buffer 2: 64 bytes
 * - Buffer 3: 64 bytes
 * - Buffer 4: 32 bytes
 * - Buffer 5: 32 bytes
 * - Buffer 6: 32 bytes
 * - Buffer 7: 32 bytes
 *********************************************************************/

#ifndef BUSES_H
#define BUSES_H

//Define size of all buffers. The size must be a exponent of 2
#define BUSBUF0_SIZE 128
#define BUSBUF1_SIZE 128
#define BUSBUF2_SIZE 64
#define BUSBUF3_SIZE 64
#define BUSBUF4_SIZE 32
#define BUSBUF5_SIZE 32
#define BUSBUF6_SIZE 32
#define BUSBUF7_SIZE 32

//Number of buffers available
#define BUS_BUFFERS 8

#define getBufSize(i) (i==0?BUSBUF0_SIZE: (i==1?BUSBUF1_SIZE:0))


/**
 * Returns the constant value from 1 to 7 used to represent a buffer of 8, 16, ... to 512 bytes in size.
 *
 * @param n Buffer size, must be one of the following values: 8, 16, 32, 64, 128, 256 or 512
 *
 * @return Returns a constant value representing the buffer size. For example, 1 is return for a buffer size of 8.
 */
#define getBufSizeConst(n) ((n==8)?1: ((n==16)?2: ((n==32)?3: ((n==64)?4: ((n==128)?5: ((n==256)?6: ((n==512)?7:0)))))))


/**
 * Structure used for configuraing TX and RX buffers for each bus.
 *
 * Bits 0-3 of each element contains a value from 0-7 giving the buffer ID of the buffer assigned
 * to this bus.
 *
 * Bits 4-7 give the buffer size, where 0=undef, 1=8, 2=16, .. 6=256 and 7=512.
 *
 * If the bus does not have a bus assigned to it, it will have a value of 0
 */
typedef struct _BUS_BUF_CONF
{
    BYTE    ser1Buf;
    BYTE    ser2Buf;
    BYTE    udp1Buf;
    BYTE    udp2Buf;
} BUS_BUF_CONF;

/**
 * The number of buses
 */
#define BUS_COUNT (sizeof(BUS_BUF_CONF))

/**
 * Structure for storing bus configuration data that needs fast access.
 * This structure is created in the processors internal RAM, and is initialized with
 * the contents of the corresponding bytes in EEPROM at startup. Whenever the application
 * needs to access any of these variables, it will read them straight from this RAM structure
 * in stead of reading them from the EEPROM.
 */
typedef struct _BUS_CONFIG
{
    BUS_BUF_CONF txBuf;
    BUS_BUF_CONF rxBuf;
} BUS_CONFIG;

/**
 * Structure containing bus data.
 *
 */
typedef struct _BUS_INFO
{
    BYTE*   buf[BUS_COUNT];     //Pointer to bus buffer
    BYTE    put[BUS_COUNT];     //Buffer put index
    BYTE    get[BUS_COUNT];     //Buffer get index
} BUS_INFO;



#if !defined(THIS_IS_BUSES_MODULE)
extern BYTE busBuf0[BUSBUF0_SIZE];
extern BYTE busBuf1[BUSBUF1_SIZE];
extern BYTE busBuf2[BUSBUF2_SIZE];
extern BYTE busBuf3[BUSBUF3_SIZE];
extern BYTE busBuf4[BUSBUF4_SIZE];
extern BYTE busBuf5[BUSBUF5_SIZE];
extern BYTE busBuf6[BUSBUF6_SIZE];
extern BYTE busBuf7[BUSBUF7_SIZE];

extern BUS_CONFIG busConfig;
extern BUS_INFO   busInfo;
#endif


/**
 * Initializes "UDP Command Port" and "UDP Command Responce Port". These
 * ports are used for sending and receiving and commands via the UDP port.
 *
 */
void busInit(void);

//void busLoadBusConfig(

/**
 * Must be called every couple of ms
 *
 */
void busProcess(void);

/**
 * Returns the number of bytes that can be written to the given buffer.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 *
 * @return The number of bytes that can be written to the given buffer
 */
BYTE busIsPutReady(BYTE bufID);

/**
 * Adds the given byte to the given buffer. If any bus is assigned to this buffer, it will transmit
 * this data.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param b     The byte to add to the given buffer
 */
void busPutByte(BYTE bufID, BYTE b);

/**
 * Adds the given byte array to the given buffer. If any bus is assigned to this buffer, it will transmit
 * this data.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param buf   The byte array to add to the given buffer
 * @param len   The length of the buf array
 */
void busPutArray(BYTE bufID, BYTE* buf, BYTE len);

/**
 * Returns the number of bytes that can be read from the given buffer.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 *
 * @return The number of bytes that can be read from the given buffer
 */
BYTE busIsGetReady(BYTE bufID);

/**
 * Gets a single byte from the given buffer. If any bus is assigned to this buffer, it will transmit
 * this data.
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param b   The byte to add to the given buffer
 */
void busGetByte(BYTE bufID, BYTE b);

/**
 * Gets an array of bytes from the given buffer. 
 *
 * @param bufID The buffer ID, is a number from 0 to n identifying the buffer
 * @param buf   The byte array to add to the read bytes to
 * @param len   The number of bytes to read
 *
 * @return The actual number of bytes read
 */
BYTE busGetArray(BYTE bufID, BYTE* buf, BYTE len);

#endif
