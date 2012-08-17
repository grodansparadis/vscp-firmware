 /**
 * @brief           HTTP Callbacks
 * @file            httpexec.c
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
#define debugPutMsg(msgCode) debugPut2Bytes(0xD1, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD1, msgCode, msgStr)

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

/////////////////////////////////////////////////
// Function prototypes
//static WORD getHTTPByteVar(BYTE ref, BYTE* val, BYTE b);
#define getHTTPByteVar(ref, val, b) getHTTPWordVar(ref, val, b)
static WORD getHTTPWordVar(BYTE ref, BYTE* val, WORD w);

static WORD getHTTPByteHexVar(BYTE ref, BYTE* val, BYTE b);

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
    BYTE param[64];
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
            #if (DEBUG_CMD >= LOG_DEBUG)
            debugPutMsg(8);     //@mxd:8:Received General Command
            #endif

            //The second character of the name part of the name-value pair will be the "Command Code".
            switch(param[1]) {
                case CMDCODE_GEN_PASSWORD:
                    executed = 1;   //Indicates that the command has been executed
            
                    // This command is NOT used any more, seeing that we now use HTTP Authentication
                    break;
                case CMDCODE_GEN_USERNAME:
                    executed = 1;   //Indicates that the command has been executed

                    // This command is NOT used any more, seeing that we now use HTTP Authentication
                    break;
            }
        }
        /////////////////////////////////////////////////
        //We received a general command without value
        else if (param[0] == CMDGROUP_GENERAL_NOVAL)
        {
            #if (DEBUG_CMD >= LOG_DEBUG)
            debugPutMsg(10);     //@mxd:10:Received General Command without value = %d
            #endif

            /////////////////////////////////////////////////
            // The following commands can ONLY be executed by "Admin" or "Super User"
            if (user != USER_GUEST) {
                //The first character of the value part of the name-value pair will be the "Command Code".
                switch(param[valueIdx]) {

                    //There is no logout command anymore with HTTP Authentication!
                    case CMDCODE_GENNOVAL_LOGOUT:
                        executed = 1;   //Indicates that the command has been executed
                        
                        #if (DEBUG_CMD >= LOG_INFO)
                        debugPutMsg(14);     //@mxd:14:Received Log Off command
                        #endif

                        //There is no logout command anymore with HTTP Authentication!
                        //Update HTTP_INFO structure's user information
                        //httpInfo->flags.bits.bUserLoggedIn = FALSE;
                        
                        break;
                }
            }

            //The first character of the value part of the name-value pair will be the "Command Code".
            switch(param[valueIdx]) {
                //Causes requested to log in, if not already Authenticated
                case CMDCODE_GENNOVAL_LOGIN:
                    executed = 1;   //Indicates that the command has been executed
                    
                    httpInfo->flags.bits.bLoginReq = TRUE;
                    break;
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
    else if (wUpdate & CMD_UD_PWM) {
        //Reconfigure PWM
        appcfgPWM();
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
 * start the variable value extraction and return updated reference. If there are no more values
 * left for this variable the application should return HTTP_END_OF_VAR. If there
 * are any bytes to send, application should return other than HTTP_START_OF_VAR and HTTP_END_OF_VAR reference.
 *
 * @preCondition    HTTPInit() must already be called.
 *
 * @param httpInfo  HTTP_INFO structure for current HTTP connection.
 * @param val       Buffer for value storage.
 *
 * @return          Variable reference as required by application, or HTTP_END_OF_VAR if there are
 *                  no more values to be sent.
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
    #define FLAGS_PWM_ENABLED   0x08

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
        //Requested pin is an input - use PORT register
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
        else {
            //Else return 0 - actually the string "00" is returned
            return getHTTPByteVar(ref, val, 0);
        }
    }
    /////////////////////////////////////////////////
    //'l' General variables
    else if (varGroup == VARGROUP_GENERAL)
    {
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
            //Return Username
            if (user != USER_GUEST) {
                return getHTTPEepromStringVar(ref, val, APPCFG_USERNAME0);
            }
            //Guest is NOT allowed this information - return "Guest"
            else {
                return getHTTPROMStringVar(ref, val, (ROM char*)"Guest");
            }
        }
        //Returns '1' if Authentication has been provided, else '0'
        else if (varValue == VARVAL_GEN_LOGGED_IN) {
            *val = (user == USER_GUEST) ? '0' : '1';
            return HTTP_END_OF_VAR;
        }
        //Returns '1' if in Demo Mode, else '0'
        else if (varValue == VARVAL_GEN_DEMO_MODE) {
            #if defined(DEMO_MODE)
            *val = '1';
            #else
            *val = '0';
            #endif
            return HTTP_END_OF_VAR;
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
    /////////////////////////////////////////////////
    //PWM variables
    else if (varGroup == VARGROUP_PWM)
    {
        BYTE ccpxcon;       //CCPxCON register
        BYTE varVal0to3;    //Variable Value bit 0-3
        flags = 0;

        varVal0to3 = (varValue & 0x0f); //Get PWM channel

        ccpxcon = 0;
        
        //PWM Channel 1
        if (varVal0to3 == 1) {
            ccpxcon = CCP1CON;
        }
        else if (varVal0to3 == 2) {
            ccpxcon = CCP2CON;
        }
        #if defined(BRD_SBC65EC)
        else if (varVal0to3 == 3) {
            ccpxcon = CCP3CON;
        }
        else if (varVal0to3 == 4) {
            ccpxcon = CCP4CON;
        }
        #endif
        
        //Has channel being configured as PWM
        if ( (ccpxcon & 0x0f) != 0)
        {
            flags |= FLAGS_PWM_ENABLED;
        }

        tmp = varValue & 0xf0;  //Get upper nibble of variable value

        //This is a request for the mode, frequency or selected PWM channels
        if (tmp == 0xf0) {
            //Request frequency value - return a constant from 0-3 indicating current frequency
            if (varVal0to3 == 0) {
                *val = '0' + pwmGetFreq();
                return HTTP_END_OF_VAR;
            }
            //Request mode - return '8' for 8-bit mode, or 'a' for 10-bit mode
            else if (varVal0to3 == 4) {
                if (pwmIs10BitMode() == TRUE) {
                    *val = 'a';
                }
                else {
                    *val = '8';
                }
                return HTTP_END_OF_VAR;
            }
            //Request enabled PWM channels
            else if (varVal0to3 == 8) {
                tmp = 0;
                //If PWM 1 is enables, set bit 0
                if (CCP1CON & 0x0f)
                    tmp |= 0x01;
                //If PWM 2 is enables, set bit 1
                if (CCP2CON & 0x0f)
                    tmp |= 0x02;
                #if defined(BRD_SBC65EC)
                    //If PWM 3 is enables, set bit 2
                    if (CCP3CON & 0x0f)
                        tmp |= 0x04;
                    //If PWM 4 is enables, set bit 3
                    if (CCP4CON & 0x0f)
                        tmp |= 0x08;
                #endif

                //Return uppercase, hex value
                return getHTTPByteHexVar(ref, val, tmp);
            }
        }
        //A PWM Channel's value has been requested. The value of the requested variable will result
        //in the following being returned:
        // 30 to 38 - "<!--" returned if PWM channel is enabled
        // 40 to 48 - "-->" returned if PWM channel is enabled
        // 50 to 58 - "<!--" returned if PWM channel is disabled
        // 60 to 68 - "-->" returned if PWM channel is disabled

        // 00 to 08 variable value = PWM Channel's value as a 2 or 3 digit hex value in capital letters
        else if (tmp == 0x00)
        {
            return getHTTPROMStringVar(ref, val, STR_NOT_IMPLEMENTED);
        }
        // 00 to 08 variable value = PWM Channel's value as a decimal value 0 - 255 or 0 - 1023
        else if (tmp == 0x10)
        {
            return getHTTPWordVar(ref, val, pwmGetValue(varVal0to3));
        }
        // 30 to 38 variable value = "<!--" returned if the PWM channel is enabled
        // 50 to 58 variable value = "<!--" returned if the PWM channel is disabled
        else if ( ((tmp == 0x30) && ((flags & FLAGS_PWM_ENABLED) != 0)) ||
                  ((tmp == 0x50) && ((flags & FLAGS_PWM_ENABLED) == 0)) )
        {
            return getHTTPROMStringVar(ref, val, HTMLSTR_CMNT_START);
        }
        // 40 to 48 variable value = "-->" returned if the PWM channel is enable
        // 60 to 68 variable value = "-->" returned if the PWM channel is disabled
        else if ( ((tmp == 0x40) && ((flags & FLAGS_PWM_ENABLED) != 0)) ||
                  ((tmp == 0x60) && ((flags & FLAGS_PWM_ENABLED) == 0)) )
        {
            return getHTTPROMStringVar(ref, val, HTMLSTR_CMNT_END);
        }
    }
    /////////////////////////////////////////////////
    //'s' Secure Tags
    else if (varGroup == VARGROUP_SECURE)
    {
        //Return 0 is not Authenticated
        if (user == USER_GUEST) {
            *val = '0';
            return HTTP_END_OF_VAR;
        }

        //Username of current user logged in
        if (varValue == VARVAL_SEC_USERNAME) {
            return getHTTPEepromStringVar(ref, val, APPCFG_USERNAME0);
        }
        //Password of current user logged in
        else if (varValue == VARVAL_SEC_PASSWORD) {
            return getHTTPEepromStringVar(ref, val, APPCFG_PASSWORD0);
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
 * Handle HTTPGetVar for a two character, uppercase hex string representation of a byte.
 *
 * @param ref   Current callback reference used by HTTPGetVar
 * @param val   Buffer for value storage.
 * @param b     Byte value to display
 *
 * @return      HTTPGetVar return value.
 */
static WORD getHTTPByteHexVar(BYTE ref, BYTE* val, BYTE b)
{
    //Both bytes have already been returned
    if (ref >= 2) {
        return HTTP_END_OF_VAR;
    }

    //Upper nibble for first byte (left most character), lower nibble for second (right most character)
    if (ref == 0)
    {
        b >>= 4;
    }

    *val = ((b&0x0f)>9) ? (b+('A'-10)) : (b+'0');

    return ref+1;
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

