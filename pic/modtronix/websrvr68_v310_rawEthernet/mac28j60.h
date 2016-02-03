/*********************************************************************
 *
 *                  MAC Module Defs for Microchip Stack
 *
 *********************************************************************
 * FileName:        MAC.h
 * Dependencies:    StackTsk.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Compiler:        Microchip C18 v3.02 or higher
 *					Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright © 2002-2007 Microchip Technology Inc.  All rights 
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and 
 * distribute: 
 * (i)  the Software when embedded on a Microchip microcontroller or 
 *      digital signal controller product (“Device”) which is 
 *      integrated into Licensee’s product; or
 * (ii) ONLY the Software driver source files ENC28J60.c and 
 *      ENC28J60.h ported to a non-Microchip device used in 
 *      conjunction with a Microchip ethernet controller for the 
 *      sole purpose of interfacing with the ethernet controller. 
 *
 * You should refer to the license agreement accompanying this 
 * Software for additional information regarding your rights and 
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT 
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT 
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL 
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF 
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS 
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE 
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER 
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT 
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/27/01     Original        (Rev 1.0)
 * Nilesh Rajbharti     11/27/01    Added SLIP
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 * Howard Schlunder     6/28/04     Added ENC28J60 specific features
 * Howard Schlunder		11/29/04	Added Get/SetLEDConfig macros
 ********************************************************************/
#ifndef __MAC_H
#define __MAC_H


#if (!defined(ENC_CS_TRIS)) && \
	 (defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60) || \
	  defined(_18F97J60) ||  defined(_18F96J65) ||  defined(_18F96J60) ||  defined(_18F87J60) ||  defined(_18F86J65) ||  defined(_18F86J60) ||  defined(_18F67J60) ||  defined(_18F66J65) ||  defined(_18F66J60))
	#include "TCPIP Stack/ETH97J60.h"
#else
	#include "TCPIP Stack/ENC28J60.h"
#endif


#define MAC_TX_BUFFER_SIZE			(1500ul)

// A generic structure representing the Ethernet header starting all Ethernet 
// frames
typedef struct _ETHER_HEADER
{
	MAC_ADDR        DestMACAddr;
	MAC_ADDR        SourceMACAddr;
	WORD_VAL        Type;
} ETHER_HEADER;


#define MAC_IP      	(0x00u)
#define MAC_ARP     	(0x06u)
#define MAC_UNKNOWN 	(0xFFu)



/*
 * Microchip Ethernet controller specific MAC items
 */
#if !defined(STACK_USE_HTTP2_SERVER)
	#define RESERVED_HTTP_MEMORY 0ul
#endif

#if !defined(STACK_USE_SSL_SERVER)
	#define RESERVED_SSL_MEMORY 0ul
#endif

// MAC RAM definitions
#define RAMSIZE	8192ul		
#define TXSTART (RAMSIZE - (1ul+1514ul+7ul) - TCP_ETH_RAM_SIZE - RESERVED_HTTP_MEMORY - RESERVED_SSL_MEMORY)
#define RXSTART	(0ul)						// Should be an even memory address; must be 0 for errata
#define	RXSTOP	((TXSTART-2ul) | 0x0001ul)	// Odd for errata workaround
#define RXSIZE	(RXSTOP-RXSTART+1ul)

#define BASE_TX_ADDR	(TXSTART + 1ul)
#define BASE_TCB_ADDR	(BASE_TX_ADDR + (1514ul+7ul))
#define BASE_HTTPB_ADDR (BASE_TCB_ADDR + TCP_ETH_RAM_SIZE)
#define BASE_SSLB_ADDR	(BASE_HTTPB_ADDR + RESERVED_HTTP_MEMORY)

#if (RXSIZE < 1400) || (RXSIZE > RAMSIZE)
	#error Warning, Ethernet RX buffer is tiny.  Reduce TCP socket count, the size of each TCP socket, or move sockets to a different RAM
#endif


WORD	MACCalcRxChecksum(WORD offset, WORD len);
WORD 	CalcIPBufferChecksum(WORD len);

void	MACPowerDown(void);
void 	MACPowerUp(void);
void	WritePHYReg(BYTE Register, WORD Data);
PHYREG	ReadPHYReg(BYTE Register);
void	SetRXHashTableEntry(MAC_ADDR DestMACAddr);

// ENC28J60 specific
void	SetCLKOUT(BYTE NewConfig);
BYTE	GetCLKOUT(void);

/******************************************************************************
 * Macro:        	void SetLEDConfig(WORD NewConfig)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           NewConfig - xxx0: Pulse stretching disabled
 *								xxx2: Pulse stretch to 40ms (default)
 *								xxx6: Pulse stretch to 73ms
 *								xxxA: Pulse stretch to 139ms
 *								
 *								xx1x: LEDB - TX
 *								xx2x: LEDB - RX (default)
 *								xx3x: LEDB - collisions
 *								xx4x: LEDB - link
 *								xx5x: LEDB - duplex
 *								xx7x: LEDB - TX and RX
 *								xx8x: LEDB - on
 *								xx9x: LEDB - off
 *								xxAx: LEDB - blink fast
 *								xxBx: LEDB - blink slow
 *								xxCx: LEDB - link and RX
 *								xxDx: LEDB - link and TX and RX
 *								xxEx: LEDB - duplex and collisions
 *
 *								x1xx: LEDA - TX
 *								x2xx: LEDA - RX
 *								x3xx: LEDA - collisions
 *								x4xx: LEDA - link (default)
 *								x5xx: LEDA - duplex
 *								x7xx: LEDA - TX and RX
 *								x8xx: LEDA - on
 *								x9xx: LEDA - off
 *								xAxx: LEDA - blink fast
 *								xBxx: LEDA - blink slow
 *								xCxx: LEDA - link and RX
 *								xDxx: LEDA - link and TX and RX
 *								xExx: LEDA - duplex and collisions
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Writes the value of NewConfig into the PHLCON PHY register.  
 *					The LED pins will beginning outputting the new 
 *					configuration immediately.
 *
 * Note:            
 *****************************************************************************/
#define SetLEDConfig(NewConfig)		WritePHYReg(PHLCON, NewConfig)


/******************************************************************************
 * Macro:        	WORD GetLEDConfig(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          WORD -	xxx0: Pulse stretching disabled
 *							xxx2: Pulse stretch to 40ms (default)
 *							xxx6: Pulse stretch to 73ms
 *							xxxA: Pulse stretch to 139ms
 *								
 *							xx1x: LEDB - TX
 *							xx2x: LEDB - RX (default)
 *							xx3x: LEDB - collisions
 *							xx4x: LEDB - link
 *							xx5x: LEDB - duplex
 *							xx7x: LEDB - TX and RX
 *							xx8x: LEDB - on
 *							xx9x: LEDB - off
 *							xxAx: LEDB - blink fast
 *							xxBx: LEDB - blink slow
 *							xxCx: LEDB - link and RX
 *							xxDx: LEDB - link and TX and RX
 *							xxEx: LEDB - duplex and collisions
 *
 * 							x1xx: LEDA - TX
 *							x2xx: LEDA - RX
 *							x3xx: LEDA - collisions
 *							x4xx: LEDA - link (default)
 *							x5xx: LEDA - duplex
 *							x7xx: LEDA - TX and RX
 *							x8xx: LEDA - on
 *							x9xx: LEDA - off
 *							xAxx: LEDA - blink fast
 *							xBxx: LEDA - blink slow
 *							xCxx: LEDA - link and RX
 *							xDxx: LEDA - link and TX and RX
 *							xExx: LEDA - duplex and collisions
 *
 * Side Effects:    None
 *
 * Overview:        Returns the current value of the PHLCON register.
 *
 * Note:            None
 *****************************************************************************/
#define GetLEDConfig()		ReadPHYReg(PHLCON).Val


void MACInit(void);
BOOL MACIsLinked(void);

BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type);
void MACSetReadPtrInRx(WORD offset);
WORD MACSetWritePtr(WORD address);
WORD MACSetReadPtr(WORD address);
BYTE MACGet(void);
WORD MACGetArray(BYTE *val, WORD len);
void MACDiscardRx(void);
WORD MACGetFreeRxSize(void);
void MACMemCopyAsync(WORD destAddr, WORD sourceAddr, WORD len);
BOOL MACIsMemCopyDone(void);

void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen);
BOOL MACIsTxReady(void);
void MACPut(BYTE val);
void MACPutArray(BYTE *val, WORD len);
void MACFlush(void);

// ROM function variants for PIC18
#if defined(__18CXX)
	void MACPutROMArray(ROM BYTE *val, WORD len);
#else
	#define MACPutROMArray(a,b)	MACPutArray((BYTE*)a,b)
#endif

#endif
