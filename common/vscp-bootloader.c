///////////////////////////////////////////////////////////////////////////////
// File: vscp_bootloader.c
// 
// Implements the VSCP bootloader
//

/* ******************************************************************************
 *  VSCP (Very Simple Control Protocol) 
 *  http://www.vscp.org
 *
 *  The MIT License (MIT)
 *
 *  Copyright (c) 2010-2016 Sven Zwiers
 *  Copyright (c) 2016-2018 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sub license, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
 */

#include <stdio.h>
#include <inttypes.h>
#include <vscp-firmware.h>
#include <vscp_class.h>
#include <vscp_type.h>
#include <crc.h>
#include "hardware.h"
#include "vscp-bootloader.h"

// Globals
volatile uint16_t vscpboot_timer = 0;
uint8_t vscpboot_nickname = 0xFE;       // Assigned node nickname

void vscpboot_init()
{
    //Initialize CRC lookup table.
    crcInit();    
}

///////////////////////////////////////////////////////////////////////////////
// vscpboot_loader
//

void vscpboot_loader( void )
{
    vscpevent e;
    vscpboot_state state = STATE_VIRGIN;        // We haven't done anything yet
    
    uint32_t blockno;                           // Current block to program
    uint8_t blockdata[ VSCPBOOT_BLOCKSIZE ];    // Block data buffer
    uint16_t offset;                            // offset into blockdata 
    
    while ( 1 ) {     // not only diamonds are forever...
        
        switch ( state ) {
        
            // ----------------------------------------------------------------
            case STATE_VIRGIN:
                
                // Say hello to the world
                e.flags = 1; // one data byte
                e.priority = VSCP_PRIORITY_NORMAL;
                e.vscp_class = VSCP_CLASS1_PROTOCOL;
                e.vscp_type = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
                e.data[ 0 ] = vscpboot_nickname;
                vscpboot_sendEvent( &e );
                
                vscpboot_timer = 0;
                state =  STATE_ANNOUNCE;
                
                break;
               
            // ----------------------------------------------------------------    
            case STATE_ANNOUNCE:
                if ( vscpboot_timer > TIMOUT_ANNOUNCE ) {
                    
                    // OK no response
                    state = STATE_BOOTWAIT;
                    
                }
                else {
                   
                    if ( vscpboot_getEvent( &e ) ) {
                        
                        if ( ( e.vscp_class == VSCP_CLASS1_PROTOCOL ) &&
                                ( e.vscp_type == VSCP_TYPE_PROTOCOL_PROBE_ACK ) ) {
                                // We have problems. Another node ACK'ed or nickname
                                // we can't continue - Go into eternety
                                while ( 1 );                                
                        }
                        
                    }
                }                    
                break; 
                
            // ----------------------------------------------------------------    
            case STATE_BOOTWAIT:
            
                if ( vscpboot_getEvent( &e ) ) {

ENTER_BOOT_MODE:
              
                    if ( ( e.vscp_class == VSCP_CLASS1_PROTOCOL ) &&
                            ( e.vscp_type == VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER ) && 
                            ( 8 == ( e.flags & 0x0F ) ) &&                 // Eight data bytes
                            ( vscpboot_nickname == e.data[ 0 ] ) &&     // Our nickname
                            ( VSCP_BOOTLOADER_VSCP == e.data[ 1 ] ) &&  // VSCP boot algorithm
                            ( vscpboot_getGUID( 0 ) == e.data[ 2 ] ) && // GUID 0
                            ( vscpboot_getGUID( 3 ) == e.data[ 3 ] ) && // GUID 3
                            ( vscpboot_getGUID( 5 ) == e.data[ 4 ] ) && // GUID 5
                            ( vscpboot_getGUID( 7 ) == e.data[ 5 ] ) && // GUID 7    
                            ( ( ( vscpboot_getPage() >> 8 ) & 0xFF ) == e.data[ 6 ] ) && // MSB of page
                            ( ( vscpboot_getPage() & 0xff ) == e.data[ 7 ] )             // LSB of page same
                            ) {
                                
                        // OK we have a valid request
                        e.priority = VSCP_PRIORITY_NORMAL;
                        e.flags = VSCP_VALID_MSG + 8;
                        e.vscp_class = VSCP_CLASS1_PROTOCOL;
                        e.vscp_type =  VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER;
                        e.data[ 0 ] = 0x00;
                        e.data[ 1 ] = 0x00;
                        e.data[ 2 ] = ( VSCPBOOT_BLOCKSIZE >> 8 ) & 0xFF;
                        e.data[ 3 ] = ( VSCPBOOT_BLOCKSIZE & 0xFF);
                        e.data[ 4 ] = 0x00;
                        e.data[ 5 ] = 0x00;
                        e.data[ 6 ] = ( VSCPBOOT_BLOCKS >> 8 ) & 0xFF;
                        e.data[ 7 ] = ( VSCPBOOT_BLOCKS & 0xFF );
                        if ( vscpboot_sendEvent( &e ) ) {	// ACK bootloader request                       
                            state = STATE_BLOCKWAIT;
                        }   
                                                                     
                    }
                    else {
                        
                        // Not a valid boot loader entry request - We stay in state
                        e.priority = VSCP_PRIORITY_NORMAL;
                        e.flags = 0;
                        e.vscp_class = VSCP_CLASS1_PROTOCOL;
                        e.vscp_type =  VSCP_TYPE_PROTOCOL_NACK_BOOT_LOADER;
                        vscpboot_sendEvent( &e );	// NACK bootloader request
                        
                    }
                    
                }                              
                break;  
                
            // ----------------------------------------------------------------    
            case STATE_BLOCKWAIT:
                if ( vscpboot_getEvent( &e ) ) {

                    if ( ( e.vscp_class == VSCP_CLASS1_PROTOCOL ) &&
                            ( e.vscp_type == VSCP_TYPE_PROTOCOL_START_BLOCK ) &&
                            ( ( e.flags & 0x0F ) >= 4 ) ) { 
                                  
                        blockno =  construct_unsigned32( e.data[0],
                                                         e.data[1],
                                                         e.data[2],
                                                         e.data[3] );
                                                         
                        // block number can't be larger than number of available blocks                                                         
                        if ( blockno >= VSCPBOOT_BLOCKS ) {
                            e.priority = VSCP_PRIORITY_NORMAL;
                            e.flags = 0;
                            e.vscp_class = VSCP_CLASS1_PROTOCOL;
                            e.vscp_type =  VSCP_TYPE_PROTOCOL_START_BLOCK_NACK;
                            vscpboot_sendEvent( &e );	// NACK start block request                        
                        }                            
                        else {            
                            
                            // ACK the block data
                            e.priority = VSCP_PRIORITY_NORMAL;
                            e.flags = 0;
                            e.vscp_class = VSCP_CLASS1_PROTOCOL;
                            e.vscp_type =  VSCP_TYPE_PROTOCOL_START_BLOCK_ACK;
                                                 
                            if ( vscpboot_sendEvent( &e ) ) {	// ACK program block request
                                memset( blockdata, 0xFF, sizeof( blockdata ) );
                                state = STATE_BLOCKDATA;
                                offset = 0;
                            }
                             
                        }                                                                            
                    }
                    else {
                        // Is this an abort?
                        if ( ( e.vscp_class == VSCP_CLASS1_PROTOCOL ) &&
                        ( e.vscp_type == VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER ) ) {
                            state = STATE_BOOTWAIT;
                            goto  ENTER_BOOT_MODE;
                        }
                    }                       
                }
                                                        
                break; 
                
            // ----------------------------------------------------------------
            case STATE_BLOCKDATA:
            if ( vscpboot_getEvent( &e ) ) { 
                
                if ( ( e.vscp_class == VSCP_CLASS1_PROTOCOL ) &&
                    ( e.vscp_type == VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER ) && 
                    ( 8 == ( e.flags & 0x0F ) ) ) {

                    // copy in data                    
                    memcpy( blockdata + offset, e.data, 8 );
                    offset += 8;  
                    
                    // ACK the block data
                    e.priority = VSCP_PRIORITY_NORMAL;
                    e.flags = 0;
                    e.vscp_class = VSCP_CLASS1_PROTOCOL;
                    e.vscp_type =  VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK;                    
                    vscpboot_sendEvent( &e );                                          
                    
                    // Check if the block is full
                    if ( offset == sizeof( blockdata ) ) {
                     
                        // ACK the block 
                        
                        
                        
                        e.priority = VSCP_PRIORITY_NORMAL;
                        e.flags = 0;
                        e.vscp_class = VSCP_CLASS1_PROTOCOL;
                        e.vscp_type =  VSCP_TYPE_PROTOCOL_START_BLOCK_ACK;
                        
                        if ( vscpboot_sendEvent( &e ) ) {	// ACK program block request
                            memset( blockdata, 0xFF, sizeof( blockdata ) );
                            state = STATE_BLOCKDATA;
                            offset = 0;
                        }
                        
                    }
                    
                }                    
                
            }
            else {
                // Is this an abort?
                if ( ( e.vscp_class == VSCP_CLASS1_PROTOCOL ) &&
                ( e.vscp_type == VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER ) ) {
                    state = STATE_BOOTWAIT;
                    goto  ENTER_BOOT_MODE;
                }
            }
            break;                           
                
            // ----------------------------------------------------------------    
            default:
                state = STATE_VIRGIN;
                break;      
                
        }
    
    } // while        
    
}    
