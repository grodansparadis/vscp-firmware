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
 * 2008-10-13, David Hosken (DH):
 *    - Application config now also saved to EEPROM for commands received via UDP
 * 2008-07-10, David Hosken (DH):
 *    - Fixed bug in UDP command, where command is not NULL terminated
 * 2006-09-03, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#define THIS_IS_CMD_MODULE

#include <string.h>

#include "projdefs.h"
#include "cmd.h"
#include "debug.h"
#include "appcfg.h"
#include "lcd2s.h"
#include "ior5e.h"
#include "mxd2r.h"

#include "net\stacktsk.h"
#include "net\udp.h"
#include "net\helpers.h"
#include "net\security.h"   //Security related code




/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xDA, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xDA, msgCode, msgStr)


/////////////////////////////////////////////////
// Function prototypes
//static WORD cmdGetByteVar(BYTE ref, BYTE* val, BYTE b);
#define cmdGetByteVar(ref, val, b) cmdGetWordVar(ref, val, b)
static WORD cmdGetWordVar(BYTE ref, BYTE* val, WORD w);
static WORD cmdGetByteHexVar(BYTE ref, BYTE* val, BYTE b);
static WORD cmdGetWordHexVar(BYTE ref, BYTE* val, WORD w);
static WORD cmdGetROMStringVar(BYTE ref, BYTE* val, ROM char* str);
static WORD cmdGetEepromStringVar(BYTE ref, BYTE* val, WORD adr);
//static WORD cmdGetStringVar(BYTE ref, BYTE* val, BYTE* str);


#define HTTP_START_OF_VAR       (0x0000ul)
#define HTTP_END_OF_VAR         (0xFFFFul)

/////////////////////////////////////////////////
// ROM Strings
ROM char HTMLSTR_CHECKED[] = "checked";
ROM char HTMLSTR_DISABLED[] = "disabled";
ROM char HTMLSTR_CMNT_START[] = "<!--";
ROM char HTMLSTR_CMNT_END[] = "-->";
ROM char HTMLSTR_ON[] = "on";
ROM char HTMLSTR_OFF[] = "off";
ROM char STR_NOT_IMPLEMENTED[] = "Not Implemented";


/////////////////////////////////////////////////
// Variables
#define STRTMP_MAXSIZE 6
char strTmp[STRTMP_MAXSIZE];


//Create a UDP socket for the "UDP command port"
static UDP_SOCKET udpSocketCmd = INVALID_UDP_SOCKET;

union
{
    struct
    {
        unsigned int bDontReadNextChar : 1;     //Don't read next character, we are still processing current one!
        unsigned int bCloseAndExit : 1;         //Close connection, and exit
        
    } bits;
    BYTE val;
} cmdFlags;


/////////////////////////////////////////////////
// "Command State Machine"
typedef enum _SM_CMD
{
    SM_CMD_IDLE=0,      /* Idle state */
    SM_CMD_UDP,         /* Currently receiving a UDP command */
    SM_CMD_TCP,         /* Currently receiving a TCP command */
    SM_CMD_BUF1         /* Currently receiving data via Buffer 1 */
} SM_CMD;
static BYTE smCmd = SM_CMD_IDLE;    //variable for "Command State Machine"

/////////////////////////////////////////////////
// "Command Execution State Machine"
typedef enum _SM_CMDX
{
    SM_CMDX_IDLE=0,         /* Idle state, search for command */
    SM_CMDX_UNPW,           /* Search for Username and Password */
    SM_CMDX_NAME,           /* We are currently reading the name part of a "Command" */
    SM_CMDX_VALUE,          /* We are currently reading the value part of a "Command" */
    SM_CMDX_EXEC,           /* We are currently executing a "Command" */
    SM_CMDX_GETTAG_GROUP,   /* We are receiving the group part of a "Get Tag Command" */
    SM_CMDX_GETTAG_VALMSB,  /* We are receiving the MSB part of the value of a "Get Tag Command" */
    SM_CMDX_GETTAG_VALLSB,  /* We are receiving the LSB part of the value of a "Get Tag Command" */
    SM_CMDX_GETTAG_EXEC     /* We are currently executing a "Get Tag Command" */
} SM_CMDX;
static BYTE smCmdExec; //Current State of "Command Execution State Machine"

/**
 * Initializes command processing
 */
void cmdInit(void) {
    NODE_INFO udpServerNode;
    
    //Initialize to idle state
    smCmd = SM_CMD_IDLE;

    //Initialize remote IP and MAC address of udpServerNode with 0, seeing that we don't know them for the node
    //that will send us an UDP message. The first time a message is received addressed to this port, the
    //remote IP and MAC addresses are automatically updated with the addresses of the remote node.
    memclr(&udpServerNode, sizeof(udpServerNode));

    //Configure for local port 54123 and remote port INVALID_UDP_PORT. This opens the socket to
    //listen on the given port.
    udpSocketCmd = UDPOpen(CMD_UDPPORT, &udpServerNode, INVALID_UDP_PORT);
    
    //An error occurred during the UDPOpen() function
    if (udpSocketCmd == INVALID_UDP_SOCKET) {
        #if (DEBUG_CMD >= LOG_ERROR)
        debugPutMsg(4);     //@mxd:4:Could not open UDP command port
        #endif
    }
}


/**
 * Must be called every couple of ms.
 * Each UDP message can consists out of any number of "Commands" and "Requests", seperated by '&'
 * characters. Each "Get Tag Command" starts with a % character.
 *
 */
void cmdTask(void) {

    //Variables
    BYTE c;
    WORD wUpdate;   //Contains CMD_UD_XXX cmdFlags indicating what has to be updated
    WORD bytesRead;

    //Variables for Command
    BYTE put;
    BYTE buf[64];

    BYTE valueIdx;  //Index of value in buf[]
    
    GETTAG_INFO getTagInfo;

    wUpdate = 0;    //Clear all update flags

    /////////////////////////////////////////////////
    //Command
    do {
        /////////////////////////////////////////////////
        //Get next command byte (from UDP, TCP, Serial.... protocol), if any

        // Idle state - check if a command is received via UDP, TCP, Serial.....
        if (smCmd == SM_CMD_IDLE) {
            //Clear all flags
            cmdFlags.val = 0;
            
            //Is there any data waiting for us on the "UDP command port"?
            //Because of the design of the Modtronix TCP/IP stack we have to consume all data sent to us
            //as soon as we detect it. Store all data to a buffer as soon as it is detected
            if (UDPIsGetReady(udpSocketCmd)) {

                //UDP Commands are disabled, discard UDP message!
                if ( (appcfgGetc(APPCFG_CMD_FLAGS) & APPCFG_CMDFLAGS_ENABLE) == 0) {
                    UDPDiscard();

                    //Go to top of do{} statement, if do{} conditions are met
                    continue;
                }
                //Process UDP command
                else {
                    smCmd = SM_CMD_UDP;

                    //UDP Commands require username and password!
                    if (appcfgGetc(APPCFG_CMD_FLAGS) & APPCFG_CMDFLAGS_AUTH_UDP) {
                        smCmdExec = SM_CMDX_UNPW;   //UDP command must be preceded by username and password
                    }
                    else {
                        smCmdExec = SM_CMDX_IDLE;   //Look for commands
                    }

                    put = 0;
                    valueIdx = 0;                   //Indicate that value has not been found yet - is everything after '=' character
                    buf[sizeof(buf) - 1] = '\0';    //Terminate last byte of buffer with NULL character
                
                    //Go to top of do{} statement, if do{} conditions are met
                    continue;
                }
            }
            //No data available
            else {
                break;  //Exit do{} statement
            }
        }
        //Currently receiving a UDP command
        else if (smCmd == SM_CMD_UDP) {
            //Discard rest of message, close connection, and exit
            if (cmdFlags.bits.bCloseAndExit == TRUE) {
                UDPDiscard();
                smCmd = SM_CMD_IDLE;   //Reset state
                break;
            }
            else {
                //Read next UDP byte
                if (cmdFlags.bits.bDontReadNextChar == FALSE) {
                    cmdFlags.val = 0;  //Clear all flags

                    //Get next character
                    if (UDPGet(&c) == 0) {
                        //Close connection and exit (done in next do{}while loop) 
                        cmdFlags.bits.bCloseAndExit = TRUE;
                    }
                }
                //Don't read next UDP byte
                else {
                    cmdFlags.bits.bDontReadNextChar = FALSE;   //Reset, next time a char will be read
                }
            }
        }
        //Currently receiving data via Buffer 1
        else if (smCmd == SM_CMD_BUF1) {
            //When receiving data from a buffer, we can NOT assume the whole command is already
            //available in the buffer! With UDP we assume that, and buf[] is not declared static.
            //We assume once we start reading the command from the UDP socket, we will get the
            //whole command, and the cmdTask() function will not be exited.
            //
            //When receiving commands from Buffer 1, we will first peek into the buffer to see
            //if the whole command has been received. Only after confirming this, will we
            //continue.
            //if (BUFFER_1_CONTAINS_WHOLE_MESSAGE == FALSE)
            //{
            //  ...
            //}
            smCmd == SM_CMD_IDLE;
            break;
        }
        //Currently receiving a TCP command
        else if (smCmd == SM_CMD_TCP) {
            smCmd == SM_CMD_IDLE;
            break;
        }

        /////////////////////////////////////////////////
        //Process next command byte
        //At this stage, the next character of the command will be in the 'c' variable
        switch(smCmdExec) {
        // Idle state, search for "Command" or "Get Tag Command"
        case SM_CMDX_IDLE:
            //Handle received character
            //This is a "Command Request"
            if (c == '%') {
                smCmdExec = SM_CMDX_GETTAG_GROUP;
            }
            //This is a "Command"
            else {
                smCmdExec = SM_CMDX_NAME;
                cmdFlags.bits.bDontReadNextChar = TRUE;    //Don't read another char, first handle current one!
            }
            break;

        //Command must be preceded by a username and password. They must be seperated by a ':'
        //character. After the password there must be a space character. For example:
        //"admin:pw c1=1" will set port c1, with username="admin" and password="pw"
        case SM_CMDX_UNPW:
            //The ' ' space character signals the end of the username and password
            if (c == ' ') {
                buf[put] = '\0';    //Terminate password with NULL

                if (strcmpee2ram((char*)&buf[0], APPCFG_USERNAME0) == 0) {
                    if (strcmpee2ram((char*)&buf[valueIdx], APPCFG_PASSWORD0) == 0) {
                        smCmdExec = SM_CMDX_IDLE;   //Look for commands
                    }
                }
                
                //Wrong Username and Password was entered!
                if (smCmdExec != SM_CMDX_IDLE) {
                    cmdFlags.bits.bCloseAndExit = TRUE;
                    break;
                }
                
                //Reset variables
                put = 0;
                valueIdx = 0;               //Indicate that value has not been found yet - is everything after '=' character
            }
            else {
                //The ':' character signals the end of the username
                if (c == ':') {
                    c = '\0';               //Terminate Username with NULL
                    valueIdx = put + 1;     //Remember position of password
                }

                //Is there still place in the buffer
                //Ensure with check that there is still one location avaiable AFTER this one - is required for NULL!
                if ( put < (sizeof(buf)-1) ) {
                    //Write all bytes to buf[], and sent it to execNameValueCmd() function to be processed
                    buf[put++] = c;
                }
            }
            break;

        //We are currently reading the name part of a "Command"
        case SM_CMDX_NAME:
            //The '=' sign signals the end of the name parameter
            if (c == '=') {
                c = '\0';               //End name with NULL character
                smCmdExec = SM_CMDX_VALUE;   //Read value next
                
                //The next position will be the first byte of the value, if any.
                //Check if buffer is large enough for first byte of value
                if ( (put+1) < sizeof(buf)) {
                    valueIdx = put + 1;     //Remember position of Value string
                    //Initialize first byte of value with NULL terminating string = no value.
                    //We do this in case there is no value to follow this '=' character
                    buf[put + 1] = '\0';
                }
            }
            //If space encountered, reset to look for next name-value pair
            //This condition will be true if the command has a username:password string, but it is not required!
            else if (c == ' ') {
                //Reset variable so that next command can be read
                valueIdx = 0;                       //Indicate that value has not been found yet - is everything after '=' character
                put = 0;
                break;
            }
            //No break, go through to value logic
            //break;
            
        //We are currently reading the value part of a "Command"
        case SM_CMDX_VALUE:
        
            //If:
            // - No more data, execute this command and exit
            // - The '&' character is used to seperate multiple commands
            if ((cmdFlags.bits.bCloseAndExit == TRUE) || (c == '&')) {
                smCmdExec = SM_CMDX_EXEC;
                c = '\0';                               //End String with NULL character
            }

            //Is there still place in the current buf[] buffer for this name-value pair.
            //Ensure with check that there is still one location avaiable AFTER this one - is required for NULL!
            if ( put < (sizeof(buf)-1) ) {
                //Write all bytes to buf[], and sent it to execNameValueCmd() function to be processed
                buf[put++] = c;
            }
            
            //Only break if command must not be executed, else continue to SM_CMDX_EXEC
            if (smCmdExec != SM_CMDX_EXEC)
                break;

        //We are currently executing a "Command"
        case SM_CMDX_EXEC:
            //Execute the given name-value command. User is ADMIN if state machine got this far
            wUpdate |= execNameValueCmd(buf, &buf[valueIdx], USER_ADMIN);

            //Reset variable so that next command can be read (if bCloseAndExit not set)
            smCmdExec = SM_CMDX_IDLE;
            valueIdx = 0;                       //Indicate that value has not been found yet - is everything after '=' character
            put = 0;
            break;

        //We are receiving the group part of a "Get Tag Command"
        case SM_CMDX_GETTAG_GROUP:
            getTagInfo.tagGroup = c;
            smCmdExec = SM_CMDX_GETTAG_VALMSB;
            break;

        //We are receiving the MSB part of the value of a "Get Tag Command". Value is hex coded
        case SM_CMDX_GETTAG_VALMSB:
            //Lowercase character
            if (c >= 'a')
                c -= 39;
            //Uppercase character
            else if (c >= 'A')
                c -= 7;
            getTagInfo.tagVal = (c - '0') << 4;

            smCmdExec = SM_CMDX_GETTAG_VALLSB;
            break;

        //We are receiving the LSB part of the value of a "Get Tag Command". Value is hex coded
        case SM_CMDX_GETTAG_VALLSB:
            //Lowercase character
            if (c >= 'a')
                c -= 39;
            //Uppercase character
            else if (c >= 'A')
                c -= 7;
            getTagInfo.tagVal |= (c - '0');

            //Initialize GETTAG_INFO structure
            getTagInfo.val  = &c;                   //Get returned value in c
            getTagInfo.user = USER_ADMIN;           //If we got here, we are ADMIN
            getTagInfo.ref  = HTTP_START_OF_VAR;    //Start of variable

            smCmdExec = SM_CMDX_GETTAG_EXEC;
            
            //No break, go straight to next case
            //break;

        //We are currently executing a "Get Tag Command"
        case SM_CMDX_GETTAG_EXEC:
            cmdFlags.bits.bDontReadNextChar = TRUE;    //Don't read another char, first handle current one!
            
            //Checks if there is a transmit buffer ready for accepting data, and that the
            //given socket is valid (not equal to INVALID_UDP_SOCKET for example)
            if (UDPIsPutReady(udpSocketCmd)) {

                while(1) {
                    bytesRead = cmdGetTag(&getTagInfo);
                    
                    //Add TAG data to UDP transmit buffer
                    if (bytesRead > 0) {
                        UDPPut(c);
                    }

                    if (getTagInfo.ref == HTTP_END_OF_VAR) {
                        // Now transmit it.
                        UDPFlush();
                        cmdFlags.bits.bCloseAndExit = TRUE;
                        break;
                    }
                }
            }
            //UDP not ready, abort
            else {
                smCmdExec = SM_CMDX_IDLE;
                cmdFlags.bits.bCloseAndExit = TRUE;
            }

            /*
            //Create UDP socket for sending data to remote client on port 54124.
            //We use the UDPGetNodeInfo() go get the IP and MAC address of the
            //remote client. Configure for local port 54130 and remote port 54124.
            udpSocketUserTx = UDPOpen((WORD)54130, UDPGetNodeInfo(), (WORD)54124);

            //An error occurred during the UDPOpen() function
            if (udpSocketUserTx == INVALID_UDP_SOCKET) {
                //Take any additional action that is required when an error occurs
            }
            */

            break;
        } // switch(smCmdExec)

    } while (smCmd != SM_CMD_IDLE);
    

    //Was any configuration data updated
    if (wUpdate != 0) {
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
}

/**
 * Execute the given name-value command. The command might not be executed if a higher user level is
 * required.
 *
 * @param name The name part of the name-value command. Has to be a null terminated string
 * @param value The value part of the name-value command. Has to be a null terminated string
 * @param user The user level, is a USER_XX constant
 *
 * @return Returns an CMD_UD_XXX flag (or multiple ones ORed together) indicating what has been updated. Not
 * all name-value commands will return a value here.
 */
WORD execNameValueCmd(BYTE * name, BYTE * value, BYTE user) {
    WORD wUpdate;   //Contains CMD_UD_XXX flags indicating what has to be updated
    BYTE tmp;
    BYTE i;

    #define SIZE_BUF 32 //Must be large enough to keep all data sent to LCD display
    BYTE buf[SIZE_BUF];

    wUpdate = 0;    //Clear all update flags


    /////////////////////////////////////////////////
    //Update Configuration value
    if ((name[0] == CMDGROUP_CONFIG) || (name[0] == CMDGROUP_BUSCONFIG)) {
        BYTE bTmp;
        WORD cfgOffset; //Offset of configuration byte
        BYTE cfgValue;  //New value of configuration byte
        BYTE mask;
        BYTE currValue;

        tmp = 1;
        cfgOffset = 0;
        
        //The name part of the name-value pair will be "knn" where nn is the offset (decimal) of the
        //configuration byte to be updated. Thus, name[1] will point to number repesenting the offset.
        //Get offset of configuration byte to be modified - is in hex format
        if ( !hextob( (char *)&name[1], &bTmp ) ) {
            #if (DEBUG_CMD >= LOG_WARN)
            debugPutMsg(1);     //@mxd:1:Invalid offset for configuration byte = %s
            debugPutByte(name[1]);
            debugPutByte(name[2]);
            debugPutByte(0);    //Null terminate string
            #endif

            return 0;   //Return 0 indicating nothing has been updated
        }
        
        //Bus Config addresses are 256 offset higher
        cfgOffset = (name[0]==CMDGROUP_BUSCONFIG) ? ((WORD)bTmp)+256 : bTmp;

        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(2);     //@mxd:2:Received Command to modify configuration byte 0x%x
        debugPutByteHex(cfgOffset);
        #endif

        /////////////////////////////////////////////////
        //Get the new value
        
        //If the command format is "kxxby", then it includes the bit to set/clear
        if (name[3] == 'b') {
            mask = 0x01 << (name[4] - '0');
            
            cfgValue = appcfgGetc(cfgOffset);
            if (value[0] == '0') {
                cfgValue &= (~mask);    //Clear given bit
            }
            else {
                cfgValue |= mask;       //Set given bit
            }
        }
        else {
            //Given value is in uppercase hex
            if (value[0] == 'H') {
                if ( !hextob( (char *)&value[1], &cfgValue ) ) {
                    return 0;   //Return 0 indicating nothing has been updated
                }
            }
            //Given value is a decimal value
            else {
                cfgValue = (BYTE)atoi((char *)value);
            }
            
            //If the command format is "kxxmyy", then it includes a mask
            if (name[3] == 'm') {
                //Get mask of configuration byte to be modified - is in hex format
                if ( !hextob( (char *)&name[4], &mask ) ) {
                    return 0;   //Return 0 indicating nothing has been updated
                }

                //Get current value, and clear bits given by mask
                currValue = appcfgGetc(cfgOffset) & (~mask);

                //Set masked bits with given value
                cfgValue = currValue | (cfgValue & mask);
            }
        }

        //Request a network setting to be changed
        if ((cfgOffset <= APPCFG_GATEWAY3) || (cfgOffset == APPCFG_NETFLAGS))
        {
            #if !defined(DEMO_MODE)
            wUpdate |= CMD_UD_NETWORK;  //Indicate the network settings must be updated
            tmp = 0;                    //Indicate that something must be updated
            #endif
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
            //New baud rate for USART1 or USART2
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
            if (appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) {
                //NOT allowed to modify port C if IOR5E Expansion Board is selected
                if (cfgOffset==APPCFG_TRISC) {
                    return 0;   //Return 0 indicating nothing has been updated
                }
                //NOT allowed to modify port B if IOR5E Expansion Board is selected
                if (cfgOffset==APPCFG_TRISB) {
                    return 0;   //Return 0 indicating nothing has been updated
                }
            }

            wUpdate |= CMD_UD_CPU_IO;   //Indicate the PORT settings must be updated
            tmp = 0;                    //Indicate that something must be updated
        }
        //Request a ADC setting to be changed
        else if (cfgOffset <= APPCFG_ADCON0_2) {
            wUpdate |= CMD_UD_ADC;   //Indicate the PORT settings must be updated
            tmp = 0;             //Indicate that something must be updated
        }
        //Request Serial Number, Username, Password, NetBIOS Name or DNS Server to be changed
        else if (cfgOffset <= APPCFG_DNS_IP3) {
            tmp = 0;                //Indicate that something must be updated
        }
        //Request UDP Command Configuration to be changed
        else if (cfgOffset <= APPCFG_CMD_FLAGS) {
            tmp = 0;             //Indicate that something must be updated
        }
        //Request PWM setting to be changed
        else if (cfgOffset <= APPCFG_PWM_5H) {
            wUpdate |= CMD_UD_PWM;  //Indicate the PWM settings must be updated
            tmp = 0;                //Indicate that something must be updated
        }
        //Request Web Server setting to be changed
        else if (cfgOffset <= APPCFG_HTTPSRVR_PORTH) {
            #if !defined(DEMO_MODE)
            wUpdate |= CMD_UD_WEB;  //Indicate the PWM settings must be updated
            tmp = 0;                //Indicate that something must be updated
            #endif
        }
        //Request I2C LCD setting to be changed
        else if (cfgOffset <= APPCFG_LCD1_RESERVED5) {
            wUpdate |= CMD_UD_I2C_LCD;  //Indicate the I2C LCD settings must be updated
            tmp = 0;                    //Indicate that something must be updated
        }
        //Request Event Configuration to be changed
        else if (cfgOffset <= APPCFG_EVENT_RESERVED8) {
            tmp = 0;             //Indicate that something must be updated
        }
        //Request Expansion Board setting to be changed
        else if (cfgOffset <= APPCFG_XBRD_RESERVED8) {
            //Check for valid board type value
            if (cfgOffset == APPCFG_XBRD_TYPE) {
                //Check for valid value
                if (cfgValue <= XBRD_TYPE_MAX) {
                    tmp = 0;            //Indicate that something must be updated
                }
            }
            //Else, update for all other cfgOffset's received - their values are not checked
            else {
                tmp = 0;
            }

            //Something must be updated
            if (tmp == 0) {
                wUpdate |= CMD_UD_XBOARD;  //Indicate the Expansion Board settings must be updated
            }
        }
        //Request BUS setting to be changed
        else if ((cfgOffset >= BUSCFG_SER1_TXBUFSIZE) &&  (cfgOffset <= BUSCFG_BUS8_RXBUFSIZE)) {
            wUpdate |= CMD_UD_BUSBUF;   //Indicate the Bus settings must be updated
            tmp = 0;                    //Indicate that something must be updated
        }
        //Request BUS Network setting to be changed
        else if (cfgOffset <= BUSCFG_UDP2_LINK) {
            wUpdate |= CMD_UD_BUSNET;   //Indicate the Bus Network settings must be updated
            tmp = 0;                    //Indicate that something must be updated
        }

        //Must anything be updated? Guest is NOT allowed to update these values!
        if ((tmp==0) && (user != USER_GUEST)) {
            appcfgPutc(cfgOffset, cfgValue);
        }
    }
    /////////////////////////////////////////////////
    //We received a port command
    //The name part of the name-value pair will start with "a" to "g" indicating the port.
    else if ((name[0] >= CMDGROUP_PORTA) && (name[0] <= CMDGROUP_PORTG))
    {
        BYTE c;
        BYTE* p;
        BYTE eeVal;
        BYTE bitPos;

        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(3);     //@mxd:3:Received Command to modify port pin
        #endif

        //NOT allowed to modify port D and E!
        if ((name[0] == CMDGROUP_PORTD) || (name[0] == CMDGROUP_PORTE)) {
            return 0;   //Return 0 indicating nothing has been updated
        }

        //NOT allowed to modify port C if IOR5E Expansion Board is selected
        if ((appcfgGetc(APPCFG_XBRD_TYPE) == XBRD_TYPE_IOR5E) && (name[0] == CMDGROUP_PORTC)) {
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
                bitPos = 0x01 << (name[2] - '0');

                //If value = '1', clear pin
                if (value[0] == '1')
                    *p &= (~bitPos);   //Clear port bit
                //If value = '0', set pin
                else if (value[0] == '0')
                    *p |= bitPos;      //Set port bit
            }
            //Configure port pin as input or output
            else if (name[1] == 'c') {
                //Third character of name will be bit position. For example ac3, bc6, fc0...
                bitPos = name[2] - '0';    //Get bit position in temp, a value from 0-7
                
                //Only continue if valid bit position between 0 and 7 was given
                if (bitPos <= 7) {
                    bitPos = 0x01 << bitPos;

                    //Get pointer to requested TRIS register
                    p = (BYTE *)(&TRISA) + (name[0] - CMDGROUP_PORTA);

                    //Get current value of configuration byte that configures port directions
                    eeVal = appcfgGetc(APPCFG_TRISA  + (name[0] - CMDGROUP_PORTA));

                    //If value = '1', set port pin as input
                    if (value[0] == '1') {
                        *p |= bitPos;      //Set TRIS bit
                        eeVal |= bitPos;   //Set bit in application configuration byte
                    }
                    //If value = '0', set port pin as output
                    else if (value[0] == '0') {
                        bitPos = ~bitPos;         //Invert all bits
                        *p &= bitPos;          //Clear TRIS bit
                        eeVal &= bitPos;       //Clear bit in application configuration byte
                    }
                    
                    //If remember, update application configuration data too!
                    if (name[3] == 'r') {
                        appcfgPutc(APPCFG_TRISA  + (name[0] - CMDGROUP_PORTA), eeVal);
                    }
                }
            }
            //Second byte is PIC Port pin
            else if ((name[1] >= '0') && (name[1] <= '9')) {
                //Second character of name will be bit position. For example a3, b6, f0...
                bitPos = name[1] - '0';    //Get bit position in temp, a value from 0-7

                //This is a Set PIC Port Pin command - PIC Port pin is followed by 'r' or nothing
                if ((name[2] == '\0') || (name[2] == 'r')) {
                    bitPos = 0x01 << bitPos;

                    //Get current value of configuration byte that configures default port values at power up
                    eeVal = appcfgGetc(APPCFG_PORTA  + (name[0] - CMDGROUP_PORTA));

                    //If value = '0', clear pin
                    if (value[0] == '0') {
                        bitPos = ~bitPos;     //Invert bits
                        *p &= bitPos;      //Clear port bit
                        eeVal &= bitPos;   //Clear port bit of configuration byte
                    }
                    //If value = '1', set pin
                    else if (value[0] == '1') {
                        *p |= bitPos;      //Set port bit
                        eeVal |= bitPos;   //Set port bit of configuration byte
                    }

                    //If remember, update application configuration data too!
                    if (name[2] == 'r') {
                        appcfgPutc(APPCFG_PORTA  + (name[0] - CMDGROUP_PORTA), eeVal);
                    }
                }
                //This is a Pulse PIC Port Pin Positive or Negative command
                else if ((name[2] == 'p') || (name[2] == 'n')) {
                    bitPos = 0x01 << bitPos;

                    //Get pulse width, is a value from 0-9
                    tmp = value[0] - '0';
                    if ((tmp < 0) || (tmp > 9)) {
                        tmp = 0;
                    }

                    //Send positive pulse
                    if (name[2] == 'p') {
                        *p |= bitPos;          //Set port bit
                        while (tmp--) {}
                        *p &= (~bitPos);       //Clear port bit
                    }
                    //If value = '1', set pin
                    else if (value[0] == '1') {
                        *p &= (~bitPos);       //Clear port bit
                        while (tmp--) {}
                        *p |= bitPos;          //Set port bit
                    }
                }
            }
        }
    }                   
    /////////////////////////////////////////////////
    //We received a general command with value
    else if (name[0] == CMDGROUP_GENERAL)
    {
        BYTE c;
        BYTE* p;
        BYTE sm;        //State machine

        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(8);     //@mxd:8:Received General Command
        #endif

        //The second character of the name part of the name-value pair will be the "Command Code".
        switch(name[1]) {
            case CMDCODE_GEN_NEW_PASSWORD:
                if (user != USER_GUEST) {
                    #if !defined(DEMO_MODE)

                    //Update password with new one just received. Can have a maximum lenght of 8
                    strcpyram2ee(APPCFG_PASSWORD0, (char *)value, 8);

                    #if (DEBUG_CMD >= LOG_INFO)
                    debugPutMsg(11);     //@mxd:11:Received new Password = %s
                    debugPutString(value);
                    #endif

                    #endif
                }
                break;
            case CMDCODE_GEN_NEW_USERNAME:
                if (user != USER_GUEST) {
                    #if !defined(DEMO_MODE)

                    //Update username with new one just received. Can have a maximum lenght of 8
                    strcpyram2ee(APPCFG_USERNAME0, (char *)value, 8);

                    #if (DEBUG_CMD >= LOG_INFO)
                    debugPutMsg(12);     //@mxd:12:Received new Username = %s
                    debugPutString(value);
                    #endif

                    #endif
                }
                break;
            case CMDCODE_GEN_NEW_NETBIOS_NAME:
                if (user != USER_GUEST) {
                    #if !defined(DEMO_MODE)
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
                    #endif
                }
                break;
            case CMDCODE_GEN_LCD_ARRAY:
            case CMDCODE_GEN_LCD_STRING:
            {
                BYTE lcdNum;
                p = (BYTE *)value;
                i = 0;
                sm = 0;    //State machine

                //Does the command include the number of the LCD display?
                lcdNum = 0;
                if ((name[2]>='0') && (name[2]<='9')) {
                    lcdNum = name[2]-'0';
                }
                
                //Parse given string. Because the LCD string can only contain a couple of character,
                //special characters must be parsed. An escape sequence starts with a '_' character,
                //and contains a two character hex value
                while (i < (SIZE_BUF-1) ) {
                    c = *p++;
                    
                    //End of string, send to LCD display
                    if (c == 0) {
                        break;
                    }

                    //Default state, search for escape sequence
                    if (sm == 0) {
                        //Escape sequence to follow
                        if (c == '_') {
                            sm++;       //Set to check escape sequence character
                        }
                        else {
                            //Parse LCD string
                            buf[i++] = c;
                        }
                    }
                    //First character of escape sequence - this is the first character following the _
                    else if (sm == 1) {
                        //This is not an escape sequence, but a _ character
                        if (c == '_') {
                            buf[i++] = c;
                            sm = 0;
                        }
                        //One character (string escape character) follows the _ escape sequence
                        else if (name[1] == CMDCODE_GEN_LCD_STRING) {
                            tmp=0;
                            switch(c) {
                                case 'n':
                                    tmp=0x0a;
                                    break;
                                case 'f':
                                    tmp=0x0c;
                                    break;
                                case 'r':
                                    tmp=0x0d;
                                    break;
                                case 'b':
                                    tmp=0x08;
                                    break;
                                case 't':
                                    tmp=0x09;
                                    break;
                            }
                            //Was a valid escape sequence found
                            if (tmp != 0)
                                buf[i++] = tmp;
                            sm = 0;
                        }
                        //Two characters (hex code) follows the _ escape sequence
                        else if (name[1] == CMDCODE_GEN_LCD_ARRAY) {
                            //Upper character of two character hex escape sequence
                            tmp = getHexNibble(c) << 4;
                            sm++;
                        }
                    }
                    //Second character of escape sequence - this is the second character following the _
                    else if (sm == 2) {
                        tmp |= getHexNibble(c);
                        buf[i++] = tmp;
                        sm = 0;
                    }
                }
                
                if (name[1] == CMDCODE_GEN_LCD_STRING) {
                    buf[i] = 0; //Terminate with NULL character
                    lcdPutString(lcdNum, buf);
                }
                else {
                    lcdPutArray(lcdNum, buf, i);
                }
                break;
            }
        }
    }
    /////////////////////////////////////////////////
    //We received a general command without value
    else if (name[0] == CMDGROUP_GENERAL_NOVAL)
    {
        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(10);     //@mxd:10:Received General Command without value
        #endif

        /////////////////////////////////////////////////
        // The following commands can ONLY be executed by "Admin" or "Super User"
        if (user != USER_GUEST) {
            //The first character of the value part of the name-value pair will be the "Command Code".
            switch(value[0]) {
                case CMDCODE_GENNOVAL_RESET:
                #if !defined(DEMO_MODE)
                    #if (DEBUG_CMD >= LOG_INFO)
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
                #endif
                    break;
            }
        }
    }
    /////////////////////////////////////////////////
    //We received a PWM command
    //The name part of the name-value pair will start with "w"
    else if (name[0] == CMDGROUP_PWM)
    {
        WORD wValue;
        WORD_VAL wv;

        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(5);     //@mxd:5:Received PWM command
        #endif
        
        //Change PWM frequency = "wf(r)" cmd
        if (name[1] == 'f') {
            tmp = value[0] - '0';    //Value should be between 0 and 3
            pwmSetFreq(tmp, name[2]=='r');
        }
        //Enable/Disable PWM Channels = "we(r)" cmd
        else if (name[1] == 'e') {
            //The value part will give channels to enable
            if (hextob( (char *)value, &tmp ) ) {
                //Enable given channels
                pwmEnable(tmp, name[2]=='r');
            }
        }
        //Set PWM Channels = "wn(r)" cmd, where n is a number from 1 to 5
        else if ((name[1] >= '1') && (name[1] <= '5'))  {
            wv.Val = atoi((char *)value);
            pwmSetValue(name[1]-'0', wv, name[2]=='r');
        }
        //Set PWM Mode = "wm(r)" cmd, a value of '8' set to 8 bit mode, and 'a' to 10 bit mode
        else if (name[1] == 'm')  {
            i = 1;
            tmp = appcfgGetc(APPCFG_PWM_FLAGS) & 0xfe; //Get current flags value, and clear mode flag

            //8 bit mode
            if (value[0] == '8') {
                //Continue if NOT already in 8-bit mode
                if (pwmIs8BitMode() == FALSE) {
                    i = 0;      //Indicate that the bit mode was changed
                    PR2 = 63;   //Configure for 8-bit mode
                }
            }
            //10 bit mode
            else if (value[0] == 'a') {
                //Continue if NOT already in 10-bit mode
                if (pwmIs10BitMode() == FALSE) {
                    i = 0;          //Indicate that the bit mode was changed
                    PR2 = 255;      //Configure for 10-bit mode
                }
                tmp |= 0x01;    //10-bit mode
            }

            //If the bit mode changed
            if (i == 0) {
                //The pwmSetFreq function configures PWM for 8 or 10 bit mode
                pwmSetFreq(appcfgGetc(APPCFG_PWM_FREQ), FALSE);
            }

            //Remember settings
            if (name[2]=='r') {
                appcfgPutc(APPCFG_PWM_FLAGS, tmp);
            }
        }
    }
    /////////////////////////////////////////////////
    //We received a Expansion Board command
    //The name part of the name-value pair will start with "x"
    else if (name[0] == CMDGROUP_XBRD)
    {
        BOOL bVal;         //value
        BYTE ch;
        WORD wValue;
        WORD_VAL wv;

        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(6);     //@mxd:5:Received Expansion Board command
        #endif
        
        bVal = (value[0]=='0') ? FALSE : TRUE;
        
        //Relay command, like "xr2=1" to set relay 2 for example
        if (name[1] == 'r') {
            ch = name[2] - '0'; //Get relay number

            //Get current type of Expansion Board
            tmp = appcfgGetc(APPCFG_XBRD_TYPE);

            //MXD2R has relay 1 and 2
            if (tmp==XBRD_TYPE_MXD2R) {
                mxd2rSetRly(ch-1, bVal);
            }
            //IOR5E has relays 1 to 5
            else if ((tmp==XBRD_TYPE_IOR5E) && (ch>=1) && (ch<=5) ) {
                ior5eSetRly(ch-1, bVal);
            }
        }
        //Output command, like "xo5=0" to clear digital output 5 for example
        else if (name[1] == 'o') {
            ch = name[2] - '0'; //Get relay number

            //Get current type of Expansion Board
            tmp = appcfgGetc(APPCFG_XBRD_TYPE);

            //MXD2R has outputs 1 to 6
            if ((tmp==XBRD_TYPE_MXD2R) && (ch>=1) && (ch<=6) ) {
                mxd2rSetIO(ch-1, bVal);
            }
            //IOR5E has outputs 1 to 6
            else if ((tmp==XBRD_TYPE_IOR5E) && (ch>=1) && (ch<=6) ) {
                ior5eSetIO(ch-1, bVal);
            }
        }
    }

    //Return wUpdate - it will indicate what has changed
    return wUpdate;
}


/**
 * Get the requested tag. A tag is always an ASCII string.
 * A tag is identified by it's "Variable Group" and "Variable Value".
 * The command might not be executed if a higher user level is required.
 *
 * @param pGetTagInfo Pointer to GETTAG_INFO structure.
 *
 * @return Returns number of bytes written to pGetTagInfo->buf. If this value is less then the size of
 *         the buffer given, then all requested data has been returned.
 *         The pGetTagInfo.ref is also updated with the new ref value.
 */
WORD cmdGetTag(GETTAG_INFO* pGetTagInfo)
{
    BYTE flags;
    BYTE tagVal;
    BYTE tagGroup;
    BYTE mask;
    BYTE tmp;
    WORD ref;

    #define FLAGS_OUTPUT_PIN    0x01
    #define FLAGS_ADC_PIN       0x02
    #define FLAGS_PWM_ENABLED   0x08

    tagVal = pGetTagInfo->tagVal;     //Tag Value of requested variable
    tagGroup = pGetTagInfo->tagGroup;   //Tag Group of requested variable
    ref      = pGetTagInfo->ref;        //Current callback reference with respect to 'var' variable.

    //In case requested val not found, set it to NULL character
    *pGetTagInfo->val = '\0';


    #if (DEBUG_CMD >= LOG_INFO)
    if (ref == HTTP_START_OF_VAR) {
        debugPutMsg(16);     //@mxd:16:Received Get Tag Command: %s
        debugPutByte(tagGroup);
        debugPutByteHex(tagVal);
        debugPutByte(0);    //Null terminate string
    }
    #endif

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
    if ((tagGroup >= VARGROUP_PORTA) && (tagGroup <= VARGROUP_PORTH))
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

        BYTE* p;
        
        flags = 0;

        //Get register offset. PORTA = a, PORTB = b, PORTC = c......
        tmp = (tagGroup - 'a');

        //Get bit mask
        mask = 0x01 << (tagVal & 07);

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

        tmp = tagVal & 0xf0;

        //Return "0" or "1"
        if (tmp == 0x00)
        {
            //Set val to '0' or '1' depending on state of pin
            *pGetTagInfo->val = ((*p & mask) == 0) ? '0' : '1';
            pGetTagInfo->ref = HTTP_END_OF_VAR;
            return 1;   //One byte was written
        }
        //Return "on" or "off"
        else if (tmp == 0x10)
        {
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val,
                    ((*p & mask) == 0) ? HTMLSTR_OFF : HTMLSTR_ON);
            return 1;   //One byte was written
        }
        //Return "<!--" or "-->" depending on if port is configured as input or output
        else if ( ((tmp == 0x20) && ((flags & FLAGS_OUTPUT_PIN) == 0)) ||
                  ((tmp == 0x30) && ((flags & FLAGS_OUTPUT_PIN) != 0)) )
        {
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val,
                    ((tagVal & 0x08) == 0) ? HTMLSTR_CMNT_START : HTMLSTR_CMNT_END);
            return 1;   //One byte was written
        }
        //Return "checked" if port configured as output or input
        else if (tmp == 0x40)
        {
            if (tagVal & 0x08) {
                //Return "checked" if port configured as input
                if ( (flags & FLAGS_OUTPUT_PIN) == 0 ) {
                    pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, HTMLSTR_CHECKED);
                    return 1;   //One byte was written
                }
            }
            else {
                //Return "checked" if port configured as output
                if ( (flags & FLAGS_OUTPUT_PIN) != 0 ) {
                    pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, HTMLSTR_CHECKED);
                    return 1;   //One byte was written
                }
            }
        }
        //Return "1" if port configured as input, else 0
        else if (tmp == 0x50)
        {
            //Set val to '0' or '1' depending on pin direction
            *pGetTagInfo->val = (flags & FLAGS_OUTPUT_PIN) ? '0' : '1';
            pGetTagInfo->ref = HTTP_END_OF_VAR;
            return 1;   //One byte was written
        }
    }
    /////////////////////////////////////////////////
    //'k' and 'u' Configuration variables
    else if ((tagGroup == VARGROUP_CONFIG) || (tagGroup == VARGROUP_BUSCFG))
    {
        //Guest is NOT allowed this information!
        if (pGetTagInfo->user != USER_GUEST) {
            //Return decimal value of requested configuration byte
            if (tagGroup == VARGROUP_CONFIG) {
                pGetTagInfo->ref = cmdGetByteVar(ref, pGetTagInfo->val, appcfgGetc(tagVal));
            }
            else {
                pGetTagInfo->ref = cmdGetByteVar(ref, pGetTagInfo->val, appcfgGetc((WORD)tagVal+256));
            }
            return 1;   //One byte was written
        }
        else {
            //Else return 0 - actually the string "00" is returned
            pGetTagInfo->ref = cmdGetByteVar(ref, pGetTagInfo->val, 0);
            return 1;   //One byte was written
        }
    }
    /////////////////////////////////////////////////
    //'l' General variables
    else if (tagGroup == VARGROUP_GENERAL)
    {
        //Version of current Modtronix TCP/IP stack. Format is "Vm.nn", for example V2.04.
        // - m can be 1 or 2 digets long, nn is always 2 digets.
        if (tagVal == VARVAL_GEN_STACK_VER_STR) {
            //Return string representing stack version
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, STACK_VER_STR);
            return 1;   //One byte was written
        }
        //Version of current Application. Format is "Vm.nn", for example V2.04.
        // - m can be 1 or 2 digets long, nn is always 2 digets.
        else if (tagVal == VARVAL_GEN_APP_VER_STR) {
            //Return string representing stack version
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, APP_VER_STR);
            return 1;   //One byte was written
        }
        //Display 'Yes' if board has a bootloader, else 'No'.
        else if (tagVal == VARVAL_GEN_HAS_BL) {
            #ifdef HAS_BOOTLOADER
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, (ROM char*)"Yes");
            #else
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, (ROM char*)"No");
            #endif
            return 1;   //One byte was written
        }
        //Usename of current user logged in
        else if (tagVal == VARVAL_GEN_USERNAME) {
            //Return Username
            if (pGetTagInfo->user != USER_GUEST) {
                pGetTagInfo->ref = cmdGetEepromStringVar(ref, pGetTagInfo->val, APPCFG_USERNAME0);
            }
            //Guest is NOT allowed this information - return "Guest"
            else {
                pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, (ROM char*)"Guest");
            }
            return 1;   //One byte was written
        }
        //Returns '1' if Authentication has been provided, else '0'
        else if (tagVal == VARVAL_GEN_LOGGED_IN) {
            *pGetTagInfo->val = (pGetTagInfo->user == USER_GUEST) ? '0' : '1';
            pGetTagInfo->ref = HTTP_END_OF_VAR;
            return 1;   //One bytes was written
        }
        //Returns '1' if in Demo Mode, else '0'
        else if (tagVal == VARVAL_GEN_DEMO_MODE) {
            #if defined(DEMO_MODE)
            *pGetTagInfo->val = '1';
            #else
            *pGetTagInfo->val = '0';
            #endif
            pGetTagInfo->ref = HTTP_END_OF_VAR;
            return 1;   //One byte was written
        }
        //Returns NetBIOS name
        else if (tagVal == VARVAL_GEN_NETBIOS_NAME) {
            pGetTagInfo->ref = cmdGetEepromStringVar(ref, pGetTagInfo->val, APPCFG_NETBIOS0);
            return 1;   //One byte was written
        }
        else if ( (tagVal >= VARVAL_GEN_IP0) && (tagVal <= VARVAL_GEN_GATEWAY3) ) {
            //Return decimal value of requested AppConfig byte
            pGetTagInfo->ref = cmdGetByteVar(ref, pGetTagInfo->val, ((BYTE*)&AppConfig)[tagVal-VARVAL_GEN_IP0] );
            return 1;   //One byte was written
        }
    }
    /////////////////////////////////////////////////
    //ADC variables
    else if (tagGroup == VARGROUP_ANALOG)
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
        if ((tagVal & 0x0f) < (~ADCON1 & 0x0f))
        {
            flags |= FLAGS_ADC_PIN;
        }

        tmp = tagVal & 0xf0;

        //Return 3 digit uppercase hex value, representing requested Analog Input Channel
        if (tmp == 0x00)
        {
            //Only display least significant 3 hex characters. Skip first character
            if (ref == HTTP_START_OF_VAR) {
                ref++;  //Increment to next character
            }
            
            pGetTagInfo->ref = cmdGetWordHexVar(ref, pGetTagInfo->val, (WORD)AdcValues[tagVal & 0x0f]);
            
            return 1;   //One byte was written
        }
        //Return decimal value 0 - 1023
        else if (tmp == 0x10)
        {
            #if defined(APP_USE_ADC8)
            pGetTagInfo->ref = cmdGetByteVar(ref, pGetTagInfo->val, AdcValues[tagVal & 0x0f]);
            #elif defined(APP_USE_ADC10)
            pGetTagInfo->ref = cmdGetWordVar(ref, pGetTagInfo->val, AdcValues[tagVal & 0x0f]);
            #endif
            
            return 1;   //One byte was written
        }
        //Return decimal value (2 decimal point precicion) for if 5V reference was used
        else if (tmp == 0x20)
        {
            #if defined(APP_USE_ADC8)
            tmp = AdcValues[tagVal & 0x0f];

            if ( ref == HTTP_START_OF_VAR )
            {
                //Convert tmp value to voltage (2 decimal points) value for 5V reference
                strTmp[0] = (BYTE)(tmp / 51) + '0';
                strTmp[1] = '.';
                tmp = (tmp % 51);   //Get rest
                strTmp[2] = (BYTE)(( ((WORD)tmp) * (WORD)10) / (WORD)51) + '0';
                tmp = (tmp / 10);
                strTmp[3] = (BYTE)( ( ((WORD)tmp) * (WORD)100) / (WORD)51) + '0';
                strTmp[4] = '\0';  //Terminate with 0
            }
            *pGetTagInfo->val = strTmp[(BYTE)ref];
            if ( strTmp[(BYTE)ref] == '\0' ) {
                pGetTagInfo->ref = HTTP_END_OF_VAR;
                return 1;   //One byte was written
            }
            pGetTagInfo->ref++;
            return 1;   //One byte was written
            #elif defined(APP_USE_ADC10)
            WORD tmpW;
            tmpW = AdcValues[tagVal & 0x0f];
            if (ref == HTTP_START_OF_VAR )
            {
                //Convert tmp value to voltage (2 decimal points) value for 5V reference
                strTmp[0] = (BYTE)(tmpW / (1023/5)) + '0';
                strTmp[1] = '.';
                tmpW = (tmpW % (1023/5));   //Get rest
                strTmp[2] = (BYTE)(( ((WORD)tmpW) * (WORD)10) / (WORD)(1023/5)) + '0';
                tmpW = (tmpW / 10);
                strTmp[3] = (BYTE)( ( ((WORD)tmpW) * (WORD)100) / (WORD)(1023/5)) + '0';
                strTmp[4] = '\0';  //Terminate with 0
            }
            *pGetTagInfo->val = strTmp[(BYTE)ref];
            if ( strTmp[(BYTE)ref] == '\0' ) {
                pGetTagInfo->ref = HTTP_END_OF_VAR;
                return 1;   //One byte was written
            }
            pGetTagInfo->ref++;
            return 1;   //One byte was written
            #endif
        }
        //Return "<!--"
        else if ( ((tmp == 0x30) && ((flags & FLAGS_ADC_PIN) != 0)) ||
                  ((tmp == 0x50) && ((flags & FLAGS_ADC_PIN) == 0)) )
        {
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, HTMLSTR_CMNT_START);
            return 1;   //One byte was written
        }
        //Return "<!--"
        else if ( ((tmp == 0x40) && ((flags & FLAGS_ADC_PIN) != 0)) ||
                  ((tmp == 0x60) && ((flags & FLAGS_ADC_PIN) == 0)) )
        {
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, HTMLSTR_CMNT_END);
            return 1;   //One byte was written
        }
    }
    /////////////////////////////////////////////////
    //PWM variables
    else if (tagGroup == VARGROUP_PWM)
    {
        BYTE ccpxcon;       //CCPxCON register
        BYTE varVal0to3;    //Variable Value bit 0-3
        flags = 0;

        varVal0to3 = (tagVal & 0x0f); //Get PWM channel

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

        tmp = tagVal & 0xf0;  //Get upper nibble of variable value

        //This is a request for the mode, frequency or selected PWM channels
        if (tmp == 0xf0) {
            //Request frequency value - return a constant from 0-3 indicating current frequency
            if (varVal0to3 == 0) {
                *pGetTagInfo->val = '0' + pwmGetFreq();
                pGetTagInfo->ref = HTTP_END_OF_VAR;
                return 1;   //One byte was written
            }
            //Request mode - return '8' for 8-bit mode, or 'a' for 10-bit mode
            else if (varVal0to3 == 4) {
                if (pwmIs10BitMode() == TRUE) {
                    *pGetTagInfo->val = 'a';
                }
                else {
                    *pGetTagInfo->val = '8';
                }
                pGetTagInfo->ref = HTTP_END_OF_VAR;
                return 1;   //One byte was written
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
                pGetTagInfo->ref = cmdGetByteHexVar(ref, pGetTagInfo->val, tmp);
                return 1;   //One byte was written
            }
        }
        //A PWM Channel's value has been requested. The value of the requested variable will result
        //in the following being returned:
        // 30 to 38 - "<!--" returned if PWM channel is enabled
        // 40 to 48 - "-->" returned if PWM channel is enabled
        // 50 to 58 - "<!--" returned if PWM channel is disabled
        // 60 to 68 - "-->" returned if PWM channel is disabled

        // 00 to 08 variable value = PWM Channel's value as a 3 digit uppercase hex value
        else if (tmp == 0x00)
        {
            //Only return least significant 3 hex characters. Skip first character
            if (ref == HTTP_START_OF_VAR) {
                ref++;  //Increment to next character
            }
            
            pGetTagInfo->ref = cmdGetWordHexVar(ref, pGetTagInfo->val, pwmGetValue(varVal0to3));
            
            return 1;   //One byte was written
        }
        // 00 to 08 variable value = PWM Channel's value as a decimal value 0 - 255 or 0 - 1023
        else if (tmp == 0x10)
        {
            pGetTagInfo->ref = cmdGetWordVar(ref, pGetTagInfo->val, pwmGetValue(varVal0to3));
            return 1;   //One byte was written
        }
        // 30 to 38 variable value = "<!--" returned if the PWM channel is enabled
        // 50 to 58 variable value = "<!--" returned if the PWM channel is disabled
        else if ( ((tmp == 0x30) && ((flags & FLAGS_PWM_ENABLED) != 0)) ||
                  ((tmp == 0x50) && ((flags & FLAGS_PWM_ENABLED) == 0)) )
        {
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, HTMLSTR_CMNT_START);
            return 1;   //One byte was written
        }
        // 40 to 48 variable value = "-->" returned if the PWM channel is enable
        // 60 to 68 variable value = "-->" returned if the PWM channel is disabled
        else if ( ((tmp == 0x40) && ((flags & FLAGS_PWM_ENABLED) != 0)) ||
                  ((tmp == 0x60) && ((flags & FLAGS_PWM_ENABLED) == 0)) )
        {
            pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val, HTMLSTR_CMNT_END);
            return 1;   //One byte was written
        }
    }
    /////////////////////////////////////////////////
    //'s' Secure Tags
    else if (tagGroup == VARGROUP_SECURE)
    {
        //Return 0 is not Authenticated
        if (pGetTagInfo->user == USER_GUEST) {
            *pGetTagInfo->val = '0';
            pGetTagInfo->ref = HTTP_END_OF_VAR;
            return 1;   //One byte was written
        }

        //Username of current user logged in
        if (tagVal == VARVAL_SEC_USERNAME) {
            pGetTagInfo->ref = cmdGetEepromStringVar(ref, pGetTagInfo->val, APPCFG_USERNAME0);
            return 1;   //One byte was written
        }
        //Password of current user logged in
        else if (tagVal == VARVAL_SEC_PASSWORD) {
            pGetTagInfo->ref = cmdGetEepromStringVar(ref, pGetTagInfo->val, APPCFG_PASSWORD0);
            return 1;   //One byte was written
        }
    }
    /////////////////////////////////////////////////
    //Expansion Board variables
    else if (tagGroup == VARGROUP_XBRD)
    {
        BYTE varVal0to3;    //Variable Value bit 0-3
        BYTE varVal4to7;
        BYTE xbrdType;
        BOOL bVal;
        BYTE ch;
        BOOL bIsValid;
        
        bVal = 0;
        bIsValid = 1;
        varVal0to3 = tagVal & 0x0f;     //Get lower 4 bits, is normally the channel
        ch = tagVal & 0x0f;     //Get lower 4 bits, is normally the channel
        varVal4to7 = tagVal & 0xf0;     //Get upper nibble of variable value

        //Get current type of Expansion Board
        xbrdType = appcfgGetc(APPCFG_XBRD_TYPE);

        // 0x00 to 0x08 variable value = return '1' or '0' if relay is on or off
        // 0x10 to 0x18 variable value = return "on" or "off" if relay is on or off
        if ( (varVal4to7==0x00) || (varVal4to7==0x10) )
        {
            /////////////////////////////////////////////////
            //Get state of requested relay

            //MXD2R has relay 1 and 2
            if (xbrdType==XBRD_TYPE_MXD2R) {
                bVal = ((mxd2rGetRly() & (0x01<<ch)) == 0) ? FALSE : TRUE;
            }
            //IOR5E has relays 1 to 5
            else if (xbrdType==XBRD_TYPE_IOR5E) {
                bVal = ((ior5eGetRly() & (0x01<<ch)) == 0) ? FALSE : TRUE;
            }
            else {
                bIsValid = 0;
            }

            if (bIsValid) {
                if (varVal4to7==0x00) {
                    //Set val to '0' or '1' depending on state of relay
                    *pGetTagInfo->val = (bVal ? '1' : '0');
                    pGetTagInfo->ref = HTTP_END_OF_VAR;
                }
                else {
                    //Set val to "on" or "off" depending on state of relay
                    pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val,
                        (bVal ? HTMLSTR_ON : HTMLSTR_OFF));
                }
                return 1;   //One byte was written
            }
        }
        // 0x20 to 0x28 variable value = return '1' or '0' if opto input is on or off
        // 0x30 to 0x38 variable value = return "on" or "off" if opto input is on or off
        else if ( (varVal4to7==0x20) || (varVal4to7==0x30) )
        {
            /////////////////////////////////////////////////
            //Get state of requested opto input

            //MXD2R has no opto inputs
            //IOR5E has 4 opto inputs
            if (xbrdType==XBRD_TYPE_IOR5E) {
                bVal = ((ior5eGetOpto() & (0x01<<ch)) == 0) ? FALSE : TRUE;
            }
            else {
                bIsValid = 0;
            }

            if (bIsValid) {
                if (varVal4to7==0x20) {
                    //Set val to '0' or '1' depending on state of relay
                    *pGetTagInfo->val = (bVal ? '1' : '0');
                    pGetTagInfo->ref = HTTP_END_OF_VAR;
                }
                else {
                    //Set val to "on" or "off" depending on state of relay
                    pGetTagInfo->ref = cmdGetROMStringVar(ref, pGetTagInfo->val,
                        (bVal ? HTMLSTR_ON : HTMLSTR_OFF));
                }
                return 1;   //One byte was written
            }
        }
    }

    pGetTagInfo->ref = HTTP_END_OF_VAR;   
    return 0;   //No bytes was written
}



/**
 * Returns a string representation of a given ROM string
 *
 * @param ref   Current callback reference used by HTTPGetVar
 * @param val   Buffer for value storage.
 * @param str   Pointer to string to display
 *
 * @return      HTTPGetVar return value. Is the offset in the given string.
 */
static WORD cmdGetROMStringVar(BYTE ref, BYTE* val, ROM char* str)
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
static WORD cmdGetEepromStringVar(BYTE ref, BYTE* val, WORD adr)
{
    WORD_VAL wAdr;
    wAdr.Val = ref + adr;
    EEADRH = wAdr.v[1];
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
static WORD cmdGetStringVar(BYTE ref, BYTE* val, BYTE* str)
{
    *val = str[(BYTE)ref];
    if (*val == '\0' )
        return HTTP_END_OF_VAR;

    (BYTE)ref++;
    return ref;
}
*/


/**
 * Returns uppercase, two character, hex string representation of given byte.
 *
 * @param ref   Current callback reference, is offset of next character to be returned
 * @param val   Buffer for value storage.
 * @param b     Byte value to display
 *
 * @return      Callback reference - is the offset of the next byte to be read. Or, HTTP_END_OF_VAR is last
 *              byte is returned.
 */
static WORD cmdGetByteHexVar(BYTE ref, BYTE* val, BYTE b)
{
    //Illegal value, return HTTP_END_OF_VAR
    if (ref > 1) {
        return HTTP_END_OF_VAR;
    }

    //Upper nibble for first byte (left most character), lower nibble for second (right most character)
    if (ref == 0)
    {
        b >>= 4;
    }

    b &= 0x0f;
    *val = (b>9) ? (b+('A'-10)) : (b+'0');

    //This is the last byte
    if (ref == 1) {
        return HTTP_END_OF_VAR;
    }
    
    return ref+1;
}


/**
 * Returns uppercase, four character, hex string representation of given byte. It is NOT
 * a NULL terminated string.
 *
 * @param ref   Current callback reference, is offset of next character to be returned
 * @param val   Buffer for value storage.
 * @param b     Byte value to display
 *
 * @return      Callback reference - is the offset of the next byte to be read. Or, HTTP_END_OF_VAR is last
 *              byte is returned.
 */
static WORD cmdGetWordHexVar(BYTE ref, BYTE* val, WORD w)
{
    BYTE b;
    
    //Illegal value, return HTTP_END_OF_VAR
    if (ref > 3) {
        return HTTP_END_OF_VAR;
    }

    //Upper nibble for first byte (left most character), lower nibble for second (right most character)
    b = w >> ((3-ref)*4);

    b &= 0x0f;
    *val = (b>9) ? (b+('A'-10)) : (b+'0');

    //This is the last byte
    if (ref == 3) {
        return HTTP_END_OF_VAR;
    }

    return ref+1;
}


 /**
 * Returns decimal string representation of given word.
 *
 * @param ref   Current callback reference, is offset of next character to be returned
 * @param val   Buffer for value storage.
 * @param b     Word value to display
 *
 * @return      Callback reference - is the offset of the next byte to be read. Or, HTTP_END_OF_VAR if last
 *              byte (NULL string terminator) is returned.
 */
static WORD cmdGetWordVar(BYTE ref, BYTE* val, WORD w)
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

