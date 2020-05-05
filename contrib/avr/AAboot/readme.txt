statusbyte resides in EEPROM adress 0x00 as VSCP_EEPROM_BOOTLOADER_FLAG
after reset, the bootloader performs actions based on this flag. The "normal" firmware must be able to set this byte.

more information on bootloader.h
 
checkout ToDo.txt for planned improvements

Tested with:
-AAmini firmware on a Olimexboard.
-AAMega


Currently used setup is AVRstudio7


Serial channel, 9600-8N1 is used for debug messages.
For activating this, define PRINT_CAN_EVENTS in vscp_projdefs.h



how to flash the bootloader:
-----------------------------
-compile bootloader.hex
-compile your AVR-build, including bootloader.h (and bootloader.c)
-merge the 2 hex files using notepad:
	open build.hex and remove the last line (this is the EOF-marker)
	open bootloader.hex and copy all text (CTRL+A)
	paste this text below in the build.hex, make shure not to insert empty lines!
	save!
-you now have 1 HEX file to upload to your AVR-board, using your normal ISP-device

-set your fuse BOOTSZ "boot flash size 4096" (depends on the bootsz specified in makefile)
-set your fuse BOOTRST

-after reset, the bootloader now starts, detects a "fresh" device and starts the normal code (jmp to 0x000)



how to upgrade the firmware:
----------------------------
only the "normal" firmware is possible to upgrade! The bootloader can only be programmed using SPI/JTAG!

-Start bootloader-software on PC
-Connect to Daemon
-select node-ID
-select HEX file
-GUID is default ok, unless firmware needs it
-same for REG 0x92 & 0x93
-algorithm = 0
-press UPLOAD

upload should start immediatly (unless you build in some extra precautions in your firmware)
When upload is not succesful, the bootloader blocks the firmware. You should always be able to restart the upload of the firmware, untill succesful upload is completed.



notes:
--------
the PC-software assumes a "solid" hex file. This is not how it should be:

******************
:10284000B984C884DF80EE80FD800C811B81AA8161
:10285000B981CE0FD11D0FB6F894DEBF0FBECDBF2C
:08286000ED010895F894FFCF8B
:10286800444D0A000A2023232323236A756D7069C7
:102878006E6720746F20626F6F746C6F616465722D
******************

To overcome this, you can read the hex file into a HEX-editor (I use ponyprog 2.08d for this: open device file & save device file) and save again. OR you can submit a solution :-)
A line with less than 16 words is handled correct.




vscp_firmware.c to be modified to remain compatible with this old & deprecated bootloader:
       case VSCP_TYPE_PROTOCOL_ENTER_BOOT_LOADER:
/*
            if ((vscp_nickname == vscp_imsg.data[ 0 ]) &&
                // byte 1 contains algorithm. Handle in callback.
                (vscp_getGUID(0) == vscp_imsg.data[ 2 ]) &&
                (vscp_getGUID(3) == vscp_imsg.data[ 3 ]) &&
                (vscp_getGUID(5) == vscp_imsg.data[ 4 ]) &&
                (vscp_getGUID(7) == vscp_imsg.data[ 5 ]) &&
                (((vscp_page_select >> 8) & 0xff) == vscp_imsg.data[ 6 ]) &&
                ((vscp_page_select & 0xff) == vscp_imsg.data[ 7 ])) {
*/
                if ((vscp_nickname == vscp_imsg.data[ 0 ])){
				vscp_goBootloaderMode( vscp_imsg.data[ 1 ] );
            }
            break;
--------------------------------------------------------------------------
 *This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.

 

