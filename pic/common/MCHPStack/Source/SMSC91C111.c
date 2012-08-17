/*********************************************************************
 *
 *                  MAC Module (SMSC LAN91C111) for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        SMSC91C111.c
 * Dependencies:    string.h
 *                  stacktsk.h
 *                  helpers.h
 *                  mac.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -O -Zg -E -C
 *
 *
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     6/6/03 Original        (Rev 1.0)
********************************************************************/
#define THIS_IS_MAC_LAYER

#include <string.h>
#include "stacktsk.h"
#include "helpers.h"
#include "mac.h"
#include "delay.h"


#if defined(STACK_USE_SLIP)
#error Unexpected module is detected.
#error This file must be linked when SLIP module is not in use.
#endif


/*
 * Hardware interface to NIC.
 */
#define NIC_CTRL_TRIS                   (TRISE)
#define NIC_RESET_IO                    (PORTE_RE2)
#define NIC_IOW_IO                      (PORTE_RE1)
#define NIC_IOR_IO                      (PORTE_RE0)
#define NIC_ADDR_IO                     (PORTB)
#define NIC_DATA_IO                     (PORTD)
#define SET_NIC_READ()                  (TRISD = 0xff)
#define SET_NIC_WRITE()                 (TRISD = 0x00)

#define LB(a)                           (a.bits.b0)
#define UB(a)                           (a.bits.b4)


#define ETHER_IP                        (0x00)
#define ETHER_ARP                       (0x06)

/* SMSC LAN91C111 register names and offsets */
#define RCR_LSB                         (0x04)
/* RCR_LSB bits */
#define ALMUL                           (0b00000100)
#define PRMS                            (0b00000010)
#define RX_ABORT                        (0b00000001)
#define RCR_MSB                         (RCR_LSB+1)

/* RCR_MSB bits */
#define SOFT_RST                        (0b10000000)
#define FILT_CAR                        (0b01000000)
#define ABORT_ENB                       (0b00100000)
#define STRIP_CRC                       (0b00000010)
#define RXEN                            (0b00000001)

#define TCR_LSB                         (0x00)

#define EPHSR_LSB                       (0x02)              // Bank 0
#define EPHSR_MSB                       (EPHSR_LSB+1)

#define ECR_LSB                         (0x06)              // Bank 0
#define ECR_MSB                         (ECR_LSB+1)
#define MIR_LSB                         (0x08)              // Bank 0
#define MIR_MSB                         (MIR_LSB+1)

#define IAR0                            (0x04)              // Bank 1
#define IAR1                            (IAR0+1)
#define IAR2                            (IAR0+2)
#define IAR3                            (IAR0+3)
#define IAR4                            (IAR0+4)
#define IAR5                            (IAR0+5)

#define CR_LSB                          (0x01)              // Bank 1
#define CR_MSB                          (CR_LSB+1)
/* CR_MSB bits. */
#define EPH_POWER_EN                    (0b10000000)
#define NO_WAIT                         (0b00010000)
#define GPCNTRL                         (0b00000100)
#define EXT_PHY                         (0b00000010)

#define BAR_LSB                         (0x02)              // Bank 1
#define BAR_MSB                         (BAR_LSB+1)

#define BSR_LSB                         (0x0e)              // Always accessible
/* BSR_LSB bits */
#define BS2                             (0b00000100)
#define BS1                             (0b00000010)
#define BS0                             (0b00000001)

#define CTR_LSB                         (0x0C)              // Bank 1
#define CTR_MSB                         (CTR_LSB+1)

#define RPCR_LSB                        (0x0a)
#define RPCR_MSB                        (RPCR_LSB+1)

#define MMUCR_LSB                       (0x0)               // Bank2
#define MMUCR_MSB                       (MMUCR_LSB+1)

/* Bits 7:5 of MMUCR_LSB */
#define NOOP                            (0b00000000)
#define ALLOCATE_TX                     (0b00100000)
#define MMU_RESET                       (0b01000000)
#define REMOVE_RX                       (0b01100000)
#define REMOVE_N_RELEASE_RX             (0b10000000)
#define RELEASE_SPECIFIC_TX_PACKET      (0b10100000)
#define ENQUE_TX_PACKET                 (0b11000000)
#define RESET_TX_FIFO                   (0b11100000)

#define PNR                             (0x02)              // Bank 2
#define ARR                             (PNR+1)


#define IST                             (0x0C)
#define ACK                             (0x0C)
#define MSK                             (0x0D)              // Bank 2

#define FIFO_LSB                        (0x4)               // Bank 2
#define FIFO_MSB                        (FIFO_LSB+1)

#define PTR_LSB                         (0x06)
#define PTR_MSB                         (PTR_LSB+1)

#define DATA_LSB                        (0x08)

#define MGMT_LSB                        (0x08)              // Bank 3
/* Bit definition of MGMT_LSB */
#define MII_MDOE_HIGH                   (0b00001000)
#define MII_MDOE_LOW                    (0b00000000)
#define MII_MCLK_HIGH                   (0b00000100)
#define MII_MCLK_LOW                    (0b00000000)
#define MII_MDO_HIGH                    (0b00000001)
#define MII_MDO_LOW                     (0b00000000)

#define MT_0                            (0)                 // Bank 4
#define MT_1                            (1)
#define MT_2                            (2)
#define MT_3                            (3)
#define MT_4                            (4)
#define MT_5                            (5)
#define MT_6                            (6)
#define MT_7                            (7)
#define MT_8                            (8)

/* Status and command bit defs for MII access */
#define MII_CMD_READ                    (0b01100000)
#define MII_CMD_WRITE                   (0b01010000)

#define PHY_CONTROL                     (0b00000010)
#define PHY_STATUS                      (0b00000110)
#define PHY_ID1                         (0b00001010)
#define PHY_ID2                         (0b00001110)
#define PHY_ANEG_ADV                    (0b00010010)
#define PHY_ANEG_CAP                    (0b00010110)
#define PHY_CONFIG1                     (0b01000010)
#define PHY_CONFIG2                     (0b01000110)
#define PHY_STATUS_OUT                  (0b01001010)
#define PHY_MASK                        (0b01001110)


#define LEDA_10_100_LINK                (0b00000000)
#define LEDA_10_LINK                    (0b01000000)
#define LEDA_FULL_DUPX                  (0b01100000)
#define LEDA_TX_RX                      (0b10000000)
#define LEDA_100_LINK                   (0b10100000)
#define LEDA_RX                         (0b11000000)
#define LEDA_TX                         (0b11100000)

#define LEDB_10_100_LINK                (0b00000000)
#define LEDB_10_LINK                    (0b00001000)
#define LEDB_FULL_DUPX                  (0b00001100)
#define LEDB_TX_RX                      (0b00010000)
#define LEDB_100_LINK                   (0b00010100)
#define LEDB_RX                         (0b00011000)
#define LEDB_TX                         (0b00011100)


#define LEDA_FUNCTION                   LEDA_10_100_LINK
#define LEDB_FUNCTION                   LEDB_TX_RX



#define SelectBank(b)                   (NICPut(BSR_LSB, b))

#define WRITE_NIC_ADDR(a)               WriteNICAddr(a)

typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    BYTE Val;
} BYTE_VAL;

typedef struct _DATA_BUFFER
{
    struct
    {
        unsigned int bIsFree : 1;
    } flags;
    WORD_VAL length;
} DATA_BUFFER;

static DATA_BUFFER TxBuffer;

typedef struct _ETHER_HEADER
{
    MAC_ADDR        DestMACAddr;
    MAC_ADDR        SourceMACAddr;
    WORD_VAL        Type;
} ETHER_HEADER;



BYTE NICCurrentRdPtr;           // Page that is being read...
BYTE NICCurrentTxBuffer;


static void NICReset(void);
static void NICPut(BYTE reg, BYTE val);
static BYTE NICGet(BYTE reg);
static void WriteNICAddr(BYTE addr);
static void MIIBeginCommand(BYTE command, BYTE reg);
static void PHYPut(BYTE addr, WORD val);
static WORD PHYGet(BYTE addr);
static BYTE MIIGet(BYTE data);
static void MIIPut(BYTE data, BYTE bitCount);
static BUFFER AllocateTxBuffer(void);

void    MACInit(void)
{
    BYTE_VAL tempByte;
    WORD_VAL tempWord;


    TxBuffer.flags.bIsFree = TRUE;

    NICReset();

    DelayMs(2);

    SelectBank(0);
    NICPut(RCR_MSB, SOFT_RST);

    SelectBank(2);
    NICPut(MMUCR_LSB, MMU_RESET);
    /*
     * MMU_RESET releases all FIFOs.  And deallocation may take some time,
     * so wait until is ready.
     */
    do
    {
        tempByte.Val = NICGet(MMUCR_LSB);
    } while( tempByte.bits.b0 );

    // Disable interrupts.
    //SelectBank(2);
    NICPut(MSK, 0x00);

    SelectBank(1);
    do
    {
        tempByte.Val = NICGet(CTR_LSB);
    } while( tempByte.bits.b1 );

    // Ignore bad CRC packets, No auto release transmit buffer
    NICPut(CTR_MSB, 0b00000000);
    // No EEPROM, no Store etc.
    NICPut(CTR_LSB, 0b00000100);


    // Set MAC Address
    //SelectBank(1);
    NICPut(IAR0, MY_MAC_BYTE1);
    NICPut(IAR1, MY_MAC_BYTE2);
    NICPut(IAR2, MY_MAC_BYTE3);
    NICPut(IAR3, MY_MAC_BYTE4);
    NICPut(IAR4, MY_MAC_BYTE5);
    NICPut(IAR5, MY_MAC_BYTE6);

    SelectBank(0);

    // Enable transmitter, enable automatic pad if packet is less than 64 bytes long
    NICPut(TCR_LSB, 0b10000001);
    NICPut(RCR_LSB, 0b00000100);            // Accept all multicast packets.
    NICPut(RCR_MSB, 0b00000001);

    // Set LEDA and LEDB functionality.
    NICPut(RPCR_LSB, LEDA_FUNCTION | LEDB_FUNCTION);
    // Set auto-negotiation mode.
    NICPut(RPCR_MSB, 0b00001000);


    // Reset PHY to restart auto-negotiation
    PHYPut(PHY_CONTROL, 0x8000);
    // Wait for RESET to complete.
    do
    {
        tempWord.Val = PHYGet(PHY_CONTROL);
        tempByte.Val = tempWord.byte.MSB;
    } while( tempByte.bits.b7 );


    // Advertise...
    PHYPut(PHY_ANEG_ADV, 0x01e1);

    // Enable Auto Negotiation and reset at the same time.
    PHYPut(PHY_CONTROL, 0x3200);

    // Majority of registers accessed from now-on is in bank 2, so make sure
    // that bank is set to 2.
    SelectBank(2);

    AllocateTxBuffer();

}


BOOL    MACIsTxReady(void)
{
    BYTE_VAL tempByte;

    SelectBank(0);
    tempByte.Val = NICGet(EPHSR_LSB);
    SelectBank(2);

    return (TxBuffer.flags.bIsFree || tempByte.bits.b0);
}

BOOL MACIsLinked(void)
{
    return TRUE;
}



void    MACPut(BYTE val)
{
    BYTE_VAL tempByte;
    BYTE_VAL tempLSB;

    // Remember LSB so that we can write it as two succesvice writes.
    tempLSB.Val = NICGet(PTR_LSB);

    // Tell MMU that we are writing to current FIFO pointer
    tempByte.Val = NICGet(PTR_MSB);
    tempByte.bits.b5 = 0;
    NICPut(PTR_LSB, tempLSB.Val);
    NICPut(PTR_MSB, tempByte.Val);

    NICPut(DATA_LSB, val);
}


void    MACPutArray(BYTE *val, WORD len)
{
    while(len--)
        NICPut(DATA_LSB, *val++);

}


BYTE    MACGet(void)
{
    BYTE_VAL tempByte;
    BYTE_VAL tempLSB;

    // Tell MMU that we are reading from current FIFO pointer.
    // All writes to PTR must be done LSB followed by MSB.
    tempLSB.Val = NICGet(PTR_LSB);
    tempByte.Val = NICGet(PTR_MSB);
    tempByte.bits.b5 = 1;
    NICPut(PTR_LSB, tempLSB.Val);
    NICPut(PTR_MSB, tempByte.Val);

    return NICGet(DATA_LSB);
}


WORD    MACGetArray(BYTE *val, WORD len)
{
    WORD_VAL t;

    t.Val = len;
    while (len--)
        *val++ = NICGet(DATA_LSB);

    return t.Val;
}

void MACReserveTxBuffer(BUFFER buffer)
{
    TxBuffer.flags.bIsFree = FALSE;
}

static BUFFER AllocateTxBuffer(void)
{
    BYTE_VAL tempByte;

    // All transmit requests results in maximum defined allocation.
#define MAC_TX_BUFFER_PAGES     (((MAC_TX_BUFFER_SIZE & 0xfffe) + 6)/256)

    // First of all allocate predefined packet to transmit data.
    NICPut(MMUCR_LSB, ALLOCATE_TX | (BYTE)MAC_TX_BUFFER_PAGES);

    // Wait for allocation to complete.
    do
    {
        tempByte.Val = NICGet(IST);
    } while( !tempByte.bits.b3 );

    // Read allocated buffer information.
    tempByte.Val = NICGet(ARR);
    if ( tempByte.bits.b7 )
        return INVALID_BUFFER;

    // Remember current transmit buffer packet number.
    NICCurrentTxBuffer = tempByte.Val;

    // Set this as transmit packet.
    NICPut(PNR, NICCurrentTxBuffer);

    // Start from begining of packet, AutoIncr etc.
    // All writes to PTR must be done LSB followed by MSB.
    NICPut(PTR_LSB, 0);
    NICPut(PTR_MSB, 0b01000000);

    return NICCurrentTxBuffer;
}


void MACDiscardTx(BUFFER buffer)
{
    TxBuffer.flags.bIsFree = TRUE;
}

void MACDiscardRx(void)
{
    BYTE_VAL tempByte;


    NICPut(ACK, 0x00);

    // Discard top of RX FIFO.
    NICPut(MMUCR_LSB, REMOVE_N_RELEASE_RX);


    // Wait until release is complete.
    do
    {
        tempByte.Val = NICGet(MMUCR_LSB);
    } while( tempByte.bits.b0 );


}


WORD MACGetFreeRxSize(void)
{
    WORD_VAL tempWord;
    WORD temp;

    SelectBank(0);
    // Value returned by NIC is Multiple of 2KB
    tempWord.byte.LSB = 0;
    tempWord.byte.MSB = NICGet(MIR_MSB);
    // Convert it to bytes.
    tempWord.byte.MSB *= 8;
    SelectBank(2);

    temp = tempWord.Val;

    return tempWord.Val;
}



BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type)
{
    ETHER_HEADER    header;
    BYTE_VAL        tempByte;
    BYTE        temp1, temp2;
    BYTE tempLSB, tempMSB;

    *type = MAC_UNKNOWN;


    // First of all check to see if there is any packet in receive FIFO.
    tempByte.Val = NICGet(FIFO_MSB);
    if ( tempByte.bits.b7 )
        return FALSE;



    // Remember current packet number.
    // There should not be need to clear upper 2 bits -
    // RXEMPTY will always be 0 and so will Reserved bit
    NICCurrentRdPtr = tempByte.Val;

    // Now mark that we are reading RX FIFO and set AUTOINCR.
    // All writes to PTR must be done LSB followed by MSB.
    NICPut(PTR_LSB, 0);
    NICPut(PTR_MSB, 0b11100000);            // RCV, AUTOINCR, READ.

    // Read Status - only interested in MSB; discard LSB
    temp2 = NICGet(DATA_LSB);
    tempByte.Val = NICGet(DATA_LSB);

    // Overrun packet would autoamtically be aborted by MMU, so we don't have
    // to worry about it (?)

    // Fetch and discard byte count of this packet.
    tempLSB = NICGet(DATA_LSB);
    tempMSB = NICGet(DATA_LSB);

    // Process it only if there is no error.
    temp1 = tempByte.Val;
    if ( tempByte.Val & 0b10101000 )
        goto MACGetHeaderDiscard;

    // Now starts the actual ethernet packet.  Read entire header
    MACGetArray((BYTE*)&header, sizeof(header));


    header.Type.Val = swaps(header.Type.Val);

    memcpy((void*)remote->v, (void*)header.SourceMACAddr.v, sizeof(*remote));

    if ( (header.Type.v[1] == 0x08) && ((header.Type.v[0] == ETHER_IP) || (header.Type.v[0] == ETHER_ARP)) )
        *type = header.Type.v[0];


    return TRUE;

MACGetHeaderDiscard:
    MACDiscardRx();
    return FALSE;

}



void    MACPutHeader(MAC_ADDR *remote,
                     BYTE type,
                     WORD dataLen)
{
    WORD_VAL tempWord;
    BYTE_VAL tempByte;
    WORD temp1;
    BYTE etherType;


    // Remember length of this packet so that we can write last control byte
    // at correct offset.
    TxBuffer.length.Val = dataLen + (WORD)sizeof(ETHER_HEADER);

    // Start from begining of packet, AutoIncr etc.
    // All writes to PTR must be done LSB followed by MSB.
    NICPut(PTR_LSB, 0);
    NICPut(PTR_MSB, 0b01000000);

    // Stuff initial housekeeping info.
    // Status word of all 0's
    NICPut(DATA_LSB, 0);
    NICPut(DATA_LSB, 0);

    // Byte count - In words only.
    tempWord.Val = TxBuffer.length.Val;
    tempWord.Val += 6;      // Include status, byte count, and control words
    temp1 = tempWord.Val;
    NICPut(DATA_LSB, tempWord.byte.LSB);
    NICPut(DATA_LSB, tempWord.byte.MSB);

    MACPutArray((BYTE*)remote, sizeof(*remote));

    MACPut(MY_MAC_BYTE1);
    MACPut(MY_MAC_BYTE2);
    MACPut(MY_MAC_BYTE3);
    MACPut(MY_MAC_BYTE4);
    MACPut(MY_MAC_BYTE5);
    MACPut(MY_MAC_BYTE6);

    if ( type == MAC_IP )
        etherType = ETHER_IP;
    else
        etherType = ETHER_ARP;

    MACPut(0x08);
    MACPut(etherType);

}

void MACFlush(void)
{
    // Set TXFIFO pointer to point to end of current tx buffer.
    TxBuffer.length.Val += 4;               // Account for status word and byte count word.

    // All writes to PTR must be done LSB followed by MSB.
    NICPut(PTR_LSB, TxBuffer.length.byte.LSB);
    NICPut(PTR_MSB, 0b01000000 | TxBuffer.length.byte.MSB);

    // Write control byte and extra byte if total buffer length is odd.
    if ( TxBuffer.length.byte.LSB & 0x01 )
        NICPut(DATA_LSB, 0b00100000);       // Control byte - ODD count
    else
    {
        NICPut(DATA_LSB, 0);                // Extra byte - will not be transmitted
        NICPut(DATA_LSB, 0);                // Control byte - even count
    }

    NICPut(PNR, NICCurrentTxBuffer);
    NICPut(MMUCR_LSB, ENQUE_TX_PACKET);
    NICPut(MMUCR_MSB, 0);

    // If we don't have to preserve this packet, wait for it to get
    // transmitted and then release it.
    if ( !TxBuffer.flags.bIsFree )
        TxBuffer.flags.bIsFree = TRUE;

}

static void NICReset(void)
{
    SET_NIC_READ();
    WRITE_NIC_ADDR(0);
    INTCON2_RBPU = 0;

    NIC_IOW_IO = 1;
    NIC_IOR_IO = 1;
    NIC_RESET_IO = 1;
    NIC_CTRL_TRIS = 0x00;

    // Reset pulse must be at least 800 ns.
    Delay10us(1);

    NIC_RESET_IO = 0;
}

static void NICPut(BYTE reg, BYTE val)
{
    WRITE_NIC_ADDR(reg);
    NIC_DATA_IO = val;
    SET_NIC_WRITE();
    NIC_IOW_IO = 0;
    NIC_IOW_IO = 1;
    SET_NIC_READ();
}

static BYTE NICGet(BYTE reg)
{
    BYTE val;

    SET_NIC_READ();
    WRITE_NIC_ADDR(reg);
    NIC_IOR_IO = 0;
    val = NIC_DATA_IO;
    NIC_IOR_IO = 1;
    return val;
}


void MACSetRxBuffer(WORD offset)
{
    WORD_VAL tempWord;
    BYTE_VAL tempByte;

    // Set current packet to current Receive packet number.

    tempWord.byte.MSB = 0;
    tempWord.byte.LSB = sizeof(ETHER_HEADER)+4;
    tempWord.Val += offset;

    tempByte.Val = tempWord.byte.LSB & 0x03;
    tempWord.byte.LSB &= 0xfc;

    NICPut(PTR_LSB, tempWord.byte.LSB);
    NICPut(PTR_MSB, 0b11100000 | tempWord.byte.MSB);

    // Fetch and discard un-aligned data bytes.
    while( tempByte.Val-- )
        NICGet(DATA_LSB);
}

void MACSetTxBuffer(BUFFER buffer, WORD offset)
{
    WORD_VAL tempWord;

    // Make given packet as one that is accessible.
    NICPut(PNR, buffer);

    tempWord.byte.MSB = 0;
    // Account for 4 bytes of status word and byte count word.
    tempWord.byte.LSB = sizeof(ETHER_HEADER)+4;
    tempWord.Val += offset;

    NICPut(PTR_LSB, tempWord.byte.LSB);
    NICPut(PTR_MSB, 0b01000000 | tempWord.byte.MSB);
}

WORD MACGetOffset(void)
{
    WORD_VAL tempWord;

    tempWord.byte.MSB = NICGet(PTR_MSB);
    tempWord.byte.MSB &= 0b00000111;

    tempWord.byte.LSB = NICGet(PTR_LSB);

    return tempWord.Val;
}

static void WriteNICAddr(BYTE addr)
{
    BYTE_VAL tAddr;

    tAddr.Val = addr;

    UB(tAddr) = !LB(tAddr);
    NIC_ADDR_IO = tAddr.Val;
    TRISB = 0xe0;
}




static WORD PHYGet(BYTE addr)
{
    WORD_VAL tempWord;

    MIIBeginCommand(MII_CMD_READ, addr);

    tempWord.byte.MSB = MIIGet(addr);
    tempWord.byte.LSB = MIIGet(addr);

    NICPut(MGMT_LSB, MII_MCLK_LOW);
    NICPut(MGMT_LSB, MII_MCLK_HIGH);

    return tempWord.Val;
}

static void PHYPut(BYTE addr, WORD val)
{
    WORD_VAL tempWord;

    MIIBeginCommand(MII_CMD_WRITE, addr);

    tempWord.Val = val;

    MIIPut(tempWord.byte.MSB, 8);
    MIIPut(tempWord.byte.LSB, 8);

    NICPut(MGMT_LSB, 0);

}

static void MIIBeginCommand(BYTE command, BYTE reg)
{
    BYTE i;

    SelectBank(3);

    // 32 '1's to synchronize MII
    for ( i = 0; i < 32; i++ )
    {
        NICPut(MGMT_LSB, MII_MDOE_HIGH | MII_MCLK_LOW | MII_MDO_HIGH);
            DelayMs(1);
        NICPut(MGMT_LSB, MII_MDOE_HIGH | MII_MCLK_HIGH | MII_MDO_HIGH);
    }

            DelayMs(1);
    MIIPut(command, 8);         // Start(01), Read(10), PhyAdr<4:1)=0000

    // Put PhyAdr<0>, REGAD<4:0>, TA<1:0>
    if ( command == MII_CMD_READ )
    {
        MIIPut(reg, 6);

        // One Z-bit of turnaround time.
        NICPut(MGMT_LSB, MII_MCLK_LOW );
        NICPut(MGMT_LSB, MII_MCLK_HIGH);

    }
    else
        MIIPut(reg, 8);
}

static BYTE MIIGet(BYTE data)
{
    BYTE_VAL temp;
    BYTE_VAL tempVal;
    BYTE i;


    for ( i = 0; i < 8; i++ )
    {
        temp.Val = NICGet(MGMT_LSB);
        if ( temp.bits.b1 )
            tempVal.bits.b0 = 1;
        else
            tempVal.bits.b0 = 0;

        NICPut(MGMT_LSB, MII_MDOE_LOW | MII_MCLK_LOW);
            DelayMs(1);
        NICPut(MGMT_LSB, MII_MDOE_LOW | MII_MCLK_HIGH);
            DelayMs(1);

        tempVal.Val <<= 1;
    }
    return tempVal.Val;
}

static void MIIPut(BYTE data, BYTE bitCount)
{
    BYTE_VAL temp;
    BYTE i;


    temp.Val = data;
    for ( i = 0; i < bitCount; i++ )
    {
        if ( temp.bits.b7 )
        {
            NICPut(MGMT_LSB, MII_MDOE_HIGH | MII_MCLK_LOW | MII_MDO_HIGH);
            DelayMs(1);
            NICPut(MGMT_LSB, MII_MDOE_HIGH | MII_MCLK_HIGH | MII_MDO_HIGH);
        }
        else
        {
            NICPut(MGMT_LSB, MII_MDOE_HIGH | MII_MCLK_LOW | MII_MDO_LOW);
            DelayMs(1);
            NICPut(MGMT_LSB, MII_MDOE_HIGH | MII_MCLK_HIGH | MII_MDO_LOW);
        }
            DelayMs(1);

        temp.Val <<= 1;
    }
}
