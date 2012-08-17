/*************************************************************
* Simple 595 shift register interface 
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#include "sr.h"

void sr_load_byte(char data){

	char i = 0;

	for (i = 0; i < 8; i++){
		sr_dta = (data & 0x01);
		data = data >> 1;
		sr_sck = 0;
		sr_sck = 1;
	}

	// clock the data to the output register
	sr_lck = 0;
	sr_lck = 1;
}

void sr_init(){

	// Set I/O direction bits
	sr_rst_tris = 0;
    sr_dta_tris = 0;
    sr_lck_tris = 0;
    sr_sck_tris = 0;
    
	// Reset the shift register logic
	// and init the IO's
	sr_rst = 0;
	sr_dta = 0;
	sr_sck = 0;
	sr_lck = 0;
	sr_rst = 1;
}
