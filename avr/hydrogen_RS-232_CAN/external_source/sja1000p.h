/** 
   sja1000p.h
   Header file for the Linux CAN-bus driver.
   Written by Arnaud Westenberg email:arnaud@wanadoo.nl
   This software is released under the GPL-License.
   Version 0.6.1 T.Motylewski@bfad.de 13.03.2001
   See app. note an97076.pdf from Philips Semiconductors 
   and SJA1000 data sheet
   PELICAN mode
 */

int sja1000p_chip_config( struct chip_t *pchip );
int sja1000p_extended_mask( struct chip_t *pchip, 
			    u32 code, 
			    u32 mask);
int sja1000p_baud_rate( struct chip_t *pchip, 
			u32 rate, 
			u32 clock, 
			u32 sjw,
			u32 sampl_pt, 
			u32 flags);
int sja1000p_pre_read_config( struct chip_t *pchip );
int sja1000p_pre_write_config( struct chip_t *pchip, 
			       struct canmsg_t *pmsg );
int sja1000p_send_msg( struct chip_t *pchip, int bRtr );


/** 
    PeliCAN mode 
*/
enum SJA1000_PeliCAN_regs {
  ///
  SJAMOD	= 0x00,

  /// Command register
  SJACMR 	= 0x01,

  /// Status register
  SJASR	= 0x02,

  /// Interrupt register
  SJAIR	= 0x03,

  /// Interrupt Enable
  SJAIER	= 0x04,

  /// Bus Timing register 0
  SJABTR0 = 0x06,

  /// Bus Timing register 1
  SJABTR1	= 0x07,

  /// Output Control register
  SJAOCR	= 0x08,

  /// Arbitration Lost Capture
  SJAALC	= 0x0b,

  /// Error Code Capture
  SJAECC	= 0x0c,

  /// Error Warning Limit
  SJAEWLR = 0x0d,

  /// RX Error Counter
  SJARXERR = 0x0e,

  /// TX Error Counter
  SJATXERR0 = 0x0e,
  SJATXERR1 = 0x0f,

  /// Rx Message Counter (number of msgs. in RX FIFO
  SJARMC	= 0x1d,

  /// Rx Buffer Start Addr. (address of current MSG)
  SJARBSA	= 0x1e,

  /// Transmit Buffer (write) Receive Buffer (read) Frame Information
  SJAFRM = 0x10,

  // ID bytes (11 bits in 0 and 1 or 16 bits in 0,1 
  //  and 13 bits in 2,3 (extended))
  SJAID0 = 0x11, SJAID1 = 0x12, 

  /// ID cont. for extended frames
  SJAID2 = 0x13, SJAID3 = 0x14,

  /// Data start standard frame
  SJADATS = 0x13,

  /// Data start extended frame
  SJADATE = 0x15,

  /// Acceptance Code (4 bytes) in RESET mode
  SJAACR0	= 0x10,

  /// Acceptance Mask (4 bytes) in RESET mode
  SJAAMR0	= 0x14,

  /// 4 bytes
  SJA_PeliCAN_AC_LEN = 4, 

  /// Clock Divider
  SJACDR = 0x1f
};

/** 
    Mode Register 0x00 
*/
enum sja1000_PeliCAN_MOD {
  MOD_SM = 1<<4,	// Sleep Mode (writable only in OPERATING mode)
  MOD_AFM= 1<<3,	// Acceptance Filter Mode (writable only in RESET)
  MOD_STM= 1<<2,	// Self Test Mode (writable only in RESET)
  MOD_LOM= 1<<1,	// Listen Only Mode (writable only in RESET)
  MOD_RM = 1	// Reset Mode
};

/** 
    Command Register 0x01 
*/
enum sja1000_PeliCAN_CMR { 
  CMR_SRR = 1<<4, // Self Reception Request (GoToSleep in BASIC mode)
  CMR_CDO = 1<<3, // Clear Data Overrun
  CMR_RRB = 1<<2, // Release Receive Buffer
  CMR_AT = 1<<1,  // Abort Transmission
  CMR_TR = 1 };   // Transmission Request

/** 
    Status Register 0x02 
*/
enum sja1000_SR {
  SR_BS = 1<<7,   // Bus Status
  SR_ES = 1<<6,   // Error Status
  SR_TS = 1<<5,   // Transmit Status
  SR_RS = 1<<4,   // Receive Status
  SR_TCS = 1<<3,  // Transmission Complete Status
  SR_TBS = 1<<2,  // Transmit Buffer Status
  SR_DOS = 1<<1,  // Data Overrun Status
  SR_RBS = 1 };   // Receive Buffer Status

/** 
    Interrupt Enable Register 0x04 
*/
enum sja1000_PeliCAN_IER {
  IER_BEIE= 1<<7,	// Bus Error Interrupt Enable
  IER_ALIE= 1<<6,	// Arbitration Lost Interrupt Enable
  IER_EPIE= 1<<5,	// Error Passive Interrupt Enable
  IER_WUIE= 1<<4,	// Wake-Up Interrupt Enable
  IER_DOIE = 1<<3,      // Data Overrun Interrupt Enable
  IER_EIE = 1<<2,       // Error Warning Interrupt Enable
  IER_TIE = 1<<1,       // Transmit Interrupt Enable
  IER_RIE = 1,  	// Receive Interrupt Enable
  ENABLE_INTERRUPTS = IER_BEIE|IER_EPIE|IER_DOIE|IER_EIE|IER_TIE|IER_RIE,
  DISABLE_INTERRUPTS = 0
  // WARNING: the chip automatically enters RESET (bus off) mode when 
  // error counter > 255
};

/** 
    Arbitration Lost Capture Register 0x0b.
    Counting starts from 0 (bit1 of ID). Bits 5-7 reserved
*/
enum sja1000_PeliCAN_ALC {
  ALC_SRTR = 0x0b,       // Arbitration lost in bit SRTR
  ALC_IDE	= 0x1c,  // Arbitration lost in bit IDE
  ALC_RTR = 0x1f,	 // Arbitration lost in RTR
};

/** 
    Error Code Capture Register 0x0c
*/
enum sja1000_PeliCAN_ECC {
  ECC_ERCC1 = 1<<7,
  ECC_ERCC0 = 1<<6,
  ECC_BIT = 0,
  ECC_FORM = ECC_ERCC0,
  ECC_STUFF = ECC_ERCC1,
  ECC_OTHER = ECC_ERCC0 | ECC_ERCC1,
  ECC_DIR = 1<<5,	 // 1 == RX, 0 == TX
  ECC_SEG_M = (1<<5) -1  // Segment mask, see page 37 of SJA1000 Data Sheet
};

/** 
    Frame format information 0x10 
*/
enum sja1000_PeliCAN_FRM {
  FRM_FF = 1<<7,        // Frame Format 1 == extended, 0 == standard
  FRM_RTR = 1<<6,       // Remote request
  FRM_DLC_M = (1<<4)-1  // Length Mask
};


/** 
    Interrupt (status) Register 0x03 
*/
enum sja1000_PeliCAN_IR {
  IR_BEI	= 1<<7, // Bus Error Interrupt
  IR_ALI	= 1<<6,	// Arbitration Lost Interrupt
  IR_EPI	= 1<<5,	// Error Passive Interrupt 
                        // (entered error passive state or error active state)
  IR_WUI = 1<<4,        // Wake-Up Interrupt
  IR_DOI = 1<<3,        // Data Overrun Interrupt
  IR_EI = 1<<2,         // Error Interrupt
  IR_TI = 1<<1,         // Transmit Interrupt
  IR_RI = 1             // Receive Interrupt
};

#if 0
/** 
    Bus Timing 1 Register 0x07 
*/
enum sja1000_BTR1 {
  MAX_TSEG1 = 15,
  MAX_TSEG2 = 7
};
#endif

/** 
    Output Control Register 0x08 
*/
enum sja1000_OCR {
  OCR_MODE_BIPHASE = 0,
  OCR_MODE_TEST = 1,
  OCR_MODE_NORMAL = 2,
  OCR_MODE_CLOCK = 3,
  /// TX0 push-pull not inverted
  OCR_TX0_LH = 0x18,
  /// TX1 floating (off)
  OCR_TX1_ZZ = 0
};

/** 
    Clock Divider register 0x1f 
*/
enum sja1000_CDR {
  CDR_PELICAN = 1<<7,
  /// bypass input comparator
  CDR_CBP = 1<<6,
  /// switch TX1 to generate RX INT
  CDR_RXINPEN = 1<<5,
  CDR_CLK_OFF = 1<<3,
  /// f_out = f_osc/(2*(CDR[2:0]+1)) or f_osc if CDR[2:0]==7
  CDR_CLKOUT_MASK = 7
};

/** 
flags for sja1000_baud_rate 
*/
#define BTR1_SAM (1<<1)



