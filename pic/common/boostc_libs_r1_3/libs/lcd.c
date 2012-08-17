/*************************************************************
* 7-pins LCD interface code 
*
* Drives an LCD in 4-bit mode, using the three control
* signals. This has the advantage that the code is
* independent of the LCD, since there are no
* timing loops in the code. The code is blocking
* and used the LCD busy signal to determine if the 
* display is ready to accept new data.
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#include "lcd.h"

// Local variable
bit lcd_found = 0;

////////////////////////////////////////////////////////////
// Drive a certain nibble on the LCD data pins, the nibble
// is taken from the lsb's of the input char
////////////////////////////////////////////////////////////
void lcd_clock_nibble(char nibble){
	
	// Since there seems to be no way to extract a bit from a byte in BoostC -> do it in asm.
	// It is replicated 4 times here, since a function cannot return a bit eiter.
	
	// Set the d4 depending on the state of nibble bit 0
	asm {
	btfss _nibble, 0
	goto  clear_lcd4
	bsf	  _lcd_d4, F
	goto  done_lcd4
	clear_lcd4:
	bcf	  _lcd_d4, F
	done_lcd4:
	}
	
	// Set the d5 depending on the state of nibble bit 1
	asm {
	btfss _nibble, 1
	goto  clear_lcd5
	bsf	  _lcd_d5, F
	goto  done_lcd5
	clear_lcd5:
	bcf	  _lcd_d5, F
	done_lcd5:
	}	

	// Set the d6 depending on the state of nibble bit 2
	asm {
	btfss _nibble, 2
	goto  clear_lcd6
	bsf	  _lcd_d6, F
	goto  done_lcd6
	clear_lcd6:
	bcf	  _lcd_d6, F
	done_lcd6:
	}	

	// Set the d7 depending on the state of nibble bit 3
	asm {
	btfss _nibble, 3
	goto  clear_lcd7
	bsf	  _lcd_d7, F
	goto  done_lcd7
	clear_lcd7:
	bcf	  _lcd_d7, F
	done_lcd7:
	}	

	lcd_toggle_e;
	
	return;
}

////////////////////////////////////////////////////////////
// Read the busy flag of the LCD and wait to return until 
// the LCD is ready.
// Skipped when running in debug mode.
////////////////////////////////////////////////////////////
void lcd_wait_busy(){

	// Skip this function if we are running in debug mode.
#ifdef DEBUG
	return;
#endif

	// If we have not detected an LCD in a previous call to this function,
	// then we just skip it this time, since else the program will hang later
	// on in this function.
	if (!lcd_found){
		return;
	}
	
	// Save the current LCD mode
	bit mode = lcd_rs;
		
	// The data lines are inputs
	lcd_tris_d4 = 1;
	lcd_tris_d5 = 1;
	lcd_tris_d6 = 1;
	lcd_tris_d7 = 1;

	// Enter command mode
	lcd_cmd_mode;
	lcd_rw      = 1;

	// Put lcd_e high
	lcd_e_hi;	
	
	char counter = 0;
	
	// Wait for completion of the operation, with a timeout of ~.5 seconds
	// LCD d7 is high if the operation is complete.
	while (lcd_d7 && counter < 0xFF){
		lcd_e_lo;
		lcd_e_hi;
		lcd_e_lo;
		lcd_e_hi;
		delay_ms(2);
		counter++;
	}
	
	// Check if the previous loop timed out 	
	if (counter == 0xFF) {
		// If it was a timeout -> disable the flag.
		lcd_found = 0;
	}
	
	// And put the lcd_lo again
	lcd_e_lo;	
		
	// Restore the TRIS
	lcd_tris_d4 = 0;
	lcd_tris_d5 = 0;
	lcd_tris_d6 = 0;
	lcd_tris_d7 = 0;
	
	// Restore LCD RS mode.
	lcd_rs = mode;
	lcd_rw = 0;

	return;
}

////////////////////////////////////////////////////////////
// Send a byte to the LCD. Don't forget to set the mode 
// (data or cmd) before calling this function.
////////////////////////////////////////////////////////////
void lcd_send_byte(char data){

	char temp = data >> 4;

	lcd_rw = 0;	// Write mode

	// Clock the high nibble
	lcd_clock_nibble(temp);

	// Clock the low nibble
	lcd_clock_nibble(data);
	
	// Wait until the LCD is finished
	lcd_wait_busy();
}

////////////////////////////////////////////////////////////
// Initialisation sequence of the LCD
////////////////////////////////////////////////////////////
void lcd_init(){

	// Wait for internal reset
	delay_ms(15);

	// Set all pins as output
	lcd_tris_rs = 0;
	lcd_tris_rw = 0;
	lcd_tris_e  = 0;
	lcd_tris_d4 = 0;
	lcd_tris_d5 = 0;
	lcd_tris_d6 = 0;
	lcd_tris_d7 = 0;
	
	clear_bit(trisa, 1);
	clear_bit(porta, 1);
	
	// Clear the control bits
	lcd_rw = 0;
	lcd_cmd_mode;

	// Init sequence (see datasheet)
	/// attention (clock 0x03 twice)
	lcd_clock_nibble(0x03);
	delay_ms(5);
	lcd_toggle_e;
	delay_10us(15);
	/// 4bit mode (clock 0x03 and 0x02)
	lcd_toggle_e;
	delay_ms(5);
	lcd_d4 = 0;
	lcd_toggle_e;
	delay_10us(15);
	
	// Assume we have an LCD attached
	lcd_found = 1;
	
	/// We're in 4 bit mode now, program general settings	
	lcd_send_byte(FUNCTION_SET);
	lcd_send_byte(DISP_OFF);
	lcd_send_byte(DISP_ON);
	lcd_send_byte(ENTRY_INC);
	lcd_send_byte(DISP_CLR);
	lcd_send_byte(LINE1);

	// Init done, you can start writing characters now
	lcd_data_mode;

	return;

}

////////////////////////////////////////////////////////////
// Send a command to the LCD
////////////////////////////////////////////////////////////
void lcd_send_cmd(char command){
	lcd_cmd_mode;
	lcd_send_byte(command);
}

////////////////////////////////////////////////////////////
// Send a line to the LCD
////////////////////////////////////////////////////////////
void lcd_printf(const char* line){
    char i = 0;
    lcd_data_mode;
    while(line[i] != 0)
        lcd_send_byte(line[i++]);
}

////////////////////////////////////////////////////////////
// Print a character to the LCD
////////////////////////////////////////////////////////////
void lcd_printf(char value){
	lcd_data_mode;
	lcd_send_byte(value);
}

////////////////////////////////////////////////////////////
// Print a hex value to the LCD
////////////////////////////////////////////////////////////
void lcd_print_hex(char value){
	lcd_data_mode;
	
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
        lcd_send_byte(hexChar);
    }
}

////////////////////////////////////////////////////////////
// Print a 16-bit hex value to the LCD
////////////////////////////////////////////////////////////
void lcd_print_hex(short value){

    char value1 = (char)((value >> 8) & 0x00FF);
    
	lcd_print_hex(value1);
 
    char value0 = (char)(value & 0x00FF);
    
	lcd_print_hex(value0);
	
	return;
}
