/*************************************************************
* 24h clock routines
*
* Keeps track of the current time 
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <system.h>

void clock_increment();
void clock_set(char hours, char mins, char secs);
void clock_clear();
void clock_print();
char clock_get_minutes();
char clock_get_seconds();
char clock_get_hours();

#endif // _CLOCK_H_
