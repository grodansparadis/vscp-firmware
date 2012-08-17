/*************************************************************
* Nokia 3310 graphical LCD interface
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#include "nokialcd.h"
#include "charset.h"
#include "test.h"
#include "logo.h"

char nlcd_scroll_data[32];
char nlcd_scroll_length;
char nlcd_scroll_line;
char nlcd_scroll_offset;

////////////////////////////////////////////////////////////
// Initialisation sequence of the LCD
////////////////////////////////////////////////////////////
void nlcd_init(){

	// Make all pins output, since we only write to the display
	nlcd_tris_sclk	= 0;
	nlcd_tris_sdin	= 0;
	nlcd_tris_dc	= 0;
	nlcd_tris_sce	= 0;
	nlcd_tris_rst	= 0;

	// All pins are inactive before reset
	nlcd_sclk 		= 0;
	nlcd_sdin		= 0;
	nlcd_dc			= 0;
	nlcd_sce		= 1;
	nlcd_rst		= 1;
	 
	// Wait for internal reset
	delay_ms(1);
	
	nlcd_rst 		= 0;

	delay_ms(1);
	
	nlcd_rst		= 1;
	
	delay_ms(1);
	
	// Init sequence
	nlcd_sce 		= 0;
	
    nlcd_write_byte(NLCD_EXTENDED_CMDS,0);	// Extended commands
    nlcd_write_byte(0xc5,0);	// LCD Vop
    nlcd_write_byte(0x06,0);    // Temp coef
    nlcd_write_byte(0x13,0);    // LCD Bias 1:48
    nlcd_write_byte(NLCD_NORMAL_CMDS,0);    // Standard commands
    //nlcd_write_byte(0x09,0);    // all on
    //delay_ms(50);
    //nlcd_write_byte(0x08,0); 	// all off
    nlcd_write_byte(NLCD_NORMAL_MODE,0);    // Normal Mode

    // Clear and position at top of LCD
    nlcd_clear();
    
    nlcd_goto(0,0);    	
	return;

}

////////////////////////////////////////////////////////////
// Print a hex value to the LCD
////////////////////////////////////////////////////////////
void nlcd_print_hex(char value){
	
	char hexChar;
    char i;
    
    for(i = 0; i < 2; i++)
    {
        if(i == 0)
            hexChar = value >> 4;
        else
            hexChar = value & 0x0F;
        if(hexChar < 10)
            hexChar = hexChar + '0';
        else
            hexChar = hexChar + ('A' - 10);
        nlcd_print_char(hexChar);
    }
}

////////////////////////////////////////////////////////////
// Print a 16-bit hex value to the LCD
////////////////////////////////////////////////////////////
void nlcd_print_hex(short value){

    char value1 = (char)((value >> 8) & 0x00FF);
    
	nlcd_print_hex(value1);
 
    char value0 = (char)(value & 0x00FF);
    
	nlcd_print_hex(value0);
	
	return;
}

////////////////////////////////////////////////////////////
// Write a byte to the LCD
// parameters: data -> byte to write
//             mode -> NLCD_DATA or NLCD_COMMAND
////////////////////////////////////////////////////////////
void nlcd_write_byte(char data, bit mode){

	char i;
	
	// Mode selection
	nlcd_dc = mode;

	// Chip select
	nlcd_sce = 0;
	
	// Clock byte, MSB first
	for (i = 0; i<8; i++){
		nlcd_sclk = 0;	// clk low
		if (data & 0x80){
			nlcd_sdin = 1;
		} else {
			nlcd_sdin = 0;
		}
		data = data << 1;
		nlcd_sclk = 1;

	}
	
	// Chip select off
	nlcd_sce = 1;
}

////////////////////////////////////////////////////////////
// Clear the LCD display area
////////////////////////////////////////////////////////////
void nlcd_clear(){

	short i;
	
	// Select data mode, 0 input, chip enable
	nlcd_sce  = 0;
	
	// Clock the entire data area full zeros (84 x 48 pixels = 4032 pixels)
	for (i=0; i<504; i++){
		nlcd_write_byte(0x00, 1);
	}
	
	nlcd_sce = 1;
	
	return;
}

////////////////////////////////////////////////////////////
// Print a character from the included character set.
////////////////////////////////////////////////////////////
void nlcd_print_char(char data){
	// Loop through the entire character
	char i;
	char index;
	char offset = 5;
	enum t_tables {TABLE1, TABLE2, OTHER};
	enum t_tables table;
	
	
	if (data >=32 && data <= 80) {
		table = TABLE1;
		index = data - 32;
	} else if (data >= 81 && data <= 126) {
		table = TABLE2;
		index = data - 81;
	} else {
		table = OTHER;
		index = data - 127;
		offset = 12;
	}
	
	index = index * offset;
	
	for (i=0; i<offset; i++){
		switch (table) {
			case TABLE1:
				nlcd_write_byte(charset_table1[index + i], 1);
				break;
			case TABLE2:
				nlcd_write_byte(charset_table2[index + i], 1);
				break;
			case OTHER:
				nlcd_write_byte(logo[index+i], 1);
				break;
		}
	}
	
	if (table != OTHER){
		nlcd_write_byte(0x00,1);
	}
	
	return;
}

////////////////////////////////////////////////////////////
// Send a string to the LCD
////////////////////////////////////////////////////////////
void nlcd_printf(const char* line){
    char i = 0;
    while(line[i] != 0)
        nlcd_print_char(line[i++]);
}

////////////////////////////////////////////////////////////
// Move the cursor to a certain display location
// -> 0 < x < 83
// -> 0 < y < 5 
////////////////////////////////////////////////////////////
void nlcd_goto(char x, char y){
	nlcd_write_byte(0x40 | (y & 0x07), NLCD_CMD);
	nlcd_write_byte(0x80 | (x & 0x7f), NLCD_CMD); 
}

////////////////////////////////////////////////////////////
// Initialise the content of a scroll line
// Select the line number that will be scrolled, and pass
// the string that will be scrolled.
////////////////////////////////////////////////////////////
void nlcd_init_scroll_line(char line_nr, const char* data){
	nlcd_scroll_length = 0;
	nlcd_scroll_line   = line_nr;

	while(data[nlcd_scroll_length] != 0){
        nlcd_scroll_data[nlcd_scroll_length] = data[nlcd_scroll_length];
		nlcd_scroll_length++;
    }
    
    nlcd_scroll_offset = 0;
    
    nlcd_display_scroll_line();
}

////////////////////////////////////////////////////////////
// Display the scroll line on the LCD
////////////////////////////////////////////////////////////
void nlcd_display_scroll_line(){
	char i;
	
    // Display first part of the scroll line on the screen
    nlcd_goto(0, nlcd_scroll_line);
    
    for (i=0; i<14; i++){
		nlcd_print_char(nlcd_scroll_data[i+nlcd_scroll_offset]);
    } 

}

////////////////////////////////////////////////////////////
// Scroll line to the left
// Stops scrolling when there is no more data on the right
// to display.
////////////////////////////////////////////////////////////
char nlcd_scroll_left(){
	if (nlcd_scroll_offset + NLCD_SCREEN_WIDTH >= nlcd_scroll_length ){
		return 1;
	}
	nlcd_scroll_offset++;
	nlcd_display_scroll_line();
	
	return 0;
}

////////////////////////////////////////////////////////////
// Scroll line to the right
// Stops scrolling when there is no more data on the left
// to display.
////////////////////////////////////////////////////////////
char nlcd_scroll_right(){
	if (nlcd_scroll_offset == 0){
		return 1;
	}
	nlcd_scroll_offset--;
	nlcd_display_scroll_line();
	return 0;
}
