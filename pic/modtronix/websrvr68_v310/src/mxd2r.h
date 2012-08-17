/**
 * @brief           MXD2R Expansion Board Driver.
 * @file            mxd2r.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_mxd2r
 *
 *
 * @section description Description
 **********************************
 * This module implements a driver for the MXD2R Input/Output/Relay board.
 *  To use this module:
 *  - Customize defines specified in "MXD2R Configuration".
 *  - Call mxd2rInit() to initialize module
 *  - Use available functions
 *
 *
 * @subsection mxd2r_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required
 * defines should be uncommended, and the rest commented out.
 @code
 //*********************************************************************
 //-------------- MXD2R Configuration --------------------
 //*********************************************************************
 //Define ...
 //#define MXD2R_....

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
 * @defgroup mod_sys_mxd2r MXD2R Input Output Relay Expansion Board
 * @ingroup mod_sys
 *********************************************************************/


#ifndef _MXD2R_H_
#define _MXD2R_H_


/////////////////////////////////////////////////
//Defines
#define MXD2R_ADC_CHANNELS  3   // There are 3 ADC channels
#define MXD2R_OPTO_INPUTS   0   // Opto Coupler Inputs
#define MXD2R_RELAYS        2   // Number of relays


//This is used by other application modules
#ifndef THIS_IS_MXD2R_MODULE

/**
 * Global Array containing converted ADC values. This array can be accessed from
 * anywhere to obtain the current value of the ADC inputs. For example, to get
 * the current value of AN2 ADC, use mxd2rAdcValues[2]
 */
extern WORD mxd2rAdcValues[MXD2R_ADC_CHANNELS];

#endif



/////////////////////////////////////////////////
//Function prototypes


/**
 * Initialize the MXD2R module
 */
void mxd2rInit(void);


/**
 * Service this module
 */
void mxd2rService(void);


/**
 * Gets the requested 16 bit filtered ADC channel.
 *
 * @param chan The channel to get, a value from 1 to 5 for channels 1 to 5 of MXD2R
 *
 * @return Returns the 16 bit filtered ADC channel
 */
WORD mxd2rGetAdc(BYTE chan);


/**
 * Set or clear relay1.
 *
 * @param value When 0, relay 1 is switched of. When 1, relay 1 is switched on
 */
#define mxd2rSetRly1(value) mxd2rSetRly(0, value)


/**
 * Set or clear relay2.
 *
 * @param value When 0, relay 2 is switched of. When 1, relay 2 is switched on
 */
#define mxd2rSetRly2(value) mxd2rSetRly(1, value)


/**
 * Set or clear given relay
 *
 * @param rly Offset of relay to perform action on. A value from 0-n. Relay 1 will be 0, relay 2 will
 *            be 1, ....
 *
 * @param bVal When 0, relay is switched of. When 1, relay is switched on
 */
void mxd2rSetRly(BYTE rly, BOOL bVal);

/**
 * Set or clear given IO port. Is only done if this IO port is current set as an ouput
 *
 * @param ch Offset of IO to perform action on. A value from 0-n. IO1 will be 0, IO2 will be 1, ....
 *
 * @param bVal When 0, relay is switched of. When 1, relay is switched on
 */
void mxd2rSetIO(BYTE ch, BOOL bVal);


/**
 * Gets the current state of both relays
 *
 * @return The relay values in bit 0-1. Bit 0 indicates state of relay 1, bit 1
 * indicated the state of relay 2.
 */
BYTE mxd2rGetRly(void);


/**
 * Gets the current state of all 6 IOs
 *
 * @return The IO values in bit 0-5. Bit 0 indicates state of IO 1, bit 1
 * indicated the state of IO2 2, ....
 */
BYTE mxd2rGetIO(void);

/**
 * Check if any events occured, and add them to all active event ports.
 *
 * @return Returns byte indicating what event ports had data added to them. Use EVT_BUF_XX defines
 */
BYTE mxd2rAddEvents(void);

#endif    //_MXD2R_H_




