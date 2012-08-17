// hydrogen.h
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
//

#include "can.h"

// Prototypes
void udelay( u32 t );

/// Base address for SJA1000 chip in
#define ADAPTER_BASE   0x4000

#ifndef TRUE
#define TRUE    -1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL    0L
#endif

// Major states
#define OFF_LINE 0
#define ON_LINE  1

// AT states
#define WAITING_FOR_A   0
#define WAITING_FOR_T   1
#define WAITING_FOR_CR  2

#define IDSTR "eurosource Hydrogen RS-232 CAN node"
#define SOFTWARE_VERSION "0.1.0"
#define HARDWARE_VERSION "1.0.0"

#define RESPONSE_OK     "OK"
#define RESPONSE_ERROR  "ERROR"

#define SIZE_INPUT_BUFFER  32

// Prototypes
void initHardware( void );
void doCommand( void );
void doResponse( int, u8 );
void doCommand( void );
int cmd_online( void );
int cmd_reset( void );
int cmd_quite( void );
int cmd_verbal( void );
int cmd_echo( void );
int cmd_info( void );
int cmd_baudrate( void );
int cmd_filter( void );
int cmd_listen( void );
int cmd_autobaud( void );
int cmd_readmsg( void );
int cmd_writemsg( void );
int cmd_register( void );
int cmd_sendBurst( void );


void nibbleout (unsigned char b );
void hexout (unsigned char b );
void print_CAN_Msg( canmsg_t *pmsg );
void crlf( void );
u32 getNumericValue( char *p );
int getHexValue( char c );
