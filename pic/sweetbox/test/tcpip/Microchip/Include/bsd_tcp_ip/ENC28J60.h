/*********************************************************************
 *
 *                   ENC28J60 registers/bits
 *
 *********************************************************************
 * FileName:        ENC28J60.h
 * Description: 	Include file for ENC28J60 control registers
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
 * $Id: ENC28J60.h,v 1.5 2006/10/24 16:39:06 C12923 Exp $
 *
 ********************************************************************/

#ifndef ENC28J60_H
#define ENC28J60_H

#include <plib.h>

// Chip Select (Pin 5)
// Direction
#define mSetUpCSDirection()     {   mPORTESetBits(BIT_2); mPORTESetPinsDigitalOut(BIT_2);  }
// CS on, active low
#define mSetCSOn()              {   mPORTEClearBits(BIT_2); }
// CS off, active low
#define mSetCSOff()             {   mPORTESetBits(BIT_2);   }

// Reset (Pin 6)
// Direction
#define mSetUpResetDirection()  {   mPORTESetBits(BIT_3); mPORTESetPinsDigitalOut(BIT_3);  }
// Reset on, active low
#define mSetResetOn()           {   mPORTEClearBits(BIT_3); }
// Reset off, active low
#define mSetResetOff()          {   mPORTESetBits(BIT_3);   }

// Not used
#define mSetUpWOLDirection()    {   mPORTBSetPinsDigitalIn(BIT_15);  }

// INT (pin 28)
// set up the direction, not used
#define mSetUpINTDirection()    {   mPORTASetPinsDigitalIn(BIT_14);  }

// Not used, but required for code
#define mSetSPILatch()          {   /*mPORTBSetPinsDigitalOut(BIT_10 | BIT_11); mPORTBSetBits(BIT_10 | BIT_11); */   }


#ifdef _EXPLORER_16
     
#ifdef TCP_USE_SPI_1

#ifdef TCP_USE_SPI_2
#error "BOTH SPIs Defined: Please define either TCP_USE_SPI1 or TCP_USE_SPI2 in eTCP.def"
#endif

//#define mSetUpCSDirection()     {   mPORTDSetPinsDigitalOut(BIT_14);  }
//#define mSetCSOn()              {   mPORTDClearBits(BIT_14); }
//#define mSetCSOff()             {   mPORTDSetBits(BIT_14);   }
//#define mSetUpResetDirection()  {   mPORTDSetPinsDigitalOut(BIT_15);  }
//#define mSetResetOn()           {   mPORTDClearBits(BIT_15); }
//#define mSetResetOff()          {   mPORTDSetBits(BIT_15);   }
//#define mSetUpWOLDirection()    {   mPORTESetPinsDigitalIn(BIT_8);  }
//#define mSetUpINTDirection()    {   mPORTESetPinsDigitalIn(BIT_9);  }
//#define mSetSPILatch()          {   mPORTBSetPinsDigitalOut(BIT_12 | BIT_13); mPORTBSetBits(BIT_12 | BIT_13);    }

#elif defined TCP_USE_SPI_2

#ifdef TCP_USE_SPI_1
#error "BOTH SPIs Defined: Please define either TCP_USE_SPI1 or TCP_USE_SPI2 in eTCP.def"
#endif

//#define mSetUpCSDirection()     {   mPORTFSetPinsDigitalOut(BIT_12);  }
//#define mSetCSOn()              {   mPORTFClearBits(BIT_12); }
//#define mSetCSOff()             {   mPORTFSetBits(BIT_12);   }
//#define mSetUpResetDirection()  {   mPORTFSetPinsDigitalOut(BIT_13);  }
//#define mSetResetOn()           {   mPORTFClearBits(BIT_13); }
//#define mSetResetOff()          {   mPORTFSetBits(BIT_13);   }
//#define mSetUpWOLDirection()    {   mPORTASetPinsDigitalIn(BIT_14);  }
//#define mSetUpINTDirection()    {   mPORTASetPinsDigitalIn(BIT_15);  }
//#define mSetSPILatch()          

#else
#error "NO SPIs defined: Please define either TCP_USE_SPI1 or TCP_USE_SPI2 in eTCP.def"
#endif
#endif


#ifndef TCP_USE_SPI_1
#ifndef TCP_USE_SPI_2
#error "NO SPIs defined: Please define either TCP_USE_SPI1 or TCP_USE_SPI2 in eTCP.def"
#endif
#endif

#define TXSTATUS_COLLISION_CNT_MASK     0x000F
#define TXSTATUS_CRC_ERR                0x0010
#define TXSTATUS_LEN_CHK_ERR            0x0020
#define TXSTATUS_LEN_OUT_OF_RANGE       0x0040
#define TXSTATUS_DONE                   0x0080
#define TXSTATUS_MULTICAST              0x0100
#define TXSTATUS_BROADCAST              0x0200
#define TXSTATUS_PKT_DEFER              0x0400
#define TXSTATUS_EXCESSIVE_DEFER        0x0800
#define TXSTATUS_MAX_COLLISIONS         0x1000
#define TXSTATUS_LATE_COLLISION         0x2000
#define TXSTATUS_GAINT                  0x4000
#define TXSTATUS_UNDERRUN               0x8000
#define TXSTATUS_CNTRL_FRAME            0x0001
#define TXSTATUS_PAUSE_CNTRL_FRAME      0x0002
#define TXSTATUS_BACK_PRESSURE_APPLIED  0x0004
#define TXSTATUS_VLAN_TAGGED_FRAME      0x0008

typedef union {
	BYTE v[7];
	struct
    {
        WORD    ByteCount;
        WORD    Status1;
        WORD    BytesTransmittedOnWire;
        BYTE    Status2;
    }bits;

} TXSTATUS;

#define RXSTATUS_PREV_IGNORED           0x0001
#define RXSTATUS_RXDC_PREV_SEEN         0x0002
#define RXSTATUS_CARRIER_PREV_SEEN      0x0004
#define RXSTATUS_CODE_VIOLATION         0x0008
#define RXSTATUS_CRC_ERR                0x0010
#define RXSTATUS_LEN_CHK_ERR            0x0020
#define RXSTATUS_LEN_OUT_OF_RANGE       0x0040
#define RXSTATUS_RX_OK                  0x0080
#define RXSTATUS_MULTICAST              0x0100
#define RXSTATUS_BROADCAST              0x0200
#define RXSTATUS_DRIBBLE_NIBBLE         0x0400
#define RXSTATUS_CTRL_FRAME             0x0800
#define RXSTATUS_PAUSE_CTRL_FRAME       0x1000
#define RXSTATUS_UNSUPPORTED_OPCODE     0x2000
#define RXSTATUS_VLAN_TYPE              0x4000
#define RXSTATUS_ZERO                   0x8000

typedef union {
	BYTE v[4];
	struct 
    {
		WORD	ByteCount;
		WORD    Status;
	} bits;
} RXSTATUS;

/******************************************************************************
* Register locations
******************************************************************************/
// Bank 0 registers --------
#define ERDPTL		0x00
#define ERDPTH		0x01
#define EWRPTL		0x02
#define EWRPTH		0x03
#define ETXSTL		0x04
#define ETXSTH		0x05
#define ETXNDL		0x06
#define ETXNDH		0x07
#define ERXSTL		0x08
#define ERXSTH		0x09
#define ERXNDL		0x0A
#define ERXNDH		0x0B
#define ERXRDPTL	0x0C
#define ERXRDPTH	0x0D
#define ERXWRPTL	0x0E
#define ERXWRPTH	0x0F
#define EDMASTL		0x10
#define EDMASTH		0x11
#define EDMANDL		0x12
#define EDMANDH		0x13
#define EDMADSTL	0x14
#define EDMADSTH	0x15
#define EDMACSL		0x16
#define EDMACSH		0x17
//#define			0x18
//#define			0x19
//#define r			0x1A
#define EIE			0x1B
#define EIR			0x1C
#define ESTAT		0x1D
#define ECON2		0x1E
#define ECON1		0x1F

// Bank 1 registers -----
#define EHT0		0x100
#define EHT1		0x101
#define EHT2		0x102
#define EHT3		0x103
#define EHT4		0x104
#define EHT5		0x105
#define EHT6		0x106
#define EHT7		0x107
#define EPMM0		0x108
#define EPMM1		0x109
#define EPMM2		0x10A
#define EPMM3		0x10B
#define EPMM4		0x10C
#define EPMM5		0x10D
#define EPMM6		0x10E
#define EPMM7		0x10F
#define EPMCSL		0x110
#define EPMCSH		0x111
//#define			0x112
//#define			0x113
#define EPMOL		0x114
#define EPMOH		0x115
#define EWOLIE		0x116
#define EWOLIR		0x117
#define ERXFCON		0x118
#define EPKTCNT		0x119
//#define r			0x11A
//#define EIE		0x11B
//#define EIR		0x11C
//#define ESTAT		0x11D
//#define ECON2		0x11E
//#define ECON1		0x11F

// Bank 2 registers -----
#define MACON1		0x200
#define MACON2		0x201
#define MACON3		0x202
#define MACON4		0x203
#define MABBIPG		0x204
//#define			0x205
#define MAIPGL		0x206
#define MAIPGH		0x207
#define MACLCON1	0x208
#define MACLCON2	0x209
#define MAMXFLL		0x20A
#define MAMXFLH		0x20B
//#define r			0x20C
#define MAPHSUP		0x20D
//#define r			0x20E
//#define			0x20F
//#define r			0x210
#define MICON		0x211
#define MICMD		0x212
//#define			0x213
#define MIREGADR	0x214
//#define r			0x215
#define MIWRL		0x216
#define MIWRH		0x217
#define MIRDL		0x218
#define MIRDH		0x219
//#define r			0x21A
//#define EIE		0x21B
//#define EIR		0x21C
//#define ESTAT		0x21D
//#define ECON2		0x21E
//#define ECON1		0x21F

// Bank 3 registers -----
#define MAADR1		0x300
#define MAADR0		0x301
#define MAADR3		0x302
#define MAADR2		0x303
#define MAADR5		0x304
#define MAADR4		0x305
#define EBSTSD		0x306
#define EBSTCON		0x307
#define EBSTCSL		0x308
#define EBSTCSH		0x309
#define MISTAT		0x30A
//#define			0x30B
//#define			0x30C
//#define			0x30D
//#define			0x30E
//#define			0x30F
//#define			0x310
//#define			0x311
#define EREVID		0x312
//#define			0x313
//#define			0x314
#define ECOCON		0x315
//#define 			0x316
#define EFLOCON		0x317
#define EPAUSL		0x318
#define EPAUSH		0x319
//#define r			0x31A
//#define EIE		0x31B
//#define EIR		0x31C
//#define ESTAT		0x31D
//#define ECON2		0x31E
//#define ECON1		0x31F



// Structures
typedef union _REG 
{
	BYTE Val;

	// EIE bits ----------
	struct {
		unsigned char RXERIE:1;
		unsigned char TXERIE:1;
		unsigned char WOLIE:1;
		unsigned char TXIE:1;
		unsigned char LINKIE:1;
		unsigned char DMAIE:1;
		unsigned char PKTIE:1;
		unsigned char INTIE:1;
	} EIEbits;

	// EIR bits ----------
	struct {
		unsigned char RXERIF:1;
		unsigned char TXERIF:1;
		unsigned char WOLIF:1;
		unsigned char TXIF:1;
		unsigned char LINKIF:1;
		unsigned char DMAIF:1;
		unsigned char PKTIF:1;
		unsigned char :1;
	} EIRbits;

	// ESTAT bits ---------
	struct {
		unsigned char CLKRDY:1;
		unsigned char TXABRT:1;
		unsigned char RXBUSY:1;
		unsigned char :1;
		unsigned char LATECOL:1;
		unsigned char :2;
		unsigned char INT:1;
	} ESTATbits;

	// ECON2 bits --------
	struct {
		unsigned char :3;
		unsigned char VRPS:1;
		unsigned char VRTP:1;
		unsigned char PWRSV:1;
		unsigned char PKTDEC:1;
		unsigned char AUTOINC:1;
	} ECON2bits;
		
	// ECON1 bits --------
	struct {
		unsigned char BSEL0:1;
		unsigned char BSEL1:1;
		unsigned char RXEN:1;
		unsigned char TXRTS:1;
		unsigned char CSUMEN:1;
		unsigned char DMAST:1;
		unsigned char RXRST:1;
		unsigned char TXRST:1;
	} ECON1bits;
		
	// EWOLIE bits ------
	struct {
		unsigned char BCWOLIE:1;
		unsigned char MCWOLIE:1;
		unsigned char HTWOLIE:1;
		unsigned char MPWOLIE:1;
		unsigned char PMWOLIE:1;
		unsigned char :1;
		unsigned char AWOLIE:1;
		unsigned char UCWOLIE:1;
	} EWOLIEbits;
		
	// EWOLIR bits ------
	struct {
		unsigned char BCWOLIF:1;
		unsigned char MCWOLIF:1;
		unsigned char HTWOLIF:1;
		unsigned char MPWOLIF:1;
		unsigned char PMWOLIF:1;
		unsigned char :1;
		unsigned char AWOLIF:1;
		unsigned char UCWOLIF:1;
	} EWOLIRbits;
		
	// ERXFCON bits ------
	struct {
		unsigned char BCEN:1;
		unsigned char MCEN:1;
		unsigned char HTEN:1;
		unsigned char MPEN:1;
		unsigned char PMEN:1;
		unsigned char CRCEN:1;
		unsigned char ANDOR:1;
		unsigned char UCEN:1;
	} ERXFCONbits;
		
	// MACON1 bits --------
	struct {
		unsigned char MARXEN:1;
		unsigned char PASSALL:1;
		unsigned char RXPAUS:1;
		unsigned char TXPAUS:1;
		unsigned char LOOPBK:1;
		unsigned char :3;
	} MACON1bits;
		
	// MACON2 bits --------
	struct {
		unsigned char TFUNRST:1;
		unsigned char MATXRST:1;
		unsigned char RFUNRST:1;
		unsigned char MARXRST:1;
		unsigned char :2;
		unsigned char RNDRST:1;
		unsigned char MARST:1;
	} MACON2bits;
		
	// MACON3 bits --------
	struct {
		unsigned char FULDPX:1;
		unsigned char FRMLNEN:1;
		unsigned char HFRMEN:1;
		unsigned char PHDRLEN:1;
		unsigned char TXCRCEN:1;
		unsigned char PADCFG0:1;
		unsigned char PADCFG1:1;
		unsigned char PADCFG2:1;
	} MACON3bits;
	struct {
		unsigned char FULDPX:1;
		unsigned char FRMLNEN:1;
		unsigned char HFRMEN:1;
		unsigned char PHDRLEN:1;
		unsigned char TXCRCEN:1;
		unsigned char PADCFG:3;
	} MACON3bits2;
		
	// MACON4 bits --------
	struct {
		unsigned char PUREPRE:1;
		unsigned char LONGPRE:1;
		unsigned char :2;
		unsigned char NOBKOFF:1;
		unsigned char BPEN:1;
		unsigned char DEFER:1;
		unsigned char :1;
	} MACON4bits;
		
	// MAPHSUP bits ----
	struct {
		unsigned char :3;
		unsigned char RSTRMII:1;
		unsigned char :4;
	} MAPHSUPbits;
		
	// MICON bits --------
	struct {
		unsigned char :7;
		unsigned char RSTMII:1;
	} MICONbits;
		
	// MICMD bits ---------
	struct {
		unsigned char MIIRD:1;
		unsigned char MIISCAN:1;
		unsigned char :6;
	} MICMDbits;

	// EBSTCON bits -----
	struct {
		unsigned char BISTST:1;
		unsigned char TME:1;
		unsigned char TMSEL0:1;
		unsigned char TMSEL1:1;
		unsigned char PSEL:1;
		unsigned char PSV0:1;
		unsigned char PSV1:1;
		unsigned char PSV2:1;
	} EBSTCONbits;
	struct {
		unsigned char BISTST:1;
		unsigned char TME:1;
		unsigned char TMSEL:2;
		unsigned char PSEL:1;
		unsigned char PSV:3;
	} EBSTCONbits2;
		
	// MISTAT bits --------
	struct {
		unsigned char BUSY:1;
		unsigned char SCAN:1;
		unsigned char NVALID:1;
		unsigned char :5;
	} MISTATbits;
		
	// ECOCON bits -------
	struct {
		unsigned char COCON0:1;
		unsigned char COCON1:1;
		unsigned char COCON2:1;
		unsigned char :5;
	} ECOCONbits;
	struct {
		unsigned char COCON:3;
		unsigned char :5;
	} ECOCONbits2;
		
	// EFLOCON bits -----
	struct {
		unsigned char FCEN0:1;
		unsigned char FCEN1:1;
		unsigned char FULDPXS:1;
		unsigned char :5;
	} EFLOCONbits;
	struct {
		unsigned char FCEN:2;
		unsigned char FULDPXS:1;
		unsigned char :5;
	} EFLOCONbits2;
} REG;


/******************************************************************************
* PH Register Locations
******************************************************************************/
#define PHCON1	0x00
#define PHSTAT1	0x01
#define PHID1	0x02
#define PHID2	0x03
#define PHCON2	0x10
#define PHSTAT2	0x11
#define PHIE	0x12
#define PHIR	0x13
#define PHLCON	0x14


typedef union {
	WORD Val;
	WORD_VAL VAL;

	// PHCON1 bits ----------
	struct {
		unsigned :8;
		unsigned PDPXMD:1;
		unsigned :2;
		unsigned PPWRSV:1;
		unsigned :2;
		unsigned PLOOPBK:1;
		unsigned PRST:1;
	} PHCON1bits;

	// PHSTAT1 bits --------
	struct {
		unsigned :1;
		unsigned JBSTAT:1;
		unsigned LLSTAT:1;
		unsigned :5;
		unsigned :3;
		unsigned PHDPX:1;
		unsigned PFDPX:1;
		unsigned :3;
	} PHSTAT1bits;

	// PHID2 bits ----------
	struct {
		unsigned PREV0:1;
		unsigned PREV1:1;
		unsigned PREV2:1;
		unsigned PREV3:1;
		unsigned PPN0:1;
		unsigned PPN1:1;
		unsigned PPN2:1;
		unsigned PPN3:1;
		unsigned PPN4:1;
		unsigned PPN5:1;
		unsigned PID19:1;
		unsigned PID20:1;
		unsigned PID21:1;
		unsigned PID22:1;
		unsigned PID23:1;
		unsigned PID24:1;
	} PHID2bits;
	struct {
		unsigned PREV:4;
		unsigned PPNL:4;
		unsigned PPNH:2;
		unsigned PID:6;
	} PHID2bits2;

	// PHCON2 bits ----------
	struct {
		unsigned :8;
		unsigned HDLDIS:1;
		unsigned :1;
		unsigned JABBER:1;
		unsigned :2;
		unsigned TXDIS:1;
		unsigned FRCLNK:1;
		unsigned :1;
	} PHCON2bits;

	// PHSTAT2 bits --------
	struct {
		unsigned :5;
		unsigned PLRITY:1;
		unsigned :2;
		unsigned :1;
		unsigned DPXSTAT:1;
		unsigned LSTAT:1;
		unsigned COLSTAT:1;
		unsigned RXSTAT:1;
		unsigned TXSTAT:1;
		unsigned :2;
	} PHSTAT2bits;

	// PHIE bits -----------
	struct {
		unsigned :1;
		unsigned PGEIE:1;
		unsigned :2;
		unsigned PLNKIE:1;
		unsigned :3;
		unsigned :8;
	} PHIEbits;

	// PHIR bits -----------
	struct {
		unsigned :2;
		unsigned PGIF:1;
		unsigned :1;
		unsigned PLNKIF:1;
		unsigned :3;
		unsigned :8;
	} PHIRbits;

	// PHLCON bits -------
	struct {
		unsigned :1;
		unsigned STRCH:1;
		unsigned LFRQ0:1;
		unsigned LFRQ1:1;
		unsigned LBCFG0:1;
		unsigned LBCFG1:1;
		unsigned LBCFG2:1;
		unsigned LBCFG3:1;
		unsigned LACFG0:1;
		unsigned LACFG1:1;
		unsigned LACFG2:1;
		unsigned LACFG3:1;
		unsigned :4;
	} PHLCONbits;
	struct {
		unsigned :1;
		unsigned STRCH:1;
		unsigned LFRQ:2;
		unsigned LBCFG:4;
		unsigned LACFG:4;
		unsigned :4;
	} PHLCONbits2;
} PHYREG;


/******************************************************************************
* Individual Register Bits
******************************************************************************/
// ETH/MAC/MII bits

// EIE bits ----------
#define	EIE_INTIE		(1<<7)
#define	EIE_PKTIE		(1<<6)
#define	EIE_DMAIE		(1<<5)
#define	EIE_LINKIE		(1<<4)
#define	EIE_TXIE		(1<<3)
#define	EIE_WOLIE		(1<<2)
#define	EIE_TXERIE		(1<<1)
#define	EIE_RXERIE		(1)

// EIR bits ----------
#define	EIR_PKTIF		(1<<6)
#define	EIR_DMAIF		(1<<5)
#define	EIR_LINKIF		(1<<4)
#define	EIR_TXIF		(1<<3)
#define	EIR_WOLIF		(1<<2)
#define	EIR_TXERIF		(1<<1)
#define	EIR_RXERIF		(1)
	
// ESTAT bits ---------
#define	ESTAT_INT		(1<<7)
#define	ESTAT_LATECOL	(1<<4)
#define	ESTAT_RXBUSY	(1<<2)
#define	ESTAT_TXABRT	(1<<1)
#define	ESTAT_CLKRDY	(1)
	
// ECON2 bits --------
#define	ECON2_AUTOINC	(1<<7)
#define	ECON2_PKTDEC	(1<<6)
#define	ECON2_PWRSV		(1<<5)
#define	ECON2_VRPS		(1<<3)
	
// ECON1 bits --------
#define	ECON1_TXRST		(1<<7)
#define	ECON1_RXRST		(1<<6)
#define	ECON1_DMAST		(1<<5)
#define	ECON1_CSUMEN	(1<<4)
#define	ECON1_TXRTS		(1<<3)
#define	ECON1_RXEN		(1<<2)
#define	ECON1_BSEL1		(1<<1)
#define	ECON1_BSEL0		(1)
	
// EWOLIE bits -------
#define	EWOLIE_UCWOLIE	(1<<7)
#define	EWOLIE_AWOLIE	(1<<6)
#define	EWOLIE_PMWOLIE	(1<<4)
#define	EWOLIE_MPWOLIE	(1<<3)
#define	EWOLIE_HTWOLIE	(1<<2)
#define	EWOLIE_MCWOLIE	(1<<1)
#define	EWOLIE_BCWOLIE	(1)
	
// EWOLIR bits -------
#define	EWOLIR_UCWOLIF	(1<<7)
#define	EWOLIR_AWOLIF	(1<<6)
#define	EWOLIR_PMWOLIF	(1<<4)
#define	EWOLIR_MPWOLIF	(1<<3)
#define	EWOLIR_HTWOLIF	(1<<2)
#define	EWOLIR_MCWOLIF	(1<<1)
#define	EWOLIR_BCWOLIF	(1)
	
// ERXFCON bits ------
#define	ERXFCON_UCEN	(1<<7)
#define	ERXFCON_ANDOR	(1<<6)
#define	ERXFCON_CRCEN	(1<<5)
#define	ERXFCON_PMEN	(1<<4)
#define	ERXFCON_MPEN	(1<<3)
#define	ERXFCON_HTEN	(1<<2)
#define	ERXFCON_MCEN	(1<<1)
#define	ERXFCON_BCEN	(1)
	
// MACON1 bits --------
#define	MACON1_LOOPBK	(1<<4)
#define	MACON1_TXPAUS	(1<<3)
#define	MACON1_RXPAUS	(1<<2)
#define	MACON1_PASSALL	(1<<1)
#define	MACON1_MARXEN	(1)
	
// MACON2 bits --------
#define	MACON2_MARST	(1<<7)
#define	MACON2_RNDRST	(1<<6)
#define	MACON2_MARXRST	(1<<3)
#define	MACON2_RFUNRST	(1<<2)
#define	MACON2_MATXRST	(1<<1)
#define	MACON2_TFUNRST	(1)
	
// MACON3 bits --------
#define	MACON3_PADCFG2	(1<<7)
#define	MACON3_PADCFG1	(1<<6)
#define	MACON3_PADCFG0	(1<<5)
#define	MACON3_TXCRCEN	(1<<4)
#define	MACON3_PHDRLEN	(1<<3)
#define	MACON3_HFRMEN	(1<<2)
#define	MACON3_FRMLNEN	(1<<1)
#define	MACON3_FULDPX	(1)
	
// MACON4 bits --------
#define	MACON4_DEFER	(1<<6)
#define	MACON4_BPEN		(1<<5)
#define	MACON4_NOBKOFF	(1<<4)
#define	MACON4_LONGPRE	(1<<1)
#define	MACON4_PUREPRE	(1)
	
// MAPHSUP bits ----
#define	MAPHSUP_RSTRMII	(1<<3)
	
// MICON bits --------
#define	MICON_RSTMII	(1<<7)
	
// MICMD bits ---------
#define	MICMD_MIISCAN	(1<<1)
#define	MICMD_MIIRD		(1)

// EBSTCON bits -----
#define	EBSTCON_PSV2	(1<<7)
#define	EBSTCON_PSV1	(1<<6)
#define	EBSTCON_PSV0	(1<<5)
#define	EBSTCON_PSEL	(1<<4)
#define	EBSTCON_TMSEL1	(1<<3)
#define	EBSTCON_TMSEL0	(1<<2)
#define	EBSTCON_TME		(1<<1)
#define	EBSTCON_BISTST	(1)

// MISTAT bits --------
#define	MISTAT_NVALID	(1<<2)
#define	MISTAT_SCAN		(1<<1)
#define	MISTAT_BUSY		(1)
	
// ECOCON bits -------
#define	ECOCON_COCON2	(1<<2)
#define	ECOCON_COCON1	(1<<1)
#define	ECOCON_COCON0	(1)
	
// EFLOCON bits -----
#define	EFLOCON_FULDPXS	(1<<2)
#define	EFLOCON_FCEN1	(1<<1)
#define	EFLOCON_FCEN0	(1)



// PHY bits

// PHCON1 bits ----------
#define	PHCON1_PRST		(1ul<<15)
#define	PHCON1_PLOOPBK	(1ul<<14)
#define	PHCON1_PPWRSV	(1ul<<11)
#define	PHCON1_PDPXMD	(1ul<<8)

// PHSTAT1 bits --------
#define	PHSTAT1_PFDPX	(1ul<<12)
#define	PHSTAT1_PHDPX	(1ul<<11)
#define	PHSTAT1_LLSTAT	(1ul<<2)
#define	PHSTAT1_JBSTAT	(1ul<<1)

// PHID2 bits --------
#define	PHID2_PID24		(1ul<<15)
#define	PHID2_PID23		(1ul<<14)
#define	PHID2_PID22		(1ul<<13)
#define	PHID2_PID21		(1ul<<12)
#define	PHID2_PID20		(1ul<<11)
#define	PHID2_PID19		(1ul<<10)
#define	PHID2_PPN5		(1ul<<9)
#define	PHID2_PPN4		(1ul<<8)
#define	PHID2_PPN3		(1ul<<7)
#define	PHID2_PPN2		(1ul<<6)
#define	PHID2_PPN1		(1ul<<5)
#define	PHID2_PPN0		(1ul<<4)
#define	PHID2_PREV3		(1ul<<3)
#define	PHID2_PREV2		(1ul<<2)
#define	PHID2_PREV1		(1ul<<1)
#define	PHID2_PREV0		(1ul)

// PHCON2 bits ----------
#define	PHCON2_FRCLNK	(1ul<<14)
#define	PHCON2_TXDIS	(1ul<<13)
#define	PHCON2_JABBER	(1ul<<10)
#define	PHCON2_HDLDIS	(1ul<<8)

// PHSTAT2 bits --------
#define	PHSTAT2_TXSTAT	(1ul<<13)
#define	PHSTAT2_RXSTAT	(1ul<<12)
#define	PHSTAT2_COLSTAT	(1ul<<11)
#define	PHSTAT2_LSTAT	(1ul<<10)
#define	PHSTAT2_DPXSTAT	(1ul<<9)
#define	PHSTAT2_PLRITY	(1ul<<5)

// PHIE bits -----------
#define	PHIE_PLNKIE		(1ul<<4)
#define	PHIE_PGEIE		(1ul<<1)

// PHIR bits -----------
#define	PHIR_PLNKIF		(1ul<<4)
#define	PHIR_PGIF		(1ul<<2)

// PHLCON bits -------
#define	PHLCON_LACFG3	(1ul<<11)
#define	PHLCON_LACFG2	(1ul<<10)
#define	PHLCON_LACFG1	(1ul<<9)
#define	PHLCON_LACFG0	(1ul<<8)
#define	PHLCON_LBCFG3	(1ul<<7)
#define	PHLCON_LBCFG2	(1ul<<6)
#define	PHLCON_LBCFG1	(1ul<<5)
#define	PHLCON_LBCFG0	(1ul<<4)
#define	PHLCON_LFRQ1	(1ul<<3)
#define	PHLCON_LFRQ0	(1ul<<2)
#define	PHLCON_STRCH	(1ul<<1)

#endif
