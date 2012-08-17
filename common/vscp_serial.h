/**
* @brief           VSCP Serial Protocol
* @file            vscp_serial.h
* @author          Ake Hedman, eurosource, www.vscp.org
*
* @section description Description
**********************************
* This module contains the code that implements the
* VSCP serial protocol.
*********************************************************************/

/* ******************************************************************************
* VSCP (Very Simple Control Protocol) 
* http://www.vscp.org
*
* Copyright (C) 1995-2011 Ake Hedman, 
* eurosource, <akhe@eurosource.se>
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
* 
*	This file is part of VSCP - Very Simple Control Protocol 	
*	http://www.vscp.org
*
* ******************************************************************************
*/

#ifndef VSCP_SERIAL_H
#define VSCP_SERIAL_H

#ifdef WIN32
#include "inttypes.h"
#else
#include <inttypes.h>
#endif

// This file should be placed in working folder and
// hold all switches for the the actual serial 
// protocol implementation.
#include "vscp_projdefs.h"

#ifndef BOOL
#define BOOL		int 
#endif

#ifndef FALSE
#define FALSE		0 
#endif

#ifndef TRUE
#define TRUE		!FALSE
#endif

// Special characters
#define DLE   0x10  // Escape character
#define STX   0x01  // Start of 'text'
#define ETX   0x02  // End of 'text'

#define VSCP_SERIAL_FRAME_SIZE    23     // Max frame size
#define VSCP_SERIAL_BUFFER_SIZE   VSCP_SERIAL_FRAME_SIZE * 2

// Protocol state machine
enum {
    VSCP_SERIAL_PROTOCOL_STATE_NONE,      // Wait for packet.
    VSCP_SERIAL_PROTOCOL_STATE_PACKET,    // DLE STX received 
    VSCP_SERIAL_PROTOCOL_STATE_ETX,
};

enum {
    VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE,   // No Escape wait
    VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE  // Escape character received
};

// Predefined serial VSCP opcodes
#define VSCP_SERIAL_OPCODE_NOOP                 0
#define VSCP_SERIAL_OPCODE_LEVEL1_EVENT         1
#define VSCP_SERIAL_OPCODE_LEVEL2_EVENT         2
#define VSCP_SERIAL_OPCODE_POLL_EVENT           3
#define VSCP_SERIAL_OPCODE_NO_EVENT             4
#define VSCP_SERIAL_OPCODE_CAN_FRAME            5
#define VSCP_SERIAL_OPCODE_RESERVED_DLE         10
#define VSCP_SERIAL_OPCODE_SENT_ACK             249
#define VSCP_SERIAL_OPCODE_SENT_NACK            250
#define VSCP_SERIAL_OPCODE_ACK                  251
#define VSCP_SERIAL_OPCODE_NACK                 252
#define VSCP_SERIAL_OPCODE_ERROR                253
#define VSCP_SERIAL_OPCODE_REPLY				254
#define VSCP_SERIAL_OPCODE_COMMAND              255

// CAN flag bits (uses data byte 0-3 for id and 
// bit 32 is Extended id, bit 31 RTR
#define VSCP_SERIAL_CAN_DATA0_EXTENDED			0x80
#define VSCP_SERIAL_CAN_DATA0_RTR				0x40

// The serial frame
typedef struct {
    uint8_t opcode;
    uint8_t flags;		// bit0-2 data count, bit3 class bit 9 
    uint8_t channel;
    uint8_t seqnumber;
    uint8_t vscpclass;
    uint8_t vscptype;
    uint8_t data[ 16 ];
} vs_frame;

#ifdef VSCP_SERIAL_ENABLE_SLIDING_WINDOWS 

// Sliding window frame structure

typedef struct {
    uint32_t timestamp;				// Time when frame sent/received
									// Zero indicate unused entry
    vs_frame frame;               	// frame
    uint8_t send_count;           	// # times theframe has been sent
} vs_swFrameStruct;



typedef struct {
    uint8_t cntOutstanding;			// # send but not ACK'd frames
    //  can be max size_window
    uint32_t maxtime;             	// maxtime waiting for ACK before resend
    uint8_t maxresend;            	// Max # resends before giving up

    vs_swFrameStruct Frames[ VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ];  // Pending frames
} vs_swStruct;

#endif


#ifdef __cplusplus
extern "C" {
#endif


	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// The follwing is external functions and should be implemented
    // by the program that use this code
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    /*!
		Send a character on the serial line. If its better to send a chunk of data
		just collect data in this routine and do the actual send with vs_sendFrame.
		\param b Byte to send.
		\return TRUE on success, FALSE on failure. 
    */
    BOOL vs_sendSerialByte( uint8_t b );

    /*!
		Send content in buffer.
		If its better to send larger chunks of data such as a frame use this call to 
		send the data that has been collected with the vs_sendSerialByte. Do not
		implement this function if vs_sendSerialByte actually send the data.
		\param pbuf buffer to send
		\param count Pointer to number of bytes to send. 
		\return TRUE on success, FALSE on failure. 
    */
    BOOL vs_sendBuffer( uint8_t *pbuf, uint16_t count );





    ///////////////////////////////////////////////////////////////////////////////
    //                            Protocol API
    ///////////////////////////////////////////////////////////////////////////////
	

    /*!
		Buffer serial byte without DLE substitution
		\param b Byte to send
    */
    void vs_bufferSerialByte( uint8_t b );

    /*!
		Send serial byte with DLE handling
		\param psum Pointer to checksum
		\param b Byte to send
    */
    BOOL vs_bufferSerialByteSubst( uint8_t *pcsum, uint8_t b ); 


    /*!
		vscp_serial_init
    */
    void vs_init( void );

    /*!
		vscp_serial_feed
		\param b Byte to feed the state machine with
		\return True when a valid event is received.
    */
    BOOL vs_feed( uint8_t b );

    /*!
		Get a serial fr5ame from the buffer
		\param pFrame Pointer to frame that will receive data
		\return TRUE och success. FALSE on failure.
    */
    BOOL vs_getFrame( vs_frame *pFrame );

    /*!
		Send ACK/NACK response
		\param opcode Either ACK or NACK opcode.
		\param channel Channel number ACK is sent for.
		\param seqnumber Id for packet which is ACK'ed
		\return TRUE och success. FALSE on failure.
    */
    BOOL vs_sendResponse( uint8_t opcode, 
							uint8_t channel, 
							uint8_t seqnumber );

    /*!
		Send ACK
		\param channel Channel number ACK is sent for.
		\param seqnumber Id for packet whcih is ACK'ed
		\return TRUE och success. FALSE on failure.
    */
    BOOL vs_sendAck( uint8_t channel, uint8_t seqnumber );

    /*!
		Send NACK
		\param channel Channel number NACK is sent for.
		\param seqnumber Id for packet which is NACK'ed
		\return TRUE och success. FALSE on failure.
    */
    BOOL vs_sendNack( uint8_t channel, uint8_t seqnumber );

    /*!
		Send serial frame
		\param pFrame Pointer to serial frame structure
		\return TRUE och success. FALSE on failure.
    */
    BOOL vs_sendFrame( vs_frame *pFrame );


    /*!
		Send command frame
		First byte of data is command code
		\param channel Channel number NACK is sent for.
		\param seqnumber Id for packet which is NACK'ed
		\param command Command code.
		\param count  Number of databytes (0-16)
		\param pData Pointer to data.
		\return TRUE och success. FALSE on failure.
    */
    BOOL vs_sendCommandFrame( uint8_t channel, 
								uint8_t seqnumber, 
								uint16_t command,
								uint8_t count,  
								uint8_t *pData  );

    /*!
		Send error frame
		First byte of data is error code
		\param channel Channel number NACK is sent for.
		\param seqnumber Id for packet which is NACK'ed
		\param count  Number of databytes (0-16)
		\param pData Pointer to data.
		\return TRUE och success. FALSE on failure.
    */
    BOOL vs_sendErrorFrame( uint8_t channel, 
								uint8_t seqnumber, 
								uint8_t count,  
								uint8_t *pData  );




#ifdef VSCP_SERIAL_ENABLE_SLIDING_WINDOWS 

    /*!
		Initialize the sliding window structure.
		\param pSlide Pointer to the sliding window struccture.
		\param timeout Timeout value in milliseconds.
		\param resend Number of times to resend a frame before giving up.
    */
    void vs_initSlidingWndFrames( vs_swStruct *pSlide, 
									uint32_t timeout,
									uint8_t resend );

    /*!
		Find a specific frame
		\param pSlide Pointer to the sliding window struccture.
		\param channel The channel for the frame.
		\param seqnumber Rge sequency number for the frame.
		\return Found frame structure or NULL if not found.
    */
    vs_swFrameStruct *vs_findFlyingFrame( vs_swStruct *pSlide, 
											uint8_t channel, 
											uint8_t seqnumber );

    /*!
		Find a position that is not used.
		\param pSlide Pointer to the sliding window struccture
		\return Frame structure or NULL if no free location found.
    */

    vs_swFrameStruct *vs_findFreeFlyingFrame( vs_swStruct *pSlide  );

    /*!
		Add sliding window frame
		Frames get added to the structure while waiting for ACK's 
		for them.
    */
    BOOL vs_addSlidingWndFrame( vs_swStruct *pSlide,
									vs_frame *pFrame,
									uint32_t now ); 

    /*!
		Remove a sliding window frame
		When an ACK is received for a frame it is removed from 
		the table.
    */
    BOOL vs_removeSlidingWndFrame( vs_swStruct *pSlide, 
									uint8_t channel, 
									uint8_t seqnumber );

    /*!
		Check if a frame needs to be retransmitted
		This function should be called on regular intervals to
		resend frames that has not got ACK's and to remove frames
		that has been retransmitted to many times.
    */
    void vs_timeoutSlidingWndFrame( vs_swStruct *pSlide, 
                                        uint32_t now );

#endif // Sliding Window


#ifdef __cplusplus
}
#endif


#endif // VSCP_SERIAL_H