/*

	SJA1000 CAN Contorller Register Definitions

*/


#define BASE_ADDRESS	0x4000		// CANDIP

// Control Register
#define SJACR		*(unsigned char *) ( CAN_BASE + 0 )

// Command Register
#define SJACMR		*(unsigned char *) ( CAN_BASE + 1 )

// Status Register
#define SJASR		*(unsigned char *) ( CAN_BASE + 2 )

// Interrupt register
#define SJAIR		*(unsigned char *) ( CAN_BASE + 3 )

// Acceptance Code
#define SJAACR	        *(unsigned char *) ( CAN_BASE + 4 )

// Acceptance Mask
#define SJAAMR		*(unsigned char *) ( CAN_BASE + 5 )

// Bus timing register 0
#define SJA1000_BTR0	*(unsigned char *) ( CAN_BASE + 6 )

// Bus timing register 1
#define SJA1000_BTR1	*(unsigned char *) ( CAN_BASE + 7 )

// Output register
#define SJA1000_OC	*(unsigned char *) ( CAN_BASE + 8 )

//
#define SJA1000_TEST	*(unsigned char *) ( CAN_BASE + 9 )

// TX - ID Bit 3 - Bit 10
#define SJA1000_TX_ID1	*(unsigned char *) ( CAN_BASE + 10 )

// TX - ID Bit 0 - Bit 2 RTS, DLC
#define SJA1000_TX_ID2	*(unsigned char *) ( CAN_BASE + 11 )

// TX _ Data byte 0
#define SJA1000_TX_D0	*(unsigned char *) ( CAN_BASE + 12 )

// TX _ Data byte 1
#define SJA1000_TX_D1	*(unsigned char *) ( CAN_BASE + 13 )

// TX _ Data byte 2
#define SJA1000_TX_D2	*(unsigned char *) ( CAN_BASE + 14 )

// TX _ Data byte 3
#define SJA1000_TX_D3	*(unsigned char *) ( CAN_BASE + 15 )

// TX _ Data byte 4
#define SJA1000_TX_D4	*(unsigned char *) ( CAN_BASE + 16 )

// TX _ Data byte 5
#define SJA1000_TX_D5	*(unsigned char *) ( CAN_BASE + 17 )

// TX _ Data byte 6
#define SJA1000_TX_D6	*(unsigned char *) ( CAN_BASE + 18 )

// TX _ Data byte 7
#define SJA1000_TX_D7	*(unsigned char *) ( CAN_BASE + 19 )


#define SJA1000_RX_ID	*(unsigned char *) ( CAN_BASE + 20 )

#define SJA1000_RX_LEN	*(unsigned char *) ( CAN_BASE + 21 )

#define SJA1000_RX_BUF0	*(unsigned char *) ( CAN_BASE + 22 )

#define SJA1000_RX_BUF1	*(unsigned char *) ( CAN_BASE + 23 )

#define SJA1000_RX_BUF2	*(unsigned char *) ( CAN_BASE + 24 )

#define SJA1000_RX_BUF3	*(unsigned char *) ( CAN_BASE + 25 )

#define SJA1000_RX_BUF4	*(unsigned char *) ( CAN_BASE + 26 )

#define SJA1000_RX_BUF5	*(unsigned char *) ( CAN_BASE + 27 )

#define SJA1000_RX_BUF6	*(unsigned char *) ( CAN_BASE + 28 )

#define SJA1000_RX_BUF7	*(unsigned char *) ( CAN_BASE + 29 )

#define SJA1000_	*(unsigned char *) ( CAN_BASE + 30 )

#define SJA1000_CLKDIV	*(unsigned char *) ( CAN_BASE + 31 )