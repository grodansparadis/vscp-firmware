########################################################################
#
#                           MassStorage.eww
#
# $Revision: 15135 $
#
########################################################################

DESCRIPTION
===========

   This example project shows how to use the IAR Embedded Workbench for ARM
  to develop code for the IAR-STR750-SK evaluation board. It implements a 
  MMC/SD card drive. The first free drive letters will be used. For example, 
  if your PC configuration includes two hard disk partitions (in C:\ and D:\) 
  and a CD-ROM drive (in E:\), the memory card drive will appear as F:\. 
   The MassStorage example project works standalone on the IAR-STR750-SK board.

COMPATIBILITY
=============

  The MassStorage example project is compatible with, and works as a
  standalone project, on the IAR-STR750-SK evaluation board. By default,
  the project is configured to use the J-Link JTAG interface.

CONFIGURATION
=============

  1) Make sure that the following jumpers are correctly configured on the
     IAR-STR750-SK evaluation board:

	PWR_SEL   	- depend of power source
	VRE       	- Absence
	BOOT0     	- 0
	BOOT1     	- 0
	SPI1_NSS/AIN5   - SPI1_NSS
	Jumper array J5 - 2-3 (near prototype area)
	Jumper array J6 - 2-3 (near prototype area)
        
  2) Use a USB cable to connect the evaluation board to your PC. The first
     time a USB device is connected to your computer, Windows will
     automatically load the proper device driver. In this case, the USB
     mass storage device driver will be loaded.

GETTING STARTED
===============

  1) Start the IAR Embedded Workbench for ARM.

  2) Select File->Open->Workspace...
     Open the following workspace:

     <installation-root>\arm\examples\ST\STR75x\STR750-SK\
     MassStorage\MassStorage.eww

  3) Run the program.

     The MassStorage application is downloaded to the flash memory on the
     evaluation board and executed. Also note that, during board operation,
     the LCD backlight will indicate drive activity.
