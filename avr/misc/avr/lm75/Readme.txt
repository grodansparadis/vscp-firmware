/*********************************************************************************************************
 Title  :   C library for LM75 I2C bus temperature sensor 
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      2/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/

I use it to drive the LM75 I2C bus temperature sensor.
Configuration is done in the lm75.h file

The temperature can be negative or positive and the accuracy is 1 degree Celsius
The LM75 is sending the temperature over the I2C bus in 0,5 degrees steps 
but there is no point to read the whole value from the temperature register
since the LM75 sensor accuracy is 2~3 degrees Celsius over the entire range.

Remember to give the LM75 an I2C bus address using the appropriate pins of the LM75 IC

I hope it can help someone.
Chris.
