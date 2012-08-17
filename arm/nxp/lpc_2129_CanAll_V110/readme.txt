** Warning: this is a modified Version - see EOT **

LPC_CANAll example program by Embedded Systems Academy, Inc.
For Philips Semiconductors and Keil Software
www.philipsmcu.com, www.keil.com, www.esacademy.com
January 5th, 2005 by ESAcademy
This version updated to be conform with all current 
errata sheets.

This example program implements a set of CAN drivers for the 
Philips LPC2000 ARM7 derivatives with CAN interfaces.

This driver does not make usage of the hardware acceptance
filters, it receives ALL CAN messages on the bus.
			
Tested with Keil's "DKARMg" tool set and the 
MCB2100 Development Board: www.keil.com/mcb2100

--- 

14. May 2005:
Modified by Martin Thomas, Kaiserslautern, Germany
<eversmith@heizung-thomas.de>

- WinARM startup-code and linker-scripts
- WinARM makefile
- Low-level IRQ-functions from R O Software
- added some volatiles to avoid compiler warnings
- replaced unsigned int with unsigned long (better: uint32_t)
- changed LED handling

Tested with WinARM 4/05 (arm-elf-gcc 4.0.0) 
(http://www.siwawi.arubi.uni-kl.de/avr_projects/arm_projects/)
and the Olimex LPC-P2129 Development Board (Philips LPC2129)
