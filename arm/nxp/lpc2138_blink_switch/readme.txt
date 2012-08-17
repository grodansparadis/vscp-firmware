//
//             Philips LPC2138 LED/Switch Example
//               for the Keil MCB-2130 board
//
// This example demonstrates writing to and reading from 
// the GPIO port. It does:
// (1) flash the firts LED 10 times
// (2) wait for key-press (INT1), turn on LED of key is pressed
//
// by Martin THOMAS, Kaiserslautern, Germany
// (eversmith@heizung-thomas.de)
// Ported to the MCB2120 by Ake Hedman, Sweden
// (akhe@eurosource.se)


This project has been my "ice-breaker" to start with the
ARM7-architecture on an MCB-2130 demo-board. A lot of information
and code used for this example has been found in the internet.

There are some copyright-notes in some of the files, but 
everything has been downloaded from freely available sites.
I do not own any commercial software for this controller from
which I could have copied code. So I hope there is no copyright 
violation. Please inform me if you think your copyrights have
been violated an I will take this project off-line.

credits:
- WinARM-200508 has been used
- The makefile is based on an nmake-makefile from R O SoftWare 
  and the WINAVR makefile-sample by E. Weddingtion, J. Wunsch et. al.
  I've added/changed/ported a lot so don't expect everything is 
  correct - it works for me in this project.
- The linker-skripts and the startup-code is also from a sample
  made by R O Software. IRC it has been found in the yahoo LPC-group 
  file-archive.
- The lpc21xx.h file is from the gnuarm-project (part of WinARM) 
  It differs from other files with the same name
- The main-code was based on a sample from Rowley Associates Limited
  but there is not much (nothing?) left from the original code.

  
Feedback welcome.

Cheers,
Martin

