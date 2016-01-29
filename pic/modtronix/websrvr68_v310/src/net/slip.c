/*********************************************************************
 *
 *                  SLIP Module for Modtronix TCP/IP Stack
 *
 *********************************************************************
 * FileName:        slip.c
 * Dependencies:    stacktsk.h
 *                  mac.h
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
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     11/27/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 ********************************************************************/
#define THIS_IS_MAC_LAYER

#include "projdefs.h"
#include "net\checkcfg.h"
#include "net\mac.h"

#if !defined(STACK_USE_SLIP)
#error SLIP module is not enabled.
#error Make sure that correct option is selected.
#endif

/////////////////////////////////////////////////
//Default defines - they can be overridden in projdefs.h file
#if !defined(SLIP_BAUD_RATE)
#define SLIP_BAUD_RATE               (38400ul)     // bps
#endif

#define MAX_SLIP_BUFFER_SIZE    MAC_TX_BUFFER_SIZE

#if (MAX_SLIP_BUFFER_SIZE > 250ul)
#error Invalid MAX_SLIP_BUFFER_SIZE detected
#endif

#define USART_USE_BRGH_LOW
#if defined(USART_USE_BRGH_LOW)
    #define SPBRG_VAL           ( ((CLOCK_FREQ/SLIP_BAUD_RATE)/64) - 1)
#else
    #define SPBRG_VAL           ( ((CLOCK_FREQ/SLIP_BAUD_RATE)/16) - 1)
#endif

#if SPBRG_VAL > 255
    #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
#endif

typedef struct _SLIP_TX_BUFFER
{
    BYTE Length;
    BYTE IOAccess;
    BYTE CallerAccess;
    union
    {
        struct
        {
            unsigned int bIsModemString : 1;
        } bits;
        BYTE byte;
    } Flags;
    BYTE Data[MAX_SLIP_BUFFER_SIZE];
} SLIP_TX_BUFFER;

typedef struct _SLIP_RX_BUFFER
{
    BYTE CurrentPacket;
    BYTE PacketCount;
    BYTE IOAccess;
    BYTE CallerAccess;
    BYTE Data[MAX_SLIP_BUFFER_SIZE];        // Format: <Next>[<Data>...]
} SLIP_RX_BUFFER;

#if defined(MCHP_C18)
#pragma udata SLIP_TX_BUFFER_SEG = 0x400
SLIP_TX_BUFFER TxBuffer;
#endif
#if defined(HITECH_C18)
SLIP_TX_BUFFER TxBuffer @ 0x400;
#endif

#if defined(MCHP_C18)
#pragma udata SLIP_RX_BUFFER_SEG = 0x300
SLIP_RX_BUFFER RxBuffer;
#endif
#if defined(HITECH_C18)
SLIP_RX_BUFFER RxBuffer @ 0x300;
#endif

#if defined(MCHP_C18)
#pragma udata
#endif

static BOOL bIsRxActive;


/*
 * SLIP escape character as per RFC 1055
 */
#define END                     (0xc0ul)
#define ESC                     (0xdbul)
#define ESC_END                 (0xdcul)
#define ESC_ESC                 (0xddul)

typedef enum _SM_SLIP
{
    SM_SLIP_SOF = 0,
    SM_MODEM_CMD,
    SM_MODEM_CMD_AT,
    SM_SLIP_DATA,
    SM_SLIP_ESC,
    SM_SLIP_OVERFLOW
} SM_SLIP;

static struct
{
    BYTE    tempData;
    BYTE    currentPacket;
    SM_SLIP smSLIP;
    BYTE    currentLength;
    union
    {
        struct
        {
            unsigned int bIsDataReady   : 1;
            unsigned int bIsPacketDone  : 1;
        } bits;
        BYTE byte;
    } Flags;
} ISRLocals;


void MACISR(void)
{
    /*
     * If we have received an character, handle it.
     */
    if ( PIR1_RCIF && PIE1_RCIE )
    {
        /*
         * Check for any error, and clear it if there is any.
         */
        if ( RCSTA & 0x06 )
        {
            RCSTA_CREN = 0;
            RCSTA_CREN = 1;
            ISRLocals.smSLIP = SM_SLIP_OVERFLOW;
        }

        ISRLocals.tempData = RCREG;

        ISRLocals.Flags.bits.bIsDataReady = FALSE;

        switch(ISRLocals.smSLIP)
        {
        default:    // case SM_SLIP_SOF:
            /*
             * Remember current packet start location in a RxBuffer.
             * This will be useful when buffer overflows and we need to
             * discard packet that was overflown, keeping previously
             * loaded packets intact.
             */
            ISRLocals.currentPacket             = RxBuffer.IOAccess;
            ISRLocals.currentLength             = 0;

            if ( ISRLocals.tempData == END )
                ISRLocals.smSLIP = SM_SLIP_DATA;

            /*
             * Handle modem commands differently.
             */
            else if ( ISRLocals.tempData == 'A' )
            {
                ISRLocals.Flags.bits.bIsDataReady = TRUE;
                ISRLocals.smSLIP = SM_MODEM_CMD_AT;
            }
            break;

        case SM_MODEM_CMD:
            ISRLocals.Flags.bits.bIsDataReady = TRUE;
            if ( ISRLocals.tempData == '\r' )
            {
                ISRLocals.Flags.bits.bIsPacketDone = TRUE;
            }
            break;

        case SM_MODEM_CMD_AT:
            ISRLocals.Flags.bits.bIsDataReady = TRUE;
            if ( ISRLocals.tempData == 'T' )
                ISRLocals.smSLIP = SM_MODEM_CMD;
            else
                ISRLocals.smSLIP = SM_SLIP_OVERFLOW;
            break;

        case SM_SLIP_DATA:
            ISRLocals.Flags.bits.bIsDataReady   = TRUE;
            /*
             * If this is a escape character, wait for next
             * character to determine its value.
             */
            if ( ISRLocals.tempData == ESC )
            {
                ISRLocals.Flags.bits.bIsDataReady = FALSE;
                ISRLocals.smSLIP = SM_SLIP_ESC;
            }

            else if ( ISRLocals.tempData == END )
            {
                /*
                 * End of packet is received.
                 * Make sure that we have at least one data byte
                 * before declaring that new packet is received.
                 */
                if ( ISRLocals.currentLength )
                {
                    ISRLocals.Flags.bits.bIsPacketDone = TRUE;

                    /*
                     * Even though we are saving END character,
                     * it will be overwritten by next packet as per
                     * bIsPacketDone logic.
                     * This saves some code space as we do not have to
                     * wrap IOAccess index twice.
                     */
                }
                else
                    ISRLocals.Flags.bits.bIsDataReady = FALSE;
            }

            break;

        case SM_SLIP_ESC:
            // Save the data.
            ISRLocals.Flags.bits.bIsDataReady   = TRUE;
            ISRLocals.smSLIP                    = SM_SLIP_DATA;

            if ( ISRLocals.tempData == ESC_END )
                ISRLocals.tempData = END;

            else if ( ISRLocals.tempData == ESC_ESC )
                ISRLocals.tempData = ESC;

            else
                /*
                 * This is an invalid character sequence.
                 * Do not save it.
                 */
            {
                ISRLocals.Flags.bits.bIsDataReady = FALSE;
                ISRLocals.smSLIP = SM_SLIP_SOF;
            }
            break;

        case SM_SLIP_OVERFLOW:
            RxBuffer.IOAccess                   = ISRLocals.currentPacket;
            ISRLocals.smSLIP                    = SM_SLIP_SOF;
            break;
        }

        if ( ISRLocals.Flags.bits.bIsDataReady )
        {
            RxBuffer.IOAccess++;
            if ( RxBuffer.IOAccess >= MAX_SLIP_BUFFER_SIZE )
                RxBuffer.IOAccess = 0;

            /*
             * Make sure that we have not reached end of buffer.
             */
            if ( RxBuffer.IOAccess != RxBuffer.CurrentPacket )
            {
                ISRLocals.currentLength++;
                RxBuffer.Data[RxBuffer.IOAccess] = ISRLocals.tempData;
            }
            else
            {
                /*
                 * We have reached end of buffer.
                 * Discard this packet by restoring IOAccess to begining
                 * of current packet.
                 */
                RxBuffer.IOAccess                   = ISRLocals.currentPacket;
                ISRLocals.smSLIP                    = SM_SLIP_SOF;
                ISRLocals.Flags.bits.bIsPacketDone  = FALSE;
            }

        }

        if ( ISRLocals.Flags.bits.bIsPacketDone )
        {
            ISRLocals.Flags.bits.bIsPacketDone      = FALSE;

            /*
             * Create link to next packet.
             */
            RxBuffer.Data[ISRLocals.currentPacket]  = RxBuffer.IOAccess;
            RxBuffer.PacketCount++;

            /*
             * Start waiting for another packet.
             */
            ISRLocals.smSLIP                        = SM_SLIP_SOF;
        }
    }

    if ( PIR1_TXIF && PIE1_TXIE )
    {
        /*
         * If there is any data, transmit it.
         */
        if ( TxBuffer.Length )
        {
            ISRLocals.tempData = TxBuffer.Data[TxBuffer.IOAccess];
            if ( ISRLocals.tempData == END )
            {
                ISRLocals.tempData = ESC;
                TxBuffer.Data[TxBuffer.IOAccess] = ESC_END;
            }
            else if ( ISRLocals.tempData == ESC )
            {
                ISRLocals.tempData = ESC;
                TxBuffer.Data[TxBuffer.IOAccess] = ESC_ESC;
            }
            else
            {
                TxBuffer.Length--;
                TxBuffer.IOAccess++;
            }
            TXREG = ISRLocals.tempData;
        }
        else
        {
            /*
             * Else, disable transmit interrupt.
             */
            PIE1_TXIE = 0;

            if ( TxBuffer.Flags.bits.bIsModemString )
                TxBuffer.Flags.bits.bIsModemString = FALSE;
            else
                TXREG = END;

        }
    }
}

void MACInit(void)
{
    MACDiscardTx(0);

    /*
     * Initialize ISR variables.
     */
    ISRLocals.smSLIP        = SM_SLIP_SOF;
    ISRLocals.Flags.byte    = 0;

    RxBuffer.CurrentPacket  = 0;
    RxBuffer.PacketCount    = 0;
    RxBuffer.CallerAccess   = 0;
    RxBuffer.IOAccess       = 0;

    bIsRxActive = FALSE;

    /*
     * Clear receive interrupt flag.
     */
    PIR1_RCIF = 0;

    /*
     * Enable receive interrupt.
     * Do not enable transmit interrupt yet.  Enable it only when
     * there is something to transmit
     */
    PIE1_RCIE = 1;
    PIE1_TXIE = 0;

    /*
     * Configure USART for Asynch, 8-bit mode.
     */
#if defined(USART_USE_BRGH_LOW)
    TXSTA = 0b00100000;
#else
    TXSTA = 0b00100100;
#endif

    RCSTA = 0b10010000;
    SPBRG = SPBRG_VAL;

    /*
     * Enable global and peripheral interrupts.
     */
    INTCON_GIEH = 1;
    INTCON_GIEL = 1;

}

BOOL    MACIsTxReady(void)
{
    //Return true if:
    // - There is no transmission currently in progress
    // - There is still place in the transmit buffer
    return ( (TxBuffer.Length < MAX_SLIP_BUFFER_SIZE) && !PIE1_TXIE);
}

BOOL    MACGetHeader(MAC_ADDR *remote, BYTE* type)
{
    /*
     * This marks that all future accesses to MACGet and MACPut
     * be applied to Receive buffer.
     */
    bIsRxActive = TRUE;

    if ( RxBuffer.PacketCount )
    {

        /*
         * Set up packet access index.
         */
        RxBuffer.CallerAccess = RxBuffer.CurrentPacket;

        RxBuffer.CallerAccess++;
        if ( RxBuffer.CallerAccess >= MAX_SLIP_BUFFER_SIZE )
            RxBuffer.CallerAccess = 0;


        /*
         * Handle modem commands differently.
         */
        if ( RxBuffer.Data[RxBuffer.CallerAccess] == 'A' )
        {
            /*
             * Once a modem command is detected, we are not interested
             * in detail.
             */
            MACDiscardRx(); //Discard the contents of the current RX buffer

            /*
             * Mark TxBuffer for Modem String.
             * This will make sure that Transmit ISR does nottransmit END
             * at the end.
             */
            TxBuffer.Flags.bits.bIsModemString = TRUE;

            /*
             * Since this special handling does not follow standard
             * SLIP buffer logic, we will setup all required variables
             * manually.
             */
            TxBuffer.Length = 4;
            TxBuffer.CallerAccess = 0;

            /*
             * Remember to use transmit buffer for MACPut
             */
            bIsRxActive = FALSE;

            /*
             * Now load modem response.
             */
            MACPut('O');
            MACPut('K');
            MACPut('\r');
            MACPut('\n');

            /*
             * Transmit it.
             */
            MACFlush();
        }
        else
        {
            /*
             * This was not a modem command.
             * It must be IP packet.  Mark it accordingly and return.
             */
            *type = MAC_IP;

            return TRUE;
        }
    }
    return FALSE;
}

BYTE    MACGet(void)
{
    BYTE c;

    /*
     * Depending on what buffer is active, act accordingly.
     */
    if ( bIsRxActive )
    {
        c = RxBuffer.Data[RxBuffer.CallerAccess++];
        if ( RxBuffer.CallerAccess >= MAX_SLIP_BUFFER_SIZE )
            RxBuffer.CallerAccess = 0;
    }
    else
    {
        /*
         * TxBuffer.Data contains data bytes only. There is no link byte.
         */
        c = TxBuffer.Data[TxBuffer.CallerAccess++];
    }

    return c;
}

WORD    MACGetArray(BYTE *val, WORD len)
{
    /*
     * This function does not verify requested bytes against available
     * bytes in current packet.  Higher level logic must always validate
     * packet and fetch data according to packet header.
     */

    while( len-- )
        *val++ = MACGet();
    return len;
}

/**
 * Discard the contents of the current RX buffer
 */
void    MACDiscardRx(void)
{
    PIE1_RCIE = 0;

    if ( RxBuffer.PacketCount )
    {
        /*
         * Once a packet is discarded, current packet becomes next one.
         */
        RxBuffer.CurrentPacket    = RxBuffer.Data[RxBuffer.CurrentPacket];

        RxBuffer.PacketCount--;
    }

    PIE1_RCIE = 1;
}

void    MACPutHeader(MAC_ADDR *remote,
                     BYTE type,
                     WORD dataLen)
{
    TxBuffer.Length         = dataLen;
    TxBuffer.CallerAccess   = 0;
    bIsRxActive             = FALSE;
}

void    MACPut(BYTE val)
{
    if ( bIsRxActive )
    {
        RxBuffer.Data[RxBuffer.CallerAccess++] = val;
        if ( RxBuffer.CallerAccess >= MAX_SLIP_BUFFER_SIZE )
            RxBuffer.CallerAccess = 0;
    }
    else
    {
        TxBuffer.Data[TxBuffer.CallerAccess++] = val;
    }
}

void    MACPutArray(BYTE *val, WORD len)
{
    while( len-- )
        MACPut(*val++);
}


void    MACFlush(void)
{
    TxBuffer.CallerAccess   = 0;
    TxBuffer.IOAccess       = 0;

    if ( !TxBuffer.Flags.bits.bIsModemString )
        TXREG = END;

    PIE1_TXIE = 1;
}

void    MACDiscardTx(BUFFER buffer)
{
    /*
     * If there is no transmission in progress,
     * discard current buffer.
     * This is different than ethernet controller where
     * discarding transmit buffer does not affect
     * transmission that is already in progress.
     */
    if ( !PIE1_TXIE )
    {
        TxBuffer.Length         = 0;
        TxBuffer.CallerAccess   = 0;
        TxBuffer.IOAccess       = 0;
        TxBuffer.Flags.byte     = 0;
    }
}

void    MACSetRxBuffer(WORD offset)
{
    /*
     * Set absolute offset.
     */
    offset += RxBuffer.CurrentPacket;

    /*
     * Skip "next packet" byte.
     */
    offset++;

    /*
     * Wrap the offset if necessary.
     */
    if ( offset >= (BYTE)(MAX_SLIP_BUFFER_SIZE-1) )
        RxBuffer.CallerAccess = (BYTE)(offset - (BYTE)MAX_SLIP_BUFFER_SIZE);
    else
        RxBuffer.CallerAccess = (BYTE)offset;

    bIsRxActive = TRUE;
}

void    MACSetTxBuffer(BUFFER buffer, WORD offset)
{
    TxBuffer.CallerAccess = offset;
    bIsRxActive = FALSE;
}

void    MACReserveTxBuffer(BUFFER buffer)
{
}

WORD    MACGetFreeRxSize(void)
{
    BYTE temp;

    PIE1_RCIE = 0;
    temp = RxBuffer.IOAccess;
    PIE1_RCIE = 1;

    if ( RxBuffer.PacketCount )
    {
        if( temp > RxBuffer.CurrentPacket )
            return ((MAX_SLIP_BUFFER_SIZE -temp) + RxBuffer.CurrentPacket);
        else
            return (RxBuffer.CurrentPacket - temp);
    }
    else
        return (MAX_SLIP_BUFFER_SIZE-2);
}















