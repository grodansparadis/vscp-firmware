/*********************************************************************
 * appcfg.c    -    Application Configuration functions
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Application configuration.
 *
 *********************************************************************
 * File History
 *
 * 2004-10-19, David Hosken (DH):
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
#define THIS_IS_APPCFG_MODULE

#include "projdefs.h"
#include "appcfg.h"
#include "debug.h"

/////////////////////////////////////////////////
//Variables
APP_CONFIG AppConfig;   //This is used by other stack elements.


/**
 * @preCondition  fsysInit() is already called.
 */
void appcfgInit(void)
{
    BYTE *p;

    /////////////////////////////////////////////////
    //Load configuration from EEPROM to RAM
    //Read contents of BLCONNFIG structure from EEPROM. It is located at the start of the EEPROM
    EEADRH = 0;
    EEADR = 0;
    
    //Configure for EEPROM reading, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x3F;

    p = (BYTE*)&AppConfig;

    for ( ; EEADR < sizeof(AppConfig); EEADR++ ) {
        EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.
        *p++ = EEDATA;
    }
}

//This contains the default values of the APP_CONFIG structure, and has to be placed at off
//0 of the EEPROM = 0xf00000.
//To do this with HiTech PICC18:
//  add "-L-Peeconfig=f00000h" to the linker command line (without the " characters)
//To do this with MPLAB C18 compiler:
//  nothing has to be done!
//
#if defined(HI_TECH_C)  //Hi-Tech compiler
    #pragma psect const=eeconfig
#elif defined(__18CXX)  //MPLAB C18 compiler
    #pragma romdata EESECT=0xf00000
    //#pragma romdata eeconfig=0xf00000
#endif
ROM BYTE eeconfigArray[] = {
    //MyIPAddr
    MY_DEFAULT_IP_ADDR_BYTE1,   //Offset 0
    MY_DEFAULT_IP_ADDR_BYTE2,
    MY_DEFAULT_IP_ADDR_BYTE3,
    MY_DEFAULT_IP_ADDR_BYTE4,
    //MyMACAddr
    MY_DEFAULT_MAC_BYTE1,       //Offset 4
    MY_DEFAULT_MAC_BYTE2,
    MY_DEFAULT_MAC_BYTE3,
    MY_DEFAULT_MAC_BYTE4,
    MY_DEFAULT_MAC_BYTE5,
    MY_DEFAULT_MAC_BYTE6,
    //MyMask
    MY_DEFAULT_MASK_BYTE1,      //Offset 10
    MY_DEFAULT_MASK_BYTE2,
    MY_DEFAULT_MASK_BYTE3,
    MY_DEFAULT_MASK_BYTE4,
    //MyGateway
    MY_DEFAULT_GATE_BYTE1,      //Offset 14
    MY_DEFAULT_GATE_BYTE2,
    MY_DEFAULT_GATE_BYTE3,
    MY_DEFAULT_GATE_BYTE4,
    //wait4bl - Delay how long we wait for the bootloader, value from 0-n. Time = 0.8sec x n
    //Default value = 2.4
    4,
    //flashHasProgram - If 1, the FLASH has been programmed and contain a valid program.
    1
};

