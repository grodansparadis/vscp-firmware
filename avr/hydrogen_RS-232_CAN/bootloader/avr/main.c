// Copyright (C) 2002 Jaroslaw Karwik, 
//   Mente Marine Oy <jaroslaw.karwik@mente-marine.com>
//
// Changes by akhe@eurosouce.se http://www.eursource.se
// 
// This file is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
// 
// In addition to the permissions in the GNU General Public License, the
// author gives you unlimited permission to link the
// compiled version of this file into combinations with other programs,
// and to distribute those combinations without any restriction coming
// from the use of this file.  (The General Public License restrictions
// do apply in other respects; for example, they cover modification of
// the file, and distribution when not linked into a combine
// executable.)
// 
// This file is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.  
// 

#include "iom161.h"
#include "ina90.h"
//#include "iomacros.h"
#include "wdt.h"
#include "prog.h"

// Definitions for data frame lengths
#define  FRAME_COMMAND          2
#define  FRAME_DATA_WRITE       66
#define  FRAME_DATA_READ        64		   

// Startup related data
#define  MARKER_BYTE_ADDRESS    ( 0x1FC00 - 2 )
#define  MARKER_MAGIC            0xACDC // 0xABBA (ACDC is much Harder!!!)  

//*****************************************************************************
// uart_init - Serial communication setup  
//
// Setup UART baudrate ( uses UART0 )
//        
//
void uart_init(void)
{    
  // Baudrate is  115200 with 7.372MHz clock
  outp( 3, UBRR0 );
  outp( 0, UBRRH );            
  
  // Setup UART control bits  
  outp( 0, UCSR0A );     
     
  // Enable recieve and transmit UART
  outp( ( ( 1<<RXEN0 ) | ( 1<<TXEN0 ) ), UCSR0B );

  // Asynchronous, no parity, 1 stop bit, 8 bit data
  //SET_PORT(UCSR0C,( 1<<UCSZ0 ) | ( 1<<UCSZ1 ) ); 
  
  // Setup pins for RS485 dataflow control  
  
  // Set control pins as outputs
  //SET_PORT(DDRD, (1<<PIN_nRE) | (1<<PIN_DE)  ); 

  // Enable receiver
  //CLR_PORT_BIT( PORTD, PIN_nRE );

  // Disable transmitter
  //CLR_PORT_BIT( PORTD, PIN_DE );
}

//*****************************************************************************
// uart_put - Sends one character, waits until completed
//
// Note: Shorter version with UDRE could be used,
//	 but it could cause problems with RS485
//	 when switching transmission direction
//	 (last character could be lost)	
//

void uart_put( u8 cc)
{
  outp( cc, UDR0 );
  
  while ( ( inp( UCSR0A ) & ( 1<<TXC0 ) ) == 0 ){
    // wait until byte sent      
  }
  
  outp( ( 1<<TXC0 ), UCSR0A );
}

//*****************************************************************************
// uart_get - Receives one character. 
//
//      Starts application if application timeout
//	occured. Returns immediately if frame character timeout occured.
//

u8 uart_get( void )
{   	    	      
  while( ( inp( UCSR0A ) & ( 1<<RXC0 ) ) == 0  ){
    
    wdt_reset();     
    
    if( bit_is_set( TIFR, TOV0 ) ){
      // Application timeout. We should start the application
      runApplication();			
    }
    
    if( bit_is_set( TIFR, TOV2 ) ){
      // Character timeout (characters in the frame have to come 
      // within certain time)
      return 0;
    }  
  }
  
  // Reset timer responsible for character timeout
  outp( 0, TCNT2 ); 
  
  return inp( UDR0 );
}


//*****************************************************************************
// isAppCorrupted 
//
// Checks for presence of valid application in the flash
// This version uses one flash sector just for these two bytes.
//

u8 isAppCorrupted( void )
{
   u8   result;
   
#if MARKER_BYTE_ADDRESS > 0x10000
   // Note - Mega128 specific - choose high 64K flash area 
   highFlashBank();
#endif
   
   if(    
      ( readProgramMemory( ( MARKER_BYTE_ADDRESS    & 0xFFFF) ) ==  
	(MARKER_MAGIC >> 8)  ) &&
      ( readProgramMemory( ( ( MARKER_BYTE_ADDRESS + 1 ) & 0xFFFF) ) ==  
	( MARKER_MAGIC& 0xFF) )        
      ){
     result = 0;
   } 
   else {
     result = 1;
   }

#if MARKER_BYTE_ADDRESS > 0x10000
   //  Note - Mega128 specific- choose low 64K flash area
   lowFlashBank();
#endif
   
   return result;
}


//*****************************************************************************
//  main - Main loader engine 
//


int main(int mode,char **argv)
{
  u8   crcOrError;
  u8   command;
  u16  address;
  u8   length;
  u8   indexCnt;
  u8   dataBuffer[256];      


  // Startup mode setup
  if( ( mode == 0 ) && ( isAppCorrupted() == 0 ) ){

    // Application is 'operational' and there was no hard request
    // to enter the boot loader. Start after a while unless
    // there is boot request from the serial communication
    
    // Enable timer 0 with frequency (Fclk/1024)
    // With 4Mhz clock it gives about 65ms
    // If not sufficent 16 bit timer 1 can be used 

    outp( ( 1<<CS02 ) | ( 1<<CS01 ) | ( 1 << CS00 ), TCCR0 );
    outp( 0, TCNT0 );
  }
  else {
    // Application is not 'operational' or hard boot request
    // There will be no timeout, the control will stay with boot loader
    outp( 0, TCCR0 );
  }
  
  // Just in case - clear 'overrun' status for the timer 0
  outp( ( 1<<TOV0 ), TIFR );   
  
  // Initialize serial communication
  uart_init();
  
  for(;;) {    
    // Wait for frame start character   
    // For the start character disable frame character timeout
    outp( 0, TCCR2 ); 
    outp( ( 1<<TOV2 ), TIFR ); 
    
    for(;;) {  
      if(  uart_get() == '!'){
	// Start character found
	break;
      }
    }
    
    // Enable timeout for frame character timouts
    // Frequency Fclk/128 gives about 8ms with 4Mhz clock
    outp( 0, TCNT2 );
    outp( ( 1<<CS02 ) | ( 1<<CS00 ), TCCR2 );
    
    // Reset crc
    crcOrError = 0;
    
    // Get frame data length
    length      = uart_get();
    crcOrError ^= length;
    
    // Get command code
    command     = uart_get();           
    crcOrError ^= command;
    

    // Get data bytes together with last crc byte
    indexCnt   = 0;  
    while( indexCnt <= length ) {	         
      dataBuffer[indexCnt]    = uart_get();
      crcOrError ^= dataBuffer[indexCnt];
      indexCnt++;
    }   

    // Check, maybe there was a timeout in the frame ?
    // (because then the data is not valid)
    if( bit_is_set( TIFR, TOV2 ) ){
      // If so, start again looking for the start character
      continue;
    }

    // Crc check
    if ( crcOrError == 0 ){     
      if ( length == FRAME_COMMAND ){	
	if ( command == 'A' ) {
	  
	  // Erase page command 
	  address  = (u16)dataBuffer[ 0 ] | (((u16)dataBuffer[ 1 ])<< 8);
	  writePage( address,( 1<<PGERS ) + ( 1<<SPMEN ));               
	  
	  // Reenable reading flash
	  writePage( address, (1<<RWWSRE) + ( 1<<SPMEN ));               
	}
	else if( command == 'B' ){
	  
	  // Write page    
	  address  = (u16)dataBuffer[ 0 ] | (( (u16)dataBuffer[ 1 ] )<< 8 );
	  writePage( address,( 1<<PGWRT ) + ( 1<<SPMEN ) ); 
	  
	  // Reenable reading flash
	  writePage( address, ( 1<<RWWSRE ) + ( 1<<SPMEN ) );               
	}
	else if(command == 'C'){
	  //Mega128 specific- Select low/high 64K area 
	  if ( dataBuffer[ 0 ] == 0 ){
	    lowFlashBank();
	  }
	  else {
	    highFlashBank();
	  }									 	}
	else if( command == 'D' ){
	  // Start the timeout again
	  // We could jump to the application directly, but 
	  // then ack would be lost.
	  // The jump will occur after the timeout expires
	  outp( ( 1<<CS02 ) | ( 1<<CS01 ) | ( 1<<CS00 ), TCCR0 );
	}
	else if( command == 'E' ){
	  // Read flash contents
	  address = (u16)dataBuffer[ 0 ] | (((u16)dataBuffer[ 1 ] ) << 8 );
	  for ( indexCnt = 0; indexCnt < FRAME_DATA_READ; indexCnt++ ){
	    uart_put( readProgramMemory( address + indexCnt ));               
	  }
	}
	else if( command == 'F' ){
	  // Disable boot loader timeout - stop the timer
	  outp( 0, TCCR0 ); 
	}
	else if(command == 'H'){
	  // Return signature
	  uart_put( 0x10 ); 
	}
	else {
	  crcOrError = 1; // Unknown command
	}	
      }
      else if ( length == FRAME_DATA_WRITE ) {
	// Program application data
	address  = (u16)dataBuffer[ 0 ] | (((u16)dataBuffer[1])<< 8 );
	indexCnt = 2;
	
	while ( indexCnt < length ) {
	  fillTempBuffer( address,*((u16 *)&dataBuffer[ indexCnt ]) );
	  address += 2;
	  indexCnt += 2;
	}
      }
      else {
	crcOrError = 1; // wrong frame length
      }      
    }
    
    // Operation result check
    if( crcOrError != 0 ){ 
      //error, send NACK
      uart_put('N');
    }
    else {
      //success, send ACK
      uart_put('A');
    }    
  }  
}














