/*
Copyright and Disclaimer Notice for MiWi Software:


Microchip Technology Inc. (“Microchip”) licenses this software to you solely for
 use with Microchip products, as described in the license agreement accompanying 
 this software (“License”). The software is owned by Microchip, and is protected 
 under applicable copyright laws.  All rights reserved.

Distribution of this software (in object code or source code) is not permitted, 
except as described in Section 2 of the License.

SOFTWARE IS PROVIDED “AS IS.”  MICROCHIP EXPRESSLY DISCLAIM ANY WARRANTY OF ANY KIND, 
WHETHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT. IN NO EVENT 
SHALL MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
TECHNOLOGY OR SERVICES, ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO 
ANY DEFENSE THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER SIMILAR COSTS.

Software License Agreement

The software supplied herewith by Microchip Technology Incorporated
(the “Company”) for its PICmicro® Microcontroller is intended and
supplied to you, the Company’s customer, for use solely and
exclusively on Microchip PICmicro Microcontroller products. The
software is owned by the Company and/or its supplier, and is
protected under applicable copyright laws. All rights are reserved.
Any use in violation of the foregoing restrictions may subject the
user to criminal sanctions under applicable laws, as well as to
civil liability for the breach of the terms and conditions of this
license.

THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

*/



#if !defined(_ZMRF24J40_H_)
#define _ZMRF24J40_H_

typedef union _MRF24J40_IFS
{
    BYTE Val;
    struct _MRF24J40_IFS_bits
    {
        BYTE RF_TXIF :1;
        BYTE :2;
        BYTE RF_RXIF :1;
        BYTE :4;
    }bits;
} MRF24J40_IFREG;


extern BYTE currentRxByte;
extern BYTE currentTxByte;
extern BYTE phyIsIdle;

//short address registers
//#define RXMCR (0x00)
//#define PANIDL (0x01)
//#define PANIDH (0x02)
//#define SADRL (0x03)
//#define SADRH (0x04)
//#define EADR0 (0x05)
//#define EADR1 (0x06)
//#define EADR2 (0x07)
//#define EADR3 (0x08)
//#define EADR4 (0x09)
//#define EADR5 (0x0a)
//#define EADR6 (0x0b)
//#define EADR7 (0x0c)
//#define RXFLUSH (0x0d)
//#define TXSTATE0 (0x0e)
//#define TXSTATE1 (0x0f)
//#define ORDER (0x10)
//#define TXMCR (0x11)
//#define ACKTMOUT (0x12)
//#define SLALLOC (0x13)
//#define SYMTICKL (0x14)
//#define SYMTICKH (0x15)
//#define PAONTIME (0x16)
//#define PAONSETUP (0x17)
//#define FFOEN (0x18)
//#define CSMACR (0x19)
//#define TXBCNTRIG (0x1a)
//#define TXNMTRIG (0x1b)
//#define TXG1TRIG (0x1c)
//#define TXG2TRIG (0x1d)
//#define ESLOTG23 (0x1e)
//#define ESLOTG45 (0x1f)
//#define ESLOTG67 (0x20)
//#define TXPEND (0x21)
//#define TXBCNINTL (0x22)
//#define FRMOFFSET (0x23)
//#define TXSR (0x24)
//#define TXLERR (0x25)
//#define GATE_CLK (0x26)
//#define TXOFFSET (0x27)
//#define HSYMTMR0 (0x28)
//#define HSYMTMR1 (0x29)
//#define SOFTRST (0x2a)
//#define BISTCR (0x2b)
//#define SECCR0 (0x2c)
//#define SECCR1 (0x2d)
//#define TXPEMISP (0x2e)
//#define SECISR (0x2f)
//#define RXSR (0x30)
//#define ISRSTS (0x31)
//#define INTMSK (0x32)
//#define GPIO (0x33)
//#define GPIODIR (0x34)
//#define SLPACK (0x35)
//#define PWRCTL (0x36)
//#define SECCR2 (0x37)
////#define  (0x38)
//#define BBREG1 (0x39)
//#define BBREG2 (0x3a)
//#define BBREG3 (0x3b)
//#define BBREG4 (0x3c)
//#define BBREG5 (0x3d)
//#define BBREG6 (0x3e)
//#define RSSITHCCA (0x3f)

//long address registers
#define RFCTRL0 (0x200)
#define RFCTRL1 (0x201)
#define RFCTRL2 (0x202)
#define RFCTRL3 (0x203)
#define RFCTRL4 (0x204)
#define RSSI (0x205)
#define RFCTRL6 (0x206)
#define RFCTRL7 (0x207)
#define RFCTRL8 (0x208)
#define CAL1 (0x209)
#define CAL2 (0x20a)
#define CAL3 (0x20b)
#define SFCNTRH (0x20c)
#define SFCNTRM (0x20d)
#define SFCNTRL (0x20e)
#define RFSTATE (0x20f)
#define BATTERYTH (0x210)
#define CLKIRQCR (0x211)
#define SRCADRMODE (0x212)
#define SRCADDR0 (0x213)
#define SRCADDR1 (0x214)
#define SRCADDR2 (0x215)
#define SRCADDR3 (0x216)
#define SRCADDR4 (0x217)
#define SRCADDR5 (0x218)
#define SRCADDR6 (0x219)
#define SRCADDR7 (0x21a)
#define RXFRAMESTATE (0x21b)
#define SECSTATUS (0x21c)
#define STCCMP (0x21d)
#define HLEN (0x21e)
#define FLEN (0x21f)
#define SCLKDIV (0x220)
//#define reserved (0x221)
#define WAKETIMEL (0x222)
#define WAKETIMEH (0x223)
#define TXREMCNTL (0x224)
#define TXREMCNTH (0x225)
#define TXMAINCNTL (0x226)
#define TXMAINCNTM (0x227)
#define TXMAINCNTH0 (0x228)
#define TXMAINCNTH1 (0x229)
#define RFMANUALCTRLEN (0x22a)
#define RFMANUALCTRL (0x22b)
#define RFRXCTRL RFMANUALCTRL
#define TxDACMANUALCTRL (0x22c)
#define RFMANUALCTRL2 (0x22d)
#define TESTRSSI (0x22e)
#define TESTMODE (0x22f)

#define NORMAL_TX_FIFO  (0x000)
#define BEACON_TX_FIFO  (0x080)
#define GTS1_TX_FIFO    (0x100)
#define GTS2_TX_FIFO    (0x180)

#define RX_FIFO         (0x300)

#define SECURITY_FIFO   (0x280)





//short address registers for reading
#define READ_RXMCR (0x00)
#define READ_PANIDL (0x02)
#define READ_PANIDH (0x04)
#define READ_SADRL (0x06)
#define READ_SADRH (0x08)
#define READ_EADR0 (0x0A)
#define READ_EADR1 (0x0C)
#define READ_EADR2 (0x0E)
#define READ_EADR3 (0x10)
#define READ_EADR4 (0x12)
#define READ_EADR5 (0x14)
#define READ_EADR6 (0x16)
#define READ_EADR7 (0x18)
#define READ_RXFLUSH (0x1a)
#define READ_TXSTATE0 (0x1c)
#define READ_TXSTATE1 (0x1e)
#define READ_ORDER (0x20)
#define READ_TXMCR (0x22)
#define READ_ACKTMOUT (0x24)
#define READ_SLALLOC (0x26)
#define READ_SYMTICKL (0x28)
#define READ_SYMTICKH (0x2A)
#define READ_PAONTIME (0x2C)
#define READ_PAONSETUP (0x2E)
#define READ_FFOEN (0x30)
#define READ_CSMACR (0x32)
#define READ_TXBCNTRIG (0x34)
#define READ_TXNMTRIG (0x36)
#define READ_TXG1TRIG (0x38)
#define READ_TXG2TRIG (0x3A)
#define READ_ESLOTG23 (0x3C)
#define READ_ESLOTG45 (0x3E)
#define READ_ESLOTG67 (0x40)
#define READ_TXPEND (0x42)
#define READ_TXBCNINTL (0x44)
#define READ_FRMOFFSET (0x46)
#define READ_TXSR (0x48)
#define READ_TXLERR (0x4A)
#define READ_GATE_CLK (0x4C)
#define READ_TXOFFSET (0x4E)
#define READ_HSYMTMR0 (0x50)
#define READ_HSYMTMR1 (0x52)
#define READ_SOFTRST (0x54)
#define READ_BISTCR (0x56)
#define READ_SECCR0 (0x58)
#define READ_SECCR1 (0x5A)
#define READ_TXPEMISP (0x5C)
#define READ_SECISR (0x5E)
#define READ_RXSR (0x60)
#define READ_ISRSTS (0x62)
#define READ_INTMSK (0x64)
#define READ_GPIO (0x66)
#define READ_GPIODIR (0x68)
#define READ_SLPACK (0x6A)
#define READ_RFCTL (0x6C)
#define READ_SECCR2 (0x6E)
//#define  (0x70)
#define READ_BBREG1 (0x72)
#define READ_BBREG2 (0x74)
#define READ_BBREG3 (0x76)
#define READ_BBREG4 (0x78)
#define READ_BBREG5 (0x7A)
#define READ_BBREG6 (0x7C)
#define READ_RSSITHCCA (0x7E)

//short address registers for writing
//short address registers for reading
#define WRITE_RXMCR (0x01)
#define WRITE_PANIDL (0x03)
#define WRITE_PANIDH (0x05)
#define WRITE_SADRL (0x07)
#define WRITE_SADRH (0x09)
#define WRITE_EADR0 (0x0B)
#define WRITE_EADR1 (0x0D)
#define WRITE_EADR2 (0x0F)
#define WRITE_EADR3 (0x11)
#define WRITE_EADR4 (0x13)
#define WRITE_EADR5 (0x15)
#define WRITE_EADR6 (0x17)
#define WRITE_EADR7 (0x19)
#define WRITE_RXFLUSH (0x1B)
#define WRITE_TXSTATE0 (0x1D)
#define WRITE_TXSTATE1 (0x1F)
#define WRITE_ORDER (0x21)
#define WRITE_TXMCR (0x23)
#define WRITE_ACKTMOUT (0x25)
#define WRITE_SLALLOC (0x27)
#define WRITE_SYMTICKL (0x29)
#define WRITE_SYMTICKH (0x2B)
#define WRITE_PAONTIME (0x2D)
#define WRITE_PAONSETUP (0x2F)
#define WRITE_FFOEN (0x31)
#define WRITE_CSMACR (0x33)
#define WRITE_TXBCNTRIG (0x35)
#define WRITE_TXNMTRIG (0x37)
#define WRITE_TXG1TRIG (0x39)
#define WRITE_TXG2TRIG (0x3B)
#define WRITE_ESLOTG23 (0x3D)
#define WRITE_ESLOTG45 (0x3F)
#define WRITE_ESLOTG67 (0x41)
#define WRITE_TXPEND (0x43)
#define WRITE_TXBCNINTL (0x45)
#define WRITE_FRMOFFSET (0x47)
#define WRITE_TXSR (0x49)
#define WRITE_TXLERR (0x4B)
#define WRITE_GATE_CLK (0x4D)
#define WRITE_TXOFFSET (0x4F)
#define WRITE_HSYMTMR0 (0x51)
#define WRITE_HSYMTMR1 (0x53)
#define WRITE_SOFTRST (0x55)
#define WRITE_BISTCR (0x57)
#define WRITE_SECCR0 (0x59)
#define WRITE_SECCR1 (0x5B)
#define WRITE_TXPEMISP (0x5D)
#define WRITE_SECISR (0x5F)
#define WRITE_RXSR (0x61)
#define WRITE_ISRSTS (0x63)
#define WRITE_INTMSK (0x65)
#define WRITE_GPIO (0x67)
#define WRITE_GPIODIR (0x69)
#define WRITE_SLPACK (0x6B)
#define WRITE_RFCTL (0x6D)
#define WRITE_SECCR2 (0x6F)
#define WRITE_BBREG0 (0x71)
#define WRITE_BBREG1 (0x73)
#define WRITE_BBREG2 (0x75)
#define WRITE_BBREG3 (0x77)
#define WRITE_BBREG4 (0x79)
#define WRITE_BBREG5 (0x7B)
#define WRITE_BBREG6 (0x7D)
#define WRITE_RSSITHCCA (0x7F)

#define CHANNEL_11 0x00
#define CHANNEL_12 0x10
#define CHANNEL_13 0x20
#define CHANNEL_14 0x30
#define CHANNEL_15 0x40
#define CHANNEL_16 0x50
#define CHANNEL_17 0x60
#define CHANNEL_18 0x70
#define CHANNEL_19 0x80
#define CHANNEL_20 0x90
#define CHANNEL_21 0xa0
#define CHANNEL_22 0xb0
#define CHANNEL_23 0xc0
#define CHANNEL_24 0xd0
#define CHANNEL_25 0xe0
#define CHANNEL_26 0xf0

#endif

