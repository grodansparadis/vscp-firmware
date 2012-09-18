/*--------------------------------------------------------------------------------------------
 * bootloader definitions to be used in firmware & bootloader routines
 *--------------------------------------------------------------------------------------------
*/  



#define BTL_RUN			0x00 	//RUN, continue to application
#define BTL_LOAD		0xFE	//LOAD NEW FIRMWARE
#define BTL_APP_BLOCK	0xFE	//BLOCKED NODE, application corrupt, only bootloader possible
#define BTL_FRESH		0xFF	//FRESH NODE, only bootloader possible (= blank eeprom)


// application / bootloader memory defines 

// for AT90CAN128 with bootsize 8192 byte (4096 words) (bootsz=00)
// AFS = 120 K of memory
// Block size = 256 bytes
// 120k = 122880 byte
// 122880/256 = 480 block left for Application flash memory 
// 0x01E0  --- 480 blocks of memory
#ifdef can128
#define BTL_BLOCKSIZE	0x0100   // Block size ====>  i.e 256 byte = 128 words	
#define BTL_NRofBLOCKS	0x1E0    // maximum blocks possible for application Flash memory	
#endif

// for AT90CAN32 with bootsize 4096 (bootsz=00)
// AFS = 0x0000 -> 0x2FFF which are 0x3000 words to program (=0x6000 bytes)
// if we use blocksize 0xFF, there are 0x6000 div 0xFF blocks to send (= 0x60 = 96) 
//#ifdef can32
//#define BTL_BLOCKSIZE	0x00FF	
//#define BTL_NRofBLOCKS	0x60 	
//#endif


//define BOOTSTART in makefile!


/*
statusbyte resides in EEPROM adress 0x00 as VSCP_EEPROM_BOOTLOADER_FLAG
after reset, the bootloader performs actions based on this flag

version 0.0
*/
