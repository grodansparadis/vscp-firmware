//
// sja1000.h
//
// Copyright (C) 2002 Ake Hedman, Eurosource, akhe@users.sourceforge.net
//
// This software is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,
// Boston, MA 02111-1307, USA.
//
// parts from code written by Ake Hedman, CC Systems AB
// parts derived from code written by Arnaud Westenberg 
// arnaud@wanadoo.nl and his can drive and
// Version 0.6.1 T.Motylewski@bfad.de 13.03.2001


// Prototypes
void sja1000_enableIRQ( void );
void sja1000_disableIRQ( void );
int sja1000_enableConfig( void );
int sja1000_disableConfig( void );
int sja1000_init( void );
int sja1000_setExtendedMask( u32 code, u32 mask );
int sja1000_setBaudrate( u32 rate,
			  u32 clock,
			  u32 sjw,
			  u32 sampl_pt,
			  u32 flags );
void sja1000_readMsg( void );
int sja1000_prereadConfig( void );
int sja1000_readMsgFromBuf( canmsg_t *pmsg );
int sja1000_sendMsg( canmsg_t *pmsg );
void sja1000_writeMsg( canmsg_t *pmsg );
int sja1000_writeMsgToBuf( canmsg_t *pmsg );
int sja1000_dosendMsg( int bRtr );
int sja1000_check_tx_stat( void );
int sja1000_setbtregs( u16 btr0, u16 btr1);
int sja1000_remoteRequest( void );
int sja1000_standardMask( u16 code, u16 mask );
u16 getRXMsgCnt( void );
u16 getTXMsgCnt( void );

#define REG(a)   *(unsigned char *)(a)   


/**
  ========================================================================
                                    PeliCAN mode 
  ========================================================================
*/

/// Mode register
#define SJAMOD          (CAN_BASE + 0x00)

/// Command register
#define SJACMR          (CAN_BASE + 0x01)

/// Status register
#define SJASR           (CAN_BASE + 0x02)

/// Interrupt register
#define SJAIR           (CAN_BASE + 0x03)

/// Interrupt Enable
#define SJAIER          (CAN_BASE + 0x04)

/// Bus Timing register 0
#define SJABTR0         (CAN_BASE + 0x06)

/// Bus Timing register 1
#define SJABTR1         (CAN_BASE + 0x07)

/// Output Control register
#define SJAOCR          (CAN_BASE + 0x08)

/// Arbitration Lost Capture
#define SJAALC          (CAN_BASE + 0x0b)

/// Error Code Capture
#define SJAECC          (CAN_BASE + 0x0c)

/// Error Warning Limit
#define SJAEWLR         (CAN_BASE + 0x0d)

/// RX Error Counter
#define SJARXERR        (CAN_BASE + 0x0e)

/// TX Error Counter
#define SJARXERRCNT     (CAN_BASE + 0x0e)
#define SJATXERRCNT     (CAN_BASE + 0x0f)

/// Rx Message Counter (number of msgs. in RX FIFO
#define SJARXMC         (CAN_BASE + 0x1d)

/// Rx Buffer Start Addr. (address of current MSG)
#define SJARXBSA        (CAN_BASE + 0x1e)

/// Transmit Buffer (write) Receive Buffer (read) Frame Information
#define SJAFRM          (CAN_BASE + 0x10)

// ID bytes (11 bits in 0 and 1 or 16 bits in 0,1 
//  and 13 bits in 2,3 (extended))
#define SJAID0          (CAN_BASE + 0x11)
#define SJAID1          (CAN_BASE + 0x12)

/// ID cont. for extended frames
#define SJAID2          (CAN_BASE + 0x13) 
#define SJAID3          (CAN_BASE + 0x14)

/// Data start standard frame
#define SJADATS         (CAN_BASE + 0x13)

/// Data start extended frame
#define SJADATE         (CAN_BASE + 0x15)

/// Acceptance Code (4 bytes) in RESET mode
#define SJAACR0         (CAN_BASE + 0x10)

/// Acceptance Mask (4 bytes) in RESET mode
#define SJAAMR0         (CAN_BASE + 0x14)

/// 4 bytes
#define SJA_PeliCAN_AC_LEN 4 

/// Clock Divider
#define SJACDR          (CAN_BASE + 0x1f)

/** 
    Mode Register 0x00 
*/
#define MOD_SM   4     // Sleep Mode (writable only in OPERATING mode)
#define MOD_AFM  3     // Acceptance Filter Mode (writable only in RESET)
#define MOD_STM  2     // Self Test Mode (writable only in RESET)
#define MOD_LOM  1     // Listen Only Mode (writable only in RESET)
#define MOD_RM   0     // Reset Mode

/** 
    Command Register 0x01 
*/
#define  CMR_SRR  4    // Self Reception Request (GoToSleep in BASIC mode)
#define  CMR_CDO  3    // Clear Data Overrun
#define  CMR_RRB  2    // Release Receive Buffer
#define  CMR_AT   1    // Abort Transmission
#define  CMR_TR   0    // Transmission Request

/** 
    Status Register 0x02 
*/
#define  SR_BS    7    // Bus Status
#define  SR_ES    6    // Error Status
#define  SR_TS    5    // Transmit Status
#define  SR_RS    4    // Receive Status
#define  SR_TCS   3    // Transmission Complete Status
#define  SR_TBS   2    // Transmit Buffer Status
#define  SR_DOS   1    // Data Overrun Status
#define  SR_RBS   0    // Receive Buffer Status

/** 
    Interrupt Enable Register 0x04 
*/
#define  IER_BEIE  7    // Bus Error Interrupt Enable
#define  IER_ALIE  6    // Arbitration Lost Interrupt Enable
#define  IER_EPIE  5    // Error Passive Interrupt Enable
#define  IER_WUIE  4    // Wake-Up Interrupt Enable
#define  IER_DOIE  3    // Data Overrun Interrupt Enable
#define  IER_EIE   2    // Error Warning Interrupt Enable
#define  IER_TIE   1    // Transmit Interrupt Enable
#define  IER_RIE   0    // Receive Interrupt Enable

// WARNING: the chip automatically enters RESET (bus off) mode when 
// error counter > 255

/** 
    Arbitration Lost Capture Register 0x0b.
    Counting starts from 0 (bit1 of ID). Bits 5-7 reserved
*/
#define  ALC_SRTR 0x0b    // Arbitration lost in bit SRTR
#define  ALC_IDE  0x1c    // Arbitration lost in bit IDE
#define  ALC_RTR  0x1f	  // Arbitration lost in RTR


/** 
    Error Code Capture Register 0x0c
*/
#define  ECC_ERCC1 7
#define  ECC_ERCC0 6
#define  ECC_BIT   0
#define  ECC_FORM  ECC_ERCC0
#define  ECC_STUFF ECC_ERCC1
#define  ECC_OTHER ECC_ERCC0 | ECC_ERCC1
#define  ECC_DIR   0x20	 // 1 == RX, 0 == TX
#define  ECC_SEG_M 0x1f  // Segment mask, see page 37 of SJA1000 Data Sheet


/** 
    Frame format information 0x10 
*/
#define  FRM_FF    7     // Frame Format 1 == extended,` 0 == standard
#define  FRM_RTR   6     // Remote request
#define  FRM_DLC_M 0x0f  // Length Mask


/** 
    Interrupt (status) Register 0x03 
*/
#define  IR_BEI	7    // Bus Error Interrupt
#define  IR_ALI	6    // Arbitration Lost Interrupt
#define  IR_EPI	5    // Error Passive Interrupt 
                        // (entered error passive state or error active state)
#define  IR_WUI 4    // Wake-Up Interrupt
#define  IR_DOI 3    // Data Overrun Interrupt
#define  IR_EI  2    // Error Interrupt
#define  IR_TI  1    // Transmit Interrupt
#define  IR_RI  0    // Receive Interrupt

/** 
    Bus Timing 1 Register 0x07 
*/
#define  MAX_TSEG1  0x0f
#define  MAX_TSEG2  0x07 


/** 
    Output Control Register 0x08 
*/
#define  OCR_MODE_BIPHASE  0x00
#define  OCR_MODE_TEST     0x01
#define  OCR_MODE_NORMAL   0x02
#define  OCR_MODE_CLOCK    0x03
/// TX0 push-pull not inverted
#define  OCR_TX0_LH        0x18
/// TX1 floating (off)
#define  OCR_TX1_ZZ        0x00

/** 
    Clock Divider register 0x1f 
*/
#define  CDR_PELICAN       7 
/// bypass input comparator
#define  CDR_CBP           6
/// switch TX1 to generate RX INT
#define  CDR_RXINPEN       5 
#define  CDR_CLK_OFF       4 
/// f_out = f_osc/(2*(CDR[2:0]+1)) or f_osc if CDR[2:0]==7
#define  CDR_CLKOUT_MASK   0x07

/** 
    flags for sja1000_baud_rate 
*/
#define BTR1_SAM           0x02


// Defaults

/** 
    This is the base address for the SJA1000 CAN controller
    on the CANDIP board.
*/
#define CAN_BASE  0x4000

#define CAN_RX_BUFFER_SIZE      8
#define CAN_TX_BUFFER_SIZE      8

#define INIT_VALUE_CRYSTAL   16000000
#define INIT_VALUE_BAUDRATE  250000 


