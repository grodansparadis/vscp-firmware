/*-------------------------------------------------------------------------------------------- 
 * VSCP bootloader support for AVR v0.0
 * 
 * include this file along with bootloader.h to enable bootloader support in the AVR-device
 * 
 * only 1 line should also be included in the firmware project:
 * #define VSCP_EEPROM_BOOTLOADER_FLAG         0x00	// Reserved for bootloader	 
 *
 *-------------------------------------------------------------------------------------------
*/
#include "bootloader.h"
#include <avr/wdt.h>
///////////////////////////////////////////////////////////////////////////////
//	vscp_goBootloaderMode
//
//	Go bootloader mode
//	This routine force the system into bootloader mode according
//	to the selected protocol.
//

void vscp_goBootloaderMode( void )
{
    writeEEPROM( VSCP_EEPROM_BOOTLOADER_FLAG, BTL_LOAD );
	#ifdef PRINT_CAN_EVENTS
	uart_puts( "\n #####jumping to bootloader!\n" );
	#endif
	//reboot using WD
	cli();
	wdt_enable(WDTO_1S);
	while(1);
}




///////////////////////////////////////////////////////////////////////////////
// vscp_getBootLoaderAlgorithm
//
// Get the bootloader algorithm code
//

uint8_t vscp_getBootLoaderAlgorithm( void )
{
    return VSCP_BOOTLOADER_VSCP; 	

}


