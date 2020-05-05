/*********************************************************************
 * sercfg.c      -  Serial port configuration
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Allows the SBC65EC to be configured via the Serial Port
 *
 *********************************************************************
 * File History
 *
 * 2006-02-01, David Hosken (DH):
 *    - Fixed bug where network setting were not stored in EEPROM, and thus
 *      not restored after powerup
 * 2005-06-07, David Hosken (DH):
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

#include <string.h>

#include "projdefs.h"
#include "sercfg.h"
#include "busser1.h"
#include "appcfg.h"

#include "net\fsee.h"
#include "net\tick.h"
#include "net\delay.h"
#include "net\helpers.h"


/////////////////////////////////////////////////
//Function prototypes
static void SetConfig(void);

static BOOL DownloadFsysImage(void);


/////////////////////////////////////////////////
//ROM Defines
ROM char PressKeyForConfig[] = "\r\nPress any key for Configuration menu.";
ROM char RunningApp[] = "\r\nRunning Application, IP address: ";

#if defined(STACK_USE_DHCP) || defined(STACK_USE_IP_GLEANING)
ROM char DHCPMsg[] = "DHCP/Gleaning...";
#endif

ROM char menu[] =
    "\r\n\r\n\r\Modtronix Web Server\r\n\r\n"

    "\t1: Change Board serial number.\r\n"
    "\t2: Change default IP address.\r\n"
    "\t3: Change default gateway address.\r\n"
    "\t4: Change default subnet mask.\r\n"
    "\t5: Enable DHCP & IP Gleaning.\r\n"
    "\t6: Disable DHCP & IP Gleaning.\r\n"
    "\t7: Download File System Image.\r\n"
    "\t8: Save & Quit.\r\n"
    "\r\n"
    "Enter a menu choice (1-8): ";

typedef enum _MENU_CMD
{
    MENU_CMD_SERIAL_NUMBER          = '1',
    MENU_CMD_IP_ADDRESS,
    MENU_CMD_GATEWAY_ADDRESS,
    MENU_CMD_SUBNET_MASK,
    MENU_CMD_ENABLE_AUTO_CONFIG,
    MENU_CMD_DISABLE_AUTO_CONFIG,
    MENU_CMD_DOWNLOAD_FSYS_IMAGE,
    MENU_CMD_QUIT,
    MENU_CMD_INVALID
} MENU_CMD;

ROM char* menuCommandPrompt[] =
{
    "\r\nSerial Number (",
    "\r\nDefault IP Address (",
    "\r\nDefault Gateway Address (",
    "\r\nDefault Subnet Mask (",
    "\r\nDHCP & IP Gleaning enabled.\r\n",
    "\r\nDHCP & IP Gleaning disabled.\r\n",
    "\r\nReady to download File System Image - Use Xmodem protocol.\r\n",
    "\r\nNow running application..."
};

ROM char InvalidInputMsg[] = "\r\nInvalid input received - Input ignored.\r\n"
                             "Press any key to continue...\r\n";

//ANSI commands
static ROM char AnsiEscClearScreen[] = "\x1B[2J";

void scfInit(BYTE delay) {
    BYTE i;
    
    //If delay = 0, return!
    if (delay == 0) return;

    //Clear Screen
    serPutRomString(AnsiEscClearScreen);

    /*
     * Wait a couple of seconds for user input.
     * - If something is detected, start config.
     * - If nothing detected, start main program.
     */
    serPutRomString(PressKeyForConfig);

    for (i = delay; i > 0; --i)    //Delay for 50mS x 60 = 3 sec
    {
        if ((i % 8) == 0) serPutByte('.');
        if (serIsGetReady())
        {
            SetConfig();
            break;
        }
        DelayMs(50);
    }

    serPutRomString(RunningApp);
    scfDisplayIPValue(&AppConfig.MyIPAddr);
}


void scfDisplayIPValue(IP_ADDR *IPVal)
{
    char IPDigit[8];

    /*
     * Rewrite the second line.
     */
    itoa(IPVal->v[0], IPDigit);
    {
        serPutString((BYTE*)IPDigit);
        serPutByte('.');
    }

    itoa(IPVal->v[1], IPDigit);
    {
        serPutString((BYTE*)IPDigit);
        serPutByte('.');
    }

    itoa(IPVal->v[2], IPDigit);
    {
        serPutString((BYTE*)IPDigit);
        serPutByte('.');
    }

    itoa(IPVal->v[3], IPDigit);
    {
        serPutString((BYTE*)IPDigit);
    }
}


BYTE USARTGetString(char *buffer, BYTE bufferLen)
{
    BYTE v;
    BYTE count;

    count = 0;
    do
    {
        while( !serIsGetReady() ) FAST_USER_PROCESS();

        v = serGetByte();

        if ( v == '\r' || v == '\n' )
            break;

        count++;
        *buffer++ = v;
        *buffer = '\0';
        if ( bufferLen-- == 0 )
            break;
        FAST_USER_PROCESS();
    } while(1);
    return count;
}

BOOL StringToIPAddress(char *str, IP_ADDR *buffer)
{
    BYTE v;
    char *temp;
    BYTE byteIndex;

    temp = str;
    byteIndex = 0;

    while( v = *str )
    {
        if ( v == '.' )
        {
            *str++ = '\0';
            buffer->v[byteIndex++] = atoi(temp);
            temp = str;
        }
        else if ( v < '0' || v > '9' )
            return FALSE;

        str++;
    }

    buffer->v[byteIndex] = atoi(temp);

    return (byteIndex == 3);
}



MENU_CMD GetMenuChoice(void)
{
    BYTE c;

    while ( !serIsGetReady() ) FAST_USER_PROCESS();

    c = serGetByte();

    if ( c >= '1' && c < MENU_CMD_INVALID )
        return c;
    else
        return MENU_CMD_INVALID;
}

#define MAX_USER_RESPONSE_LEN   (20)
void ExecuteMenuChoice(MENU_CMD choice)
{
    char response[MAX_USER_RESPONSE_LEN];
    IP_ADDR tempIPValue;
    IP_ADDR *destIPValue;
    WORD_VAL w;
    BYTE offset;

    serPutByte('\r');
    serPutByte('\n');
    serPutRomString(menuCommandPrompt[choice-'0'-1]);

    switch(choice)
    {
    case MENU_CMD_SERIAL_NUMBER:
        w.byte.LSB = appcfgGetc(APPCFG_SERIAL_NUMBER0);
        w.byte.MSB = appcfgGetc(APPCFG_SERIAL_NUMBER1);
        itoa(w.Val, response);
        serPutString((BYTE*)response);
        serPutByte(')');
        serPutByte(':');
        serPutByte(' ');

        if ( USARTGetString(response, sizeof(response)) )
        {
            w.Val = atoi(response);
            //Store serial number to non volotile memory
            appcfgPutc(APPCFG_SERIAL_NUMBER0, w.byte.LSB);
            appcfgPutc(APPCFG_SERIAL_NUMBER1, w.byte.MSB);
            
            //Use serial number for last two digits of MAC address
            appcfgPutc(APPCFG_MAC4, w.byte.MSB);
            appcfgPutc(APPCFG_MAC5, w.byte.LSB);
            AppConfig.MyMACAddr.v[4] = w.byte.MSB;
            AppConfig.MyMACAddr.v[5] = w.byte.LSB;
        }
        else
            goto HandleInvalidInput;

        break;

    case MENU_CMD_IP_ADDRESS:
        destIPValue = &AppConfig.MyIPAddr;
        offset = APPCFG_IP0;
        goto ReadIPConfig;

    case MENU_CMD_GATEWAY_ADDRESS:
        destIPValue = &AppConfig.MyGateway;
        offset = APPCFG_GATEWAY0;
        goto ReadIPConfig;

    case MENU_CMD_SUBNET_MASK:
        destIPValue = &AppConfig.MyMask;
        offset = APPCFG_MASK0;

    ReadIPConfig:
        scfDisplayIPValue(destIPValue);
        serPutByte(')');
        serPutByte(':');
        serPutByte(' ');

        USARTGetString(response, sizeof(response));

        if ( !StringToIPAddress(response, &tempIPValue) )
        {
HandleInvalidInput:
            serPutRomString(InvalidInputMsg);
            while( !serIsGetReady() ) FAST_USER_PROCESS();
            serGetByte();
        }
        else
        {
            destIPValue->Val = tempIPValue.Val;
            
            //Update new configuration data just entered
            appcfgPutc(offset++, tempIPValue.v[0]);
            appcfgPutc(offset++, tempIPValue.v[1]);
            appcfgPutc(offset++, tempIPValue.v[2]);
            appcfgPutc(offset++, tempIPValue.v[3]);
        }
        break;


    case MENU_CMD_ENABLE_AUTO_CONFIG:
        //Set DHCP flag
        appcfgPutc(APPCFG_NETFLAGS, appcfgGetc(APPCFG_NETFLAGS) | APPCFG_NETFLAGS_DHCP);
        break;

    case MENU_CMD_DISABLE_AUTO_CONFIG:
        //Clear DHCP flag
        appcfgPutc(APPCFG_NETFLAGS, appcfgGetc(APPCFG_NETFLAGS) & ~APPCFG_NETFLAGS_DHCP);
        break;

    case MENU_CMD_DOWNLOAD_FSYS_IMAGE:
        DownloadFsysImage();
        break;

    case MENU_CMD_QUIT:
        break;
    }
}

static void SetConfig(void)
{
    MENU_CMD choice;

    do
    {
        serPutRomString(menu);
        choice = GetMenuChoice();
        if ( choice != MENU_CMD_INVALID )
            ExecuteMenuChoice(choice);
        FAST_USER_PROCESS();
    } while(choice != MENU_CMD_QUIT);

}


/*********************************************************************
 * Function:        BOOL DownloadFsysImage(void)
 *
 * @preCondition     fsysInit() is already called.
 *
 * @param None
 *
 * @return          TRUE if successful
 *                  FALSE otherwise
 *
 * Side Effects:    This function uses 128 bytes of Bank 4 using
 *                  indirect pointer.  This requires that no part of
 *                  code is using this block during or before calling
 *                  this function.  Once this function is done,
 *                  that block of memory is available for general use.
 *
 * Overview:        This function implements XMODEM protocol to
 *                  be able to receive a binary file from PC
 *                  applications such as HyperTerminal.
 *
 * Note:            In current version, this function does not
 *                  implement user interface to set IP address and
 *                  other informations.  User should create their
 *                  own interface to allow user to modify IP
 *                  information.
 *                  Also, this version implements simple user
 *                  action to start file transfer.  User may
 *                  evaulate its own requirement and implement
 *                  appropriate start action.
 *
 ********************************************************************/
#define XMODEM_SOH      0x01ul
#define XMODEM_EOT      0x04ul
#define XMODEM_ACK      0x06ul
#define XMODEM_NAK      0x15ul
#define XMODEM_CAN      0x18ul
#define XMODEM_BLOCK_LEN 128ul

static BOOL DownloadFsysImage(void)
{
    enum SM_MPFS
    {
        SM_MPFS_SOH,
        SM_MPFS_BLOCK,
        SM_MPFS_BLOCK_CMP,
        SM_MPFS_DATA,
    } state;

    BYTE c;
    BOOL lbDone;
    BYTE blockLen;
    BYTE lResult;
    BYTE tempData[XMODEM_BLOCK_LEN];
    TICK16 lastTick;

    state = SM_MPFS_SOH;
    lbDone = FALSE;

    
    //Prepare the File System to receive a new Image - all data is overwritten! It is assigned handle 0
    fsysOpenImage();

    /*
     * Notify the host that we are ready to receive...
     */
    lastTick = TickGet16bit();
    do
    {
        //More than 500ms has passed since last tick
        if ( TickGetDiff16bit(lastTick) >= ((TICK16)TICKS_PER_SECOND / (TICK16)2) )
        {
            lastTick = TickGet16bit();
            serPutByte(XMODEM_NAK);

            /*
             * Blink LED to indicate that we are waiting for
             * host to send the file.
             */
            //LATA2 ^= 1;
        }
        FAST_USER_PROCESS();
    } while( !serIsGetReady() );


    while(!lbDone)
    {
        FAST_USER_PROCESS();
        
        if ( serIsGetReady() )
        {
            /*
             * Toggle LED as we receive the data from host.
             */
            //LATA2 ^= 1;
            c = serGetByte();
        }
        else
        {
            /*
             * Real application should put some timeout to make sure
             * that we do not wait forever.
             */
            continue;
        }

        switch(state)
        {
        default:
            if ( c == XMODEM_SOH )
            {
                state = SM_MPFS_BLOCK;
            }
            else if ( c == XMODEM_EOT )
            {
                /*
                 * Turn off LED when we are done.
                 */
                //LATA2 = 1;

                fsysCloseImage();
                serPutByte(XMODEM_ACK);
                lbDone = TRUE;
            }
            else
                serPutByte(XMODEM_NAK);

            break;

        case SM_MPFS_BLOCK:
            /*
             * We do not use block information.
             */
            lResult = XMODEM_ACK;
            blockLen = 0;
            state = SM_MPFS_BLOCK_CMP;
            break;

        case SM_MPFS_BLOCK_CMP:
            /*
             * We do not use 1's comp. block value.
             */
            state = SM_MPFS_DATA;
            break;

        case SM_MPFS_DATA:
            /*
             * Buffer block data until it is over.
             */
            tempData[blockLen++] = c;
            if ( blockLen > XMODEM_BLOCK_LEN )
            {
                lResult = XMODEM_ACK;
                for ( c = 0; c < XMODEM_BLOCK_LEN; c++ ) {
                    fsysPutByteImage(tempData[c]);
                 }

                fileRelease(0); //FILE Handle 0 is always used for File System Image functions

                serPutByte(lResult);
                state = SM_MPFS_SOH;
            }
            break;

        }
    }


/*
 * This small wait is required if SLIP is in use.
 * If this is not used, PC might misinterpret SLIP
 * module communication and never close file transfer
 * dialog box.
 */
#if defined(STACK_USE_SLIP)
    {
        BYTE i;
        i = 255;
        while( i-- );
    }
#endif
    return TRUE;
}
