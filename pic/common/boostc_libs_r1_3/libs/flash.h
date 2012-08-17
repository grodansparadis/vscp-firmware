/*************************************************************
* Flash program memory interface for PIC16F87X devices 
*
* Allows for read and write of the program memory. Be aware
* that write operations by design are performed by four bytes 
* at once. 
*
* Warning! Interrupts are disabled during write !
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#ifndef _FLASH_H_
#define _FLASH_H_

#include <system.h>

short flash_read(short address);
void  flash_dump(short startaddr, short stopaddr);
void  flash_loadbuffer(short data);
void  flash_write(short address);

#endif _FLASH_H_
