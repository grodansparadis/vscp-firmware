 /*  
	
	// Now we can get a node ACK for this id in that case
    // we should give up
    while ( 1 ) {

		e.flags = 0;
	    vscpboot_getEvent( &e );
	
		if ( e.flags & VSCP_VALID_MSG ) {	// incoming event?
#ifdef PRINT_CAN_EVENTS
		    char buf[30];
			uint8_t i;
			sprintf(buf, "rx: %03x/%02x/%02x/",
			e.vscp_class, e.vscp_type, e.oaddr);
			for ( i=0; i<5; i++ ) {
				char dbuf[5];
				sprintf(dbuf, "/%02x", e.data[i]);
				strcat(buf, dbuf);
			}
			uart_puts(buf);
#endif
					
		    if (VSCP_CLASS1_PROTOCOL == e.vscp_class) {

                //vscp_handleProtocolEvent();}
	            if (e.vscp_type == VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER)
				{
					if ((VSCP_ADDRESS_BLANKDEV == e.data[ 0 ]) &&
                // byte 1 contains algorithm. Handle in callback.
                (vscp_getGUID(0) == e.data[ 2 ]) &&
                (vscp_getGUID(3) == e.data[ 3 ]) &&
                (vscp_getGUID(5) == e.data[ 4 ]) &&
                (vscp_getGUID(7) == e.data[ 5 ]) &&
                (((vscp_page_select >> 8) & 0xff) == e.data[ 6 ]) &&
                ((vscp_page_select & 0xff) == e.data[ 7 ])) )
                //vscp_goBootloaderMode( e.data[ 1 ] );
					mainbootloader();
				}
				}
            }
			
		}
}

*/
  
  
///////////////////////////////////////////////////////////////////////////////
// vscpboot_bootloaders
//

void vscpboot_bootloader( void )
{
    vscpevent e;
    vscpboot_state state = STATE_VIRGIN;    // We haven't done anything yet
    uint32_t block = 1;					    // Block to program
    uint8_t buf[ VSCPBOOT_BLOCKSIZE ]; 	    // data buffer
    unsigned int page  = 0;                 // page to program in AVR
    unsigned char i = 0;
    
	uint16_t crc16 = 0;

	cli();
    wdt_disable();

	//Initialize CRC lookup table.
	crcInit();

	//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED

	e.priority = VSCP_PRIORITY_NORMAL;
	e.flags = VSCP_VALID_MSG + 8;
	e.vscp_class = VSCP_CLASS1_PROTOCOL;
	e.vscp_type =  VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER;
	e.data[ 0 ] = 0x00;
	e.data[ 1 ] = 0x00;
	e.data[ 2 ] = ( VSCPBOOT_BLOCKSIZE >> 8 ) & 0xFF;
	e.data[ 3 ] = ( VSCPBOOT_BLOCKSIZE & 0x00FF);
	e.data[ 4 ] = 0x00;
	e.data[ 5 ] = 0x00;
	e.data[ 6 ] = ( VSCPBOOT_BLOCKS >> 8 ) & 0xFF; 
	e.data[ 7 ] = ( VSCPBOOT_BLOCKS & 0x00FF );
	vscp_sendEvent();	// Send ACK
	
	

	// received data blocks go into VSCPBOOT_BUFFER, when full this buffer is flashed into page set by BTL_PAGE
	while (state != 'R')
	{
		while (state == 'A')
		{ 
			// Check for any valid CAN message
	    	e.flags = 0;
	    	vscp_getEvent();
			if ( e.flags & VSCP_VALID_MSG ) 
			{	
				//e.flags &= ~(VSCP_VALID_MSG);
				// incoming message?
				#ifdef PRINT_CAN_EVENTS
                    char buf[30];
                    uint8_t i;
                    sprintf(buf, "rx: %03x/%02x/%02x/",
                    e.vscp_class, e.vscp_type, e.oaddr);
                    for (i=0; i<(e.flags&0xf); i++) 
					{
                        char dbuf[5];
                        sprintf(dbuf, "/%02x", e.data[i]);
                        strcat(buf, dbuf);
                    }
                    uart_puts(buf);
				#endif
                //vscp_handleProtocolEvent();
				if (e.vscp_class == VSCP_CLASS1_PROTOCOL)
				{
					switch( e.vscp_type ) 
					{
						case VSCP_TYPE_PROTOCOL_START_BLOCK:

						//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED

							page = (e.data[2]*0x100) + e.data[3];
							block = 1;
							state = 'S';  //Start block received

						    //sendChar(BTL_PAGE);
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // Toggle LED
							break;
						case VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE:
							//writeEEPROM(VSCP_EEPROM_BOOTLOADER_FLAG,BTL_RUN);
							state = 'R'; //reboot

							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // Toggle LED
							break;
					} //switch
				}// if protocol
			}// if valid msg
		}//while stat A



	
		while (state == STATE_VIRGIN)
		{ 
			// Check for any valid CAN message
	    	e.flags = 0;
	    	vscp_getEvent();
			if ( e.flags & VSCP_VALID_MSG ) 
			{	
				//e.flags &= ~(VSCP_VALID_MSG);
				// incoming message?
				#ifdef PRINT_CAN_EVENTS
	                char buf[30];
	                uint8_t i;
	                sprintf(buf, "rx: %03x/%02x/%02x/",
	                e.vscp_class, e.vscp_type, e.oaddr);
	                for (i=0; i<(e.flags&0xf); i++) 
					{
	                    char dbuf[5];
	                    sprintf(dbuf, "/%02x", e.data[i]);
	                    strcat(buf, dbuf);
	                }
	                uart_puts(buf);
				#endif
				if (e.vscp_class == VSCP_CLASS1_PROTOCOL)
				{
					//LED_PORT ^= (1<< LED_PIN_NUMBER);  // Toggle LED

					switch( e.vscp_type ) 
						{
						case VSCP_TYPE_PROTOCOL_BLOCK_DATA:
							//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
					        //while(1);

/*i.e from 1 to 32*/	    if (VSCPBOOT_BLOCKS <= (VSCPBOOT_BLOCKSIZE/8)/*32*/) //prevent overflow                                 //  BUFFERSIZE         128  words = 256 bytes -- i.e 32*8 = 256
							{                                                // Datasheet ATCAN128.pdf  --- // Table 25-11. No. of Words in a Page and No. of Pages in the Flash
								for (i=0; i<(e.flags&0xf); i++) 
								{
									//#ifdef PRINT_DEBUG_EVENTS				
									//uart_puts("BOOT_DATA");
									//#endif
									buf[(((VSCPBOOT_BLOCKS-1)*0x08) + i)] = e.data[i]; //put incoming data in buffer
	                    		}
             /*After 32 programmed -- it will become 33*/
								block++;
							}

	                		break;
						} //switch

					    /*After 32 programmed -- it will become 33*/
						if ( block > (VSCPBOOT_BLOCKSIZE/8)/*32*/)
						{	
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT &= ~(1<< LED_PIN_NUMBER);  // ON LED
							//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
							 
							// Calculate CRC in host
							crc16   = crcFast( buf, VSCPBOOT_BLOCKSIZE);

							//while(1);
#ifdef PRINT_DEBUG_EVENTS				
							//uart_puts("BOOT_FULLBLOCK");
#endif
							// todo: check CRC of written data, if ok send confirm block event
							block = 1;
							// send confirm block event
						    e.priority = 0;
							e.flags = VSCP_VALID_MSG + 4;
			    			e.vscp_class = VSCP_CLASS1_PROTOCOL;
				    		e.vscp_type =  VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK;
							e.data[ 0 ] = (uint8_t)(crc16>>8); //MSB 
							e.data[ 1 ] = (uint8_t)(crc16);
							e.data[ 2 ] = (page & 0xFF00) >> 8;
							e.data[ 3 ] = (page & 0x00FF);
								
				    		crc16 = 0;

							vscp_sendEvent();	// Send ACK

								//boot_program_page ((BTL_PAGE*0x100),BTL_BUFFER);

								state = 'B'; //Block complete, waiting to write
						}//if btl_block==32
				}// if valid msg
			}// if ( e.flags & VSCP_VALID_MSG ) 
		}//while stat S

		while (state == STATE_VIRGIN)
		{ 
			
			// Check for any valid CAN message
	    	e.flags = 0;
	    	vscp_getEvent();
			if ( e.flags & VSCP_VALID_MSG ) 
			{	// incoming message?

				//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
				//LED_PORT ^= 0xff;
				//e.flags &= ~(VSCP_VALID_MSG);

				if ((e.vscp_class == VSCP_CLASS1_PROTOCOL)&&(e.vscp_type == VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA)) 
				{
					/*
					if(hhh >6)
				hhh=0;

			LED_PORT ^= (1<< hhh);
			hhh++;*/
					//LED_PORT = 0xff;

					//LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
					//PORTA = 0xFF;

					boot_program_page( (page*0x100), buf );
					// send ACK program block event 
					e.priority = 0;
					e.flags = VSCP_VALID_MSG;
		    		e.vscp_class = VSCP_CLASS1_PROTOCOL;
			    	e.vscp_type =  VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK;
					vscp_sendEvent();	// Send ACK
					state = STATE_VIRGIN;  //page written, back Awaiting new block 
				}
				// else send NACK program block event ?
			}// if valid msg
		}//while stat B
	} //while stat != R



    //LED_PORT ^= (1<< LED_PIN_NUMBER);  // ON LED
	//DDRA = 0xFF;
	//PORTA = 0xFF;
	//LED_PORT |= (1<< 0);
    //writeEEPROM( VSCP_EEPROM_BOOTLOADER_FLAG, VSCPBOOT_RUN );
	//while ( EECR & ( 1 << EEWE ) );


	//reboot by using WD
	//cli();
	//boot_rww_enable ();
	//wdt_enable(WDTO_1S);
	//while(1);
      
} //mainbootloader
