#ifndef _1wire_h_
#define _1wire_h_

#include "types.h"

/*******************************************/
/* Hardware connection                     */
/*******************************************/

// OW connected to P0.17 - parsite power not supported so far
//#define OW_PIN  17
//#define OW_BIT ( 1 << OW_PIN )
#define OW_CONF_DELAYOFFSET 30

#define OW_MATCH_ROM	0x55
#define OW_SKIP_ROM	    0xCC
#define	OW_SEARCH_ROM	0xF0

#define	OW_SEARCH_FIRST	0xFF		// start new search
#define	OW_PRESENCE_ERR	0xFF
#define	OW_DATA_ERR	    0xFE
#define OW_LAST_DEVICE	0x00		// last device found
//			0x01 ... 0x40: continue searching

// rom-code size including CRC
#define OW_ROMCODE_SIZE 8

extern BOOL ow_reset(void);

extern uint8_t ow_bit_io( uint8_t b );
extern uint8_t ow_byte_wr( uint8_t b );
extern uint8_t ow_byte_rd( void );

extern uint8_t ow_rom_search( uint8_t diff, uint8_t *id );

extern void ow_command( uint8_t command, uint8_t *id );

extern void ow_parasite_enable( void );

extern BOOL ow_input_pin_state( void );

extern void ow_delay_debug( void );

#endif

