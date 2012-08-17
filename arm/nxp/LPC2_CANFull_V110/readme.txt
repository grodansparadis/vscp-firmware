LPC_FullCAN example program by Embedded Systems Academy, Inc.
For Philips Semiconductors and Keil Software
www.philipsmcu.com, www.keil.com, www.esacademy.com
January 5th, 2005 by ESAcademy
This version implements the latest changes suggested by
current errata sheets

This example program implements a set of CAN drivers for the 
Philips LPC2000 ARM7 derivatives with CAN interfaces.

The Philips LPC2000 CAN devices offer a Full CAN mode that
on the receiving side automatically store received messages
into destination buffers. This Full CAN mode is not fully
functional in the first revisions of this chip.

This driver implements a compatible Full CAN mode in software:
The Full CAN Buffers are maintained and updated by CAN receive
interrupts.
			
Tested with Keil's "DKARMg" tool set and the 
MCB2100 Development Board: www.keil.com/mcb2100
