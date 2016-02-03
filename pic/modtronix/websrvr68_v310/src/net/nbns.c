/*********************************************************************
 *
 *    NetBIOS Name Service (NBNS) Server
 *     -Responds to NBNS name requests to allow human name assignment 
 *      to the board
 *
 *********************************************************************
 * FileName:        NBNS.c
 * Dependencies:    UDP, ARP, Tick
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Complier:        Microchip C18 v3.02 or higher
 *                    Microchip C30 v2.01 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * This software is owned by Microchip Technology Inc. ("Microchip") 
 * and is supplied to you for use exclusively as described in the 
 * associated software agreement.  This software is protected by 
 * software and other intellectual property laws.  Any use in 
 * violation of the software license may subject the user to criminal 
 * sanctions as well as civil liability.  Copyright 2006 Microchip
 * Technology Inc.  All rights reserved.
 *
 * This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
 * WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
 * TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
 * INFRINGEMENT.  Microchip shall in no event be liable for special, 
 * incidental, or consequential damages.
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder     8/01/06    Original
 ********************************************************************/
#define THIS_IS_NET_BIOS_NAME_SERVICE

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\nbns.h"
#include "net\compiler.h"
#include "net\stacktsk.h"
#include "net\udp.h"
#include "net\tick.h"
#include "net\helpers.h"
#include "debug.h"

#if defined(STACK_USE_NBNS)

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE7, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xE7, msgCode, msgStr)

/////////////////////////////////////////////////
//Defines
#ifdef NETBIOS_NAME_GETCHAR_DEFAULT
    //Default NetBIOS name, must be 16 char, and all upper case, 16th char = 0x00
    ROM char NETBIOS_NAME_DEF[] =
        {'M','X','B','O','A','D', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 0x00};
#endif


#define NBNS_PORT        (137ul)

typedef struct _NBNS_HEADER
{
    WORD_VAL TransactionID;
    WORD_VAL Flags;
    WORD_VAL Questions;
    WORD_VAL Answers;
    WORD_VAL AuthoritativeRecords;
    WORD_VAL AdditionalRecords;
} NBNS_HEADER;

static void NBNSPutName(void);
static void NBNSGetName(BYTE *String);
static BOOL NBNSCmpName(BYTE *name);

#define NBNS_NAME_MAXLENGTH 16ul

/**
 * Sends responses to NetBIOS name requests
 */
void NBNSTask(void)
{
    static UDP_SOCKET   MySocket;
    BYTE                i;
    WORD_VAL            Type, Class;
    NBNS_HEADER         NBNSHeader;
    BYTE                NameString[NBNS_NAME_MAXLENGTH];
    static enum
    {
        NBNS_HOME = 0,
        NBNS_OPEN_SOCKET,
        NBNS_LISTEN
    } smNBNS = NBNS_HOME;

    switch(smNBNS)
    {
        case NBNS_HOME:
            smNBNS++;
            break;

        case NBNS_OPEN_SOCKET:
            MySocket = UDPOpen(NBNS_PORT, NULL, NBNS_PORT);
            if(MySocket == INVALID_UDP_SOCKET) {
                #if (DEBUG_NBNS >= LOG_ERROR)
                debugPutMsg(2);     //@mxd:2:Could not open UDP socket for NBNS!
                #endif

                break;
            }

            smNBNS++;

        case NBNS_LISTEN:
            if(!UDPIsGetReady(MySocket))
                break;

            #if (DEBUG_NBNS >= LOG_DEBUG)
            debugPutMsg(2);     //@mxd:2:Received NBNS request!
            #endif

            // Retrieve the NBNS header and de-big-endian it
            UDPGet(&NBNSHeader.TransactionID.v[1]);
            UDPGet(&NBNSHeader.TransactionID.v[0]);
            UDPGet(&NBNSHeader.Flags.v[1]);
            UDPGet(&NBNSHeader.Flags.v[0]);
            UDPGet(&NBNSHeader.Questions.v[1]);
            UDPGet(&NBNSHeader.Questions.v[0]);
            UDPGet(&NBNSHeader.Answers.v[1]);
            UDPGet(&NBNSHeader.Answers.v[0]);
            UDPGet(&NBNSHeader.AuthoritativeRecords.v[1]);
            UDPGet(&NBNSHeader.AuthoritativeRecords.v[0]);
            UDPGet(&NBNSHeader.AdditionalRecords.v[1]);
            UDPGet(&NBNSHeader.AdditionalRecords.v[0]);

            // Remove all questions
            while(NBNSHeader.Questions.Val--)
            {
                NBNSGetName(NameString);
                UDPGet(&i);                // <??> Trailing character on string
                UDPGet(&Type.v[1]);        // Question type
                UDPGet(&Type.v[0]);
                UDPGet(&Class.v[1]);    // Question class
                UDPGet(&Class.v[0]);

                #if (DEBUG_NBNS >= LOG_INFO)
                debugPutMsg(3);     //@mxd:3:Received NBNS request for NetBIOS name '%s'
                debugPutString(NameString);
                #endif
                
                //if(Type.Val == 0x0020 && Class.Val == 0x0001 && strcmp(NameString, AppConfig.NetBIOSName) == 0)
                if(Type.Val == 0x0020 && Class.Val == 0x0001 && NBNSCmpName(NameString) )
                {
                    #if (DEBUG_NBNS >= LOG_INFO)
                    debugPutMsg(4);     //@mxd:4:NBNS Name Match!
                    #endif

                    while(!UDPIsPutReady(MySocket)) FAST_USER_PROCESS();

                    NBNSHeader.Flags.Val = 0x8400;

                    UDPPut(NBNSHeader.TransactionID.v[1]);
                    UDPPut(NBNSHeader.TransactionID.v[0]);
                    UDPPut(NBNSHeader.Flags.v[1]);
                    UDPPut(NBNSHeader.Flags.v[0]);
                    UDPPut(0x00);    // 0x0000 Questions
                    UDPPut(0x00);
                    UDPPut(0x00);    // 0x0001 Answers
                    UDPPut(0x01);
                    UDPPut(0x00);    // 0x0000 Athoritative records
                    UDPPut(0x00);
                    UDPPut(0x00);    // 0x0000 Additional records
                    UDPPut(0x00);

                    NBNSPutName();
                    UDPPut(0x00);    // 0x0020 Type: NetBIOS
                    UDPPut(0x20);
                    UDPPut(0x00);    // 0x0001 Class: Internet
                    UDPPut(0x01);
                    UDPPut(0x00);    // 0x00000000 Time To Live
                    UDPPut(0x00);
                    UDPPut(0x00);
                    UDPPut(0x00);

                    UDPPut(0x00);    // 0x0006 Data length
                    UDPPut(0x06);    
                    UDPPut(0x60);    // 0x6000 Flags: H-node, Unique
                    UDPPut(0x00);
                    UDPPut(MY_IP_BYTE1);    // Put out IP address
                    UDPPut(MY_IP_BYTE2);
                    UDPPut(MY_IP_BYTE3);
                    UDPPut(MY_IP_BYTE4);

                    UDPFlush();                
                }

            }
            
            UDPDiscard();

            break;
    }
}


/**
 * Put our NetBIOS Name
 */
static void NBNSPutName(void)
{
    BYTE i, j;

    UDPPut(32);    // NetBIOS names are always 32 bytes long (16 decoded bytes)
    for(i = 0; i < 16; i++)
    {
        j = NETBIOS_NAME_GETCHAR(i);
        UDPPut((j>>4) + 'A');
        UDPPut((j & 0x0F) + 'A');
    }
    
    UDPPut(0x00);
}


/**
 * Get NetBIOS name. The name is decoded with 2 bytes for each character of string.
 */
static void NBNSGetName(BYTE *String)
{
    BYTE i, j, k;
    
//    if(String == NULL)
//    {
//        UDPGet(&i);
//        while(i--)
//        {
//            UDPGet(&j);
//        }
//    }
//    else
    {
        //Initialize with all 0
        //for (i=0; i<NBNS_NAME_MAXLENGTH; i++) {
        //    String[i] = '\0';
        //}
        
        UDPGet(&i);     //Read number of bytes * 2. For example, for 16 byte string, this will be 32

        while(i--)
        {
            UDPGet(&j);
            j -= 'A';
            k = j<<4;
            i--;
            UDPGet(&j);
            j -= 'A';
            
            //Ensure we do not overwrite string
            if (i <= ((NBNS_NAME_MAXLENGTH*2)-2) ) {
                *String++ = k | j;
            }
        }

        //Terminate with 0
        String[NBNS_NAME_MAXLENGTH-1] = '\0';
    }
}


/**
 * Compare given name with this modules name, and return TRUE or FALSE.
 *
 * @param name      Name given by NetBIOS
 * @return          TRUE if the given name is our device's name
 *                  FALSE if not
 */
static BOOL NBNSCmpName(BYTE *name) {
    BYTE i;
    BYTE c1, c2;
    
    //Compare first 15 characters, 16th character is NULL
    for (i = 0; i < 15; i++) {
        c1 = NETBIOS_NAME_GETCHAR(i);
        c2 = *name++;

        //Characters don't match!
        if (c1 != c2) {
            return FALSE;   //No match!
        }
        
        //End of both strings reached, Match!
        if (c1 == c2 == NULL) {
            return TRUE;    //Match!
        }
    }
    
    return TRUE;    //Match!
}


#endif //#if defined(STACK_USE_NBNS)
