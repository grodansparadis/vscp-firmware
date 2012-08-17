/*************************************************************
* SPI interface for the MCP2515 CAN controller from Microchip
*
* Depends on the SPI lib.
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _MCP2515_H_
#define _MCP2515_H_

#include <system.h>

// Register map of the MCP2515
#define RXF0SIDH	0x00
#define RXF0SIDL	0x01
#define RXF0EID8	0x02
#define RXF0EID0	0x03
#define RXF1SIDH	0x04
#define RXF1SIDL	0x05
#define RXF1EID8	0x06
#define RXF1EID0	0x07
#define RXF2SIDH	0x08
#define RXF2SIDL	0x09
#define RXF2EID8	0x0A
#define RXF2EID0	0x0B
#define BFPCTRL		0x0C
#define TXRTSCTRL	0x0D
#define CANSTAT		0x0E
#define CANCTRL		0x0F

#define RXF3SIDH	0x10
#define RXF3SIDL	0x11
#define RXF3EID8	0x12
#define RXF3EID0	0x13
#define RXF4SIDH	0x14
#define RXF4SIDL	0x15
#define RXF4EID8	0x16
#define RXF4EID0	0x17
#define RXF5SIDH	0x18
#define RXF5SIDL	0x19
#define RXF5EID8	0x1A
#define RXF5EID0	0x1B
#define TEC			0x1C
#define REC         0x1D
#define CANSTAT1	0x1E
#define CANCTRL1	0x1F

#define RXM0SIDH	0x20
#define RXM0SIDL	0x21
#define RXM0EID8	0x22
#define RXM0EID0	0x23
#define RXM1SIDH	0x24
#define RXM1SIDL	0x25
#define RXM1EID8	0x26
#define RXM1EID0	0x27
#define CNF3		0x28
#define CNF2		0x29
#define CNF1		0x2A
#define CANINTE		0x2B
#define CANINTF		0x2C
#define EFLG		0x2D
#define CANSTAT2	0x2E
#define CANCTRL2	0x2F

#define TXB0CTRL	0x30
#define TXB0SIDH	0x31
#define TXB0SIDL	0x32
#define TXB0EID8	0x33
#define TXB0EID0	0x34
#define TXB0DLC		0x35
#define TXB0D0		0x36
#define TXB0D1		0x37
#define TXB0D2		0x38
#define TXB0D3		0x39
#define TXB0D4		0x3A
#define TXB0D5		0x3B
#define TXB0D6		0x3C
#define TXB0D7		0x3D
#define CANSTAT3	0x3E
#define CANCTRL3	0x3F

#define TXB1CTRL	0x40
#define TXB1SIDH	0x41
#define TXB1SIDL	0x42
#define TXB1EID8	0x43
#define TXB1EID0	0x44
#define TXB1DLC		0x45
#define TXB1D0		0x46
#define TXB1D1		0x47
#define TXB1D2		0x48
#define TXB1D3		0x49
#define TXB1D4		0x4A
#define TXB1D5		0x4B
#define TXB1D6		0x4C
#define TXB1D7		0x4D
#define CANSTAT4	0x4E
#define CANCTRL4	0x4F

#define TXB2CTRL	0x50
#define TXB2SIDH	0x51
#define TXB2SIDL	0x52
#define TXB2EID8	0x53
#define TXB2EID0	0x54
#define TXB2DLC		0x55
#define TXB2D0		0x56
#define TXB2D1		0x57
#define TXB2D2		0x58
#define TXB2D3		0x59
#define TXB2D4		0x5A
#define TXB2D5		0x5B
#define TXB2D6		0x5C
#define TXB2D7		0x5D
#define CANSTAT5	0x5E
#define CANCTRL5	0x5F

#define RXB0CTRL	0x60
#define RXB0SIDH	0x61
#define RXB0SIDL	0x62
#define RXB0EID8	0x63
#define RXB0EID0	0x64
#define RXB0DLC		0x65
#define RXB0D0		0x66
#define RXB0D1		0x67
#define RXB0D2		0x68
#define RXB0D3		0x69
#define RXB0D4		0x6A
#define RXB0D5		0x6B
#define RXB0D6		0x6C
#define RXB0D7		0x6D
#define CANSTAT6	0x6E
#define CANCTRL6	0x6F

#define RXB1CTRL	0x70
#define RXB1SIDH	0x71
#define RXB1SIDL	0x72
#define RXB1EID8	0x73
#define RXB1EID0	0x74
#define RXB1DLC		0x75
#define RXB1D0		0x76
#define RXB1D1		0x77
#define RXB1D2		0x78
#define RXB1D3		0x79
#define RXB1D4		0x7A
#define RXB1D5		0x7B
#define RXB1D6		0x7C
#define RXB1D7		0x7D
#define CANSTAT7	0x7E
#define CANCTRL7	0x7F

// Bit definitions of the various registers

// Bit definitions BFPCTRL
#define trB1BFS		BFPCTRL,5
#define trB0BFS		BFPCTRL,4
#define trB1BFE		BFPCTRL,3
#define trB0BFE		BFPCTRL,2
#define trB1BFM		BFPCTRL,1
#define trB0BFM		BFPCTRL,0

// Bit definitions TXRTSCTRL
#define trB2RTS		BFPCTRL,5
#define trB1RTS		BFPCTRL,4
#define trB0RTS		BFPCTRL,3
#define trB2RTSM	BFPCTRL,2
#define trB1RTSM	BFPCTRL,1
#define trB0RTSM	BFPCTRL,0

// Bit definitions CANSTAT
#define trOPMOD2	CANSTAT,7
#define trOPMOD1	CANSTAT,6
#define trOPMOD0	CANSTAT,5
#define trICOD2		CANSTAT,3
#define trICOD1		CANSTAT,2
#define trICOD0		CANSTAT,1

// Bit definitions CANCTRL
#define trREQOP2	CANCTRL,7
#define trREQOP1	CANCTRL,6
#define trREQOP0	CANCTRL,5
#define trABAT		CANCTRL,4
#define trCLKEN		CANCTRL,2
#define trCLKPRE1	CANCTRL,1
#define trCLKPRE0	CANCTRL,0

// Dit definitions CNF3
#define trWAKFIL	CNF3,6
#define trPHSEG22	CNF3,2
#define trPHSEG21	CNF3,1
#define trPHSEG20	CNF3,0

// Bit definitions CNF2
#define trBTLMODE	CNF2,7
#define trSAM	CNF2,6
#define trPHSEG12	CNF2,5
#define trPHSEG11	CNF2,4
#define trPHSEG10	CNF2,3
#define trPHSEG2	CNF2,2
#define trPHSEG1	CNF2,1
#define trPHSEG0	CNF2,0

// Bit definitions CNF1
#define trSJW1		CNF1,7
#define trSJW0		CNF1,6
#define trBRP5		CNF1,5
#define trBRP4		CNF1,4
#define trBRP3		CNF1,3
#define trBRP2		CNF1,2
#define trBRP1		CNF1,1
#define trBRP0		CNF1,0

// Bit definitions CANINTE
#define trMERRE		CANINTE,7
#define trWAKIE		CANINTE,6
#define trERRIE		CANINTE,5
#define trTX2IE		CANINTE,4
#define trTX1IE		CANINTE,3
#define trTX0IE		CANINTE,2
#define trRX1IE		CANINTE,1
#define trRX0IE		CANINTE,0

// Bit definitions CANINTF
#define trMERRF		CANINTF,7
#define trWAKIF		CANINTF,6
#define trERRIF		CANINTF,5
#define trTX2IF		CANINTF,4
#define trTX1IF		CANINTF,3
#define trTX0IF		CANINTF,2
#define trRX1IF		CANINTF,1
#define trRX0IF		CANINTF,0

// Bit definitions EFLG
#define trRX1OVR	EFLG,7
#define trRX0OVR	EFLG,6
#define trTXB0		EFLG,5
#define trTXEP		EFLG,4
#define trRXEP		EFLG,3
#define trTXWAR		EFLG,2
#define trRXWAR		EFLG,1
#define trEWARN		EFLG,0

// Bit definitions TXB0CTRL
#define trABTF0		TXB0CTRL,6
#define trMLOA0		TXB0CTRL,5
#define trTXERR0	TXB0CTRL,4
#define trTXREQ0	TXB0CTRL,3
#define trTXP10		TXB0CTRL,1
#define trTXP00		TXB0CTRL,0

// Bit definitions TXB1CTRL
#define trABTF1		TXB1CTRL,6
#define trMLOA1		TXB1CTRL,5
#define trTXERR1	TXB1CTRL,4
#define trTXREQ1	TXB1CTRL,3
#define trTXP11		TXB1CTRL,1
#define trTXP01		TXB1CTRL,0

// Bit definitions TXB2CTRL
#define trABTF2		TXB2CTRL,6
#define trMLOA2		TXB2CTRL,5
#define trTXERR2	TXB2CTRL,4
#define trTXREQ2	TXB2CTRL,3
#define trTXP12		TXB2CTRL,1
#define trTXP02		TXB2CTRL,0

// Bit definitions RXB0CTRL
#define trRXM10		RXB0CTRL,6
#define trRXM00		RXB0CTRL,5
#define trRXRTR0	RXB0CTRL,3
#define trBUKT01	RXB0CTRL,2
#define trBUKT00	RXB0CTRL,1
#define trFILHIT00	RXB0CTRL,0

// Bit definitions RXB1CTRL
#define trRXM11		RXB1CTRL,6
#define trRXM01		RXB1CTRL,5
#define trRXRTR1	RXB1CTRL,3
#define trFILHIT12	RXB1CTRL,2
#define trFILHIT11	RXB1CTRL,1
#define trFILHIT10	RXB1CTRL,0


// use with SPI_Rts function
#define RTS0        0x01 
#define RTS1        0x02
#define RTS2        0x04

// Instruction set
#define c2515Read   0x03      // MCP2515 read instruction
#define c2515Write  0x02      // MCP2515 write instruction
#define c2515Reset  0xC0      // MCP2515 reset instruction
#define c2515RTS    0x80      // MCP2515 RTS instruction
#define c2515Status 0xA0      // MCP2515 Status instruction
#define c2515BitMod 0x05      // MCP2515 bit modify instruction

// ID on the CAN bus
#define CAN_RX_ID 0x00
#define CAN_TX_ID 0x01

// Connections to the chip
//volatile bit mcp2515_rst @ PORTC . 1;
//volatile bit mcp2515_rst_tris @ TRISC . 1;

// Function declarations
void can_reset();
char can_read_register(char reg);
void can_write_bits(char address, char data, char mask);
void can_init(short id);
char can_peek_message();
char can_rx_byte();
void can_set_id(short id); 
void can_init_buffer();
void can_load_byte(char input);
void can_tx_buffer();
void can_read_status();

#endif // _MCP2515_H_

