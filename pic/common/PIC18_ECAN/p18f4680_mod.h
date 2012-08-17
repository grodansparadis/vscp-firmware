/*-------------------------------------------------------------------------
 * $Id: p18f4680.h,v 1.2.2.5 2004/05/18 17:50:28 curtiss Exp $
 * MPLAB-Cxx  PIC18F4680 processor header
 *
 * (c) Copyright 1999-2004 Microchip Technology, All rights reserved
 *-------------------------------------------------------------------------*/

#ifndef __18F4680_H
#define __18F4680_H

extern volatile far  unsigned char       RXF6SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF6SIDHbits;
extern volatile far  unsigned char       RXF6SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF6SIDLbits;
extern volatile far  unsigned char       RXF6EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF6EIDHbits;
extern volatile far  unsigned char       RXF6EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF6EIDLbits;
extern volatile far  unsigned char       RXF7SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF7SIDHbits;
extern volatile far  unsigned char       RXF7SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF7SIDLbits;
extern volatile far  unsigned char       RXF7EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF7EIDHbits;
extern volatile far  unsigned char       RXF7EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF7EIDLbits;
extern volatile far  unsigned char       RXF8SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF8SIDHbits;
extern volatile far  unsigned char       RXF8SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF8SIDLbits;
extern volatile far  unsigned char       RXF8EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF8EIDHbits;
extern volatile far  unsigned char       RXF8EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF8EIDLbits;
extern volatile far  unsigned char       RXF9SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF9SIDHbits;
extern volatile far  unsigned char       RXF9SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF9SIDLbits;
extern volatile far  unsigned char       RXF9EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF9EIDHbits;
extern volatile far  unsigned char       RXF9EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF9EIDLbits;
extern volatile far  unsigned char       RXF10SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF10SIDHbits;
extern volatile far  unsigned char       RXF10SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF10SIDLbits;
extern volatile far  unsigned char       RXF10EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF10EIDHbits;
extern volatile far  unsigned char       RXF10EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF10EIDLbits;
extern volatile far  unsigned char       RXF11SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF11SIDHbits;
extern volatile far  unsigned char       RXF11SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF11SIDLbits;
extern volatile far  unsigned char       RXF11EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF11EIDHbits;
extern volatile far  unsigned char       RXF11EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF11EIDLbits;
extern volatile far  unsigned char       RXF12SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF12SIDHbits;
extern volatile far  unsigned char       RXF12SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF12SIDLbits;
extern volatile far  unsigned char       RXF12EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF12EIDHbits;
extern volatile far  unsigned char       RXF12EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF12EIDLbits;
extern volatile far  unsigned char       RXF13SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF13SIDHbits;
extern volatile far  unsigned char       RXF13SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF13SIDLbits;
extern volatile far  unsigned char       RXF13EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF13EIDHbits;
extern volatile far  unsigned char       RXF13EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF13EIDLbits;
extern volatile far  unsigned char       RXF14SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF14SIDHbits;
extern volatile far  unsigned char       RXF14SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF14SIDLbits;
extern volatile far  unsigned char       RXF14EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF14EIDHbits;
extern volatile far  unsigned char       RXF14EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF14EIDLbits;
extern volatile far  unsigned char       RXF15SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF15SIDHbits;
extern volatile far  unsigned char       RXF15SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXF15SIDLbits;
extern volatile far  unsigned char       RXF15EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF15EIDHbits;
extern volatile far  unsigned char       RXF15EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF15EIDLbits;
extern volatile far  unsigned char       RXFCON0;
extern volatile far  struct {
  unsigned RXF0EN:1;
  unsigned RXF1EN:1;
  unsigned RXF2EN:1;
  unsigned RXF3EN:1;
  unsigned RXF4EN:1;
  unsigned RXF5EN:1;
  unsigned RXF6EN:1;
  unsigned RXF7EN:1;
} RXFCON0bits;
extern volatile far  unsigned char       RXFCON1;
extern volatile far  struct {
  unsigned RXF8EN:1;
  unsigned RXF9EN:1;
  unsigned RXF10EN:1;
  unsigned RXF11EN:1;
  unsigned RXF12EN:1;
  unsigned RXF13EN:1;
  unsigned RXF14EN:1;
  unsigned RXF15EN:1;
} RXFCON1bits;
extern volatile far  unsigned char       SDFLC;
extern volatile far  struct {
  unsigned DFLC0:1;
  unsigned DFLC1:1;
  unsigned DFLC2:1;
  unsigned DFLC3:1;
  unsigned DFLC4:1;
} SDFLCbits;
extern volatile far  unsigned char       RXFBCON0;
extern volatile far  struct {
  unsigned F0BP_0:1;
  unsigned F0BP_1:1;
  unsigned F0BP_2:1;
  unsigned F0BP_3:1;
  unsigned F1BP_0:1;
  unsigned F1BP_1:1;
  unsigned F1BP_2:1;
  unsigned F1BP_3:1;
} RXFBCON0bits;
extern volatile far  unsigned char       RXFBCON1;
extern volatile far  struct {
  unsigned F2BP_0:1;
  unsigned F2BP_1:1;
  unsigned F2BP_2:1;
  unsigned F2BP_3:1;
  unsigned F3BP_0:1;
  unsigned F3BP_1:1;
  unsigned F3BP_2:1;
  unsigned F3BP_3:1;
} RXFBCON1bits;
extern volatile far  unsigned char       RXFBCON2;
extern volatile far  struct {
  unsigned F4BP_0:1;
  unsigned F4BP_1:1;
  unsigned F4BP_2:1;
  unsigned F4BP_3:1;
  unsigned F5BP_0:1;
  unsigned F5BP_1:1;
  unsigned F5BP_2:1;
  unsigned F5BP_3:1;
} RXFBCON2bits;
extern volatile far  unsigned char       RXFBCON3;
extern volatile far  struct {
  unsigned F6BP_0:1;
  unsigned F6BP_1:1;
  unsigned F6BP_2:1;
  unsigned F6BP_3:1;
  unsigned F7BP_0:1;
  unsigned F7BP_1:1;
  unsigned F7BP_2:1;
  unsigned F7BP_3:1;
} RXFBCON3bits;
extern volatile far  unsigned char       RXFBCON4;
extern volatile far  struct {
  unsigned F8BP_0:1;
  unsigned F8BP_1:1;
  unsigned F8BP_2:1;
  unsigned F8BP_3:1;
  unsigned F9BP_0:1;
  unsigned F9BP_1:1;
  unsigned F9BP_2:1;
  unsigned F9BP_3:1;
} RXFBCON4bits;
extern volatile far  unsigned char       RXFBCON5;
extern volatile far  struct {
  unsigned F10BP_0:1;
  unsigned F10BP_1:1;
  unsigned F10BP_2:1;
  unsigned F10BP_3:1;
  unsigned F11BP_0:1;
  unsigned F11BP_1:1;
  unsigned F11BP_2:1;
  unsigned F11BP_3:1;
} RXFBCON5bits;
extern volatile far  unsigned char       RXFBCON6;
extern volatile far  struct {
  unsigned F12BP_0:1;
  unsigned F12BP_1:1;
  unsigned F12BP_2:1;
  unsigned F12BP_3:1;
  unsigned F13BP_0:1;
  unsigned F13BP_1:1;
  unsigned F13BP_2:1;
  unsigned F13BP_3:1;
} RXFBCON6bits;
extern volatile far  unsigned char       RXFBCON7;
extern volatile far  struct {
  unsigned F14BP_0:1;
  unsigned F14BP_1:1;
  unsigned F14BP_2:1;
  unsigned F14BP_3:1;
  unsigned F15BP_0:1;
  unsigned F15BP_1:1;
  unsigned F15BP_2:1;
  unsigned F15BP_3:1;
} RXFBCON7bits;
extern volatile far  unsigned char       MSEL0;
extern volatile far  struct {
  unsigned FIL0_0:1;
  unsigned FIL0_1:1;
  unsigned FIL1_0:1;
  unsigned FIL1_1:1;
  unsigned FIL2_0:1;
  unsigned FIL2_1:1;
  unsigned FIL3_0:1;
  unsigned FIL3_1:1;
} MSEL0bits;
extern volatile far  unsigned char       MSEL1;
extern volatile far  struct {
  unsigned FIL4_0:1;
  unsigned FIL4_1:1;
  unsigned FIL5_0:1;
  unsigned FIL5_1:1;
  unsigned FIL6_0:1;
  unsigned FIL6_1:1;
  unsigned FIL7_0:1;
  unsigned FIL7_1:1;
} MSEL1bits;
extern volatile far  unsigned char       MSEL2;
extern volatile far  struct {
  unsigned FIL8_0:1;
  unsigned FIL8_1:1;
  unsigned FIL9_0:1;
  unsigned FIL9_1:1;
  unsigned FIL10_0:1;
  unsigned FIL10_1:1;
  unsigned FIL11_0:1;
  unsigned FIL11_1:1;
} MSEL2bits;
extern volatile far  unsigned char       MSEL3;
extern volatile far  struct {
  unsigned FIL12_0:1;
  unsigned FIL12_1:1;
  unsigned FIL13_0:1;
  unsigned FIL13_1:1;
  unsigned FIL14_0:1;
  unsigned FIL14_1:1;
  unsigned FIL15_0:1;
  unsigned FIL15_1:1;
} MSEL3bits;
extern volatile far  unsigned char       BSEL0;
extern volatile far  struct {
  unsigned :2;
  unsigned B0TXEN:1;
  unsigned B1TXEN:1;
  unsigned B2TXEN:1;
  unsigned B3TXEN:1;
  unsigned B4TXEN:1;
  unsigned B5TXEN:1;
} BSEL0bits;
extern volatile far  unsigned char       BIE0;
extern volatile far  struct {
  unsigned RXB0IE:1;
  unsigned RXB1IE:1;
  unsigned B0IE:1;
  unsigned B1IE:1;
  unsigned B2IE:1;
  unsigned B3IE:1;
  unsigned B4IE:1;
  unsigned B5IE:1;
} BIE0bits;
extern volatile far  unsigned char       TXBIE;
extern volatile far  struct {
  unsigned :2;
  unsigned TXB0IE:1;
  unsigned TXB1IE:1;
  unsigned TXB2IE:1;
} TXBIEbits;
extern volatile far  unsigned char       B0CON;
extern volatile far  union {
  struct {
    unsigned FILHIT0:1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned RTREN:1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIF:1;
  };
} B0CONbits;
extern volatile far  unsigned char       B0SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} B0SIDHbits;
extern volatile far  unsigned char       B0SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXID:1;
    unsigned SRR:1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDE:1;
  };
} B0SIDLbits;
extern volatile far  unsigned char       B0EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} B0EIDHbits;
extern volatile far  unsigned char       B0EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} B0EIDLbits;
extern volatile far  unsigned char       B0DLC;
extern volatile far  union {
  struct {
    unsigned DLC0:1;
    unsigned DLC1:1;
    unsigned DLC2:1;
    unsigned DLC3:1;
    unsigned RESRB0:1;
    unsigned RESRB1:1;
    unsigned RXRTR:1;
  };
  struct {
    unsigned :6;
    unsigned TXRTR:1;
  };
} B0DLCbits;
extern volatile far  unsigned char       B0D0;
extern volatile far  struct {
  unsigned B0D00:1;
  unsigned B0D01:1;
  unsigned B0D02:1;
  unsigned B0D03:1;
  unsigned B0D04:1;
  unsigned B0D05:1;
  unsigned B0D06:1;
  unsigned B0D07:1;
} B0D0bits;
extern volatile far  unsigned char       B0D1;
extern volatile far  struct {
  unsigned B0D10:1;
  unsigned B0D11:1;
  unsigned B0D12:1;
  unsigned B0D13:1;
  unsigned B0D14:1;
  unsigned B0D15:1;
  unsigned B0D16:1;
  unsigned B0D17:1;
} B0D1bits;
extern volatile far  unsigned char       B0D2;
extern volatile far  struct {
  unsigned B0D20:1;
  unsigned B0D21:1;
  unsigned B0D22:1;
  unsigned B0D23:1;
  unsigned B0D24:1;
  unsigned B0D25:1;
  unsigned B0D26:1;
  unsigned B0D27:1;
} B0D2bits;
extern volatile far  unsigned char       B0D3;
extern volatile far  struct {
  unsigned B0D30:1;
  unsigned B0D31:1;
  unsigned B0D32:1;
  unsigned B0D33:1;
  unsigned B0D34:1;
  unsigned B0D35:1;
  unsigned B0D36:1;
  unsigned B0D37:1;
} B0D3bits;
extern volatile far  unsigned char       B0D4;
extern volatile far  struct {
  unsigned B0D40:1;
  unsigned B0D41:1;
  unsigned B0D42:1;
  unsigned B0D43:1;
  unsigned B0D44:1;
  unsigned B0D45:1;
  unsigned B0D46:1;
  unsigned B0D47:1;
} B0D4bits;
extern volatile far  unsigned char       B0D5;
extern volatile far  struct {
  unsigned B0D50:1;
  unsigned B0D51:1;
  unsigned B0D52:1;
  unsigned B0D53:1;
  unsigned B0D54:1;
  unsigned B0D55:1;
  unsigned B0D56:1;
  unsigned B0D57:1;
} B0D5bits;
extern volatile far  unsigned char       B0D6;
extern volatile far  struct {
  unsigned B0D60:1;
  unsigned B0D61:1;
  unsigned B0D62:1;
  unsigned B0D63:1;
  unsigned B0D64:1;
  unsigned B0D65:1;
  unsigned B0D66:1;
  unsigned B0D67:1;
} B0D6bits;
extern volatile far  unsigned char       B0D7;
extern volatile far  struct {
  unsigned B0D70:1;
  unsigned B0D71:1;
  unsigned B0D72:1;
  unsigned B0D73:1;
  unsigned B0D74:1;
  unsigned B0D75:1;
  unsigned B0D76:1;
  unsigned B0D77:1;
} B0D7bits;
extern volatile far  unsigned char       CANSTAT_RO9;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO9bits;
extern volatile far  unsigned char       CANCON_RO9;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO9bits;
extern volatile far  unsigned char       B1CON;
extern volatile far  union {
  struct {
    unsigned FILHIT0:1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned RTREN:1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIF:1;
  };
} B1CONbits;
extern volatile far  unsigned char       B1SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} B1SIDHbits;
extern volatile far  unsigned char       B1SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXID:1;
    unsigned SRR:1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDE:1;
  };
} B1SIDLbits;
extern volatile far  unsigned char       B1EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} B1EIDHbits;
extern volatile far  unsigned char       B1EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} B1EIDLbits;
extern volatile far  unsigned char       B1DLC;
extern volatile far  union {
  struct {
    unsigned DLC0:1;
    unsigned DLC1:1;
    unsigned DLC2:1;
    unsigned DLC3:1;
    unsigned RESRB0:1;
    unsigned RESRB1:1;
    unsigned RXRTR:1;
  };
  struct {
    unsigned :6;
    unsigned TXRTR:1;
  };
} B1DLCbits;
extern volatile far  unsigned char       B1D0;
extern volatile far  struct {
  unsigned B1D00:1;
  unsigned B1D01:1;
  unsigned B1D02:1;
  unsigned B1D03:1;
  unsigned B1D04:1;
  unsigned B1D05:1;
  unsigned B1D06:1;
  unsigned B1D07:1;
} B1D0bits;
extern volatile far  unsigned char       B1D1;
extern volatile far  struct {
  unsigned B1D10:1;
  unsigned B1D11:1;
  unsigned B1D12:1;
  unsigned B1D13:1;
  unsigned B1D14:1;
  unsigned B1D15:1;
  unsigned B1D16:1;
  unsigned B1D17:1;
} B1D1bits;
extern volatile far  unsigned char       B1D2;
extern volatile far  struct {
  unsigned B1D20:1;
  unsigned B1D21:1;
  unsigned B1D22:1;
  unsigned B1D23:1;
  unsigned B1D24:1;
  unsigned B1D25:1;
  unsigned B1D26:1;
  unsigned B1D27:1;
} B1D2bits;
extern volatile far  unsigned char       B1D3;
extern volatile far  struct {
  unsigned B1D30:1;
  unsigned B1D31:1;
  unsigned B1D32:1;
  unsigned B1D33:1;
  unsigned B1D34:1;
  unsigned B1D35:1;
  unsigned B1D36:1;
  unsigned B1D37:1;
} B1D3bits;
extern volatile far  unsigned char       B1D4;
extern volatile far  struct {
  unsigned B1D40:1;
  unsigned B1D41:1;
  unsigned B1D42:1;
  unsigned B1D43:1;
  unsigned B1D44:1;
  unsigned B1D45:1;
  unsigned B1D46:1;
  unsigned B1D47:1;
} B1D4bits;
extern volatile far  unsigned char       B1D5;
extern volatile far  struct {
  unsigned B1D50:1;
  unsigned B1D51:1;
  unsigned B1D52:1;
  unsigned B1D53:1;
  unsigned B1D54:1;
  unsigned B1D55:1;
  unsigned B1D56:1;
  unsigned B1D57:1;
} B1D5bits;
extern volatile far  unsigned char       B1D6;
extern volatile far  struct {
  unsigned B1D60:1;
  unsigned B1D61:1;
  unsigned B1D62:1;
  unsigned B1D63:1;
  unsigned B1D64:1;
  unsigned B1D65:1;
  unsigned B1D66:1;
  unsigned B1D67:1;
} B1D6bits;
extern volatile far  unsigned char       B1D7;
extern volatile far  struct {
  unsigned B1D70:1;
  unsigned B1D71:1;
  unsigned B1D72:1;
  unsigned B1D73:1;
  unsigned B1D74:1;
  unsigned B1D75:1;
  unsigned B1D76:1;
  unsigned B1D77:1;
} B1D7bits;
extern volatile far  unsigned char       CANSTAT_RO8;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO8bits;
extern volatile far  unsigned char       CANCON_RO8;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO8bits;
extern volatile far  unsigned char       B2CON;
extern volatile far  union {
  struct {
    unsigned FILHIT0:1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned RTREN:1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIF:1;
  };
} B2CONbits;
extern volatile far  unsigned char       B2SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} B2SIDHbits;
extern volatile far  unsigned char       B2SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXID:1;
    unsigned SRR:1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDE:1;
  };
} B2SIDLbits;
extern volatile far  unsigned char       B2EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} B2EIDHbits;
extern volatile far  unsigned char       B2EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} B2EIDLbits;
extern volatile far  unsigned char       B2DLC;
extern volatile far  union {
  struct {
    unsigned DLC0:1;
    unsigned DLC1:1;
    unsigned DLC2:1;
    unsigned DLC3:1;
    unsigned RESRB0:1;
    unsigned RESRB1:1;
    unsigned RXRTR:1;
  };
  struct {
    unsigned :6;
    unsigned TXRTR:1;
  };
} B2DLCbits;
extern volatile far  unsigned char       B2D0;
extern volatile far  struct {
  unsigned B2D00:1;
  unsigned B2D01:1;
  unsigned B2D02:1;
  unsigned B2D03:1;
  unsigned B2D04:1;
  unsigned B2D05:1;
  unsigned B2D06:1;
  unsigned B2D07:1;
} B2D0bits;
extern volatile far  unsigned char       B2D1;
extern volatile far  struct {
  unsigned B2D10:1;
  unsigned B2D11:1;
  unsigned B2D12:1;
  unsigned B2D13:1;
  unsigned B2D14:1;
  unsigned B2D15:1;
  unsigned B2D16:1;
  unsigned B2D17:1;
} B2D1bits;
extern volatile far  unsigned char       B2D2;
extern volatile far  struct {
  unsigned B2D20:1;
  unsigned B2D21:1;
  unsigned B2D22:1;
  unsigned B2D23:1;
  unsigned B2D24:1;
  unsigned B2D25:1;
  unsigned B2D26:1;
  unsigned B2D27:1;
} B2D2bits;
extern volatile far  unsigned char       B2D3;
extern volatile far  struct {
  unsigned B2D30:1;
  unsigned B2D31:1;
  unsigned B2D32:1;
  unsigned B2D33:1;
  unsigned B2D34:1;
  unsigned B2D35:1;
  unsigned B2D36:1;
  unsigned B2D37:1;
} B2D3bits;
extern volatile far  unsigned char       B2D4;
extern volatile far  struct {
  unsigned B2D40:1;
  unsigned B2D41:1;
  unsigned B2D42:1;
  unsigned B2D43:1;
  unsigned B2D44:1;
  unsigned B2D45:1;
  unsigned B2D46:1;
  unsigned B2D47:1;
} B2D4bits;
extern volatile far  unsigned char       B2D5;
extern volatile far  struct {
  unsigned B2D50:1;
  unsigned B2D51:1;
  unsigned B2D52:1;
  unsigned B2D53:1;
  unsigned B2D54:1;
  unsigned B2D55:1;
  unsigned B2D56:1;
  unsigned B2D57:1;
} B2D5bits;
extern volatile far  unsigned char       B2D6;
extern volatile far  struct {
  unsigned B2D60:1;
  unsigned B2D61:1;
  unsigned B2D62:1;
  unsigned B2D63:1;
  unsigned B2D64:1;
  unsigned B2D65:1;
  unsigned B2D66:1;
  unsigned B2D67:1;
} B2D6bits;
extern volatile far  unsigned char       B2D7;
extern volatile far  struct {
  unsigned B2D70:1;
  unsigned B2D71:1;
  unsigned B2D72:1;
  unsigned B2D73:1;
  unsigned B2D74:1;
  unsigned B2D75:1;
  unsigned B2D76:1;
  unsigned B2D77:1;
} B2D7bits;
extern volatile far  unsigned char       CANSTAT_RO7;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO7bits;
extern volatile far  unsigned char       CANCON_RO7;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO7bits;
extern volatile far  unsigned char       B3CON;
extern volatile far  union {
  struct {
    unsigned FILHIT0:1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned RTREN:1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIF:1;
  };
} B3CONbits;
extern volatile far  unsigned char       B3SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} B3SIDHbits;
extern volatile far  unsigned char       B3SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXID:1;
    unsigned SRR:1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDE:1;
  };
} B3SIDLbits;
extern volatile far  unsigned char       B3EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} B3EIDHbits;
extern volatile far  unsigned char       B3EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} B3EIDLbits;
extern volatile far  unsigned char       B3DLC;
extern volatile far  union {
  struct {
    unsigned DLC0:1;
    unsigned DLC1:1;
    unsigned DLC2:1;
    unsigned DLC3:1;
    unsigned RESRB0:1;
    unsigned RESRB1:1;
    unsigned RXRTR:1;
  };
  struct {
    unsigned :6;
    unsigned TXRTR:1;
  };
} B3DLCbits;
extern volatile far  unsigned char       B3D0;
extern volatile far  struct {
  unsigned B3D00:1;
  unsigned B3D01:1;
  unsigned B3D02:1;
  unsigned B3D03:1;
  unsigned B3D04:1;
  unsigned B3D05:1;
  unsigned B3D06:1;
  unsigned B3D07:1;
} B3D0bits;
extern volatile far  unsigned char       B3D1;
extern volatile far  struct {
  unsigned B3D10:1;
  unsigned B3D11:1;
  unsigned B3D12:1;
  unsigned B3D13:1;
  unsigned B3D14:1;
  unsigned B3D15:1;
  unsigned B3D16:1;
  unsigned B3D17:1;
} B3D1bits;
extern volatile far  unsigned char       B3D2;
extern volatile far  struct {
  unsigned B3D20:1;
  unsigned B3D21:1;
  unsigned B3D22:1;
  unsigned B3D23:1;
  unsigned B3D24:1;
  unsigned B3D25:1;
  unsigned B3D26:1;
  unsigned B3D27:1;
} B3D2bits;
extern volatile far  unsigned char       B3D3;
extern volatile far  struct {
  unsigned B3D30:1;
  unsigned B3D31:1;
  unsigned B3D32:1;
  unsigned B3D33:1;
  unsigned B3D34:1;
  unsigned B3D35:1;
  unsigned B3D36:1;
  unsigned B3D37:1;
} B3D3bits;
extern volatile far  unsigned char       B3D4;
extern volatile far  struct {
  unsigned B3D40:1;
  unsigned B3D41:1;
  unsigned B3D42:1;
  unsigned B3D43:1;
  unsigned B3D44:1;
  unsigned B3D45:1;
  unsigned B3D46:1;
  unsigned B3D47:1;
} B3D4bits;
extern volatile far  unsigned char       B3D5;
extern volatile far  struct {
  unsigned B3D50:1;
  unsigned B3D51:1;
  unsigned B3D52:1;
  unsigned B3D53:1;
  unsigned B3D54:1;
  unsigned B3D55:1;
  unsigned B3D56:1;
  unsigned B3D57:1;
} B3D5bits;
extern volatile far  unsigned char       B3D6;
extern volatile far  struct {
  unsigned B3D60:1;
  unsigned B3D61:1;
  unsigned B3D62:1;
  unsigned B3D63:1;
  unsigned B3D64:1;
  unsigned B3D65:1;
  unsigned B3D66:1;
  unsigned B3D67:1;
} B3D6bits;
extern volatile far  unsigned char       B3D7;
extern volatile far  struct {
  unsigned B3D70:1;
  unsigned B3D71:1;
  unsigned B3D72:1;
  unsigned B3D73:1;
  unsigned B3D74:1;
  unsigned B3D75:1;
  unsigned B3D76:1;
  unsigned B3D77:1;
} B3D7bits;
extern volatile far  unsigned char       CANSTAT_RO6;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO6bits;
extern volatile far  unsigned char       CANCON_RO6;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO6bits;
extern volatile far  unsigned char       B4CON;
extern volatile far  union {
  struct {
    unsigned FILHIT0:1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned RTREN:1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIF:1;
  };
} B4CONbits;
extern volatile far  unsigned char       B4SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} B4SIDHbits;
extern volatile far  unsigned char       B4SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXID:1;
    unsigned SRR:1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDE:1;
  };
} B4SIDLbits;
extern volatile far  unsigned char       B4EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} B4EIDHbits;
extern volatile far  unsigned char       B4EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} B4EIDLbits;
extern volatile far  unsigned char       B4DLC;
extern volatile far  union {
  struct {
    unsigned DLC0:1;
    unsigned DLC1:1;
    unsigned DLC2:1;
    unsigned DLC3:1;
    unsigned RESRB0:1;
    unsigned RESRB1:1;
    unsigned RXRTR:1;
  };
  struct {
    unsigned :6;
    unsigned TXRTR:1;
  };
} B4DLCbits;
extern volatile far  unsigned char       B4D0;
extern volatile far  struct {
  unsigned B4D00:1;
  unsigned B4D01:1;
  unsigned B4D02:1;
  unsigned B4D03:1;
  unsigned B4D04:1;
  unsigned B4D05:1;
  unsigned B4D06:1;
  unsigned B4D07:1;
} B4D0bits;
extern volatile far  unsigned char       B4D1;
extern volatile far  struct {
  unsigned B4D10:1;
  unsigned B4D11:1;
  unsigned B4D12:1;
  unsigned B4D13:1;
  unsigned B4D14:1;
  unsigned B4D15:1;
  unsigned B4D16:1;
  unsigned B4D17:1;
} B4D1bits;
extern volatile far  unsigned char       B4D2;
extern volatile far  struct {
  unsigned B4D20:1;
  unsigned B4D21:1;
  unsigned B4D22:1;
  unsigned B4D23:1;
  unsigned B4D24:1;
  unsigned B4D25:1;
  unsigned B4D26:1;
  unsigned B4D27:1;
} B4D2bits;
extern volatile far  unsigned char       B4D3;
extern volatile far  struct {
  unsigned B4D30:1;
  unsigned B4D31:1;
  unsigned B4D32:1;
  unsigned B4D33:1;
  unsigned B4D34:1;
  unsigned B4D35:1;
  unsigned B4D36:1;
  unsigned B4D37:1;
} B4D3bits;
extern volatile far  unsigned char       B4D4;
extern volatile far  struct {
  unsigned B4D40:1;
  unsigned B4D41:1;
  unsigned B4D42:1;
  unsigned B4D43:1;
  unsigned B4D44:1;
  unsigned B4D45:1;
  unsigned B4D46:1;
  unsigned B4D47:1;
} B4D4bits;
extern volatile far  unsigned char       B4D5;
extern volatile far  struct {
  unsigned B4D50:1;
  unsigned B4D51:1;
  unsigned B4D52:1;
  unsigned B4D53:1;
  unsigned B4D54:1;
  unsigned B4D55:1;
  unsigned B4D56:1;
  unsigned B4D57:1;
} B4D5bits;
extern volatile far  unsigned char       B4D6;
extern volatile far  struct {
  unsigned B4D60:1;
  unsigned B4D61:1;
  unsigned B4D62:1;
  unsigned B4D63:1;
  unsigned B4D64:1;
  unsigned B4D65:1;
  unsigned B4D66:1;
  unsigned B4D67:1;
} B4D6bits;
extern volatile far  unsigned char       B4D7;
extern volatile far  struct {
  unsigned B4D70:1;
  unsigned B4D71:1;
  unsigned B4D72:1;
  unsigned B4D73:1;
  unsigned B4D74:1;
  unsigned B4D75:1;
  unsigned B4D76:1;
  unsigned B46D77:1;
} B4D7bits;
extern volatile far  unsigned char       CANSTAT_RO5;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO5bits;
extern volatile far  unsigned char       CANCON_RO5;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO5bits;
extern volatile far  unsigned char       B5CON;
extern volatile far  union {
  struct {
    unsigned FILHIT0:1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned RTREN:1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIF:1;
  };
} B5CONbits;
extern volatile far  unsigned char       B5SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} B5SIDHbits;
extern volatile far  unsigned char       B5SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXID:1;
    unsigned SRR:1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDE:1;
  };
} B5SIDLbits;
extern volatile far  unsigned char       B5EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} B5EIDHbits;
extern volatile far  unsigned char       B5EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} B5EIDLbits;
extern volatile far  unsigned char       B5DLC;
extern volatile far  struct {
  unsigned DLC0:1;
  unsigned DLC1:1;
  unsigned DLC2:1;
  unsigned DLC3:1;
  unsigned RESRB0:1;
  unsigned RESRB1:1;
  unsigned RXRTR:1;
} B5DLCbits;
extern volatile far  unsigned char       B5D0;
extern volatile far  struct {
  unsigned B5D00:1;
  unsigned B5D01:1;
  unsigned B5D02:1;
  unsigned B5D03:1;
  unsigned B5D04:1;
  unsigned B5D05:1;
  unsigned B5D06:1;
  unsigned B57D07:1;
} B5D0bits;
extern volatile far  unsigned char       B5D1;
extern volatile far  struct {
  unsigned B5D10:1;
  unsigned B5D11:1;
  unsigned B5D12:1;
  unsigned B5D13:1;
  unsigned B5D14:1;
  unsigned B5D15:1;
  unsigned B5D16:1;
  unsigned B5D17:1;
} B5D1bits;
extern volatile far  unsigned char       B5D2;
extern volatile far  struct {
  unsigned B5D20:1;
  unsigned B5D21:1;
  unsigned B5D22:1;
  unsigned B57D23:1;
  unsigned B5D24:1;
  unsigned B5D25:1;
  unsigned B5D26:1;
  unsigned B5D27:1;
} B5D2bits;
extern volatile far  unsigned char       B5D3;
extern volatile far  struct {
  unsigned B5D30:1;
  unsigned B5D31:1;
  unsigned B5D32:1;
  unsigned B5D33:1;
  unsigned B5D34:1;
  unsigned B5D35:1;
  unsigned B5D36:1;
  unsigned B5D37:1;
} B5D3bits;
extern volatile far  unsigned char       B5D4;
extern volatile far  struct {
  unsigned B5D40:1;
  unsigned B5D41:1;
  unsigned B5D42:1;
  unsigned B5D43:1;
  unsigned B5D44:1;
  unsigned B5D45:1;
  unsigned B5D46:1;
  unsigned B5D47:1;
} B5D4bits;
extern volatile far  unsigned char       B5D5;
extern volatile far  struct {
  unsigned B5D50:1;
  unsigned B5D51:1;
  unsigned B5D52:1;
  unsigned B5D53:1;
  unsigned B5D54:1;
  unsigned B5D55:1;
  unsigned B5D56:1;
  unsigned B5D57:1;
} B5D5bits;
extern volatile far  unsigned char       B5D6;
extern volatile far  struct {
  unsigned B5D60:1;
  unsigned B5D61:1;
  unsigned B5D62:1;
  unsigned B5D63:1;
  unsigned B5D64:1;
  unsigned B5D65:1;
  unsigned B5D66:1;
  unsigned B5D67:1;
} B5D6bits;
extern volatile far  unsigned char       B5D7;
extern volatile far  struct {
  unsigned B5D70:1;
  unsigned B5D71:1;
  unsigned B5D72:1;
  unsigned B5D73:1;
  unsigned B5D74:1;
  unsigned B5D75:1;
  unsigned B5D76:1;
  unsigned B5D77:1;
} B5D7bits;
extern volatile far  unsigned char       CANSTAT_RO4;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO4bits;
extern volatile far  unsigned char       CANCON_RO4;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO4bits;
extern volatile far  unsigned char       RXF0SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF0SIDHbits;
extern volatile far  unsigned char       RXF0SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXIDE:1;
    unsigned :1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDEN:1;
  };
} RXF0SIDLbits;
extern volatile far  unsigned char       RXF0EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF0EIDHbits;
extern volatile far  unsigned char       RXF0EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF0EIDLbits;
extern volatile far  unsigned char       RXF1SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF1SIDHbits;
extern volatile far  unsigned char       RXF1SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXIDE:1;
    unsigned :1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDEN:1;
  };
} RXF1SIDLbits;
extern volatile far  unsigned char       RXF1EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF1EIDHbits;
extern volatile far  unsigned char       RXF1EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF1EIDLbits;
extern volatile far  unsigned char       RXF2SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF2SIDHbits;
extern volatile far  unsigned char       RXF2SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXIDE:1;
    unsigned :1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDEN:1;
  };
} RXF2SIDLbits;
extern volatile far  unsigned char       RXF2EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF2EIDHbits;
extern volatile far  unsigned char       RXF2EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF2EIDLbits;
extern volatile far  unsigned char       RXF3SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF3SIDHbits;
extern volatile far  unsigned char       RXF3SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXIDE:1;
    unsigned :1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDEN:1;
  };
} RXF3SIDLbits;
extern volatile far  unsigned char       RXF3EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF3EIDHbits;
extern volatile far  unsigned char       RXF3EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF3EIDLbits;
extern volatile far  unsigned char       RXF4SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF4SIDHbits;
extern volatile far  unsigned char       RXF4SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXIDE:1;
    unsigned :1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDEN:1;
  };
} RXF4SIDLbits;
extern volatile far  unsigned char       RXF4EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF4EIDHbits;
extern volatile far  unsigned char       RXF4EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF4EIDLbits;
extern volatile far  unsigned char       RXF5SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXF5SIDHbits;
extern volatile far  unsigned char       RXF5SIDL;
extern volatile far  union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXIDE:1;
    unsigned :1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDEN:1;
  };
} RXF5SIDLbits;
extern volatile far  unsigned char       RXF5EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXF5EIDHbits;
extern volatile far  unsigned char       RXF5EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXF5EIDLbits;
extern volatile far  unsigned char       RXM0SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXM0SIDHbits;
extern volatile far  unsigned char       RXM0SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDEN:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXM0SIDLbits;
extern volatile far  unsigned char       RXM0EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXM0EIDHbits;
extern volatile far  unsigned char       RXM0EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXM0EIDLbits;
extern volatile far  unsigned char       RXM1SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXM1SIDHbits;
extern volatile far  unsigned char       RXM1SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDEN:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXM1SIDLbits;
extern volatile far  unsigned char       RXM1EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXM1EIDHbits;
extern volatile far  unsigned char       RXM1EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXM1EIDLbits;
extern volatile far  unsigned char       TXB2CON;
extern volatile far  union {
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned :1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIFBXB2CON:1;
  };
  struct {
    unsigned :7;
    unsigned TXBIF:1;
  };
} TXB2CONbits;
extern volatile far  unsigned char       TXB2SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} TXB2SIDHbits;
extern volatile far  unsigned char       TXB2SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} TXB2SIDLbits;
extern volatile far  unsigned char       TXB2EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} TXB2EIDHbits;
extern volatile far  unsigned char       TXB2EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} TXB2EIDLbits;
extern volatile far  unsigned char       TXB2DLC;
extern volatile far  struct {
  unsigned DLC0:1;
  unsigned DLC1:1;
  unsigned DLC2:1;
  unsigned DLC3:1;
  unsigned :2;
  unsigned TXRTR:1;
} TXB2DLCbits;
extern volatile far  unsigned char       TXB2D0;
extern volatile far  struct {
  unsigned TXB2D00:1;
  unsigned TXB2D01:1;
  unsigned TXB2D02:1;
  unsigned TXB2D03:1;
  unsigned TXB2D04:1;
  unsigned TXB2D05:1;
  unsigned TXB2D06:1;
  unsigned TXB2D07:1;
} TXB2D0bits;
extern volatile far  unsigned char       TXB2D1;
extern volatile far  struct {
  unsigned TXB2D10:1;
  unsigned TXB2D11:1;
  unsigned TXB2D12:1;
  unsigned TXB2D13:1;
  unsigned TXB2D14:1;
  unsigned TXB2D15:1;
  unsigned TXB2D16:1;
  unsigned TXB2D17:1;
} TXB2D1bits;
extern volatile far  unsigned char       TXB2D2;
extern volatile far  struct {
  unsigned TXB2D20:1;
  unsigned TXB2D21:1;
  unsigned TXB2D22:1;
  unsigned TXB2D23:1;
  unsigned TXB2D24:1;
  unsigned TXB2D25:1;
  unsigned TXB2D26:1;
  unsigned TXB2D27:1;
} TXB2D2bits;
extern volatile far  unsigned char       TXB2D3;
extern volatile far  struct {
  unsigned TXB2D30:1;
  unsigned TXB2D31:1;
  unsigned TXB2D32:1;
  unsigned TXB2D33:1;
  unsigned TXB2D34:1;
  unsigned TXB2D35:1;
  unsigned TXB2D36:1;
  unsigned TXB2D37:1;
} TXB2D3bits;
extern volatile far  unsigned char       TXB2D4;
extern volatile far  struct {
  unsigned TXB2D40:1;
  unsigned TXB2D41:1;
  unsigned TXB2D42:1;
  unsigned TXB2D43:1;
  unsigned TXB2D44:1;
  unsigned TXB2D45:1;
  unsigned TXB2D46:1;
  unsigned TXB2D47:1;
} TXB2D4bits;
extern volatile far  unsigned char       TXB2D5;
extern volatile far  struct {
  unsigned TXB2D50:1;
  unsigned TXB2D51:1;
  unsigned TXB2D52:1;
  unsigned TXB2D53:1;
  unsigned TXB2D54:1;
  unsigned TXB2D55:1;
  unsigned TXB2D56:1;
  unsigned TXB2D57:1;
} TXB2D5bits;
extern volatile far  unsigned char       TXB2D6;
extern volatile far  struct {
  unsigned TXB2D60:1;
  unsigned TXB2D61:1;
  unsigned TXB2D62:1;
  unsigned TXB2D63:1;
  unsigned TXB2D64:1;
  unsigned TXB2D65:1;
  unsigned TXB2D66:1;
  unsigned TXB2D67:1;
} TXB2D6bits;
extern volatile far  unsigned char       TXB2D7;
extern volatile far  struct {
  unsigned TXB2D70:1;
  unsigned TXB2D71:1;
  unsigned TXB2D72:1;
  unsigned TXB2D73:1;
  unsigned TXB2D74:1;
  unsigned TXB2D75:1;
  unsigned TXB2D76:1;
  unsigned TXB2D77:1;
} TXB2D7bits;
extern volatile far  unsigned char       CANSTAT_RO3;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO3bits;
extern volatile far  unsigned char       CANCON_RO3;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO3bits;
extern volatile far  unsigned char       TXB1CON;
extern volatile far  union {
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned :1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
    unsigned TXBIFTXB1CON:1;
  };
  struct {
    unsigned :7;
    unsigned TXBIF:1;
  };
} TXB1CONbits;
extern volatile far  unsigned char       TXB1SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} TXB1SIDHbits;
extern volatile far  unsigned char       TXB1SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} TXB1SIDLbits;
extern volatile far  unsigned char       TXB1EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} TXB1EIDHbits;
extern volatile far  unsigned char       TXB1EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} TXB1EIDLbits;
extern volatile far  unsigned char       TXB1DLC;
extern volatile far  struct {
  unsigned DLC0:1;
  unsigned DLC1:1;
  unsigned DLC2:1;
  unsigned DLC3:1;
  unsigned :2;
  unsigned TXRTR:1;
} TXB1DLCbits;
extern volatile far  unsigned char       TXB1D0;
extern volatile far  struct {
  unsigned TXB1D00:1;
  unsigned TXB1D01:1;
  unsigned TXB1D02:1;
  unsigned TXB1D03:1;
  unsigned TXB1D04:1;
  unsigned TXB1D05:1;
  unsigned TXB1D06:1;
  unsigned TXB1D07:1;
} TXB1D0bits;
extern volatile far  unsigned char       TXB1D1;
extern volatile far  struct {
  unsigned TXB1D10:1;
  unsigned TXB1D11:1;
  unsigned TXB1D12:1;
  unsigned TXB1D13:1;
  unsigned TXB1D14:1;
  unsigned TXB1D15:1;
  unsigned TXB1D16:1;
  unsigned TXB1D17:1;
} TXB1D1bits;
extern volatile far  unsigned char       TXB1D2;
extern volatile far  struct {
  unsigned TXB1D20:1;
  unsigned TXB1D21:1;
  unsigned TXB1D22:1;
  unsigned TXB1D23:1;
  unsigned TXB1D24:1;
  unsigned TXB1D25:1;
  unsigned TXB1D26:1;
  unsigned TXB1D27:1;
} TXB1D2bits;
extern volatile far  unsigned char       TXB1D3;
extern volatile far  struct {
  unsigned TXB1D30:1;
  unsigned TXB1D31:1;
  unsigned TXB1D32:1;
  unsigned TXB1D33:1;
  unsigned TXB1D34:1;
  unsigned TXB1D35:1;
  unsigned TXB1D36:1;
  unsigned TXB1D37:1;
} TXB1D3bits;
extern volatile far  unsigned char       TXB1D4;
extern volatile far  struct {
  unsigned TXB1D40:1;
  unsigned TXB1D41:1;
  unsigned TXB1D42:1;
  unsigned TXB1D43:1;
  unsigned TXB1D44:1;
  unsigned TXB1D45:1;
  unsigned TXB1D46:1;
  unsigned TXB1D47:1;
} TXB1D4bits;
extern volatile far  unsigned char       TXB1D5;
extern volatile far  struct {
  unsigned TXB1D50:1;
  unsigned TXB1D51:1;
  unsigned TXB1D52:1;
  unsigned TXB1D53:1;
  unsigned TXB1D54:1;
  unsigned TXB1D55:1;
  unsigned TXB1D56:1;
  unsigned TXB1D57:1;
} TXB1D5bits;
extern volatile far  unsigned char       TXB1D6;
extern volatile far  struct {
  unsigned TXB1D60:1;
  unsigned TXB1D61:1;
  unsigned TXB1D62:1;
  unsigned TXB1D63:1;
  unsigned TXB1D64:1;
  unsigned TXB1D65:1;
  unsigned TXB1D66:1;
  unsigned TXB1D67:1;
} TXB1D6bits;
extern volatile far  unsigned char       TXB1D7;
extern volatile far  struct {
  unsigned TXB1D70:1;
  unsigned TXB1D71:1;
  unsigned TXB1D72:1;
  unsigned TXB1D73:1;
  unsigned TXB1D74:1;
  unsigned TXB1D75:1;
  unsigned TXB1D76:1;
  unsigned TXB1D77:1;
} TXB1D7bits;
extern volatile far  unsigned char       CANSTAT_RO2;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO2bits;
extern volatile far  unsigned char       CANCON_RO2;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO2bits;
extern volatile far  unsigned char       TXB0CON;
extern volatile far  union {
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned :1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
  };
  struct {
    unsigned :7;
    unsigned TXBIF:1;
  };
} TXB0CONbits;
extern volatile far  unsigned char       TXB0SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} TXB0SIDHbits;
extern volatile far  unsigned char       TXB0SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXIDE:1;
  unsigned :1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} TXB0SIDLbits;
extern volatile far  unsigned char       TXB0EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} TXB0EIDHbits;
extern volatile far  unsigned char       TXB0EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} TXB0EIDLbits;
extern volatile far  unsigned char       TXB0DLC;
extern volatile far  struct {
  unsigned DLC0:1;
  unsigned DLC1:1;
  unsigned DLC2:1;
  unsigned DLC3:1;
  unsigned :2;
  unsigned TXRTR:1;
} TXB0DLCbits;
extern volatile far  unsigned char       TXB0D0;
extern volatile far  struct {
  unsigned TXB0D00:1;
  unsigned TXB0D01:1;
  unsigned TXB0D02:1;
  unsigned TXB0D03:1;
  unsigned TXB0D04:1;
  unsigned TXB0D05:1;
  unsigned TXB0D06:1;
  unsigned TXB0D07:1;
} TXB0D0bits;
extern volatile far  unsigned char       TXB0D1;
extern volatile far  struct {
  unsigned TXB0D10:1;
  unsigned TXB0D11:1;
  unsigned TXB0D12:1;
  unsigned TXB0D13:1;
  unsigned TXB0D14:1;
  unsigned TXB0D15:1;
  unsigned TXB0D16:1;
  unsigned TXB0D17:1;
} TXB0D1bits;
extern volatile far  unsigned char       TXB0D2;
extern volatile far  struct {
  unsigned TXB0D20:1;
  unsigned TXB0D21:1;
  unsigned TXB0D22:1;
  unsigned TXB0D23:1;
  unsigned TXB0D24:1;
  unsigned TXB0D25:1;
  unsigned TXB0D26:1;
  unsigned TXB0D27:1;
} TXB0D2bits;
extern volatile far  unsigned char       TXB0D3;
extern volatile far  struct {
  unsigned TXB0D30:1;
  unsigned TXB0D31:1;
  unsigned TXB0D32:1;
  unsigned TXB0D33:1;
  unsigned TXB0D34:1;
  unsigned TXB0D35:1;
  unsigned TXB0D36:1;
  unsigned TXB0D37:1;
} TXB0D3bits;
extern volatile far  unsigned char       TXB0D4;
extern volatile far  struct {
  unsigned TXB0D40:1;
  unsigned TXB0D41:1;
  unsigned TXB0D42:1;
  unsigned TXB0D43:1;
  unsigned TXB0D44:1;
  unsigned TXB0D45:1;
  unsigned TXB0D46:1;
  unsigned TXB0D47:1;
} TXB0D4bits;
extern volatile far  unsigned char       TXB0D5;
extern volatile far  struct {
  unsigned TXB0D50:1;
  unsigned TXB0D51:1;
  unsigned TXB0D52:1;
  unsigned TXB0D53:1;
  unsigned TXB0D54:1;
  unsigned TXB0D55:1;
  unsigned TXB0D56:1;
  unsigned TXB0D57:1;
} TXB0D5bits;
extern volatile far  unsigned char       TXB0D6;
extern volatile far  struct {
  unsigned TXB0D60:1;
  unsigned TXB0D61:1;
  unsigned TXB0D62:1;
  unsigned TXB0D63:1;
  unsigned TXB0D64:1;
  unsigned TXB0D65:1;
  unsigned TXB0D66:1;
  unsigned TXB0D67:1;
} TXB0D6bits;
extern volatile far  unsigned char       TXB0D7;
extern volatile far  struct {
  unsigned TXB0D70:1;
  unsigned TXB0D71:1;
  unsigned TXB0D72:1;
  unsigned TXB0D73:1;
  unsigned TXB0D74:1;
  unsigned TXB0D75:1;
  unsigned TXB0D76:1;
  unsigned TXB0D77:1;
} TXB0D7bits;
extern volatile far  unsigned char       CANSTAT_RO1;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO1bits;
extern volatile far  unsigned char       CANCON_RO1;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO1bits;
extern volatile far  unsigned char       RXB1CON;
extern volatile far  union {
  struct {
    unsigned FILHIT0:1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned RXRTRRO:1;
    unsigned :1;
    unsigned RXM0:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned :3;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
  };
} RXB1CONbits;
extern volatile far  unsigned char       RXB1SIDH;
extern volatile far  struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXB1SIDHbits;
extern volatile far  unsigned char       RXB1SIDL;
extern volatile far  struct {
  unsigned EID16:1;
  unsigned EID17:1;
  unsigned :1;
  unsigned EXID:1;
  unsigned SRR:1;
  unsigned SID0:1;
  unsigned SID1:1;
  unsigned SID2:1;
} RXB1SIDLbits;
extern volatile far  unsigned char       RXB1EIDH;
extern volatile far  struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXB1EIDHbits;
extern volatile far  unsigned char       RXB1EIDL;
extern volatile far  struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXB1EIDLbits;
extern volatile far  unsigned char       RXB1DLC;
extern volatile far  struct {
  unsigned DLC0:1;
  unsigned DLC1:1;
  unsigned DLC2:1;
  unsigned DLC3:1;
  unsigned RESRB0:1;
  unsigned RESRB1:1;
  unsigned RXRTR:1;
} RXB1DLCbits;
extern volatile far  unsigned char       RXB1D0;
extern volatile far  struct {
  unsigned RXB1D00:1;
  unsigned RXB1D01:1;
  unsigned RXB1D02:1;
  unsigned RXB1D03:1;
  unsigned RXB1D04:1;
  unsigned RXB1D05:1;
  unsigned RXB1D06:1;
  unsigned RXB1D07:1;
} RXB1D0bits;
extern volatile far  unsigned char       RXB1D1;
extern volatile far  struct {
  unsigned RXB1D10:1;
  unsigned RXB1D11:1;
  unsigned RXB1D12:1;
  unsigned RXB1D13:1;
  unsigned RXB1D14:1;
  unsigned RXB1D15:1;
  unsigned RXB1D16:1;
  unsigned RXB1D17:1;
} RXB1D1bits;
extern volatile far  unsigned char       RXB1D2;
extern volatile far  struct {
  unsigned RXB1D20:1;
  unsigned RXB1D21:1;
  unsigned RXB1D22:1;
  unsigned RXB1D23:1;
  unsigned RXB1D24:1;
  unsigned RXB1D25:1;
  unsigned RXB1D26:1;
  unsigned RXB1D27:1;
} RXB1D2bits;
extern volatile far  unsigned char       RXB1D3;
extern volatile far  struct {
  unsigned RXB1D30:1;
  unsigned RXB1D31:1;
  unsigned RXB1D32:1;
  unsigned RXB1D33:1;
  unsigned RXB1D34:1;
  unsigned RXB1D35:1;
  unsigned RXB1D36:1;
  unsigned RXB1D37:1;
} RXB1D3bits;
extern volatile far  unsigned char       RXB1D4;
extern volatile far  struct {
  unsigned RXB1D40:1;
  unsigned RXB1D41:1;
  unsigned RXB1D42:1;
  unsigned RXB1D43:1;
  unsigned RXB1D44:1;
  unsigned RXB1D45:1;
  unsigned RXB1D46:1;
  unsigned RXB1D47:1;
} RXB1D4bits;
extern volatile far  unsigned char       RXB1D5;
extern volatile far  struct {
  unsigned RXB1D50:1;
  unsigned RXB1D51:1;
  unsigned RXB1D52:1;
  unsigned RXB1D53:1;
  unsigned RXB1D54:1;
  unsigned RXB1D55:1;
  unsigned RXB1D56:1;
  unsigned RXB1D57:1;
} RXB1D5bits;
extern volatile far  unsigned char       RXB1D6;
extern volatile far  struct {
  unsigned RXB1D60:1;
  unsigned RXB1D61:1;
  unsigned RXB1D62:1;
  unsigned RXB1D63:1;
  unsigned RXB1D64:1;
  unsigned RXB1D65:1;
  unsigned RXB1D66:1;
  unsigned RXB1D67:1;
} RXB1D6bits;
extern volatile far  unsigned char       RXB1D7;
extern volatile far  struct {
  unsigned RXB1D70:1;
  unsigned RXB1D71:1;
  unsigned RXB1D72:1;
  unsigned RXB1D73:1;
  unsigned RXB1D74:1;
  unsigned RXB1D75:1;
  unsigned RXB1D76:1;
  unsigned RXB1D77:1;
} RXB1D7bits;
extern volatile far  unsigned char       CANSTAT_RO0;
extern volatile far  struct {
  unsigned ICODE0:1;
  unsigned ICODE1:1;
  unsigned ICODE2:1;
  unsigned ICODE3:1;
  unsigned ICODE4:1;
  unsigned OPMODE:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANSTAT_RO0bits;
extern volatile far  unsigned char       CANCON_RO0;
extern volatile far  struct {
  unsigned :1;
  unsigned WIN0:1;
  unsigned WIN1:1;
  unsigned WIN2:1;
  unsigned ABAT:1;
  unsigned REQOP0:1;
  unsigned REQOP1:1;
  unsigned REQOP2:1;
} CANCON_RO0bits;
extern volatile near unsigned char       RXB0CON;
extern volatile near union {
  struct {
    unsigned FILHIT0:1;
    unsigned JTOFF:1;
    unsigned RXBODBEN:1;
    unsigned RXRTRRO:1;
    unsigned :1;
    unsigned RXM0:1;
    unsigned RXM1:1;
    unsigned RXFUL:1;
  };
  struct {
    unsigned :1;
    unsigned FILHIT1:1;
    unsigned FILHIT2:1;
    unsigned FILHIT3:1;
    unsigned FILHIT4:1;
    unsigned RTRRO:1;
  };
  struct {
    unsigned TXPRI0:1;
    unsigned TXPRI1:1;
    unsigned :1;
    unsigned TXREQ:1;
    unsigned TXERR:1;
    unsigned TXLARB:1;
    unsigned TXABT:1;
  };
  struct {
    unsigned :7;
    unsigned TXBIF:1;
  };
} RXB0CONbits;
extern volatile near unsigned char       RXB0SIDH;
extern volatile near struct {
  unsigned SID3:1;
  unsigned SID4:1;
  unsigned SID5:1;
  unsigned SID6:1;
  unsigned SID7:1;
  unsigned SID8:1;
  unsigned SID9:1;
  unsigned SID10:1;
} RXB0SIDHbits;
extern volatile near unsigned char       RXB0SIDL;
extern volatile near union {
  struct {
    unsigned EID16:1;
    unsigned EID17:1;
    unsigned :1;
    unsigned EXID:1;
    unsigned SRR:1;
    unsigned SID0:1;
    unsigned SID1:1;
    unsigned SID2:1;
  };
  struct {
    unsigned :3;
    unsigned EXIDE:1;
  };
} RXB0SIDLbits;
extern volatile near unsigned char       RXB0EIDH;
extern volatile near struct {
  unsigned EID8:1;
  unsigned EID9:1;
  unsigned EID10:1;
  unsigned EID11:1;
  unsigned EID12:1;
  unsigned EID13:1;
  unsigned EID14:1;
  unsigned EID15:1;
} RXB0EIDHbits;
extern volatile near unsigned char       RXB0EIDL;
extern volatile near struct {
  unsigned EID0:1;
  unsigned EID1:1;
  unsigned EID2:1;
  unsigned EID3:1;
  unsigned EID4:1;
  unsigned EID5:1;
  unsigned EID6:1;
  unsigned EID7:1;
} RXB0EIDLbits;
extern volatile near unsigned char       RXB0DLC;
extern volatile near struct {
  unsigned DLC0:1;
  unsigned DLC1:1;
  unsigned DLC2:1;
  unsigned DLC3:1;
  unsigned RESRB0:1;
  unsigned RESRB1:1;
  unsigned RXRTR:1;
} RXB0DLCbits;
extern volatile near unsigned char       RXB0D0;
extern volatile near struct {
  unsigned RXB0D00:1;
  unsigned RXB0D01:1;
  unsigned RXB0D02:1;
  unsigned RXB0D03:1;
  unsigned RXB0D04:1;
  unsigned RXB0D05:1;
  unsigned RXB0D06:1;
  unsigned RXB0D07:1;
} RXB0D0bits;
extern volatile near unsigned char       RXB0D1;
extern volatile near struct {
  unsigned RXB0D10:1;
  unsigned RXB0D11:1;
  unsigned RXB0D12:1;
  unsigned RXB0D13:1;
  unsigned RXB0D14:1;
  unsigned RXB0D15:1;
  unsigned RXB0D16:1;
  unsigned RXB0D17:1;
} RXB0D1bits;
extern volatile near unsigned char       RXB0D2;
extern volatile near struct {
  unsigned RXB0D20:1;
  unsigned RXB0D21:1;
  unsigned RXB0D22:1;
  unsigned RXB0D23:1;
  unsigned RXB0D24:1;
  unsigned RXB0D25:1;
  unsigned RXB0D26:1;
  unsigned RXB0D27:1;
} RXB0D2bits;
extern volatile near unsigned char       RXB0D3;
extern volatile near struct {
  unsigned RXB0D30:1;
  unsigned RXB0D31:1;
  unsigned RXB0D32:1;
  unsigned RXB0D33:1;
  unsigned RXB0D34:1;
  unsigned RXB0D35:1;
  unsigned RXB0D36:1;
  unsigned RXB0D37:1;
} RXB0D3bits;
extern volatile near unsigned char       RXB0D4;
extern volatile near struct {
  unsigned RXB0D40:1;
  unsigned RXB0D41:1;
  unsigned RXB0D42:1;
  unsigned RXB0D43:1;
  unsigned RXB0D44:1;
  unsigned RXB0D45:1;
  unsigned RXB0D46:1;
  unsigned RXB0D47:1;
} RXB0D4bits;
extern volatile near unsigned char       RXB0D5;
extern volatile near struct {
  unsigned RXB0D50:1;
  unsigned RXB0D51:1;
  unsigned RXB0D52:1;
  unsigned RXB0D53:1;
  unsigned RXB0D54:1;
  unsigned RXB0D55:1;
  unsigned RXB0D56:1;
  unsigned RXB0D57:1;
} RXB0D5bits;
extern volatile near unsigned char       RXB0D6;
extern volatile near struct {
  unsigned RXB0D60:1;
  unsigned RXB0D61:1;
  unsigned RXB0D62:1;
  unsigned RXB0D63:1;
  unsigned RXB0D64:1;
  unsigned RXB0D65:1;
  unsigned RXB0D66:1;
  unsigned RXB0D67:1;
} RXB0D6bits;
extern volatile near unsigned char       RXB0D7;
extern volatile near struct {
  unsigned RXB0D70:1;
  unsigned RXB0D71:1;
  unsigned RXB0D72:1;
  unsigned RXB0D73:1;
  unsigned RXB0D74:1;
  unsigned RXB0D75:1;
  unsigned RXB0D76:1;
  unsigned RXB0D77:1;
} RXB0D7bits;
extern volatile near unsigned char       CANSTAT;
extern volatile near union {
  struct {
    unsigned :1;
    unsigned ICODE0:1;
    unsigned ICODE1:1;
    unsigned ICODE2:1;
    unsigned :1;
    unsigned OPMODE0:1;
    unsigned OPMODE1:1;
    unsigned OPMODE2:1;
  };
  struct {
    unsigned EICODE0:1;
    unsigned EICODE1:1;
    unsigned EICODE2:1;
    unsigned EICODE3:1;
    unsigned EICODE4:1;
  };
} CANSTATbits;
extern volatile near unsigned char       CANCON;
extern volatile near union {
  struct {
    unsigned :1;
    unsigned WIN0:1;
    unsigned WIN1:1;
    unsigned WIN2:1;
    unsigned ABAT:1;
    unsigned REQOP0:1;
    unsigned REQOP1:1;
    unsigned REQOP2:1;
  };
  struct {
    unsigned FP0:1;
    unsigned FP1:1;
    unsigned FP2:1;
    unsigned FP3:1;
  };
} CANCONbits;
extern volatile near unsigned char       BRGCON1;
extern volatile near struct {
  unsigned BRP0:1;
  unsigned BRP1:1;
  unsigned BRP2:1;
  unsigned BRP3:1;
  unsigned BRP4:1;
  unsigned BRP5:1;
  unsigned SJW0:1;
  unsigned SJW1:1;
} BRGCON1bits;
extern volatile near unsigned char       BRGCON2;
extern volatile near union {
  struct {
    unsigned PRSEG0:1;
    unsigned PRSEG1:1;
    unsigned PRSEG2:1;
    unsigned SEG1PH0:1;
    unsigned SEG1PH1:1;
    unsigned SEG1PH2:1;
    unsigned SAM:1;
    unsigned SEG2PHTS:1;
  };
  struct {
    unsigned :7;
    unsigned SEG2PHT:1;
  };
} BRGCON2bits;
extern volatile near unsigned char       BRGCON3;
extern volatile near struct {
  unsigned SEG2PH0:1;
  unsigned SEG2PH1:1;
  unsigned SEG2PH2:1;
  unsigned :3;
  unsigned WAKFIL:1;
  unsigned WAKDIS:1;
} BRGCON3bits;
extern volatile near unsigned char       CIOCON;
extern volatile near struct {
  unsigned :4;
  unsigned CANCAP:1;
  unsigned ENDRHI:1;
} CIOCONbits;
extern volatile near unsigned char       COMSTAT;
extern volatile near union {
  struct {
    unsigned EWARN:1;
    unsigned RXWARN:1;
    unsigned TXWARN:1;
    unsigned RXBP:1;
    unsigned TXBP:1;
    unsigned TXBO:1;
    unsigned RXB1OVFL:1;
    unsigned RXB0OVFL:1;
  };
  struct {
    unsigned :6;
    unsigned RXBnOVFL:1;
  };
  struct {
    unsigned :7;
    unsigned FIFOEMPTY:1;
  };
} COMSTATbits;
extern volatile near unsigned char       RXERRCNT;
extern volatile near struct {
  unsigned REC0:1;
  unsigned REC1:1;
  unsigned REC2:1;
  unsigned REC3:1;
  unsigned REC4:1;
  unsigned REC5:1;
  unsigned REC6:1;
  unsigned REC7:1;
} RXERRCNTbits;
extern volatile near unsigned char       TXERRCNT;
extern volatile near struct {
  unsigned TEC0:1;
  unsigned TEC1:1;
  unsigned TEC2:1;
  unsigned TEC3:1;
  unsigned TEC4:1;
  unsigned TEC5:1;
  unsigned TEC6:1;
  unsigned TEC7:1;
} TXERRCNTbits;
extern volatile near unsigned char       ECANCON;
extern volatile near union {
  struct {
    unsigned EWIN0:1;
    unsigned EWIN1:1;
    unsigned EWIN2:1;
    unsigned EWIN3:1;
    unsigned EWIN4:1;
    unsigned FIFOWM:1;
    unsigned MDSEL0:1;
    unsigned MDSEL1:1;
  };
  struct {
    unsigned :5;
    unsigned F:1;
  };
} ECANCONbits;
extern volatile near unsigned char       PORTA;
extern volatile near union {
  struct {
    unsigned RA0:1;
    unsigned RA1:1;
    unsigned RA2:1;
    unsigned RA3:1;
    unsigned RA4:1;
    unsigned RA5:1;
    unsigned RA6:1;
    unsigned RA7:1;
  };
  struct {
    unsigned AN0:1;
    unsigned AN1:1;
    unsigned AN2:1;
    unsigned AN3:1;
    unsigned T0CKI:1;
    unsigned AN4:1;
    unsigned OSC2:1;
    unsigned OSC1:1;
  };
  struct {
    unsigned CVREF:1;
    unsigned :1;
    unsigned VREFM:1;
    unsigned VREFP:1;
    unsigned :1;
    unsigned LVDIN:1;
    unsigned CLKO:1;
    unsigned CLKI:1;
  };
  struct {
    unsigned :5;
    unsigned SS:1;
  };
  struct {
    unsigned :5;
    unsigned NOT_SS:1;
  };
} PORTAbits;
extern volatile near unsigned char       PORTB;
extern volatile near union {
  struct {
    unsigned RB0:1;
    unsigned RB1:1;
    unsigned RB2:1;
    unsigned RB3:1;
    unsigned RB4:1;
    unsigned RB5:1;
    unsigned RB6:1;
    unsigned RB7:1;
  };
  struct {
    unsigned INT0:1;
    unsigned INT1:1;
    unsigned INT2:1;
    unsigned CANRX:1;
    unsigned KBI0:1;
    unsigned KBI1:1;
    unsigned KBI2:1;
    unsigned KBI3:1;
  };
  struct {
    unsigned AN10:1;
  };
  struct {
    unsigned FLT0:1;
    unsigned AN8:1;
    unsigned CANTX:1;
    unsigned :1;
    unsigned AN9:1;
    unsigned PGM:1;
    unsigned PGC:1;
    unsigned PGD:1;
  };
} PORTBbits;
extern volatile near unsigned char       PORTC;
extern volatile near union {
  struct {
    unsigned RC0:1;
    unsigned RC1:1;
    unsigned RC2:1;
    unsigned RC3:1;
    unsigned RC4:1;
    unsigned RC5:1;
    unsigned RC6:1;
    unsigned RC7:1;
  };
  struct {
    unsigned T1OSO:1;
    unsigned T1OSI:1;
    unsigned CCP1:1;
    unsigned SCK:1;
    unsigned SDI:1;
    unsigned SDO:1;
    unsigned TX:1;
    unsigned RX:1;
  };
  struct {
    unsigned T13CKI:1;
    unsigned :2;
    unsigned SCL:1;
    unsigned SDA:1;
    unsigned :1;
    unsigned CK:1;
    unsigned DT:1;
  };
} PORTCbits;
extern volatile near unsigned char       PORTD;
extern volatile near union {
  struct {
    unsigned RD0:1;
    unsigned RD1:1;
    unsigned RD2:1;
    unsigned RD3:1;
    unsigned RD4:1;
    unsigned RD5:1;
    unsigned RD6:1;
    unsigned RD7:1;
  };
  struct {
    unsigned PSP0:1;
    unsigned PSP1:1;
    unsigned PSP2:1;
    unsigned PSP3:1;
    unsigned PSP4:1;
    unsigned PSP5:1;
    unsigned PSP6:1;
    unsigned PSP7:1;
  };
  struct {
    unsigned C1INB:1;
    unsigned C1INA:1;
    unsigned C2INB:1;
    unsigned C2INA:1;
    unsigned P1A:1;
    unsigned P1B:1;
    unsigned P1C:1;
    unsigned P1D:1;
  };
  struct {
    unsigned :4;
    unsigned ECCP1:1;
  };
} PORTDbits;
extern volatile near unsigned char       PORTE;
extern volatile near struct {
  unsigned RE0:1;
  unsigned RE1:1;
  unsigned RE2:1;
  unsigned RE3:1;
} PORTEbits;
extern volatile near unsigned char       LATA;
extern volatile near struct {
  unsigned LATA0:1;
  unsigned LATA1:1;
  unsigned LATA2:1;
  unsigned LATA3:1;
  unsigned LATA4:1;
  unsigned LATA5:1;
  unsigned LATA6:1;
  unsigned LATA7:1;
} LATAbits;
extern volatile near unsigned char       LATB;
extern volatile near struct {
  unsigned LATB0:1;
  unsigned LATB1:1;
  unsigned LATB2:1;
  unsigned LATB3:1;
  unsigned LATB4:1;
  unsigned LATB5:1;
  unsigned LATB6:1;
  unsigned LATB7:1;
} LATBbits;
extern volatile near unsigned char       LATC;
extern volatile near struct {
  unsigned LATC0:1;
  unsigned LATC1:1;
  unsigned LATC2:1;
  unsigned LATC3:1;
  unsigned LATC4:1;
  unsigned LATC5:1;
  unsigned LATC6:1;
  unsigned LATC7:1;
} LATCbits;
extern volatile near unsigned char       LATD;
extern volatile near struct {
  unsigned LATD0:1;
  unsigned LATD1:1;
  unsigned LATD2:1;
  unsigned LATD3:1;
  unsigned LATD4:1;
  unsigned LATD5:1;
  unsigned LATD6:1;
  unsigned LATD7:1;
} LATDbits;
extern volatile near unsigned char       LATE;
extern volatile near struct {
  unsigned LATE0:1;
  unsigned LATE1:1;
  unsigned LATE2:1;
} LATEbits;
extern volatile near unsigned char       DDRA;
extern volatile near struct {
  unsigned RA0:1;
  unsigned RA1:1;
  unsigned RA2:1;
  unsigned RA3:1;
  unsigned RA4:1;
  unsigned RA5:1;
  unsigned RA6:1;
} DDRAbits;
extern volatile near unsigned char       TRISA;
extern volatile near struct {
  unsigned TRISA0:1;
  unsigned TRISA1:1;
  unsigned TRISA2:1;
  unsigned TRISA3:1;
  unsigned TRISA4:1;
  unsigned TRISA5:1;
  unsigned TRISA6:1;
} TRISAbits;
extern volatile near unsigned char       DDRB;
extern volatile near struct {
  unsigned RB0:1;
  unsigned RB1:1;
  unsigned RB2:1;
  unsigned RB3:1;
  unsigned RB4:1;
  unsigned RB5:1;
  unsigned RB6:1;
  unsigned RB7:1;
} DDRBbits;
extern volatile near unsigned char       TRISB;
extern volatile near struct {
  unsigned TRISB0:1;
  unsigned TRISB1:1;
  unsigned TRISB2:1;
  unsigned TRISB3:1;
  unsigned TRISB4:1;
  unsigned TRISB5:1;
  unsigned TRISB6:1;
  unsigned TRISB7:1;
} TRISBbits;
extern volatile near unsigned char       DDRC;
extern volatile near struct {
  unsigned RC0:1;
  unsigned RC1:1;
  unsigned RC2:1;
  unsigned RC3:1;
  unsigned RC4:1;
  unsigned RC5:1;
  unsigned RC6:1;
  unsigned RC7:1;
} DDRCbits;
extern volatile near unsigned char       TRISC;
extern volatile near struct {
  unsigned TRISC0:1;
  unsigned TRISC1:1;
  unsigned TRISC2:1;
  unsigned TRISC3:1;
  unsigned TRISC4:1;
  unsigned TRISC5:1;
  unsigned TRISC6:1;
  unsigned TRISC7:1;
} TRISCbits;
extern volatile near unsigned char       DDRD;
extern volatile near struct {
  unsigned RD0:1;
  unsigned RD1:1;
  unsigned RD2:1;
  unsigned RD3:1;
  unsigned RD4:1;
  unsigned RD5:1;
  unsigned RD6:1;
  unsigned RD7:1;
} DDRDbits;
extern volatile near unsigned char       TRISD;
extern volatile near struct {
  unsigned TRISD0:1;
  unsigned TRISD1:1;
  unsigned TRISD2:1;
  unsigned TRISD3:1;
  unsigned TRISD4:1;
  unsigned TRISD5:1;
  unsigned TRISD6:1;
  unsigned TRISD7:1;
} TRISDbits;
extern volatile near unsigned char       DDRE;
extern volatile near struct {
  unsigned RE0:1;
  unsigned RE1:1;
  unsigned RE2:1;
  unsigned :1;
  unsigned PSPMODE:1;
  unsigned IBOV:1;
  unsigned OBF:1;
  unsigned IBF:1;
} DDREbits;
extern volatile near unsigned char       TRISE;
extern volatile near struct {
  unsigned TRISE0:1;
  unsigned TRISE1:1;
  unsigned TRISE2:1;
  unsigned :1;
  unsigned PSPMODE:1;
  unsigned IBOV:1;
  unsigned OBF:1;
  unsigned IBF:1;
} TRISEbits;
extern volatile near unsigned char       OSCTUNE;
extern volatile near struct {
  unsigned TUN0:1;
  unsigned TUN1:1;
  unsigned TUN2:1;
  unsigned TUN3:1;
  unsigned TUN4:1;
  unsigned :1;
  unsigned PLLEN:1;
  unsigned INTSCR:1;
} OSCTUNEbits;
extern volatile near unsigned char       PIE1;
extern volatile near struct {
  unsigned TMR1IE:1;
  unsigned TMR2IE:1;
  unsigned CCP1IE:1;
  unsigned SSPIE:1;
  unsigned TXIE:1;
  unsigned RCIE:1;
  unsigned ADIE:1;
  unsigned PSPIE:1;
} PIE1bits;
extern volatile near unsigned char       PIR1;
extern volatile near struct {
  unsigned TMR1IF:1;
  unsigned TMR2IF:1;
  unsigned CCP1IF:1;
  unsigned SSPIF:1;
  unsigned TXIF:1;
  unsigned RCIF:1;
  unsigned ADIF:1;
  unsigned PSPIF:1;
} PIR1bits;
extern volatile near unsigned char       IPR1;
extern volatile near struct {
  unsigned TMR1IP:1;
  unsigned TMR2IP:1;
  unsigned CCP1IP:1;
  unsigned SSPIP:1;
  unsigned TXBIP:1;
  unsigned RCIP:1;
  unsigned ADIP:1;
  unsigned PSPIP:1;
} IPR1bits;
extern volatile near unsigned char       PIE2;
extern volatile near struct {
  unsigned ECCP1IE:1;
  unsigned TMR3IE:1;
  unsigned LVDIE:1;
  unsigned BCLIE:1;
  unsigned EEIE:1;
  unsigned :1;
  unsigned CMIE:1;
  unsigned OSCFIE:1;
} PIE2bits;
extern volatile near unsigned char       PIR2;
extern volatile near struct {
  unsigned ECCP1IF:1;
  unsigned TMR3IF:1;
  unsigned LVDIF:1;
  unsigned BCLIF:1;
  unsigned EEIF:1;
  unsigned :1;
  unsigned CMIF:1;
  unsigned OSCFIF:1;
} PIR2bits;
extern volatile near unsigned char       IPR2;
extern volatile near struct {
  unsigned ECCP1IP:1;
  unsigned TMR3IP:1;
  unsigned LVDIP:1;
  unsigned BCLIP:1;
  unsigned EEIP:1;
  unsigned :1;
  unsigned CMIP:1;
  unsigned OSCFIP:1;
} IPR2bits;
extern volatile near unsigned char       PIE3;
extern volatile near union {
  struct {
    unsigned RXB0IE:1;
    unsigned RXB1IE:1;
    unsigned TXB0IE:1;
    unsigned TXB1IE:1;
    unsigned TXB2IE:1;
    unsigned ERRIE:1;
    unsigned WAKIE:1;
    unsigned IRXIE:1;
  };
  struct {
    unsigned :1;
    unsigned RXBnIE:1;
    unsigned :2;
    unsigned TXBnIE:1;
  };
} PIE3bits;
extern volatile near unsigned char       PIR3;
extern volatile near union {
  struct {
    unsigned RXB0IF:1;
    unsigned RXB1IF:1;
    unsigned TXB0IF:1;
    unsigned TXB1IF:1;
    unsigned TXB2IF:1;
    unsigned ERRIF:1;
    unsigned WAKIF:1;
    unsigned IRXIF:1;
  };
  struct {
    unsigned :1;
    unsigned RXBnIF:1;
    unsigned :2;
    unsigned TXBnIF:1;
  };
} PIR3bits;
extern volatile near unsigned char       IPR3;
extern volatile near union {
  struct {
    unsigned RXB0IP:1;
    unsigned RXB1IP:1;
    unsigned TXB0IP:1;
    unsigned TXB1IP:1;
    unsigned TXB2IP:1;
    unsigned ERRIP:1;
    unsigned WAKIP:1;
    unsigned IRXIP:1;
  };
  struct {
    unsigned :1;
    unsigned RXBnIP:1;
    unsigned :2;
    unsigned TXBnIP:1;
  };
} IPR3bits;
extern volatile near unsigned char       EECON1;
extern volatile near struct {
  unsigned RD:1;
  unsigned WR:1;
  unsigned WREN:1;
  unsigned WRERR:1;
  unsigned FREE:1;
  unsigned :1;
  unsigned CFGS:1;
  unsigned EEPGD:1;
} EECON1bits;
extern volatile near unsigned char       EECON2;
extern volatile near unsigned char       EEDATA;
extern volatile near unsigned char       EEADR;
extern volatile near unsigned char       EEADRH;
extern volatile near unsigned char       RCSTA;
extern volatile near struct {
  unsigned RX9D:1;
  unsigned OERR:1;
  unsigned FERR:1;
  unsigned ADEN:1;
  unsigned CREN:1;
  unsigned SREN:1;
  unsigned RX9:1;
  unsigned SPEN:1;
} RCSTAbits;
extern volatile near unsigned char       TXSTA;
extern volatile near struct {
  unsigned TX9D:1;
  unsigned TRMT:1;
  unsigned BRGH:1;
  unsigned SENDB:1;
  unsigned SYNC:1;
  unsigned TXEN:1;
  unsigned TX9:1;
  unsigned CSRC:1;
} TXSTAbits;
extern volatile near unsigned char       TXREG;
extern volatile near unsigned char       RCREG;
extern volatile near unsigned char       SPBRG;
extern volatile near unsigned char       SPBRGH;
extern volatile near unsigned char       T3CON;
extern volatile near union {
  struct {
    unsigned TMR3ON:1;
    unsigned TMR3CS:1;
    unsigned T3SYNC:1;
    unsigned T3CCP1:1;
    unsigned T3CKPS0:1;
    unsigned T3CKPS1:1;
    unsigned T3CCP2:1;
    unsigned RD16:1;
  };
  struct {
    unsigned :2;
    unsigned T3NSYNC:1;
  };
  struct {
    unsigned :5;
    unsigned T3ECCP1:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_T3SYNC:1;
  };
} T3CONbits;
extern volatile near unsigned char       TMR3L;
extern volatile near unsigned char       TMR3H;
extern volatile near unsigned char       CMCON;
extern volatile near struct {
  unsigned CM0:1;
  unsigned CM1:1;
  unsigned CM2:1;
  unsigned CIS:1;
  unsigned C1INV:1;
  unsigned C2INV:1;
  unsigned C1OUT:1;
  unsigned C2OUT:1;
} CMCONbits;
extern volatile near unsigned char       CVRCON;
extern volatile near union {
  struct {
    unsigned CVR0:1;
    unsigned CVR1:1;
    unsigned CVR2:1;
    unsigned CVR3:1;
    unsigned CVREF:1;
    unsigned CVRR:1;
    unsigned CVROE:1;
    unsigned CVREN:1;
  };
  struct {
    unsigned :4;
    unsigned CVRSS:1;
  };
} CVRCONbits;
extern volatile near unsigned char       ECCP1AS;
extern volatile near struct {
  unsigned PSSBD0:1;
  unsigned PSSBD1:1;
  unsigned PSSAC0:1;
  unsigned PSSAC1:1;
  unsigned ECCPAS0:1;
  unsigned ECCPAS1:1;
  unsigned ECCPAS2:1;
  unsigned ECCPASE:1;
} ECCP1ASbits;
extern volatile near unsigned char       ECCP1DEL;
extern volatile near struct {
  unsigned PDC0:1;
  unsigned PDC1:1;
  unsigned PDC2:1;
  unsigned PDC3:1;
  unsigned PDC4:1;
  unsigned PDC5:1;
  unsigned PDC6:1;
  unsigned PRSEN:1;
} ECCP1DELbits;
extern volatile near unsigned char       BAUDCON;
extern volatile near struct {
  unsigned ABDEN:1;
  unsigned WUE:1;
  unsigned :1;
  unsigned BRG16:1;
  unsigned SCKP:1;
  unsigned :1;
  unsigned RCIDL:1;
  unsigned ABDOVF:1;
} BAUDCONbits;
extern volatile near unsigned char       ECCP1CON;
extern volatile near struct {
  unsigned ECCP1M0:1;
  unsigned ECCP1M1:1;
  unsigned ECCP1M2:1;
  unsigned ECCP1M3:1;
  unsigned EDC1B0:1;
  unsigned EDC1B1:1;
  unsigned EPWM1M0:1;
  unsigned EPWM1M1:1;
} ECCP1CONbits;
extern volatile near unsigned            ECCPR1;
extern volatile near unsigned char       ECCPR1L;
extern volatile near unsigned char       ECCPR1H;
extern volatile near unsigned char       CCP1CON;
extern volatile near struct {
  unsigned CCP1M0:1;
  unsigned CCP1M1:1;
  unsigned CCP1M2:1;
  unsigned CCP1M3:1;
  unsigned DC1B0:1;
  unsigned DC1B1:1;
} CCP1CONbits;
extern volatile near unsigned            CCPR1;
extern volatile near unsigned char       CCPR1L;
extern volatile near unsigned char       CCPR1H;
extern volatile near unsigned char       ADCON2;
extern volatile near struct {
  unsigned ADCS0:1;
  unsigned ADCS1:1;
  unsigned ADCS2:1;
  unsigned ACQT0:1;
  unsigned ACQT1:1;
  unsigned ACQT2:1;
  unsigned :1;
  unsigned ADFM:1;
} ADCON2bits;
extern volatile near unsigned char       ADCON1;
extern volatile near struct {
  unsigned PCFG0:1;
  unsigned PCFG1:1;
  unsigned PCFG2:1;
  unsigned PCFG3:1;
  unsigned VCFG0:1;
  unsigned VCFG1:1;
} ADCON1bits;
extern volatile near unsigned char       ADCON0;
extern volatile near union {
  struct {
    unsigned ADON:1;
    unsigned GO_DONE:1;
    unsigned CHS0:1;
    unsigned CHS1:1;
    unsigned CHS2:1;
    unsigned CHS3:1;
  };
  struct {
    unsigned :1;
    unsigned DONE:1;
  };
  struct {
    unsigned :1;
    unsigned GO:1;
  };
  struct {
    unsigned :1;
    unsigned NOT_DONE:1;
  };
} ADCON0bits;
extern volatile near unsigned            ADRES;
extern volatile near unsigned char       ADRESL;
extern volatile near unsigned char       ADRESH;
extern volatile near unsigned char       SSPCON2;
extern volatile near struct {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned ACKSTAT:1;
  unsigned GCEN:1;
} SSPCON2bits;
extern volatile near unsigned char       SSPCON1;
extern volatile near struct {
  unsigned SSPM0:1;
  unsigned SSPM1:1;
  unsigned SSPM2:1;
  unsigned SSPM3:1;
  unsigned CKP:1;
  unsigned SSPEN:1;
  unsigned SSPOV:1;
  unsigned WCOL:1;
} SSPCON1bits;
extern volatile near unsigned char       SSPSTAT;
extern volatile near union {
  struct {
    unsigned BF:1;
    unsigned UA:1;
    unsigned R_W:1;
    unsigned S:1;
    unsigned P:1;
    unsigned D_A:1;
    unsigned CKE:1;
    unsigned SMP:1;
  };
  struct {
    unsigned :2;
    unsigned I2C_READ:1;
    unsigned I2C_START:1;
    unsigned I2C_STOP:1;
    unsigned I2C_DAT:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_W:1;
    unsigned :2;
    unsigned NOT_A:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_WRITE:1;
    unsigned :2;
    unsigned NOT_ADDRESS:1;
  };
  struct {
    unsigned :2;
    unsigned READ_WRITE:1;
    unsigned :2;
    unsigned DATA_ADDRESS:1;
  };
  struct {
    unsigned :2;
    unsigned R:1;
    unsigned :2;
    unsigned D:1;
  };
} SSPSTATbits;
extern volatile near unsigned char       SSPADD;
extern volatile near unsigned char       SSPBUF;
extern volatile near unsigned char       T2CON;
extern volatile near struct {
  unsigned T2CKPS0:1;
  unsigned T2CKPS1:1;
  unsigned TMR2ON:1;
  unsigned T2OUTPS0:1;
  unsigned T2OUTPS1:1;
  unsigned T2OUTPS2:1;
  unsigned T2OUTPS3:1;
} T2CONbits;
extern volatile near unsigned char       PR2;
extern volatile near unsigned char       TMR2;
extern volatile near unsigned char       T1CON;
extern volatile near union {
  struct {
    unsigned TMR1ON:1;
    unsigned TMR1CS:1;
    unsigned T1SYNC:1;
    unsigned T1OSCEN:1;
    unsigned T1CKPS0:1;
    unsigned T1CKPS1:1;
    unsigned T1RUN:1;
    unsigned RD16:1;
  };
  struct {
    unsigned :2;
    unsigned T1INSYNC:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_T1SYNC:1;
  };
} T1CONbits;
extern volatile near unsigned char       TMR1L;
extern volatile near unsigned char       TMR1H;
extern volatile near unsigned char       RCON;
extern volatile near union {
  struct {
    unsigned NOT_BOR:1;
    unsigned NOT_POR:1;
    unsigned NOT_PD:1;
    unsigned NOT_TO:1;
    unsigned NOT_RI:1;
    unsigned :1;
    unsigned SBOREN:1;
    unsigned IPEN:1;
  };
  struct {
    unsigned BOR:1;
    unsigned POR:1;
    unsigned PD:1;
    unsigned TO:1;
    unsigned RI:1;
  };
} RCONbits;
extern volatile near unsigned char       WDTCON;
extern volatile near union {
  struct {
    unsigned SWDTEN:1;
  };
  struct {
    unsigned SWDTE:1;
  };
} WDTCONbits;
extern volatile near unsigned char       HLVDCON;
extern volatile near union {
  struct {
    unsigned LVDL0:1;
    unsigned LVDL1:1;
    unsigned LVDL2:1;
    unsigned LVDL3:1;
    unsigned LVDEN:1;
    unsigned IRVST:1;
    unsigned :1;
    unsigned LVDMAG:1;
  };
  struct {
    unsigned LVV0:1;
    unsigned LVV1:1;
    unsigned LVV2:1;
    unsigned LVV3:1;
    unsigned :1;
    unsigned BGST:1;
  };
  struct {
    unsigned HLVV0:1;
    unsigned HLVV1:1;
    unsigned HLVV2:1;
    unsigned HLVV3:1;
    unsigned :1;
    unsigned HBGST:1;
    unsigned :1;
    unsigned HLVDMAG:1;
  };
  struct {
    unsigned HLVDL0:1;
    unsigned HLVDL1:1;
    unsigned HLVDL2:1;
    unsigned HLVDL3:1;
    unsigned HLVDEN:1;
    unsigned HIRVST:1;
  };
} HLVDCONbits;
extern volatile near unsigned char       LVDCON;
extern volatile near union {
  struct {
    unsigned LVDL0:1;
    unsigned LVDL1:1;
    unsigned LVDL2:1;
    unsigned LVDL3:1;
    unsigned LVDEN:1;
    unsigned IRVST:1;
    unsigned :1;
    unsigned LVDMAG:1;
  };
  struct {
    unsigned LVV0:1;
    unsigned LVV1:1;
    unsigned LVV2:1;
    unsigned LVV3:1;
    unsigned :1;
    unsigned BGST:1;
  };
  struct {
    unsigned HLVV0:1;
    unsigned HLVV1:1;
    unsigned HLVV2:1;
    unsigned HLVV3:1;
    unsigned :1;
    unsigned HBGST:1;
    unsigned :1;
    unsigned HLVDMAG:1;
  };
  struct {
    unsigned HLVDL0:1;
    unsigned HLVDL1:1;
    unsigned HLVDL2:1;
    unsigned HLVDL3:1;
    unsigned HLVDEN:1;
    unsigned HIRVST:1;
  };
} LVDCONbits;
extern volatile near unsigned char       OSCCON;
extern volatile near struct {
  unsigned SCS0:1;
  unsigned SCS1:1;
  unsigned IOFS:1;
  unsigned OSTS:1;
  unsigned IRCF0:1;
  unsigned IRCF1:1;
  unsigned IRCF2:1;
  unsigned IDLEN:1;
} OSCCONbits;
extern volatile near unsigned char       T0CON;
extern volatile near struct {
  unsigned T0PS0:1;
  unsigned T0PS1:1;
  unsigned T0PS2:1;
  unsigned T0PS3:1;
  unsigned T0SE:1;
  unsigned T0CS:1;
  unsigned T08BIT:1;
  unsigned TMR0ON:1;
} T0CONbits;
extern volatile near unsigned char       TMR0L;
extern volatile near unsigned char       TMR0H;
extern          near unsigned char       STATUS;
extern          near struct {
  unsigned C:1;
  unsigned DC:1;
  unsigned Z:1;
  unsigned OV:1;
  unsigned N:1;
} STATUSbits;
extern          near unsigned            FSR2;
extern          near unsigned char       FSR2L;
extern          near unsigned char       FSR2H;
extern volatile near unsigned char       PLUSW2;
extern volatile near unsigned char       PREINC2;
extern volatile near unsigned char       POSTDEC2;
extern volatile near unsigned char       POSTINC2;
extern          near unsigned char       INDF2;
extern          near unsigned char       BSR;
extern          near unsigned            FSR1;
extern          near unsigned char       FSR1L;
extern          near unsigned char       FSR1H;
extern volatile near unsigned char       PLUSW1;
extern volatile near unsigned char       PREINC1;
extern volatile near unsigned char       POSTDEC1;
extern volatile near unsigned char       POSTINC1;
extern          near unsigned char       INDF1;
extern          near unsigned char       WREG;
extern          near unsigned            FSR0;
extern          near unsigned char       FSR0L;
extern          near unsigned char       FSR0H;
extern volatile near unsigned char       PLUSW0;
extern volatile near unsigned char       PREINC0;
extern volatile near unsigned char       POSTDEC0;
extern volatile near unsigned char       POSTINC0;
extern          near unsigned char       INDF0;
extern volatile near unsigned char       INTCON3;
extern volatile near union {
  struct {
    unsigned INT1IF:1;
    unsigned INT2IF:1;
    unsigned :1;
    unsigned INT1IE:1;
    unsigned INT2IE:1;
    unsigned :1;
    unsigned INT1IP:1;
    unsigned INT2IP:1;
  };
  struct {
    unsigned INT1F:1;
    unsigned INT2F:1;
    unsigned :1;
    unsigned INT1E:1;
    unsigned INT2E:1;
    unsigned :1;
    unsigned INT1P:1;
    unsigned INT2P:1;
  };
} INTCON3bits;
extern volatile near unsigned char       INTCON2;
extern volatile near union {
  struct {
    unsigned RBIP:1;
    unsigned :1;
    unsigned TMR0IP:1;
    unsigned :1;
    unsigned INTEDG2:1;
    unsigned INTEDG1:1;
    unsigned INTEDG0:1;
    unsigned NOT_RBPU:1;
  };
  struct {
    unsigned :2;
    unsigned T0IP:1;
    unsigned :4;
    unsigned RBPU:1;
  };
} INTCON2bits;
extern volatile near unsigned char       INTCON;
extern volatile near union {
  struct {
    unsigned RBIF:1;
    unsigned INT0IF:1;
    unsigned TMR0IF:1;
    unsigned RBIE:1;
    unsigned INT0IE:1;
    unsigned TMR0IE:1;
    unsigned PEIE:1;
    unsigned GIE:1;
  };
  struct {
    unsigned :1;
    unsigned INT0F:1;
    unsigned T0IF:1;
    unsigned :1;
    unsigned INT0E:1;
    unsigned T0IE:1;
    unsigned GIEL:1;
    unsigned GIEH:1;
  };
} INTCONbits;
extern          near unsigned            PROD;
extern          near unsigned char       PRODL;
extern          near unsigned char       PRODH;
extern volatile near unsigned char       TABLAT;
extern volatile near unsigned short long TBLPTR;
extern volatile near unsigned char       TBLPTRL;
extern volatile near unsigned char       TBLPTRH;
extern volatile near unsigned char       TBLPTRU;
extern volatile near unsigned short long PC;
extern volatile near unsigned char       PCL;
extern volatile near unsigned char       PCLATH;
extern volatile near unsigned char       PCLATU;
extern volatile near unsigned char       STKPTR;
extern volatile near union {
  struct {
    unsigned STKPTR0:1;
    unsigned STKPTR1:1;
    unsigned STKPTR2:1;
    unsigned STKPTR3:1;
    unsigned STKPTR4:1;
    unsigned :1;
    unsigned STKUNF:1;
    unsigned STKOVF:1;
  };
  struct {
    unsigned :7;
    unsigned STKFUL:1;
  };
} STKPTRbits;
extern          near unsigned short long TOS;
extern          near unsigned char       TOSL;
extern          near unsigned char       TOSH;
extern          near unsigned char       TOSU;

#pragma varlocate 13 RXF6SIDH
#pragma varlocate 13 RXF6SIDHbits
#pragma varlocate 13 RXF6SIDL
#pragma varlocate 13 RXF6SIDLbits
#pragma varlocate 13 RXF6EIDH
#pragma varlocate 13 RXF6EIDHbits
#pragma varlocate 13 RXF6EIDL
#pragma varlocate 13 RXF6EIDLbits
#pragma varlocate 13 RXF7SIDH
#pragma varlocate 13 RXF7SIDHbits
#pragma varlocate 13 RXF7SIDL
#pragma varlocate 13 RXF7SIDLbits
#pragma varlocate 13 RXF7EIDH
#pragma varlocate 13 RXF7EIDHbits
#pragma varlocate 13 RXF7EIDL
#pragma varlocate 13 RXF7EIDLbits
#pragma varlocate 13 RXF8SIDH
#pragma varlocate 13 RXF8SIDHbits
#pragma varlocate 13 RXF8SIDL
#pragma varlocate 13 RXF8SIDLbits
#pragma varlocate 13 RXF8EIDH
#pragma varlocate 13 RXF8EIDHbits
#pragma varlocate 13 RXF8EIDL
#pragma varlocate 13 RXF8EIDLbits
#pragma varlocate 13 RXF9SIDH
#pragma varlocate 13 RXF9SIDHbits
#pragma varlocate 13 RXF9SIDL
#pragma varlocate 13 RXF9SIDLbits
#pragma varlocate 13 RXF9EIDH
#pragma varlocate 13 RXF9EIDHbits
#pragma varlocate 13 RXF9EIDL
#pragma varlocate 13 RXF9EIDLbits
#pragma varlocate 13 RXF10SIDH
#pragma varlocate 13 RXF10SIDHbits
#pragma varlocate 13 RXF10SIDL
#pragma varlocate 13 RXF10SIDLbits
#pragma varlocate 13 RXF10EIDH
#pragma varlocate 13 RXF10EIDHbits
#pragma varlocate 13 RXF10EIDL
#pragma varlocate 13 RXF10EIDLbits
#pragma varlocate 13 RXF11SIDH
#pragma varlocate 13 RXF11SIDHbits
#pragma varlocate 13 RXF11SIDL
#pragma varlocate 13 RXF11SIDLbits
#pragma varlocate 13 RXF11EIDH
#pragma varlocate 13 RXF11EIDHbits
#pragma varlocate 13 RXF11EIDL
#pragma varlocate 13 RXF11EIDLbits
#pragma varlocate 13 RXF12SIDH
#pragma varlocate 13 RXF12SIDHbits
#pragma varlocate 13 RXF12SIDL
#pragma varlocate 13 RXF12SIDLbits
#pragma varlocate 13 RXF12EIDH
#pragma varlocate 13 RXF12EIDHbits
#pragma varlocate 13 RXF12EIDL
#pragma varlocate 13 RXF12EIDLbits
#pragma varlocate 13 RXF13SIDH
#pragma varlocate 13 RXF13SIDHbits
#pragma varlocate 13 RXF13SIDL
#pragma varlocate 13 RXF13SIDLbits
#pragma varlocate 13 RXF13EIDH
#pragma varlocate 13 RXF13EIDHbits
#pragma varlocate 13 RXF13EIDL
#pragma varlocate 13 RXF13EIDLbits
#pragma varlocate 13 RXF14SIDH
#pragma varlocate 13 RXF14SIDHbits
#pragma varlocate 13 RXF14SIDL
#pragma varlocate 13 RXF14SIDLbits
#pragma varlocate 13 RXF14EIDH
#pragma varlocate 13 RXF14EIDHbits
#pragma varlocate 13 RXF14EIDL
#pragma varlocate 13 RXF14EIDLbits
#pragma varlocate 13 RXF15SIDH
#pragma varlocate 13 RXF15SIDHbits
#pragma varlocate 13 RXF15SIDL
#pragma varlocate 13 RXF15SIDLbits
#pragma varlocate 13 RXF15EIDH
#pragma varlocate 13 RXF15EIDHbits
#pragma varlocate 13 RXF15EIDL
#pragma varlocate 13 RXF15EIDLbits
#pragma varlocate 13 RXFCON0
#pragma varlocate 13 RXFCON0bits
#pragma varlocate 13 RXFCON1
#pragma varlocate 13 RXFCON1bits
#pragma varlocate 13 SDFLC
#pragma varlocate 13 SDFLCbits
#pragma varlocate 13 RXFBCON0
#pragma varlocate 13 RXFBCON0bits
#pragma varlocate 13 RXFBCON1
#pragma varlocate 13 RXFBCON1bits
#pragma varlocate 13 RXFBCON2
#pragma varlocate 13 RXFBCON2bits
#pragma varlocate 13 RXFBCON3
#pragma varlocate 13 RXFBCON3bits
#pragma varlocate 13 RXFBCON4
#pragma varlocate 13 RXFBCON4bits
#pragma varlocate 13 RXFBCON5
#pragma varlocate 13 RXFBCON5bits
#pragma varlocate 13 RXFBCON6
#pragma varlocate 13 RXFBCON6bits
#pragma varlocate 13 RXFBCON7
#pragma varlocate 13 RXFBCON7bits
#pragma varlocate 13 MSEL0
#pragma varlocate 13 MSEL0bits
#pragma varlocate 13 MSEL1
#pragma varlocate 13 MSEL1bits
#pragma varlocate 13 MSEL2
#pragma varlocate 13 MSEL2bits
#pragma varlocate 13 MSEL3
#pragma varlocate 13 MSEL3bits
#pragma varlocate 13 BSEL0
#pragma varlocate 13 BSEL0bits
#pragma varlocate 13 BIE0
#pragma varlocate 13 BIE0bits
#pragma varlocate 13 TXBIE
#pragma varlocate 13 TXBIEbits
#pragma varlocate 14 B0CON
#pragma varlocate 14 B0CONbits
#pragma varlocate 14 B0SIDH
#pragma varlocate 14 B0SIDHbits
#pragma varlocate 14 B0SIDL
#pragma varlocate 14 B0SIDLbits
#pragma varlocate 14 B0EIDH
#pragma varlocate 14 B0EIDHbits
#pragma varlocate 14 B0EIDL
#pragma varlocate 14 B0EIDLbits
#pragma varlocate 14 B0DLC
#pragma varlocate 14 B0DLCbits
#pragma varlocate 14 B0D0
#pragma varlocate 14 B0D0bits
#pragma varlocate 14 B0D1
#pragma varlocate 14 B0D1bits
#pragma varlocate 14 B0D2
#pragma varlocate 14 B0D2bits
#pragma varlocate 14 B0D3
#pragma varlocate 14 B0D3bits
#pragma varlocate 14 B0D4
#pragma varlocate 14 B0D4bits
#pragma varlocate 14 B0D5
#pragma varlocate 14 B0D5bits
#pragma varlocate 14 B0D6
#pragma varlocate 14 B0D6bits
#pragma varlocate 14 B0D7
#pragma varlocate 14 B0D7bits
#pragma varlocate 14 CANSTAT_RO9
#pragma varlocate 14 CANSTAT_RO9bits
#pragma varlocate 14 CANCON_RO9
#pragma varlocate 14 CANCON_RO9bits
#pragma varlocate 14 B1CON
#pragma varlocate 14 B1CONbits
#pragma varlocate 14 B1SIDH
#pragma varlocate 14 B1SIDHbits
#pragma varlocate 14 B1SIDL
#pragma varlocate 14 B1SIDLbits
#pragma varlocate 14 B1EIDH
#pragma varlocate 14 B1EIDHbits
#pragma varlocate 14 B1EIDL
#pragma varlocate 14 B1EIDLbits
#pragma varlocate 14 B1DLC
#pragma varlocate 14 B1DLCbits
#pragma varlocate 14 B1D0
#pragma varlocate 14 B1D0bits
#pragma varlocate 14 B1D1
#pragma varlocate 14 B1D1bits
#pragma varlocate 14 B1D2
#pragma varlocate 14 B1D2bits
#pragma varlocate 14 B1D3
#pragma varlocate 14 B1D3bits
#pragma varlocate 14 B1D4
#pragma varlocate 14 B1D4bits
#pragma varlocate 14 B1D5
#pragma varlocate 14 B1D5bits
#pragma varlocate 14 B1D6
#pragma varlocate 14 B1D6bits
#pragma varlocate 14 B1D7
#pragma varlocate 14 B1D7bits
#pragma varlocate 14 CANSTAT_RO8
#pragma varlocate 14 CANSTAT_RO8bits
#pragma varlocate 14 CANCON_RO8
#pragma varlocate 14 CANCON_RO8bits
#pragma varlocate 14 B2CON
#pragma varlocate 14 B2CONbits
#pragma varlocate 14 B2SIDH
#pragma varlocate 14 B2SIDHbits
#pragma varlocate 14 B2SIDL
#pragma varlocate 14 B2SIDLbits
#pragma varlocate 14 B2EIDH
#pragma varlocate 14 B2EIDHbits
#pragma varlocate 14 B2EIDL
#pragma varlocate 14 B2EIDLbits
#pragma varlocate 14 B2DLC
#pragma varlocate 14 B2DLCbits
#pragma varlocate 14 B2D0
#pragma varlocate 14 B2D0bits
#pragma varlocate 14 B2D1
#pragma varlocate 14 B2D1bits
#pragma varlocate 14 B2D2
#pragma varlocate 14 B2D2bits
#pragma varlocate 14 B2D3
#pragma varlocate 14 B2D3bits
#pragma varlocate 14 B2D4
#pragma varlocate 14 B2D4bits
#pragma varlocate 14 B2D5
#pragma varlocate 14 B2D5bits
#pragma varlocate 14 B2D6
#pragma varlocate 14 B2D6bits
#pragma varlocate 14 B2D7
#pragma varlocate 14 B2D7bits
#pragma varlocate 14 CANSTAT_RO7
#pragma varlocate 14 CANSTAT_RO7bits
#pragma varlocate 14 CANCON_RO7
#pragma varlocate 14 CANCON_RO7bits
#pragma varlocate 14 B3CON
#pragma varlocate 14 B3CONbits
#pragma varlocate 14 B3SIDH
#pragma varlocate 14 B3SIDHbits
#pragma varlocate 14 B3SIDL
#pragma varlocate 14 B3SIDLbits
#pragma varlocate 14 B3EIDH
#pragma varlocate 14 B3EIDHbits
#pragma varlocate 14 B3EIDL
#pragma varlocate 14 B3EIDLbits
#pragma varlocate 14 B3DLC
#pragma varlocate 14 B3DLCbits
#pragma varlocate 14 B3D0
#pragma varlocate 14 B3D0bits
#pragma varlocate 14 B3D1
#pragma varlocate 14 B3D1bits
#pragma varlocate 14 B3D2
#pragma varlocate 14 B3D2bits
#pragma varlocate 14 B3D3
#pragma varlocate 14 B3D3bits
#pragma varlocate 14 B3D4
#pragma varlocate 14 B3D4bits
#pragma varlocate 14 B3D5
#pragma varlocate 14 B3D5bits
#pragma varlocate 14 B3D6
#pragma varlocate 14 B3D6bits
#pragma varlocate 14 B3D7
#pragma varlocate 14 B3D7bits
#pragma varlocate 14 CANSTAT_RO6
#pragma varlocate 14 CANSTAT_RO6bits
#pragma varlocate 14 CANCON_RO6
#pragma varlocate 14 CANCON_RO6bits
#pragma varlocate 14 B4CON
#pragma varlocate 14 B4CONbits
#pragma varlocate 14 B4SIDH
#pragma varlocate 14 B4SIDHbits
#pragma varlocate 14 B4SIDL
#pragma varlocate 14 B4SIDLbits
#pragma varlocate 14 B4EIDH
#pragma varlocate 14 B4EIDHbits
#pragma varlocate 14 B4EIDL
#pragma varlocate 14 B4EIDLbits
#pragma varlocate 14 B4DLC
#pragma varlocate 14 B4DLCbits
#pragma varlocate 14 B4D0
#pragma varlocate 14 B4D0bits
#pragma varlocate 14 B4D1
#pragma varlocate 14 B4D1bits
#pragma varlocate 14 B4D2
#pragma varlocate 14 B4D2bits
#pragma varlocate 14 B4D3
#pragma varlocate 14 B4D3bits
#pragma varlocate 14 B4D4
#pragma varlocate 14 B4D4bits
#pragma varlocate 14 B4D5
#pragma varlocate 14 B4D5bits
#pragma varlocate 14 B4D6
#pragma varlocate 14 B4D6bits
#pragma varlocate 14 B4D7
#pragma varlocate 14 B4D7bits
#pragma varlocate 14 CANSTAT_RO5
#pragma varlocate 14 CANSTAT_RO5bits
#pragma varlocate 14 CANCON_RO5
#pragma varlocate 14 CANCON_RO5bits
#pragma varlocate 14 B5CON
#pragma varlocate 14 B5CONbits
#pragma varlocate 14 B5SIDH
#pragma varlocate 14 B5SIDHbits
#pragma varlocate 14 B5SIDL
#pragma varlocate 14 B5SIDLbits
#pragma varlocate 14 B5EIDH
#pragma varlocate 14 B5EIDHbits
#pragma varlocate 14 B5EIDL
#pragma varlocate 14 B5EIDLbits
#pragma varlocate 14 B5DLC
#pragma varlocate 14 B5DLCbits
#pragma varlocate 14 B5D0
#pragma varlocate 14 B5D0bits
#pragma varlocate 14 B5D1
#pragma varlocate 14 B5D1bits
#pragma varlocate 14 B5D2
#pragma varlocate 14 B5D2bits
#pragma varlocate 14 B5D3
#pragma varlocate 14 B5D3bits
#pragma varlocate 14 B5D4
#pragma varlocate 14 B5D4bits
#pragma varlocate 14 B5D5
#pragma varlocate 14 B5D5bits
#pragma varlocate 14 B5D6
#pragma varlocate 14 B5D6bits
#pragma varlocate 14 B5D7
#pragma varlocate 14 B5D7bits
#pragma varlocate 14 CANSTAT_RO4
#pragma varlocate 14 CANSTAT_RO4bits
#pragma varlocate 14 CANCON_RO4
#pragma varlocate 14 CANCON_RO4bits
#pragma varlocate 15 RXF0SIDH
#pragma varlocate 15 RXF0SIDHbits
#pragma varlocate 15 RXF0SIDL
#pragma varlocate 15 RXF0SIDLbits
#pragma varlocate 15 RXF0EIDH
#pragma varlocate 15 RXF0EIDHbits
#pragma varlocate 15 RXF0EIDL
#pragma varlocate 15 RXF0EIDLbits
#pragma varlocate 15 RXF1SIDH
#pragma varlocate 15 RXF1SIDHbits
#pragma varlocate 15 RXF1SIDL
#pragma varlocate 15 RXF1SIDLbits
#pragma varlocate 15 RXF1EIDH
#pragma varlocate 15 RXF1EIDHbits
#pragma varlocate 15 RXF1EIDL
#pragma varlocate 15 RXF1EIDLbits
#pragma varlocate 15 RXF2SIDH
#pragma varlocate 15 RXF2SIDHbits
#pragma varlocate 15 RXF2SIDL
#pragma varlocate 15 RXF2SIDLbits
#pragma varlocate 15 RXF2EIDH
#pragma varlocate 15 RXF2EIDHbits
#pragma varlocate 15 RXF2EIDL
#pragma varlocate 15 RXF2EIDLbits
#pragma varlocate 15 RXF3SIDH
#pragma varlocate 15 RXF3SIDHbits
#pragma varlocate 15 RXF3SIDL
#pragma varlocate 15 RXF3SIDLbits
#pragma varlocate 15 RXF3EIDH
#pragma varlocate 15 RXF3EIDHbits
#pragma varlocate 15 RXF3EIDL
#pragma varlocate 15 RXF3EIDLbits
#pragma varlocate 15 RXF4SIDH
#pragma varlocate 15 RXF4SIDHbits
#pragma varlocate 15 RXF4SIDL
#pragma varlocate 15 RXF4SIDLbits
#pragma varlocate 15 RXF4EIDH
#pragma varlocate 15 RXF4EIDHbits
#pragma varlocate 15 RXF4EIDL
#pragma varlocate 15 RXF4EIDLbits
#pragma varlocate 15 RXF5SIDH
#pragma varlocate 15 RXF5SIDHbits
#pragma varlocate 15 RXF5SIDL
#pragma varlocate 15 RXF5SIDLbits
#pragma varlocate 15 RXF5EIDH
#pragma varlocate 15 RXF5EIDHbits
#pragma varlocate 15 RXF5EIDL
#pragma varlocate 15 RXF5EIDLbits
#pragma varlocate 15 RXM0SIDH
#pragma varlocate 15 RXM0SIDHbits
#pragma varlocate 15 RXM0SIDL
#pragma varlocate 15 RXM0SIDLbits
#pragma varlocate 15 RXM0EIDH
#pragma varlocate 15 RXM0EIDHbits
#pragma varlocate 15 RXM0EIDL
#pragma varlocate 15 RXM0EIDLbits
#pragma varlocate 15 RXM1SIDH
#pragma varlocate 15 RXM1SIDHbits
#pragma varlocate 15 RXM1SIDL
#pragma varlocate 15 RXM1SIDLbits
#pragma varlocate 15 RXM1EIDH
#pragma varlocate 15 RXM1EIDHbits
#pragma varlocate 15 RXM1EIDL
#pragma varlocate 15 RXM1EIDLbits
#pragma varlocate 15 TXB2CON
#pragma varlocate 15 TXB2CONbits
#pragma varlocate 15 TXB2SIDH
#pragma varlocate 15 TXB2SIDHbits
#pragma varlocate 15 TXB2SIDL
#pragma varlocate 15 TXB2SIDLbits
#pragma varlocate 15 TXB2EIDH
#pragma varlocate 15 TXB2EIDHbits
#pragma varlocate 15 TXB2EIDL
#pragma varlocate 15 TXB2EIDLbits
#pragma varlocate 15 TXB2DLC
#pragma varlocate 15 TXB2DLCbits
#pragma varlocate 15 TXB2D0
#pragma varlocate 15 TXB2D0bits
#pragma varlocate 15 TXB2D1
#pragma varlocate 15 TXB2D1bits
#pragma varlocate 15 TXB2D2
#pragma varlocate 15 TXB2D2bits
#pragma varlocate 15 TXB2D3
#pragma varlocate 15 TXB2D3bits
#pragma varlocate 15 TXB2D4
#pragma varlocate 15 TXB2D4bits
#pragma varlocate 15 TXB2D5
#pragma varlocate 15 TXB2D5bits
#pragma varlocate 15 TXB2D6
#pragma varlocate 15 TXB2D6bits
#pragma varlocate 15 TXB2D7
#pragma varlocate 15 TXB2D7bits
#pragma varlocate 15 CANSTAT_RO3
#pragma varlocate 15 CANSTAT_RO3bits
#pragma varlocate 15 CANCON_RO3
#pragma varlocate 15 CANCON_RO3bits
#pragma varlocate 15 TXB1CON
#pragma varlocate 15 TXB1CONbits
#pragma varlocate 15 TXB1SIDH
#pragma varlocate 15 TXB1SIDHbits
#pragma varlocate 15 TXB1SIDL
#pragma varlocate 15 TXB1SIDLbits
#pragma varlocate 15 TXB1EIDH
#pragma varlocate 15 TXB1EIDHbits
#pragma varlocate 15 TXB1EIDL
#pragma varlocate 15 TXB1EIDLbits
#pragma varlocate 15 TXB1DLC
#pragma varlocate 15 TXB1DLCbits
#pragma varlocate 15 TXB1D0
#pragma varlocate 15 TXB1D0bits
#pragma varlocate 15 TXB1D1
#pragma varlocate 15 TXB1D1bits
#pragma varlocate 15 TXB1D2
#pragma varlocate 15 TXB1D2bits
#pragma varlocate 15 TXB1D3
#pragma varlocate 15 TXB1D3bits
#pragma varlocate 15 TXB1D4
#pragma varlocate 15 TXB1D4bits
#pragma varlocate 15 TXB1D5
#pragma varlocate 15 TXB1D5bits
#pragma varlocate 15 TXB1D6
#pragma varlocate 15 TXB1D6bits
#pragma varlocate 15 TXB1D7
#pragma varlocate 15 TXB1D7bits
#pragma varlocate 15 CANSTAT_RO2
#pragma varlocate 15 CANSTAT_RO2bits
#pragma varlocate 15 CANCON_RO2
#pragma varlocate 15 CANCON_RO2bits
#pragma varlocate 15 TXB0CON
#pragma varlocate 15 TXB0CONbits
#pragma varlocate 15 TXB0SIDH
#pragma varlocate 15 TXB0SIDHbits
#pragma varlocate 15 TXB0SIDL
#pragma varlocate 15 TXB0SIDLbits
#pragma varlocate 15 TXB0EIDH
#pragma varlocate 15 TXB0EIDHbits
#pragma varlocate 15 TXB0EIDL
#pragma varlocate 15 TXB0EIDLbits
#pragma varlocate 15 TXB0DLC
#pragma varlocate 15 TXB0DLCbits
#pragma varlocate 15 TXB0D0
#pragma varlocate 15 TXB0D0bits
#pragma varlocate 15 TXB0D1
#pragma varlocate 15 TXB0D1bits
#pragma varlocate 15 TXB0D2
#pragma varlocate 15 TXB0D2bits
#pragma varlocate 15 TXB0D3
#pragma varlocate 15 TXB0D3bits
#pragma varlocate 15 TXB0D4
#pragma varlocate 15 TXB0D4bits
#pragma varlocate 15 TXB0D5
#pragma varlocate 15 TXB0D5bits
#pragma varlocate 15 TXB0D6
#pragma varlocate 15 TXB0D6bits
#pragma varlocate 15 TXB0D7
#pragma varlocate 15 TXB0D7bits
#pragma varlocate 15 CANSTAT_RO1
#pragma varlocate 15 CANSTAT_RO1bits
#pragma varlocate 15 CANCON_RO1
#pragma varlocate 15 CANCON_RO1bits
#pragma varlocate 15 RXB1CON
#pragma varlocate 15 RXB1CONbits
#pragma varlocate 15 RXB1SIDH
#pragma varlocate 15 RXB1SIDHbits
#pragma varlocate 15 RXB1SIDL
#pragma varlocate 15 RXB1SIDLbits
#pragma varlocate 15 RXB1EIDH
#pragma varlocate 15 RXB1EIDHbits
#pragma varlocate 15 RXB1EIDL
#pragma varlocate 15 RXB1EIDLbits
#pragma varlocate 15 RXB1DLC
#pragma varlocate 15 RXB1DLCbits
#pragma varlocate 15 RXB1D0
#pragma varlocate 15 RXB1D0bits
#pragma varlocate 15 RXB1D1
#pragma varlocate 15 RXB1D1bits
#pragma varlocate 15 RXB1D2
#pragma varlocate 15 RXB1D2bits
#pragma varlocate 15 RXB1D3
#pragma varlocate 15 RXB1D3bits
#pragma varlocate 15 RXB1D4
#pragma varlocate 15 RXB1D4bits
#pragma varlocate 15 RXB1D5
#pragma varlocate 15 RXB1D5bits
#pragma varlocate 15 RXB1D6
#pragma varlocate 15 RXB1D6bits
#pragma varlocate 15 RXB1D7
#pragma varlocate 15 RXB1D7bits
#pragma varlocate 15 CANSTAT_RO0
#pragma varlocate 15 CANSTAT_RO0bits
#pragma varlocate 15 CANCON_RO0
#pragma varlocate 15 CANCON_RO0bits

/*-------------------------------------------------------------------------
 * Some useful defines for inline assembly stuff
 *-------------------------------------------------------------------------*/
#define ACCESS 0
#define BANKED 1

/*-------------------------------------------------------------------------
 * Some useful macros for inline assembly stuff
 *-------------------------------------------------------------------------*/
#define Nop()    {_asm nop _endasm}
#define ClrWdt() {_asm clrwdt _endasm}
#define Sleep()  {_asm sleep _endasm}
#define Reset()  {_asm reset _endasm}

#define Rlcf(f,dest,access)  {_asm movlb f rlcf f,dest,access _endasm}
#define Rlncf(f,dest,access) {_asm movlb f rlncf f,dest,access _endasm}
#define Rrcf(f,dest,access)  {_asm movlb f rrcf f,dest,access _endasm}
#define Rrncf(f,dest,access) {_asm movlb f rrncf f,dest,access _endasm}
#define Swapf(f,dest,access) {_asm movlb f swapf f,dest,access _endasm }

/*-------------------------------------------------------------------------
 * A fairly inclusive set of registers to save for interrupts.
 * These are locations which are commonly used by the compiler.
 *-------------------------------------------------------------------------*/
#define INTSAVELOCS TBLPTR, TABLAT, PROD


#endif
