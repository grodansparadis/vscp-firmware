/************************************************************
* SPI Slave													*
************************************************************/
#include <io8515.h>

/************************ID that each node uses "Ut"****************
**Sensor		  In				  Ut			  Err
**Video 1		  0x88-0x8F 		  0x80-0x86 	  0x87
**Video 2		  0x98-0x9F 		  0x90-0x96 	  0x97
**Video 3		  0xA8-0xAF 		  0xA0-0xA6 	  0xA7
**Video 4		  0xB8-0xBF 		  0xB0-0xB6 	  0xB7
**-------------Acceptance settings for each node--------------------
**	   !!VIDEO1!!
**CAN_AC0	  = 0x11;
**	   !!VIDEO2!!
**CAN_AC0	  = 0x13;
**	   !!VIDEO3!!
**CAN_AC0	  = 0x15;
**	   !!VIDEO4!!
**CAN_AC0	  = 0x17;
*********************************************************************/

#define CAMERA4

// VIDEO 1

#ifdef CAMERA1
#define VIDEO_AC0 0x11
#define VIDEO_RX 0x88
#define VIDEO_TX 0x80
#define VIDEO_TX_DEBUG 0x81
#endif


// VIDEO 2
#ifdef CAMERA2
#define VIDEO_AC0 0x13
#define VIDEO_RX 0x98
#define VIDEO_TX 0x90
#define VIDEO_TX_DEBUG 0x91
#endif

// VIDEO 3
#ifdef CAMERA3
#define VIDEO_AC0 0x15
#define VIDEO_RX 0xA8
#define VIDEO_TX 0xA0 
#define VIDEO_TX_DEBUG 0xA1
#endif

// VIDEO 4
#ifdef CAMERA4
#define VIDEO_AC0 0x17
#define VIDEO_RX 0xB8
#define VIDEO_TX 0xB0
#define VIDEO_TX_DEBUG 0xB1
#endif

/******************Do not mess up under this line*******************/
#define CAN_BASE	0x4000

/*
** Mode & Control Register
*/
#define CAN_MODECTRL	*(unsigned char *) (CAN_BASE + 0)
#define RESREQ			0x01	   // Reset Mode Request Bit
#define LISTENONLY		0x02	   // Listen Only Mode Bit
#define SELFTEST		0x04	   // Self Test Mode Bit
#define ACCFILT 		0x08	   // Acceptance Filter Mode Bit

#define SLEEP			0x10	   // Enter Sleep Mode Bit

/*
** Command Register
*/
#define CAN_CMD 		*(unsigned char *) (CAN_BASE + 1)
#define TXREQ			0x01	   // Transmit Request Bit
#define TXABORT 		0x02	   // Abort Transmission Bit
#define RRB 			0x04	   // Release Receive Buffer Bit
#define CDO 			0x08	   // Clear Data Overrun Bit
#define SRR 			0x10	   // Self Reception Request Bit

/*
** Status Register
*/
#define CAN_STATUS		*(unsigned char *) (CAN_BASE + 2)
#define RBS 			0x01	   // Receive Buffer Status Bit
#define DOS 			0x02	   // Data overrun Status Bit
#define TBS 			0x04	   // Transmit Buffer Status Bit
#define TCS 			0x08	   // Transmission Complete Status Bit
#define RS				0x10	   // Receive Status Bit
#define TS				0x20	   // Transmit Status Bit
#define ES				0x40	   // Error Status Bit
#define BS				0x80	   // Bus Status Bit

/*
** Interrupt Register
*/
#define CAN_INT 		*(unsigned char *) (CAN_BASE + 3)
#define RI				0x01	   // Receive Interrupt Bit
#define TI				0x02	   // Transmit Interrupt Bit
#define EI				0x04	   // Error Warning Interrupt Bit
#define DOI 			0x08	   // Data Overrun Interrupt Bit
#define WUI 			0x10	   // Wake-up Interrupt Bit
#define EPI 			0x20	   // Error Passive Interrupt Bit
#define ALI 			0x40	   // Arbitration Lost Interrupt Bit
#define BEI 			0x80	   // Bus Error Interrupt Bit

/*
** Interrupt Enable & Control Register
*/
#define CAN_INTEN		*(unsigned char *) (CAN_BASE + 4)
#define RIEN			0x01	   // Receive Interrupt Enable Bit
#define TIEN			0x02	   // Transmit Interrupt Enable Bit
#define EIEN			0x04	   // Error Warning Interrupt Enable Bit
#define DOIEN			0x08	   // Data Overrun Interrupt Enable Bit
#define WUIEN			0x10	   // Wake-up Interrupt Enable Bit
#define EPIEN			0x20	   // Error Passive Interrupt Enable Bit
#define ALIEN			0x40	   // Arbitration Lost Interrupt Enable Bit
#define BEIEN			0x80	   // Bus Error Interrupt Enable Bit

/*
** Bus Timing Register
*/
#define CAN_TMG0		*(unsigned char *) (CAN_BASE + 6)
#define CAN_TMG1		*(unsigned char *) (CAN_BASE + 7)
#define SAMPLEMODE		0x80	   // Sample Mode Bit

/*
** Output Control Register
*/
#define CAN_OCR 		*(unsigned char *) (CAN_BASE + 8)

/*
** Acceptance Code & Mask Register
*/
#define CAN_AC0 		*(unsigned char *) (CAN_BASE + 16)
#define CAN_AC1 		*(unsigned char *) (CAN_BASE + 17)
#define CAN_AC2 		*(unsigned char *) (CAN_BASE + 18)
#define CAN_AC3 		*(unsigned char *) (CAN_BASE + 19)
#define CAN_AM0 		*(unsigned char *) (CAN_BASE + 20)
#define CAN_AM1 		*(unsigned char *) (CAN_BASE + 21)
#define CAN_AM2 		*(unsigned char *) (CAN_BASE + 22)
#define CAN_AM3 		*(unsigned char *) (CAN_BASE + 23)

/*
** Receive (RX) Register
*/
#define CAN_RX_FRMINFO	*(unsigned char *) (CAN_BASE + 16)
#define CAN_RX_BUF1 	*(unsigned char *) (CAN_BASE + 17)
#define CAN_RX_BUF2 	*(unsigned char *) (CAN_BASE + 18)
#define CAN_RX_BUF3 	*(unsigned char *) (CAN_BASE + 19)
#define CAN_RX_BUF4 	*(unsigned char *) (CAN_BASE + 20)
#define CAN_RX_BUF5 	*(unsigned char *) (CAN_BASE + 21)
#define CAN_RX_BUF6 	*(unsigned char *) (CAN_BASE + 22)
#define CAN_RX_BUF7 	*(unsigned char *) (CAN_BASE + 23)
#define CAN_RX_BUF8 	*(unsigned char *) (CAN_BASE + 24)
#define CAN_RX_BUF9 	*(unsigned char *) (CAN_BASE + 25)
#define CAN_RX_BUF10	*(unsigned char *) (CAN_BASE + 26)
#define CAN_RX_BUF11	*(unsigned char *) (CAN_BASE + 27)
#define CAN_RX_BUF12	*(unsigned char *) (CAN_BASE + 28)

/*
** Transmit (TX) Register
*/
#define CAN_TX_FRMINFO	*(unsigned char *) (CAN_BASE + 16)
#define CAN_TX_BUF1 	*(unsigned char *) (CAN_BASE + 17)
#define CAN_TX_BUF2 	*(unsigned char *) (CAN_BASE + 18)
#define CAN_TX_BUF3 	*(unsigned char *) (CAN_BASE + 19)
#define CAN_TX_BUF4 	*(unsigned char *) (CAN_BASE + 20)
#define CAN_TX_BUF5 	*(unsigned char *) (CAN_BASE + 21)
#define CAN_TX_BUF6 	*(unsigned char *) (CAN_BASE + 22)
#define CAN_TX_BUF7 	*(unsigned char *) (CAN_BASE + 23)
#define CAN_TX_BUF8 	*(unsigned char *) (CAN_BASE + 24)
#define CAN_TX_BUF9 	*(unsigned char *) (CAN_BASE + 25)
#define CAN_TX_BUF10	*(unsigned char *) (CAN_BASE + 26)
#define CAN_TX_BUF11	*(unsigned char *) (CAN_BASE + 27)
#define CAN_TX_BUF12	*(unsigned char *) (CAN_BASE + 28)

/*
** Clock Divider Register
*/
#define CAN_CLKDIV		*(unsigned char *) (CAN_BASE + 31)
#define COMPBYPASS		0x40	   // CAN Comperator Bypass Control Bit
#define PCANMODE		0x80	   // PeliCAN mode

/*
** Video defines
*/
#define PORTB_DIR 0x40			/* PORTB direction 0100 0000*/
#define PORTD_DIR 0x28			/* PORTD direction 0010 1000*/

#define SPICONTROL 0xC0
// SPI interrupt enable
// SPI enable

#define wdtReset() asm("wdr\n")
#define INCOMING_SPI_LENGTH 8
#define OUTGOING_SPI_LENGTH 8

unsigned int Ready_To_Send = 1;
unsigned int RX_Lenght;
unsigned int RX_ID;
unsigned int MESSAGE = 0;

unsigned int videoTx = VIDEO_TX_DEBUG;
unsigned char bytesSentSPI = OUTGOING_SPI_LENGTH; // number of bytes that has been transmitted via SPI
unsigned char videoData[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char dataIn[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char dataOut[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
unsigned char canData[8];

/**********************************************************************
**					Initialize´S The sja1000
***********************************************************************/
void Init(void)
{
	while (!(CAN_MODECTRL & RESREQ))	// Make sure SJA1000 is in RESET MODE
	{
		CAN_MODECTRL = RESREQ;
	}
	
	CAN_CLKDIV	= PCANMODE | COMPBYPASS;// Set PeliCAN mode and disable comparator
	CAN_INTEN	= 0x00; 				// Disable all interrupts
	
	CAN_AC0 	= VIDEO_AC0;			//This is the acc. settings.
	CAN_AC1 	= 0x00; 				//Each node has its own settings
	CAN_AC2 	= 0x00; 				//Have a look further down!
	CAN_AC3 	= 0x00; 				//
	CAN_AM0 	= 0x00; 				//
	CAN_AM1 	= 0xFF; 				//
	CAN_AM2 	= 0xFF; 				//
	CAN_AM3 	= 0xFF; 				//
	/*500k:BTR0=0x00 BTR1=0x1c, 800k:BTR0=0x00 BTR1=0x16 1M:BTR0=0x00 BTR1=0x14*/
	CAN_TMG0	= 0x40; 				// CAN speed 500kbit with 16Mhz Chrystal
	CAN_TMG1	= 0x2b;   /* 00 1c innan*/
	
	CAN_OCR 	= 0xDE; 				// Set Output Control for CANDIP & 82C250 Tranciever
}

/*************************************************************************
**		 Puts the sja1000 controller in run mode
**************************************************************************/
void RunMode(void)
{
	while (CAN_MODECTRL & RESREQ)		 // Make sure SJA1000 is set in OPERATIONAL MODE
	{
		CAN_MODECTRL = ACCFILT; 		 //Singel acceptance filter
	}
	
	CAN_INTEN	= RIEN | TIEN;
}


/*********************************************************************
**					This is the CAN transmit function
***********************************************************************/
void TransmitCAN(unsigned int id,unsigned char lenght,unsigned char *dataOut)
{
	Ready_To_Send=0;
	while (!(CAN_STATUS & TBS));		// Wait until Transmit Buffer is released
	
	CAN_TX_FRMINFO	= lenght;			// SFF (data), number of buffers to send
	CAN_TX_BUF1 	= id >> 3;			//TX ID 1
	CAN_TX_BUF2 	= id << 5;			//TX ID 2
	CAN_TX_BUF3 	= dataOut[0];
	CAN_TX_BUF4 	= dataOut[1];
	CAN_TX_BUF5 	= dataOut[2];
	CAN_TX_BUF6 	= dataOut[3];
	CAN_TX_BUF7 	= dataOut[4];
	CAN_TX_BUF8 	= dataOut[5];
	CAN_TX_BUF9 	= dataOut[6];
	CAN_TX_BUF10	= dataOut[7];
	
	CAN_CMD 		= TXREQ;
}

/******************************************************************************
*						This is the Recive function                           *
*******************************************************************************/
void ReciveCAN(void)
{
	while(!(CAN_STATUS & RBS));
	
	RX_Lenght=CAN_RX_FRMINFO & 0x0f;	//Lenght of recived data
	RX_ID=((unsigned int)CAN_RX_BUF1 << 3) + ((CAN_RX_BUF2 >> 5));
	
	dataIn[0]=CAN_RX_BUF3;
	dataIn[1]=CAN_RX_BUF4;
	dataIn[2]=CAN_RX_BUF5;
	dataIn[3]=CAN_RX_BUF6;
	dataIn[4]=CAN_RX_BUF7;
	dataIn[5]=CAN_RX_BUF8;
	dataIn[6]=CAN_RX_BUF9;
	dataIn[7]=CAN_RX_BUF10;
	
	MESSAGE=1;
}

/***********************************************************************
**					This is the INT0 handler
************************************************************************/
#pragma interrupt_handler CAN_INT0:2
void CAN_INT0(void)
{
	if (CAN_INT & TI) // Transmit Interrupt
	{
		Ready_To_Send=1;
	}
	if (CAN_INT & RI) // Receive Interrupt
	{
		ReciveCAN();
	}
}

/*************************************************************
* SPI Interrupt function 									 *
*************************************************************/
#pragma interrupt_handler SPI_REC:9
void SPI_REC(void)
{
 	static signed char bytesReceivedSPI = -1;
 	static unsigned char tmpSPIRecByte;

	PORTD &= ~0x08; // clear slave message flag

	tmpSPIRecByte = SPDR;

	if (bytesReceivedSPI < 0) 
	{
	  if (tmpSPIRecByte == 0xff)
	  {
		bytesReceivedSPI = 0;
	  }
	}
	
	else
	{
		videoData[bytesReceivedSPI] = tmpSPIRecByte;
		bytesReceivedSPI++;
	}
/*	
	if (videoData[bytesReceivedSPI] == 0xff)
	{
		bytesReceivedSPI = 0;
		videoTx = VIDEO_TX;
	}
	else if (videoData[bytesReceivedSPI] == 0xfe)
	{
	 	bytesReceivedSPI = 0;
		videoTx = VIDEO_TX_DEBUG;	 
	}
	else 
	{
		bytesReceivedSPI++;
	}
*/
	if(bytesSentSPI<OUTGOING_SPI_LENGTH)
	{
		SPDR = canData[bytesSentSPI];
		PORTD |= 0x08; // set slave message flag
		bytesSentSPI++;
	}	
	
	if (bytesReceivedSPI == INCOMING_SPI_LENGTH)
	{
		videoTx = VIDEO_TX; // BY MUNIN 2000!!!
		while(!Ready_To_Send);
		TransmitCAN(videoTx,8,videoData);		
		bytesReceivedSPI = -1;
	}
}

/******************************Main*********************************/
int main(void)
{	
	unsigned char j;
	unsigned long i;

	MCUCR = 0xC2;
	// External SRAM enable
	// External SDRAM wait-state
	// Falling edge of INT0 generates interrupt
	SREG |= 0x80;
	// Global interrupt enable
	GIMSK = 0x40;
	// Enable INT0
	wdtReset();
	WDTCR = 0xd; 
	// enable watchdog timer
	// 0.49 s timeout.
	
	DDRB = PORTB_DIR; // Set port B direction
	DDRD = PORTD_DIR; // Set port D direction
	SPCR = SPICONTROL;
	// SPI interrupt enable
	// SPI enable
	
	PORTD &= ~0x08; // clear slave message flag
	PORTD |= 0x20; // set VIP- and AVR-reset pin high
	
	Init();
	RunMode();
	
	// reset VIP and AVR
	PORTD &= ~0x20; // set VIP- and AVR-reset pin low
	for(j=0;j<100;j++);
	PORTD |= 0x20; // set VIP- and AVR-reset pin high
	
	TransmitCAN(VIDEO_TX_DEBUG,8,dataOut); // Alive and kicking
	
	while(1)
	{
	 	wdtReset();
		if (MESSAGE)
		{
			if((bytesSentSPI >= OUTGOING_SPI_LENGTH) && 
			   (RX_ID == VIDEO_RX)) // if last CAN message received has been sent and the msg is ours
			{
			 	bytesSentSPI = 0;
								
				for (j = 0; j < 8; j++)
				{
					canData[j] = dataIn[j];
				}
				if(canData[0] == 0x99) // reset message
				{  
				    WDTCR = 0x80; 
					// enable watchdog timer
					// short timeout.
					bytesSentSPI = OUTGOING_SPI_LENGTH;
					for(i=0;i<200000;i++); // wait for timeout
				}
				else if ((PINB & 0x10) == 0x10) // if slave select is high
				{
				 SPDR = canData[bytesSentSPI];
				 PORTD |= 0x08;
				 bytesSentSPI++;
				}
			}
			MESSAGE = 0;
			CAN_CMD = RRB;
		}
	}
	return 1;
}

