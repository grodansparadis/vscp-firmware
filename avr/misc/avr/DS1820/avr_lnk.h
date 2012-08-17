#include "iom16.h"

#define   DQ_DDR	 DDRA
#define   DQ_PIN	 PINA
#define   DQ_PORT    PORTA
#define   DQ    7

#include "ownet.h"

void usDelay(int useconds);
//-----------------------------------------------------------------------------

// defined in avr_lnk_asm file
// these functions are in assembly, due to timing critical reasons
extern SMALLINT owTouchBit(int portnum, SMALLINT sendbit);
extern SMALLINT owTouchReset(int portnum);
extern SMALLINT owLevel(int portnum, SMALLINT new_level);
extern SMALLINT ow_PinStatus(void);
//-----------------------------------------------------------------------------

// Serial functions 
void serial0_init(unsigned char value);
int  getchar( void );
int  putchar( int data );
void PrintString(char *sz_string);
void PrintChar(unsigned char value);
void PrintInt(unsigned int value);
void PrintFloat(float value);

//_____________________________________________________________________________
// XTAL_FREQUENCY is used to calculate baudrate
//#define XTAL_FREQUENCY 16000000
//#define XTAL_FREQUENCY 7372800
//#define XTAL_FREQUENCY 3686400
#define XTAL_FREQUENCY 1000000
// . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
#define UBRR_19200  ((XTAL_FREQUENCY/(19200.0*8.0))-1)
#define UBRR_9600   ((XTAL_FREQUENCY/(9600.0*8.0))-1)

//_____________________________________________________________________________

/* USART Buffer Defines */
#define USART_RX_BUFFER_SIZE  (4)              // 1,2,4,8,16,32,64,128 or 256 bytes
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1 )

#if ( USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK )
        #error RX buffer size is not a power of 2
#endif

#define USART_TX_BUFFER_SIZE  (32)             // 1,2,4,8,16,32,64,128 or 256 bytes
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1 )

#if ( USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK )
        #error TX buffer size is not a power of 2
#endif


