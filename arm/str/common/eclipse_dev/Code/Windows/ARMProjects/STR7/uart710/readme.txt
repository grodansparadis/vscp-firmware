*** This README.TXT file comes with the STR71x Software Library ***

Example description
===================
This example demonstrates how to communicate with a PC using RS232 protocol.
The main program waits for the user to send 4 ASCII characters from the PC, which will
return them to the PC.
The communication is performed using the «Hyperterminal» application under Windows.

Directory contents
==================
scat.scf    Scatter file
71x_conf.h  Library Configuration file
71x_it.c    Interrupt handlers
main.c      Main program

How to use it
=============
In order to make the program work, you must do the following :
- Open «Hyperterminal» application and configure the right COM port as following :
  * 9600 bps
  * 8 data bit
  * no parity
  * no flow control

(C) COPYRIGHT 2003 STMicroelectronics
