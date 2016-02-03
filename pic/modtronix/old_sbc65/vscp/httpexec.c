/*********************************************************************
 * httpexec.c    -  HTTP Callbacks
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * HTTP Callbacks.
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
#define THIS_IS_HTTPEXEC_MODULE

#include <string.h>
#include "projdefs.h"
#include "httpexec.h"
#include "http.h"
#include "appcfg.h"
#include "delay.h"
#include <stdio.h>

/*
 * Define DEBUG_HTTPEXEC in projdefs.h (or command line) if debug output is required.
 */
#if defined(DEBUG_HTTPEXEC)
#include "debug.h"
#else
#define debugPutByte(a)
#define debugPutRomString(a)
#define debugPutString(a)
#endif

/*
 * HTTP User types that can be logged in.
 * - Guest = user that has not logged in
 * - Admin = user that has logged in
 * - Super = super user, has all rights
 */
#define HTTP_USER_GUEST         (0)
#define HTTP_USER_ADMIN         (1)
#define HTTP_USER_SUPER         (2)

ROM char* HTTP_USERS[] =
{
    "Guest",
    "Admin",
    "Super"
};

ROM char STR_NOT_IMPLEMENTED[] = "Not Implemented";

/*
 * Stores information on current logged in users
 */
typedef struct _HTTP_USER {
    IP_ADDR ipAdr;  //IP Address of logged in user
    BYTE user;      //HTTP User type, is a HTTP_USER_XXX variable
} HTTP_USER;


/////////////////////////////////////////////////
//Variables
#define STRTMP_MAXSIZE 10
char strTmp[ STRTMP_MAXSIZE ];

static HTTP_USER httpUser;

/////////////////////////////////////////////////
//Function prototypes
static WORD getHTTPByteVar(BYTE ref, BYTE* val, BYTE b);
static WORD getHTTPWordVar(BYTE ref, BYTE* val, WORD w);
static WORD getHTTPROMStringVar(BYTE ref, BYTE* val, ROM char* str);
static WORD getHTTPStringVar(BYTE ref, BYTE* val, BYTE* str);
static BYTE strStartsWith(ROM char* strToLookFor, BYTE* strSrc);

ROM char HTMLSTR_SELECTED[] = "selected";
ROM char HTMLSTR_CHECKED[] = "checked";
ROM char HTMLSTR_DISABLED[] = "disabled";
ROM char HTMLSTR_CMNT_START[] = "<!--";
ROM char HTMLSTR_CMNT_END[] = "-->";
ROM char HTMLSTR_ON[] = "on";
ROM char HTMLSTR_OFF[] = "off";


/************************************************
 ***** CPU Port variables                   *****
 ************************************************/
#define HTTPVAR_GRP_PORTA ('a')
#define HTTPVAR_GRP_PORTB ('b')
#define HTTPVAR_GRP_PORTC ('c')
#define HTTPVAR_GRP_PORTD ('d')
#define HTTPVAR_GRP_PORTE ('e')
#define HTTPVAR_GRP_PORTF ('f')
#define HTTPVAR_GRP_PORTG ('g')
#define HTTPVAR_GRP_PORTH ('h')


/************************************************
 ***** General purpose variables            *****
 ************************************************/
#define HTTPVAR_GRP_GENERAL ('j')

#define VAL_USERNAME        (0x00)


/************************************************
 ***** Default page                         *****
 ************************************************/
ROM char PAGE_UNKNOWN[] = "INDEX.HTM";


/************************************************
 ***** Login page                           *****
 ************************************************/
ROM char PAGE_LOGIN[] = "LOGIN.CGI";

//Admin username and password
ROM char USERNAME_ADMIN[]    = "mxadmin";
ROM char PASSWORD_ADMIN[]    = "pw";
//Superuser username and password
ROM char USERNAME_SUPER[]    = "mxsuper";
ROM char PASSWORD_SUPER[]    = "pw";

#define USERNAME_ADMIN_FLAG         (0b11111110)
#define PASSWORD_ADMIN_FLAG         (0b11111101)
#define LOGIN_SUCCESS_ADMIN_MASK    (0b00000011)

#define USERNAME_SUPER_FLAG         (0b11111011)
#define PASSWORD_SUPER_FLAG         (0b11110111)
#define LOGIN_SUCCESS_SUPER_MASK    (0b00001100)


/************************************************
 ***** Network Settings page                *****
 ************************************************/
ROM char PAGE_NETWORK_CONFIG[] = "NCFG.CGI";

//HTTP Variable group and values
#define HTTPVAR_GRP_NETWORK_CONFIG ('N')
#define HTTPVAR_VAL_ENABLE_MACEDIT (0xE0)

ROM char HTMLSTR_NET_INPUT[] =        "<td><input type=text size=5 maxlength=3 name=";
#define VAL_HTMLSTR_NET_INPUT        (0xF0)

//IP address
#define VAL_IP0	            (0x00)
#define VAL_IP1             (0x01)
#define VAL_IP2             (0x02)
#define VAL_IP3             (0x03)
//Subnet mask
#define VAL_MASK0           (0x04)
#define VAL_MASK1           (0x05)
#define VAL_MASK2           (0x06)
#define VAL_MASK3           (0x07)
//Gateway address
#define VAL_GATEWAY0        (0x08)
#define VAL_GATEWAY1        (0x09)
#define VAL_GATEWAY2        (0x0A)
#define VAL_GATEWAY3        (0x0B)
//MAC address
#define VAL_MAC0            (0x10)
#define VAL_MAC1            (0x11)
#define VAL_MAC2            (0x12)
#define VAL_MAC3            (0x13)
#define VAL_MAC4            (0x14)
#define VAL_MAC5            (0x15)


/************************************************
 ***** USART Settings page                  *****
 ************************************************/
ROM char PAGE_USART_CONFIG[] = "UCFG.CGI";

//HTTP Variable group and values
#define HTTPVAR_GRP_USART_CONFIG ('U')


/************************************************
 ***** CPU Port Settings AND Values pages   *****
 ************************************************/
ROM char PAGE_IO_CONFIG[] = "IOCFG.CGI";
ROM char PAGE_IO_VALUE[] = "IOVAL.CGI";

//HTTP Variable group and values - Reserves A00 to A3F
#define HTTPVAR_GRP_CPU_PORT ('I')

ROM char HTMLSTR_TD_INPUT[] =       "<td align=\"center\"><input type=\"checkbox\" name=";
ROM char HTMLSTR_TD_CEN40[] =       "<td width=\"40\" align=\"center\">";
ROM char HTMLSTR_TD_CEN[] =         "<td align=\"center\">";
ROM char HTMLSTR_INPUT_SUBMIT[] =   "<input class=\"pinbtn\" type=\"submit\" value=";
ROM char HTMLSTR_SPAN[] =           "<span class=\"pinlbl\">";

#define VAL_HTMLSTR_TD_INPUT        (0x00)
#define VAL_HTMLSTR_TD_CEN40        (0x01)
#define VAL_HTMLSTR_TD_CEN          (0x02)
#define VAL_HTMLSTR_INPUT_SUBMIT    (0x03)
#define VAL_HTMLSTR_SPAN            (0x04)


/************************************************
 ***** CPU ADC Settings AND Values pages    *****
 ************************************************/
ROM char PAGE_ADC_CONFIG[] = "ACFG.CGI";
ROM char PAGE_ADC_VALUE[] = "AVAL.CGI";

//HTTP Variable group and values
#define HTTPVAR_GRP_CPU_ADC ('A')

ROM char HTMLSTR_OPTION[] =         "<option value=";
ROM char HTMLSTR_OPTION_END[] =     "</option>";
#define VAL_HTMLSTR_OPTION          (0xF0)
#define VAL_HTMLSTR_OPTION_END      (0xF1)

// AKHE
/************************************************
 ***** CPU PWM Settings AND Values pages    *****
 ************************************************/
ROM char PAGE_PWM_CONFIG[] = "PWMCFG.CGI";
ROM char PAGE_PWM_VALUE[] = "PWMVAL.CGI";

//HTTP Variable group and values
#define HTTPVAR_GRP_CPU_PWM ('P')

// AKHE
/************************************************
 *****        SCALED Values pages           *****
 ************************************************/
//ROM char PAGE_PWM_CONFIG[] = "PWMCFG.CGI";
ROM char PAGE_SCALED_VALUE[] = "SCVAL.CGI";

//HTTP Variable group and values
#define HTTPVAR_GRP_CPU_SCALED ('S')


#ifdef DEBUG_HTTPEXEC
static ROM char DebugMsgGetFile[] = "\r\nHTTPEXEC Get f='";
static ROM char DebugMsgGetName[] = "' n='";
static ROM char DebugMsgGetValue[] = "' v='";
static ROM char DebugMsgGetEnd[] = "'\r\n";
#endif

/*********************************************************************
 * Function:        void htpexecInit(void)
 *
 * Overview:        Initializes global variables.
 ********************************************************************/
#if defined(STACK_USE_HTTP_SERVER)

void htpexecInit(void)
{
    httpUser.user = HTTP_USER_GUEST;    //Initialze with GUEST
}
#endif

/*********************************************************************
 * Function:        void HTTPExecGetCmd(TCP_SOCKET s, BYTE* rqstRes)
 *
 * PreCondition:    None
 *
 * Input:           s           - Socket
 *                  rqstRes     - Name of the Requested resource - GET command's action
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a "callback" from HTTPServer task. Whenever a remote node
 *                  performs interactive GET task on page that was served, HTTPServer calls this
 *                  functions. Use HTTPGetParam() to get all name-value parameters.
 *
 * Note:            none
 ********************************************************************/
#if defined(STACK_USE_HTTP_SERVER)

#define FLAGS_SAVE_APPCFG 0x01
ROM char GET_UPDATE[] = "Update";

void HTTPExecGetCmd(TCP_SOCKET s, BYTE* rqstRes)
{
    BYTE param[15];

    //Used as input AND output parameter for HTTPGetParams.
    // - Input parameter indicates size of param buffer
    // - On return of HTTPGerParam() will contain index of value string in param
    BYTE valueIdx;
    BYTE moreParams;
    BYTE tmp;  
    WORD wtmp;	     
    BYTE saveConf;
    BYTE* p;        //Temperory pointer
    BYTE flags;

    #ifdef DEBUG_HTTPEXEC
    debugPutRomString(DebugMsgGetFile);
    debugPutString(rqstRes);
    #endif

    //Clear all flags
    flags = 0;

    /////////////////////////////////////////////////
    //Login Page
    if (strStartsWith(PAGE_LOGIN, rqstRes)) {
        tmp = 0xff;  //Reset

        //Get next name-value parameter
        do {
            valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

            //Get name-value parameters. Returns true if there are more name-value parameters to follow
            //- Pointer to Name parameter = &param[0]
            //- Pointer to Value parameter = &param[valueIdx]
            moreParams = HTTPGetParam(s, param, &valueIdx);

            #ifdef DEBUG_HTTPEXEC
            debugPutRomString(DebugMsgGetName);
            debugPutString(param);
            debugPutRomString(DebugMsgGetValue);
            debugPutString(&param[valueIdx]);
            debugPutRomString(DebugMsgGetEnd);
            #endif

            switch (param[0]) {
            case 'U':
                if ( !memcmppgm2ram((char *)&param[valueIdx], (ROM void*)USERNAME_ADMIN, sizeof(USERNAME_ADMIN) - 1) ) {
                    tmp &= USERNAME_ADMIN_FLAG;
                }
                if ( !memcmppgm2ram((char *)&param[valueIdx], (ROM void*)USERNAME_SUPER, sizeof(USERNAME_SUPER) - 1) ) {
                    tmp &= USERNAME_SUPER_FLAG;
                }
                break;
            case 'P':
                if ( !memcmppgm2ram((char *)&param[valueIdx], (ROM void*)PASSWORD_ADMIN, sizeof(PASSWORD_ADMIN) - 1) ) {
                    tmp &= PASSWORD_ADMIN_FLAG;
                }
                if ( !memcmppgm2ram((char *)&param[valueIdx], (ROM void*)PASSWORD_SUPER, sizeof(PASSWORD_SUPER) - 1) ) {
                    tmp &= PASSWORD_SUPER_FLAG;
                }
                break;
            }
        } while (moreParams);

        //Store the current HTTP User type
        httpUser.ipAdr.Val = TCPGetSocketInfo(s)->remote.IPAddr.Val;
        if ((tmp & LOGIN_SUCCESS_SUPER_MASK)==0)  {
            httpUser.user = HTTP_USER_SUPER;
            debugPutByte(' ');
            debugPutByte('S');
        }
        else if ((tmp & LOGIN_SUCCESS_ADMIN_MASK)==0)  {
            httpUser.user = HTTP_USER_ADMIN;
            debugPutByte(' ');
            debugPutByte('A');
        }
        else {
            httpUser.user = HTTP_USER_GUEST;
            debugPutByte(' ');
            debugPutByte('G');
        }
    }
    /////////////////////////////////////////////////
    //Network Configuration Page
    else if (strStartsWith(PAGE_NETWORK_CONFIG, rqstRes)) {
        //Get next name-value parameter
        do {
            valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

            //Get name-value parameters. Returns true if there are more name-value parameters to follow
            //- Pointer to Name parameter = &param[0]
            //- Pointer to Value parameter = &param[valueIdx]
            moreParams = HTTPGetParam(s, param, &valueIdx);

            #ifdef DEBUG_HTTPEXEC
            debugPutRomString(DebugMsgGetName);
            debugPutString(param);
            debugPutRomString(DebugMsgGetValue);
            debugPutString(&param[valueIdx]);
            debugPutRomString(DebugMsgGetEnd);
            #endif

            tmp = (BYTE)atoi((char *)&param[valueIdx]);

            //Switch on first character of Name
            switch (param[0]) {
            case 'A':
                AppConfig.MyIPAddr.v[0] = tmp;
                break;
            case 'B':
                AppConfig.MyIPAddr.v[1] = tmp;
                break;
            case 'C':
                AppConfig.MyIPAddr.v[2] = tmp;
                break;
            case 'D':
                AppConfig.MyIPAddr.v[3] = tmp;
                break;
            case 'E':
                AppConfig.MyMask.v[0] = tmp;
                break;
            case 'F':
                AppConfig.MyMask.v[1] = tmp;
                break;
            case 'G':
                AppConfig.MyMask.v[2] = tmp;
                break;
            case 'H':
                AppConfig.MyMask.v[3] = tmp;
                break;
            case 'I':
                AppConfig.MyGateway.v[0] = tmp;
                break;
            case 'J':
                AppConfig.MyGateway.v[1] = tmp;
                break;
            case 'K':
                AppConfig.MyGateway.v[2] = tmp;
                break;
            case 'L':
                AppConfig.MyGateway.v[3] = tmp;
                break;
            case 'M':
                AppConfig.MyMACAddr.v[0] = tmp;
                break;
            case 'N':
                AppConfig.MyMACAddr.v[1] = tmp;
                break;
            case 'O':
                AppConfig.MyMACAddr.v[2] = tmp;
                break;
            case 'P':
                AppConfig.MyMACAddr.v[3] = tmp;
                break;
            case 'Q':
                AppConfig.MyMACAddr.v[4] = tmp;
                break;
            case 'R':
                AppConfig.MyMACAddr.v[5] = tmp;
                break;
            //default:    //Name not supported, do nothing
            }
        } while (moreParams);

        //Save application config data
        flags |= FLAGS_SAVE_APPCFG;

        //If Guest logged in, load login page
        /*
        if (httpUser.user == HTTP_USER_GUEST)
        {
            memcpypgm2ram((void*)rqstRes,
                    (ROM void*)PAGE_LOGIN, sizeof(PAGE_LOGIN));
        }
        */
    }
    /////////////////////////////////////////////////
    //USART Configuration Page
    else if (strStartsWith(PAGE_USART_CONFIG, rqstRes)) {
        //Get next name-value parameter
        do {
            valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

            //Get name-value parameters. Returns true if there are more name-value parameters to follow
            //- Pointer to Name parameter = &param[0]
            //- Pointer to Value parameter = &param[valueIdx]
            moreParams = HTTPGetParam(s, param, &valueIdx);

            #ifdef DEBUG_HTTPEXEC
            debugPutRomString(DebugMsgGetName);
            debugPutString(param);
            debugPutRomString(DebugMsgGetValue);
            debugPutString(&param[valueIdx]);
            debugPutRomString(DebugMsgGetEnd);
            #endif

            //Switch on first character of Name
            switch (param[0]) {
            case 'A':
                tmp = param[valueIdx] - '0';
                if ( (tmp >= 0) && (tmp <= 9) ) {
                    AppConfig.USART1Cfg.baud = tmp;
                }
                break;
            }
        } while (moreParams);

        //Reconfigure USART with new value
        appcfgUSART();

        //Save application config data
        flags |= FLAGS_SAVE_APPCFG;
    }
    /////////////////////////////////////////////////
    //IO Configuration Page
    else if (strStartsWith(PAGE_IO_CONFIG, rqstRes)) {

        //This page will only send name-value parameters for ports that are configured as outputs.
        //We thus have go set all ports as inputs here, and following name-value parameters will set
        //the outputs
        AppConfig.PortCfg.trisa = 0x3f; //Set A0 - A5 as inputs
        AppConfig.PortCfg.trisb = 0xfe; //Set B1 - B7 as inputs
        AppConfig.PortCfg.trisc = 0x27; //Set C0, C1, C2 and C5 as inputs
        AppConfig.PortCfg.trisf = 0xff; //Set F0 - F7 as inputs

        //Get next name-value parameter
        do {
            valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

            //Get name-value parameters. Returns true if there are more name-value parameters to follow
            //- Pointer to Name parameter = &param[0]
            //- Pointer to Value parameter = &param[valueIdx]
            moreParams = HTTPGetParam(s, param, &valueIdx);

            #ifdef DEBUG_HTTPEXEC
            debugPutRomString(DebugMsgGetName);
            debugPutString(param);
            debugPutRomString(DebugMsgGetValue);
            debugPutString(&param[valueIdx]);
            debugPutRomString(DebugMsgGetEnd);
            #endif

            //We received a request to clear a TRIS bit
            if ((param[0] == 'A') || (param[0] == 'B') || (param[0] == 'C') || (param[0] == 'F'))
            {
                //Get pointer to tris config register
                p = (&AppConfig.PortCfg.trisa) + (param[0] - 'A');

                //Second character of name will be bit position. For example A3, B6, F0...
                tmp = 0x01 << (param[1] - '0');

                //Clear tris bit for received name - set it as an output.
                *p &= (~tmp);
            }

        } while (moreParams);

        //Reconfigure CPU I/O ports
        appcfgCpuIO();

        //Save application config data
        flags |= FLAGS_SAVE_APPCFG;
    }
    /////////////////////////////////////////////////
    //ADC Configuration Page
    else if (strStartsWith(PAGE_ADC_CONFIG, rqstRes)) {
        //Get next name-value parameter
        do {
            valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

            //Get name-value parameters. Returns true if there are more name-value parameters to follow
            //- Pointer to Name parameter = &param[0]
            //- Pointer to Value parameter = &param[valueIdx]
            moreParams = HTTPGetParam(s, param, &valueIdx);

            #ifdef DEBUG_HTTPEXEC
            debugPutRomString(DebugMsgGetName);
            debugPutString(param);
            debugPutRomString(DebugMsgGetValue);
            debugPutString(&param[valueIdx]);
            debugPutRomString(DebugMsgGetEnd);
            #endif

            //Switch on first character of Name
            switch (param[0]) {
            //Selects number of channels
            case 'A':
                AppConfig.ADCCfg.adcon1 &= 0xf0;    //Clear number of channel selection bits

                //The value characters were chose so that their lower nibble has the correct 4 bit binary value
                AppConfig.ADCCfg.adcon1 |= (param[valueIdx] & 0x0F);
                break;
            //Selects reference voltage
            case 'B':
                AppConfig.ADCCfg.adcon1 &= ~0x30;    //Clear reference voltage selection bits

                AppConfig.ADCCfg.adcon1 |= ((param[valueIdx] << 4) & 0x30);
                break;
            //Selects Acquisition time
            case 'C':
                AppConfig.ADCCfg.adcon2 &= ~0x38;    //Clear Acquisition time selection bits

                //The value characters were chose so that the lower 3 bits has the correct 3 bit binary value
                AppConfig.ADCCfg.adcon2 |= ((param[valueIdx] << 3) & 0x38);
                break;
            }
        } while (moreParams);

        //Reconfigure USART with new value
        appcfgADC();

        //Save application config data
        flags |= FLAGS_SAVE_APPCFG;
    }
    // AKHE
    /////////////////////////////////////////////////
    //PWM Configuration Page
    else if (strStartsWith(PAGE_PWM_CONFIG, rqstRes)) {
		tmp = 1;
	}
    /////////////////////////////////////////////////
    //Parse parameters contained on page
    else  {
        //Get next name-value parameter
        do {
            valueIdx = (BYTE)sizeof(param);   //Input parameter is size of param buffer

            //Get name-value parameters. Returns true if there are more name-value parameters to follow
            //- Pointer to Name parameter = &param[0]
            //- Pointer to Value parameter = &param[valueIdx]
            moreParams = HTTPGetParam(s, param, &valueIdx);

            #ifdef DEBUG_HTTPEXEC
            debugPutRomString(DebugMsgGetName);
            debugPutString(param);
            debugPutRomString(DebugMsgGetValue);
            debugPutString(&param[valueIdx]);
            debugPutRomString(DebugMsgGetEnd);
            #endif

            //We received a request to set or clear a PORT bit
            if ((param[0] >= 'A') && (param[0] <= 'F'))
            {
                //Get pointer to requested LATCH register
                p = (BYTE *)(&LATA) + (param[0] - 'A');

                //Second character of name will be bit position. For example A3, B6, F0...
                tmp = 0x01 << (param[1] - '0');

                //Toggle port pin. If current value is 1, clear is, and visa-versa
                if (param[valueIdx] == '1') {
                    //Clear port bit
                    *p &= (~tmp);
                }
                else if (param[valueIdx] == '0') {
                    //Set port bit
                    *p |= tmp;
                }
            }
            // Set PWM value AKHE
            else if ( param[0] == 'P')
            {
	           wtmp = atoi( (char *)&param[valueIdx] );
	           wtmp &= 0x3ff; 			// max 1023
	           CCP1CON &= 0x0f;
	           CCP1CON |= ( wtmp & 3 ) << 4;
	           CCPR1L = (wtmp >> 2 );	 
	        }

        } while (moreParams);
    }
    //Unknown page
    //else {
    //    memcpypgm2ram((void*)rqstRes,
    //            (ROM void*)PAGE_UNKNOWN, sizeof(PAGE_UNKNOWN));
    //}

    //Must application config be saved?
    if ((flags & FLAGS_SAVE_APPCFG) != 0) {
        appcfgSave();   //Save new configuration to EEPROM and delay till I2C write is finished
        DelayMs(50);
    }
}
#endif


/*********************************************************************
 * Function:        WORD HTTPGetVar(HTTP_VAR httpVar, WORD ref, BYTE* val)
 *
 * PreCondition:    None
 *
 * Input:           var         - HTTP Variable. Contains a group and variable part.
 *                  ref         - Current callback reference with
 *                                respect to 'var' variable.
 *                  val         - Buffer for value storage. 
 *
 * Output:          Variable reference as required by application.
 *
 * Side Effects:    None
 *
 * Overview:        This is a callback function from HTTPServer() to main application.
 *                  Whenever a variable substitution is required on any html pages, HTTPServer
 *                  calls this function with a 8-bit variable identifier, variable reference,
 *                  which indicates whether this is a first call or not. Application should
 *                  return one character at a time as a variable value. If there are no more
 *                  characters, or requeseted variable not found, this function must set
 *                  var to NULL ('\0').
 *
 * Note:            Since this function only allows one character to be returned at a time as
 *                  part of variable value, HTTPServer() calls this function multiple times
 *                  until main application indicates that there is no more value left for this
 *                  variable. On begining, HTTPGetVar() is called with ref = HTTP_START_OF_VAR to
 *                  indicate that this is a first call.  Application should use this reference to
 *                  start the variable value extraction and return updated reference.  If there is
 *                  no more values left for this variable application should send HTTP_END_OF_VAR
 *                  AND set var to NULL. If there are any bytes to send, application should return
 *                  other than HTTP_START_OF_VAR and HTTP_END_OF_VAR reference.
 ********************************************************************/
#if defined(STACK_USE_HTTP_SERVER)


WORD HTTPGetVar(HTTP_VAR httpVar, WORD ref, BYTE* val)
{
    BYTE tmp;
    WORD wtmp;
    long ltmp;
    BYTE flags;
    BYTE varValue;
    BYTE* p;
    BYTE mask;
    char buf[ 6 ];

    #define FLAGS_OUTPUT_PIN    0x01
    #define FLAGS_ADC_PIN       0x02

    varValue = httpVar.val;

    //In case requested var not found, set it to NULL character and return HTTP_END_OF_VAR
    *val = '\0';    


    /////////////////////////////////////////////////
    //General purpose and Port pin variables - they are:
    // - 'a' Used for port a value. a0x will display 1 or 0, a1x will display on or off
    // - 'b' Used for port b value.
    // - 'c' Used for port c value.
    // - 'd' Reserved for port d value.
    // - 'e' Reserved for port e value.
    // - 'f' Used for port f value.
    // - 'g' Reserved for port g value.
    // - 'h' Reserved for port h value.
    // - 'i' Used for Analog port input value. i0x will display 
    // - 'j' Used for general purpose values
  
    if ((httpVar.group >= 'a') && (httpVar.group <= 'k'))
    {
        //The value of the requested variable will result in the following being returned:
        // 00 to 07 - 1 or 0 returned depending on PORT state
        // 10 to 17 - "on" or "off" returned depending on PORT state
        // 20 to 27 - "<!--" returned if port is configured as input
        // 28 to 2f - "-->" returned if port is configured as input
        // 30 to 37 - "<!--" returned if port is configured as output
        // 38 to 3f - "-->" returned if port is configured as output
        // 40 to 47 - "checked" returned if port is configured as output

        flags = 0;

        //A PORT pin has been requested
        if ((httpVar.group >= HTTPVAR_GRP_PORTA) && (httpVar.group <= HTTPVAR_GRP_PORTH))
        {
            //Get register offset. PORTA = a, PORTB = b, PORTC = c......
            tmp = (httpVar.group - 'a');

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

            tmp = httpVar.val & 0xf0;

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
                        ((httpVar.val & 0x08) == 0) ? HTMLSTR_CMNT_START : HTMLSTR_CMNT_END);
            }
            //Return "checked" if port configured as output
            else if ((tmp == 0x40) && ((flags & FLAGS_OUTPUT_PIN) != 0))
            {
                return getHTTPROMStringVar(ref, val, HTMLSTR_CHECKED);
            }
        }
        //A general purpose variable has been requested
        else if (httpVar.group == HTTPVAR_GRP_GENERAL)
        {
            switch(varValue)
            {
            case VAL_USERNAME:
                return getHTTPROMStringVar(ref, val, HTTP_USERS[httpUser.user]);
            }
        }
    }
    /////////////////////////////////////////////////
    //ADC variables
    if (httpVar.group == 'n')
    {
        //The value of the requested variable will result in the following being returned:
        // 00 to 0A - 2 or 3 digit hex value in capital letters
        // 10 to 1A - decimal value 0 - 255 or 0 - 1023
        // 20 to 2A - decimale value, 2 decimal places for 5V reference
        // 30 to 3A - "<!--" returned if the channels is configured for ADC
        // 40 to 4A - "-->" returned if the channels is configured for ADC
        // 50 to 5A - "<!--" returned if the channels is NOT configured for ADC
        // 60 to 6A - "-->" returned if the channels is NOT configured for ADC
        // 70 to 7A - decimale value, 2 decimal places for 5V reference and 3 times voltage diveder = 0-15V range

        flags = 0;

        //Has channel being configured as ADC
        if ((httpVar.val & 0x0f) < (~ADCON1 & 0x0f))
        {
            flags |= FLAGS_ADC_PIN;
        }

        tmp = httpVar.val & 0xf0;

        //Return hex value
        if (tmp == 0x00)
        {
            return getHTTPROMStringVar(ref, val, STR_NOT_IMPLEMENTED);
        }
        //Return decimal value 0 - 1023
        else if (tmp == 0x10)
        {
            //AppConfig.Flags.bFreezeADCBuf = 1;

#if defined(APP_USE_ADC8)
            return getHTTPByteVar(ref, val, AdcValues[httpVar.val & 0x0f]);
#elif defined(APP_USE_ADC10)
            return getHTTPWordVar(ref, val, AdcValues[httpVar.val & 0x0f]);
#endif
        }
        //Return decimal value (2 decimal point precicion) for if 5V reference was used
        else if (tmp == 0x20)
        {
#if defined(APP_USE_ADC8)

            tmp = AdcValues[httpVar.val & 0x0f];

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

  			// AKHE
  			wtmp = AdcValues[httpVar.val & 0x0f];

            if ( ref == (BYTE)HTTP_START_OF_VAR )
            {
	         	// Convert tmp value to voltage (3 decimal points) value for 5V reference
	         	strTmp[0] = (10 * wtmp)/2046 + '0';
                
                strTmp[1] = '.';
                
                // First decimal
                wtmp = ( wtmp * 10 ) % 2046;
                strTmp[2] = (10 * wtmp)/2046 + '0';
                
                // Second decimal
                wtmp = ( wtmp * 10 ) % 2046;
                strTmp[3] = (10 * wtmp)/2046 + '0';
                
                // Third decimal
                wtmp = ( wtmp * 10 ) % 2046;
                strTmp[4] = (10 * wtmp)/2046 + '0';
                
                strTmp[5] = '\0';  //Terminate with 0
	        }
	        
            *val = strTmp[(BYTE)ref];
            if ( strTmp[(BYTE)ref] == '\0' )
                return HTTP_END_OF_VAR;
            (BYTE)ref++;
            return ref;
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
        //Return "checked" if port configured as output
        else if ((tmp == 0x40) && ((flags & FLAGS_OUTPUT_PIN) != 0))
        {
            return getHTTPROMStringVar(ref, val, HTMLSTR_CHECKED);
        }
        //Return decimal value (2 decimal point precicion) for if 12V reference was used
        else if (tmp == 0x70)
        {
#if defined(APP_USE_ADC8)

            tmp = AdcValues[httpVar.val & 0x0f];

            if ( ref == (BYTE)HTTP_START_OF_VAR )
            {
                //Convert tmp value to voltage (2 decimal points) value for 5V reference and 3 time
                //voltage divider (0 - 15V input range)
                //Add you code here to convert 0-255 range in tmp to ASCII 0-15V (or 0 -12V for you) range
                //Place ASCII result in strTmp[] array, and end with 0 termination.
                //Example below will always return "15.00" string.
                strTmp[0] = '1';
                strTmp[1] = '5';
                strTmp[2] = '.';
                strTmp[3] = '0';
                strTmp[4] = '0';
                strTmp[5] = '\0';  //Terminate with 0

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
        
    }
    
    // AKHE
    /////////////////////////////////////////////////
    // PWM Settings and Values pages
    else if ( httpVar.group == HTTPVAR_GRP_CPU_PWM )
    {
	    wtmp = ( CCP1CON >> 4 );
		wtmp |= ( (WORD)CCPR1L << 2 );
		
		return getHTTPWordVar(ref, val, wtmp );   
	}
	
	
    /////////////////////////////////////////////////
    //Network Configuration variables
    else if (httpVar.group == HTTPVAR_GRP_NETWORK_CONFIG)
    {
        switch(varValue)
        {
        case VAL_IP0:
            return getHTTPByteVar(ref, val, AppConfig.MyIPAddr.v[0]);
        case VAL_IP1:
            return getHTTPByteVar(ref, val, AppConfig.MyIPAddr.v[1]);
        case VAL_IP2:
            return getHTTPByteVar(ref, val, AppConfig.MyIPAddr.v[2]);
        case VAL_IP3:
            return getHTTPByteVar(ref, val, AppConfig.MyIPAddr.v[3]);
        case VAL_MASK0:
            return getHTTPByteVar(ref, val, AppConfig.MyMask.v[0]);
        case VAL_MASK1:
            return getHTTPByteVar(ref, val, AppConfig.MyMask.v[1]);
        case VAL_MASK2:
            return getHTTPByteVar(ref, val, AppConfig.MyMask.v[2]);
        case VAL_MASK3:
            return getHTTPByteVar(ref, val, AppConfig.MyMask.v[3]);
        case VAL_GATEWAY0:
            return getHTTPByteVar(ref, val, AppConfig.MyGateway.v[0]);
        case VAL_GATEWAY1:
            return getHTTPByteVar(ref, val, AppConfig.MyGateway.v[1]);
        case VAL_GATEWAY2:
            return getHTTPByteVar(ref, val, AppConfig.MyGateway.v[2]);
        case VAL_GATEWAY3:
            return getHTTPByteVar(ref, val, AppConfig.MyGateway.v[3]);
        case VAL_MAC0:
            return getHTTPByteVar(ref, val, AppConfig.MyMACAddr.v[0]);
        case VAL_MAC1:
            return getHTTPByteVar(ref, val, AppConfig.MyMACAddr.v[1]);
        case VAL_MAC2:
            return getHTTPByteVar(ref, val, AppConfig.MyMACAddr.v[2]);
        case VAL_MAC3:
            return getHTTPByteVar(ref, val, AppConfig.MyMACAddr.v[3]);
        case VAL_MAC4:
            return getHTTPByteVar(ref, val, AppConfig.MyMACAddr.v[4]);
        case VAL_MAC5:
            return getHTTPByteVar(ref, val, AppConfig.MyMACAddr.v[5]);
        case HTTPVAR_VAL_ENABLE_MACEDIT:
            //Only super user can edit MAC address. For all the rest, the input field is disabled.
            if (httpUser.user != HTTP_USER_SUPER) {
                return getHTTPROMStringVar(ref, val, HTMLSTR_DISABLED);
            }
            break;
        case VAL_HTMLSTR_NET_INPUT:
            return getHTTPROMStringVar(ref, val, HTMLSTR_NET_INPUT);
        }
    }
    /////////////////////////////////////////////////
    //USART Configuration variables
    else if (httpVar.group == HTTPVAR_GRP_USART_CONFIG)
    {
        //If the requested var is for the current baud rate, return "selected" string. This
        //string will be inserted in the option form, and cause it to be selected
        if (varValue == AppConfig.USART1Cfg.baud)
        {
            return getHTTPROMStringVar(ref, val, HTMLSTR_SELECTED);
        }
    }
    /////////////////////////////////////////////////
    //PIC Port Settings and Values pages
    else if (httpVar.group == HTTPVAR_GRP_CPU_PORT)
    {
        //The following HTML code snippets are inserted to reduce the size of the HTML page stored
        //in EEPROM.
        if (varValue == VAL_HTMLSTR_TD_INPUT) {
            return getHTTPROMStringVar(ref, val, HTMLSTR_TD_INPUT);
        }
        else if (varValue == VAL_HTMLSTR_TD_CEN40) {
            return getHTTPROMStringVar(ref, val, HTMLSTR_TD_CEN40);
        }
        else if (varValue == VAL_HTMLSTR_TD_CEN) {
            return getHTTPROMStringVar(ref, val, HTMLSTR_TD_CEN);
        }
        else if (varValue == VAL_HTMLSTR_INPUT_SUBMIT) {
            return getHTTPROMStringVar(ref, val, HTMLSTR_INPUT_SUBMIT);
        }
        else if (varValue == VAL_HTMLSTR_SPAN) {
            return getHTTPROMStringVar(ref, val, HTMLSTR_SPAN);
        }
    }
    /////////////////////////////////////////////////
    //ADC Settings and Values pages
    else if (httpVar.group == HTTPVAR_GRP_CPU_ADC)
    {
        tmp = varValue & 0xf0;

        //The following HTML code snippets are inserted to reduce the size of the HTML page stored
        //in EEPROM.
        if (tmp == 0xf0)
        {
            if (varValue == VAL_HTMLSTR_OPTION) {
                return getHTTPROMStringVar(ref, val, HTMLSTR_OPTION);
            }
            else if (varValue == VAL_HTMLSTR_OPTION_END) {
                return getHTTPROMStringVar(ref, val, HTMLSTR_OPTION_END);
            }
        }
        //Current selected channel
        else if (tmp == 0x00)
        {
            //If current value = selected number of channels
            if (varValue == (AppConfig.ADCCfg.adcon1 & 0x0f))
                return getHTTPROMStringVar(ref, val, HTMLSTR_SELECTED);
        }
        //Current selected channel
        else if (tmp == 0x10)
        {
            //If current value = selected number of channels
            if ((varValue & 0x03) == ((AppConfig.ADCCfg.adcon1 >> 4) & 0x03))
                return getHTTPROMStringVar(ref, val, HTMLSTR_SELECTED);
        }
        //Current selected aquisition time
        else if (tmp == 0x20)
        {
            //If current value = selected aquisition time
            if ((varValue & 0x0f) == ((AppConfig.ADCCfg.adcon2 >> 3) & 0x0f))
                return getHTTPROMStringVar(ref, val, HTMLSTR_SELECTED);
        }
    }
    
    return HTTP_END_OF_VAR;
}
#endif





/*
 * Returns a string representation of a given ROM string to repeditive calls by HTTPGetVar()
 *
 * Input:           ref         - Current callback reference used by HTTPGetVar
 *                  val         - Buffer for value storage.
 *                  b           - Byte value to display
 * Output:          HTTPGetVar return value.
 * 
 */
static WORD getHTTPROMStringVar(BYTE ref, BYTE* val, ROM char* str)
{
    *val = str[(BYTE)ref];
    if (*val == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}

/*
 * Returns a string representation of a given ROM string to repeditive calls by HTTPGetVar()
 *
 * Input:           ref         - Current callback reference used by HTTPGetVar
 *                  val         - Buffer for value storage.
 *                  str         - String to display
 * Output:          HTTPGetVar return value.
 * 
 */
static WORD getHTTPStringVar(BYTE ref, BYTE* val, BYTE* str)
{
    *val = str[(BYTE)ref];
    if (*val == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}

/*
 * Handle HTTPGetVar for a string representation of a byte.
 *
 * Input:           ref         - Current callback reference used by HTTPGetVar
 *                  val         - Buffer for value storage.
 *                  b           - Byte value to display
 * Output:          HTTPGetVar return value.
 * 
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

/*
 * Handle HTTPGetVar for a string representation of a word.
 *
 * Input:           ref         - Current callback reference used by HTTPGetVar
 *                  val         - Buffer for value storage.
 *                  w           - Byte value to display
 * Output:          HTTPGetVar return value.
 * 
 */
static WORD getHTTPWordVar(BYTE ref, BYTE* val, WORD w)
{
    if ( ref == (BYTE)HTTP_START_OF_VAR )
    {
        ltoa(w, strTmp);
        strTmp[STRTMP_MAXSIZE - 1] = '\0';  //Terminate with 0
        ref = (BYTE)0;
    }
    *val = strTmp[(BYTE)ref];
    if ( strTmp[(BYTE)ref] == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}

/*
 * Checks if given string begins with string to look for.
 *
 * Output:          True if found, else 0
 */
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
