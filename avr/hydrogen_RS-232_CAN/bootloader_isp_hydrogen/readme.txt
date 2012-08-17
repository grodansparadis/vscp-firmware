The code is a derivative of ATMELS AVR109 in System programming software that allows reprogramming of 
ATMEGA parts over serial port.All files are assembler using ATMELS syntax. Original "C" has been left in 
as comments but be aware the code has been heavily optimised by hand so it doesn't tie up very well.

The code as written looks for a low on PD4. If this is low, it runs bootloader. Otherwise it runs 
application code at $0000. Baud rate it fixed at 19200 with 3.69Mhz clock. All of this can easily be 
changed.

It is compiled for the ATmega 163 and uses about 206 words. This is much less that the atmel version and 
leaves enough space to add a more eleborate method of starting the bootloader/application. As with AVT109 
it works with AVR prog software (also part of Studio but also available separately)

Put all files in same directory. cstartup.s90 is the root file. This will include the others. It will 
assembler under AVR studio.

Make sure you set fuses to give 512 byte boot block. The ORG 0x1F00 in startup expects the code to reside 
at $1f00.

I have tried this on ATmega part with STK500. The only issue I have found is that it doesn't not seem to 
read PD4 correctly if the RESET button is pushed. (It is ok from power up).


Andy Hutchinson




