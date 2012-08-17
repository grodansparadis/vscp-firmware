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
 //The default UDP port to use for the UDP 1 Port.
 //#define DEFAULT_BUS_UDP1PORT  (54125ul)
 //
 //The default UDP port to use for the UDP 2 Port.
 //#define DEFAULT_BUS_UDP2PORT  (54126ul)
 //
 //UDP1 Port
 //#define BUS_UDP1PORT DEFAULT_BUS_UDP1PORT
 //
 //UDP2 Port
 //#define BUS_UDP2PORT DEFAULT_BUS_UDP2PORT
 //
 //Serial Port 1 buffer sizes
 //#define BUS_SER1_TXBUFSIZE   50
 //#define BUS_SER1_RXBUFSIZE   100
 //
 //Serial Port 2 buffer sizes
 //#define BUS_SER2_TXBUFSIZE   20
 //#define BUS_SER2_RXBUFSIZE   20
 //
 //I2C Port 1 buffer sizes
 //#define BUS_I2C1_TXBUFSIZE   100
 //#define BUS_I2C1_RXBUFSIZE   20
 //
 //SPI Port 1 buffer sizes
 //#define BUS_SPI1_TXBUFSIZE   20
 //#define BUS_SPI1_RXBUFSIZE   20
 //
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
 * The @appname implements many different serial buses that can be linked together via the web
 * based interface. For example, USART 1 (RS232 port) can be linked to a UDP port to implement a
 * USART to UDP interface converters.
 * The following buses are currently implemented:
 * - Serial 1 bus, is available via the 3 pin serial connector on the board. This bus is
 *   connected to USART1 of the PIC chip, and can be configured for RS-232 or 5V TTL voltage levels.
 * - Serial 2 bus, is available via the daughter board connector. This bus is connected to the
 *   USART of the PIC chip, and has 5V TTL voltage levels.
 * - UDP Port 1, has a default port address of 54124
 * - UDP Port 2, has a default port address of 54125
 * - One master I2C buses. This bus is configured for 400kbit/sec. It available via the red
 *   MicroMatch connector on the @boardname. It can only be used in Master Mode.
 * - One master SPI port. This is a software SPI port. It consists of 4 signals, SDO (Data Out),
 *   SDI (Data In), CLK (clock) and CS (chip Select). These 4 pins have 5V TTL levels, and can be
 *   configured via the web page.
 *********************************************************************/

#ifndef BUSES_H
#define BUSES_H

//Total size of available buffer space
#define BUSBUFFER_SIZE 512ul

//Use this define if maximum buffer size is 256 bytes
#define BUFTYPE BYTE
//Use this define if maximum buffer size more than 256 bytes
//#define BUFTYPE WORD


/////////////////////////////////////////////////
//UDP Port 1 and 2 Configuration
#define BUSSTAT_UDP_ENABLE 0x01


/////////////////////////////////////////////////
//Stat defines
/** The transmit buffer has overrun. Must be cleared by the user with busStatClearOverrun() function. */
#define BUSSTAT_TX_OVERRUN    0x01

/** Indicates if this bus is currently transmitting. For interrupt driven buses, this means that
    a transmit interrupt can occur at any time. Checking this function should be performed in a critical
    section, so that no interrupt can occur while checking! */
#define BUSSTAT_TXING         0x02

/** The transmit buffer has overrun. Must be cleared by the user with busStatClearOverrun() function. */
#define BUSSTAT_RX_OVERRUN    0x10

/** The buffer is enabled */
#define BUSSTAT_ENABLED       0x80

//Bus identifiers, must be values from 0 to n
enum BUSES {
    BUSID_SER1 = 0,
    BUSID_SER2,
    BUSID_I2C1,
    BUSID_SPI1,
    BUSID_MAX
};

// TEST TEST
// Check if this check is correct???? Does BUS_UDP1_xXBUFSIZE and BUS_UDP2_xXBUFSIZE have to be included in check
    #if ((BUS_SER1_TXBUFSIZE + BUS_SER1_RXBUFSIZE + BUS_SER2_TXBUFSIZE + BUS_SER2_RXBUFSIZE + BUS_UDP1_TXBUFSIZE + BUS_UDP1_RXBUFSIZE + BUS_UDP2_TXBUFSIZE + BUS_UDP2_RXBUFSIZE + BUS_I2C1_TXBUFSIZE + BUS_I2C1_RXBUFSIZE + BUS_SPI1_TXBUFSIZE + BUS_SPI1_RXBUFSIZE) > BUSBUFFER_SIZE)
    #error "Serial bus buffers too large, not enough space!"
    #endif


/**
 * The number of buses
 */
#define BUS_COUNT (BUSID_MAX)


/**
 * Structure containing buffer data for a single bus.
 * 
 * The "txCount" member gives the number of entries currently contained in the buffer. It is also used
 * to indicate when the buffer is emptey: txCount == 0.
 *
 * To test if the buffer is full, check if txCount == txBufSize
 *
 * DO NOT use putTx == putTx to test if buffer is empty! This will be true when the buffer is empty AND
 * when it is full!
 *
 */
typedef struct _BUS_BUFFER
{
    BYTE*   txBuf;      //Pointer to Transmit buffer
    BUFTYPE putTx;      //put pointer(offset) for TxBuffer, a value from 0-(txBufSize-1)
    BUFTYPE getTx;      //get pointer(offset) for TxBuffer, a value from 0-(txBufSize-1)
    BUFTYPE txCount;    //Number of bytes contained in the tx buffer
    BUFTYPE txBufSize;  //Size of tx buffer
    BYTE*   rxBuf;      //Pointer to Receive buffer
    BUFTYPE putRx;      //put pointer(offset) for RxBuffer, a value from 0-(rxBufSize-1)
    BUFTYPE getRx;      //get pointer(offset) for RxBuffer, a value from 0-(rxBufSize-1)
    BUFTYPE rxCount;    //Number of bytes contained in the rx buffer
    BUFTYPE rxBufSize;  //Size of rx buffer
} BUS_BUFFER;


/**
 * Structure containing bus data.
 *
 */
typedef struct _BUSES_INFO
{
    BUS_BUFFER  buf[BUS_COUNT];   //Array of BUS_BUFFER structures, one for each bus
    BYTE*       bufEnd;           //Pointer to last byte of buffer
    BYTE        stat[BUS_COUNT];  //Current Status, is a BUSSTAT_XXX flag
} BUS_INFO;


#if !defined(THIS_IS_BUSES_MODULE)
extern BUS_INFO   busInfo;
#endif

/**
 * Empty the given buses transmit buffer. Any data contained in the transmit buffer is lost.
 *
 * @param busId The bus whos transmit buffer has to be cleared. Is a BUSID_XXX variable
 */
#define busEmptyTxBuf(busId) {busInfo.buf[busId].getTx = busInfo.buf[busId].putTx = busInfo.buf[busId].txCount = 0;}

/**
 * Empty the given buses receive buffer. Any data contained in the receive buffer is lost.
 *
 * @param busId The bus whos receive buffer has to be cleared. Is a BUSID_XXX variable
 */
#define busEmptyRxBuf(busId) {busInfo.buf[busId].getRx = busInfo.buf[busId].putRx = busInfo.buf[busId].rxCount = 0;}

/**
 * Checks if the given bus's transmit buffer is empty - it has nothing to transmit.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns true if the given bus's TX buffer is empty. Else, returns false.
 */
#define busIsTxBufEmpty(busId) (busInfo.buf[busId].txCount == 0)

/**
 * Checks if the given bus's receive buffer is empty - it has no pending data in it's receive buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns true if the given bus's RX buffer is empty. Else, returns false.
 */
#define busIsRxBufEmpty(busId) (busInfo.buf[busId].rxCount == 0)


/**
 * Gets number of bytes available in Transmit buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns number of bytes available in Transmit buffer to send of peek.
 */
#define busGetTxBufCount(busId) (busInfo.buf[busId].txCount)


/**
 * Gets number of bytes available in Receive buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns number of bytes available to read or peek in Receive buffer.
 */
#define busGetRxBufCount(busId) (busInfo.buf[busId].rxCount)

/**
 * Checks if the given bus's transmit buffer contains data.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns true if the given bus's TX buffer has data.
 */
#define busTxBufHasData(busId) (busInfo.buf[busId].txCount != 0)


/**
 * Checks if the given bus's receive buffer contains data.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns true if the given bus's RX buffer has data.
 */
#define busRxBufHasData(busId) (busInfo.buf[busId].rxCount != 0)


/**
 * Checks if the given bus's transmit buffer is full - no more space for adding data.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns true if the given bus's TX buffer is full. Else, returns false.
 */
#define busIsTxBufFull(busId) (busInfo.buf[busId].txCount == busInfo.buf[busId].txBufSize)


/**
 * Checks if the given bus's receive buffer is full - no more space for adding data.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns true if the given bus's RX buffer is full. Else, returns false.
 */
#define busIsRxBufFull(busId) (busInfo.buf[busId].rxCount == busInfo.buf[busId].rxBufSize)


/**
 * Checks if the given bus's receive buffer still has space - more space for adding data.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns true if the given bus's RX buffer is not full. Else, returns false.
 */
#define busRxBufNotFull(busId) (busInfo.buf[busId].rxCount != busInfo.buf[busId].rxBufSize)


/**
 * Gets a byte from the TX Buffer, without removing it. The byte is NOT removed from the
 * buffer, and the buffer pointers are NOT updated! To remove the byte, and update the
 * pointers, call the busRemoveByteTxBuf() function.
 *
 * @preCondition busIsTxBufEmpty() must have been called to confirm the TX Buffer has data!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns the next byte in the given bus's TX buffer.
 */
#define busPeekByteTxBuf(busId) (*(busInfo.buf[busId].txBuf + busInfo.buf[busId].getTx))


/**
 * Gets a byte from the RX Buffer, without removing it. The byte is NOT removed from the
 * buffer, and the buffer pointers are NOT updated! To remove the byte, and update the
 * pointers, call the busRemoveByteRxBuf() function.
 *
 * @preCondition busIsRxBufEmpty() must have been called to confirm the RX Buffer has data!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns the next byte in the given bus's RX buffer.
 */
#define busPeekByteRxBuf(busId) (*(busInfo.buf[busId].rxBuf + busInfo.buf[busId].getRx))


/**
 * Gets a byte at the given offset from the Transmit Buffer, without removing it.
 * The byte is NOT removed from the buffer, and the buffer pointers are NOT updated! 
 * The byte at the given offset it returned. The offset is how deep the byte is in
 * the buffer. For example, 0 will return first byte in buffer, 5 will return the 6th
 * byte in the buffer.
 *
 * @preCondition Ensure offset parameter is not larger than current number of bytes
 * contained in buffer. Call busGetTxBufCount() to get current number of bytes in buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 * @param offset Offset of byte to return. Is a value from 0-n, where n = (busGetTxBufCount() - 1)
 *
 * @return Returns the byte at the given offset in the given bus's Transmit buffer.
 */
BYTE busPeekByteTxBufAt(BYTE busId, BYTE offset);

/**
 * Gets a byte at the given offset from the Receive Buffer, without removing it.
 * The byte is NOT removed from the buffer, and the buffer pointers are NOT updated! 
 * The byte at the given offset it returned. The offset is how deep the byte is in
 * the buffer. For example, 0 will return first byte in buffer, 5 will return the 6th
 * byte in the buffer.
 *
 * @preCondition Ensure offset parameter is not larger than current number of bytes
 * contained in buffer. Call busGetRxBufCount() to get current number of bytes in buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 * @param offset Offset of byte to return. Is a value from 0-n, where n = (busGetRxBufCount() - 1)
 *
 * @return Returns the byte at the given offset in the given bus's Receive buffer.
 */
BYTE busPeekByteRxBufAt(BYTE busId, BYTE offset);

/**
 * Removes a byte from the Transmit Buffer.
 *
 * @preCondition Before calling this function, it must be confirmed that the Transmit buffer has data.
 * The busIsTxBufEmpty() is an example function that can be used to do this check.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 */
#define busRemoveByteTxBuf(busId) { if((++busInfo.buf[busId].getTx)==busInfo.buf[busId].txBufSize) {busInfo.buf[busId].getTx = 0;} busInfo.buf[busId].txCount--; /*Update available data left in tx buffer*/}


/**
 * Removes a byte from the Receive Buffer.
 *
 * @preCondition Before calling this function, it must be confirmed that the Receive buffer has data.
 * The busIsRxBufEmpty() is an example function that can be used to do this check.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 */
#define busRemoveByteRxBuf(busId) { if((++busInfo.buf[busId].getRx)==busInfo.buf[busId].rxBufSize) {busInfo.buf[busId].getRx = 0;} busInfo.buf[busId].rxCount--; /*Update available data left in rx buffer*/}


/**
 * Gets a byte from the Transmit Buffer, and removes it from buffer.
 *
 * @preCondition busIsTxBufEmpty() must have been called to confirm the TX Buffer has data!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns the next byte in the given bus's TX buffer.
 */
BYTE busGetByteTxBuf(BYTE busId);


/**
 * Gets a byte from the Receive Buffer, and removes it from buffer.
 *
 * @preCondition busIsRxBufEmpty() must have been called to confirm the RX Buffer has data!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns the next byte in the given bus's RX buffer.
 */
BYTE busGetByteRxBuf(BYTE busId);


/**
 * Adds a byte to the TX Buffer, and updates the buffer pointers.
 *
 * @preCondition busIsTxBufFull() must have been called to confirm the TX Buffer is not full!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 * @param b Byte to add to the buffer
 *
 */
#define busPutByteTxBuf(busId, b) {*(busInfo.buf[busId].txBuf + busInfo.buf[busId].putTx) = b; /* Write byte to buf */ if((++busInfo.buf[busId].putTx)==busInfo.buf[busId].txBufSize) {busInfo.buf[busId].putTx = 0;} busInfo.buf[busId].txCount++; /*One more byte available in buffer*/}


/**
 * Adds a byte to the RX Buffer, and updates the buffer pointers.
 *
 * @preCondition busIsRxBufFull() must have been called to confirm the RX Buffer is not full!
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 * @param b Byte to add to the buffer
 *
 */
#define busPutByteRxBuf(busId, b) {*(busInfo.buf[busId].rxBuf + busInfo.buf[busId].putRx) = b; /* Write byte to buf */ if((++busInfo.buf[busId].putRx)==busInfo.buf[busId].rxBufSize) {busInfo.buf[busId].putRx = 0;} busInfo.buf[busId].rxCount++; /*One more byte available in buffer*/}


/**
 * Reads a byte from the given bus's receive buffer. Ensure that the bus has data before calling
 * this function! The read byte is removed from the buses receive buffer.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return Returns the next byte from the given bus's RX buffer.
 */
//#define busGetByte(busId) (*busInfo.buf[busId].getRx;)


/**
 * Get the current status of this module. Use the busStatClearOverrun() function to clear
 * the overrun flags if required. This byte contains the following flags:
 * - BUSSTAT_TX_OVERRUN
 * - BUSSTAT_RX_OVERRUN
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return The status of this module. Consists out of SER_STAT_XXX flags
 */
#define busGetStat(busId)  (busInfo.stat[busId])


/**
 * Get the current status of this module. The following flags are cleared
 * during this function:
 * - BUSSTAT_TX_OVERRUN
 * - BUSSTAT_RX_OVERRUN
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 * 
 * @return The status of this module. Consists out of SER_STAT_XXX flags
 */
#define busStatClearOverrun(busId)  (busInfo.stat[busId] &= (~(BUSSTAT_TX_OVERRUN | BUSSTAT_RX_OVERRUN)) )


/**
 * Indicates if this bus is currently transmitting - returns the status of the BUSSTAT_TX flag.
 * For interrupt driven buses, this means that a transmit interrupt can occur at any time, and
 * clear this flag.
 *
 * When checking if the bus is transmitting, it should be remembered that a transmit interrupt can
 * occur at any time, and clear the trasmitting flag. This means that if this function returns true,
 * it could change to false (transmit interrupt occurs) by the time we receive the true reply.
 * To prevent this, this function should be called in a critical section, so that no interrupt can
 * occur while calling it.
 *
 * When checking if the bus is NOT transmitting, it does not have to be performed in a critical
 * section. Not transmitting, means no transmit interrupt can occur and change this flag.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return True if the given bus is currently transmitting, else false.
 */
#define busIsTxing(busId)  (busInfo.stat[busId] & BUSSTAT_TXING)


/**
 * Indicates if this bus is enabled.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 * @return True if the given bus is enabled, else false.
 */
#define busIsEnabled(busId)  (busInfo.stat[busId] & BUSSTAT_ENABLED)


/**
 * Enable the given bus.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 */
#define busEnable(busId)  (busInfo.stat[busId] |= BUSSTAT_ENABLED)


/**
 * Disaable the given bus.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 */
#define busDisabled(busId)  (busInfo.stat[busId] &= ~BUSSTAT_ENABLED)


/**
 * Service the given bus. If our code has a section where it has to wait for the transmit
 * buffer of a bus to be empties, it should call this function while waiting.
 *
 * @param busId The bus ID of the requested bus. Is a BUSID_XXX variable
 *
 */
void busService(BYTE busId);


/**
 * Initialization code.
 */
void busInit(void);


/**
 * Initializes the busInfo structure with pointers to all buffers.
 */
void busInfoInit(void);


/**
 * Initialize bus network components
 *
 * @preCondition Do NOT call this function before stackInit() has been called!
 */
void busNetInit(void);


//void busLoadBusConfig(

/**
 * Must be called every couple of ms
 *
 */
void busTask(void);


#endif
