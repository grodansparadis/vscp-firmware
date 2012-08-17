 /**
 * @brief           HTTP Callbacks
 * @file            httpexec.c
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    -
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_serint
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
 * 2004-10-19, David Hosken (DH):
 *    - Initial version
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 * 2006-09-04, David Hosken (DH):
 *    - Moved execNameValueCmd() funcion to new file, "cmd.c"
 *    - Added new tags to HTTPGetVar() function
 *********************************************************************/

#define THIS_IS_HTTPEXEC_MODULE

#include <string.h>

#include "projdefs.h"
#include "httpexec.h"
#include "cmd.h"
#include "debug.h"
#include "appcfg.h"

#include "net\stacktsk.h"
#include "net\http.h"
#include "net\delay.h"
#include "net\helpers.h"
#include "net\security.h"   //Security related code


/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD4, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD4, msgCode, msgStr)

//TODO Remove this later
#if defined(APP_USE_ADC8)
extern BYTE AdcValues[ADC_CHANNELS];
#elif defined(APP_USE_ADC10)
extern WORD AdcValues[ADC_CHANNELS];
#endif

ROM char STR_NOT_IMPLEMENTED[] = "Not Implemented";


/////////////////////////////////////////////////
// Variables
#define STRTMP_MAXSIZE 6
char strTmp[STRTMP_MAXSIZE];

// TEST TEST
//Future releases of this stack will NOT use this variable anymore!
extern HTTP_USER httpUser;


/////////////////////////////////////////////////
// Function prototypes
static WORD getHTTPByteVar(BYTE ref, BYTE* val, BYTE b);
#if defined(APP_USE_ADC10)  //Currently only used if APP_USE_ADC10 is enabled
static WORD getHTTPWordVar(BYTE ref, BYTE* val, WORD w);
#endif
static WORD getHTTPROMStringVar(BYTE ref, BYTE* val, ROM char* str);
static WORD getHTTPEepromStringVar(BYTE ref, BYTE* val, WORD adr);
//static WORD getHTTPStringVar(BYTE ref, BYTE* val, BYTE* str);
//static BYTE strStartsWith(ROM char* strToLookFor, BYTE* strSrc);


/////////////////////////////////////////////////
// ROM Strings
ROM char HTMLSTR_CHECKED[] = "checked";
ROM char HTMLSTR_DISABLED[] = "disabled";
ROM char HTMLSTR_CMNT_START[] = "<!--";
ROM char HTMLSTR_CMNT_END[] = "-->";
ROM char HTMLSTR_ON[] = "on";
ROM char HTMLSTR_OFF[] = "off";


#if defined(STACK_USE_HTTP_SERVER)
/**
 * Initializes global variables.
 */
void htpexecInit(void)
{

}

/**
 * Returns the current user logged in.
 *
 * @return The current user logged in. Is a HTTP_USER_XX constant
 */
BYTE getCurrentUser(HTTP_INFO* httpInfo) {
    //Get the current user logged in for this HTTP connection
    if (httpInfo->flags.bits.bUserLoggedIn) {
        if (httpInfo->flags.bits.bUserSuper) {
            return USER_SUPER;
        }
        else {
            return USER_ADMIN;
        }
    }
    
    return USER_GUEST;
}


/**
 * This function is a "callback" from HTTPServer task. Whenever a remote node performs interactive
 * GET task on page that was served, HTTPServer calls this functions. Use HTTPGetParam()
 * to get all name-value parameters.
 *
 * @param httpInfo  Socket that is currently receiving this HTTP command
 * @param rqstRes   Name of the Requested resource - GET command's action. All characters are
 *                  in uppercase!
 */
void HTTPExecGetCmd(HTTP_INFO* httpInfo, BYTE* rqstRes) {
    BYTE param[20];
    WORD wUpdate;   //Contains CMD_UD_XXX flags indicating what has to be updated

    //Used as input AND output parameter for HTTPGetParams.
    // - Input parameter indicates size of param buffer
    // - On return of HTTPGerParam() valueIdx will contain index of value string in param
    BYTE valueIdx;
    BYTE moreParams;
    BYTE paramCount;    //The number of parameters (name-value pairs) we have received
    BYTE user;
    BYTE tmp;
    BYTE executed;      //Indicates if the command has been executed already or not

    #if (DEBUG_HTTPEXEC >= LOG_DEBUG)
    debugPutMsg(1);     //@mxd:1:HTTPExecGetCmd() called for file %s
    debugPutString(rqstRes);
    #endif

    wUpdate = 0;    //Clear all update flags
    paramCount = 0;
    
    //Get the current user logged in for this HTTP connection
    user = getCurrentUser(httpInfo);

    //Get next name-value parameter
    do {
        valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

        //Get name-value parameters. Returns true if there are more name-value parameters to follow
        //- Pointer to Name parameter = &param[0]
        //- Pointer to Value parameter = &param[valueIdx]
        moreParams = HTTPGetParam(httpInfo->socket, param, &valueIdx);
        paramCount++;

        executed = 0;   //Current name-value command has not been executed yet

        /////////////////////////////////////////////////
        //We received a general command with value
        if (param[0] == CMDGROUP_GENERAL)
        {
            #if (DEBUG_HTTPEXEC >= LOG_DEBUG)
            debugPutMsg(8);     //@mxd:8:Received General Command
            #endif

            //The second character of the name part of the name-value pair will be the "Command Code".
            switch(param[1]) {
                case CMDCODE_GEN_PASSWORD:
                    executed = 1;   //Indicates that the command has been executed
                    
                    //Compare value with password
                    if (strcmpee2ram((char *)&param[valueIdx], APPCFG_PASSWORD0) == 0) {
                        if ((paramCount == 2) && (tmp & 0x01)) {
                            #if (DEBUG_HTTPEXEC >= LOG_INFO)
                            debugPutMsg(9);     //@mxd:9:Received valid Username and Password
                            #endif

                            // TEST TEST
                            //This code will be remove in future release. Password implementation will be
                            //done via HTTP Authorization or Cookies! This logic will move to http.c!
                            httpUser.ipAdr.Val = TCPGetSocketInfo(httpInfo->socket)->remote.IPAddr.Val;
                            httpUser.user = USER_ADMIN;

                            //Update HTTP_INFO structure's user information
                            httpInfo->flags.bits.bUserLoggedIn = TRUE;
                            httpInfo->flags.bits.bUserSuper = FALSE;
                        }
                    }
                    break;
                case CMDCODE_GEN_USERNAME:
                    executed = 1;   //Indicates that the command has been executed
                    /*
                    #if (DEBUG_GEN >= LOG_INFO)
                    debugPutGenMsg(6);     //@mxd:6:%s%s
                    debugPutRomString((ROM char*)"Received username: ");
                    debugPutString(&param[valueIdx]);
                    #endif
                    */
                    //Compare value with username
                    if (strcmpee2ram((char *)&param[valueIdx], APPCFG_USERNAME0) == 0) {
                        tmp |= 0x01;    //Use bit 1 to indicate USERNAME was received
                    }
                    break;
            }
        }
        /////////////////////////////////////////////////
        //We received a general command without value
        else if (param[0] == CMDGROUP_GENERAL_NOVAL)
        {
            #if (DEBUG_HTTPEXEC >= LOG_DEBUG)
            debugPutMsg(10);     //@mxd:10:Received General Command without value = %d
            #endif

            /////////////////////////////////////////////////
            // The following commands can ONLY be executed by "Admin" or "Super User"
            if (user != USER_GUEST) {
                //The first character of the value part of the name-value pair will be the "Command Code".
                switch(param[valueIdx]) {
                    case CMDCODE_GENNOVAL_LOGOUT:
                        executed = 1;   //Indicates that the command has been executed
                        
                        #if (DEBUG_HTTPEXEC >= LOG_INFO)
                        debugPutMsg(14);     //@mxd:14:Received Log Off command
                        #endif
                        //Log Out
                        httpUser.user = USER_GUEST;

                        //Update HTTP_INFO structure's user information
                        httpInfo->flags.bits.bUserLoggedIn = FALSE;
                        break;
                }
            }
        }
        
        //If this command has not been executed yet, pass it on to the execNaveValueCmd function
        if ( !executed) {
            //Execute the given name-value command
            wUpdate |= execNameValueCmd(param, &param[valueIdx], user);
        }
    } while (moreParams);


    //Network settings have changed
    if (wUpdate & CMD_UD_NETWORK) {
        //Reconfigure USART with new value
        //appcfgNetwork();
    }
    else if (wUpdate & CMD_UD_USART) {
        //Reconfigure USART with new value
        appcfgUSART();
    }
    else if (wUpdate & CMD_UD_CPU_IO) {
        //Reconfigure IO ports
        appcfgCpuIO();
    }
    else if (wUpdate & CMD_UD_ADC) {
        //Reconfigure IO ports
        appcfgADC();
    }
}
#endif


#if defined(STACK_USE_HTTP_SERVER)


/**
 * This is a callback function from HTTPServer() to main application. This function
 * must be implemented by the user somewhere in the code. Whenever a variable substitution
 * is required on any html pages, HTTPServer calls this function with a 8-bit variable
 * identifier, variable reference, which indicates whether this is a first call or not.
 * Application should return one character at a time as a variable value. If there are no more
 * characters, or requeseted variable not found, this function must set var to NULL.
 * Since this function only allows one character to be returned at a time as part of the variable
 * value, HTTPServer() calls this function multiple times until main application indicates that
 * there is no more value left for this variable. On begining, HTTPGetVar() is called with
 * ref = HTTP_START_OF_VAR to indicate that this is a first call.  Application should use this reference to
 * start the variable value extraction and return updated reference. If there is no more values
 * left for this variable the application should send HTTP_END_OF_VAR AND set var to NULL. If there
 * are any bytes to send, application should return other than HTTP_START_OF_VAR and HTTP_END_OF_VAR reference.
 *
 * @preCondition    HTTPInit() must already be called.
 *
 * @param httpInfo  HTTP_INFO structure for current HTTP connection.
 * @param val       Buffer for value storage.
 *
 * @return          Variable reference as required by application.
 */
WORD HTTPGetVar(HTTP_INFO* httpInfo, BYTE* val)
{
    BYTE flags;
    BYTE varValue;
    BYTE varGroup;
    BYTE* p;
    BYTE mask;
    BYTE tmp;
    WORD ref;
    BYTE user;

    #define FLAGS_OUTPUT_PIN    0x01
    #define FLAGS_ADC_PIN       0x02
    #define FLAGS_EXECUTED      0x04

    varValue = httpInfo->var.get.tagVal;        //Variable Value of requested variable
    varGroup = httpInfo->var.get.tagGroup;      //Variable Group of requested variable
    ref      = httpInfo->var.get.varRef;        //Current callback reference with respect to 'var' variable.

    //In case requested var not found, set it to NULL character and return HTTP_END_OF_VAR
    *val = '\0';    
    
    //Get the current user logged in for this HTTP connection
    user = getCurrentUser(httpInfo);
    

    /////////////////////////////////////////////////
    //General purpose and Port pin variables - they are:
    // - 'a' Used for port a value. a0x will display 1 or 0, a1x will display on or off
    // - 'b' Used for port b value.
    // - 'c' Used for port c value.
    // - 'd' Reserved for port d value.
    // - 'e' Reserved for port e value.
    // - 'f' Used for port f value.
    // - 'g' Used for port g value.
    // - 'h' Reserved for port h value.
    // - 'i' Reserved for future use
    // - 'j' Reserved for port h value.
    if ((varGroup >= VARGROUP_PORTA) && (varGroup <= VARGROUP_PORTH))
    {
        //The value of the requested variable will result in the following being returned:
        // 00 to 07 - 1 or 0 returned depending on PORT state
        // 10 to 17 - "on" or "off" returned depending on PORT state
        // 20 to 27 - "<!--" returned if port is configured as input
        // 28 to 2f - "-->" returned if port is configured as input
        // 30 to 37 - "<!--" returned if port is configured as output
        // 38 to 3f - "-->" returned if port is configured as output
        // 40 to 47 - "checked" returned if port is configured as output
        // 48 to 4f - "checked" returned if port is configured as input
        // 50 to 57 - 1 returned if port is input, else 0

        flags = 0;

        //Get register offset. PORTA = a, PORTB = b, PORTC = c......
        tmp = (varGroup - 'a');

        //Get bit mask
        mask = 0x01 << (varValue & 07);

        //Get pointer to TRIS register. Seeing that all TRIS registers are always next to each
        //other in the PICs RAM, we can use TRISA as base register, and add offset to it.
        p = (BYTE *)(&TRISA) + tmp;

        //Requested pin is an output - use LATCH register
        if ( (*p & mask) == 0 )
        {
            p = (BYTE *)(&LATA) + tmp;  //Get pointer to LATCH register
            flags |= FLAGS_OUTPUT_PIN;
        }
        //Requested pin in an input - use PORT register
        else
        {
            p = (BYTE *)(&PORTA) + tmp;
        }

        tmp = varValue & 0xf0;

        //Return "0" or "1"
        if (tmp == 0x00)
        {
            //Set val to '0' or '1' depending on state of pin
            *val = ((*p & mask) == 0) ? '0' : '1';
            return HTTP_END_OF_VAR;
        }
        //Return "on" or "off"
        else if (tmp == 0x10)
        {
            return getHTTPROMStringVar(ref, val,
                    ((*p & mask) == 0) ? HTMLSTR_OFF : HTMLSTR_ON);
        }
        //Return "<!--" or "-->" depending on if port is configured as input or output
        else if ( ((tmp == 0x20) && ((flags & FLAGS_OUTPUT_PIN) == 0)) ||
                  ((tmp == 0x30) && ((flags & FLAGS_OUTPUT_PIN) != 0)) )
        {
            return getHTTPROMStringVar(ref, val,
                    ((varValue & 0x08) == 0) ? HTMLSTR_CMNT_START : HTMLSTR_CMNT_END);
        }
        //Return "checked" if port configured as output or input
        else if (tmp == 0x40)
        {
            if (varValue & 0x08) {
                //Return "checked" if port configured as input
                if ( (flags & FLAGS_OUTPUT_PIN) == 0 ) {
                    return getHTTPROMStringVar(ref, val, HTMLSTR_CHECKED);
                }
            }
            else {
                //Return "checked" if port configured as output
                if ( (flags & FLAGS_OUTPUT_PIN) != 0 ) {
                    return getHTTPROMStringVar(ref, val, HTMLSTR_CHECKED);
                }
            }
        }
        //Return "1" if port configured as input, else 0
        else if (tmp == 0x50)
        {
            //Set val to '0' or '1' depending on pin direction
            *val = (flags & FLAGS_OUTPUT_PIN) ? '0' : '1';
            return HTTP_END_OF_VAR;
        }
    }
    /////////////////////////////////////////////////
    //'k' Configuration variables
    else if (varGroup == VARGROUP_CONFIG)
    {
        //Guest is NOT allowed this information!
        if (user != USER_GUEST) {
            //Return decimal value of requested configuration byte
            return getHTTPByteVar(ref, val, appcfgGetc(varValue));
        }
    }
    /////////////////////////////////////////////////
    //'l' General variables
    else if (varGroup == VARGROUP_GENERAL)
    {
        flags &= ~FLAGS_EXECUTED; //Clear EXECUTED flag
        
        //Only Admin and Super Users can access this information!
        /*
        if (user != USER_GUEST) {
 
            flags |= FLAGS_EXECUTED;  //Set EXECUTED flag

            //Version of current Modtronix TCP/IP stack. Format is "Vm.nn", for example V2.04.
            // - m can be 1 or 2 digets long, nn is always 2 digets.
            if (varValue == VARVAL_GEN_STACK_VER_STR) {
                return getHTTPROMStringVar(ref, val, STACK_VER_STR);
            }
            else {
                flags &= ~FLAGS_EXECUTED; //Clear EXECUTED flag
            }
        }
        */

        //Everyone can get this information - only executed if not executed yet!
        if ( !(flags & FLAGS_EXECUTED)) {
            //Version of current Modtronix TCP/IP stack. Format is "Vm.nn", for example V2.04.
            // - m can be 1 or 2 digets long, nn is always 2 digets.
            if (varValue == VARVAL_GEN_STACK_VER_STR) {
                //Return string representing stack version
                return getHTTPROMStringVar(ref, val, STACK_VER_STR);
            }
            //Version of current Application. Format is "Vm.nn", for example V2.04.
            // - m can be 1 or 2 digets long, nn is always 2 digets.
            else if (varValue == VARVAL_GEN_APP_VER_STR) {
                //Return string representing stack version
                return getHTTPROMStringVar(ref, val, APP_VER_STR);
            }
            //Display 'Yes' if board has a bootloader, else 'No'.
            else if (varValue == VARVAL_GEN_HAS_BL) {
                #ifdef HAS_BOOTLOADER
                return getHTTPROMStringVar(ref, val, (ROM char*)"Yes");
                #else
                return getHTTPROMStringVar(ref, val, (ROM char*)"No");
                #endif
            }
            //Usename of current user logged in
            else if (varValue == VARVAL_GEN_USERNAME) {
                //Admin user currently logged in
                if (user == USER_ADMIN) {
                    return getHTTPEepromStringVar(ref, val, APPCFG_USERNAME0);
                }
                //Else Super user currently logged in
                else if (user == USER_SUPER) {
                    return getHTTPROMStringVar(ref, val, (ROM char*)"Super User");
                }

                //Guest currently logged in
                return getHTTPROMStringVar(ref, val, (ROM char*)"Guest");
            }
            //Returns '0' if we are currently not logged in, or '1' if we are
            else if (varValue == VARVAL_GEN_LOGGED_IN) {
                tmp = (user == USER_GUEST) ? '0' : '1';
                return getHTTPByteVar(ref, val, tmp);
            }
            //Returns NetBIOS name
            else if (varValue == VARVAL_GEN_NETBIOS_NAME) {
                return getHTTPEepromStringVar(ref, val, APPCFG_NETBIOS0);
            }
            else if ( (varValue >= VARVAL_GEN_IP0) && (varValue <= VARVAL_GEN_GATEWAY3) ) {
                //Return decimal value of requested AppConfig byte
                return getHTTPByteVar(ref, val, ((BYTE*)&AppConfig)[varValue-VARVAL_GEN_IP0] );
            }
        }
    }
    /////////////////////////////////////////////////
    //ADC variables
    else if (varGroup == VARGROUP_ANALOG)
    {
        //The value of the requested variable will result in the following being returned:
        // 00 to 0A - 2 or 3 digit hex value in capital letters
        // 10 to 1A - decimal value 0 - 255 or 0 - 1023
        // 20 to 2A - decimale value, 2 decimal places for 5V reference
        // 30 to 3A - "<!--" returned if the channels is configured for ADC
        // 40 to 4A - "-->" returned if the channels is configured for ADC
        // 50 to 5A - "<!--" returned if the channels is NOT configured for ADC
        // 60 to 6A - "-->" returned if the channels is NOT configured for ADC

        flags = 0;

        //Has channel being configured as ADC
        if ((varValue & 0x0f) < (~ADCON1 & 0x0f))
        {
            flags |= FLAGS_ADC_PIN;
        }

        tmp = varValue & 0xf0;

        //Return hex value
        if (tmp == 0x00)
        {
            return getHTTPROMStringVar(ref, val, STR_NOT_IMPLEMENTED);
        }
        //Return decimal value 0 - 1023
        else if (tmp == 0x10)
        {
            //mainFlags.bits.bFreezeADCBuf = 1;

            #if defined(APP_USE_ADC8)
            return getHTTPByteVar(ref, val, AdcValues[varValue & 0x0f]);
            #elif defined(APP_USE_ADC10)
            return getHTTPWordVar(ref, val, AdcValues[varValue & 0x0f]);
            #endif
        }
        //Return decimal value (2 decimal point precicion) for if 5V reference was used
        else if (tmp == 0x20)
        {
            #if defined(APP_USE_ADC8)

            tmp = AdcValues[varValue & 0x0f];

            if ( ref == (BYTE)HTTP_START_OF_VAR )
            {
                //Convert tmp value to voltage (2 decimal points) value for 5V reference
                strTmp[0] = (BYTE)(tmp / 51) + '0';
                strTmp[1] = '.';
                tmp = (tmp % 51);   //Get rest
                strTmp[2] = (BYTE)(( ((WORD)tmp) * (WORD)10) / (WORD)51) + '0';
                tmp = (tmp / 10);
                strTmp[3] = (BYTE)( ( ((WORD)tmp) * (WORD)100) / (WORD)51) + '0';
                strTmp[4] = '\0';  //Terminate with 0

                ref = (BYTE)0;
            }
            *val = strTmp[(BYTE)ref];
            if ( strTmp[(BYTE)ref] == '\0' )
                return HTTP_END_OF_VAR;
            (BYTE)ref++;
            return ref;
            #elif defined(APP_USE_ADC10)
                // TODO - Implement this!
                return HTTP_END_OF_VAR;
            #endif
        }
        //Return "<!--"
        else if ( ((tmp == 0x30) && ((flags & FLAGS_ADC_PIN) != 0)) ||
                  ((tmp == 0x50) && ((flags & FLAGS_ADC_PIN) == 0)) )
        {
            return getHTTPROMStringVar(ref, val, HTMLSTR_CMNT_START);
        }
        //Return "<!--"
        else if ( ((tmp == 0x40) && ((flags & FLAGS_ADC_PIN) != 0)) ||
                  ((tmp == 0x60) && ((flags & FLAGS_ADC_PIN) == 0)) )
        {
            return getHTTPROMStringVar(ref, val, HTMLSTR_CMNT_END);
        }
    }
    return HTTP_END_OF_VAR;
}
#endif

/**
 * Returns a string representation of a given ROM string to repeditive calls by HTTPGetVar()
 *
 * @param ref   Current callback reference used by HTTPGetVar
 * @param val   Buffer for value storage.
 * @param str   Pointer to string to display
 *
 * @return      HTTPGetVar return value. Is the offset in the given string.
 */
static WORD getHTTPROMStringVar(BYTE ref, BYTE* val, ROM char* str)
{
    *val = str[(BYTE)ref];
    if (*val == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}

/**
 * Returns a string representation of a given Internal EEPROM string to repeditive calls by HTTPGetVar()
 *
 * @param ref   Current callback reference used by HTTPGetVar
 * @param val   Buffer for value storage.
 * @param adr   The start address of the string in the internal EEPROM
 *
 * @return      HTTPGetVar return value. Is the offset in the given string.
 */
static WORD getHTTPEepromStringVar(BYTE ref, BYTE* val, WORD adr)
{
    WORD_VAL wAdr;
    wAdr.Val = ref + adr;
    #if (EEPROM_SIZE > 256)
    EEADRH = wAdr.v[1];
    #endif
    EEADR = wAdr.v[0];
    
    //Configure for EEPROM reading, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x3F;

    EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.

    //Read value from EEPROM
    *val = EEDATA;
    if (EEDATA == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}


/**
 * Returns a string representation of a given ROM string to repeditive calls by HTTPGetVar()
 *
 * @param ref   Current callback reference used by HTTPGetVar
 * @param val   Buffer for value storage.
 * @param b     Byte value to display
 *
 * @return      HTTPGetVar return value.
 */
 /*
static WORD getHTTPStringVar(BYTE ref, BYTE* val, BYTE* str)
{
    *val = str[(BYTE)ref];
    if (*val == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}
*/

/**
 * Handle HTTPGetVar for a string representation of a byte.
 *
 * @param ref   Current callback reference used by HTTPGetVar
 * @param val   Buffer for value storage.
 * @param b     Byte value to display
 *
 * @return      HTTPGetVar return value.
 */
static WORD getHTTPByteVar(BYTE ref, BYTE* val, BYTE b)
{
    if ( ref == (BYTE)HTTP_START_OF_VAR )
    {
        itoa(b, strTmp);
        strTmp[STRTMP_MAXSIZE - 1] = '\0';  //Terminate with 0
        ref = (BYTE)0;
    }
    *val = strTmp[(BYTE)ref];
    if ( strTmp[(BYTE)ref] == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}

/**
 * Handle HTTPGetVar for a string representation of a word.
 *
 * @param ref   Current callback reference used by HTTPGetVar
 * @param val   Buffer for value storage.
 * @param b     Byte value to display
 *
 * @return      HTTPGetVar return value.
 * 
 */
#if defined(APP_USE_ADC10)  //Currently only used if APP_USE_ADC10 is enabled
static WORD getHTTPWordVar(BYTE ref, BYTE* val, WORD w)
{
    if ( ref == (BYTE)HTTP_START_OF_VAR )
    {
        itoa(w, strTmp);
        strTmp[STRTMP_MAXSIZE - 1] = '\0';  //Terminate with 0
        ref = (BYTE)0;
    }
    *val = strTmp[(BYTE)ref];
    if ( strTmp[(BYTE)ref] == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}
#endif


/**
 * Checks if given string begins with string to look for.
 *
 * @return      True if found, else 0
 */
/*
static BYTE strStartsWith(ROM char* strToLookFor, BYTE* strSrc)
{
    while (*strToLookFor != '\0') {
        if (*strToLookFor != *strSrc)
            return 0;
        strToLookFor++;
        strSrc++;
    }

    return 1;
}
*/

