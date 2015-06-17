This project is a very simple project that show the use of VSCP on the AT90CAN128 chip. 
Code is built using AVR studio. It is tested on the AVR STK-501 board or the Olimex 
AT90CAN128 board (https://www.olimex.com/Products/AVR/Development/AVR-CAN/).

Project configurations are done in the file vscp_projdefs.h  Also paths to include folders is 
in the Makefile and with a moves source tree you probably have to change the paths.

If you port this project to use another compiler please use the vscp_compiler.h header to add 
compiler specific stuff. This file and the vscp_projdefs.h should be included in every file
of the project.

The MDF file describes the VSCP functionality in full.


SW0 - Init. button. Press to initiate node and start nickname process.
SW1 - Send ON event, Zone set by register 0, Subzone set by register 3. 
SW2 - Send ON event, Zone set by register 0, Subzone set by register 4.
SW3 - Send ON event, Zone set by register 0, Subzone set by register 5.
SW4 - Send ON event, Zone set by register 0, Subzone set by register 6.
SW5 - Send ON event, Zone set by register 0, Subzone set by register 7.
SW6 - Send ON event, Zone set by register 0, Subzone set by register 8.
SW7 - Send ON event, Zone set by register 0, Subzone set by register 9.

LED0 - Status LED. Will blink during init. process.  
LED1 - LED 1 can be controlled.
LED2 - LED 2 can be controlled.
LED3 - LED 3 can be controlled.
LED4 - LED 4 can be controlled.
LED5 - LED 5 can be controlled.
LED6 - LED 6 can be controlled.
LED7 - LED 7 can be controlled.


Serial channel, 9600-8N1 is used for debug messages.

Two DM rows are available.

Actions
=======
0x00 - No operation.
0x01 - Light LED's.
0x02 - Write "Hello World" on serial channel.
