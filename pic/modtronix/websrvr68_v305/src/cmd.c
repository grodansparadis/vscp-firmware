 /**
 * @brief           Commands
 * @file            cmd.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
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
 * 2006-09-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#define THIS_IS_CMD_MODULE

#include <string.h>

#include "projdefs.h"
#include "cmd.h"
#include "debug.h"
#include "appcfg.h"

#include "net\helpers.h"
#include "net\security.h"   //Security related code


/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xDA, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xDA, msgCode, msgStr)


/**
 * Execute the given name-value command. The command might not be executed if a higher user level is
 * required.
 *
 * @param name The name part of the name-value command
 * @param value The value part of the name-value command
 * @param user The user level, is a HTTP_USER_XX constant
 *
 * @return Returns an CMD_UD_XXX flag (or multiple ones ORed together) indicating what has been updated. Not
 * all name-value commands will return a value here.
 */
WORD execNameValueCmd(BYTE * name, BYTE * value, BYTE user) {
    WORD wUpdate;   //Contains CMD_UD_XXX flags indicating what has to be updated
    BYTE* p;        //Temperory pointer
    BYTE tmp;
    BYTE i;
    BYTE eeVal;

    wUpdate = 0;    //Clear all update flags

    /////////////////////////////////////////////////
    //Update Configuration value
    if (name[0] == CMDGROUP_CONFIG) {
        BYTE cfgOffset; //Offset of configuration byte
        BYTE cfgValue;  //New value of configuration byte

        tmp = 1;
        
        //The name part of the name-value pair will be "knn" where nn is the offset (decimal) of the
        //configuration byte to be updated. Thus, name[1] will point to number repesenting the offset.
        //Get offset of configuration byte to be modified - is in hex format
        if ( !hextob( (char *)&name[1], &cfgOffset ) ) {
            #if (DEBUG_CMD >= LOG_WARN)
            debugPutMsg(1);     //@mxd:1:Invalid offset for configuration byte = %s
            debugPutByte(name[1]);
            debugPutByte(name[2]);
            debugPutByte(0);    //Null terminate string
            #endif

            return 0;   //Return 0 indicating nothing has been updated
        }
        
        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(2);     //@mxd:2:Received Command to modify configuration byte 0x%x
        debugPutByteHex(cfgOffset);
        #endif

        //The config value is a decimal value
        cfgValue = (BYTE)atoi((char *)value);
        
        //Request a network setting to be changed
        if ((cfgOffset <= APPCFG_GATEWAY3) || (cfgOffset == APPCFG_NETFLAGS))
        {
            wUpdate |= CMD_UD_NETWORK;  //Indicate the network settings must be updated
            tmp = 0;                //Indicate that something must be updated
        }
        //Request a system setting to be changed (FLASH_HAS_PROGRAM byte can NOT be changed)
        else if (cfgOffset <= APPCFG_SYSFLAGS) {
            if (cfgOffset != APPCFG_FLASH_HAS_PROGRAM) {
                wUpdate |= CMD_UD_SYSTEM;   //Indicate the System settings must be updated
                tmp = 0;                //Indicate that something must be updated
            }
        }
        //Request a SMTP setting to be changed
        else if (cfgOffset <= APPCFG_SMTP_SRVR_ADR3) {
            #if defined(STACK_USE_SMTP)
            wUpdate |= CMD_UD_SMTP; //Indicate the SMTP settings must be updated
            tmp = 0;            //Indicate that something must be updated
            #endif
        }
        //Request a TFTP setting to be changed
        else if (cfgOffset <= APPCFG_TFTP_SRVR_ADR3) {
            #if defined(STACK_USE_TFTP_CLIENT)
            wUpdate |= CMD_UD_TFTP; //Indicate the TFTP settings must be updated
            tmp = 0;            //Indicate that something must be updated
            #endif
        }
        //Request a USART setting to be changed
        else if (cfgOffset <= APPCFG_USART2_CFG) {
            //New baud rate for USART1
            if ((cfgOffset == APPCFG_USART1_BAUD) || (cfgOffset == APPCFG_USART2_BAUD)) {
                //Check for valid value
                if (cfgValue <= BAUD_115200) {
                    tmp = 0;            //Indicate that something must be updated
                }
            }
            //Accept all values for APPCFG_USART1_CFG and APPCFG_USART2_CFG
            else {
                tmp = 0;
            }
            //Something must be updated
            if (tmp == 0) {
                wUpdate |= CMD_UD_USART;    //Indicate the USART settings must be updated
            }
        }
        //Request a PORT setting to be changed. This can be a port direction setting OR a port default value setting
        else if (cfgOffset <= APPCFG_PORTG) {
            wUpdate |= CMD_UD_CPU_IO;   //Indicate the PORT settings must be updated
            tmp = 0;                //Indicate that something must be updated
        }
        //Request a ADC setting to be changed
        else if (cfgOffset <= APPCFG_ADCON0_2) {
            wUpdate |= CMD_UD_ADC;   //Indicate the PORT settings must be updated
            tmp = 0;             //Indicate that something must be updated
        }

        //Must anything be updated? Guest is NOT allowed to update these values!
        if ((tmp==0) && (user != USER_GUEST)) {
            appcfgPutc(cfgOffset, cfgValue);
        }
    }
    /////////////////////////////////////////////////
    //We received a request to set or clear a PORT bit
    //The name part of the name-value pair will be "ann" to "gnn" where nn is the offset (decimal) of the
    //configuration byte to be updated. Thus, name[1] will point to number repesenting the offset.
    else if ((name[0] >= CMDGROUP_PORTA) && (name[0] <= CMDGROUP_PORTG))
    {
        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(3);     //@mxd:3:Received Command to modify port pin
        #endif
        
        //NOT allowed to modify port D and E!
        if ((name[0] == CMDGROUP_PORTD) || (name[0] == CMDGROUP_PORTE)) {
            return 0;   //Return 0 indicating nothing has been updated
        }

        //Get pointer to requested LATCH register
        p = (BYTE *)(&LATA) + (name[0] - CMDGROUP_PORTA);

        //If there is NO second parameter, we must set the whole port!
        if (name[1] == '\0') {
            //The value part will be the new port value in uppercase hex
            if (hextob( (char *)value, &tmp ) ) {
                *p = tmp;   //Set port to new value
            }
        }
        //If there is a second parameter
        else {
            //Set port pin value, inverted (0=on, 1=off)
            if (name[1] == 'x') {
                //Third character of name will be bit position. For example ax3, bx6, fx0...
                //The value is INVERTED.
                tmp = 0x01 << (name[2] - '0');

                //If value = '1', clear pin
                if (value[0] == '1')
                    *p &= (~tmp);   //Clear port bit
                //If value = '0', set pin
                else if (value[0] == '0')
                    *p |= tmp;      //Set port bit
            }
            //Configure port pin as input or output
            else if (name[1] == 'c') {
                //Third character of name will be bit position. For example ac3, bc6, fc0...
                tmp = name[2] - '0';    //Get bit position in temp, a value from 0-7
                
                //Only continue if valid bit position between 0 and 7 was given
                if (tmp <= 7) {
                    tmp = 0x01 << tmp;

                    //Get pointer to requested TRIS register
                    p = (BYTE *)(&TRISA) + (name[0] - CMDGROUP_PORTA);

                    //Get current value of configuration byte that configures port directions
                    eeVal = appcfgGetc(APPCFG_TRISA  + (name[0] - CMDGROUP_PORTA));

                    //If value = '1', set port pin as input
                    if (value[0] == '1') {
                        *p |= tmp;      //Set TRIS bit
                        eeVal |= tmp;   //Set bit in application configuration byte
                    }
                    //If value = '0', set port pin as output
                    else if (value[0] == '0') {
                        tmp = ~tmp;         //Invert all bits
                        *p &= tmp;          //Clear TRIS bit
                        eeVal &= tmp;       //Clear bit in application configuration byte
                    }
                    
                    //If remember, update application configuration data too!
                    if (name[3] == 'r') {
                        appcfgPutc(APPCFG_TRISA  + (name[0] - CMDGROUP_PORTA), eeVal);
                    }
                }
            }
            //Assume this is the "Set Port Pin Command"
            else {
                //Second character of name will be bit position. For example a3, b6, f0...
                tmp = name[1] - '0';    //Get bit position in temp, a value from 0-7
                
                //Only continue if valid bit position between 0 and 7 was given
                if (tmp <= 7) {
                    tmp = 0x01 << tmp;

                    //Get current value of configuration byte that configures default port values at power up
                    eeVal = appcfgGetc(APPCFG_PORTA  + (name[0] - CMDGROUP_PORTA));

                    //If value = '0', clear pin
                    if (value[0] == '0') {
                        tmp = ~tmp;     //Invert bits
                        *p &= tmp;      //Clear port bit
                        eeVal &= tmp;   //Clear port bit of configuration byte
                    }
                    //If value = '1', set pin
                    else if (value[0] == '1') {
                        *p |= tmp;      //Set port bit
                        eeVal |= tmp;   //Set port bit of configuration byte
                    }

                    //If remember, update application configuration data too!
                    if (name[2] == 'r') {
                        appcfgPutc(APPCFG_PORTA  + (name[0] - CMDGROUP_PORTA), eeVal);
                    }
                }
            }
        }
    }
    /////////////////////////////////////////////////
    //We received a general command with value
    else if (name[0] == CMDGROUP_GENERAL)
    {
        #if (DEBUG_HTTPEXEC >= LOG_DEBUG)
        debugPutMsg(8);     //@mxd:8:Received General Command
        #endif

        /////////////////////////////////////////////////
        // The following commands can ONLY be executed by "Admin" or "Super User"
        if (user != USER_GUEST) {
            //The second character of the name part of the name-value pair will be the "Command Code".
            switch(name[1]) {
                case CMDCODE_GEN_NEW_PASSWORD:
                    //Update password with new one just received. Can have a maximum lenght of 8
                    strcpyram2ee(APPCFG_PASSWORD0, (char *)value, 8);
                    #if (DEBUG_HTTPEXEC >= LOG_INFO)
                    debugPutMsg(11);     //@mxd:11:Received new Password = %s
                    debugPutString(value);
                    #endif
                    break;
                case CMDCODE_GEN_NEW_USERNAME:
                    //Update username with new one just received. Can have a maximum lenght of 8
                    strcpyram2ee(APPCFG_USERNAME0, (char *)value, 8);
                    #if (DEBUG_HTTPEXEC >= LOG_INFO)
                    debugPutMsg(12);     //@mxd:12:Received new Username = %s
                    debugPutString(value);
                    #endif
                    break;
                case CMDCODE_GEN_NEW_NETBIOS_NAME:
                    //Update NetBIOS name with new one just received. Can have a maximum lenght of 16 chars
                    tmp = 1;
                    for (i=0; i<15; i++) {
                        if (tmp!=0) {
                            tmp = value[i];
                        }
                            
                        if (tmp==0) {
                            //Replace all characters following (and including) first NULL char with spaces
                            appcfgPutc(APPCFG_NETBIOS0+i, ' ');
                        }
                        else {
                            appcfgPutc(APPCFG_NETBIOS0+i, tmp);
                        }
                        FAST_USER_PROCESS();
                    }
                    appcfgPutc(APPCFG_NETBIOS15, 0);    //16th character must be 0
                    break;
            }
        }
    }
    /////////////////////////////////////////////////
    //We received a general command without value
    else if (name[0] == CMDGROUP_GENERAL_NOVAL)
    {
        #if (DEBUG_HTTPEXEC >= LOG_DEBUG)
        debugPutMsg(10);     //@mxd:10:Received General Command without value = %d
        #endif

        /////////////////////////////////////////////////
        // The following commands can ONLY be executed by "Admin" or "Super User"
        if (user != USER_GUEST) {
            //The first character of the value part of the name-value pair will be the "Command Code".
            switch(value[0]) {
                case CMDCODE_GENNOVAL_RESET:
                    #if (DEBUG_HTTPEXEC >= LOG_INFO)
                    debugPutMsg(13);     //@mxd:13:Received Reset command
                    #endif
                    NICReset(); //Reset the NIC
                    #if defined(BRD_SBC44EC) || defined(BRD_SBC45EC)
                        TRISB = 0xff;   //Set all ports used by NIC to inputs - this is NOT done by RESET() command
                        PORTB = 0;      //Clear all ports use by NIC - this is NOT done by reset command
                    #elif defined(BRD_SBC65EC) || defined(BRD_SBC68EC)
                        TRISB = TRISD = TRISE = TRISG = 0xff;   //Set all ports used by NIC to inputs - this is NOT done by RESET() command
                        PORTB = PORTD = PORTE = PORTG = 0;      //Clear all ports use by NIC - this is NOT done by reset command
                    #else
                        #error "Board type not defined!"
                    #endif
                    RESET();    //Perform a device reset - Bye bye world!!
                    break;
            }
        }
    }

    //Return wUpdate - it will indicate what has changed
    return wUpdate;
}



