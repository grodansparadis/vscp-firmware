#ifndef REGISTERS_H
#define REGISTERS_H

//
// EEPROM locations for persistant storage
//


// Storage for module persistent data

// EEPROM Storage
#define VSCP_EEPROM_BOOTLOADER_FLAG				0x00	// Reserved for bootloader	 
#define VSCP_EEPROM_NICKNAME							0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC						0x02	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL								0x03	// Persistant control byte
                                      		
#define VSCP_EEPROM_REG_USERID						0x04
#define VSCP_EEPROM_REG_USERID1						0x05
#define VSCP_EEPROM_REG_USERID2						0x06
#define VSCP_EEPROM_REG_USERID3						0x07
#define VSCP_EEPROM_REG_USERID4						0x08

// The following can be stored in flash or eeprom
#define VSCP_EEPROM_REG_MANUFACTUR_ID0		0x09
#define VSCP_EEPROM_REG_MANUFACTUR_ID1		0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID2		0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID3		0x0C

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0	0x0D	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1	0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2	0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3	0x10


// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID							0x11	// Start of GUID MSB	0x11 - 0x20				
#define VSCP_EEPROM_REG_DEVICE_URL				0x21	// Start of Device URL storage		0x21 - 0x40

#define VSCP_EEPROM_REG_FREE_1						0x41
#define VSCP_EEPROM_REG_FREE_2            0x42
#define VSCP_EEPROM_REG_FREE_3            0x43
#define VSCP_EEPROM_REG_FREE_4            0x44
#define VSCP_EEPROM_REG_FREE_5            0x45
#define VSCP_EEPROM_REG_FREE_6            0x46
#define VSCP_EEPROM_REG_FREE_7            0x47
#define VSCP_EEPROM_REG_FREE_8            0x48
#define VSCP_EEPROM_REG_FREE_9            0x49
#define VSCP_EEPROM_REG_FREE_10           0x4A
#define VSCP_EEPROM_REG_FREE_11           0x4B
#define VSCP_EEPROM_REG_FREE_12           0x4C
#define VSCP_EEPROM_REG_FREE_13           0x4D
#define VSCP_EEPROM_REG_FREE_14           0x4E
#define VSCP_EEPROM_REG_FREE_15           0x4F

#define REG_APP_START											0x50	// marks start of Device EEPROM usage  

#define REG_APP_ZONE											0x50	// Zone node belongs to
#define REG_DOOR_SUBZONE									0x51	// Subzone for Door Open System
#define REG_ALERT_SUBZONE								  0x52	// Subzone for Buzzer

#define REG_APP_FREE_1										0x53
#define REG_APP_FREE_2                    0x54
#define REG_APP_FREE_3                    0x55
#define REG_APP_FREE_4                    0x56
#define REG_APP_FREE_5                    0x57
#define REG_APP_FREE_6                    0x58
#define REG_APP_FREE_7                    0x59
#define REG_APP_FREE_8                    0x5A
#define REG_APP_FREE_9                    0x5B
#define REG_APP_FREE_10                   0x5C
#define REG_APP_FREE_11                   0x5D
#define REG_APP_FREE_12                   0x5E
#define REG_APP_FREE_13                   0x5F

#define DOOR_TABLE_MAX_NBR								46		// MAX Quantity of table secuence door open
#define DOOR_TABLE_START									0x60
#define DOOR_TABLE_END										0xBB

#define DOOR_RALENTI_OPEN									0xBC
#define DOOR_RALENTI_CLOSED								0xBD

#define REG_APP_FREE_15                   0xBE
#define REG_APP_FREE_16                   0xBF
#define REG_APP_END												0xBf	// marks End of Device EEPROM usage  

#define DMATRIX_NBR												8			// Quantity of Decision Matrix Elements (8)
#define REG_DMATRIX_START									0xc0	// Decision Matrix Start
#endif
