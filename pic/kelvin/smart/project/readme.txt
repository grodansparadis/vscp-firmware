Readme file for the Kelvin SHT11 project. This project is also the base for a VSCP implemention
on a Microchip 18F PIC device.

Version 0.03 2004-12-24

PATHS
=====

Four paths must be setup for the project to build

The full path to the project folder so project specific config.h and projdefs.h files can
be found
e.g. D:\dev\vscp\firmware\pic\Kelvin\Smart2\project\

The full path to the vscp top level firmware common folder
e.g. D:/dev/vscp/firmware/common 

The full path to the vscp pic common folder
e.g. D:/dev/vscp/firmware/pic/common

And standard includes for the compiler selected.
e.g. C:/MCC18/h

VERSIONS
========
This project ia available in three versions for each processor. One have an "i" at the end meaning that it 
is for the ICD2 debugger.

"_reloc" projects are built for use with the bootloader. The bootloader occupies space between 0x000 and 0x1ff. 
Vectors are relocated to 0x200. This is managed in the linker scripts. Also the C initialization code has 
been changed to start code at the relocated vector. This also means that the bootloader has to be programmed
into the chip before the working with the project. It also means that the environment must be configured 
*not* to erase the chip before programming it.

If no bootloader is required it is OK to use the standard C startup code, a standard linker description. In this 
case the absolute vector address for the low priority interrup in main.c  must be changed back froo 0x218 to 0x18.



Steps you should go through to adopt this file for your own VSCP project.
=========================================================================

1.) The frequency for your system may be different. This will affect the CAN bit rate settings
which are defined in the biginning of the can18f.c file. Here you also find the filter/mask 
defines if you need to change these.

Timer0 is used as a 1 ms timer for the VSCP functionality. You should adopt the timer reload value 
in the main.h file to your system.

2.) The GUID (Glocal Unique ID) is defined in the beginning of vscp.c This number should be 
different for each piece of equiment you produce. Regard it is as the device serial number. It
can be moved to EEPROM if that is found more convinient. In that case the vscp_rcv_readreg 
procedure in vscp.c also has to be changed.

3.) The device URL is defined in the beginning of the vscp.c file. This is an URL without the
initial "http://" that should point to a XML file that give information about your piece of 
equipment.

4.) The manufacturer id is also defined in the beginning of the vscp.c file. You can use this
id in any way you like.

In addition to this you may use different pins for the INIT button (RC0 today) and the 
status LED (RC1) today. Changes affect the main interrupt vector.


Available builds
================
- kelvin_smart2_258i.mcw is for a special 18F258 board *with* ICD2 support.
- kelvin_smart2_258.mcw is for 18F258 board *without* bootloader.
- kelvin_smart2_258_reloc.mcw is for 18F258 board *with* bootloader.
- kelvin_smart2_2580i.mcw is for a special 18F2580 board *with* ICD2 support.
- kelvin_smart2_2580.mcw is for 18F2580 board *without* bootloader.
- kelvin_smart2_2580_reloc.mcw is for 18F2580 board *with* bootloader.

VSCP (Very Simple Control Protocol) 
http://www.vscp.org

<akhe@eurosource.se>

