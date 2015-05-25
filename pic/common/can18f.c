// File:  can18f.c

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	Version 0.03 2006-04-11
 * 	akhe@grodansparadis.com
 *
 *  Based on Microchip application note AN738
 *  Copyright by Microchip
 *  Original Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the ?Company?) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company?s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 *  Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN ?AS IS? CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *  
 * ============================================================================
 *          Changes Copyright (C) 2004-3012 Ake Hedman,Grodans Paradis AB
 * ============================================================================
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

#include <p18cxxx.h>
#include <vscp_compiler.h> 	// This file should be in your project folder
#include <vscp_projdefs.h>	// This file should be in your project folder
#include "can18f.h"

// Local prototypes
static void vscp18F_writeRegs(uint8_t* ptr, uint32_t val, BOOL bExtended);
static void vscp18F_readRegs(uint8_t* ptr, uint32_t *val, BOOL bExtended);

//
// CAN controller configuration
// ============================
//
// http://www.port.de/engl/canprod/sv_req_form.html
//

#define VSCPNODE_CAN_SJW			1
#define VSCPNODE_CAN_BRP			9
#define VSCPNODE_CAN_PHSEG1			8
#define VSCPNODE_CAN_PHSEG2			6
#define VSCPNODE_CAN_PROPSEG			1

//#define VSCPNODE_CAN_MASK_BUF1 		0x01ff0000	// Only class = 0
//#define VSCPNODE_CAN_MASK_BUF2 		0x01ff0000
#define VSCPNODE_CAN_MASK_BUF1 			0x0000000	// All clases
#define VSCPNODE_CAN_MASK_BUF2 			0x0000000

#define VSCPNODE_CAN_FILTER1_BUF1		0
#define VSCPNODE_CAN_FILTER2_BUF1		0
#define VSCPNODE_CAN_FILTER1_BUF2		0
#define VSCPNODE_CAN_FILTER2_BUF2		0
#define VSCPNODE_CAN_FILTER3_BUF2		0
#define VSCPNODE_CAN_FILTER4_BUF2		0

///////////////////////////////////////////////////////////////////////////////
// vscp18f_init
//

void vscp18f_init(BOOL bExtended) {

    // Set CAN module into configuration mode
    vscp18f_cansetmode(CAN18F_MODE_CONFIG);

    // Initialize the CAN module
    //BRGCON1 = 0x01;  // 40 MHz, 500kbits, 85.0% 
    //BRGCON2 = 0xbf;
    //BRGCON3 = 0x02;

    // 32MHZ clock, 125kbps
    // T1=12, T2=4, BTQ=16, Sample point=75,SWJ=1,bitrate=125,error=0%
    //BRGCON1 = 0x07;
    //BRGCON2 = 0xAC;
    //BRGCON3 = 0x03;

    // 40MHZ clock, 125kbps
    // T1=17, T2=3, BTQ=20, Sample point=85%,SWJ=1,bitrate=125,error=0%
    // http://www.kvaser.com/index.htm
    //BRGCON1 = 0x07;
    //BRGCON2 = 0xbf;
    //BRGCON3 = 0x02;

    // http://www.port.de/cgi-bin/tq.cgi?ctype=Pic18&CLK=20&sample_point=87.5
    // 40 MHz clock ( Fcan = 0.5 * Fosc = 20 MHz )
    // 87.5% sample point
    // Prescaler = 10 Tq = 0.5uS
    // SJW = 1
    // Propseg = 5
    // Phaseseg1 = 8
    // SAM = 0
    // Seg2 Freely prog.
    // Phaseseg2 = 2
#if (_XTAL_FREQ == 32000000)
    BRGCON1 = 0x07;
    BRGCON2 = 0xac;
    BRGCON3 = 0x03;
#else
#error("Current crystal frequency is not supported")
#endif
    // Receive only valid extended messages
    // Receive buffer 0 overflow will write to buffer 1
    if (bExtended) {
        RXB0CON = 0x44; // all = 0x64, extended = 0x44
    } else {
        RXB0CON = 0x64; // all = 0x64, extended = 0x44
    }
    RXB1CON = RXB0CON; // 0x40;

    // CANTX will drive VDD when rececive
    CIOCON = 0x20;

    // Set Buffer1 Mask value
    vscp18f_setmask(CAN_MASK_B1, VSCPNODE_CAN_MASK_BUF1, TRUE);

    // Set Buffer 2 Mask value
    vscp18f_setmask(CAN_MASK_B2, VSCPNODE_CAN_MASK_BUF2, TRUE);

    // Set Buffer 1 Filter Values
    vscp18f_setfilter(CAN_FILTER_B1_F1, VSCPNODE_CAN_FILTER1_BUF1, TRUE);
    vscp18f_setfilter(CAN_FILTER_B1_F2, VSCPNODE_CAN_FILTER2_BUF1, TRUE);

    vscp18f_setfilter(CAN_FILTER_B2_F1, VSCPNODE_CAN_FILTER1_BUF2, TRUE);
    vscp18f_setfilter(CAN_FILTER_B2_F2, VSCPNODE_CAN_FILTER2_BUF2, TRUE);
    vscp18f_setfilter(CAN_FILTER_B2_F3, VSCPNODE_CAN_FILTER3_BUF2, TRUE);
    vscp18f_setfilter(CAN_FILTER_B2_F4, VSCPNODE_CAN_FILTER4_BUF2, TRUE);

    // Set CAN module into normal mode
    vscp18f_cansetmode(CAN18F_MODE_NORMAL);
}

///////////////////////////////////////////////////////////////////////////////
//	vscp18f_cansetmode
//

void vscp18f_cansetmode(uint8_t opmode) {
    CANCON = opmode;

    // Wait until set
    while ((CANSTAT & 0b11100000) != opmode);
}

///////////////////////////////////////////////////////////////////////////////
//	vscp18F_writeRegs
//

static void vscp18F_writeRegs(uint8_t* ptr, uint32_t val, BOOL bExtended) {
    CAN_MESSAGE_ID *Value;

    Value = (CAN_MESSAGE_ID*) & val;

    if (bExtended) {
        // Extended Identifier
        *ptr = Value->BYTES.BYTE_3 >> 5; // Copy EID<23:21> to SIDH<2:0>
        *ptr |= Value->BYTES.BYTE_4 << 3; // Copy EID<28:24> to SIDH<7:3>
        ptr++; // Point to SIDL
        *ptr = (Value->BYTES.BYTE_3 << 3) & 0xE0; // Copy EID<20:18> to SIDL<7:5>
        // mask out EID<17:16> bits
        *ptr |= 0b00001000; // Set EXIDEN bit  to SIDL<3>
        *ptr |= Value->BYTES.BYTE_3 & 0x03; // Copy EID<17:16> to SIDL<1:0>
        ptr++; // Point to EIDH
        *ptr = Value->BYTES.BYTE_2; // Copy EID<15:8> to EIDH<7:0>
        ptr++; // Point to EIDL
        *ptr = Value->BYTES.BYTE_1; // Copy EID<7:0> to EIDL<7:0>
    } else {
        // Standard Identifier
        *ptr = Value->BYTES.BYTE_1 >> 3; // Copy SID<7:3> to SIDH<4:0>
        *ptr |= (Value->BYTES.BYTE_2 << 5); // Copy SID<10:8> to SIDH<7:5>
        ptr++; // Point to SIDL
        *ptr = Value->BYTES.BYTE_1 << 5; // Copy SID<2:0> to SIDL<7:5>
    }
}

///////////////////////////////////////////////////////////////////////////////
//	vscp18F_readRegs
//

static void vscp18F_readRegs(uint8_t* ptr, uint32_t *val, BOOL bExtended) {
    CAN_MESSAGE_ID *Value;

    Value = (CAN_MESSAGE_ID*) val;

    if (bExtended) {
        // Extended Identifier
        Value->BYTES.BYTE_3 = (*ptr << 5); // Copy SIDH<2:0> to EID<23:21>
        Value->BYTES.BYTE_4 = (*ptr >> 3); // Copy SIDH<7:3> to EID<29:25>
        ptr++; // Point to SIDL
        Value->BYTES.BYTE_3 |= (*ptr & 0x03); // Copy SIDH<1:0> to EID<17:16>
        // Bug-Fix NKR 11/20/00
        Value->BYTES.BYTE_3 |= ((*ptr & 0xe0) >> 3); // Copy SIDL<7:6> to EID<20:18>
        ptr++; // Point to EIDH
        Value->BYTES.BYTE_2 = *ptr; // Copy EIDH<15:8> to EID<15:8>
        ptr++; // Point to EIDL
        Value->BYTES.BYTE_1 = *ptr; // Copy EIDH<7:0> to EID<7:0>
    } else {
        // Standard Identifier
        Value->BYTES.BYTE_1 = (*ptr << 3); // Copy SIDH<4:0> to SID<7:3>
        Value->BYTES.BYTE_2 = *ptr >> 5; // Copy SIDH<7:5> to SID<10:8>
        ptr++; // Point to SIDL
        Value->BYTES.BYTE_1 |= (*ptr >> 5); // Copy SIDL<7:6> to SID<2:0>
        Value->BYTES.BYTE_3 = 0x00;
        Value->BYTES.BYTE_4 = 0x00;
    }
}

///////////////////////////////////////////////////////////////////////////////
//	vscp18f_setmask
//

void vscp18f_setmask(uint8_t nmask, uint32_t val, BOOL bExtended) {
    uint8_t *ptr;

    // Set base pointer
    if (nmask == CAN_MASK_B1) {
        ptr = (uint8_t*) & RXM0SIDH;
    } else {
        ptr = (uint8_t*) & RXM1SIDH;
    }

    // Write to regs
    vscp18F_writeRegs(ptr, val, bExtended);
}

///////////////////////////////////////////////////////////////////////////////
//	vscp18f_setmask
//

void vscp18f_setfilter(uint8_t nfilter, uint32_t val, BOOL bExtended) {
    uint8_t *ptr;

    // Set base pointer
    switch (nfilter) {

        case CAN_FILTER_B1_F1:
            ptr = (uint8_t*) & RXF0SIDH;
            break;

        case CAN_FILTER_B1_F2:
            ptr = (uint8_t*) & RXF1SIDH;
            break;

        case CAN_FILTER_B2_F1:
            ptr = (uint8_t*) & RXF2SIDH;
            break;

        case CAN_FILTER_B2_F2:
            ptr = (uint8_t*) & RXF3SIDH;
            break;

        case CAN_FILTER_B2_F3:
            ptr = (uint8_t*) & RXF4SIDH;
            break;

        default:
            ptr = (uint8_t*) & RXF5SIDH;
            break;
    }

    // Write to regs
    vscp18F_writeRegs(ptr, val, bExtended);

}


///////////////////////////////////////////////////////////////////////////////
//	vscp18f_sendMsg
//

BOOL vscp18f_sendMsg(uint32_t id, uint8_t* data, uint8_t dlc, uint8_t flags) {
    uint8_t i;
    uint8_t *ptr;

    // Find the first empty transmitter.
    if (TXB0CONbits.TXREQ == 0) {
        // TxBuffer0 is available.  Set WIN bits to point to TXB0
        CANCON &= 0b11110001;
        CANCON |= 0b00001000;
    } else if (TXB1CONbits.TXREQ == 0) {
        // TxBuffer1 is available. Set WIN bits to point to TXB1
        CANCON &= 0b11110001;
        CANCON |= 0b00000110;
    } else if (TXB2CONbits.TXREQ == 0) {
        // TxBuffer2 is available.. Set WIN bits to point to TXB2
        CANCON &= 0b11110001;
        CANCON |= 0b00000100;
    } else {
        // uanble to send message - all buffers full
        return FALSE;
    }

    //
    // Now that WIN has remapped RXB0 to empty buffer, simply
    // populate RXB0 buffer
    //

    // Set transmit priority.
    RXB0CON = flags & 0b00000011;

    // Populate Extended identifier information only if it is
    // desired.
    if (flags & CAN_TX_XTD_FRAME) {
        vscp18F_writeRegs((uint8_t*) & RXB0SIDH, id, TRUE);
    } else {
        vscp18F_writeRegs((uint8_t*) & RXB0SIDH, id, FALSE);
    }

    RXB0DLC = dlc;

    // RTR
    if (flags & CAN_TX_RTR_FRAME) {
        RXB0DLC |= 0b01000000;
    }

    // Populate data values.
    ptr = (uint8_t*) & RXB0D0;
    for (i = 0; i < dlc; i++) {
        ptr[ i ] = data[ i ];
    }

    //
    // Mark this buffer as ready to start transmit.
    // We are not using C bit field structure because RXB0 registers
    // are remapped to one of the empty transmit buffers and their
    // bit3 is not same as RXB0CON bit3.  To avoid confusion, in-line
    // assembly is used to directly set bit 3 of corresponding TXBnCON
    // register.
    //

#if defined(MCHP_C18)
    #if defined(__18F248) || defined(__18F258) ||defined(__18F448) || defined(__18F458)
        _asm
        bsf RXB0CON, 3, 0
                _endasm
    #else
        #error "PIC 18Fxxx(x) not supported."
    #endif
#endif
#if defined(HITECH_C18)
    #if defined(__18F248) || defined(__18F258) ||defined(__18F448) || defined(__18F458)
        asm("bsf _RXB0CON,3");
    #elif defined(__18F2480) || defined(__18F2580)  ||defined(__18F4480) || defined(__18F4580)
        RXB0RTRR0 = 1;
    #else
    #error "PIC 18Fxxx(x) not supported."
    #endif
#endif

    //
    // Restore CAN buffer mapping so that subsequent access to RXB0
    // buffers are to the real RXB0 buffer.
    //
    CANCON &= 0b11110001;

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//	vscp18f_readMsg
//

BOOL vscp18f_readMsg(uint32_t *id, uint8_t *data, uint8_t *dlc, uint8_t *flags) {
    uint8_t i;
    uint8_t *ptr;
    BOOL lbIsItBuffer0;

    // Start with no error or flags set.
    *flags = 0x00;

    // Find which buffer is ready.
    if (RXB0CONbits.RXFUL) {
        // Buffer0 contains a message.
        CANCON &= 0b11110001;

        lbIsItBuffer0 = TRUE;

        // Clear the received flag.
        PIR3bits.RXB0IF = 0;

        // Record and forget any previous overflow 
#if defined(__18F248) || defined(__18F258) ||defined(__18F448) || defined(__18F458)
        if (COMSTATbits.RXB0OVFL) {
            *flags |= CAN_RX_OVERFLOW;
            COMSTATbits.RXB0OVFL = 0;
        }
#elif defined(__18F2480) || defined(__18F2580) ||defined(__18F4480) || defined(__18F4580)
        if (COMSTATbits.RXB0OVFL) {
            *flags |= CAN_RX_OVERFLOW;
            COMSTATbits.RXB0OVFL = 0;
        }
#elif defined(__18F6680) 
        if (COMSTATbits.RXB0OVFL) {
            *flags |= CAN_RX_OVERFLOW;
            // errata...
            //COMSTATbits.RXB0OVFL = 0;
#if defined(MCHP_C18)
            _asm
            BCF COMSTAT, 6, 0
                    _endasm
#endif
#if defined(HITECH_C18)
                    asm("BCF COMSTAT, 6, 0?");
#endif
        }
#else
#error "PIC 18Fxxx(x) not supported."
#endif       

        if (RXB0CONbits.RXB0DBEN) {
            *flags |= RXB0CON & 0b00000111;
            *flags &= 0x01;
        }
    } else if (RXB1CONbits.RXFUL) {
        // RXBuffer1 is full
        CANCON &= 0b11110001;
        CANCON |= 0b00001010;

        lbIsItBuffer0 = FALSE;

        // Clear the received flag.
        PIR3bits.RXB1IF = 0;

        // Record and forget any previous overflow
#if defined(__18F248) || defined(__18F258) ||defined(__18F448) || defined(__18F458)
        if (COMSTATbits.RXB0OVFL) {
            *flags |= CAN_RX_OVERFLOW;
            COMSTATbits.RXB1OVFL = 0;
        }
#elif defined(__18F2480) || defined(__18F2580)  ||defined(__18F4480) || defined(__18F4580)
        if (COMSTATbits.RXB0OVFL) {
            *flags |= CAN_RX_OVERFLOW;
            COMSTATbits.RXB1OVFL = 0;
        }
#elif defined(__18F6680) 
        if (COMSTATbits.RXB0OVFL) {
            *flags |= CAN_RX_OVERFLOW;
            // errata...
            //COMSTATbits.RXB1OVFL = 0;
#if defined(MCHP_C18)
            _asm
            BCF COMSTAT, 7, 0
                    _endasm
#endif
#if defined(HITECH_C18)
                    asm("BCF COMSTAT, 7, 0?");
#endif
        }
#else
#error "PIC 18Fxxx(x) not supported."
#endif        
        *flags |= RXB1CON & 0b00000111;
        if (*flags < 0x02)
            *flags |= CAN_RX_DBL_BUFFERED;
    } else {
        return FALSE;
    }

    // Retrieve message length.
    *dlc = RXB0DLC & 0b00001111;

    // Determine whether this was RTR or not.
    if ((RXB0DLC & 0x40)) {
        *flags |= CAN_RX_RTR_FRAME;
    }

    // Retrieve EIDX bytes only if this is extended message
    if (RXB0SIDLbits.EXID) {
        *flags |= CAN_RX_XTD_FRAME;

        vscp18F_readRegs((uint8_t*) & RXB0SIDH, id, TRUE);
    } else {
        vscp18F_readRegs((uint8_t*) & RXB0SIDH, id, FALSE);
    }

    // Get message data itself
    ptr = (uint8_t*) & RXB0D0;
    for (i = 0; i < *dlc; i++) {
        data[ i ] = ptr[ i ];
    }

    // Restore default RXB0 mapping.
    CANCON &= 0b11110001;

    // Record and Clear any previous invalid message bit flag.
    if (PIR3bits.IRXIF) {
        *flags |= CAN_RX_INVALID_MSG;
        PIR3bits.IRXIF = 0;
    }

    if (lbIsItBuffer0) {
        RXB0CONbits.RXFUL = 0;
    } else {
        RXB1CONbits.RXFUL = 0;
    }

    return TRUE;
}




