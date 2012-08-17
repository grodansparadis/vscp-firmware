   
   4X4 KEYBOARD DRIVER FOR GCC
   Written: by Chris Efstathiou
   Date:    20/Aug/2002 
   TARGET:  All AVR MPU's 
   Info:    This code is FREE. There are no copyright issues with this code.

This code is a hex (4x4) keybord driver for the AVR MPU
and is written entirely in GCC. 
It does not use any interrupts or timers.
 
Feel free to modify any part you wish but please
give me a note so i can get feedback.

TO CONFIGURE THE KEYBOARD DRIVER 
edit hex_keyb.h header file.
Instructions for setting up the driver are provided
within the hex_keyb.h header file.
In case you want to use your lcd driver
you can define the lcd functions needed
by the driver in the hex_keyb.h header file
(They are already defined for my LCD driver).


The supplied LCD driver is set for 2 pin operation
so if you want the 6 pin operation remember to edit
the lcd_io.h header file.

NOTICE: 
When the test_hex_keyb test program asks you for a password
the password is hardcoded to be 1234


