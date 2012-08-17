/** 
 *************************************************************************************
 * @file	debug.h
 * 
 * @brief	Library debug support
 * @author	Alex Raimondi
 * 		  
 * This library implements some functions to support debugging over USART.
 * There are some functions to print different data types. 
 * An assert is also implemented. More debug functions, mostly for basic 
 * data types are in AT91Library.<br>
 * If DEBUG is not specified while compiling all calls to debug functions
 * will be removed from output code.
 *************************************************************************************
*/

#ifndef DEBUG_H
#define DEBUG_H


#ifdef DEBUG 

#include <string-avr.h>
#include <sig-avr.h>
#include <io.h>

#include "typedefs.h"

/**
 *************************************************************************************
 * @brief	Assert function.
 *	
 * Formats all the supplied information (argument, file name, line number)	
 * into a string and puts it on the debug interface.
 * 
 * @warning	This function does <B>not</B> return
 *
 * @note	debug must be set up prior calling this function
 *
 * @return	None
 *************************************************************************************
*/
void _Assert( const char *cpArg, const char *cpFile, int16 nLine);

/**
 *************************************************************************************
 * @brief	Assert macro
 *	
 * If the expression arg is true no action is taken. If the expression is false
 * the assert function is called.
 *
 * @note	debug must be set up prior using this macro
 *
 * @return	None
 *************************************************************************************
*/
#define assert(arg) ((arg) ? (void)0 : (void) _Assert ( #arg,__FILE__, __LINE__ ) )

char * u8tohexStr( uint8 i, char * buf );
char * u16tohexStr( uint16 i, char * buf );

void debug_init( eBaudRate br );
void debug_str( const char * str );
void debug_u8( const uint8 i );
void debug_u16( const uint16 i );

void uartSendStr( const char * str );

#else ///< DEBUG is not defined: ignore all debug_xxx statements

#define assert(ignore)				( (void) 0) 
#define debug_init(ignore)			( (void) 0)
#define debug_str(ignore)			( (void) 0)
#define debug_u8(ignore)			( (void) 0)

#endif

#endif
