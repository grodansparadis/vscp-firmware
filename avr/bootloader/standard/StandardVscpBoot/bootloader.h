/*--------------------------------------------------------------------------------------------
 * VSCP Standard bootloader definitions to be used in firmware & bootloader routines
 *--------------------------------------------------------------------------------------------
*/  
#ifndef _VSCP_STANDARD_BOOTLOADER_H_
#define _VSCP_STANDARD_BOOTLOADER_H_


#define BOOT_APPLICATION    0x00 	// Boot application
#define NODEID_VIRGIN       0xFE    // Use for node without modeid




void vscp_goBootloaderMode( uint8_t algo );

#endif
//define BOOTSTART in makefile!

