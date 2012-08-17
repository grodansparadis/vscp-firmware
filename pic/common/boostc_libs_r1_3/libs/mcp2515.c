/*************************************************************
* SPI interface for the MCP2515 CAN controller from Microchip
*
* Depends on the SPI lib.
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#include "mcp2515.h"
#include "spi.h"
#include "serial.h"

// Local variables
char  can_rx_buffer[8];
//char  can_tx_buffer[8]; -> is located in the MCP2515
char  can_rx_pointer = 0;
char  can_tx_pointer = 0;
short can_msg_id = 0;
char  can_rx_count = 0;

// Flags for internal housekeeping
bit rx_message_pending = 0;
bit tx_message_pending = 0;

////////////////////////////////////////////////////////////
// Software reset in case the reset pin is not available
////////////////////////////////////////////////////////////
void can_reset(){

	// Reset the SPI buffer
	spi_init_buffer();
	// Load and TX command
	spi_load_byte(c2515Reset);	
	spi_exchange();
	spi_wait_for_completion();
	return;
}

////////////////////////////////////////////////////////////
// Write <data> in <address> using <mask>
////////////////////////////////////////////////////////////
void can_write_bits(char address, char data, char mask){

	spi_init_buffer();
	spi_load_byte(c2515BitMod);
	spi_load_byte(address);
	spi_load_byte(mask);
	spi_load_byte(data);
	spi_exchange();
	spi_wait_for_completion();

}

////////////////////////////////////////////////////////////
// Write byte <data> to register with address <reg>
////////////////////////////////////////////////////////////
void can_write_reg(char address, char data){

	spi_init_buffer();
	spi_load_byte(c2515Write);
	spi_load_byte(address);
	spi_load_byte(data);
	spi_exchange();
	spi_wait_for_completion();

}

////////////////////////////////////////////////////////////
// Write command to the MCP2515
////////////////////////////////////////////////////////////
void can_write_cmd(char cmd){

	spi_init_buffer();
	spi_load_byte(cmd);
	spi_exchange();
	spi_wait_for_completion();

}

////////////////////////////////////////////////////////////
// Read the contents of the register at <address>
////////////////////////////////////////////////////////////
char can_read_reg(char address){

	spi_init_buffer();
	// Read command
	spi_load_byte(c2515Read);
	// Register address
	spi_load_byte(address);
	// Expect one byte answer
	spi_load_zeros(0x01);
	// Start the transfer 
	spi_exchange();
	spi_wait_for_completion();
	
	return spi_get_byte(2);
	
}

////////////////////////////////////////////////////////////
// Set the controller in loopback mode
////////////////////////////////////////////////////////////
void can_set_loopback_mode(){

	can_write_bits(CANCTRL, 0x40, 0xE0);
	while ((can_read_reg(CANSTAT) & 0xE0 != 0x40) != 0);
	
	return;
}

////////////////////////////////////////////////////////////
// Set the controller in normal mode
////////////////////////////////////////////////////////////
void can_set_normal_mode(){

	can_write_bits(CANCTRL, 0x00, 0xE0);
	while ((can_read_reg(CANSTAT) & 0xE0) != 0);
	
	return;
}

////////////////////////////////////////////////////////////
// Set the controller in config mode
// This mode is required of you need to change the following
// registers:
//  CNF1, CNF2, CNF3
//  TXRTSCTRL
//  filters and masks
////////////////////////////////////////////////////////////
void can_set_config_mode(){

	can_write_bits(CANCTRL, 0x40, 0xE0);
	while ((can_read_reg(CANSTAT) & 0xE0 != 0x40) != 0);
	
	return;
}


////////////////////////////////////////////////////////////
// can_peek_messages
//
// Checks for message in Receive Buf 1.  If there is already
// a message pending, return with a value 1.
//         
// If message pending:
//     Load 'can_msg_id' with ID.
//     Load 'can_rx_count' with number of bytes of data received.
//     Load 'can_rx_buffer' with data
//     Clear 2510 Receive Buffer 1 interrupt flag
//     Set rx_message_pending flag
//
// If message is already pending, the function doesn't 
// check for new messages since it would overwrite the
// current data.
////////////////////////////////////////////////////////////
char can_peek_message(){
	
	// Check if there are already messages pending
	if (rx_message_pending){
		return 1;
	}
	
	// Check for pending messages in RX buffer 0
	if ((can_read_reg(CANINTF) & 0x01) == 0){
		return 0;
	}

	// If we're here, there is something waiting
	rx_message_pending = 1;
	can_rx_pointer     = 0;
	
	// Strip the ID of the message source
	can_msg_id = (can_read_reg(RXB0SIDH) << 8);
	can_msg_id |= (can_read_reg(RXB0SIDL) & 0xE0);
	
	// Get number of databytes
	can_rx_count = (can_read_reg(RXB0DLC) & 0x0F);
	
	// Read the data from the MCP2515 rx buffer
	char lcount = 0;
	char data   = 0;
	
	while (lcount < can_rx_count){
		// Get the data byte and store in the local buffer
		can_rx_buffer[lcount] = can_read_reg(RXB0D0 + lcount);	
		lcount++;	
	}
	
	// Clear RX buffer 0 interrupt signal
	can_write_bits(CANINTF, 0x00, 0x01);
	
	return 1;
}

////////////////////////////////////////////////////////////
// Get the next byte from the RX data buffer
////////////////////////////////////////////////////////////
char can_rx_byte(){

	if (!rx_message_pending){
		// Don't call this function when there is nothing received
		return 0xEE; // <<- error code
	}
	
	if (can_rx_pointer == can_rx_count){
		// We are returning the last byte, so reset the data-in-buffer flag
		rx_message_pending = 0;
	} 
	
	char retval = can_rx_buffer[can_rx_pointer];
	can_rx_pointer++;
	
	return retval; 
}

////////////////////////////////////////////////////////////
// MCP2515 init routine
////////////////////////////////////////////////////////////
void can_init(short id){

	delay_ms(25);
	// We don't do a hardware reset, since we use the CLKOUT to clock the PIC
	//mcp2515_rst_tris = 0;
	//mcp2515_rst = 0;
	//mcp2515_rst = 1;
	// Do software reset instead
	can_reset();
	
	// We're in config mode now (auto after reset)
	
	rx_message_pending = 0;
	tx_message_pending = 0;
	

	// Set physical layer configuration 
	//
	//     Fosc = 16MHz
	//     BRP        =   7  (divide by 8)
	//     Sync Seg   = 1TQ
	//     Prop Seg   = 1TQ
	//     Phase Seg1 = 3TQ
	//     Phase Seg2 = 3TQ
	//
	//    TQ = 2 * (1/Fosc) * (BRP+1) 
	//     Bus speed = 1/(Total # of TQ) * TQ	
	
	// set BRP to div by 8
    can_write_reg(CNF1, 0x07);      

    can_write_reg(CNF2, 0x90);

	can_write_reg(CNF3, 0x02);
	
	
    // Configure Receive buffer 0 Mask and Filters 
    // Receive buffer 0 will not be used
    can_write_reg(RXM0SIDH, 0xFF);
    can_write_reg(RXM0SIDL, 0xFF);
    
	can_write_reg(RXF0SIDH, 0xFF);
	can_write_reg(RXF0SIDL, 0xFF);

	// Configure Receive Buffer 1 Mask and Filters 
    can_write_reg(RXF1SIDH, 0xFF);
    can_write_reg(RXF1SIDL, 0xFF);

    can_write_reg(RXM1SIDH, 0x00); // was FF 
    can_write_reg(RXM1SIDL, 0x00);

    // Initialize Filter 2 to match x0 bBaseRecID 
	can_write_reg(RXF2SIDH, CAN_RX_ID);
	can_write_reg(RXF2SIDL, 0x00); //; Make sure EXIDE bit (bit 3) is set correctly in filter
	
	// Initialize Filter 3 to match x1
    can_write_reg(RXF3SIDH, CAN_RX_ID + 1);
    can_write_reg(RXF3SIDL, 0x00);

	// Initialize Filter 4 to match x2 
	can_write_reg(RXF4SIDH, CAN_RX_ID + 2);
    can_write_reg(RXF4SIDL, 0x00);
    
    // Initialize Filter 5 to match x3 
	can_write_reg(RXF5SIDH, CAN_RX_ID + 3);
    can_write_reg(RXF5SIDL, 0x00);
    

    // Disable all MCP2510 Interrupts
    can_write_reg(CANINTE, 0x00);
    
    // Set the TX id
    can_set_id(id);
    
    // Set normal mode (not loopback)
	//can_set_loopback_mode();
	can_set_normal_mode();
	
	// Disable filter and mask on buffer 0
	can_write_bits(RXB0CTRL, 0x60, 0x60);

	
}

void can_init_buffer(){
	can_tx_pointer = 0;
	return;
}

void can_load_byte(char input){
	can_write_reg(TXB0D0+can_tx_pointer, input);
	can_tx_pointer++;
}

void can_tx_buffer(){
	// Wait for pending messages to be sent
	while(can_read_reg(TXB0CTRL)& 0x08){
		nop();
	}
	
	// Load the number of bytes to transmit
	can_write_bits(TXB0DLC, can_tx_pointer, 0x0F);
	
	// Request transmission
	can_write_cmd(0x81);
	
	return;
	
}

void can_set_id(short id){
	char idh = (id & 0x03FF) >> 3;
	char idl = (id & 0x0007);
	/*
	serial_printf("IDH=");
	serial_print_hex(idh);
	serial_printf(" -- IDL=");
	serial_print_hex(idl);
	serial_print_lf();
	*/
	can_write_reg(TXB0SIDH, idh);
	can_write_bits(TXB0SIDL, idl << 5, 0xE0);
	
}

void can_read_status(){
	// Reset the SPI buffer
	spi_init_buffer();
	// Load and TX command
	spi_load_byte(c2515Status);	
	spi_load_byte(0x00);
	spi_exchange();
	spi_wait_for_completion();
	
	serial_printf("Status = ");
	serial_print_hex(spi_get_byte(1));
	
	return;
}
