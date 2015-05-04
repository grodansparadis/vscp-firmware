This project implements a latchnig relay draive on an Atmega AT90CAN32 device. 

Pulse is sent after receiving Open (0x03) or Close (0x04) event.

Code is built using the GCC (AVR GCC) compiler.

Project configurations are done in the file vscp_projdefs.h  Also paths to include folders is 
in the Makefile and you probably have to change the paths.

If you port this project to use another compiler please use the vscp_compiler.h header to add 
compiler specific stuff. This file and the vscp_projdefs.h should be included in every file
of the project.

The mdf file describes the VSCP functionality.

SW0 - Init button. Press to initiate node and start nickname process.

LED0 - Status LED. Will blink during init process, stays steady on during operation.  

Serial channel, 38400-8N1 is used for debug messages.

Decision matrix is implemented.
No rows in the DM

Actions
=======
0x00 - No operation.

More information to be found at http://www.vscp.org/wiki/doku.php/modules/atmel_temp_humid