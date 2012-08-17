/*************************************************************
* One wire interface lib for the Dallas DS1820 and alike
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/
#ifndef _OO_H_
#define _OO_H_

#include <system.h>

// Comment the following define if CRC checking does not need to be performed.
// Can be useful on devices with limited program memory available
#define OO_CRC_CHECKING

// Offset in EEPROM memory of the nr of items and the ID list
#define OO_EEPROM_NR_IDS	0
#define OO_EEPROM_ID_BASE	8

#define OO_PORT PORTB
#define OO_TRIS TRISB
#define OO_PIN  0

// Structure for data exchange
typedef struct s_oo_data {
                 char  id[8];
                 char  t_msb;
                 char  t_lsb;
                 char  remain;
                 char  nr_count;
                 char  valid;
              } oo_tdata;

// ====================================================================
// Commands for the DS1820
//    1. ROM function commands
#define        OO_READROM        	0x33    
#define        OO_MATCHROM       	0x55
#define        OO_SKIPROM        	0xCC
#define        OO_SEARCHROM      	0xF0
#define        OO_ALARMSEARCH    	0xEC

//    2. Memory Command Functions
#define        OO_WRITEPAD       	0x4E
#define        OO_READPAD        	0xBE
#define        OO_COPYPAD        	0x48
#define        OO_CONVERTT       	0x44
#define        OO_RECALLE2			0xB8
#define        OO_READSUPPLY		0xB4

volatile bit oo_bus      @ OO_PORT . OO_PIN;
volatile bit oo_bus_tris @ OO_TRIS . OO_PIN;

#define oo_readmode  oo_bus_tris = 1
#define oo_writemode oo_bus_tris = 0

char  oo_busreset();
short oo_get_temp();
char  oo_get_pad_byte(char index);
char  oo_read_scratchpad();
char  oo_conversion_busy();
void  oo_start_conversion();
char  oo_wait_for_completion();
char  oo_device_search();
char  oo_scanbus();
char     oo_get_next_id();
char     oo_get_devicecount();
oo_tdata oo_read_device(char count);

#endif // _OO_H_
