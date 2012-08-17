/*********************************************************************
 *
 *                  FTP ServerModule for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        ftp.c
 * Dependencies:    stacktsk.h
 *                  tcp.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -Zg -E -C
 *
*
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/23/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     11/13/02 Fixed FTPServer()
  ********************************************************************/
#define THIS_IS_FTP

#include <string.h>
#include <stdlib.h>

#include "projdefs.h"
#include "net\ftp.h"
#include "net\tcp.h"
#include "net\tick.h"
#include "net\fsee.h"
#include "debug.h"

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD2, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xD2, msgCode, msgStr)


#if !defined(STACK_USE_FTP_SERVER)
    #error FTP SERVER module is not enabled.
    #error If you do not want FTP module, remove this file from your
    #error project to reduce your code size.
    #error If you do want FTP module, make sure that STACK_USE_FTP_SERVER
    #error is defined in projdefs.h file.
#endif

typedef enum _SM_FTP
{
    SM_FTP_NOT_CONNECTED_ENTRY,
    SM_FTP_NOT_CONNECTED,
    SM_FTP_CONNECTED,
    SM_FTP_USER_NAME,
    SM_FTP_USER_PASS,
    SM_FTP_RESPOND
} SM_FTP;

typedef enum _SM_FTP_CMD
{
    SM_FTP_CMD_IDLE,
    SM_FTP_CMD_WAIT,
    SM_FTP_CMD_RECEIVE,
    SM_FTP_CMD_WAIT_FOR_DISCONNECT
} SM_FTP_CMD;

typedef enum _FTP_COMMAND
{
    FTP_CMD_USER,
    FTP_CMD_PASS,
    FTP_CMD_QUIT,
    FTP_CMD_STOR,
    FTP_CMD_PORT,
    FTP_CMD_ABORT,
    FTP_CMD_UNKNOWN,
    FTP_CMD_NONE,
} FTP_COMMAND;

/*
 * Each entry in following table must match with that of FTP_COMMAND enum.
 */
ROM char *FTPCommandString[] =
{
    { "USER" },                         // FTP_CMD_USER
    { "PASS" },                         // FTP_CMD_PASS
    { "QUIT" },                         // FTP_CMD_QUIT
    { "STOR" },                         // FTP_CMD_STOR
    { "PORT" },                         // FTP_CMD_PORT
    { "ABOR" }                          // FTP_CMD_ABORT
};
#define FTP_COMMAND_TABLE_SIZE  ( sizeof(FTPCommandString)/sizeof(FTPCommandString[0]) )


typedef enum _FTP_RESPONSE
{
    FTP_RESP_BANNER,
    FTP_RESP_USER_OK,
    FTP_RESP_PASS_OK,
    FTP_RESP_QUIT_OK,
    FTP_RESP_STOR_OK,
    FTP_RESP_UNKNOWN,
    FTP_RESP_LOGIN,
    FTP_RESP_DATA_OPEN,
    FTP_RESP_DATA_READY,
    FTP_RESP_DATA_CLOSE,
    FTP_RESP_OK,

    FTP_RESP_NONE                       // This must always be the last
                                        // There is no corresponding string.
} FTP_RESPONSE;

/*
 * Each entry in following table must match with FTP_RESPONE enum
 */
ROM char *FTPResponseString[] =
{
    "220 Ready\r\n",                    // FTP_RESP_BANNER
    "331 Password required\r\n",        // FTP_RESP_USER_OK
    "230 Logged in\r\n",                // FTP_RESP_PASS_OK
    "221 Bye\r\n",                      // FTP_RESP_QUIT_OK
    "500 \r\n",                         // FTP_RESP_STOR_OK
    "502 Not implemented\r\n",          // FTP_RESP_UNKNOWN
    "530 Login required\r\n",           // FTP_RESP_LOGIN
    "150 Transferring data...\r\n",     // FTP_RESP_DATA_OPEN
    "125 Done.\r\n",                    // FTP_RESP_DATA_READY
    "226 Transfer Complete\r\n",        // FTP_RESP_DATA_CLOSE
    "200 ok\r\n"                        // FTP_RESP_OK
};


static union
{
    struct
    {
        unsigned int bUserSupplied : 1;
        unsigned int bLoggedIn: 1;
    } Bits;
    BYTE Val;
} FTPFlags;


static TCP_SOCKET       FTPSocket;      // Main ftp command socket.
static TCP_SOCKET       FTPDataSocket;  // ftp data socket.
static WORD_VAL         FTPDataPort;    // ftp data port number as supplied by client

static SM_FTP           smFTP;          // ftp server FSM state
static SM_FTP_CMD       smFTPCommand;   // ftp command FSM state

static FTP_COMMAND      FTPCommand;
static FTP_RESPONSE     FTPResponse;

static char             FTPUser[FTP_USER_NAME_LEN];
static char             FTPString[MAX_FTP_CMD_STRING_LEN+2];
static BYTE             FTPStringLen;               //Length

// Element 0 will contain FTP command, remaining elements will contain Parameters
static char             *FTP_argv[MAX_FTP_ARGS];    
//number of parameters - 1". For example, it it contains 2, there is 1 parameter
static BYTE             FTP_argc;
static TICK16           lastActivity;               // Timeout keeper.


/*
 * Private helper functions.
 */
static FTP_COMMAND ParseFTPCommand(char *cmd);
static void ParseFTPString(void);
static BOOL ExecuteFTPCommand(FTP_COMMAND cmd);
static BOOL PutFile(void);
static BOOL Quit(void);


/**
 * Initializes internal variables of Telnet
 *
 * @preCondition    TCP module is already initialized.
 */
 
void FTPInit(void)
{
    FTPSocket       = TCPListen(FTP_COMMAND_PORT);
    smFTP           = SM_FTP_NOT_CONNECTED_ENTRY;
    FTPStringLen    = 0;
    FTPFlags.Val    = 0;
    FTPDataPort.Val = FTP_DATA_PORT;
}


/**
 * This function acts as a task (similar to one in RTOS). This function
 * performs its task in co-operative manner.  Main application must call
 * this function repeatdly to ensure all open or new connections are served on time.
 *
 * @preCondition    FTPInit() must already be called.
 * @return          Opened Telnet connections are served.
 */
BOOL FTPServer(void)
{
    BYTE c;

    //If NOT connected, set variables to default values and return
    if ( !TCPIsConnected(FTPSocket) )
    {
        FTPStringLen    = 0;
        FTPCommand      = FTP_CMD_NONE;
        smFTP           = SM_FTP_NOT_CONNECTED_ENTRY;
        FTPFlags.Val    = 0;
        smFTPCommand    = SM_FTP_CMD_IDLE;

        return TRUE;
    }

    //There is data ready to be read!
    if ( TCPIsGetReady(FTPSocket) )
    {
        lastActivity    = TickGet16bit();   //Store last time FTP port was active

        //Read FTP string into FTPString[] buffer and set FTPStringLen with length of read string.
        while( TCPGet(FTPSocket, &c ) )
        {
            //No more space in buffer!
            if ( FTPStringLen >= MAX_FTP_CMD_STRING_LEN ) {
                #if (DEBUG_FTP >= LOG_WARN)
                debugPutMsg(4);     //@mxd:4:FTP command truncated - command buffer too small!
                #endif
            }
            else {
                FTPString[FTPStringLen++] = c;
            }
        }
        TCPDiscard(FTPSocket);

        if ( c == '\n' )
        {
            FTPString[FTPStringLen]     = '\0';
            FTPStringLen                = 0;
            
            #if (DEBUG_FTP >= LOG_INFO)
            debugPutMsg(1);     //Received string on FTP port = %s
            debugPutString(FTPString);
            #endif

            //Parse FTPString for FTP command and parameters. The found FTP command and parameters are
            //written to the FTP_argv[] array.
            ParseFTPString();
            
            #if (DEBUG_FTP >= LOG_INFO)
            debugPutMsg(6);     //@mxd:6:Parsed string, cmd=%s, found %d parameters
            debugPutString(FTP_argv[0]);
            debugPutByte(((BYTE)(FTP_argc - 1)) & 0x7f);

            debugPutMsg(7);     //@mxd:7:Parameters:%s
            for (i = 1; i < FTP_argc; i++) {
                debugPutByte(' '); //Space between parameters
                debugPutStringXNull(FTP_argv[i]);
            }
            debugPutByte('\0'); //Null terminate
            #endif

            //Parse the received FTP command (FTP-argv[0]) , and return the found command, FTP_CMD_XXX
            FTPCommand                  = ParseFTPCommand(FTP_argv[0]);
        }
        else {
            #if (DEBUG_FTP >= LOG_WARN)
            debugPutMsg(5);     //@mxd:5:Received string on FTP port did NOT end with newline char!
            #endif
        }
    }
    // - There is NO data ready to read AND
    // - The state machine is NOT in the "Not Connected Entry" state
    else if ( smFTP != SM_FTP_NOT_CONNECTED_ENTRY )
    {
        //FTP Timeout has expired
        if (TickGetDiff16bit(lastActivity) >= FTP_TIMEOUT )
        {
            lastActivity                = TickGet16bit();
            FTPCommand                  = FTP_CMD_QUIT;
            smFTP                       = SM_FTP_CONNECTED;
        }
    }

    switch(smFTP)
    {
    case SM_FTP_NOT_CONNECTED_ENTRY:
        FTPResponse = FTP_RESP_BANNER;
        lastActivity = TickGet16bit();

        smFTP = SM_FTP_NOT_CONNECTED;     //We can respond
        /* No break, speed up things */
        
    case SM_FTP_NOT_CONNECTED:
        //Current status is:
        // - TCP connection is active to out FTP port
        // - We are waiting for TCPIsPutReady() to go active so we can send the "220 Ready" message to the client
        if ( !TCPIsPutReady(FTPSocket)) {
            break;      //Break, can not continue until TCPIsPutReady() returns true!
        }
        
        smFTP = SM_FTP_RESPOND;     //We can respond
        /* No break, will speed up things a bit! */
        
    case SM_FTP_RESPOND:
SM_FTP_RESPOND_Label:
        if ( TCPIsPutReady(FTPSocket) )
        {
            ROM char* pMsg;

            pMsg = FTPResponseString[FTPResponse];
            
            #if (DEBUG_FTP >= LOG_INFO)
            debugPutMsg(3);     //Sending Responce message = %s
            debugPutRomString(pMsg);
            #endif

            while( (c = *pMsg++) )
            {
                TCPPut(FTPSocket, c);
            }
            TCPFlush(FTPSocket);
            FTPResponse = FTP_RESP_NONE;
            smFTP = SM_FTP_CONNECTED;
        }
        /* No break - this will speed up little bit */

    case SM_FTP_CONNECTED:
        if ( FTPCommand != FTP_CMD_NONE )
        {
            if ( ExecuteFTPCommand(FTPCommand) )
            {
                if ( FTPResponse != FTP_RESP_NONE )
                    smFTP = SM_FTP_RESPOND;
                else if ( FTPCommand == FTP_CMD_QUIT )
                    smFTP = SM_FTP_NOT_CONNECTED;

                FTPCommand = FTP_CMD_NONE;
                smFTPCommand = SM_FTP_CMD_IDLE;
            }
            else if ( FTPResponse != FTP_RESP_NONE )
            {
                smFTP = SM_FTP_RESPOND;
                goto SM_FTP_RESPOND_Label;
            }
        }
        break;
    }

    return TRUE;
}

static BOOL ExecuteFTPCommand(FTP_COMMAND cmd)
{
    switch(cmd)
    {
    case FTP_CMD_USER:
        FTPFlags.Bits.bUserSupplied = TRUE;
        FTPFlags.Bits.bLoggedIn = FALSE;
        FTPResponse = FTP_RESP_USER_OK;
        //Argument 1 contains FTP User
        strncpy(FTPUser, FTP_argv[1], sizeof(FTPUser));
        break;

    case FTP_CMD_PASS:
        if ( !FTPFlags.Bits.bUserSupplied )
            FTPResponse = FTP_RESP_LOGIN;
        else
        {
            #if (DEBUG_FTP >= LOG_INFO)
                debugPutMsg(8); //@mxd:8:Received FTP Login (%s) and Password (%s)
                debugPutString(FTPUser);
                debugPutString(FTP_argv[1]);
            #endif
        
            //Verify Username and Password
            if ( FTPVerify(FTPUser, FTP_argv[1]) )
            {
                FTPFlags.Bits.bLoggedIn = TRUE;
                FTPResponse = FTP_RESP_PASS_OK;
            }
            else
                FTPResponse = FTP_RESP_LOGIN;
        }
        break;

    case FTP_CMD_QUIT:
        return Quit();

    case FTP_CMD_PORT:
        FTPDataPort.v[1] = (BYTE)atoi(FTP_argv[5]);
        FTPDataPort.v[0] = (BYTE)atoi(FTP_argv[6]);
        FTPResponse = FTP_RESP_OK;
        break;

    case FTP_CMD_STOR:
        return PutFile();

    case FTP_CMD_ABORT:
        FTPResponse = FTP_RESP_OK;
        if ( FTPDataSocket != INVALID_SOCKET )
            TCPDisconnect(FTPDataSocket);
        break;

    default:
        FTPResponse = FTP_RESP_UNKNOWN;
        break;
    }
    return TRUE;
}

/**
 * FTP Quite command received
 */
static BOOL Quit(void)
{
    switch(smFTPCommand)
    {
    case SM_FTP_CMD_IDLE:
#if defined(FTP_PUT_ENABLED)
        if ( smFTPCommand == SM_FTP_CMD_RECEIVE )
            fsysCloseImage();
#endif

        if ( FTPDataSocket != INVALID_SOCKET )
        {
#if defined(FTP_PUT_ENABLED)
            fsysCloseImage();
#endif
            TCPDisconnect(FTPDataSocket);
            smFTPCommand = SM_FTP_CMD_WAIT;
        }
        else
            goto Quit_Done;
        break;

    case SM_FTP_CMD_WAIT:
        if ( !TCPIsConnected(FTPDataSocket) )
        {
Quit_Done:
            FTPResponse = FTP_RESP_QUIT_OK;
            smFTPCommand = SM_FTP_CMD_WAIT_FOR_DISCONNECT;
        }
        break;

    case SM_FTP_CMD_WAIT_FOR_DISCONNECT:
        if ( TCPIsPutReady(FTPSocket) )
        {
            if ( TCPIsConnected(FTPSocket) )
                TCPDisconnect(FTPSocket);
        }
        break;

    }
    return FALSE;
}

/**
 * FTP STOR command received
 */
static BOOL PutFile(void)
{
    BYTE c;


    switch(smFTPCommand)
    {
    case SM_FTP_CMD_IDLE:
        if ( !FTPFlags.Bits.bLoggedIn )
        {
            FTPResponse     = FTP_RESP_LOGIN;
            return TRUE;    //Error
        }
        else
        {
            FTPResponse     = FTP_RESP_DATA_OPEN;
            FTPDataSocket   = TCPConnect(&REMOTE_HOST(FTPSocket), FTPDataPort.Val);
            smFTPCommand    = SM_FTP_CMD_WAIT;
        }
        break;

    case SM_FTP_CMD_WAIT:
        if ( TCPIsConnected(FTPDataSocket) )
        {
#if defined(FTP_PUT_ENABLED)
            if ( !fsysIsInUse() )
#endif
            {
#if defined(FTP_PUT_ENABLED)
                //Prepare the File System to receive a new Image - all data is overwritten! It is assigned handle 0
                fsysOpenImage();
                //FTPFileHandle   = fsysFormat();
#endif

                smFTPCommand    = SM_FTP_CMD_RECEIVE;
            }
        }
        break;

    case SM_FTP_CMD_RECEIVE:
        if ( TCPIsGetReady(FTPDataSocket) )
        {
            /*
             * Reload timeout timer.
             */
            lastActivity    = TickGet16bit();
            
            #if (DEBUG_FTP >= LOG_INFO)
            //debugPutMsg(2);     //Received command = 0x%x
            //debugPutByteHex(c);
            #endif
            
            while( TCPGet(FTPDataSocket, &c) )
            {
#if defined(FTP_PUT_ENABLED)
                fsysPutByteImage(c);
#endif
            }
            fileRelease(0);
            TCPDiscard(FTPDataSocket);
        }
        else if ( !TCPIsConnected(FTPDataSocket) )
        {
#if defined(FTP_PUT_ENABLED)
            fsysCloseImage();
#endif
            TCPDisconnect(FTPDataSocket);
            FTPDataSocket   = INVALID_SOCKET;
            FTPResponse     = FTP_RESP_DATA_CLOSE;
            return TRUE;    //Error
        }
    }
    return FALSE;
}


/**
 * Parse the given string for a FTP command, and return the found command, FTP_CMD_XXX
 *
 * @param cmd String to parse, must contain an uppercase, 4 character long FTP command.
 *
 * @return Returns the FTP command found in the given string. Is a FTP_CMD_XX constant
 */
static FTP_COMMAND ParseFTPCommand(char *cmd)
{
    FTP_COMMAND i;

    for ( i = 0; i < (FTP_COMMAND)FTP_COMMAND_TABLE_SIZE; i++ )
    {
    if ( !memcmppgm2ram((void*)cmd, (ROM void*)FTPCommandString[i], 4) )
            return i;
    }

    return FTP_CMD_UNKNOWN;
}

/**
 * Parse FTPString for FTP command and parameters. The found FTP command and parameters are
 * written to the FTP_argv[] array. On return:
 * - FTP_argv[0] will contain the NULL terminated FTP command
 * - FTP_argc will contain "number of parameters - 1". For example, it it contains 2, there is 1 parameter
 *   at FTP_argv[1].
 */
static void ParseFTPString(void)
{
    BYTE *p;
    BYTE c;
    enum { SM_FTP_PARSE_PARAM, SM_FTP_PARSE_SPACE } smParseFTP;

    smParseFTP  = SM_FTP_PARSE_PARAM;
    p           = (BYTE*)&FTPString[0];

    //Skip leading blanks
    while( *p == ' ' )
        p++;

    //Store FTP command at FTP_argv[0]. The additional FTP_argv[] locations will be used to store the command
    //parameters. For example, if the command has two parameters, they will be stored at FTP_argv[1] and
    //FTP_argv[2]
    FTP_argv[0]  = (char*)p;
    FTP_argc     = 1;

    while( (c = *p) )
    {
        switch(smParseFTP)
        {
        case SM_FTP_PARSE_PARAM:
            //Space or comma indicates next parameter is to follow. Replace with NULL character so that
            //out FTP_argv[] array always points to NULL terminated commands and parameters.
            if ( c == ' ' || c == ',' )
            {
                *p = '\0';
                smParseFTP = SM_FTP_PARSE_SPACE;
            }
            else if ( c == '\r' || c == '\n' ) {
                *p = '\0';
            }
            
            break;

        case SM_FTP_PARSE_SPACE:
            if ( c != ' ' )
            {
                //Store parameter
                FTP_argv[FTP_argc++] = (char*)p;
                smParseFTP = SM_FTP_PARSE_PARAM;
            }
            break;
        }
        p++;
    }
}

