/*************************************************************
* Blocking serial interface code for the hardware UART
* of PIC16-range microcontrollers from Microchip
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
* Based on code from Yann Hendrikx.
*************************************************************/

#include "serial.h"

char buffer[SERIAL_BUFSIZE];
char buffer_pointer;

////////////////////////////////////////////////////////////
// Hardware initialisation
// input argument: the baudrate scaler
////////////////////////////////////////////////////////////
void serial_init(char brg)
{
	// Enable USART function on PORTC (see PIC18F4620 datasheet page 201)
	trisc.7       = 1;			
	trisc.6       = 1; 		
	
	// Transmit control register
	txsta.TX9     = 0;
	txsta.TXEN    = 1;
	txsta.SYNC    = 0;
	txsta.BRGH    = 1;
	
	// Receive control register
	rcsta.SPEN    = 1;
	rcsta.RX9     = 0;
	rcsta.CREN    = 1;
	rcsta.ADDEN   = 0;
	
	// Baudcon register, if supported by the target architecture
#ifdef baudcon
	baudcon.BRG16 = 0;
	baudcon.WUE   = 0;
	baudcon.ABDEN = 0;
#endif
	// Baud rate setting
    spbrg 	   = brg;	
 }

////////////////////////////////////////////////////////////
// Print a single character to the serial port
// This funtion is skipped in debug mode since it will 
// block the debugger (the function waits for a hardware
// signal)
////////////////////////////////////////////////////////////
void serial_printf(char value)
{

#ifdef DBG_COMPILE
	// Skip when debugging
#warning "Debug build, serial_printf will return immediately"
	return;
#endif

    while((txsta & 1 << TRMT) == 0); 
    txreg = value;
}

////////////////////////////////////////////////////////////
// Prints a line to the serial port. Ends with a linefeed.
// The function parameter is stored in rom.
////////////////////////////////////////////////////////////
void serial_printf(const char* text)
{
    char i = 0;
    while(text[i] != 0)
        serial_printf(text[i++]);
        
}

////////////////////////////////////////////////////////////
// Print a decimal value to the serial port
////////////////////////////////////////////////////////////
void serial_print_dec(char number)
{
    if(number > 99) {
        serial_printf(((number / 100) % 10) + '0');
    }
    if(number > 9) {
        serial_printf(((number / 10) % 10) + '0');
    }
    serial_printf((number % 10) + '0');
}

////////////////////////////////////////////////////////////
// Print a decimal value to the serial port
// The number takes multiple characters. If a character 
// position is not used, it is filled with 0.
// Positions should be 1, 2 or 3.
////////////////////////////////////////////////////////////
void serial_print_dec(char number, char positions)
{

    if(number > 99) {
        serial_printf(((number / 100) % 10) + '0');
    } else {
		if (positions > 2){
			serial_printf('0');
		}
	}
    if(number > 9) {
        serial_printf(((number / 10) % 10) + '0');
    } else {
		if (positions > 1){
			serial_printf('0');
		}
	}
	serial_printf((number % 10) + '0');
}

////////////////////////////////////////////////////////////
// Print a decimal value to the serial port
////////////////////////////////////////////////////////////
void serial_print_dec(short number)
{
	if(number > 9999) {
        serial_printf(((number / 10000) % 10) + '0');
    }
	if(number > 999) {
        serial_printf(((number / 1000) % 10) + '0');
    }
    if(number > 99) {
        serial_printf(((number / 100) % 10) + '0');
    }
    if(number > 9) {
        serial_printf(((number / 10) % 10) + '0');
    }
    serial_printf((number % 10) + '0');
}

////////////////////////////////////////////////////////////
// Print a hex value to the serial port
////////////////////////////////////////////////////////////
void serial_print_hex(char number)
{
    char hexChar;
    char i;
    for(i = 0; i < 2; i++)
    {
        if(i == 0)
            hexChar = number >> 4;
        else
            hexChar = number & 0x0F;
        if(hexChar < 10)
            hexChar = hexChar + '0';
        else
            hexChar = hexChar + ('A' - 10);
        serial_printf(hexChar);
    }
}

////////////////////////////////////////////////////////////
// Print a 16-bit hex value to the serial port
////////////////////////////////////////////////////////////
void serial_print_hex(short value){
	
    char value1 = (char)((value >> 8) & 0x00FF);
    
	serial_print_hex(value1);
 
    char value0 = (char)(value & 0x00FF);
    
	serial_print_hex(value0);
	
	return;
    
}
////////////////////////////////////////////////////////////
// Print a binary value to the serial port
////////////////////////////////////////////////////////////
void serial_print_bin(char number)
{
    char i;
    for(i = 0; i < 8; i++)
    {
		if (number & 10000000b){
			serial_printf('1');
		} else {
		    serial_printf('0');
		}
        number <<= 1;
    }
}

////////////////////////////////////////////////////////////
// Wait for a character to read it from the serial port
// This function is skipped when running in debug mode
// since else it blocks the simulator (waits for a 
// hardware signal from the UART)
////////////////////////////////////////////////////////////
char serial_getch(){

#ifdef DBG_COMPILE
#warning "Debug build, serial_getch will return 0x00"
	return 0x00;
#endif

	// Check for overrun error. Reset the 
	// UART if any.
	if (rcsta.OERR){
		serial_printf("OERR");
		rcsta.CREN = 0;
		rcsta.CREN = 1;
	}
	
	while (!serial_peek());
	return rcreg;
}

// Read a decimal value from the serial port
char serial_get_decimal(){
	char value = 0;
	char input = 0;

	input = serial_getch();
	
	while (input != 0x0D){ 
		serial_printf(input);
		value *= 10;
		value += (input-0x30);
		input = serial_getch();
	}
	serial_print_lf();
	return value;
}

void serial_safe_init(){
	buffer_pointer = 0;
}

char serial_safe_putbyte(char data){
	buffer[buffer_pointer] = data;
	buffer_pointer++;
	
	if (buffer_pointer == SERIAL_BUFSIZE){
		return 1;
	}
	
	return 0;
}

char serial_safe_send(){
	char loper;
	char cur;
	char checksum;
	
	// Safety check
	if (buffer_pointer == 0){
		return 1;
	}
	
	checksum = 0;
	
	// Send header
	serial_printf(SERIAL_PRE);
	serial_printf(SERIAL_PRE);
	
	// Transmit buffer
	for (loper = 0; loper < buffer_pointer; loper++){
		// Byte to send
		cur = buffer[loper];
		
		// Check if we need to send the ESCAPE byte
		if ((cur == SERIAL_PRE) || (cur == SERIAL_EOP) || (cur == SERIAL_ESC)){
			serial_printf(SERIAL_ESC);
		}
	
		// Send data
		serial_printf(cur);
		
		// Update checksum
		checksum += cur;
	}
	
	// Send checksum and EOP
	if ((checksum == SERIAL_PRE) || (checksum == SERIAL_EOP) || (checksum == SERIAL_ESC)){
		serial_printf(SERIAL_ESC);
	}
	
	serial_printf(~checksum + 1);
	serial_printf(SERIAL_EOP);
	
	buffer_pointer = 0;
	return 0;
}

signed char serial_safe_receive(){

	buffer_pointer = 0;
	char cur;
	char checksum  = 0;
	char loper     = 0;
	bit  got_escape;
	
	// Expecting a packet that has the structure
	// <PRE><PRE>[<DATA1><DATA2> ... ]<CRC><EOP>
	
	while (loper < 2){
		cur = serial_getch();
		if (cur == SERIAL_PRE){
			loper ++;
		} else {
			loper = 0;
		}
	}
	
	//serial_printf("Passed header...");
	//serial_print_lf();
	
	// Receive data
	do {
		got_escape = 0;
		
		cur = serial_getch();
		if (cur == SERIAL_ESC){
			cur = serial_getch();
			got_escape = 1;
		}
		
		buffer[buffer_pointer] = cur;
		buffer_pointer++;
		if (buffer_pointer == SERIAL_BUFSIZE){
			return -1;
		}
	} while (cur != SERIAL_EOP || got_escape);
	
	//serial_printf("received: ");
	//for (loper = 0; loper < buffer_pointer; loper++){
	//	serial_print_hex(buffer[loper]);
	//}
	
	// TODO: implement checksum
	
	// Correct the pointer so we can reuse a part of the packet for the response
	buffer_pointer -= 2;
	return buffer_pointer;
}

char serial_safe_getbyte(char index){
	return buffer[index];
}
