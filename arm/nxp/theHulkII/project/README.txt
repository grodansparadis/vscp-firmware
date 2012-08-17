The source for "the HULK II" a furnace and electrical water heater system. It is called 
"the HULK" beacuse it is really gigant (but now green though) when in delivers its 25KW.

The mainline code should run in either Thumb or ARM mode.  Certain
files, however, must be compiled in ARM mode.  See the Makefile for
specifics.

armVIC.* - interface routines for setting up and controlling the
various interrupt modes present on the ARM processor.  It was going to
have routines for interacting with the VIC, but not yet.

uart.* - interface routines for setting up and using the two UARTs
present on the LPC processors.  Polled and Interrupt modes are
supported, but MODEM control is not implemented for UART2.

armISR.* - interrupt service routines for the two UARTs.

sysTime.* - interface routines for setting up and using a elapsed time
system timer.  This version is good up to a little over 7 mins. in
100nSec steps.  It does not use interrupts.

lpc???.h - register address location files.  Still in work but are OK
for this project.

Because of historical events, the Makefile is in nmake format.  Yes, I
know, one day I will need to convert everything to gnu make format.
But for now...

22Mar2004 - updated to disable global interrupts around handling of the
uart interrupt masks.

30Jul2004 - fixed incorrect FIFO Control Register bit positons in
lpcUART.h

30Jul2004 - fixed incorrect Line Control Register bit definitions
(Stop Bits) in lpcUART.h

Enjoy!
-Bill Knight
R O SoftWare
BillK@rosw.com

Changes by Martin Thomas <eversmith(at)heizung-thomas.de>

28Oct2004
- adapted to GNU-makefile (THUMB-Mode not tested)
- adapted to OLIMEX LPC-P2106-Board
- added button-detection with debouncing
- minor "cosmetic" changes


Changes by Ake Hedman <akhe@eurosource.se>
2005-10-20 Started the HULK II

