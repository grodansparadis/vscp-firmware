/*
Debug implementation. This file used to define the following debug function:
- dbPutByte
- dbPutRomString
- dbPutRomString
*/

#ifndef _DEBUG_H_
#define _DEBUG_H_

/*******************************************************
Defines for USART1 implementation
********************************************************/
//#include "serbasic.h"
#include "serint.h"

//If any debug is enabled:
//- define all debug functions - are mapped to functions that will write to USART or somewhere else
#define debugPutByte(a) serPutByte(a)
#define debugPutRomString(a) serPutRomString(a)
#define debugPutString(a) serPutString(a)


#endif  //_DEBUG_H_
