Readme for the SweetBox project.

To compile this project please download the VSCP source tree and execute 
"setenv_vscp.bat" before compiling. Insructions on how to get the VSCP source 
can be found at http://www.vscp.org

Also FreeRTOS is needed by the project. Please download 5.0.4 or later version from 
www.freertos.org  If plcaed at another location than in the C root the include paths
of the project need to be changed.


Queues
======
queueHMI - HMI related commands are sent on this queue. It is described in taskHMI.h


Resources used
==============

Timer 1		-	FreeRTOS ticks.
Timer 2		-	Humidity device timer.


Devices hooked to communication channels
========================================

SPI1
====
- Ethernet Controller - ENV28J80.
- MiWi Controller - MRF24J40MA.

SPI2
====
- CAN controller - MCP2515
- Volume control - MCP42010

I2C1
====
- Temperature sensor, MCP9803		3 (7 on proto)	Address = 0x4F. (0b0100 1111)
- EEPROM 24A1025					1				Address = 0x51 (low bank) / 0x55 (high bank).
- EEPROM 24A1025					2				Address = 0x52 (low bank) / 0x56 (high bank).

I2C2
====
- PCA9635 1							0
- PCA9635 2							2
- PCA9635 3							3
- PCA9635 4							4
- PCA9635 5							5
- PCA9635 6							6
						