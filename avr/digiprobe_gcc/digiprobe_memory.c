/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Common Memory allocation (shared by application and bootloader
*******************************************************************************/

#ifdef GUID_IN_FLASH
	const uint8_t STD_REG_GUID[16] = { MY_GUID };
#else
	uint8_t STD_REG_GUID[16] EEMEM = { MY_GUID };
#endif

#ifdef MDF_URL_IN_FLASH
	const uint8_t STD_REG_MDF_URL[] = MY_MDF_URL;
#else
	uint8_t STD_REG_MDF_URL[] EEMEM = MY_MDF_URL;
#endif

// Manufacturer id's is stored in ROM for this module
// offset 0-3 Manufacturer device id 0-3
// offset 4-7 Manufacturer subdevice id 0-3
#ifdef MANUFACTID_IN_FLASH
	const uint8_t STD_REG_MANUFACTID[8] = { MY_MANUFACTID };
#else
	uint8_t STD_REG_MANUFACTID[8] EEMEM = { MY_MANUFACTID };
#endif

uint8_t STD_REG_NICKNAME 			EEMEM = VSCP_ADDRESS_FREE;
uint8_t STD_REG_SEGMENT_CRC			EEMEM = 0;
uint8_t STD_REG_CONTROL				EEMEM = 0;
uint8_t STD_REG_ZONE 				EEMEM = 0;
uint8_t STD_REG_SUBZONE 			EEMEM = 0;
uint8_t STD_REG_USERID[5]			EEMEM = {17,3,4,18,5};

uint8_t P0_USER_REGS[PAGE0_USER_REGISTERS] EEMEM;
