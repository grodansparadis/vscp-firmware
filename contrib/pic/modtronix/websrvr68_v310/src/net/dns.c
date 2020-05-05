/*********************************************************************
 *
 *                  DNS Module for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        DNS.c
 * Dependencies:    UDP, ARP, Tick
 * Processor:       PIC18
 * Complier:        MPLAB C18 v2.10 or higher <br>
 *                  Microchip C30 v2.01 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Microchip Technology, Inc.
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
 ********************************************************************/
#define THIS_IS_DNS

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\dns.h"
#include "net\compiler.h"
#include "net\udp.h"
#include "net\tick.h"
#include "net\arp.h"
#include "net\arptsk.h"
#include "net\helpers.h"
#include "debug.h"

#if defined(STACK_USE_DNS)

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xE8, msgCode)
#define debugPutMsgRomStr(msgCode, strStr) debugMsgRomStr(0xE8, msgCode, msgStr)


#ifndef DNS_PORT
#define DNS_PORT        53
#endif

#ifndef DNS_TIMEOUT
#define DNS_TIMEOUT        ((TICK)TICK_SECOND * (TICK)2)
#endif


static BYTE *DNSHostName;
static enum
{
    DNS_HOME = 0,
    DNS_RESOLVE_ARP,
    DNS_OPEN_SOCKET,
    DNS_QUERY,
    DNS_GET_RESULT,
    DNS_DONE
} smDNS = DNS_DONE;

typedef struct _DNS_HEADER
{
    WORD_VAL TransactionID;
    WORD_VAL Flags;
    WORD_VAL Questions;
    WORD_VAL Answers;
    WORD_VAL AuthoritativeRecords;
    WORD_VAL AdditionalRecords;
} DNS_HEADER;

typedef struct _DNS_ANSWER_HEADER
{
    WORD_VAL    ResponseName;
    WORD_VAL    ResponseType;
    WORD_VAL    ResponseClass;
    DWORD_VAL    ResponseTTL;
    WORD_VAL    ResponseLen;
} DNS_ANSWER_HEADER;

static void DNSPutString(BYTE *String);
static void DNSGetString(BYTE *String);


/**
 * Call DNSIsResolved() until the host is resolved.
 * A UDP socket must be available before this function is called. It is freed
 * at the end of the resolution.  MAX_UDP_SOCKETS may need to be increased if
 * other modules use UDP sockets. You must not modify Hostname until
 * DNSIsResolved() returns TRUE.
 *
 * @preCondition        Stack is initialized()
 *
 * @param Hostname      Null terminated string specifying the host address to
 *                      resolve to an IP address.
 */
void DNSResolve(BYTE *Hostname)
{
    DNSHostName = Hostname;
    smDNS = DNS_HOME;
}


/**
 * Call DNSIsResolved() until the host is resolved.
 * You cannot start two DNS resolution proceedures concurrently.
 * You must not modify *Hostname until DNSIsResolved() returns TRUE.
 *
 * @preCondition        DNSResolve() was called.
 *
 * @param HostIP        4 byte IP address
 */
BOOL DNSIsResolved(IP_ADDR *HostIP)
{
    static UDP_SOCKET   MySocket;
    static NODE_INFO    Remote;
    static TICK         StartTime;
    BYTE                i;
    WORD_VAL            w;
    DNS_HEADER          DNSHeader;
    DNS_ANSWER_HEADER   DNSAnswerHeader;
    IP_ADDR             tmpIpAddr;

    switch(smDNS)
    {
        case DNS_HOME:
            tmpIpAddr.v[0] = MY_DNS_BYTE1;
            tmpIpAddr.v[1] = MY_DNS_BYTE2;
            tmpIpAddr.v[2] = MY_DNS_BYTE3;
            tmpIpAddr.v[3] = MY_DNS_BYTE4;
            ARPResolve(&tmpIpAddr);
            StartTime = TickGet();
            smDNS++;
            break;
        case DNS_RESOLVE_ARP:
            if(!ARPIsResolved(&tmpIpAddr, &Remote.MACAddr))
            {
                if(TickGet() - StartTime > DNS_TIMEOUT)
                {
                    smDNS--;
                }
                break;
            }
            Remote.IPAddr.Val = tmpIpAddr.Val;
            smDNS++;
            // No need to break, we can immediately start resolution

        case DNS_OPEN_SOCKET:
            MySocket = UDPOpen(0, &Remote, DNS_PORT);
            if(MySocket == INVALID_UDP_SOCKET) {
                #if (DEBUG_DNS >= LOG_ERROR)
                debugPutMsg(1); //@mxd:1:Could not open UDP socket
                #endif

                break;
            }

            smDNS++;
            // No need to break, we can immediately start resolution
            
        case DNS_QUERY:
            if(!UDPIsPutReady(MySocket))
                break;
            
            // Put DNS query here
            UDPPut(0x12);        // User chosen ID
            UDPPut(0x34);
            UDPPut(0x01);        // Standard query with recursion
            UDPPut(0x00);    
            UDPPut(0x00);        // 0x0001 questions
            UDPPut(0x01);
            UDPPut(0x00);        // 0x0000 answers
            UDPPut(0x00);
            UDPPut(0x00);        // 0x0000 name server resource records
            UDPPut(0x00);
            UDPPut(0x00);        // 0x0000 additional records
            UDPPut(0x00);

            // Put hostname string to resolve
            DNSPutString(DNSHostName);

            UDPPut(0x00);        // Type: A (host address)
            UDPPut(0x01);
            UDPPut(0x00);        // Class: IN (Internet)
            UDPPut(0x01);

            UDPFlush();
            StartTime = TickGet();
            smDNS++;
            break;

        case DNS_GET_RESULT:
            if(!UDPIsGetReady(MySocket))
            {
                if(TickGet() - StartTime > DNS_TIMEOUT)
                {
                    smDNS--;
                }
                break;
            }

            // Retrieve the DNS header and de-big-endian it
            UDPGet(&DNSHeader.TransactionID.v[1]);
            UDPGet(&DNSHeader.TransactionID.v[0]);
            UDPGet(&DNSHeader.Flags.v[1]);
            UDPGet(&DNSHeader.Flags.v[0]);
            UDPGet(&DNSHeader.Questions.v[1]);
            UDPGet(&DNSHeader.Questions.v[0]);
            UDPGet(&DNSHeader.Answers.v[1]);
            UDPGet(&DNSHeader.Answers.v[0]);
            UDPGet(&DNSHeader.AuthoritativeRecords.v[1]);
            UDPGet(&DNSHeader.AuthoritativeRecords.v[0]);
            UDPGet(&DNSHeader.AdditionalRecords.v[1]);
            UDPGet(&DNSHeader.AdditionalRecords.v[0]);

            // Remove all questions
            while(DNSHeader.Questions.Val--)
            {
                DNSGetString(NULL);
                UDPGet(&w.v[1]);        // Question type
                UDPGet(&w.v[0]);
                UDPGet(&w.v[1]);        // Question class
                UDPGet(&w.v[0]);
            }
            
            // Scan through answers
            while(DNSHeader.Answers.Val--)
            {
                UDPGet(&DNSAnswerHeader.ResponseName.v[1]);        // Response name
                UDPGet(&DNSAnswerHeader.ResponseName.v[0]);
                UDPGet(&DNSAnswerHeader.ResponseType.v[1]);        // Response type
                UDPGet(&DNSAnswerHeader.ResponseType.v[0]);
                UDPGet(&DNSAnswerHeader.ResponseClass.v[1]);    // Response class
                UDPGet(&DNSAnswerHeader.ResponseClass.v[0]);
                UDPGet(&DNSAnswerHeader.ResponseTTL.v[3]);        // Time to live
                UDPGet(&DNSAnswerHeader.ResponseTTL.v[2]);
                UDPGet(&DNSAnswerHeader.ResponseTTL.v[1]);
                UDPGet(&DNSAnswerHeader.ResponseTTL.v[0]);
                UDPGet(&DNSAnswerHeader.ResponseLen.v[1]);        // Response length
                UDPGet(&DNSAnswerHeader.ResponseLen.v[0]);

                // Make sure that this is a 4 byte IP address, response type A, class 1
                // Check if this is Type A
                if(    DNSAnswerHeader.ResponseType.Val    == 0x0001u && 
                    DNSAnswerHeader.ResponseClass.Val    == 0x0001u && // Internet class
                    DNSAnswerHeader.ResponseLen.Val        == 0x0004u)
                {
                    UDPGet(&HostIP->v[0]);
                    UDPGet(&HostIP->v[1]);
                    UDPGet(&HostIP->v[2]);
                    UDPGet(&HostIP->v[3]);
                    break;
                }
                else
                {
                    while(DNSAnswerHeader.ResponseLen.Val--)
                    {
                        UDPGet(&i);
                    }
                }
            }

            UDPDiscard();
            UDPClose(MySocket);
            MySocket = INVALID_UDP_SOCKET;
            smDNS++;
            // No need to break, we are done and need to return TRUE

        case DNS_DONE:
            return TRUE;
    }
    
    return FALSE;
}

static void DNSPutString(BYTE *String)
{
    BYTE *RightPtr;
    BYTE i;
    BYTE Len;

    RightPtr = String;

    while(1)
    {
        do
        {
            i = *RightPtr++;
        } while((i != 0x00) && (i != '.') && (i != '/'));
    
        // Put the length parameter
        Len = (BYTE)(RightPtr-String-1);
        UDPPut(Len);
        while(Len--)
        {
            UDPPut(*String++);
        }
        if(i == 0x00 || i == '/')
            break;

        // Skip over the '.' in the input string
        String++;
    }
    
    // Put the string terminator character
    UDPPut(0x00);
}

static void DNSGetString(BYTE *String)
{
    BYTE i;
    BYTE j;

    if(String == NULL)
    {
        UDPGet(&i);
        while(i)
        {
            while(i--)
            {
                UDPGet(&j);
            }
            UDPGet(&i);
        }
    }
    else
    {
        UDPGet(&i);
        while(i)
        {
            while(i--)
            {
                UDPGet(String);
                String++;
            }
            UDPGet(&i);
        }
    }
}


#endif    //#if defined(STACK_USE_DNS)
