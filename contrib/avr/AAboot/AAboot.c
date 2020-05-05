//AVR bootloader 0.0

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
#include "vscp_firmware.h"
#include "vscp_class.h"
#include "vscp_type.h"
#include "vscp_registers.h"
#include <avr/wdt.h>


#ifndef GUID_IN_EEPROM
// GUID is stored in ROM for this module
//		IMPORTANT!!!
//		if using this GUID in a real environment
//		please set the four MSB to a unique id.
//
// IMPORTANT!!!!!
// The GUID is stored MSB byte first
const uint8_t GUID[ 16 ] = {
	0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x00, 0x04, 0x00, 0x00, 0x12
};
#endif

//no mdf for bootloader
// Device string is stored in ROM for this module (max 32 bytes)
//const uint8_t vscp_deviceURL[]  = "127.0.0.1/mdf/AAboot01.xml";

// Manufacturer id's is stored in ROM for this module
// offset 0 - Manufacturer device id 0
// offset 1 - Manufacturer device id 1
// offset 2 - Manufacturer device id 2
// offset 3 - Manufacturer device id 3
// offset 4 - Manufacturer subdevice id 0
// offset 5 - Manufacturer subdevice id 1
// offset 6 - Manufacturer subdevice id 2
// offset 7 - Manufacturer subdevice id 3
const uint8_t vscp_manufacturer_id[8] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x11
};



// Prototypes
static void init_app_eeprom( void );
static void doDM( void );
static void doWork();

///////////////////////////////////////////////////////////////////////////////
// main
//

int main( void )
{
 	// Initialize UART
    UCSRA = 0;
    UCSRC = MSK_UART_8BIT;	// 8N1
    Uart_set_baudrate( 9600 );
    UCSRB = MSK_UART_ENABLE_TX | MSK_UART_ENABLE_RX;

	#ifdef PRINT_DEBUG_EVENTS
	uart_puts( "AAboot 0.2 2017\n" );
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
			asm volatile("jmp 0x00" "\n\t");
			break;
		case BTL_FRESH:
		// todo add this function, with nodeid 255?
		//or always upload a mixed build hex file to avoid this
			#ifdef PRINT_DEBUG_EVENTS
				uart_puts( "boot->fresh app\n" );
			#endif
			//while (1) {} //to add if no mix is possible
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
    wdt_disable();
	// Init can
    if ( ERROR_OK != can_Open( CAN_BITRATE_125K, 0, 0, 0 ) ) 
	{
        uart_puts("CAN Failed!!\n");
    }

	//send out ACK to bootloader
	#ifdef PRINT_DEBUG_EVENTS
		uart_puts( "boot->ack\n" );
	#endif
	
	vscp_omsg.priority = 7;
	vscp_omsg.flags = VSCP_VALID_MSG + 8;
	vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
	vscp_omsg.vscp_type =  VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER;
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
	unsigned int BTL_BLOCK = 0;					// received datapointer
	uint8_t BTL_BUFFER[BTL_BLOCKSIZE+1] = "0"; 	// databuffer
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
			{	// incoming message?
				#ifdef PRINT_CAN_EVENTS
                    char buf[30];
                    uint8_t i;
                    sprintf(buf, "rx: %03x/%02x/%02x/",
                    vscp_imsg.vscp_class, vscp_imsg.vscp_type, vscp_imsg.oaddr);
                    for (i=0; i<(vscp_imsg.flags&0xf); i++) 
					{
                        char dbuf[5];
                        sprintf(dbuf, "/%02x", vscp_imsg.data[i]);
                        strcat(buf, dbuf);
                    }
                    uart_puts(buf);
				#endif
                //vscp_handleProtocolEvent();
				if (vscp_imsg.vscp_class == VSCP_CLASS1_PROTOCOL)
				{
					switch( vscp_imsg.vscp_type ) 
					{
						case VSCP_TYPE_PROTOCOL_START_BLOCK:
						
							BTL_PAGE = (vscp_imsg.data[0]*0x100) + vscp_imsg.data[1];
							BTL_BLOCK = 0;
							BTL_STAT = 'S';  //Start block received
							#ifdef PRINT_DEBUG_EVENTS					
							uart_puts("STARTBLOCK");
							#endif
						    //sendChar(BTL_PAGE);
							break;
						case VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE:
							writeEEPROM(VSCP_EEPROM_BOOTLOADER_FLAG,BTL_RUN);
							BTL_STAT = 'R'; //reboot
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
			{	// incoming message?
				#ifdef PRINT_CAN_EVENTS
	                char buf[30];
	                uint8_t i;
	                sprintf(buf, "rx: %03x/%02x/%02x/",
	                vscp_imsg.vscp_class, vscp_imsg.vscp_type, vscp_imsg.oaddr);
	                for (i=0; i<(vscp_imsg.flags&0xf); i++) 
					{
	                    char dbuf[5];
	                    sprintf(dbuf, "/%02x", vscp_imsg.data[i]);
	                    strcat(buf, dbuf);
	                }
	                uart_puts(buf);
				#endif
				if (vscp_imsg.vscp_class == VSCP_CLASS1_PROTOCOL)
				{
					switch( vscp_imsg.vscp_type ) 
						{
						case VSCP_TYPE_PROTOCOL_BLOCK_DATA:
							#ifdef PRINT_DEBUG_EVENTS_EACH_BLOCK				
							uart_puts("BOOT_DATABLOCK");
							#endif
							if (BTL_BLOCK < 32) //prevent overflow
							{
								for (i=0; i<(vscp_imsg.flags&0xf); i++) 
								{
									//#ifdef PRINT_DEBUG_EVENTS				
									//uart_puts("BOOT_DATA");
									//#endif
									BTL_BUFFER[((BTL_BLOCK*0x08) + i)] = vscp_imsg.data[i]; //put incoming data in buffer
	                    		}
								BTL_BLOCK++;
							}

	                		break;
						} //switch
				if (BTL_BLOCK == 32)
				{	
					#ifdef PRINT_DEBUG_EVENTS				
					uart_puts("BOOT_FULLBLOCK");
					#endif
					// todo: check CRC of written data, if ok send confirm block event
					BTL_BLOCK = 0;
					// send confirm block event
						vscp_omsg.priority = 0;
						vscp_omsg.flags = VSCP_VALID_MSG + 4;
			    		vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
				    	vscp_omsg.vscp_type =  VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK;
						vscp_omsg.data[ 0 ] = 0; //MSB 
						vscp_omsg.data[ 1 ] = 0;
						vscp_omsg.data[ 2 ] = (BTL_PAGE & 0xFF00) >> 8;
						vscp_omsg.data[ 3 ] = (BTL_PAGE & 0x00FF);
				    	vscp_sendEvent();	// Send ACK
						BTL_STAT= 'B'; //Block complete, waiting to write
				}//if btl_block==32
			}// if valid msg
		}//while stat S

		while (BTL_STAT == 'B')
		{ 
			// Check for any valid CAN message
	    	vscp_imsg.flags = 0;
	    	vscp_getEvent();
			if ( vscp_imsg.flags & VSCP_VALID_MSG ) 
			{	// incoming message?
				if ((vscp_imsg.vscp_class == VSCP_CLASS1_PROTOCOL)&(vscp_imsg.vscp_type == VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA)) 
				{
					#ifdef PRINT_DEBUG_EVENTS				
						uart_puts("BOOT_WRITE");
					#endif
					boot_program_page ((BTL_PAGE*0x100),BTL_BUFFER);
					// send ACK program block event 
					vscp_omsg.priority = 0;
					vscp_omsg.flags = VSCP_VALID_MSG;
		    		vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
			    	vscp_omsg.vscp_type =  VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK;
					vscp_sendEvent();	// Send ACK
					BTL_STAT = 'A';  //page written, back Awaiting new block 
				}
				// else send NACK program block event ?
			}// if valid msg
		}//while stat B
	}


	} //while stat != R
	#ifdef PRINT_DEBUG_EVENTS					
		uart_puts("BOOT_END");
	#endif
	//reboot by using WD
	cli();
	wdt_enable(WDTO_1S);
	while(1);
} //mainbootloader


void mainvscp()
{
	// Check VSCP persistent storage and
	// restore if needed
	if ( !vscp_check_pstorage() ) 
	{
		// Spoiled or not initialized - reinitialize
		init_app_eeprom();
	}

	// Initialize the VSCP functionality
	vscp_init();

    while ( 1 ) {  // Loop Forever


        if ( ( vscp_initbtncnt > 200 ) && ( VSCP_STATE_INIT != vscp_node_state ) ) 
		{
	        // State 0 button pressed
            vscp_nickname = VSCP_ADDRESS_FREE;
            writeEEPROM( VSCP_EEPROM_NICKNAME, VSCP_ADDRESS_FREE );
            vscp_init();
			#ifdef PRINT_CAN_EVENTS
            uart_puts("Node initialization");
			#endif
        }


        // Check for any valid CAN message
        vscp_imsg.flags = 0;
        vscp_getEvent();

       switch ( vscp_node_state ) 
		{
		    case VSCP_STATE_STARTUP:        // Cold/warm reset

                // Get nickname from EEPROM
                if ( VSCP_ADDRESS_FREE == vscp_nickname ) 
				{
                    // new on segment need a nickname
                    vscp_node_state = VSCP_STATE_INIT; 	
                }
                else 
				{
                    // been here before - go on
                    vscp_node_state = VSCP_STATE_ACTIVE;
                    vscp_goActiveState();
                }
                break;


            case VSCP_STATE_INIT:           // Assigning nickname
                vscp_handleProbeState();
                break;

            case VSCP_STATE_PREACTIVE:      // Waiting for host initialisation
                vscp_goActiveState();					
                break;


            case VSCP_STATE_ACTIVE:         // The normal state
                if ( vscp_imsg.flags & VSCP_VALID_MSG ) 
				{	// incoming message?
					#ifdef PRINT_CAN_EVENTS
	                    char buf[30];
	                    uint8_t i;
	                    sprintf(buf, "rx: %03x/%02x/%02x/",
	                    vscp_imsg.vscp_class, vscp_imsg.vscp_type, vscp_imsg.oaddr);
	                    for (i=0; i<(vscp_imsg.flags&0xf); i++) 
						{
	                        char dbuf[5];
	                        sprintf(dbuf, "/%02x", vscp_imsg.data[i]);
	                        strcat(buf, dbuf);
	                    }
	                    uart_puts(buf);
					#endif
                    vscp_handleProtocolEvent();
					
					doDM();					// run message through DM
                }
                break;


            case VSCP_STATE_ERROR:          // Everything is *very* *very* bad.
                vscp_error();
                break;

            default:                        // Should not be here...
                vscp_node_state = VSCP_STATE_STARTUP;
                break;

        } // switch 

		
		doWork();

    } // while
}


///////////////////////////////////////////////////////////////////////////////
// init_app_eeprom
// should  only be performed after first startup!

static void init_app_eeprom( void )
{
// none for bootloader
}



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


///////////////////////////////////////////////////////////////////////////////
// vscp_readAppReg
//

uint8_t vscp_readAppReg( uint8_t reg )
{
    uint8_t rv;


    // Zone
    if ( REG_ZONE == reg ) {
        rv =  readEEPROM( REG_ZONE + VSCP_EEPROM_END );
		//rv = 32;
    }

    // SubZone
    else if ( REG_SUBZONE == reg ) {
        rv =  readEEPROM( REG_SUBZONE + VSCP_EEPROM_END );
    }
            

   else {
        rv = 0xff;
    }


    return rv;

}

///////////////////////////////////////////////////////////////////////////////
// vscp_writeAppReg
//

uint8_t vscp_writeAppReg( uint8_t reg, uint8_t val )
{
    uint8_t rv;

    rv = ~val; // error return

    // Zone
    if ( REG_ZONE == reg ) {
        writeEEPROM( REG_ZONE + VSCP_EEPROM_END, val );
        rv =  readEEPROM( REG_ZONE + VSCP_EEPROM_END );
    }

    // SubZone
	else if ( REG_SUBZONE == reg ) {
            writeEEPROM( REG_SUBZONE + VSCP_EEPROM_END, val );
            rv =  readEEPROM( REG_SUBZONE + VSCP_EEPROM_END );
    }	
            
 


    else {
        rv = ~val; // error return	
    }


    return rv;	

}

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
//	vscp_getZone
//
//	Get Zone for device
//	Just return zero if not used.
//

uint8_t vscp_getZone( void )
{
    return readEEPROM( VSCP_EEPROM_REG_ZONE );
}


///////////////////////////////////////////////////////////////////////////////
//	vscp_getSubzone
//
//	Get Subzone for device
// 	Just return zero if not used.
//

uint8_t vscp_getSubzone( void )
{
    return readEEPROM( VSCP_EEPROM_REG_SUBZONE );
}

///////////////////////////////////////////////////////////////////////////////
//	vscp_goBootloaderMode
//
//	Go bootloader mode
//	This routine force the system into bootloader mode according
//	to the selected protocol.
//

void vscp_goBootloaderMode( uint8_t algo )
{
    uart_puts( "\n #####already bootloader!\n" );
}


///////////////////////////////////////////////////////////////////////////////
// vscp_getEmbeddedMdfInfo
//
//	Get embedded MDF info
//	If available this routine sends an embedded MDF file
//	in several events. See specification CLASS1.PROTOCOL
//	Type=35/36
//

void vscp_getEmbeddedMdfInfo( void )
{
    // TODO
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMajorVersion
//
// Get Major version number for this hardware module
//

uint8_t vscp_getMajorVersion()
{
    return FIRMWARE_MAJOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMinorVersion
//
// Get Minor version number for this hardware module
//

uint8_t vscp_getMinorVersion()
{
    return FIRMWARE_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getSubMinorVersion
//
// Get Subminor version number for this hardware module
//

uint8_t vscp_getSubMinorVersion()
{
    return FIRMWARE_SUB_MINOR_VERSION;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getGUID
//
// Get GUID from EEPROM
//

uint8_t vscp_getGUID( uint8_t idx )
{
#ifdef GUID_IN_EEPROM
    return readEEPROM( VSCP_EEPROM_REG_GUID + idx );
#else
    return GUID[ idx ];
#endif
}


///////////////////////////////////////////////////////////////////////////////
// vscp_setGUID
//
// Write GUID if it is in EEPROM
//

void vscp_setGUID( uint8_t idx, uint8_t data )
{
#ifdef GUID_IN_EEPROM
    writeEEPROM( VSCP_EEPROM_REG_GUID + idx, data );	
#endif	

}

///////////////////////////////////////////////////////////////////////////////
// vscp_getMDF_URL
//
// Get device URL from EEPROM
//

uint8_t vscp_getMDF_URL( uint8_t idx )
{
    //return vscp_deviceURL[ idx ];  
	//none for bootloader
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getUserID
//
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getUserID( uint8_t idx )
{
    return readEEPROM( VSCP_EEPROM_REG_USERID + idx );	

}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setUserID
//

void vscp_setUserID( uint8_t idx, uint8_t data )
{
    writeEEPROM( VSCP_EEPROM_REG_USERID + idx, data );
}

///////////////////////////////////////////////////////////////////////////////
// vscp_getManufacturerId
//
// Get Manufacturer id and subid from EEPROM
//

uint8_t vscp_getManufacturerId( uint8_t idx )
{
    return readEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx );	

}

///////////////////////////////////////////////////////////////////////////////
// vscp_setManufacturerId
//
// Get Manufacturer id and subid from EEPROM
//

void vscp_setManufacturerId( uint8_t idx, uint8_t data )
{
    writeEEPROM( VSCP_EEPROM_REG_MANUFACTUR_ID0 + idx, data );	

}

///////////////////////////////////////////////////////////////////////////////
// vscp_getBootLoaderAlgorithm
//
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void )
{
    return VSCP_BOOTLOADER_AVR1; 	

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
// vscp_getRegisterPagesUsed
//
// Return number of register pages this application uses.
//

uint8_t vscp_getRegisterPagesUsed( void )
{
    return RegisterPagesUsed;   // Application uses ? register page
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getNickname
//

uint8_t vscp_getNickname( void )
{
    return readEEPROM( VSCP_EEPROM_NICKNAME );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setNickname
//

void vscp_setNickname( uint8_t nickname )
{
    writeEEPROM( VSCP_EEPROM_NICKNAME, nickname );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_getSegmentCRC
//

uint8_t vscp_getSegmentCRC( void )
{
    return readEEPROM( VSCP_EEPROM_SEGMENT_CRC );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setSegmentCRC
//

void vscp_setSegmentCRC( uint8_t crc )
{
    writeEEPROM( VSCP_EEPROM_SEGMENT_CRC, crc );
}

///////////////////////////////////////////////////////////////////////////////
//  vscp_setControlByte
//

void vscp_setControlByte( uint8_t idx, uint8_t ctrl )
{
	writeEEPROM( VSCP_EEPROM_CONTROL+idx, ctrl );
}


///////////////////////////////////////////////////////////////////////////////
//  vscp_getControlByte
//

uint8_t vscp_getControlByte( uint8_t idx )
{
	return readEEPROM( VSCP_EEPROM_CONTROL + idx );
}


uint32_t vscp_getFamilyCode(void)
{
	return 0;
}

uint32_t vscp_getFamilyType(void)
{
	return 0;
}

void vscp_restoreDefaults(void)
{
	// to do
}

///////////////////////////////////////////////////////////////////////////////
//                       Implemention of Decision Matrix
//////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
// Do decision Matrix handling
//
// The routine expects vscp_imsg to contain a valid incoming event
//

static void doDM( void )
{
	return;
}

///////////////////////////////////////////////////////////////////////////////
// Send Decsion Matrix Information
//

void vscp_getMatrixInfo( char *pData )
{
    pData[ 0 ] = DESCION_MATRIX_ELEMENTS;
    pData[ 1 ] = REG_DM_START;
    pData[ 2 ] = 0;
    pData[ 3 ] = 0;
    pData[ 4 ] = 0;
    pData[ 5 ] = 0;
    pData[ 6 ] = 0;
    uart_puts("DM\n");
}


///////////////////////////////////////////////////////////////////////////////
// SendInformationEvent
//

void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId )
{
    vscp_omsg.priority = VSCP_PRIORITY_MEDIUM;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.vscp_class = eventClass;
    vscp_omsg.vscp_type = eventTypeId;

    vscp_omsg.data[ 0 ] = idx;	// Register
    vscp_omsg.data[ 1 ] = readEEPROM( VSCP_EEPROM_END + REG_ZONE );
    vscp_omsg.data[ 2 ] = readEEPROM( VSCP_EEPROM_END + REG_ZONE + idx );

    vscp_sendEvent();	// Send data
}

void vscp_init_pstorage( void )
{
	/*
	#ifdef PRINT_GENERAL_EVENTS
	uart_puts( "cold start" );
	#endif
	no cold start should be implemented with firmware
	*/
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       end of VSCP Required Methods
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// SendInformationEventExtended same function as SendInformationEvent but 
// with more options to have more control over the events sent
// SendInformationEvent is not altered to remain compatible with common routines

void SendInformationEventExtended(uint8_t priority, uint8_t zone, uint8_t subzone, uint8_t idx, uint8_t eventClass, uint8_t eventTypeId )
{
    vscp_omsg.priority = priority;
    vscp_omsg.flags = VSCP_VALID_MSG + 3;
    vscp_omsg.vscp_class = eventClass;
    vscp_omsg.vscp_type = eventTypeId;

    vscp_omsg.data[ 0 ] = idx;	// Register
    vscp_omsg.data[ 1 ] = zone;
    vscp_omsg.data[ 2 ] = subzone;

    vscp_sendEvent();	// Send data
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

        for (i=0; i<BTL_BLOCKSIZE; i+=2)
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




