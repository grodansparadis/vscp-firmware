/* sja1000.h
 * Header file for the Linux CAN-bus driver.
 * Written by Arnaud Westenberg email:arnaud@wanadoo.nl
 * This software is released under the GPL-License.
 */

int sja1000_enable_configuration( struct chip_t *pchip );
int sja1000_disable_configuration( struct chip_t *pchip );
int sja1000_chip_config( struct chip_t *pchip );
int sja1000_standard_mask( struct chip_t *pchip, 
			   u16 code, 
			   u16 mask );
int sja1000_baud_rate( struct chip_t *pchip, 
		       u32 rate, 
		       u32 clock, 
		       u32 sjw, 
		       u32 sampl_pt, 
		       u32 flags );
int sja1000_pre_read_config( struct chip_t *pchip );
int sja1000_pre_write_config( struct chip_t *pchip,  
			      struct canmsg_t *pmsg );
int sja1000_send_msg( struct chip_t *pchip, int bRts );
int sja1000_check_tx_stat( struct chip_t *pchip );
int sja1000_set_btregs( struct chip_t *pchip, 
			unsigned short btr0, 
			unsigned short btr1 );
int sja1000_start_chip( struct chip_t *pchip );
int sja1000_stop_chip( struct chip_t *pchip );
void sja1000_irq_handler( int irq, void *dev_id, struct pt_regs *regs );

int sja1000_register( struct chip_t *pchip );

// BasicCAN mode address map 
#define SJACR		0x00	/* Control register */
#define SJACMR		0x01	/* Command register */
#define SJASR		0x02	/* Status register */
#define SJAIR		0x03	/* Interrupt register */
#define SJAACR		0x04	/* Acceptance Code register */
#define SJAAMR		0x05	/* Acceptance Mask Register */
#define SJABTR0		0x06	/* Bus Timing register 0 */
#define SJABTR1		0x07 	/* Bus Timing register 1 */
#define SJAOCR		0x08	/* Output Control register */
#define SJACDR		0x1f	/* Clock Divider register */

#define SJATXID1	0x0a	/* Identifier byte 1 */
#define SJATXID0	0x0b	/* Identifier byte 0 */
#define SJATXDAT0	0x0c	/* First data byte */
#define SJATXDAT1	0x0d
#define SJATXDAT2	0x0e
#define SJATDDAT3	0x0f
#define SJATXDAT4	0x10
#define SJATXDAT5	0x11
#define SJATXDAT6	0x12
#define SJATXDAT7	0x13

#define SJARXID1	0x14	/* Identifier byte 1 */
#define SJARXID0	0x15	/* Identifier byte 0 */
#define SJARXDAT0	0x16	/* First data byte */
#define SJARXDAT1	0x17
#define SJARXDAT2	0x18
#define SJARXDAT3	0x19
#define SJARXDAT4	0x1a
#define SJARXDAT5	0x1b
#define SJARXDAT6	0x1c
#define SJARXDAT7	0x1d

/* Command register */
enum sja1000_BASIC_CMR {
  CMR_TR  = 1,		// Transmission request
  CMR_AT  = 1<<1,	// Abort Transmission
  CMR_RRB = 1<<2,	// Release Receive Buffer
  CMR_CDO = 1<<3,	// Clear Data Overrun
  CMR_GTS = 1<<4	// Go To Sleep
};

/* Status Register */
enum sja1000_BASIC_SR {
  SR_RBS = 1,		// Receive Buffer Status
  SR_DOS = 1<<1,	// Data Overrun Status
  SR_TBS = 1<<2,	// Transmit Buffer Status
  SR_TCS = 1<<3,	// Transmission Complete Status
  SR_RS  = 1<<4,	// Receive Status
  SR_TS  = 1<<5,	// Transmit Status
  SR_ES  = 1<<6,	// Error Status
  SR_BS  = 1<<7		// Bus Status
};

/* Control Register */
enum sja1000_BASIC_CR {
  CR_RR  = 1,		// Reset Request
  CR_RIE = 1<<1,	// Receive Interrupt Enable
  CR_TIE = 1<<2,	// Transmit Interrupt Enable
  CR_EIE = 1<<3,	// Error Interrupt Enable
  CR_OIE = 1<<4		// Overrun Interrupt Enable
};

/* Interrupt (status) Register */
enum sja1000_BASIC_IR {
  IR_RI  = 1,		// Receive Interrupt
  IR_TI  = 1<<1,	// Transmit Interrupt
  IR_EI  = 1<<2,	// Error Interrupt
  IR_DOI = 1<<3,	// Data Overrun Interrupt
  IR_WUI = 1<<4		// Wake-Up Interrupt
};

/* Clock Divider Register */
enum sja1000_CDR {
  /* f_out = f_osc/(2*(CDR[2:0]+1)) or f_osc if CDR[2:0]==7 */
  CDR_CLKOUT_MASK = 7,
  CDR_CLK_OFF = 1<<3,	// Clock Off
  CDR_RXINPEN = 1<<5,	// TX1 output is RX irq output
  CDR_CBP = 1<<6,	// Input Comparator By-Pass
  CDR_PELICAN = 1<<7	// PeliCAN Mode 
};

/* Output Control Register */
enum sja1000_OCR {
  OCR_MODE_BIPHASE = 0,
  OCR_MODE_TEST = 1,
  OCR_MODE_NORMAL = 2,
  OCR_MODE_CLOCK = 3,
  // TX0 push-pull not inverted
  OCR_TX0_LH = 0x18,
  // TX0 push-pull inverted
  OCR_TX0_HL = 0x1c,
  // TX1 floating (off)
  OCR_TX1_ZZ = 0,
  // TX1 pull-down not inverted
  OCR_TX1_LZ = 0x40
};

/** Frame format information 0x11 */
enum sja1000_BASIC_ID0 {
  ID0_RTR = 1<<4,		// Remote request
  ID0_DLC_M = (1<<4)-1	// Length Mask
};
