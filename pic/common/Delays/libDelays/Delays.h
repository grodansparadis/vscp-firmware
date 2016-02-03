#ifndef _LIBSUSER_DELAYS_H
#define _LIBSUSER_DELAYS_H

#include <$FEDPATH/Libs/delays.h>	// Include standard FED library

#define Delay1TCY() 	_Pragma(asmline nop)

extern void Delay10TCYx(unsigned char unit);
#pragma asmfunc Delay10TCYx

extern void Delay100TCYx(unsigned char unit);
#pragma asmfunc Delay100TCYx

extern void Delay1KTCYx(unsigned char unit);
#pragma asmfunc Delay1KTCYx

extern void Delay10KTCYx(unsigned char unit);
#pragma asmfunc Delay10KTCYx

/*********************************************************************
** Tell the world about our library
*********************************************************************/
#if _SERIES == 18
   #pragma wizcpp uselib "$__PATHNAME__/libDelays/delays16.c"
#else
   #pragma wizcpp uselib "$__PATHNAME__/libDelays/delays.c"
#endif

#endif	/* _LIBSUSER_DELAYS_H */
