/*************************************************************
* One wire interface lib for the Dallas DS1820 temperature 
* sensor
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
* last update: 2000625
*************************************************************/

#include "oo.h"
#include "serial.h"
#include "eeprom.h"

char oo_scratchpad[9];

// Scratchpad contents
// 0	Temperature LSB
// 1	Temperature MSB
// 2	Hi alarm temperature
// 3	Lo alarm temperature
// 4	Reserved, 0xFF
// 5	Reserved, 0xFF
// 6	Remainder register
// 7	Nr of counts per degree
// 8	CRC of pad contents

#ifdef OO_CRC_CHECKING

// CRC working variable
char crc = 0;

// CRC lookup table
rom char* crc_rom = {0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
	157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
	35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
	190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
	70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
	219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
	101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
	248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
	140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
	17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
	175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
	50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
	202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
	87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
	233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
	116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53};

// CRC function prototypes
void oo_crc_init();
void oo_crc_shuffle_byte(char input);
#endif

// Internal variables
char        id[8];
signed char conflict;
bit         new_conflict;



////////////////////////////////////////////////////////////
// oo_busreset()
// 
// Issues a 'ping' on the bus. If there is at least one
// sensor on the bus, it will generate a presence pulse.
// Returns: 0 if a presence pulse was detected
//          1 if no device was detected
////////////////////////////////////////////////////////////
char oo_busreset(){
	
	oo_bus = 0;			// Send the reset pulse
	oo_writemode;		// therefore, make port low
	
	delay_10us(60);		// Wait for 600 µs, this is the reset pulse
	
	oo_readmode;		// Release bus
	
	delay_10us(10);		// Wait 60 us (release) + 40 us (let device drive the bus)
	
	// Now sample, if there is a sensor on the bus, the line should be low
	if (oo_bus){
		return 1; // <<-error
	}
	
	delay_10us(40);		// Wait another 400 us to complete the reset sequence
	
	return 0;
	
}

////////////////////////////////////////////////////////////
// oo_tx_bit(bit value)
// 
// Transmits a single bit over the bus
////////////////////////////////////////////////////////////
void oo_tx_bit(bit value){

	// Pull bus low
	oo_bus = 0;
	oo_writemode;
	
	// Start the write slot
#warning "Assuming clock frequency of 8 MHz, adjust nr. of nop() to obtain a delay of 1us".	
	nop();
	nop();
	
	if (value == 0){
		// Write a '0' (hold the bus for another 60 us)
		delay_10us(6);
	}

	// Release the bus
	oo_readmode;
	
	// Final wait for the bus to stabilise
	delay_10us(1);
	
	// Make sure a write 0 and a write 1 take the same time to complete
	if (value == 1){
		delay_10us(6);
	}
	
	return;
}

////////////////////////////////////////////////////////////
// char oo_rx_bit()
// 
// Receives a single bit from the bus
////////////////////////////////////////////////////////////
char oo_rx_bit(){

	char value = 0;
	
	// Bus low
	oo_bus = 0;
	oo_writemode;
	
	// Wait 1 us
#warning "Assuming clock frequency of 8 MHz, adjust nr. of nop() to obtain a delay of 1us".
	nop();
	nop();
	
	// Release bus
	oo_readmode;
	
	// Wait 10 us before sampling
	delay_10us(1);

	value = oo_bus;
	
	// Wait for the end of the read time slot
	delay_10us(5);
	
	return value;
}

////////////////////////////////////////////////////////////
// oo_tx_byte(char data)
// 
// Transmits a single byte from the bus
////////////////////////////////////////////////////////////
void oo_tx_byte(char data){
	char counter = 0;
	while (counter < 8){
		if (data & 0x01){
			oo_tx_bit(1);
		} else {
			oo_tx_bit(0);
		}
		
		data = data >> 1;
		counter++;
	}
}

////////////////////////////////////////////////////////////
// char oo_rx_byte()
// 
// Receives a single byte from the bus
////////////////////////////////////////////////////////////
char oo_rx_byte(){

	char counter = 0;
	char data    = 0;
	bit  oo_bit  = 0;
	
	while (counter < 8){
	
		// Rx bit
		oo_bit = oo_rx_bit();
		
		// Shift and add one depending on the value of the bus
		data = data >> 1;
		
		if (oo_bit){
			data |= 0x80;
		}
		
		counter++;
		
	}
	
#ifdef OO_CRC_CHECKING
	oo_crc_shuffle_byte(data);
#endif	
	return data;
}


////////////////////////////////////////////////////////////
// char oo_read_scratchpad()
// 
// Reads the scratchpad of the currently selected device
////////////////////////////////////////////////////////////
char oo_read_scratchpad(){
	
	// Read the scratchpad
	oo_tx_byte(OO_READPAD);
	
	char counter = 0;
	
#ifdef OO_CRC_CHECKING
	// Reset the CRC register, CRC is updated in the oo_rx_byte() function.
	oo_crc_init();
#endif
	
	while (counter < 9){
		oo_scratchpad[counter] = oo_rx_byte();
		counter++;
	}

#ifdef OO_CRC_CHECKING
	// Verify the CRC
	return crc;
#endif

	return 0;
}

////////////////////////////////////////////////////////////
// short oo_get_temp()
// 
// DS1820 specific
// Returns the 2-byte value of the temperature register
////////////////////////////////////////////////////////////
short oo_get_temp(){
	short retval;
	retval = (short)oo_scratchpad[1];
	retval = retval << 8;
	retval += (short)oo_scratchpad[0];
	return retval; 
}

////////////////////////////////////////////////////////////
// short oo_get_count()
// 
// DS1820 specific
// Returns the 2-byte value of the count register
////////////////////////////////////////////////////////////
short oo_get_count(){
	short retval;
	retval = (short)oo_scratchpad[7];
	retval = retval << 8;
	retval += (short)oo_scratchpad[6];
	return retval; 
}

////////////////////////////////////////////////////////////
// oo_conversion_busy()
// 
// DS1820 specific
// Returns 1 if a sensor is still processing
//         0 if all sensors are done
////////////////////////////////////////////////////////////
char oo_conversion_busy(){
	if (oo_rx_byte() == 0xFF){
		return 1;
	} else {
		return 0;
	}
}

////////////////////////////////////////////////////////////
// oo_start_conversion()
// 
// DS1820 specific
// Commands all sensors to start a temperature conversion
////////////////////////////////////////////////////////////
void oo_start_conversion(){
	// Command all temp sensors on the bus to start a conversion
	oo_tx_byte(OO_SKIPROM);
	
	// Convert temperature
	oo_tx_byte(OO_CONVERTT);
	
	return;
}

////////////////////////////////////////////////////////////
// char oo_wait_for_completion()
// 
// DS1820 specific
// Wait for the completion of the temperature conversion
// returns 0 is success, returns 1 when a timeout occurred.
////////////////////////////////////////////////////////////
char oo_wait_for_completion(){

	char counter = 0;
	
	while (oo_conversion_busy()){
		// Security: if the conversion is not completed
		// after > 1 sec -> break.
		delay_ms(4);
		counter++;
		if (counter == 0xFF){
			return 1;
		}	
	}
	
	return 0;
}

#ifdef OO_CRC_CHECKING
////////////////////////////////////////////////////////////
// oo_crc_init()
//
// Initialise the CRC working register to be able to 
// start a new calculation
////////////////////////////////////////////////////////////
void oo_crc_init(){
	crc = 0;
}

////////////////////////////////////////////////////////////
// oo_crc_shuffle_byte()
//
// Shuffle the next byte into the CRC
////////////////////////////////////////////////////////////
void oo_crc_shuffle_byte(char input){
	crc = crc_rom[crc ^ input];
}
#endif

////////////////////////////////////////////////////////////
// oo_get_next_id()
//
// Helper function for the scanbus option. Gets the next 
// id on the bus.
////////////////////////////////////////////////////////////
char oo_get_next_id(){

	bit  val0;
	bit  val1;
	char counter    = 0;
	char bit_index  = 0;
	char byte_index = 0;
	char current_conflict = -1;

	new_conflict = 0;
	
	// Reset the bus
	if (oo_busreset()){
		return 1; // -> error
	}
	
	// Issue the search ROM command
	oo_tx_byte(OO_SEARCHROM);
	
	// Start the search for the sensor	
	for (byte_index = 0; byte_index < 8; byte_index++){
		
		char data_in_byte = 0;
	
		for (bit_index = 0; bit_index < 8; bit_index++){
	
			// Read 2 consecutive bits from the One Wire bus
			val0 = oo_rx_bit();
			val1 = oo_rx_bit();

			// Evaluate the result
			if ((val0 == 0) && (val1 == 0)){
				// meepmeep, conflict
				//serial_printf("C");
				current_conflict = counter;
			
				// if conflict occurs earlier than the previous conflict, then use the previous value
				if (current_conflict < conflict) {
					// use previous value
					if (id[byte_index] && (1 << bit_index)){
						val0 = 1;
					} else {
						val0 = 0;
					}
				} else if (current_conflict == conflict) {
					// use 1
					val0 = 1; 
	} else {
					// use zero on new conflicts
					val0 = 0;
					conflict = current_conflict;
					new_conflict = 1;
				}
				
			} else if (val0 && val1){
				//serial_printf("H");
				return 1;
			}
			
			// Store the bit in the ID array
			data_in_byte = data_in_byte >> 1;
			if (val0){
				data_in_byte |= 0x80;
			}
			oo_tx_bit(val0);
			//serial_print_dec(val0);
		
			counter++;
			
		}

		id[byte_index] = data_in_byte;

	}
	
	//serial_print_lf();
	
	// Verify ID CRC
#ifdef OO_CRC_CHECKING
	oo_crc_init();
#endif
	
	//serial_printf("Found ID: ");
	for (byte_index = 0; byte_index < 8; byte_index++){
		//serial_print_hex(id[byte_index]);
#ifdef OO_CRC_CHECKING
		oo_crc_shuffle_byte(id[byte_index]); 
#endif
	}
	
	//serial_print_lf();
	
	// Bail on CRC errors
#ifdef OO_CRC_CHECKING
	if (crc) return crc;
#endif
	
	return 0;
}

////////////////////////////////////////////////////////////
// oo_scanbus()
//
// Scan the bus to detect all devices
// This function stores the following information in the 
// EEPROM:
// @ OO_EEPROM_NR_IDS  => the number of IDs detected
// @ OO_EEPROM_ID_BASE => base address of the IDs, one ID
//                        takes 8 bytes
////////////////////////////////////////////////////////////
char oo_scanbus(void){

	char retval;
	char device_count = 0;
	char loper;

	conflict = -1;
	new_conflict = 1;
	
	// Reset the bus, wake up devices.
	if (oo_busreset()){
		return 1; // -> error
	}
	
	// new_conflict gets updated in the oo_get_next_id() function.
	while (new_conflict){
		retval = oo_get_next_id(); 

		// Bail in case of errors
		if (retval) return retval;
		
		// Add the device id to EEPROM
		for (loper=0; loper < 8; loper++){
			eeprom_write(OO_EEPROM_ID_BASE+loper+(device_count<<3), id[loper]);
		}
		
		device_count++;
		
	}
	
	// Write number of devices to the EEPROM
	eeprom_write(OO_EEPROM_NR_IDS, device_count);
	
	return 0;
	
}

////////////////////////////////////////////////////////////
// char oo_get_devicecount()
//
// Returns the number of detected devices
////////////////////////////////////////////////////////////
char oo_get_devicecount(){
	return eeprom_read(OO_EEPROM_NR_IDS);
}

////////////////////////////////////////////////////////////
// char oo_read_device()
//
// Reads the temperature of the device @ location 'count'
// in the EEPROM
////////////////////////////////////////////////////////////
oo_tdata oo_read_device(char count){

	char loper;
	
	oo_tdata data;
	
	data.valid = 0;
	
	oo_busreset();
	oo_tx_byte(OO_MATCHROM);
	//serial_printf("Going for matchrom\n");
	
	for (loper=0; loper < 8; loper++){
		id[loper] = eeprom_read(OO_EEPROM_ID_BASE+loper+(count<<3));
		oo_tx_byte(id[loper]);
		data.id[loper] = id[loper];
		//serial_print_hex(id[loper]);
	}
	//serial_print_lf();
	
	// Read the scratchpad
	if (oo_read_scratchpad()){
		return data;
	}
	
	// And extract the temperature information
	data.t_msb    = oo_scratchpad[1];
	data.t_lsb    = oo_scratchpad[0];
	data.remain   = oo_scratchpad[6];
	data.nr_count = oo_scratchpad[7];
	data.valid    = 1;
	return data;
	
}

char  oo_get_pad_byte(char index){
	return oo_scratchpad[index];
}

//char  oo_get_id_byte(char index){
//	return oo_current_id[index];
//}
