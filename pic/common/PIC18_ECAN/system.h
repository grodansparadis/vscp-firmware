/******************************************************************************
*                                                                             *
*  File:                        SYSTEM.H                                      *
*                                                                             *
*  Description: This file contains the API for the PIC18F258 system           *
*               initialization, test, and control functions.                  *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Software License Agreement                                                 *
*                                                                             *
*  The software supplied herewith by Microchip Technology Incorporated        *
*  (the “Company”) for its PICmicro® Microcontroller is intended and          *
*  supplied to you, the Company’s customer, for use solely and                *
*  exclusively on Microchip PICmicro Microcontroller products. The            *
*  software is owned by the Company and/or its supplier, and is               *
*  protected under applicable copyright laws. All rights are reserved.        *
*   Any use in violation of the foregoing restrictions may subject the        *
*  user to criminal sanctions under applicable laws, as well as to            *
*  civil liability for the breach of the terms and conditions of this         *
*  license.                                                                   *
*                                                                             *
*  THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,          *
*  WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED          *
*  TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A                *
*  PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,          *
*  IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR                 *
*  CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.                          *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Created By:  Thomas R. Shelburne 09/30/02                                  *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*    1.0   - T.R.S. - Initial release of the system API definitions...        *
*                                                                             *
******************************************************************************/

#include "define.h"
#include <p18cxxx.h>
#include "stderror.h"

/* Only declare the API functions as external for calling modules... */
#ifdef  _SYSTEM_H_
#define GLOBAL
#else
#define GLOBAL extern
#endif

/* Global macro definitions... */
// #define GBL_OSC_CLOCK                  8000000 /* 8MHz oscillator...  */
// #define GBL_OSC_CLOCK                 16000000 /* 16MHz oscillator... */
// #define GBL_OSC_CLOCK                 20000000 /* 20MHz oscillator... */
#define GBL_OSC_CLOCK                 25000000 /* 25MHz oscillator... */
#define GBL_INSTRUCT_CLOCK            (((ULONG)GBL_OSC_CLOCK)/4)

/* Use these macros if IPEN = 0, interrupt bits have different functions based on IPEN value!!! */
#define GBL_INTR_ENABLE               INTCONbits.GIE  = 1
#define GBL_INTR_DISABLE              INTCONbits.GIE  = 0
#define PERIPHERAL_INTR_ENABLE        INTCONbits.PEIE = 1
#define PERIPHERAL_INTR_DISABLE       INTCONbits.PEIE = 0

/* Use these macros if IPEN = 1, interrupt bits have different functions based on IPEN value!!! */
#define GBL_HI_INTR_ENABLE            INTCONbits.GIEH = 1
#define GBL_HI_INTR_DISABLE           INTCONbits.GIEH = 0
#define GBL_LO_INTR_ENABLE            INTCONbits.GIEL = 1
#define GBL_LO_INTR_DISABLE           INTCONbits.GIEL = 0

/* Global pin control macros... */
#define INCR_SWITCH                   0x20
#define READ_INCR_SWITCH              (PORTB & INCR_SWITCH)

#define DECR_SWITCH                   0x10
#define READ_DECR_SWITCH              (PORTB & DECR_SWITCH)

#define LED_8                         0x04
#define LED_8_HIGH                    PORTCbits.RC2 = 1
#define LED_8_LOW                     PORTCbits.RC2 = 0

#define LED_7_HIGH                    PORTDbits.RD7 = 1
#define LED_7_LOW                     PORTDbits.RD7 = 0
#define LED_6_HIGH                    PORTDbits.RD6 = 1
#define LED_6_LOW                     PORTDbits.RD6 = 0
#define LED_5_HIGH                    PORTDbits.RD5 = 1
#define LED_5_LOW                     PORTDbits.RD5 = 0
#define LED_4_HIGH                    PORTDbits.RD4 = 1
#define LED_4_LOW                     PORTDbits.RD4 = 0
#define LED_3_HIGH                    PORTDbits.RD3 = 1
#define LED_3_LOW                     PORTDbits.RD3 = 0
#define LED_2_HIGH                    PORTDbits.RD2 = 1
#define LED_2_LOW                     PORTDbits.RD2 = 0
#define LED_1_HIGH                    PORTDbits.RD1 = 1
#define LED_1_LOW                     PORTDbits.RD1 = 0
#define LED_0_HIGH                    PORTDbits.RD0 = 1
#define LED_0_LOW                     PORTDbits.RD0 = 0

/* Global function declarations... */
GLOBAL void system_init(void);
GLOBAL STDERROR system_test(void);

#undef GLOBAL
