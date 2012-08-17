/*************************************************************
* Simple 595 shift register interface 
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _SR_H_
#define _SR_H_

#include <system.h>

// IO's
//volatile bit sr_noe 	@ PORTA . 0;
volatile bit sr_rst		@ PORTA . 1;
volatile bit sr_dta		@ PORTA . 5;
volatile bit sr_lck		@ PORTA . 2;
volatile bit sr_sck 	@ PORTA . 3;

// Tris bits
volatile bit sr_rst_tris @ TRISA . 1;
volatile bit sr_dta_tris @ TRISA . 5;
volatile bit sr_lck_tris @ TRISA . 2;
volatile bit sr_sck_tris @ TRISA . 3;

void sr_load_byte(char data);
void sr_init();

#endif // _SR_H_
