// Copyright (C) 2002 Jaroslaw Karwik, 
//   Mente Marine Oy <jaroslaw.karwik@mente-marine.com>
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




#include "target.h"
#include "prog.h"


// Definitions for data frame lengths
#define  FRAME_COMMAND          2
#define  FRAME_DATA_WRITE       66
#define  FRAME_DATA_READ        64		   

// Startup related data
#define  MARKER_BYTE_ADDRESS    (0x1FC00 - 2)
#define  MARKER_MAGIC            0xABBA

// RS485 related definitions
// Note that the code assumes that these pins are on port D

#define  PIN_nRE	3
#define  PIN_DE		4

//******************************************************************************************
//
//  uart_init - Serial communication setup  
//
//******************************************************************************************

void   uart_init(void)
{  
       // Setup UART baudrate (uses UART0)
       //
       // Note : The baudrate is formed by combination of
       //        UBRR0H, UBRR0L and bit U2X in UCSRA
	   //        Currently 19200 with 4MHz clock

	   SET_PORT(UBRR0L,12);
       SET_PORT(UBRR0H,0);            
                                       
       // Setup UART control bits 

	   SET_PORT(UCSR0A,0);          
       SET_PORT(UCSR0B,(1<<RXEN)  | (1<<TXEN)  ); // Enable recieve and transmit UART
       SET_PORT(UCSR0C,(1<<UCSZ0) | (1<<UCSZ1) ); // Asynchronous, no parity, 1 stop bit, 8 bit data

       // Setup pins for RS485 dataflow control  

	   SET_PORT(DDRD, (1<<PIN_nRE)|(1<<PIN_DE)  ); // Set control pins as outputs
	   CLR_PORT_BIT(PORTD, PIN_nRE );  			   // Enable receiver
	   CLR_PORT_BIT(PORTD, PIN_DE );  			   // Disable transmitter
}


//******************************************************************************************
//
//  uart_put - Sends one character, waits until completed
//
//			   Note: Shorter version with UDRE could be used,
//					 but it could cause problems with RS485
//					 when switching transmission direction
//					 (last character could be lost)	
//
//******************************************************************************************

void   uart_put(Uint8 cc)
{
       SET_PORT(UDR0,cc);

       while ( ( GET_PORT(UCSR0A) & (1<<TXC) ) == 0 ){
          // wait until byte sent      
       }

	   SET_PORT(UCSR0A,(1<<TXC));
}


//******************************************************************************************
//
//  uart_get - Receives one character. Starts application if application timeout
//			   occured. Returns immediately if frame character timeout occured.
//
//******************************************************************************************
       
Uint8   uart_get(void)
{
   	    	      
       while( ( GET_PORT(UCSR0A) & (1<<RXC) ) == 0  ){

          WATCHDOG_RESET();     
       	
	      if( CHK_PORT_BIT(TIFR,TOV0) ){
		     // Application timeout. We should start the application
		   	 pr_runApplication();			
		  }
	 
		  if( CHK_PORT_BIT(TIFR,TOV2) ){
		     // Character timeout (characters in the frame have to come within certain time)
			 return 0;
		  }
	 
       }

       // Reset timer responsible for character timeout
	   SET_PORT(TCNT2,0); 

       return   GET_PORT(UDR0);
}


//******************************************************************************************
//
//  isAppCorupted - Checks for presence of valid application in the flash
//					This version uses one flash sector just for these two bytes.
//
//******************************************************************************************

Uint8 isAppCorupted(void)
{
   Uint8   result;
  
#if MARKER_BYTE_ADDRESS > 0x10000
   // Note - Mega128 specific - choose high 64K flash area 
   pr_highFlashBank();
#endif
  
   if(    
       (pr_readProgramMemory( ( MARKER_BYTE_ADDRESS    & 0xFFFF) ) ==  (MARKER_MAGIC >> 8)  ) &&
	   (pr_readProgramMemory( ((MARKER_BYTE_ADDRESS+1) & 0xFFFF) ) ==  (MARKER_MAGIC& 0xFF) )        
   ){
	  result = 0;
   }else
   {
      result = 1;
   }

#if MARKER_BYTE_ADDRESS > 0x10000
   //  Note - Mega128 specific- choose low 64K flash area
   pr_lowFlashBank();
#endif
  
   return result;
}


//******************************************************************************************
//
//  main - Main loader engine 
//
//******************************************************************************************

int main(int mode,char **argv)
{
   Uint8   crcOrError;
   Uint8   command;
   Uint16  address;
   Uint8   length;
   Uint8   indexCnt;
   Uint8   dataBuffer[256];      



   // Startup mode setup
   if( (mode == 0) && ( isAppCorupted() == 0 ) ){
       // Application is 'operational' and there was no hard request
	   // to enter the boot loader. Start after a while unless
	   // there is boot request from the serial communication

	   // Enable timer 0 with frequency (Fclk/1024)
	   // With 4Mhz clock it gives about 65ms
	   // If not sufficent 16 bit timer 1 can be used 

       SET_PORT(TCCR0,(1<<CS2)|(1<<CS1)|(1<<CS0));
       SET_PORT(TCNT0,0);
   }else
   {
       // Application is not 'operational' or hard boot request
       // There will be no timeout, the control will stay with boot loader
	   SET_PORT(TCCR0,0);
   }

   // Just in case - clear 'overrun' status for the timer 0
   SET_PORT(TIFR,(1<<TOV0));   

   // Initialize serial communication
   uart_init();
                                        
   for(;;)
   {

       // Wait for frame start character   
       // For the start character disable frame character timeout
       SET_PORT(TCCR2,0); 
	   SET_PORT(TIFR,(1<<TOV2)); 
	    
       for(;;)
       {  
         if(  uart_get() == '!'){
            // Start character found
            break;
         }
       }

	   // Enable timeout for frame character timouts
  	   // Frequency Fclk/128 gives about 8ms with 4Mhz clock
	   SET_PORT(TCNT2,0);
       SET_PORT(TCCR2,(1<<CS2)|(1<<CS0));
       


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
       while(indexCnt <= length)
       {	         
	         dataBuffer[indexCnt]    = uart_get();
        	 crcOrError    			^= dataBuffer[indexCnt];
    	     indexCnt++;
	   }
	   
	   // Check, maybe there was a timeout in the frame ?
	   // (because then the data is not valid)
	   if( CHK_PORT_BIT(TIFR,TOV2) ){
	      // If so, start again looking for the start character
		  continue;
	   }
	


       // RS485 data flow - disable receiver / enable transmitter  
	   SET_PORT_BIT(PORTD, PIN_nRE );  		   
	   SET_PORT_BIT(PORTD, PIN_DE );  		   


       // Crc check
       if( crcOrError == 0 ){
               
          if(length == FRAME_COMMAND){

              if(command == 'A'){
                   // Erase page command 
                   address  = (Uint16)dataBuffer[0] | (((Uint16)dataBuffer[1])<< 8);
                   pr_writePage(address,(1<<PGERS) + (1<<SPMEN));               
                   // Reenable reading flash
                   pr_writePage(address,(1<<RWWSRE) + (1<<SPMEN));               
              }else if(command == 'B'){
                   // Write page    
                   address  = (Uint16)dataBuffer[0] | (((Uint16)dataBuffer[1])<< 8);
                   pr_writePage(address,(1<<PGWRT) + (1<<SPMEN)); 
                   // Reenable reading flash
                   pr_writePage(address,(1<<RWWSRE) + (1<<SPMEN));               
              }else if(command == 'C'){
                   //Mega128 specific- Select low/high 64K area 
                   if(dataBuffer[0] == 0){
                     pr_lowFlashBank();
                   }else
                   {
                     pr_highFlashBank();
                   }									 
              }else if(command == 'D'){
                   // Start the timeout again
				   // We could jump to the application directly, but then ack would be lost.
				   // The jump will occur after the timeout expires
				   SET_PORT(TCCR0,(1<<CS2)|(1<<CS1)|(1<<CS0));
              }else if(command == 'E'){
                   // Read flash contents
                   address    = (Uint16)dataBuffer[0] | (((Uint16)dataBuffer[1])<< 8);
   				   for(indexCnt   = 0; indexCnt < FRAME_DATA_READ; indexCnt++){
                      uart_put(pr_readProgramMemory(address + indexCnt));                     
                   }
              }else if(command == 'F'){
                   // Disable boot loader timeout - stop the timer
                   SET_PORT(TCCR0,0); 
              }else if(command == 'H'){
                   // Return signature
                   uart_put(0x10); 
              }else           
              {
                   crcOrError = 1; // Unknown command
              }

          }else if (length == FRAME_DATA_WRITE){
              // Program application data
              address  = (Uint16)dataBuffer[0] | (((Uint16)dataBuffer[1])<< 8);
              indexCnt = 2;

              while( indexCnt < length )
              {
                pr_fillTempBuffer( address,*((Uint16 *)&dataBuffer[indexCnt]) );
                address  += 2;
                indexCnt += 2;
              }
          }else
          {
            crcOrError = 1; // wrong frame length
          }      
       }

       // Operation result check
       if(crcOrError != 0){ 
          //error, send NACK
          uart_put('N');
       }else
       {
          //success, send ACK
          uart_put('A');
       }

       // RS485 data flow - disable transmitter / enable receiver
	   CLR_PORT_BIT(PORTD, PIN_DE );  		   
	   CLR_PORT_BIT(PORTD, PIN_nRE );  		   

   }

}






