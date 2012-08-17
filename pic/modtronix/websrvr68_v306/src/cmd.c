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

#include "net\udp.h"
#include "net\helpers.h"
#include "net\security.h"   //Security related code


/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xDA, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xDA, msgCode, msgStr)

//Create a UDP socket for the "UDP command port"
static UDP_SOCKET udpSocketCmd = INVALID_UDP_SOCKET;
//Create a UDP socket for the "UDP command request port"
static UDP_SOCKET udpSocketCmdRqst = INVALID_UDP_SOCKET;

/**
 * Cmd Command Structure
 */
typedef struct _CMD
{
    struct
    {
        unsigned char bFinished : 1;    //If set, getVarCmd() returned all requested data
    } flags;
} CMD;

//variable for "Command State Machine"
static BYTE smCmd;

/////////////////////////////////////////////////
// "Command State Machine"
typedef enum _SM_CMD
{
    SM_CMD_IDLE,        /* Idle state */
    SM_CMD_UDP,         /* Currently receiving a UDP command */
    SM_CMD_TCP          /* Currently receiving a TCP command */
} SM_CMD;



/**
 * Initializes "UDP Command Port" and "UDP Command Responce Port". These
 * ports are used for sending and receiving and commands via the UDP port.
 *
 */
void cmdUdpInit(void) {
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

/////////////////////////////////////////////////
// "Command Execution State Machine"
typedef enum _SM_CMDX
{
    SM_CMDX_IDLE,        /* Idle state, search for "UDP Command" or "UDP Request Command" */
    SM_CMDX_UNPW,        /* Search for Username and Password */
    SM_CMDX_NAME,        /* We are currently reading the name part of a "UDP Command" */
    SM_CMDX_VALUE,       /* We are currently reading the value part of a "UDP Command" */
    SM_CMDX_EXEC,        /* We are currently executing a "UDP Command" */
    SM_CMDX_RQST_GROUP,  /* We are receiving the group part of a "UDP Request Command" */
    SM_CMDX_RQST_VALUE,  /* We are receiving the value part of a "UDP Request Command" */
    SM_CMDX_RQST_EXEC    /* We are currently executing a "UDP Request Command" */
} SM_CMDX;

/**
 * Must be called every couple of ms.
 * Each UDP message can consists out of any number of "Commands" and "Requests", seperated by '&'
 * characters. Each "UDP Request Command" starts with a % character.
 *
 */
void cmdProcess(void) {
    union
    {
        struct
        {
            unsigned int bDontReadNextChar : 1; //Don't read next character, we are still processing current one!
            unsigned int bNoMoreData : 1;       //There is no more data!
        } bits;
        BYTE val;
    } flags;

    //Variables
    BYTE c;

    /////////////////////////////////////////////////
    //Variables for Command
    BYTE put;
    BYTE buf[64];
    BYTE smCmdExec; //Current State of "Command Execution State Machine"
    WORD wUpdate;   //Contains CMD_UD_XXX flags indicating what has to be updated
    //Used as input AND output parameter for HTTPGetParams.
    // - Input parameter indicates size of param buffer
    // - On return of HTTPGerParam() valueIdx will contain index of value string in buf[]
    BYTE valueIdx;      //Index of value in buf[]
    BYTE user;
    
    user = USER_GUEST;  //Initialize with guest status

    do {
        /////////////////////////////////////////////////
        //Get next command byte, if any
        
        // Idle state - check if a command is reaceived via UDP, TCP, Serial.....
        if (smCmd == SM_CMD_IDLE) {
            //Is there any data waiting for us on the "UDP command port"?
            //Because of the design of the Modtronix TCP/IP stack we have to consume all data sent to us as soon
            //as we detect it. Store all data to a buffer as soon as it is detected
            if (UDPIsGetReady(udpSocketCmd)) {

                //UDP Commands are disabled, discard UDP message!
                if ( (appcfgGetc(APPCFG_CMD_FLAGS) & APPCFG_CMDFLAGS_ENABLE) == 0) {
                    UDPDiscard();
                }
                //Process UDP command
                else {
                    smCmd = SM_CMD_UDP;

                    //UDP Commands require username and password!
                    if (appcfgGetc(APPCFG_CMD_FLAGS) & APPCFG_CMDFLAGS_AUTH_UDP) {
                        //UDP command must be preceded by username and password
                        smCmdExec = SM_CMDX_UNPW;
                    }
                    else {
                        user = USER_ADMIN;          //Dont require username and password - set to admin
                        smCmdExec = SM_CMDX_IDLE;   //Look for commands
                    }

                    flags.val = 0;  //Clear all flags
                    put = 0;
                    valueIdx = 0;                   //Indicate that value has not been found yet - is everything after '=' character
                    buf[sizeof(buf) - 1] = '\0';    //Terminate last byte of buffer with NULL character
                
                    //Go to top of do{} statement
                    continue;
                }
            }
        }
        //Currently receiving a UDP command
        else if (smCmd == SM_CMD_UDP) {
            //Read next UDP byte
            if (flags.bits.bDontReadNextChar == FALSE) {
                //No more data, exit
                if (flags.bits.bNoMoreData) {
                    smCmd = SM_CMD_IDLE;
                }
                else {
                    flags.val = 0;  //Clear all flags

                    //Get next character
                    if (UDPGet(&c) == 0) {
                        flags.bits.bNoMoreData = TRUE;
                        UDPDiscard();
                    }
                    else {
                        flags.bits.bNoMoreData = FALSE;
                    }
                }
            }
            //Don't read next UDP byte
            else {
                flags.bits.bDontReadNextChar = FALSE;   //Reset, next time a char will be read
            }
        }
        //Currently receiving a TCP command
        else if (smCmd == SM_CMD_TCP) {

        }


        /////////////////////////////////////////////////
        //Process next command byte
        //At this stage, the next character of the command will be in the 'c' variable
        if (smCmd != SM_CMD_IDLE) {
            //Command Execution State Machine
            switch(smCmdExec) {
            // Idle state, search for "Command" or "Request Command"
            case SM_CMDX_IDLE:
                //Handle received character
                if (flags.bits.bNoMoreData == FALSE) {
                    //This is a "Command Request"
                    if (c == '%') {
                        smCmdExec = SM_CMDX_RQST_GROUP;
                    }
                    //This is a "Command"
                    else {
                        smCmdExec = SM_CMDX_NAME;
                        flags.bits.bDontReadNextChar = TRUE;    //Don't read another char, first handle current one!
                    }
                }
                break;

            //Command must be preceded by a username and password. They must be seperated by a ';'
            //character. After the password there must be a space character. For example:
            //"admin:pw c1=1" will set port c1, with username="admin" and password="pw"
            case SM_CMDX_UNPW:
                //The ' ' space character signals the end of the username and password
                if (c == ' ') {
                    buf[put] = '\0';    //Terminate Username with NULL

                    if (strcmpee2ram((char*)&buf[0], APPCFG_USERNAME0) == 0) {
                        if (strcmpee2ram((char*)&buf[valueIdx], APPCFG_PASSWORD0) == 0) {
                            smCmdExec = SM_CMDX_IDLE;   //Look for commands
                        }
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
                
                //No break, so it falls thru to SM_CMDX_VALUE, and read c gets added to buf[] buffer
                //break;

            //We are currently reading the value part of a "Command"
            case SM_CMDX_VALUE:
                //No more data, execute this command and exit
                if (flags.bits.bNoMoreData == TRUE) {
                    smCmdExec = SM_CMDX_EXEC;
                    flags.bits.bDontReadNextChar = TRUE;    //Don't read another char, first execute cmd
                }
                //The '&' character is used to seperate multiple commands
                else if (c == '&') {
                    smCmdExec = SM_CMDX_EXEC;
                    flags.bits.bDontReadNextChar = TRUE;    //Don't read another char, first execute cmd
                    c = '\0';                               //End String with NULL character
                }

                //Is there still place in the current buf[] buffer for this name-value pair.
                //Ensure with check that there is still one location avaiable AFTER this one - is required for NULL!
                if ( put < (sizeof(buf)-1) ) {
                    //Write all bytes to buf[], and sent it to execNameValueCmd() function to be processed
                    buf[put++] = c;
                }
                break;

            //We are currently executing a "Command"
            case SM_CMDX_EXEC:
                //Execute the given name-value command
                wUpdate |= execNameValueCmd(buf, &buf[valueIdx], user);

                //There is more data to follow
                if (flags.bits.bNoMoreData == FALSE) {
                    //Reset variable so that next command can be read
                    smCmdExec = SM_CMDX_IDLE;
                    valueIdx = 0;                       //Indicate that value has not been found yet - is everything after '=' character
                    put = 0;
                }
                break;

            //We are receiving the group part of a "Request Command"
            case SM_CMDX_RQST_GROUP:
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
                
                smCmdExec = SM_CMDX_RQST_VALUE;
                break;

            //We are receiving the value part of a "Request Command"
            case SM_CMDX_RQST_VALUE:
                smCmdExec = SM_CMDX_RQST_EXEC;
                break;

            //We are currently executing a "Request Command"
            case SM_CMDX_RQST_EXEC:
                smCmdExec = SM_CMDX_IDLE;
                break;
            } // switch(smCmdExec)
        } // if (smCmd != SM_CMD_IDLE)

    } while (smCmd != SM_CMD_IDLE);
}

/**
 * Execute the given name-value command. The command might not be executed if a higher user level is
 * required.
 *
 * @param name The name part of the name-value command. Has to be a null terminated string
 * @param value The value part of the name-value command. Has to be a null terminated string
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
    BYTE bitPos;

    wUpdate = 0;    //Clear all update flags

    /////////////////////////////////////////////////
    //Update Configuration value
    if (name[0] == CMDGROUP_CONFIG) {
        BYTE cfgOffset; //Offset of configuration byte
        BYTE cfgValue;  //New value of configuration byte
        BYTE mask;
        BYTE currValue;

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
        //Request Serial Number, Username, Password, NetBIOS Name or DNS Server to be changed
        else if (cfgOffset <= APPCFG_DNS_IP3) {
            tmp = 0;                //Indicate that something must be updated
        }
        //Request UDP Command or UDP Command Response ports to be changed
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
        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(8);     //@mxd:8:Received General Command
        #endif

        /////////////////////////////////////////////////
        // The following commands can ONLY be executed by "Admin" or "Super User"
        if (user != USER_GUEST) {
            //The second character of the name part of the name-value pair will be the "Command Code".
            switch(name[1]) {
                case CMDCODE_GEN_NEW_PASSWORD:
                    #if !defined(DEMO_MODE)

                    //Update password with new one just received. Can have a maximum lenght of 8
                    strcpyram2ee(APPCFG_PASSWORD0, (char *)value, 8);

                    #if (DEBUG_CMD >= LOG_INFO)
                    debugPutMsg(11);     //@mxd:11:Received new Password = %s
                    debugPutString(value);
                    #endif

                    #endif
                    break;
                case CMDCODE_GEN_NEW_USERNAME:
                    #if !defined(DEMO_MODE)

                    //Update username with new one just received. Can have a maximum lenght of 8
                    strcpyram2ee(APPCFG_USERNAME0, (char *)value, 8);

                    #if (DEBUG_CMD >= LOG_INFO)
                    debugPutMsg(12);     //@mxd:12:Received new Username = %s
                    debugPutString(value);
                    #endif

                    #endif
                    break;
                case CMDCODE_GEN_NEW_NETBIOS_NAME:
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
                    break;
            }
        }
    }
    /////////////////////////////////////////////////
    //We received a general command without value
    else if (name[0] == CMDGROUP_GENERAL_NOVAL)
    {
        #if (DEBUG_CMD >= LOG_DEBUG)
        debugPutMsg(10);     //@mxd:10:Received General Command without value = %d
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
    else if (name[0] >= CMDGROUP_PWM)
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

    //Return wUpdate - it will indicate what has changed
    return wUpdate;
}



