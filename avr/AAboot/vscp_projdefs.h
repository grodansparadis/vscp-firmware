
#ifndef CAN128_TEST_PROJDEFS_H
#define CAN128_TEST_PROJDEFS_H

#include <avr/io.h>






//
// Undefine one of the following suitable for the
// platform you want to compile for 
//defined in makefile
//#define ATMEL_STK501
//#define OLIMEX_AT90CAN128
//#define OLIMEX_AT90CAN32


// System clock in kHz
//defined in makefile
//#define FOSC                16000  // Available value: 22118 16000 12000 11059 custom 
//#define FOSC                14300  // Available value: 22118 16000 12000 11059 custom 

// UART channel to use 
//defined in makefile
//#define USE_UART1 
//#define USE_UART2

// Baudrate
#define BAUDRAT             9600 
 

#ifndef FALSE
#define FALSE               0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

#ifndef ON
#define ON                  !FALSE
#endif

#ifndef OFF
#define OFF                 FALSE
#endif

#ifndef BIT
#define BIT(n)              (1 << (n))
#endif   

#ifndef BOOL
#define BOOL                int
#endif


// VSCP Defines
// ============
//
// Described in vscp.h

// If GUID should be in EEPROM define the following
//#define GUID_IN_EEPROM

// If manufacturing D and GUID should bw possible to write when page_select = 0xffff
// undef the following
#define ENABLE_WRITE_2PROTECTED_LOCATIONS

// Disable initial nickname discovery by undefining the following
//#define DISABLE_AUTOMATIC_NICKNAME_DISCOVERY

#endif
