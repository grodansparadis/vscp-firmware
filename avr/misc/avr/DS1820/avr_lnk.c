/***********************************************************************************
Project:          Dallas 1-wire program
Filename:         avr_lnk.c    

Processor:        AVR family
Compiler:         IAR Version 1.50C
  
Author:               SH
Company:         ATMEL Norway     
***********************************************************************************/
#include "avr_lnk.h"
#include "iom16.h"

#include <math.h>	// used by PrintFloat(f)

#define POLLED_UART	// Comment this if you want an interrupt driven RingBuffer


#ifndef POLLED_UART
// Static Variables 
static unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile unsigned char USART_RxHead;
static volatile unsigned char USART_RxTail;

static unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile unsigned char USART_TxHead;
static volatile unsigned char USART_TxTail;
#endif
//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'len' milliseconds
//
void msDelay(int delay)
{
	while(delay--)
	{					// 1mS = 1016 uS
		usDelay(987);	// 1 S =  990 mS
	}
}


//--------------------------------------------------------------------------
//  Description:
//     Delay for at least 'len' microseconds
//  Works only @ 1Mhz.  , Must be recalculated on other frequency
//
//  useful from 18 to 32768 uS  (in steps of 7)
//
void usDelay(int useconds)
{								// entering this	: 6 cycles (LDI R16,valueL  LDI R17,valueH  CALL usDelay)
	useconds-=14;				// subtracting		: 2 cycles
	while((useconds-=7) >0);	// every iteration	: 7 cycles (6 when no iteration)
}								// return			: 4 cycles, so 6+2+4 = 14?  yes,14 and 7 is buddy

//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of delivering power. 
//
SMALLINT owHasPowerDelivery(int portnum)
{
   portnum = 0;
   // add adapter specific code here
   return TRUE;
}

//--------------------------------------------------------------------------
// This procedure indicates wether the adapter can deliver power.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  if adapter is capable of over drive. 
//
SMALLINT owHasOverDrive(int portnum)
{
   portnum = 0;
   // add adapter specific code here
   return TRUE;
}

//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse 
// on the 1-Wire Net for programming EPROM iButtons.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
//
// Returns:  TRUE  program volatage available
//           FALSE program voltage not available  
SMALLINT owHasProgramPulse(int portnum)
{
   portnum = 0;
   // Not supported yet
   return FALSE;
}
//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'new_speed'  - new speed defined as
//                MODE_NORMAL     0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
SMALLINT owSpeed(int portnum, SMALLINT new_speed)
{
   portnum = new_speed = 0; // avoid compiler warning
   // This is written for AVR running at 1MHz
   return 0;
}


//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).  
// The parameter 'sendbyte' least significant 8 bits are used.  After the
// 8 bits are sent change the level of the 1-Wire net.
//
// 'portnum'  - number 0 to MAX_PORTNUM-1.  This number was provided to
//              OpenCOM to indicate the port number.
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same 
//
SMALLINT owWriteBytePower(int portnum, SMALLINT sendbyte)
{
   owTouchByte(portnum,sendbyte);	// No error checking here, the Power has to raise within 10uS
   owLevel(portnum,MODE_STRONG5);

   return TRUE;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  8 bits read from sendbyte
//
SMALLINT owTouchByte(int portnum, SMALLINT sendbyte)
{
   SMALLINT i;
   SMALLINT result = 0;
   
   for (i = 0; i < 8; i++)
   {
       result |= (owTouchBit(portnum, sendbyte & 1) << i);
       sendbyte >>= 1;
   }
   return result;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
// 'sendbyte'   - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
SMALLINT owWriteByte(int portnum, SMALLINT sendbyte)
{
   return (owTouchByte(portnum, sendbyte) == sendbyte) ? TRUE : FALSE;
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// 'portnum'    - number 0 to MAX_PORTNUM-1.  This number is provided to
//                indicate the symbolic port number.
//
// Returns:  8 bytes read from 1-Wire Net
//
SMALLINT owReadByte(int portnum)
{
   return owTouchByte(portnum, 0xFF);
}



//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void PrintString(char *sz_string)
{
	while(*sz_string)
		putchar(*sz_string++);
}

void PrintChar(unsigned char value)
{
	unsigned char ch;
    
    ch = (value>>4)+'0';
	if(ch > '9')
		ch+=7;
	putchar(ch);
	
	ch = (value&0x0F)+'0';
	if(ch > '9')
		ch+=7;
	putchar(ch);
	
}
void PrintInt(unsigned int value)
{
	PrintChar(value>>8);
	PrintChar(value);
}

void PrintFloat(float f )
{
    // printout floating point value using precision           
    unsigned char Precision = 3;
    char i,n;            
  	float temp=0.5;
	float round;
    signed char pow10;
    char dp;

    for (i=0;i<Precision;i++) 
		temp/=10;
	round=temp;

    // check sign and make positive
    if (f<0) 
    {                                                  
        f=-f;
        putchar('-');              // print minus sign
    }
    // normalise between +-1.0000 and +-9.9999 
    // first get f below 10 when rounded       
    // if we don't allow for rounding 9.9999 could change to 10.00! 
    pow10=0;
    {
        float temp=10.0+round; 
        while (f>=temp) 
        {
            f/=10;               
            pow10++;
        }
        f+=round;                                                     
    } 
    // we leave number between 0 and 9.999 - so we can give a FIXED result
    dp=pow10;              // handle exponent by moving decimal point
    pow10=0;
    if (dp>Precision)      // can't print more than precision
    {
        PrintString("OVERLOAD");
    }
    
    //pow10 is now equal to exponent
    // f is in range 1.0<=f<10
    // PRINT MANTISSA. Include decimal point and print until precision runs out  
    for (i=0;i<Precision;i++) 
    {  
        f-= (n=f);
        if ((i>=dp)||n)        // remove leading zeros
            putchar(n+'0');
        else
            putchar(' ');
        if (i==dp) 
            putchar('.');
        f=ldexp(f+ldexp(f,2),1);       //f*=10 but faster
    }         
}
////////////////////////////////////////////////////////////////////
void serial0_init(unsigned char value)
{
	unsigned char x=0;
	//UBRRH = x;
	UBRRL = value;
	UCSRA = (1<<U2X);
#ifdef POLLED_UART
	UCSRB = (1<<RXEN) | (1<<TXEN);
#else
	UCSRB = (1<<RXCIE) | (1<<RXEN) | (1<<TXEN);
	//UCSRC = (1<<UCSZ1) | (1<<UCSZ0);

	USART_RxTail = x;
	USART_RxHead = x;
	USART_TxTail = x;
	USART_TxHead = x;
	
	asm("sei");
#endif
}            
// printf uses putchar, 
// and AVRStudio's terminal window will show the output in DEBUG mode
// if putchar & getchar is commented out
int getchar( void )
{
#ifdef POLLED_UART
	while ( !(UCSRA & (1<<RXC)) ) /* wait for incomming data */
		; /* return the data */
	return UDR;
#else
    unsigned char tmptail;
    
    while ( USART_RxHead == USART_RxTail );         // wait for incomming data 
   
    tmptail = ( USART_RxTail + 1 ) & USART_RX_BUFFER_MASK; // calculate buffer index 
    USART_RxTail = tmptail;                      // store new index 
    
    return USART_RxBuf[tmptail];
#endif
}

int putchar( int data )
{
#ifdef POLLED_UART
	while ( !(UCSRA & (1<<UDRE)) )
		; /* wait for empty transmit buffer */
	UDR = data; /* start transmittion */
    return data;
#else
    unsigned char tmphead;
   
    tmphead = ( USART_TxHead + 1 ) & USART_TX_BUFFER_MASK; // calculate buffer index 
    
    while ( tmphead == USART_TxTail );           // wait for free space in buffer 
    
    USART_TxBuf[tmphead] = (unsigned char)data;  // store data in buffer 
    USART_TxHead = tmphead;                      // store new index 
    
    UCSRB |= (1<<UDRIE);                          // enable UDRE0 interrupt 
    return data;
#endif
}

#ifndef POLLED_UART
// interrupt handlers 
interrupt [USART_RXC_vect] void USART_RX_interrupt( void )
{
    unsigned char data;
    unsigned char tmphead;
    
    data = UDR;                                    // read the received data 
    
    tmphead = ( USART_RxHead + 1 ) & USART_RX_BUFFER_MASK; // calculate buffer index 
    USART_RxHead = tmphead;                      // store new index 
    
    if ( tmphead == USART_RxTail )
    {
        // ERROR! Receive buffer overflow 
    }
    
    USART_RxBuf[tmphead] = data;                 // store received data in buffer 
}

interrupt [USART_UDRE_vect] void USART_UDRE_interrupt( void )
{
    unsigned char tmptail;
    
    if( USART_TxHead != USART_TxTail )        // check if all data is transmitted 
    {
        
        tmptail = ( USART_TxTail + 1 ) & USART_TX_BUFFER_MASK; // calculate buffer index 
        USART_TxTail = tmptail;                  // store new index 
        
        UDR = USART_TxBuf[tmptail];             // start transmition 
    }
    else
    {
        UCSRB &= ~(1<<UDRIE);                     // disable UDRE interrupt 
    }
}
#endif
