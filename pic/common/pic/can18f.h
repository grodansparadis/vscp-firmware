// File:  can18f.h

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * 	Version 0.03 2006-04-11
 * 	akhe@eurosource.se
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
 * Changes Copyright (C) 2004 Ake Hedman, eurosource, <akhe@eurosource.se>
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
#ifndef CAN18F_H
#define CAN18F_H

#include <inttypes.h>


#if defined(HI_TECH_C)
#define HITECH_C18
#else
#define MCHP_C18
#endif

#if defined(MCHP_C18) && defined(HITECH_C18)
#error "Invalid Compiler selection."
#endif

#if !defined(MCHP_C18) && !defined(HITECH_C18)
#error "Compiler not supported."
#endif

typedef enum _BOOL {
    FALSE = 0, TRUE
} BOOL;

// CAN 18F operation modes
#define CAN18F_MODE_BITS	0b11100000
#define CAN18F_MODE_NORMAL  	0b00000000
#define CAN18F_MODE_SLEEP   	0b00100000
#define CAN18F_MODE_LOOP    	0b01000000
#define CAN18F_MODE_LISTEN  	0b01100000
#define CAN18F_MODE_CONFIG  	0b10000000

// Mask selection
#define CAN_MASK_B1			0
#define CAN_MASK_B2			1

// Filter selection
#define CAN_FILTER_B1_F1		0
#define CAN_FILTER_B1_F2		1
#define CAN_FILTER_B2_F1		2
#define CAN_FILTER_B2_F2		3
#define CAN_FILTER_B2_F3		4
#define CAN_FILTER_B2_F4		5

// Message flags
/*
#define CAN_TX_PRIORITY_0	0b11111100    
#define CAN_TX_PRIORITY_1	0b11111101     
#define CAN_TX_PRIORITY_2	0b11111110     
#define CAN_TX_PRIORITY_3	0b11111111     

#define CAN_TX_STD_FRAME    	0b11111111    
#define CAN_TX_XTD_FRAME    	0b11110111     

#define CAN_TX_NO_RTR_FRAME 	0b11111111    
#define CAN_TX_RTR_FRAME    	0b10111111   
 */
#define CAN_TX_PRIORITY_0	0b00000000    
#define CAN_TX_PRIORITY_1	0b00000001     
#define CAN_TX_PRIORITY_2	0b00000010     
#define CAN_TX_PRIORITY_3	0b00000011     

#define CAN_TX_XTD_FRAME    	0b00001000

#define CAN_TX_RTR_FRAME    	0b01000000

#define CAN_RX_OVERFLOW     	0b00001000    
#define CAN_RX_INVALID_MSG  	0b00010000   
#define CAN_RX_XTD_FRAME    	0b00100000    
#define CAN_RX_RTR_FRAME    	0b01000000    
#define CAN_RX_DBL_BUFFERED	0b10000000                            

// Parse-out 29-bit or 11-bit (saved in 32-bit number)

typedef union _CAN_MESSAGE_ID {
    unsigned long ID;

    struct {

        struct {
            unsigned SIDL : 3; // SIDL<5:7>
            unsigned SIDH : 5; // SIDH<0:4>
        } BYTE1;

        struct {
            unsigned SIDHU : 3; // SIDH<5:7>
            unsigned EIDL_LN : 5; // EIDL<0:4>
        } BYTE2;

        struct {
            unsigned EIDL_UN : 3; // EIDL<5:7>
            unsigned EIDH_LN : 5; // EIDH<0:4>
        } BYTE3;

        struct {
            unsigned EIDH_UN : 3; // EIDH<5:7>
            unsigned EIDHU : 2; // SIDL<0:1>
            unsigned : 3;
        } BYTE4;
    } ID_VALS;

    // This is to allow individual byte access within message id.

    struct {
        uint8_t BYTE_1;
        uint8_t BYTE_2;
        uint8_t BYTE_3;
        uint8_t BYTE_4;
    } BYTES;
} CAN_MESSAGE_ID;


/*********************************************************************
 * Macro:           ECANSetBaudRate(sjw,
 *                                 brp,
 *                                 phseg1,
 *                                 phseg2,
 *                                 propseg)
 *
 * Overview:        Use this macro to change ECAN baud rate. Normally
 *                  you would call this macro only, if your application
 *                  needs run-time baud rate changes.
 *
 * PreCondition:    ECAN is in Configuration mode
 *
 * Input:           sjw     - SJW value of 1-4
 *                  brp     - BRP value of 1-64
 *                  phseg1  - PHSEG1 value of 1-8
 *                  phseg2  - PHSEG2 value of 1-8
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Note:            If possible, call this macro with constant values
 *                  to reduce generated code size.
 ********************************************************************/
#if defined(__18F6680) 
#define ECANSetBaudRate(sjw, brp, phseg1, phseg2, propseg)           \
        BRGCON1 = ((sjw-1) << 6) | (brp-1);                         \
        BRGCON2 = (((phseg1-1) << 3) | (propseg-1));               \
        BRGCON3 = phseg2;
#else
#define ECANSetBaudRate(sjw, brp, phseg1, phseg2, propseg)           \
        BRGCON1 = ((sjw-1) << 6) | (brp-1);                         \
        BRGCON2 |= (((phseg1-1) << 3) | (propseg-1));               \
        BRGCON3 = phseg2;
#endif


// * * * Prototypes * * *
void vscp18f_init(BOOL bExtended);
void vscp18f_cansetmode(uint8_t opmode);
void vscp18f_setmask(uint8_t nmask, uint32_t val, BOOL bExtended);
void vscp18f_setfilter(uint8_t nfilter, uint32_t val, BOOL bExtended);
BOOL vscp18f_sendMsg(uint32_t id, uint8_t* data, uint8_t dlc, uint8_t flags);
BOOL vscp18f_readMsg(uint32_t *id, uint8_t *data, uint8_t *dlc, uint8_t *flags);

#endif
