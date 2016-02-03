/*********************************************************************
 *
 *                  General Delay rouines
 *
 *********************************************************************
 * FileName:        delay.c
 * Dependencies:    compiler.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -Zg -E -C
 *
 * Note: HITECH portions is adopted from PICC18 sample "delay.c".
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/9/02  Original        (Rev 1.0)
 ********************************************************************/

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\delay.h"

#define DUMMY

/**
 * Delays for the given time.
 * @param us    Time to delay = given time x 10us
 */
void Delay10us(unsigned char us)
{
    unsigned char i;

    while (us--)
    {
        #if (CLOCK_FREQ == 40000000)
            i = 13;
        #elif (CLOCK_FREQ == 20000000)
            i = 6;
        #else
            #error "Delay does not suppurt current Clock Frequency"
        #endif

        //Delay 10us
        //This takes 6 Instuction cycles per loop
        while (i != 0) {
            i--;
        }
    }
}

/**
 * Delays for the given time.
 * @param ms Time to delay = given time x 1ms
 */
void DelayMs(unsigned char ms)
{
    #if defined(HITECH_C18)
        while (ms--)
        {
            Delay10us(99);
            FAST_USER_PROCESS();
        }
    #elif defined(MCHP_C18)
        while (ms--)
        {
            #if (CLOCK_FREQ == 40000000)
                Delay1KTCYx(8); //Delay 1ms
            #elif (CLOCK_FREQ == 20000000)
                Delay1KTCYx(4); //Delay 1ms
            #endif
            FAST_USER_PROCESS();
        }
    #endif
}




