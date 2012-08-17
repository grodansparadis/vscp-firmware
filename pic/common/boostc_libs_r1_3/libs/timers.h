/*************************************************************
* Timer helper functions for the PIC16F87X series 
*
* Allows for easy setup of the timers with interrupts
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _TIMERS_H_
#define _TIMERS_H_

#include <system.h>

// Timer 0 controlling functions
void tmr0_setup();
#define tmr0_set(value) tmr0 = value;
#define tmr0_start() set_bit(intcon, T0IE);
#define tmr0_stop()  clear_bit(intcon, T0IE);

// Timer 1 controlling functions
void tmr1_setup();
void tmr1_set(short value);
#define tmr1_start() set_bit(t1con, TMR1ON);
#define tmr1_stop()  clear_bit(t1con, TMR1ON);

// Timer 2 controlling functions
void tmr2_setup();
#define tmr2_set(value) tmr2 = value;
#define tmr2_start()    t2con.TMR2ON = 1;
#define tmr2_stop()     t2con.TMR2ON = 0;

#endif // _TIMERS_H_
