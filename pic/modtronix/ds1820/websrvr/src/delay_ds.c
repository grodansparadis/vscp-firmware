/*
 *	Delay functions
 *	See delay.h for details
 *
 *	Make sure this code is compiled with full optimization!!!
 */

#include	"delay_ds.h"

void
DelayMs_ds(unsigned char cnt)
{
#if	XTAL_FREQ <= 2MHZ
	do {
EER
		Delayus_ds(996);
	} while(--cnt);
#endif

#if    XTAL_FREQ > 2MHZ	
	unsigned char	i;
	do {
//	ERR234
		i = 4;
		do {
			Delayus_ds(250);
		} while(--i);
	} while(--cnt);
#endif
}
