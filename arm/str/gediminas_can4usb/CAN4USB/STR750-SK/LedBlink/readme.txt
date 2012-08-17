########################################################################
#
#                         LedBlink.eww
#
#                                                   $Revision: 15135 $
#
########################################################################


DESCRIPTION
===========
 This example project shows how to use the IAR Embedded Workbench
for ARM to develop code for the IAR STR750-SK board. It shows basic
use of parallel I/O, timer and interrupt controller.

It starts by blinking the red STAT LED with period of 1 second.

COMPATIBILITY
=============
The project is compatible with the IAR STR750-SK board.

GETTING STARTED
===============
Start the IAR Embedded Workbench for ARM.
Select File->Open->Workspace...
Open the workspace file


JTAG
====
Build the project.
Select Project->Options...
In the category field select Debugger.
In the setup tab set driver to Macraigor, J-Link or RDI depending on your JTAG hardware.
Start the debugger.
The board may require special setup in a Debugger macro before the code is downloaded
by the debugger.

Jumpers:
 PWR_SEL   - depend of power source
 VRE       - Absence
 BOOT0     - 0
 BOOT1     - 0
