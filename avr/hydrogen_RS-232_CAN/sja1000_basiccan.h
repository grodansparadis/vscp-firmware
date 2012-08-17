/**
    =========================================================================
                                    BasicCAN mode 
    =========================================================================
*/
/*
// Control register 
#define SJACR *(unsigned char *) (CAN_BASE + 0x00)

// Command register 
#define  SJACMR *(unsigned char *) (CAN_BASE + 0x01)	

// Status register 
#define  SJASR *(unsigned char *) (CAN_BASE + 0x02)

// Interrupt register 
#define  SJAIR *(unsigned char *) (CAN_BASE + 0x03)	

// Acceptance Code register 
#define  SJAACR *(unsigned char *) (CAN_BASE + 0x04)	

// Acceptance Mask Register 
#define  SJAAMR *(unsigned char *) (CAN_BASE + 0x05)	

// Bus Timing register 0 
#define  SJABTR0 *(unsigned char *) (CAN_BASE + 0x06)

// Bus Timing register 1 	
#define  SJABTR1 *(unsigned char *) (CAN_BASE + 0x07)	

// Output Control register 
#define  SJAOCR *(unsigned char *) (CAN_BASE + 0x08)

// Clock Divider register 
#define  SJACDR *(unsigned char *) (CAN_BASE + 0x1f)	
  
// Identifier byte 1 
#define  SJATXID1 *(unsigned char *) (CAN_BASE + 0x0a)	

// Identifier byte 0 
#define  SJATXID0 *(unsigned char *) (CAN_BASE + 0x0b)	

// First data byte 
#define  SJATXDAT0 *(unsigned char *) (CAN_BASE + 0x0c)	
#define  SJATXDAT1 *(unsigned char *) (CAN_BASE + 0x0d)
#define  SJATXDAT2 *(unsigned char *) (CAN_BASE + 0x0e)
#define  SJATDDAT3 *(unsigned char *) (CAN_BASE + 0x0f)
#define  SJATXDAT4 *(unsigned char *) (CAN_BASE + 0x10)
#define  SJATXDAT5 *(unsigned char *) (CAN_BASE + 0x11)
#define  SJATXDAT6 *(unsigned char *) (CAN_BASE + 0x12)
#define  SJATXDAT7 *(unsigned char *) (CAN_BASE + 0x13)
  
// Identifier byte 1 
#define  SJARXID1 *(unsigned char *) (CAN_BASE + 0x14)

// Identifier byte 0 
#define  SJARXID0 *(unsigned char *) (CAN_BASE + 0x15)	

// First data byte 
#define  SJARXDAT0 *(unsigned char *) (CAN_BASE + 0x16)	
#define  SJARXDAT1 *(unsigned char *) (CAN_BASE + 0x17)
#define  SJARXDAT2 *(unsigned char *) (CAN_BASE + 0x18)
#define  SJARXDAT3 *(unsigned char *) (CAN_BASE + 0x19)
#define  SJARXDAT4 *(unsigned char *) (CAN_BASE + 0x1a)
#define  SJARXDAT5 *(unsigned char *) (CAN_BASE + 0x1b)
#define  SJARXDAT6 *(unsigned char *) (CAN_BASE + 0x1c)
#define  SJARXDAT7 *(unsigned char *) (CAN_BASE + 0x1d)

/**
    Command register 
*/

#define  CMR_TR  1        // Transmission request
#define  CMR_AT  1<<1     // Abort Transmission
#define  CMR_RRB 1<<2     // Release Receive Buffer
#define  CMR_CDO 1<<3     // Clear Data Overrun
#define  CMR_GTS 1<<4	  // Go To Sleep

/** 
    Status Register 
*/

#define  SR_RBS  1	  // Receive Buffer Status
#define  SR_DOS  1<<1	  // Data Overrun Status
#define  SR_TBS  1<<2	  // Transmit Buffer Status
#define  SR_TCS  1<<3	  // Transmission Complete Status
#define  SR_RS   1<<4	  // Receive Status
#define  SR_TS   1<<5	  // Transmit Status
#define  SR_ES   1<<6	  // Error Status
#define  SR_BS   1<<7	  // Bus Status

/**
    Control Register 
*/

#define CR_RR    1	  // Reset Request
#define CR_RIE   1<<1	  // Receive Interrupt Enable
#define CR_TIE   1<<2	  // Transmit Interrupt Enable
#define CR_EIE   1<<3	  // Error Interrupt Enable
#define CR_OIE   1<<4	  // Overrun Interrupt Enable

/** 
    Interrupt (status) Register 
*/

#define  IR_RI   1	  // Receive Interrupt
#define  IR_TI   1<<1	  // Transmit Interrupt
#define  IR_EI   1<<2	  // Error Interrupt
#define  IR_DOI  1<<3	  // Data Overrun Interrupt
#define  IR_WUI  1<<4	  // Wake-Up Interrupt

/** 
    Clock Divider Register 
    f_out = f_osc/(2*(CDR[2:0]+1)) or f_osc if CDR[2:0]==7 
*/
#define CDR_CLKOUT_MASK 0x07
#define CDR_CLK_OFF     0x08	// Clock Off
#define CDR_RXINPEN     0x20	// TX1 output is RX irq output
#define CDR_CBP         0x40	// Input Comparator By-Pass
#define CDR_PELICAN     0x80	// PeliCAN Mode 


/** 
    Output Control Register 
*/
#define OCR_MODE_BIPHASE  0
#define OCR_MODE_TEST     1
#define OCR_MODE_NORMAL   2
#define OCR_MODE_CLOCK    3

// TX0 push-pull not inverted
//#define OCR_TX0_LH  x18

// TX0 push-pull inverted
#define OCR_TX0_HL 0x1c

// TX1 floating (off)
#define OCR_TX1_ZZ 0

// TX1 pull-down not inverted
#define OCR_TX1_LZ 0x40

/** 
    Frame format information 0x11 
*/
#define ID0_RTR    1<<4	        // Remote request
#define ID0_DLC_M  (1<<4)-1	// Length Mask
*/
