#ifndef VSCP_APPLAYER_H
#define	VSCP_APPLAYER_H

#include "inttypes.h"


#ifdef	__cplusplus
extern "C" {
#endif



/*
 * Data Register
 *
 * Page 0x00
 *  0x00 - 0x7F --> 
 *  0x80 - 0xFF --> VSCP Register
 *
 * Page 0x01  --> Window Engine 0
 * Page 0x02  --> Window Engine 1
 * Page 0x03  --> Window Engine 2
 * Page 0x04  --> Window Engine 3
 *
 * Page 0x80  --> Decision Matrix
 */

//EEPROM-mirrored variables
uint8_t vscp_zone;
extern uint8_t deviceFamilyCode;
extern uint8_t deviceFamilyType;
extern uint8_t firmwareVersion[3];
extern uint8_t GuID[16];

void init_app_eeprom();
void vscp_freeRunning();
void vscp_10mS_Running();
void vscp_100mS_Running();
void vscp_ledActivity();

void doDM();

void vscp_debugMsg(uint8_t subzone, char *msg, uint8_t size, uint8_t msgIdx);
void init_augusto_ram( void );
void init_augusto_eeprom();

typedef enum{
    VSCP_ACTION_WINDOW_ENGINE = 1
}VSCP_ACTION;

#ifdef	__cplusplus
}
#endif

#endif	/* VSCP_APPLAYER_H */

