//AVR bootloader 0.0

#include <avr/io.h>
#include "vscp_projdefs.h"
#include "vscp_compiler.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <avr/boot.h>
#include "bootloader.h"
#include "AAboot.h"
#include "version.h"
#include "can_at90can128.h"
#include "uart.h"
//#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_registers.h"
#include <avr/wdt.h>
//#include <util/delay.h>

#define CRC16
#include "./../../../src/common/crc.h"

// ========================================================================================================================================================
uint8_t vscp_nickname; ///< Assigned node nickname
struct _imsg vscp_imsg; ///< Current input event
struct _omsg vscp_omsg; ///< Current outgoing event


#define LED_PORT PORTA 
#define LED_PIN  PINA
#define LED_DDR  DDRA 

volatile uint8_t LED_PIN_NUMBER = 0;

///////////////////////////////////////////////////////////////////////////////
// main
//

int main( void )
{
	//uint32_t i;

	//LED_DDR |= (1<< LED_PIN_NUMBER);
	LED_DDR = 0xff;
	//DDRA = 0xFF;
	 //LED_PORT |= (1<< LED_PIN_NUMBER);  // OFF LED

	 //DDRA |= (1<< LED_PIN_NUMBER);
	 //PORTA = (1<< LED_PIN_NUMBER);

	//for( i=0; i<=5000 ; i++)
	//{
	  
	
	//}
	//_delay_ms(20);
	//_delay_ms(20);
	//_delay_ms(20);
	//_delay_ms(20);
	//_delay_ms(20);
	//_delay_ms(20);
	//_delay_ms(20);
	//_delay_ms(20);
	//_delay_ms(20);
	//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
	 //while(1){}

 	// Initialize UART
    UCSRA = 0;
    UCSRC = MSK_UART_8BIT;	// 8N1
    Uart_set_baudrate( 9600 );
    UCSRB = MSK_UART_ENABLE_TX | MSK_UART_ENABLE_RX;

	#ifdef PRINT_DEBUG_EVENTS
	uart_puts( "AAboot 0.0\n" );
	#endif

   //checkout bootloader_flag
	int bootloader_flag = readEEPROM(VSCP_EEPROM_BOOTLOADER_FLAG);
	switch (bootloader_flag)
	{
		case BTL_RUN:
			//jump to app
			#ifdef PRINT_DEBUG_EVENTS
				uart_puts( "boot->run app\n" );
			#endif
			//while (1) {} //remove for debug

			//LED_PORT |= (1<< LED_PIN_NUMBER);  // OFF LED

			asm volatile("jmp 0x00" "\n\t");
			break;
		case BTL_FRESH:
		// todo add this function, with nodeid 255?
		//or always upload a mixed build hex file to avoid this
			#ifdef PRINT_DEBUG_EVENTS
				uart_puts( "boot->fresh app\n" );
			#endif
			//while (1) {} //to add if no mix is possible

			//LED_PORT |= (1<< LED_PIN_NUMBER);  // OFF LED

			asm volatile("jmp 0x00" "\n\t");
			break;
		case BTL_LOAD:
			#ifdef PRINT_DEBUG_EVENTS
				uart_puts( "boot->load app\n" );
			#endif
			vscp_init();
			mainbootloader();
			break;
	}
	while (1){}// should not be reached
}//main



void mainbootloader()
{
	//uint8_t hhh = 0;
	uint16_t crc_16 = 0;
	

	cli();
    wdt_disable();

	//Initialise CRC lookup table.
	crcInit();

	// Init can
    if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) 
	{
        uart_puts("CAN Failed!!\n");
    }

	//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED

	//send out ACK to bootloader
	#ifdef PRINT_DEBUG_EVENTS
		uart_puts( "boot->ack\n" );
	#endif
	
	vscp_omsg.priority = 7;
	vscp_omsg.flags = VSCP_VALID_MSG + 8;
	vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.type =  VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER;
	vscp_omsg.data[ 0 ] = 0x00;
	vscp_omsg.data[ 1 ] = 0x00;
	vscp_omsg.data[ 2 ] = (BTL_BLOCKSIZE & 0xFF00) >> 8;
	vscp_omsg.data[ 3 ] = (BTL_BLOCKSIZE & 0x00FF);
	vscp_omsg.data[ 4 ] = 0x00;
	vscp_omsg.data[ 5 ] = 0x00;
	vscp_omsg.data[ 6 ] = (BTL_NRofBLOCKS & 0xFF00) >> 8;
	vscp_omsg.data[ 7 ] = (BTL_NRofBLOCKS & 0x00FF);
	vscp_sendEvent();	// Send ACK

	unsigned char BTL_STAT = 'A';  				//awaiting to receive a start block data transfer
	unsigned int BTL_BLOCK = 1;					// received datapointer
	uint8_t BTL_BUFFER[BTL_BLOCKSIZE] = "0"; 	// databuffer
	unsigned int BTL_PAGE  = 0;					// page to program in AVR
	unsigned char i=0;

	// received datablocks go into BTL_BUFFER, when full this buffer is flashed into page set by BTL_PAGE
	while (BTL_STAT != 'R')
	{
		while (BTL_STAT == 'A')
		{ 
			// Check for any valid CAN message
	    	vscp_imsg.flags = 0;
	    	vscp_getEvent();
			if ( vscp_imsg.flags & VSCP_VALID_MSG ) 
			{	
				//vscp_imsg.flags &= ~(VSCP_VALID_MSG);
				// incoming message?
				#ifdef PRINT_CAN_EVENTS
                    char buf[30];
                    uint8_t i;
                    sprintf(buf, "rx: %03x/%02x/%02x/",
                    vscp_imsg.class, vscp_imsg.type, vscp_imsg.oaddr);
                    for (i=0; i<(vscp_imsg.flags&0xf); i++) 
					{
                        char dbuf[5];
                        sprintf(dbuf, "/%02x", vscp_imsg.data[i]);
                        strcat(buf, dbuf);
                    }
                    uart_puts(buf);
				#endif
                //vscp_handleProtocolEvent();
				if (vscp_imsg.class == VSCP_CLASS1_PROTOCOL)
				{
					switch( vscp_imsg.type ) 
					{
						case VSCP_TYPE_PROTOCOL_START_BLOCK:

						//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED

							BTL_PAGE = (vscp_imsg.data[2]*0x100) + vscp_imsg.data[3];
							BTL_BLOCK = 1;
							BTL_STAT = 'S';  //Start block received
							#ifdef PRINT_DEBUG_EVENTS					
							uart_puts("STARTBLOCK");
							#endif
						    //sendChar(BTL_PAGE);
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // Toggle LED
							break;
						case VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE:
							//writeEEPROM(VSCP_EEPROM_BOOTLOADER_FLAG,BTL_RUN);
							BTL_STAT = 'R'; //reboot

							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // Toggle LED
							break;
					} //switch
				}// if protocol
			}// if valid msg
		}//while stat A

	
	
		while (BTL_STAT == 'S')
		{ 
			// Check for any valid CAN message
	    	vscp_imsg.flags = 0;
	    	vscp_getEvent();
			if ( vscp_imsg.flags & VSCP_VALID_MSG ) 
			{	
				//vscp_imsg.flags &= ~(VSCP_VALID_MSG);
				// incoming message?
				#ifdef PRINT_CAN_EVENTS
	                char buf[30];
	                uint8_t i;
	                sprintf(buf, "rx: %03x/%02x/%02x/",
	                vscp_imsg.class, vscp_imsg.type, vscp_imsg.oaddr);
	                for (i=0; i<(vscp_imsg.flags&0xf); i++) 
					{
	                    char dbuf[5];
	                    sprintf(dbuf, "/%02x", vscp_imsg.data[i]);
	                    strcat(buf, dbuf);
	                }
	                uart_puts(buf);
				#endif
				if (vscp_imsg.class == VSCP_CLASS1_PROTOCOL)
				{
					//LED_PORT ^= (1<< LED_PIN_NUMBER);  // Toggle LED

					switch( vscp_imsg.type ) 
						{
						case VSCP_TYPE_PROTOCOL_BLOCK_DATA:
							//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
					        //while(1);

							#ifdef PRINT_DEBUG_EVENTS				
							uart_puts("BOOT_DATABLOCK");
							#endif
/*i.e from 1 to 32*/	    if (BTL_BLOCK <= (BTL_BLOCKSIZE/8)/*32*/) //prevent overflow                                 //  BUFFERSIZE         128  words = 256 bytes -- i.e 32*8 = 256
							{                                                // Datasheet ATCAN128.pdf  --- // Table 25-11. No. of Words in a Page and No. of Pages in the Flash
								for (i=0; i<(vscp_imsg.flags&0xf); i++) 
								{
									//#ifdef PRINT_DEBUG_EVENTS				
									//uart_puts("BOOT_DATA");
									//#endif
									BTL_BUFFER[(((BTL_BLOCK-1)*0x08) + i)] = vscp_imsg.data[i]; //put incoming data in buffer
	                    		}
             /*After 32 programmed -- it will become 33*/
								BTL_BLOCK++;
							}

	                		break;
						} //switch

					    /*After 32 programmed -- it will become 33*/
						if (BTL_BLOCK > (BTL_BLOCKSIZE/8)/*32*/)
						{	
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
							 
							// Calculate CRC in host
							crc_16   = crcFast(&BTL_BUFFER[ 0], BTL_BLOCKSIZE);

							//while(1);
							#ifdef PRINT_DEBUG_EVENTS				
							uart_puts("BOOT_FULLBLOCK");
							#endif
							// todo: check CRC of written data, if ok send confirm block event
							BTL_BLOCK = 1;
							// send confirm block event
								vscp_omsg.priority = 0;
								vscp_omsg.flags = VSCP_VALID_MSG + 4;
			    				vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
				    			vscp_omsg.type =  VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK;
								vscp_omsg.data[ 0 ] = (uint8_t)(crc_16>>8); //MSB 
								vscp_omsg.data[ 1 ] = (uint8_t)(crc_16);
								vscp_omsg.data[ 2 ] = (BTL_PAGE & 0xFF00) >> 8;
								vscp_omsg.data[ 3 ] = (BTL_PAGE & 0x00FF);
								
								LED_PORT ^= (1<< LED_PIN_NUMBER);
				    			crc_16 = 0;

								vscp_sendEvent();	// Send ACK

								//boot_program_page ((BTL_PAGE*0x100),BTL_BUFFER);

								BTL_STAT= 'B'; //Block complete, waiting to write
						}//if btl_block==32
				}// if valid msg
			}// if ( vscp_imsg.flags & VSCP_VALID_MSG ) 
		}//while stat S

		while (BTL_STAT == 'B')
		{ 
			
			// Check for any valid CAN message
	    	vscp_imsg.flags = 0;
	    	vscp_getEvent();
			if ( vscp_imsg.flags & VSCP_VALID_MSG ) 
			{	// incoming message?

				//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
				//LED_PORT ^= 0xff;
				//vscp_imsg.flags &= ~(VSCP_VALID_MSG);

				if ((vscp_imsg.class == VSCP_CLASS1_PROTOCOL)&&(vscp_imsg.type == VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA)) 
				{
					/*
					if(hhh >6)
				hhh=0;

			LED_PORT ^= (1<< hhh);
			hhh++;*/
					//LED_PORT = 0xff;

					//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
					//PORTA = 0xFF;
					#ifdef PRINT_DEBUG_EVENTS				
						uart_puts("BOOT_WRITE");
					#endif
					boot_program_page ((BTL_PAGE*0x100),BTL_BUFFER);
					// send ACK program block event 
					vscp_omsg.priority = 0;
					vscp_omsg.flags = VSCP_VALID_MSG;
		    		vscp_omsg.class = VSCP_CLASS1_PROTOCOL;
			    	vscp_omsg.type =  VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK;
					vscp_sendEvent();	// Send ACK
					BTL_STAT = 'A';  //page written, back Awaiting new block 
				}
				// else send NACK program block event ?
			}// if valid msg
		}//while stat B
	} //while stat != R

	#ifdef PRINT_DEBUG_EVENTS					
		uart_puts("BOOT_END");
	#endif

    //LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
	//DDRA = 0xFF;
	//PORTA = 0xFF;
	//LED_PORT |= (1<< 0);
    writeEEPROM(VSCP_EEPROM_BOOTLOADER_FLAG,BTL_RUN);
	while ( EECR & ( 1 << EEWE ) );


	//reboot by using WD
	//cli();
	boot_rww_enable ();
	wdt_enable(WDTO_1S);
	while(1);
} //mainbootloader



///////////////////////////////////////////////////////////////////////////////
//   						VSCP Functions
//							--------------
//
// All methods below must be implemented to handle VSCP functionality
//
///////////////////////////////////////////////////////////////////////////////




///////////////////////////////////////////////////////////////////////////////
// sendVSCPFrame
//
//

int8_t sendVSCPFrame( uint16_t vscpclass,
                        uint8_t vscptype,
                        uint8_t nodeid,
                        uint8_t priority,
                        uint8_t size,
                        uint8_t *pData )
{
    CANMsg msg;


#ifdef PRINT_CAN_EVENTS
	char buf[32];
	uint8_t i;

	sprintf(buf, "tx: %03x/%02x/%02x/", vscpclass, vscptype, nodeid);
	for (i=0; i<size; i++) {
		char dbuf[5];
		sprintf(dbuf, "/%02x", pData[i]);
		strcat(buf, dbuf);
	}
	uart_puts(buf);
#endif

	msg.id = ( (uint32_t)priority << 26 ) |
		( (uint32_t)vscpclass << 16 ) |
		( (uint32_t)vscptype << 8) |
		nodeid;		// nodeaddress (our address)


    msg.flags = CAN_IDFLAG_EXTENDED;
    msg.len = size;
	if ( size ) {
		memcpy( msg.byte, pData, size );
	}

    if ( ERROR_OK != can_SendFrame( &msg ) ) {
        return FALSE;
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// getVSCPFrame
//
//

int8_t getVSCPFrame( uint16_t *pvscpclass,
                        uint8_t *pvscptype,
                        uint8_t *pNodeId,
                        uint8_t *pPriority,
                        uint8_t *pSize,
                        uint8_t *pData )
{
    CANMsg msg;

    // Check for received frame
    if ( ERROR_OK != can_readFrame( &msg ) ) {
        return FALSE;
    }

    *pNodeId = msg.id & 0x0ff;
    *pvscptype = ( msg.id >> 8 ) & 0xff;
    *pvscpclass = ( msg.id >> 16 ) & 0x1ff;
    *pPriority = (uint16_t)( 0x07 & ( msg.id >> 26 ) );
    *pSize = msg.len;
    if ( msg.len ) {
        memcpy( pData, msg.byte, msg.len );
    }

    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// readEEPROM
//
//

int readEEPROM( uint8_t addr )
{
    while ( EECR & ( 1 << EEWE ) );
    EEAR = addr;
    EECR |= ( 1 << EERE );
    return EEDR;
}

///////////////////////////////////////////////////////////////////////////////
// writeEEPROM
//
//

int writeEEPROM( uint8_t addr, uint8_t data )
{
    while ( EECR & ( 1 << EEWE ) );
    EEAR = addr;
    EEDR=data;
    EECR |= ( 1 << EEMWE );
    EECR |= ( 1 << EEWE );
    return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
//                        VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// vscp_readNicknamePermanent
//
// Fetch nickname from permanent storage
// return read nickname.
//

uint8_t vscp_readNicknamePermanent( void )
{
    return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
// 	vscp_writeNicknamePermanent
//
//	Write nickname to permanent storage
//

void vscp_writeNicknamePermanent( uint8_t nickname )
{
    writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getBufferSize
//
// Get the buffer size
//

uint8_t vscp_getBufferSize( void )
{
    return VSCP_SIZE_STD_DM_ROW;	// Standard CAN frame
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getNickname
//

uint8_t vscp_getNickname( void )
{
    return readEEPROM( VSCP_EEPROM_NICKNAME );
}


///////////////////////////////////////////////////////////////////////////////
void boot_program_page (uint32_t page, uint8_t *buf)
    {
        uint16_t i;
        uint8_t sreg;

        // Disable interrupts.

        sreg = SREG;
//        cli();
    
        eeprom_busy_wait ();

        boot_page_erase (page);
        boot_spm_busy_wait ();      // Wait until the memory is erased.

        for (i=0; i<(BTL_BLOCKSIZE-1); i+=2)
        {
            // Set up little-endian word.

            uint16_t w = *buf++;
            w += (*buf++) << 8;
        
            boot_page_fill (page + i, w);
//		boot_page_fill (i, 0x1123);
        }

        boot_page_write (page);     // Store buffer in flash page.
        boot_spm_busy_wait();       // Wait until the memory is written.

        // Reenable RWW-section again. We need this if we want to jump back
        // to the application after bootloading.

        boot_rww_enable ();

        // Re-enable interrupts (if they were ever enabled).

        SREG = sreg;
    }





// doWork
//
// Do work here
//

void doWork( void )
{

}






//debug routines

void sendCharHex (char data)
{
	// sends out a byte in its HEX-notation
	unsigned char dataC = 0;
	dataC = data; //backup
	
	data = (data >> 4);
	data = (data & 0x0F);
	if ((data >= 0) && (data <= 9)) sendChar(data+0x30);
	else sendChar(data+0x37);

	data = (dataC & 0x0F); //start from the backup
	if ((data >= 0) && (data <= 9)) sendChar(data+0x30);
	else sendChar(data+0x37);
	
	sendChar (' ');

	
}

void sendChar (char data)
{	
	int i = 0;
	// to send data with the usart put the data in the usart data register
	UDR0 = data;
	
	// check to see if the global interrupts are enabled
	if (SREG & 0x80) 
	{
		// wait until the byte is sent or we count out
			while ( !(UCSR0A&0x40) && (i<10000) )
			{
				asm ("nop");
				i++;
			}
	}
	else
		// wait until the byte is sent
		while ( !(UCSR0A&0x40) );
		
		// clear the TXCflag
		UCSR0A=(UCSR0A|0x40);
}





//==================================================================================================================================================================

///////////////////////////////////////////////////////////////////////////////
// vscp_init
//

void vscp_init(void)
{
    //vscp_initledfunc = VSCP_LED_BLINK1;

    // read the nickname id
    vscp_nickname = vscp_readNicknamePermanent();

    //	if zero set to uninitialized
    if (!vscp_nickname) vscp_nickname = VSCP_ADDRESS_FREE;

    // Init incoming event
    vscp_imsg.flags = 0;
    vscp_imsg.priority = 0;
    vscp_imsg.class = 0;
    vscp_imsg.type = 0;

    // Init outgoing event
    vscp_omsg.flags = 0;
    vscp_omsg.priority = 0;
    vscp_omsg.class = 0;
    vscp_omsg.type = 0;

/*    vscp_errorcnt = 0; // No errors yet
    vscp_alarmstatus = 0; // No alarmstatus

    vscp_probe_address = 0;

    // Initial state
    vscp_node_state = VSCP_STATE_STARTUP;
    vscp_node_substate = VSCP_SUBSTATE_NONE;

    vscp_probe_cnt = 0;
    vscp_page_select = 0;

    // Initialize time keeping
    vscp_timer = 0;
    vscp_second = 0;
    vscp_minute = 0;
    vscp_hour = 0;
	*/
}


///////////////////////////////////////////////////////////////////////////////
// vscp_sendEvent
//

int8_t vscp_sendEvent(void)
{
    int8_t rv;

    if (!(rv = sendVSCPFrame(vscp_omsg.class,
            vscp_omsg.type,
            vscp_nickname,
            vscp_omsg.priority,
            (vscp_omsg.flags & 0x0f),
            vscp_omsg.data))) {
        //vscp_errorcnt++;
    }

    return rv;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getEvent
//

int8_t vscp_getEvent(void)
{
    int8_t rv;


    // Dont read in new event if there already is an event
    // in the input buffer. We return TRUE though to indicate there is
    // a valid event.
    if (vscp_imsg.flags & VSCP_VALID_MSG) return TRUE;


    if ((rv = getVSCPFrame(&vscp_imsg.class,
            &vscp_imsg.type,
            &vscp_imsg.oaddr,
            &vscp_imsg.priority,
            &vscp_imsg.flags,
            vscp_imsg.data))) {

        vscp_imsg.flags |= VSCP_VALID_MSG;
    }

    return rv;

}
