/**
 * @brief           IOR5E Expansion Board Driver.
 * @file            ior5e.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_ior5e
 *
 *
 * @section description Description
 **********************************
 * This module implements a driver for the IOR5E Input/Output/Relay board.
 *  To use this module:
 *  - Customize defines specified in "IOR5E Configuration".
 *  - Call ior5eInit() to initialize module
 *  - Use available functions
 *
 *
 * @subsection ior5e_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- IOR5E Configuration --------------------
 //*********************************************************************
 //Define ...
 //#define IOR5E_....

 @endcode
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

/**
 * @defgroup mod_sys_ior5e IOR5E Input Output Relay Expansion Board
 * @ingroup mod_sys
 *********************************************************************/


#ifndef _IOR5E_H_
#define _IOR5E_H_


/////////////////////////////////////////////////
//Defines
#define IOR5E_ADC_CHANNELS  5   // There are 5 ADC channels
#define IOR5E_OPTO_INPUTS   4   // Opto Coupler Inputs
#define IOR5E_RELAYS        5   // Number of relays

//I/O ports when used as outputs
#define IOR5E_O1    LATA0
#define IOR5E_O2    LATA1
#define IOR5E_O3    LATA2
#define IOR5E_O4    LATA3
#define IOR5E_O5    LATA5
#define IOR5E_O6    LATA4

//I/O ports when used as inputs
#define IOR5E_I1    PORTA_RA0
#define IOR5E_I2    PORTA_RA1
#define IOR5E_I3    PORTA_RA2
#define IOR5E_I4    PORTA_RA3
#define IOR5E_I5    PORTA_RA5
#define IOR5E_I6    PORTA_RA4

//I/O ports direction
#define IOR5E_IO1_DIR   TRISA_RA0
#define IOR5E_IO2_DIR   TRISA_RA1
#define IOR5E_IO3_DIR   TRISA_RA2
#define IOR5E_IO4_DIR   TRISA_RA3
#define IOR5E_IO5_DIR   TRISA_RA5
#define IOR5E_IO6_DIR   TRISA_RA4

typedef union _IOR5E
{
    struct
    {
        unsigned int ledPWR : 1;
        unsigned int ledRLY1 : 1;
        unsigned int ledRLY4 : 1;
        unsigned int ledRLY2 : 1;
        unsigned int ledRLY5 : 1;
        unsigned int ledRLY3 : 1;
        unsigned int ledIN4 : 1;
        unsigned int ledIN1 : 1;

        unsigned int ledIN5 : 1;
        unsigned int ledIN2 : 1;
        unsigned int ledIN6 : 1;
        unsigned int ledIN3 : 1;
        unsigned int ledOPTO3 : 1;
        unsigned int ledOPTO1 : 1;
        unsigned int ledOPTO4 : 1;
        unsigned int ledOPTO2 : 1;
        
        unsigned int rly1 : 1;
        unsigned int rly2 : 1;
        unsigned int rly3 : 1;
        unsigned int rly4 : 1;
        unsigned int rly5 : 1;
        unsigned int unused1 : 1;
        unsigned int unused2 : 1;
        unsigned int unused3 : 1;
    } bits;
    unsigned short long Val;
    unsigned char bytes[3];
} ;


//This is used by other application modules
#ifndef THIS_IS_IOR5E_MODULE

/**
 * Global Array containing converted ADC values. This array can be accessed from
 * anywhere to obtain the current value of the ADC inputs. For example, to get
 * the current value of AN2 ADC, use ior5eAdcValues[2]
 */
extern WORD ior5eAdcValues[IOR5E_ADC_CHANNELS];

extern union _IOR5E ior5eLatchData;
extern union _IOR5E ior5eLatchDataOld;

#endif



/////////////////////////////////////////////////
//Function prototypes


/**
 * Initialize the IOR5E module
 */
void ior5eInit(void);


/**
 * Service this module
 */
void ior5eService(void);


/**
 * Gets the requested 16 bit filtered ADC channel.
 *
 * @param chan The channel to get, a value from 1 to 5 for channels 1 to 5 of IOR5E
 *
 * @return Returns the 16 bit filtered ADC channel
 */
WORD ior5eGetAdc(BYTE chan);


/**
 * Get all 4 opto coupler inputs values
 *
 * @return The Opto Coupler values in bit 0-3
 */
BYTE ior5eGetOpto(void);


/**
 * Set or clear relay1.
 *
 * @param value When 0, relay 1 is switched of. When 1, relay 1 is switched on
 */
#define ior5eSetRly1(value) ior5eSetRly(0, value)


/**
 * Set or clear relay2.
 *
 * @param value When 0, relay 2 is switched of. When 1, relay 2 is switched on
 */
#define ior5eSetRly2(value) ior5eSetRly(1, value)


/**
 * Set or clear relay3.
 *
 * @param value When 0, relay 3 is switched of. When 1, relay 3 is switched on
 */
#define ior5eSetRly3(value) ior5eSetRly(2, value)


/**
 * Set or clear relay4.
 *
 * @param value When 0, relay 4 is switched of. When 1, relay 4 is switched on
 */
#define ior5eSetRly4(value) ior5eSetRly(3, value)


/**
 * Set or clear relay5.
 *
 * @param value When 0, relay 5 is switched of. When 1, relay 5 is switched on
 */
#define ior5eSetRly5(value) ior5eSetRly(4, value)


/**
 * Set or clear given relay
 *
 * @param rly Offset of relay to perform action on. A value from 0-n. Relay 1 will be 0, relay 2 will
 *            be 1, ....
 *
 * @param bVal When 0, relay is switched of. When 1, relay is switched on
 */
void ior5eSetRly(BYTE rly, BOOL bVal);


/**
 * Gets the current state of all 5 relays
 *
 * @return The relay values in bit 0-4. Bit 0 indicates state of relay 1, bit 1
 * indicated the state of relay 2, ....
 */
BYTE ior5eGetRly(void);


/**
 * Set or clear given IO port. Is only done if this IO port is current set as an ouput
 *
 * @param ch Offset of IO to perform action on. A value from 0-n. IO1 will be 0, IO2 will be 1, ....
 *
 * @param bVal When 0, relay is switched of. When 1, relay is switched on
 */
void ior5eSetIO(BYTE ch, BOOL bVal);


/**
 * Gets the current state of all 6 IOs
 *
 * @return The IO values in bit 0-5. Bit 0 indicates state of IO 1, bit 1
 * indicated the state of IO2 2, ....
 */
BYTE ior5eGetIO(void);

/**
 * Check if any events occured, and add them to all active event ports.
 *
 * @return Returns byte indicating what event ports had data added to them. Use EVT_BUF_XX defines
 */
BYTE ior5eAddEvents(void);



#endif    //_IOR5E_H_




