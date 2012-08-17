/*************************************************************
* Flash program memory interface for PIC16F87X devices 
*
* Allows for read and write of the program memory. Be aware
* that write operations by design are performed by four bytes 
* at once. 
*
* Warning! Interrupts are disabled during write !
*
* (c) 2005, L. Hollevoet, boostc compiler.
*************************************************************/
#ifndef _FLASH_TEST_H_
#define _FLASH_TEST_H_

#include <system.h>

// Define oscillator frequency
#pragma CLOCK_FREQ 10000000

// Define port directions
#define PortAConfig  11111111b  //  1=input
#define PortBConfig  11000000b  //  1=input 
#define PortCConfig  11010000b	

// Function declarations
void init(void);

#endif //_FLASH_TEST_H_
