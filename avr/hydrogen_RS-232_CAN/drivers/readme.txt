
/**********************************************************************************
 Title  :   DRIVER PACK VERSION 4.45
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      22/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  Read the readme.txt of each driver located in each drivers directory.

***********************************************************************************/

DRIVERS FOR: 1)  HD44780 LCD USING 6 OR 2 I/O LINES AND MULTIPLE LCD UNITS
             2)  UART, SINGLE OR DUAL
             3)  SOFTWARE UART (MIXED C AND ASSEMBLY) TTL & RS232 CAPABLE
             4)  ANALOG TO DIGITAL CONVERTER
             5)  HEXADECIMAL MATRIX KEYBORD WITH EDITOR
             6)  I2C MASTER
             7)  DELAY FUNCTIONS IN ASM
             8)  SRF08 ULTRASONIC RANGING MODULE
             9)  SRF04 ULTRASONIC RANGING MODULE
             10) LM75 temperature sensor driver

The drivers have a readme.txt file inside each directory that you should read.
Driver information can be found in the above named file and inside the .h files.

To compile the code copy one of the supplied avr-make file inside the include
directory.
The avr-make files supplied are in no way altered by me.
There are two avr-make files supplied. One is for the standard gcc setup
and the other is for use with the elf2coff converter so a coff file can be generated.

The drivers have been tested with the standard gcc version as well with the
modified one that compiles code for the MEGA128.
Also they have been tested with the use of the modified io headers that permit
assignments and the new release of avrgcc 3.2

 
WARNING: You might experience errors with the elf2coff converter and AVRGCC 3.2
         There is nothing i can do about it. The code is working fine though.

Credits to a fiend named Daniel for his help and tips on the I2C driver.
I dont write his full name or email in order not to make him a possible spam victim.
 
I release that code with no warranty and copyright issues.
This is the best i can do for now and believe me i have tried hard.
so please dont shoot at me if something is wrong.

I also grand you the right to change, alter, modify anything you want.
Also i seek no credits, so feel free to put your name in the files if you
modify them (to the better i hope!).

I write code just for the fun of it.
Other people helped me in the beggining of my AVR programming learning
by posting free code so the only thing i want is to help newcomers
or experienced (i hope!) AVR users if possible.

Chris Efstathiou.



       


