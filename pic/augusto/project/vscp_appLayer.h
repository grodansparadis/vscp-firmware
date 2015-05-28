#ifndef VSCP_APPLAYER_H
#define	VSCP_APPLAYER_H

#include "inttypes.h"
#include "vscp_firmware.h"

#ifdef	__cplusplus
extern "C" {
#endif



#define VSCP_EEPROM_BOOTLOADER_FLAG	0x00    // Reserved for bootloader
#define VSCP_EEPROM_NICKNAME            0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC         0x02    // Persistant segment crc storage
#define VSCP_EEPROM_ZONE                0x03    // Persistant vscp zone
    
//EEPROM-mirrored variables
uint8_t vscp_zone;
extern const uint8_t deviceFamilyCode;
extern const uint8_t deviceFamilyType;
extern const uint8_t firmwareVersion[3];
extern const uint8_t GuID[16];

/* Decision Matrix */
extern struct _dmrow decisionMatrix[VSCP_DM_SIZE];
extern void doApplicationDM(int DecisionMatrixIndex);

void init_app_eeprom();
void vscp_freeRunning();
void vscp_10mS_Running();
void vscp_100mS_Running();
void vscp_ledActivity();

void doDM();

void init_augusto_ram( void );
void init_augusto_eeprom();

#ifdef	__cplusplus
}
#endif

#endif	/* VSCP_APPLAYER_H */

