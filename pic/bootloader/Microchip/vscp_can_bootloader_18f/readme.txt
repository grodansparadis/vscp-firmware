This bootloader will work with the VSCP PIC1 algorithm of VSCP Works.

Use file/export in MPLAB after build to write the HEX file.

When programmed into a device and activated (byte 0 in EEPROM is 0xFF on startup) a confirm bootloader 
mode CAN message with id= 0x000014nn and no data will be sent. Node id (nn) (least eight bits of id) is
 taken from EEPROM byte 1.  

If byte 0 in EEPROM is not oxFF on startup a normal boot of the relocated code will take place.


Ake Hedman
akhe@grodansparadis.com
