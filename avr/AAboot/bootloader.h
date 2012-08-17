/*--------------------------------------------------------------------------------------------
 * bootloader definitions to be used in firmware & bootloader routines
 *--------------------------------------------------------------------------------------------
*/  



#define BTL_RUN			0x00 	//RUN, continue to application
#define BTL_LOAD		0xFE	//LOAD NEW FIRMWARE
#define BTL_APP_BLOCK	0xFE	//BLOCKED NODE, application corrupt, only bootloader possible
#define BTL_FRESH		0xFF	//FRESH NODE, only bootloader possible (= blank eeprom)


// application / bootloader memory defines 
// for AT90CAN128 with bootsize 4096 (bootsz=00)
// AFS = 0x0000 -> 0xEFFF which are 0xF000 words to program (= 1E000 bytes)
// if we use blocksize 0xFF, there are 0x1E000 div 0xFF blocks to send (= 0x1E1 = 481) 
#ifdef can128
#define BTL_BLOCKSIZE	0x00FF	
#define BTL_NRofBLOCKS	0xFF //so far 0xFF is maximum blocks possible	
#endif

// for AT90CAN32 with bootsize 4096 (bootsz=00)
// AFS = 0x0000 -> 0x2FFF which are 0x3000 words to program (=0x6000 bytes)
// if we use blocksize 0xFF, there are 0x6000 div 0xFF blocks to send (= 0x60 = 96) 
#ifdef can32
#define BTL_BLOCKSIZE	0x00FF	
#define BTL_NRofBLOCKS	0x60 	
#endif


//define BOOTSTART in makefile!


/*
statusbyte resides in EEPROM adress 0x00 as VSCP_EEPROM_BOOTLOADER_FLAG
after reset, the bootloader performs actions based on this flag

version 0.0
*/
