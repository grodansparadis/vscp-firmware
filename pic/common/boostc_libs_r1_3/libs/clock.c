/*************************************************************
* 24h clock routines
*
* Keeps track of the current time 
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/
#include "clock.h"
#include "serial.h"

char clock_hours;
char clock_mins;
char clock_secs;

// Called every second
void clock_increment(){
	clock_secs++;
	if (clock_secs >= 60) {
		clock_mins++;
		clock_secs = 0;
	}
	if (clock_mins >= 60) {
		clock_hours++;
		clock_mins = 0;
	}
	if (clock_hours >= 24) {
		clock_hours = 0;
	}
		
	return;
}

void clock_set(char hours, char mins, char secs){
	clock_hours = hours;
	clock_mins  = mins;
	clock_secs  = secs;
	
	return;
}

void clock_clear(){
	clock_secs = 0;
	clock_mins = 0;
	clock_hours = 0;
	
	return;
}

void clock_print(){

	serial_print_dec(clock_hours, 2);		
	serial_printf(":");
	serial_print_dec(clock_mins, 2);
	serial_printf(".");
	serial_print_dec(clock_secs, 2);
		
	return;	
}

char clock_get_minutes(){
	return clock_mins;
}

char clock_get_seconds(){
	return clock_secs;
}

char clock_get_hours(){
	return clock_hours;	
}
