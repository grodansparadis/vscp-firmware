/*********************************************************************
 * serbasic.h    -  Simple USART1 routines
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Simple USART1 routines
 *
 *********************************************************************
 * File History
 *
 * 2005-01-07, David Hosken (DH):
 *    - Initial version
 *
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/
#ifndef SERBASIC_H
#define SERBASIC_H

#include "projdefs.h"

#define serInit()   ()  /* Nothing to be done for this serial port implementation */

/**
 * Transmit the given byte on the USART. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutByte(BYTE c);

/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutString(BYTE* s);


/**
 * Transmit a NULL terminated string. It is added to the transmit buffer, and asynchronously
 * transmitted.
 */
void serPutRomString(ROM char* str);

#define serIsGetReady()   (PIR1_RCIF)

#define serGetByte()          (RCREG)

#endif
