This project is template to be used as a starting point when you want to create a VSCP project using an Atmega AT90CAN32 or AT90CANXXX device. 

Code is built using the GCC (AVR GCC) compiler. It is tested on a standard bread bord.

Project configurations are done in the file vscp_projdefs.h  Also paths to include folders is 
in the Makefile and with a moves source tree you probably have to change the paths.

If you port this project to use another compiler please use the vscp_compiler.h header to add 
compiler specific stuff. This file and the vscp_projdefs.h should be included in every file
of the project.

The mdf file describes the VSCP functionality in full.


SW0 - Init button. Press to initiate node and start nickname process.

LED0 - Status LED. Will blink during init process.  

Serial channel, 38400-8N1 is used for debug messages.

Decision matrix is implemented.
No rows in the DM

Actions
=======
0x00 - No operation.
