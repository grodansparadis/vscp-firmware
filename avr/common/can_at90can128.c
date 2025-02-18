/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-03-20
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2025 Ake Hedman, eurosource
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

#include "vscp_compiler.h"
#include "vscp_projdefs.h"
#include <avr/io.h>
#include "can_at90can128.h"

///////////////////////////////////////////////////////////////////////////////
// clrMob
//

void clrMob( unsigned char idx )
{
	unsigned char i;
    
	CANPAGE  = ( idx << 4 );
    CANSTMOB = 0;
    CANCDMOB = 0;
    CANIDT4  = 0;
    CANIDT3  = 0;
    CANIDT2  = 0;
    CANIDT1  = 0;
    CANIDM4  = 0;
    CANIDM3  = 0;
    CANIDM2  = 0;
    CANIDM1  = 0;
	
    for ( i = 0; i < 8; i++ ) {
      CANMSG = 0;
    }
}

///////////////////////////////////////////////////////////////////////////////
// clrAllMob
//

void clrAllMob( void )
{
	unsigned char i;
    
	for ( i=0; i<15; i++ ) {
    	clrMob( i );
    }
}

///////////////////////////////////////////////////////////////////////////////
// getFreeMob
//
// We use the first MOB for reciption so the search is 
// started from MOB == 1
//
// Returns ERROR_BUFFER_FULL if no free Mob available.
//

int getFreeMob( void )
{
    int Save_page;
    int i;

  	Save_page = CANPAGE;
  	for ( i = 1; i < 15; i++ ) {
    	setMob( i );
    	if ( !( CANCDMOB & 0xC0 ) ) { // Disabled?
      		CANPAGE = Save_page;
      		return i;
    	}
        else if ( CANSTMOB & 0x40 ) { // Tranmsitted?
        	CANPAGE = 0x00;	
        	CANPAGE = Save_page;
      		return i;
        }
  	}
  	
    CANPAGE = Save_page;
  	return ERROR_BUFFER_FULL;
}

///////////////////////////////////////////////////////////////////////////////
// can_Open
//
// Return values:
// 

int can_Open( unsigned char StdSpeed, 
				unsigned char Btr0, 
				unsigned char Btr1, 
				unsigned char Btr2 )
{
 	 unsigned char set_btr0, set_btr1, set_btr2;
	 
 	 // Timing
	 switch ( StdSpeed ) {
	 
	 	case CAN_BITRATE_10K:
			 set_btr0 = CANBT1_10KBPS;
			 set_btr1 = CANBT2_10KBPS;
			 set_btr2 = CANBT3_10KBPS;
			 break;
			 
		case CAN_BITRATE_20K:
			 set_btr0 = CANBT1_20KBPS;
			 set_btr1 = CANBT2_20KBPS;
			 set_btr2 = CANBT3_20KBPS;
			 break;
			 
		case CAN_BITRATE_50K:
			 set_btr0 = CANBT1_50KBPS;
			 set_btr1 = CANBT2_50KBPS;
			 set_btr2 = CANBT3_50KBPS;
			 break;
			 
		case CAN_BITRATE_100K:
			 set_btr0 = CANBT1_100KBPS;
			 set_btr1 = CANBT2_100KBPS;
			 set_btr2 = CANBT3_100KBPS;
			 break;
			 
		case CAN_BITRATE_125K:
			 set_btr0 = CANBT1_125KBPS;
			 set_btr1 = CANBT2_125KBPS;
			 set_btr2 = CANBT3_125KBPS;
			 break;
			 
		case CAN_BITRATE_250K:
			 set_btr0 = CANBT1_250KBPS;
			 set_btr1 = CANBT2_250KBPS;
			 set_btr2 = CANBT3_250KBPS;
			 break;
			 
		case CAN_BITRATE_500K:
			 set_btr0 = CANBT1_500KBPS;
			 set_btr1 = CANBT2_500KBPS;
			 set_btr2 = CANBT3_500KBPS;
			 break;
			 
		case CAN_BITRATE_800K:
			 set_btr0 = CANBT1_800KBPS;
			 set_btr1 = CANBT2_800KBPS;
			 set_btr2 = CANBT3_800KBPS;
			 break;
			 
		case CAN_BITRATE_1M:
			 set_btr0 = CANBT1_1000KBPS;
			 set_btr1 = CANBT2_1000KBPS;
			 set_btr2 = CANBT3_1000KBPS;
		 	 break;
			    
	    case CAN_BITRATE_MANUAL:
	 	  	 set_btr0 = Btr0;
		  	 set_btr1 = Btr1;
	 	  	 set_btr2 = Btr2;
		  	 break;
			 
		default:
			return ERROR_BITRATE; 
 	 }
	 
	 // Set bitrate
	 CANBT1 = set_btr0;
	 CANBT2 = set_btr1;
	 CANBT3 = set_btr2;
     
     // Clear all MOB's
     clrAllMob();
     
     // MOB 0 is for reception - enable it for receive
     setMob( 0 );
     CANCDMOB |= 0x80;
	 
	 CANGCON |= 2;
	 
	 // Put CAN Controller is in run state.
 	 return ERROR_OK;
}


///////////////////////////////////////////////////////////////////////////////
// can_Close
//
// Return value:
 

int can_Close( void )
{
 	 CANGCON &= 0xfd;
 	 return ERROR_OK;
}

///////////////////////////////////////////////////////////////////////////////
// can_SendFrame
//
// Return value:
 
//

int can_SendFrame( CANMsg *pmsg )
{
	int mobIdx;
 	int i;
        
 	if ( ERROR_BUFFER_FULL == ( mobIdx = getFreeMob() ) ) {
    	return ERROR_BUFFER_FULL; 
    }
    
    // Select the free MOB
    setMob( mobIdx ); // Not needed done is clrMob below
        
    // Clear the MOB
    clrMob( mobIdx );
    
    // Set Extended flag if needed
    if ( pmsg->flags & CAN_IDFLAG_EXTENDED ) {
    	
        // Extended frame
        CANCDMOB = CAN_MASK_EXTENDED + ( pmsg->len & CAN_MASK_SIZE );
        
        // RTR + id
        if ( pmsg->flags & CAN_MASK_RTR ) {
        	CANIDT4 =  ( ( pmsg->id & 0x1F ) << 3 ) | CAN_MASK_RTR; 
        }
        else {
        	CANIDT4 = ( ( pmsg->id & 0x1F ) << 3 );
        }
      
      	// Set id
        CANIDT3 =  ( ( pmsg->id >> 5 ) & 0xff );
        CANIDT2 =  ( ( pmsg->id >> 13 ) & 0xff );
        CANIDT1 =  ( ( pmsg->id >> 21 ) & 0xff );
    }
    else {
    	// Standard frame
    	CANCDMOB = ( pmsg->len & CAN_MASK_SIZE );
        if ( pmsg->flags & CAN_IDFLAG_RTR ) {
        	CANIDT4 = CAN_MASK_RTR; 
        }
        
        // Set id
        CANIDT2 = ( pmsg->id & 0x07 ) << 5;
        CANIDT1  = pmsg->id >> 3;
    }
    
    // Write data
    for ( i=0; i<pmsg->len; i++ ) {
    	CANMSG = pmsg->byte[ i ];
    }
    
    // Send frame
    CANCDMOB |=  0x40;
    
 	return ERROR_OK;
}

///////////////////////////////////////////////////////////////////////////////
// can_readFrame
//

int can_readFrame( CANMsg *pmsg )
{
	int rv = ERROR_OK;
    unsigned char i;
     
    // Select receive MOB
    setMob( 0 ); 
     
    // Check for frame
    if ( !( CANSTMOB & 0x20 ) ) {
    	return ERROR_BUFFER_EMPTY;
    } 
     
    pmsg->flags = 0;
 	pmsg->id = 0;
    pmsg->len = 0;
    
    // Check if RTR flga is set
    if ( CANIDT4 & 0x04 ) {
    	pmsg->flags |= CAN_IDFLAG_RTR;
    }
    
    if ( CANCDMOB & 0x10 ) {
    	// Extended fram
    	pmsg->flags |= CAN_IDFLAG_EXTENDED;
        pmsg->id = ( (unsigned long)CANIDT1 << 21 ) +
        				 ( (unsigned long)CANIDT2 << 13 ) +
                         ( (unsigned long)CANIDT3 << 5 ) +
                         ( ( CANIDT4 >> 3 ) & 0x1f );
    }				
    else {
    	// Standard frame
        pmsg->id = ( CANIDT1 << 3 ) + ( ( CANIDT2 >> 5 ) & 0x07 );
    }
    
    // Data
    pmsg->len = CANCDMOB & CAN_MASK_SIZE;
    for ( i=0; i<pmsg->len; i++ ) {
    	pmsg->byte[ i ] = CANMSG;
    }
    
    // Make the MOB ready to receive again
    clrMob( 0 );
    CANCDMOB |= 0x80;
	
 	return rv;
}

///////////////////////////////////////////////////////////////////////////////
// can_getStatus
//

int can_getStatus( void )
{
 	 return CANGSTA;
}

 
///////////////////////////////////////////////////////////////////////////////
// can_setFilter
//

int can_setFilter( unsigned char Filter0, 
						unsigned char Filter1, 
						unsigned char Filter2, 
						unsigned char Filter3 )  
{
 	int rv = ERROR_OK;
    
	// Select receive MOB
    setMob( 0 );
 	
    CANIDT1 = Filter0;            
    CANIDT2 = Filter1;
    CANIDT3 = Filter2;
    CANIDT4 = Filter3;
    
    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// can_setMask
//

int can_setMask( unsigned char Mask0, 
					unsigned char Mask1, 
					unsigned char Mask2, 
					unsigned char Mask3 ) 
{
 	int rv = ERROR_OK;
	
	// Select receive MOB
    setMob( 0 );

	CANIDM1 = Mask0;            
    CANIDM2 = Mask1;
    CANIDM3 = Mask2;
    CANIDM4 = Mask3;
	
 	return rv;
}
