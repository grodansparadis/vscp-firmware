This project is aimed to act as a base for home-automation
It was based on the work  by Ake Hedman, eurosource

Hardware supported: (specified in makefile128 or makefile32)
olimex AVRCAN -board 
custom hardware (AAmini)

you can add additional hardware in hardware.c

Currently used setup is AVRstudio 4.18 together with WINAVR 20090313


Serial channel, 9600-8N1 is used for debug messages.
For activating this, define PRINT_CAN_EVENTS in vscp_projdefs.h

8 DM rows are available.

Actions
=======
Toggle output.
Turn ON output.
Turn OFF output.




--------------
The file AAmini.hexample can be uploaded to a olimex AVRCAN, or similar board with 16Mhz clock as precompiled HEX-file