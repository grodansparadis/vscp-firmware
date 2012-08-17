This project is aimed to act as a base for home-automation
It was based on the work  by Ake Hedman, eurosource

Hardware supported: custom hardware (AAmega) Schematics are available, but due to a few minor bugs, i'm not spreading them to the public yet. If you are interested, i'll supply them after a simple request.

All hardware related routines are placed in hardware.h / .c

Currently used setup is AVRstudio 4.18 together with WINAVR 20090313


Serial channel, 9600-8N1 is used for debug messages.
For activating this, define PRINT_CAN_EVENTS in vscp_projdefs.h

11 DM rows are available.

Actions
=======
Toggle output.
Turn ON output.
Turn OFF output.


Bugs:
=====
-bootloader doesn't work as it should


Todo list:
==========
-add more DM rows
-solve bootloader
