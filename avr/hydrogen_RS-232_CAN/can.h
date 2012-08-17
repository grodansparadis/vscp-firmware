//
// can.h
//
// Copyright (C) 2002 Ake Hedman, Eurosource, akhe@users.sourceforge.net
//
// This software is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
// parts from code written by Ake Hedman, CC Systems AB
// parts derived from code written by Arnaud Westenberg 
// arnaud@wanadoo.nl and his can driver.

#ifndef ___CAN_INCLUDE___
#define ___CAN_INCLUDE___

/// Generic datatypes
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;


/**
   Length of can message
*/
#define CAN_MSG_LENGTH 8

/**
   Definition of can message structure
*/
struct canmsg {
  u32 flags;                  // flags for packet   
  u32 id;                     // id for frame
  u32 timestamp;              // timestamp for frame
  u8 length;                  // length for data part of frame
  u8 data[ CAN_MSG_LENGTH ];  // can frame data
} PACKED;


typedef struct canmsg canmsg_t;

/**
   Structure for statistics
*/
struct canstatistics
{
  u32 cntRxPkt;      // Counter for received packets.
  u32 cntRxData;     // Counter for received data bytes.
  u32 cntTxPkt;      // Counter for sent packets.
  u32 cntTxData;     // Counter for sent data bytes.
  u32 cntWakeup;     // Counter for chip wakeup's
  u32 cntWarnings;   // Counter for chip warning states.
  u32 cntBusOff;     // Counter for chip bus off conditions.
  u32 cntRxFifoOvr;  // Counter for Rx FIFO overruns.
  u32 cntTxFifoOvr;  // Counter for transmitter overruns (if NONBLOCK).
  u32 cntStuffErr;   // Counter for stuff errors.
  u32 cntFormErr;    // Counter for form erros.
  u32 cntAckErr;     // Counter for ACK errors.
  u32 cntBit0Err;    // Counter for Bit 0 errors.
  u32 cntBit1Err;    // Counter for Bit 1 errors.
  u32 cntCrcErr;     // Counter for CRC errors.
};

typedef struct canstatistics canstatistics_t;

/**
   Definition of a CAN frame types.
*/
typedef unsigned long canframetype_t;

/**
   Definition of a CAN message.
*/
typedef unsigned long canmsgid_t;


/// Definitions to use for canmsg_t flags 
#define MSG_EXT ( 1<<0 )  // Extended frame
#define MSG_RTR ( 1<<1 )  // Remote frame   
#define MSG_OVR ( 1<<2 )  

#endif











