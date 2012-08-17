 /**
 * @brief           IOR5E Expansion Board Driver.
 * @file            ior5e.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_ior5e
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

#define THIS_IS_IOR5E

#include "projdefs.h"
#include "ior5e.h"
#include "io.h"
#include "event.h"


/////////////////////////////////////////////////
// Defines
#define LATCH_SER       LATC0
#define LATCH_SCLK      LATC1
#define LATCH_RCLK      LATC2
#define LATCH_OE        LATC5

#define LATCH_SER_DIR   TRISC_RC0
#define LATCH_SCLK_DIR  TRISC_RC1
#define LATCH_RCLK_DIR  TRISC_RC2
#define LATCH_OE_DIR    TRISC_RC5

//Opto Coupled Inputs
#define OPTO1       PORTB_RB0
#define OPTO2       PORTB_RB1
#define OPTO3       PORTB_RB4
#define OPTO4       PORTB_RB5

#define OPTO1_DIR   TRISB_RB0
#define OPTO2_DIR   TRISB_RB1
#define OPTO3_DIR   TRISB_RB4
#define OPTO4_DIR   TRISB_RB5

//Defines used in evtLatchExIn and evtLatchExInUpdt variables
#define IO1_LAT_MASK    0x0001ul
#define IO2_LAT_MASK    0x0002ul
#define IO3_LAT_MASK    0x0004ul
#define IO4_LAT_MASK    0x0008ul
#define IO5_LAT_MASK    0x0010ul
#define IO6_LAT_MASK    0x0020ul
#define OPTO1_LAT_MASK  0x0040ul
#define OPTO2_LAT_MASK  0x0080ul
#define OPTO3_LAT_MASK  0x0100ul
#define OPTO4_LAT_MASK  0x0200ul


/////////////////////////////////////////////////
// Global variables.
union _IOR5E ior5eLatchData;
union _IOR5E ior5eLatchDataOld;

static BYTE adcChannel;
WORD ior5eAdcValues[IOR5E_ADC_CHANNELS];
static BYTE adcState;     //Current ADC State

//Each ADC channel has a filter consisting of 4 words. Each time the ADC is read, the result
//is placed in the next word. The actual ADC value is obtained by the sum of all words devided
//by 4.
static WORD adcFilter[IOR5E_ADC_CHANNELS][4];
static BYTE putAdcFilter;

static BYTE tmrSysLed;          //System LED timer


/////////////////////////////////////////////////
// Function Prototypes


/**
 * Initialize the IOR5E module
 */
void ior5eInit(void)
{
    BYTE i;
    
    //Configure all latch ports as outputs
    LATCH_SER_DIR  = 0;
    LATCH_SCLK_DIR = 0;
    LATCH_RCLK_DIR = 0;
    
    //Configure all Opto coupled input ports as inputs
    OPTO1_DIR = 1;
    OPTO2_DIR = 1;
    OPTO3_DIR = 1;
    OPTO4_DIR = 1;

	ior5eLatchDataOld.Val = 1;  //Set Old to different value than current, cuases an update
	ior5eLatchData.Val = 0;

    adcChannel = 0;

    //Indicate that we are waiting for the ADC aquisition time to finish
    //adcState |= ADC_AQUIS_WAIT;
    
    //Initialize all adcFilter registers and put pointer with 0
    for (i=0; i<IOR5E_ADC_CHANNELS; i++) {
        adcFilter[i][0] = 0;
        adcFilter[i][1] = 0;
        adcFilter[i][2] = 0;
        adcFilter[i][3] = 0;
    }
    putAdcFilter = 0;

    tmrSysLed = 0;
    
    //IO variables
    evtLatchExIn = 0;
    evtLatchExInUpdt = 0;
}


/**
 * Service this module. This code is called every
 */
void ior5eService(void)
{
    BYTE mask;
    BYTE i;
    BOOL bitVal;

    //Read Opto Inputs
    if (OPTO1 == ior5eLatchData.bits.ledOPTO1) {
        ior5eLatchData.bits.ledOPTO1 = !OPTO1;
    }
    if (OPTO2 == ior5eLatchData.bits.ledOPTO2) {
        ior5eLatchData.bits.ledOPTO2 = !OPTO2;
    }
    if (OPTO3 == ior5eLatchData.bits.ledOPTO3) {
        ior5eLatchData.bits.ledOPTO3 = !OPTO3;
    }
    if (OPTO4 == ior5eLatchData.bits.ledOPTO4) {
        ior5eLatchData.bits.ledOPTO4 = !OPTO4;
    }

    //Update IO Leds
    if (IOR5E_I1 != ior5eLatchData.bits.ledIN1) {
        ior5eLatchData.bits.ledIN1 = IOR5E_I1;
    }
    if (IOR5E_I2 != ior5eLatchData.bits.ledIN2) {
        ior5eLatchData.bits.ledIN2 = IOR5E_I2;
    }
    if (IOR5E_I3 != ior5eLatchData.bits.ledIN3) {
        ior5eLatchData.bits.ledIN3 = IOR5E_I3;
    }
    if (IOR5E_I4 != ior5eLatchData.bits.ledIN4) {
        ior5eLatchData.bits.ledIN4 = IOR5E_I4;
    }
    if (IOR5E_I5 != ior5eLatchData.bits.ledIN5) {
        ior5eLatchData.bits.ledIN5 = IOR5E_I5;
    }
    if (IOR5E_I6 != ior5eLatchData.bits.ledIN6) {
        ior5eLatchData.bits.ledIN6 = IOR5E_I6;
    }


    // Has something changed for IOR5E latches on PCB
    if (ior5eLatchData.Val != ior5eLatchDataOld.Val) {

        LATCH_RCLK = 0;

        for (i=0;i<3;i++)
        {
            mask = 0x80;

            //Shift all 8 bits into latch
           for (;;) {
                //Shift out next bit
                LATCH_SCLK = 0;
                if (ior5eLatchData.bytes[i] & mask)
                    LATCH_SER = 1;
                else
                    LATCH_SER = 0;
                LATCH_SCLK = 1;

                //Break if all 8 bits have been shifted out
                if (mask == 0x01) break;
                mask = mask >> 1;
            }

            LATCH_SCLK = 0;
        }

        //Clock 595's internal register
        LATCH_RCLK = 1;
        LATCH_RCLK = 0;

        //Now (after all output have been initialized with 0, Enable output latches
        LATCH_OE_DIR = 0;
        LATCH_OE = 0;
        ior5eLatchDataOld.Val = ior5eLatchData.Val; // remember for next time round
    }


    /////////////////////////////////////////////////
    //Event Handling
    //Check all inputs, and set event latch if any have changed
    // - If IO is configured as Input
    // - If Event Latch Update bit is 0. When 1, means it already contains updated value that has to be read

    //IO1
    if ( ( IOR5E_IO1_DIR==1) && ((IO1_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = IOR5E_I1;
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
    if ( ( IOR5E_IO2_DIR==1) && ((IO2_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = IOR5E_I2;
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
    if ( ( IOR5E_IO3_DIR==1) && ((IO3_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = IOR5E_I3;
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

    //IO4
    if ( ( IOR5E_IO4_DIR==1) && ((IO4_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = IOR5E_I4;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & IO4_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= IO4_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= IO4_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~IO4_LAT_MASK;   //Clear bit
        }
    }

    //IO5
    if ( ( IOR5E_IO5_DIR==1) && ((IO5_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = IOR5E_I5;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & IO5_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= IO5_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= IO5_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~IO5_LAT_MASK;   //Clear bit
        }
    }

    //IO6
    if ( ( IOR5E_IO6_DIR==1) && ((IO6_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = IOR5E_I6;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & IO6_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= IO6_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= IO6_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~IO6_LAT_MASK;   //Clear bit
        }
    }

    //OPTO1
    if ( ( OPTO1_DIR==1) && ((OPTO1_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = !OPTO1;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & OPTO1_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= OPTO1_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= OPTO1_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~OPTO1_LAT_MASK;   //Clear bit
        }
    }

    //OPTO2
    if ( ( OPTO2_DIR==1) && ((OPTO2_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = !OPTO2;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & OPTO2_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= OPTO2_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= OPTO2_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~OPTO2_LAT_MASK;   //Clear bit
        }
    }

    //OPTO3
    if ( ( OPTO3_DIR==1) && ((OPTO3_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = !OPTO3;
        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & OPTO3_LAT_MASK)!=0) ) {
            //Indicate latch has been updated
            evtLatchExInUpdt |= OPTO3_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= OPTO3_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~OPTO3_LAT_MASK;   //Clear bit
        }
    }

    //OPTO4
    if ( (OPTO4_DIR==1) && ((OPTO4_LAT_MASK & evtLatchExInUpdt)==0) ) {
        bitVal = !OPTO4;

        //Only update if latched value has changed
        if (bitVal != ((evtLatchExIn & OPTO4_LAT_MASK)!=0) ) {

            //Indicate latch has been updated
            evtLatchExInUpdt |= OPTO4_LAT_MASK;    //Set bit

            //Update Latch
            if (bitVal)
                evtLatchExIn |= OPTO4_LAT_MASK;    //Set bit
            else
                evtLatchExIn &= ~OPTO4_LAT_MASK;   //Clear bit
        }
    }
}


/**
 * Check if any events occured, and add them to all active event ports.
 *
 * @return Returns byte indicating what event ports had data added to them. Use EVT_BUF_XX defines
 */
BYTE ior5eAddEvents(void)
{
    BYTE ret;
    BYTE i;
    WORD mask;

    ret = 0;

    //UDP Event port is active
    if ( (activeEventPorts & EVT_PORT_UDP) && UDPIsPutReady(udpSocketEvt) ) {
        mask = 0x0001;

        //Check IO1 to IO6 inputs, and OPTO1 to OPTO4 opto inputs
        for(i=0; i<10; i++) {
            //Current latch has been updated
            if (mask & evtLatchExInUpdt) {

                //Clear latch updated flag - we have now serviced it
                evtLatchExInUpdt &= ~mask;    //Clear bit

                //Add event to UDP port
                ret |= EVT_PORT_UDP; //Indicate that data was added to UDP event port

                //Tags for expansion board Inputs 1 to 6 are: x40 to x45
                //Tags for expansion board Opto Inputs 1 to 4 are: x20 to x23
                UDPPut('x');
                if (i<6) {  //Inputs 1 to 6
                    UDPPut('4');
                    UDPPut('0'+i);
                }
                else {      //Opto Inputs 1 to 4
                    UDPPut('2');
                    UDPPut('0'+i-6);
                }
                UDPPut('=');
                UDPPut( ((evtLatchExIn & mask)==0)?'0':'1' );
                UDPPut(';');
            }

            //Rotate 1 position to left
            mask = mask << 1;
        }
    }

    return ret;
}


/**
 * Gets the requested 16 bit filtered ADC channel.
 *
 * @param chan The channel to get, a value from 1 to 5 for channels 1 to 5 of IOR5E
 *
 * @return Returns the 16 bit filtered ADC channel
 */
WORD ior5eGetAdc(BYTE chan)
{

    return 0;
}


/**
 * Get all 4 opto coupler inputs values
 *
 * @return The Opto Coupler values in bit 0-3
 */
BYTE ior5eGetOpto(void)
{
    BYTE ret;
    ret = 0;
    
    if (!OPTO1)
        ret |= 0x01;
    if (!OPTO2)
        ret |= 0x02;
    if (!OPTO3)
        ret |= 0x04;
    if (!OPTO4)
        ret |= 0x08;
        
    return ret;
}


/**
 * Set or clear given relay
 *
 * @param rly Offset of relay to perform action on. A value from 0-n. Relay 1 will be 0, relay 2 will
 *            be 1, ....
 *
 * @param value When 0, relay is switched of. When 1, relay is switched on
 */
void ior5eSetRly(BYTE rly, BOOL bVal)
{
    switch(rly) {
    case 0:
        ior5eLatchData.bits.ledRLY1 = bVal;
        ior5eLatchData.bits.rly1 = bVal;
        break;
    case 1:
        ior5eLatchData.bits.ledRLY2 = bVal;
        ior5eLatchData.bits.rly2 = bVal;
        break;
    case 2:
        ior5eLatchData.bits.ledRLY3 = bVal;
        ior5eLatchData.bits.rly3 = bVal;
        break;
    case 3:
        ior5eLatchData.bits.ledRLY4 = bVal;
        ior5eLatchData.bits.rly4 = bVal;
        break;
    case 4:
        ior5eLatchData.bits.ledRLY5 = bVal;
        ior5eLatchData.bits.rly5 = bVal;
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
void ior5eSetIO(BYTE ch, BOOL bVal)
{
    switch(ch) {
    case 0:
        ior5eLatchData.bits.ledIN1 = bVal;
        IOR5E_O1 = bVal;
        break;
    case 1:
        ior5eLatchData.bits.ledIN2 = bVal;
        IOR5E_O2 = bVal;
        break;
    case 2:
        ior5eLatchData.bits.ledIN3 = bVal;
        IOR5E_O3 = bVal;
        break;
    case 3:
        ior5eLatchData.bits.ledIN4 = bVal;
        IOR5E_O4 = bVal;
        break;
    case 4:
        ior5eLatchData.bits.ledIN5 = bVal;
        IOR5E_O5 = bVal;
        break;
    case 5:
        ior5eLatchData.bits.ledIN6 = bVal;
        IOR5E_O6 = bVal;
        break;
    }
}


/**
 * Gets the current state of all 6 IOs
 *
 * @return The IO values in bit 0-5. Bit 0 indicates state of IO 1, bit 1
 * indicated the state of IO2 2, ....
 */
BYTE ior5eGetIO(void) {


}



/**
 * Gets the current state of all 5 relays
 *
 * @return The relay values in bit 0-4. Bit 0 indicates state of relay 1, bit 1
 * indicated the state of relay 2, ....
 */
BYTE ior5eGetRly(void)
{
    return ior5eLatchData.bytes[2];
}

