This project is aimed to act as a base for home-automation
It was based on the work  by Ake Hedman, eurosource

Hardware supported: custom hardware (AAmega) Schematics are available on VSCP wiki

All hardware related routines are placed in hardware.h / .c

Currently used setup is AVRstudio7


Serial channel, 9600-8N1 is used for debug messages.
For activating this, define PRINT_CAN_EVENTS (or other) in vscp_compiler.h

11 DM rows are available.

Actions
=======
Toggle output.
Turn ON output.
Turn OFF output.
Start Timers
Enable / disable DM row

