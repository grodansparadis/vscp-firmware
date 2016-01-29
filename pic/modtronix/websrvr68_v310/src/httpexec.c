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


/////////////////////////////////////////////////
// Function prototypes


#if defined(STACK_USE_HTTP_SERVER)
/**
 * Initializes global variables.
 */
void htpexecInit(void)
{

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
    user = HTTPGetCurrentUser(httpInfo);

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
            debugPutMsg(10);     //@mxd:10:Received General Command without value
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
        appcfgUSART2();
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
    else if (wUpdate & CMD_UD_XBOARD) {
        //Reconfigure Expansion Board
        appcfgXboard();
    }
    else if (wUpdate & CMD_UD_BUSBUF) {
        //Reconfigure Bus Buffers
        busInfoInit();
    }
    else if (wUpdate & CMD_UD_BUSNET) {
        //Reconfigure UDP Buses
        busNetInit();
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
    GETTAG_INFO getTagInfo;

    getTagInfo.tagVal   = httpInfo->var.get.tagVal;       //Value of requested tag
    getTagInfo.tagGroup = httpInfo->var.get.tagGroup;     //Group of requested tag
    getTagInfo.ref      = httpInfo->var.get.varRef;       //Current callback reference with respect to 'var' variable.
    getTagInfo.val      = val;

    //In case requested var not found, set it to NULL character and return HTTP_END_OF_VAR
    *val = '\0';
    
    //Get the current user logged in for this HTTP connection
    getTagInfo.user = HTTPGetCurrentUser(httpInfo);
    
    cmdGetTag(&getTagInfo);
    return getTagInfo.ref;
}
#endif


