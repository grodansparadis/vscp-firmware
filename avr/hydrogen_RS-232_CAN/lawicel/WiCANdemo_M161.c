/*
**  File:       WiCANdemo_M161.C
**
**  Purpose:    Demonstrate WiCAN LIB for SJA1000 in Demo mode
**
**  Chip:       ATMega161
**
**  Version:    1.0.4, 17:th of September 2002
**
**  Author:     Lars Wictorsson
**              LAWICEL / SWEDEN
**              http://www.lawicel.com   lars@lawicel.com
**
**  Copyright:  The copyright to the computer program(s) herein is the
**              property of LAWICEL HB, Sweden. The program(s) may be used
**              and/or copied only with the written permission of LAWICEL HB
**              in accordance with the terms and conditions stipulated in
**              the agreement/contract under which the program(s) have been
**              supplied.
**
**  Remarks:    This program is tested with ICCAVR version 6.25C.
**
**  History:    2000-05-18  1.0.0   Created (LWI)
**              2001-05-06  1.0.1   Modified to test 29bit (LWI)
**              2001-05-14  1.0.2   Modified to test RTR (LWI)
**              2002-01-23  1.0.3   Re-Compiled with latest ICCAVR (LWI)
**              2002-09-17  1.0.4   Added support for M161 (LWI)
*/

#include <iom161v.h>

#include <stdio.h>
#include "WiCAN.h"

WiCAN_Object MyFrame;

void main( void)
{
/*
**  Setup CAN controller
*/
    if ((WiCAN_Init(WiCAN_SPEED_20K, 0, 0, 0xDE)) == WiCAN_OK) {
        WiCAN_Start();

        MyFrame.id      = 0x500;                    // Set up a standard 11-bit ID Frame with
        MyFrame.len     = 1;                        // DLC = 1 (i.e. 1 byte)
        MyFrame.byte[0] = 0x01;                     // The first byte to '0x01'
        WiCAN_SendFrame(&MyFrame);                  // Send the frame

        MyFrame.id      = 0x510;                    // Set new ID 
        MyFrame.flags   = WiCAN_29;                 // Change the frame type to Extended
        MyFrame.byte[0]++;                          // Increase the value of byte 1
        WiCAN_SendFrame(&MyFrame);                  // Send the frame again

        MyFrame.id      = 0x520;                    // Set new ID 
        MyFrame.flags   = WiCAN_RTR;                // Change the STD frame type and set RTR bit
        WiCAN_SendFrame(&MyFrame);                  // Send the frame again (notice DLC is set to zero when sending)

        while(1) {                                  // Do a forever loop
            if (WiCAN_GetRxQueueSize() >= 4) {      // Wait until input FIFO has 4 messages or more
                while (WiCAN_GetFrame(&MyFrame) == WiCAN_OK) {
                    WiCAN_SendFrame(&MyFrame);      // Send all of them back as they came in.
                }
            }
        }
    }
}

