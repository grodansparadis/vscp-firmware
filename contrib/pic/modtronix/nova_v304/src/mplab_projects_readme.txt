********************************************************
MPLAB Projects
********************************************************

MPLAB projects for all versions of the SBC65EC are included in this folder.
The MPLAB file name indicates which C Compiler is used to build it, if it is
meant for a board with a bootloader, and for what hardware version board it is.

The trailing "_hwxyy" part of the file name indicates for what hardware version
board the file is meant (see below for details on identifying the version number
of a board). For example, the "websrvr_ht_hw211.mcp" file will be for a hardware
V2.11 board.

The following MPLAB project files are included for all SBC65EC board versions:
- <i>websrvr_ht_hwxyy.hex</i> - Project file for use with bootloader, compiled with
  the HiTech compiler. The firmware hex file is created by opening and building
  this MPLAB project file.
- <i>websrvr_mc_hwxyy.hex</i> - Project file for use with bootloader, compiled with
  the MPLAB C18 compiler. The firmware hex file is created by opening and building
  this MPLAB project file.
- <i>websrvr_nobl_ht_hwxyy.hex</i> - Project file for use with bootloader, compiled
  with the HiTech compiler. The firmware hex file is created by opening and building
  this MPLAB project file.
- <i>websrvr_nobl_mc_hwxyy.hex</i> - Project file for use with bootloader, compiled
  with the MPLAB C18 compiler. The firmware hex file is created by opening and building
  this MPLAB project file.

To create hex files for any of the above mentioned MPLAB project, open it in MPLAB,
and click on the "Build All" toolbar button. This will compile and link the project,
and create the hex file.



********************************************************
Identifying the hardware version of a board
********************************************************

There are a couple of different hardware versions of the @boardname. The current version is hardare V2.11.
The following is a complete list of all version boards, and information how to identify them:

- <b>Hardware V2.11</b>: Label on front of board with "HW: V2.11". The PCB is marked "SBC65EC REV2", and is
assembled with the PIC18F6627 chip. There are two small surface mount resistors soldered onto the back of the
board, and space for a 8 pin surface mount chip (Ramtron FRAM). This board is programmed with the bootloader.

- <b>Hardware V2.01</b> with bootloader: Label on front of board with "HW: V2.01". The PCB is marked "SBC65EC REV2", and is
assembled with the PIC18F6621 chip. There are two small surface mount resistors soldered onto the back of the
board, and space for a 8 pin surface mount chip (Ramtron FRAM).	This board is programmed with the bootloader.

- <b>Hardware V2.01</b> without bootloader: NO Label on front of board! The PCB is marked "SBC65EC REV2", and is
assembled with the PIC18F6621 chip. There are two small surface mount resistors soldered onto the back of the
board, and space for a 8 pin surface mount chip (Ramtron FRAM).

- <b>Hardware V1.00</b> without bootloader: NO Label on front of board! The PCB is marked "SBC65EC REV1", and is
assembled with the PIC18F6621 chip. There are NO components on the back of the board.
