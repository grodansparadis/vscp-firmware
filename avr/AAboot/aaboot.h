

// # of elements in decision matrix
#define DESCION_MATRIX_ELEMENTS             0

// Start register position for decision matrix
//#define REG_DESCION_MATRIX                  112		// should equal the MDF!


// EEPROM Storage
#define VSCP_EEPROM_BOOTLOADER_FLAG         0x00	// Reserved for bootloader	 

#define VSCP_EEPROM_NICKNAME                0x01	// Persistant nickname id storage
#define VSCP_EEPROM_SEGMENT_CRC             0x02	// Persistant segment crc storage
#define VSCP_EEPROM_CONTROL                 0x03	// Persistant control byte

#define VSCP_EEPROM_REG_USERID              0x04
#define VSCP_EEPROM_REG_USERID1             0x05
#define VSCP_EEPROM_REG_USERID2             0x06
#define VSCP_EEPROM_REG_USERID3             0x07
#define VSCP_EEPROM_REG_USERID4             0x08

// The following can be stored in flash or eeprom

#define VSCP_EEPROM_REG_MANUFACTUR_ID0      0x09
#define VSCP_EEPROM_REG_MANUFACTUR_ID1      0x0A
#define VSCP_EEPROM_REG_MANUFACTUR_ID2      0x0B
#define VSCP_EEPROM_REG_MANUFACTUR_ID3      0x0C

#define VSCP_EEPROM_REG_MANUFACTUR_SUBID0   0x0D	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID1   0x0E	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID2   0x0F	
#define VSCP_EEPROM_REG_MANUFACTUR_SUBID3   0x10

#define VSCP_EEPROM_REG_ZONE                0x11

#define VSCP_EEPROM_REG_SUBZONE             0x12

// The following can be stored in program ROM (recommended) or in EEPROM 

#define VSCP_EEPROM_REG_GUID                0x11    // Start of GUID MSB	
                                                    // 		0x11 - 0x20		not used		

#define VSCP_EEPROM_REG_DEVICE_URL          0x21	// Start of Device URL storage
                                                    // 		0x21 - 0x40		not used

#define VSCP_EEPROM_END                     0x11	// marks end of VSCP EEPROM usage
                                                    //   (next free position) was 0x41



// Prototypes
void mainbootloader();
void mainvscp();
void SendInformationEvent( uint8_t idx, uint8_t eventClass, uint8_t eventTypeId );;
int readEEPROM( uint8_t addr );
int writeEEPROM( uint8_t addr, uint8_t data );

void SendInformationEventExtended
	(uint8_t priority, uint8_t zone, uint8_t subzone, uint8_t idx, 
	uint8_t eventClass, uint8_t eventTypeId );

void boot_program_page (uint32_t page, uint8_t *buf);




// debug routines
void sendCharHex (char data);
void sendChar (char data);


