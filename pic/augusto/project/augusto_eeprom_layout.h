/* 
 * File:   eeprom_layout.h
 * Author: F20472A
 *
 * Created on 12 agosto 2014, 15.33
 */

#ifndef EEPROM_LAYOUT_H
#define	EEPROM_LAYOUT_H


#ifdef	__cplusplus
extern "C" {
#endif

#define VSCP_EEPROM_BOOTLOADER_FLAG 0
#define VSCP_EEPROM_NICKNAME        VSCP_EEPROM_BOOTLOADER_FLAG + 1
#define VSCP_CHECKSUM               VSCP_EEPROM_NICKNAME + 1
#define VSCP_EEPROM_ZONE            VSCP_CHECKSUM +1
#define VSCP_EEPROM_GUID            VSCP_EEPROM_ZONE + 1
#define VSCP_EEPROM_USERID          VSCP_EEPROM_GUID + 5
#define VSCP_EEPROM_MANUFACTUR_ID   VSCP_EEPROM_USERID + 8
#define __next                   VSCP_EEPROM_MANUFACTUR_ID + 16

    
#define VSCP_DM_EEPROM_START_LOC 0x100 //Starting position for DM



#ifdef	__cplusplus
}
#endif

#endif	/* EEPROM_LAYOUT_H */

