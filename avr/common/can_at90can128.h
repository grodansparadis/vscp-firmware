/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	2006-03-20
 * 	akhe@eurosource.se
 *
 *  Copyright (C) 2006-2007 Ake Hedman, eurosource
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

/*       
 *  MOB = 0 is used for receiption.
 *  MOB = 1-14 is used for transmission.     
 */


#ifndef  FALSE
#define  FALSE  0
#endif

#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef	__CAN_AT90CAN1238__H
#define	__CAN_AT90CAN1238__H

#ifdef F_CPU
#ifndef FOSC
#define FOSC (F_CPU / 1000)
#endif
#endif

#ifdef FOSC
#ifndef F_CPU
#define F_CPU (FOSC * 1000)
#endif
#endif

#ifdef FOSC
#ifdef F_CPU
#if F_CPU != (FOSC * 1000)
#error F_CPU and FOSC defines do not match
#endif
#endif
#endif

#ifndef FOSC
#error You must define FOSC in kHz or F_CPU in Hz (or both) in your makefile

// 8MHz clock
// 
//	Tq/bit	Tq 	t	PRS	PHS1	PHS2	%	BRP	CANBT1	CANBT2	CANBT3
//=================================================================================================
//	1000    8	0.125	3	2	2	75%	0x00	0x00	0x04	0x12	
//	800		10	0.125	3	4	2	80%	0x00	0x00	0x04	0x16
//	500		16	0.125	8	5	2	87.5%	0x00	0x00	0x0E	0x17
//	250		16	0.250	8	5	2	87.5%	0x01	0x02	0x0E	0x18
//	125		16	0.500	8	5	2	87.5%	0x03	0x06	0x0E	0x18
//	100		16	0.625	8	5	2	87.5% 	0x07	0x0E	0x0E	0x18
//	50		16	1.250	8	5	2	87.5%	0x09	0x12	0x0E	0x18
//	20		16	3.125	8	5	2	87.5%	0x13	0x24	0x0E	0x18
//	10		16	6.250	8	5	2	87.5%	0x31	0x62	0x0E	0x18

#elif FOSC == 8000

	// 10Kbps  
    #define CANBT1_10KBPS  0x62
    #define CANBT2_10KBPS  0x0E
    #define CANBT3_10KBPS  0x18
	
	// 20Kbps  
    #define CANBT1_20KBPS  0x24
    #define CANBT2_20KBPS  0x0E
    #define CANBT3_20KBPS  0x18
	
	// 50Kbps  
    #define CANBT1_50KBPS  0x12
    #define CANBT2_50KBPS  0x0E
    #define CANBT3_50KBPS  0x18
	
	// 100Kbps  
    #define CANBT1_100KBPS  0x0E
    #define CANBT2_100KBPS  0x0E
    #define CANBT3_100KBPS  0x18
	
	// 125Kbps  
    #define CANBT1_125KBPS  0x06
    #define CANBT2_125KBPS  0x0E
    #define CANBT3_125KBPS  0x18
	
	// 250Kbps  
    #define CANBT1_250KBPS  0x02
    #define CANBT2_250KBPS  0x0E
    #define CANBT3_250KBPS  0x18
	
	// 500Kbps  
    #define CANBT1_500KBPS  0x00
    #define CANBT2_500KBPS  0x0E
    #define CANBT3_500KBPS  0x17
	
	// 800Kbps  
    #define CANBT1_800KBPS  0x00
    #define CANBT2_800KBPS  0x04
    #define CANBT3_800KBPS  0x16
	
	// 1000Kbps  
    #define CANBT1_1000KBPS  0x00
    #define CANBT2_1000KBPS  0x04
    #define CANBT3_1000KBPS  0x12

// 16MHz clock
//    
//	Tq/bit	Tq t	PRS	PHS1	PHS2	%	BRP	CANBT1	CANBT2	CANBT3
//=================================================================================================
//	1000    8	0.125	3    	2  	2	75%	0x01	0x02	0x04	0x12	
//	800	10	0.125	3	4	2	80%	0x01	0x02	0x04	0x16
//	500	16	0.125	8	5	2	87.5%	0x01	0x02	0x0E	0x18
//	250	16	0.250	8	5	2	87.5%	0x03	0x06	0x0E	0x18
//	125	16	0.500	8	5	2	87.5%	0x07	0x0E	0x0E	0x18
//	100	16	0.625	8	5	2	87.5%	0x09	0x12	0x0E	0x18
//	50	16	1.250	8	5	2	87.5%	0x13	0x24	0x0E	0x18

//	20	16	3.125	8	5	2	87.5%	0x31	0x62	0x0E	0x18
//	10	16	6.250	8	5	2	87.5%	0x63	0xC6	0x0E	0x18
	
#elif FOSC == 16000	
	 
    // 10Kbps 
    #define CANBT1_10KBPS  0xC6
    #define CANBT2_10KBPS  0x0E
    #define CANBT3_10KBPS  0x18
	
	// 20Kbps 
    #define CANBT1_20KBPS  0x62
    #define CANBT2_20KBPS  0x0E
    #define CANBT3_20KBPS  0x18
	
	// 50Kbps   
    #define CANBT1_50KBPS  0x24
    #define CANBT2_50KBPS  0x0E
    #define CANBT3_50KBPS  0x18
	
	// 100Kbps  
    #define CANBT1_100KBPS  0x12
    #define CANBT2_100KBPS  0x0E
    #define CANBT3_100KBPS  0x18
	
	// 125Kbps  
    #define CANBT1_125KBPS  0x0E
    #define CANBT2_125KBPS  0x0E
    #define CANBT3_125KBPS  0x18
	
	// 250Kbps  
    #define CANBT1_250KBPS  0x06
    #define CANBT2_250KBPS  0x0E
    #define CANBT3_250KBPS  0x18
	
	// 500Kbps  
    #define CANBT1_500KBPS  0x02 
    #define CANBT2_500KBPS  0x0E 
    #define CANBT3_500KBPS  0x18 

	// 800Kbps  
    #define CANBT1_800KBPS  0x02
    #define CANBT2_800KBPS  0x04
    #define CANBT3_800KBPS  0x16
	
	// 87.5% BRP=0, PRS=0, PHS1=4, PHS2=0, Quanta=8 
    #define CANBT1_1000KBPS  0x02
    #define CANBT2_1000KBPS  0x04
    #define CANBT3_1000KBPS  0x12

#else
  #error This FOSC is currently not supported
#endif

// * * *  MACROS * * *

// Set current MOB
#define setMob(idx) (CANPAGE = (idx << 4))

#define MOB_RECEIVE		0	// MOB used to receive frames

// Masks for AT90CAN128
#define CAN_MASK_RTR   		0x04
#define CAN_MASK_EXTENDED   0x10
#define CAN_MASK_SIZE   	0x0F

// Standard busrates
#define	CAN_BITRATE_10K		0
#define	CAN_BITRATE_20K		1
#define	CAN_BITRATE_50K		2
#define	CAN_BITRATE_100K	3
#define	CAN_BITRATE_125K	4
#define	CAN_BITRATE_250K	5
#define	CAN_BITRATE_500K	6
#define CAN_BITRATE_800K	7
#define	CAN_BITRATE_1M		8
#define CAN_BITRATE_MANUAL	255

/// ID flags
#define CAN_IDFLAG_STANDARD			0x00000000	// Standard message id (11-bit)
#define CAN_IDFLAG_EXTENDED			0x00000001	// Extended message id (29-bit)
#define CAN_IDFLAG_RTR				0x00000002	// RTR-Frame
#define CAN_IDFLAG_ERROR			0x00000004	// This package is an error indication (id holds error code)

#define ERROR_OK			0	// all is well
#define ERROR_BUFFER_FULL	-1	// No buffer available for transmit
#define ERROR_BUFFER_EMPTY	-2	// No frame to read
#define ERROR_BITRATE		-3	// Errornious bitrate

//
//  CAN Message structure
//
typedef struct {
    unsigned long id;
    unsigned char len;
    unsigned char flags;
    unsigned char byte[ 8 ];
} CANMsg;

// Helpers
void clrMob( unsigned char idx );
void clrAllMob( void );
int getFreeMob( void );


// API
int can_Open( unsigned char StdSpeed, 
				unsigned char Btr0, 
				unsigned char Btr1, 
				unsigned char Btr2 );
int can_Close( void );
int can_SendFrame( CANMsg *pmsg );
int can_readFrame( CANMsg *pmsg );
int can_getStatus( void );
int can_setFilter( unsigned char Filter0, 
						unsigned char Filter1, 
						unsigned char Filter2, 
						unsigned char Filter3 );
int can_setMask( unsigned char Mask0, 
					unsigned char Mask1, 
					unsigned char Mask2, 
					unsigned char Mask3 );
				

#endif // CAN_AT90CAN1238
