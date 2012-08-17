/*************************************************************
* Timer helper functions for the PIC16F87X series 
*
* Allows for easy setup of the timers with interrupts
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/
#include "timers.h"

void tmr0_setup(){
	// Timer setup
	clear_bit(option_reg, T0CS); // tmr0 runs on internal clock
	// Set and activate the prescaler here if required
	option_reg.PSA = 0; // prescaler assigned to TMR0
	option_reg |= 0x07; // maximum prescaler

	// Clear the TMR register
	tmr0 = 0x00;
	
	// Clear possible interrupt flag
	clear_bit(intcon, T0IF);
}

void tmr1_setup(){
	// Timer setup
	t1con = 0x30; // prescaler = 8, internal clock
	
	// Clear the TMR registers
	tmr1h = 0x00;
	tmr1l = 0x00;
	
	// Interrupt enable
	clear_bit(pir1, TMR1IF);
	set_bit(pie1, TMR1IE);
	set_bit(intcon, PEIE);
}

void tmr2_setup(){
	// Timer setup
	t2con = 0x3A; // 2 x max prescaler
	tmr2  = 0x00; // reset the TMR register
	pr2   = 0xFF; // generate interrupt when register gets full

	// Interrupt enable
	pir1.TMR2IF = 0;
	pie1.TMR2IE = 1;
	intcon.PEIE = 1;	
	
	
}
void tmr1_set(short value){
	// Program timer 1 registers
	HIBYTE(tmr1h, value);
	LOBYTE(tmr1l, value);
	
	return;
}
