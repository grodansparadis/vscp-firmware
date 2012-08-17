########################################################################
#
#                    VirtualCom.eww
#
#                                                   $Revision: 18265 $
#
########################################################################

DESCRIPTION
===========
 This example project shows how to use the IAR Embedded Workbench for ARM 
to develop code for the IAR STR750-SK evaluation boards. It implements USB 
CDC (Communication Device Class) device and install it like a Virtual COM 
port. The UART1 is used for physical implementation of the RS232 port.
This example can work standalone on the IAR STR750-SK board.

COMPATIBILITY
=============
The project is compatible with the IAR STR750-SK evaluation board.

The project is by default configured to use the J-Link JTAG interface.


GETTING STARTED
===============
Start the IAR Embedded Workbench for ARM.
Select File->Open->Workspace...
Open the workspace file

   - put the jumpers:
	PWR_SEL   - depend of power source
	VRE       - Absence
	BOOT0     - 0
	BOOT1     - 0
	
   - run program
   - use the USB cable to connect the PC to USB of the Board
The first time the device is connected to the computer, Windows will 
load the driver for identified device. The Virtual COM port driver is 
in the $PROJ_DIR$\VirCOM Driver XP\.

CONFIGURATION
=============
The application is downloaded to the flash.
