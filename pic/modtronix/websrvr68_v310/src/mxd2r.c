 /**
 * @brief           MXD2R Expansion Board Driver.
 * @file            mxd2r.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_mxd2r
 *
 *********************************************************************/

 /*********************************************************************
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
 **********************************************************************
 * File History
 *
 * 2009-04-22, David Hosken (DH):
 *    - Initial code
 *********************************************************************/

#define THIS_IS_MXD2R

#include "projdefs.h"
#include "mxd2r.h"
#include "io.h"
#include "event.h"


/////////////////////////////////////////////////
// Defines

//Relays
#define MXD2R_RLY1   LATB4
#define MXD2R_RLY2   LATB5

#define TRIS_RLY1  TRISB_RB4
#define TRIS_RLY2  TRISB_RB5

//I/O ports
#define OPTO1       PORTB_RB0

#define MXD2R_I1    PORTA_RA0
#define MXD2R_I2    PORTA_RA1
#define MXD2R_I3    PORTA_RA2
#define MXD2R_O1    LATA0
#define MXD2R_O2    LATA1
#define MXD2R_O3    LATA2
#define MXD2R_O4    LATC2
#define MXD2R_O5    LATC1
#define MXD2R_O6    LATC0

//I/O port direction
#define MXD2R_IO1_DIR   TRISA_RA0
#define MXD2R_IO2_DIR   TRISA_RA1
#define MXD2R_IO3_DIR   TRISA_RA2

//Defines used in evtLatchExIn and evtLatchExInUpdt variables
#define IO1_LAT_MASK    0x0001ul
#define IO2_LAT_MASK    0x0002ul
#define IO3_LAT_MASK    0x0004ul


/////////////////////////////////////////////////
// Global variables.
static BYTE adcChannel;
WORD mxd2rAdcValues[MXD2R_ADC_CHANNELS];
static BYTE adcState;     //Current ADC State

//Each ADC channel has a filter consisting of 4 words. Each time the ADC is read, the result
//is placed in the next word. The actual ADC value is obtained by the sum of all words devided
//by 4.
static WORD adcFilter[MXD2R_ADC_CHANNELS][4];
static BYTE putAdcFilter;


/////////////////////////////////////////////////
// Function Prototypes


/**
 * Initialize the MXD2R module
 */
void mxd2rInit(void)
{
    BYTE i;
    
    //Relay ports are outputs
    TRIS_RLY1 = 0;
    TRIS_RLY2 = 0;

    //Initialize all adcFilter registers and put pointer with 0
    for (i=0; i<MXD2R_ADC_CHANNELS; i++) {
        adcFilter[i][0] = 0;
        adcFilter[i][1] = 0;
        adcFilter[i][2] = 0;
        adcFilter[i][3] = 0;
    }
    putAdcFilter = 0;

    //IO variables
    evtLatchExIn = 0;
    evtLatchExInUpdt = 0;
}

/**
 * Service this module. This code is called every
 */
void mxd2rService(void)
{
    BOOL bitVal;

    /////////////////////////////////////////////////
    //Event Handling
    //Check all inputs, and set event latch if any have changed
    // - If IO is configured as Input
    // - If Event Latch Update bit is 0. When 1, means it already contains updated value that has to be read

    //IO1
    if ( (MXD2R_IO1_DIR==1) && ((IO1_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = MXD2R_I1;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & IO1_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= IO1_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= IO1_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~IO1_LAT_MASK;   //Clear bit
        }
    }


    //IO2
    if ( (MXD2R_IO2_DIR==1) && ((IO2_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = MXD2R_I2;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & IO2_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= IO2_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= IO2_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~IO2_LAT_MASK;   //Clear bit
        }
    }


    //IO3
    if ( (MXD2R_IO3_DIR==1) && ((IO3_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = MXD2R_I3;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & IO3_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= IO3_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= IO3_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~IO3_LAT_MASK;   //Clear bit
        }
    }
}


/**
 * Check if any events occured, and add them to all active event ports.
 *
 * @return Returns byte indicating what event ports had data added to them. Use EVT_BUF_XX defines
 */
BYTE mxd2rAddEvents(void)
{
    BYTE ret = 0;

    //UDP Event port is active
    if ( (activeEventPorts & EVT_PORT_UDP) && UDPIsPutReady(udpSocketEvt) ) {
    //if ( ((activeEventPorts & EVT_PORT_UDP)!=0) && UDPIsPutReady(udpSocketEvt) ) {

        //IO1 latch has been updated
        if ((IO1_LAT_MASK & evtLatchExInUpdt) != 0) {
            //Clear latch updated flag - we have now serviced it
            evtLatchExInUpdt &= ~IO1_LAT_MASK;    //Set bit

            //Add event to UDP port

            ret |= EVT_PORT_UDP; //Indicate that data was added to UDP event port

            //Tag for expansion board input 1 = x40
            UDPPut('x');
            UDPPut('4');
            UDPPut('0');
            UDPPut('=');
            UDPPut( ((evtLatchExIn&IO1_LAT_MASK)==0)?'0':'1' );
            UDPPut(';');
        }

        //IO2 latch has been updated
        if ((IO2_LAT_MASK & evtLatchExInUpdt) != 0) {
            //Clear latch updated flag - we have now serviced it
            evtLatchExInUpdt &= ~IO2_LAT_MASK;    //Set bit

            //Add event to UDP port

            ret |= EVT_PORT_UDP; //Indicate that data was added to UDP event port

            //Tag for expansion board input 2 = x41
            UDPPut('x');
            UDPPut('4');
            UDPPut('1');
            UDPPut('=');
            UDPPut( ((evtLatchExIn&IO2_LAT_MASK)==0)?'0':'1' );
            UDPPut(';');
        }

        //IO3 latch has been updated
        if ((IO3_LAT_MASK & evtLatchExInUpdt) != 0) {
            //Clear latch updated flag - we have now serviced it
            evtLatchExInUpdt &= ~IO3_LAT_MASK;    //Set bit

            //Add event to UDP port

            ret |= EVT_PORT_UDP; //Indicate that data was added to UDP event port

            //Tag for expansion board input 3 = x42
            UDPPut('x');
            UDPPut('4');
            UDPPut('2');
            UDPPut('=');
            UDPPut( ((evtLatchExIn&IO3_LAT_MASK)==0)?'0':'1' );
            UDPPut(';');
        }
    }

    return ret;
}


/**
 * Gets the requested 16 bit filtered ADC channel.
 *
 * @param chan The channel to get, a value from 1 to 5 for channels 1 to 5 of MXD2R
 *
 * @return Returns the 16 bit filtered ADC channel
 */
WORD mxd2rGetAdc(BYTE chan)
{
    return 0;
}


/**
 * Set or clear given relay
 *
 * @param rly Offset of relay to perform action on. A value from 0-n. Relay 1 will be 0, relay 2 will
 *            be 1, ....
 *
 * @param value When 0, relay is switched of. When 1, relay is switched on
 */
void mxd2rSetRly(BYTE rly, BOOL bVal)
{
    switch(rly) {
    case 0:
        TRIS_RLY1 = 0;
        MXD2R_RLY1 = bVal;
        break;
    case 1:
        TRIS_RLY2 = 0;
        MXD2R_RLY2 = bVal;
        break;
    }
}

/**
 * Set or clear given IO port. Is only done if this IO port is current set as an ouput
 *
 * @param ch Offset of IO to perform action on. A value from 0-n. IO1 will be 0, IO2 will be 1, ....
 *
 * @param value When 0, relay is switched of. When 1, relay is switched on
 */
void mxd2rSetIO(BYTE ch, BOOL bVal)
{
    switch(ch) {
    case 0:
        MXD2R_O1 = bVal;
        break;
    case 1:
        MXD2R_O2 = bVal;
        break;
    case 2:
        MXD2R_O3 = bVal;
        break;
    case 3:
        MXD2R_O4 = bVal;
        break;
    case 4:
        MXD2R_O5 = bVal;
        break;
    case 5:
        MXD2R_O6 = bVal;
        break;
    }
}



/**
 * Gets the current state of all 5 relays
 *
 * @return The relay values in bit 0-4. Bit 0 indicates state of relay 1, bit 1
 * indicated the state of relay 2, ....
 */
BYTE mxd2rGetRly(void)
{
    return (MXD2R_RLY1 | (MXD2R_RLY2<<1));
}

