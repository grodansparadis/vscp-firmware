/**
* @brief           VSCP Serial Protocol
* @file            vscp_serial.c
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
*  This file is part of VSCP - Very Simple Control Protocol 	
*  http://www.vscp.org
*
* ******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include "vscp_serial.h"
//#include "c:/development/m2m/firmware/common/vscp_serial.h"
//#include "vscp_firmware.h"

#include "../../src/common/crc.h"
#else

#endif

// Globals
uint8_t vs_main_state;     // state for the state machine
uint8_t vs_sub_state;      // substate for the state machine
uint8_t vs_msgbuffer[ VSCP_SERIAL_BUFFER_SIZE ];  // Buffer for current event 
uint8_t vs_buffer_cnt;     // Character count for buffer
uint8_t vs_checksum;       // Packet checksum

#ifdef VSCP_SERIAL_ENABLE_SLIDING_WINDOWS 

// Flying windows frames
uint8_t vs_cntSlidingWndFrames;   // Number of outstanding sliding window frames

// Array with info about outstanding flying window frames 
//vscp_serial_SlidingWndStruct slidingWndFrames[ VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ];   

#endif



///////////////////////////////////////////////////////////////////////////////
// vs_sendchar_subst
//

void vs_bufferSerialByte( uint8_t b )
{
    if ( vs_buffer_cnt >= VSCP_SERIAL_BUFFER_SIZE ) return;
    vs_msgbuffer[ vs_buffer_cnt++ ] = b;
}


///////////////////////////////////////////////////////////////////////////////
// vs_sendchar_subst
//

BOOL vs_bufferSerialByteSubst( uint8_t *pcsum, uint8_t b )
{
    if ( DLE == b ) {
        vs_bufferSerialByte( DLE );
        vs_bufferSerialByte( DLE );
        *pcsum ^= DLE; // Only one is used for checksum calculations
    }
    else {
        vs_bufferSerialByte( b );	
        *pcsum ^= b;
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// vscp_serial_init
//

void vs_init( void ) 
{
    vs_main_state = VSCP_SERIAL_PROTOCOL_STATE_NONE;
    vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE; 
    vs_buffer_cnt = 0;
    vs_checksum = 0;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_serial_feed
//

BOOL vs_feed( uint8_t b )
{
    BOOL rv = FALSE;

    switch ( b ) {

    case DLE: // escape
        if ( VSCP_SERIAL_PROTOCOL_STATE_NONE == vs_main_state ) {
            vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE;  
        }
        else if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {

            if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {
                // Second DLE => it's a DLE character
                vs_checksum ^= DLE;
                vs_msgbuffer[ vs_buffer_cnt++ ] = DLE;
                vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE; 
            }
            else {
                vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE;
            }

        }
        break;

    case STX: // start
        if ( VSCP_SERIAL_PROTOCOL_STATE_NONE == vs_main_state ) {

            if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {
                // Here if waiting for packet and escape has been received.
                // Start to receive a package    
                vs_init();
                vs_main_state = VSCP_SERIAL_PROTOCOL_STATE_PACKET; 
            }

        }
        else if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {

            if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {
                // DLE STX in package - Start all over
                vs_init();
            }
            else if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE == vs_sub_state ) {
                // Just a stray STX
                vs_checksum ^= b;
                vs_msgbuffer[ vs_buffer_cnt++ ] = b; 
            }

        }
        break;

    case ETX: // End
        if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {

            if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_ESCAPE == vs_sub_state ) {

                // DLE ETX in package - We have received a packet
                // Check the packet checksum and return OK

                if ( 0 == vs_checksum ) {
                    rv = TRUE;
                    // Ack the frame
                    if (  ( VSCP_SERIAL_OPCODE_ACK != vs_msgbuffer[ 0 ] ) && 
                        ( VSCP_SERIAL_OPCODE_SENT_ACK != vs_msgbuffer[ 0 ] ) ) {
                            //vs_sendAck( vs_msgbuffer[ 2 ], vs_msgbuffer[ 3 ] );
                    }
                }
                else {
                    // Nack the frame
                    if (  ( VSCP_SERIAL_OPCODE_NACK != vs_msgbuffer[ 0 ] ) && 
                        ( VSCP_SERIAL_OPCODE_SENT_NACK != vs_msgbuffer[ 0 ] ) ) {
                            //vs_sendNack( vs_msgbuffer[ 2 ], vs_msgbuffer[ 3 ] );
                    }
                }

                vs_init();

            }
            else if ( VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE == vs_sub_state ) {
                // Just a stray ETX
                vs_checksum ^= b;
                vs_msgbuffer[ vs_buffer_cnt++ ] = b; 
            }

        }
        break;

    default:
        // Receiving packet
        if ( VSCP_SERIAL_PROTOCOL_STATE_PACKET == vs_main_state ) {
            vs_checksum ^= b;
            vs_msgbuffer[ vs_buffer_cnt++ ] = b;
        }
        break;
    }

    // Check for buffer overflow
    if ( vs_buffer_cnt > VSCP_SERIAL_FRAME_SIZE ) {
        // Start all over
        vs_main_state = VSCP_SERIAL_PROTOCOL_STATE_NONE;
        vs_sub_state = VSCP_SERIAL_PROTOCOL_SUBSTATE_NONE;
    }

    return rv;
}


///////////////////////////////////////////////////////////////////////////////
// vs_getFrame
// 

BOOL vs_getFrame( vs_frame *pFrame )
{     
    // Check pointer
    if ( NULL == pFrame ) return FALSE; 

    pFrame->opcode = vs_msgbuffer[ 0 ];
    pFrame->flags = vs_msgbuffer[ 1 ];
    pFrame->channel = vs_msgbuffer[ 2 ];
    pFrame->seqnumber = vs_msgbuffer[ 3 ];
    pFrame->vscpclass = vs_msgbuffer[ 4 ];
    pFrame->vscptype = vs_msgbuffer [ 5 ];
    memcpy( pFrame->data, ( vs_msgbuffer + 6 ), 16 );

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// vs_sendResponse
//
// Sent inline so it can be used inside frame feed routine
//

BOOL vs_sendResponse( uint8_t opcode, uint8_t channel, uint8_t seqnumber )
{
    uint8_t checksum = 0;

    // Start of frame
    if ( !vs_sendSerialByte( DLE ) ) {
        return FALSE;
    }
    if ( !vs_sendSerialByte( STX ) ) {
        return FALSE;
    }

    // opcode Code
    if ( !vs_sendSerialByte( opcode ) ) {
        return FALSE;
    }
    checksum ^= opcode;

    // Flags
    if ( !vs_sendSerialByte( 0 ) ) {
        return FALSE;
    }
    checksum ^= 0;

    // Channel
    if ( DLE == channel ) {
        vs_sendSerialByte( DLE );
        vs_sendSerialByte( DLE );
        checksum ^= DLE; // Only one is used for checksum calculations
    }
    else {
        vs_sendSerialByte( channel );	
        checksum ^= channel;
    }

    // Counter
    if ( DLE == seqnumber ) {
        vs_sendSerialByte( DLE );
        vs_sendSerialByte( DLE );
        checksum ^= DLE; // Only one is used for checksum calculations
    }
    else {
        vs_sendSerialByte( seqnumber );	
        checksum ^= seqnumber;
    }

    // Checksum
    if ( DLE == checksum ) {
        vs_sendSerialByte( DLE );
        vs_sendSerialByte( DLE );
    }
    else {
        vs_sendSerialByte( checksum );	
    }

    // End of frame
    if ( !vs_sendSerialByte( DLE ) ) {
        return FALSE;
    }
    if ( !vs_sendSerialByte( ETX ) ) {
        return FALSE;
    }

    return TRUE; 		
}

///////////////////////////////////////////////////////////////////////////////
// vs_sendAck
//
// Sent inline so it can be used inside frame feed routine
//

BOOL vs_sendAck( uint8_t channel, uint8_t seqnumber )
{
    return vs_sendResponse( VSCP_SERIAL_OPCODE_ACK,
        channel,
        seqnumber );
}

///////////////////////////////////////////////////////////////////////////////
// vs_sendNack
//             

BOOL vs_sendNack( uint8_t channel, uint8_t seqnumber )
{
    return vs_sendResponse( VSCP_SERIAL_OPCODE_NACK,
        channel,
        seqnumber );
}

///////////////////////////////////////////////////////////////////////////////
// vs_sendFrame
//             

BOOL vs_sendFrame( vs_frame *pFrame )
{
    int i;
    uint8_t checksum = 0;

    vs_buffer_cnt = 0;

    // Check pointer
    if ( NULL == pFrame ) return FALSE; 

    // Start of frame
    vs_bufferSerialByte( DLE );
    vs_bufferSerialByte( STX );

    // Operation     
    vs_bufferSerialByteSubst( &checksum, pFrame->opcode );    

    // Flags     
    vs_bufferSerialByteSubst( &checksum, pFrame->flags ); 

    // Channel
    vs_bufferSerialByteSubst( &checksum, pFrame->channel );

    // Counter
    vs_bufferSerialByteSubst( &checksum, pFrame->seqnumber );

    // VSCP Class
    vs_bufferSerialByteSubst( &checksum, pFrame->vscpclass );

    //  VSCP type
    vs_bufferSerialByteSubst( &checksum, pFrame->vscptype );

    // data
    for ( i=0; i < ( pFrame->flags & 0x1f ); i++ ) {
        vs_bufferSerialByteSubst( &checksum, pFrame->data[ i ] );
    }

    //  Checksum - Note: checksum byte(s) sent before checksum calcuulated so OK
    vs_bufferSerialByteSubst( &checksum, checksum );

    // End of frame
    vs_bufferSerialByte( DLE );
    vs_bufferSerialByte( ETX );     

    // Send the buffer content
    if ( !vs_sendBuffer(  vs_msgbuffer, vs_buffer_cnt  ) ) {
        vs_buffer_cnt = 0;
        return FALSE;
    }

    vs_buffer_cnt = 0;

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// vs_sendCommandFrame
//             

BOOL vs_sendCommandFrame( uint8_t channel, 
                         uint8_t seqnumber, 
                         uint16_t command,
                         uint8_t count,  
                         uint8_t *pData  )
{
    int i;
    uint8_t checksum = 0;

    vs_buffer_cnt = 0;

    // Check pointer - Must be at least command byte
    if ( NULL == pData ) return FALSE;

    count &= 0x1f;  // Max 16 bytes

    // Start of frame
    vs_bufferSerialByte( DLE );
    vs_bufferSerialByte( STX );

    // ACK Code
    vs_bufferSerialByteSubst( &checksum, VSCP_SERIAL_OPCODE_COMMAND );

    // Flags: Data byte count
    vs_bufferSerialByteSubst( &checksum, count );

    // Channel
    vs_bufferSerialByteSubst( &checksum, channel );

    // Command
    vs_bufferSerialByteSubst( &checksum, ( command & 0xff ) );
    vs_bufferSerialByteSubst( &checksum, ( ( command >> 8 ) & 0xff ) );

    // Counter
    vs_bufferSerialByteSubst( &checksum, seqnumber );

    for ( i = 0; i<count; i++ ) {
        vs_bufferSerialByteSubst( &checksum, pData[ i ] );
    }

    // Checksum
    vs_bufferSerialByteSubst( &checksum, checksum );

    // End of frame
    vs_bufferSerialByte( DLE );
    vs_bufferSerialByte( ETX );

    // Send the buffer content
    if ( !vs_sendBuffer(  vs_msgbuffer, vs_buffer_cnt  ) ) {
        vs_buffer_cnt = 0;
        return FALSE;
    }

    vs_buffer_cnt = 0;

    return TRUE;  
}


///////////////////////////////////////////////////////////////////////////////
// vs_sendErrorFrame
//             

BOOL vs_sendErrorFrame( uint8_t channel, 
                       uint8_t seqnumber, 
                       uint8_t count,  
                       uint8_t *pData  )
{
    int i;
    uint8_t checksum = 0;

    vs_buffer_cnt = 0;

    // Check pointer - must at least be error byte
    if ( NULL == pData ) return FALSE;

    count &= 0x1f;  // Max 16 bytes

    // Start of frame
    vs_bufferSerialByte( DLE );
    vs_bufferSerialByte( STX );

    // ACK Code
    vs_bufferSerialByteSubst( &checksum, VSCP_SERIAL_OPCODE_ERROR );

    // Flags: Data byte count
    vs_bufferSerialByteSubst( &checksum, count );

    // Channel
    vs_bufferSerialByteSubst( &checksum, channel );

    // Counter
    vs_bufferSerialByteSubst( &checksum, seqnumber );

    for ( i = 0; i<count; i++ ) {
        vs_bufferSerialByteSubst( &checksum, pData[ i ] );
    }

    // Checksum
    vs_bufferSerialByteSubst( &checksum, checksum );

    // End of frame
    vs_bufferSerialByte( DLE );
    vs_bufferSerialByte( ETX );

    // Send the buffer content
    if ( !vs_sendBuffer(  vs_msgbuffer, vs_buffer_cnt  ) ) {
        vs_buffer_cnt = 0;
        return FALSE;
    }

    vs_buffer_cnt = 0;

    return TRUE;  
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



#ifdef VSCP_SERIAL_ENABLE_SLIDING_WINDOWS 


///////////////////////////////////////////////////////////////////////////////
// vs_initSlidingWndFrames
//

void vs_initSlidingWndFrames( vs_swStruct *pSlide, 
                                uint32_t timeout,
                                uint8_t resend ) 
{
    int i;

    for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        pSlide->Frames[ i ].timestamp = 0;
        pSlide->Frames[ i ].send_count = 0;
    }

    pSlide->cntOutstanding = 0;
    pSlide->maxresend = resend;
    pSlide->maxtime = timeout;
}



///////////////////////////////////////////////////////////////////////////////
// vs_findFlyingFrame
//

vs_swFrameStruct *
vs_findFlyingFrame( vs_swStruct *pSlide, 
                   uint8_t channel, 
                   uint8_t seqnumber ) 
{
    int i;

    for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        if ( ( channel == pSlide->Frames[ i ].frame.channel ) &&
            ( seqnumber == pSlide->Frames[ i ].frame.seqnumber ) ) {
                return &( pSlide->Frames[ i ] );      
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// vs_findFreeFlyingFrame
//

vs_swFrameStruct *
vs_findFreeFlyingFrame( vs_swStruct *pSlide ) 
{
    int i;

    if ( pSlide->cntOutstanding >= VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ) {
        return NULL;
    }

    for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        if ( ( 0 == pSlide->Frames[ i ].timestamp ) &&
            ( 0 == pSlide->Frames[ i ].send_count ) ) {
                return &( pSlide->Frames[ i ] );      
        }
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// vs_addSlidingWndFrame
//

BOOL vs_addSlidingWndFrame( vs_swStruct *pSlide,
                           vs_frame *pFrame,
                           uint32_t now ) 
{
    int i;

    // Check pointer
    if ( NULL == pSlide ) return FALSE;

    // Check if there is room for another frame
    if ( pSlide->cntOutstanding >= VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES ) return FALSE;

    for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        if ( 0 == pSlide->Frames[ i ].timestamp   ) {
            memcpy( &pSlide->Frames[ i ].frame,
                pFrame,
                sizeof( vs_frame  ) );
            pSlide->Frames[ i ].timestamp = now;
            pSlide->Frames[ i ].send_count = 0;
            pSlide->cntOutstanding++; // Another frame added
            return TRUE;
        }
    }  

    // No room for a new frame found - strange!!!
    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// vs_removeSlidingWndFrame
//

BOOL vs_removeSlidingWndFrame( vs_swStruct *pSlide, 
                              uint8_t channel, 
                              uint8_t seqnumber ) 
{
    int i;

    for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {
        if ( ( 0 != pSlide->Frames[ i ].timestamp ) &&
            ( channel == pSlide->Frames[ i ].frame.channel ) &&
            ( seqnumber == pSlide->Frames[ i ].frame.seqnumber ) ) {
                pSlide->Frames[ i ].timestamp = 0;
                pSlide->Frames[ i ].send_count = 0;
                pSlide->cntOutstanding--; // Another frame removed
                return TRUE;      
        }
    }  

    return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// vs_timeoutSlidingWndFrame
//

void vs_timeoutSlidingWndFrame( vs_swStruct *pSlide, 
                                    uint32_t now ) 
{
    int i;
    uint32_t diff;

    for ( i=0; i<VSCP_SERIAL_MAX_SLIDING_WINDOW_FRAMES; i++ ) {

        // Skip unused positions
        if ( 0 == pSlide->Frames[ i ].timestamp ) continue;

        diff = now - pSlide->Frames[ i ].timestamp;
        if ( diff > pSlide->maxtime  ) {
            if ( pSlide->Frames[ i ].send_count > pSlide->maxresend ) {
                // Abort sending of this frame
                vs_removeSlidingWndFrame( pSlide, 
                    pSlide->Frames[ i ].frame.channel,
                    pSlide->Frames[ i ].frame.seqnumber );
            }
            else {
                // Retransmit the frame
                if ( vs_sendFrame( &pSlide->Frames[ i ].frame ) ) {
                    pSlide->Frames[ i ].timestamp = now;
                    pSlide->Frames[ i ].send_count++;
                }
            }
        }
    }  
}


#endif  // Sliding winow