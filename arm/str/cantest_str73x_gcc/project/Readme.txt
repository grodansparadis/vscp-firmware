Simple CAN test sample
====================

Ake Hedman, D of Scandinavia akhe@dofscandinavia.com, http://www.dofscandinavia.com

This file is built for the IAR str73x test board.

The sample code send a CAN frame every 500 ms and display a "RECEIVE CAN MESSAGE"
on the second line of the LCD if a CAN message with id 0x123, length=4 and 
data=1,2,4,8 is received. If another msg is received the line is cleared.

CAN bitrate is 500 kbps.

Based on a sample from ST
