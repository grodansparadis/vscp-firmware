/**
 * @brief           FRAM Module for reading and writing from and to the FRAM
 * @file            fram.h
 * @author          <a href="www.ziggurat29.com">Ziggurat29</a>
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_sys_fram
 *
 * @section description Description
 **********************************
 * This module contains code for reading and writing from and to the FRAM, like the FM64640 or FM64256.
 *
 * @section fram_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required defines should be uncommended, and the rest commented out.
 @code
//*********************************************************************
//--------------------  FRAM System Configuration --------------------
//*********************************************************************
//this may work with either a 'true' hardware SPI, or a 'bitbang' software
//SPI implementation.  If you uncomment the following, the software
//implementation will be used.
//#define SOFTWARE_SPI 1

//the following defines which IO pins are used for the various SPI
//signals to the FRAM.  You can change them to suit your configuration.
//IO pin definitions;
//f.7 is CS
//d.4 is SO
//d.5 is SI
//d.6 is CK
#define FRAM_SPI_BIT_CS     PORTF_RF7
#define FRAM_SPI_TRI_CS     TRISF_RF7

#define FRAM_SPI_BIT_SI     PORTD_RD5
#define FRAM_SPI_TRI_SI     TRISD_RD5

#define FRAM_SPI_BIT_SO     PORTD_RD4
#define FRAM_SPI_TRI_SO     TRISD_RD4

#define FRAM_SPI_BIT_SCK    PORTD_RD6
#define FRAM_SPI_TRI_SCK    TRISD_RD6

//clock speed (only relevant for hardware SPI)
//SPI_FOSC_xx depends on device and clock speed
//using SPI_FOSC_16 will provide a 2.5 MHz clock (for FM25640-G)
//using SPI_FOSC_4 will provide a 10 MHz clock (for FM25256-G)
#define SPI_ROLE SPI_FOSC_4
//#define SPI_ROLE SPI_FOSC_16


 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on code written
 * by Ziggurat29. For details, see www.ziggurat29.com
 * The code may be modified and can be used free of charge for non commercial and
 * commercial applications. All rights are reserved. 
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-11-16, David G Lemley:
 *    - Initial version (ver 1.0.0)
 * 2007-04-23, David Hosken (DH):
 *    - Integrated into Modtronix TCP/IP stack
********************************************************************/



/////////////////////////////////////////////////
// FRAM - documentation module
/**
@defgroup mod_sys_fram FRAM
@ingroup mod_sys

This describes some routines provided for using the Ramtron FM25256/FM25640. These routines provide basic
storage and retrieval of data, and also a modified version of the fsee file system is provided which has
write support.


@section mod_sys_fram_intro Basic FRAM I/O

I developed the basic FRAM interface routines first.  These provide the basic capability of reading and
writing arbitrarily sized chunks of data to the FRAM at a specific location.  They work somewhat like memcpy().

If you use the FRAM routines, you must call FRAMInit() once prior to use.  This sets up the SPI parameters
and IO pin directions.

As-is, the code supports hardware SPI relevant for the SBC65EC.  If for some reason you need a software
SPI, there is an option on line 28 of FRAM.c which will cause a software implementation to be used.  Also,
the particular io pins are configured in the subsequent section if you are using a non-standard connection.
Lastly, the clock speed may be changed at line 53.  For the slower FM25640 you will need SPI_FOSC_16, and
for the faster FM25656 the default of SPI_FOSC_4 will work.

There are some additional sequential read and write routines. 
These were provided to ease porting of the file system and probably are of limited  use otherwise. They leave
the read or write transaction open so that multiple calls may be made to sequential addresses. There is a
caveat that FRAMEndWrite()/FRAMEndRead() must be called before relinquishing control back to the system
because the ethernet controller shares the same IO pins.  This is a little different than is with EE since
those pins are dedicated.


@section mod_sys_fram_fs fsFRAM File System
Thinking it would be useful to someone, I made a modified version of fsee.h, .c that implement the fsys
interface via the FRAM io routines.  Additionally, I provided implementation for the filePutByte() routine
so that writing is possible.  Writing continues from the current file position until end-of-file is reached.


@section mod_sys_fram_testing Testing

I used the out-of-box Modtronix web server as a development platform for testing.  Strictly for testing
purposes I modified the web server to only use the FRAM file system, not EE.  This made it easy for me to
exercise all the code.  In real life you probably wouldn't want to do this as EE is perfectly fine for
serving web pages.

For testing I created a page experi.htm.  This page is rewritten periodically and updated with the
current tick count.  This allowed me to easily see that file writing was working (in real life you would
use cgi variables -- it was only a test).

Some modifications were made to the project to coax it into using fsFRAM instead of fsee.  The most
interesting changes were made in mxwebsrvr.c, which is where the 'test suite' is executed.  Modifications
in that file include:
- Including appropriate headers
- performing needed initialization calls
- reading and writing test data on a periodic basis (once every 32 sec)

Modifications can be found by searching for the comment marker HHH.


@section mod_sys_fram_limitations Known Limitations

The sequential read and write methods in "fram.h" work fine, but you must complete the operation with
FRAMEndRead/Write before relinquishing control to other tasks.  This is because the IO pins are shared
with other devices.

The embedded file system on which fsee (and hence fsFRAM) are based is very simple, and several limitations
are imposed by it's design:
- There is not a facility for dynamically allocating additional space for files after an image is built. This means you will need to anticipate the file size before hand.  Do not compress it (or you will change it's real size of course).
- You cannot create new files
- You cannot delete old files
- You cannot append to the end of a file

Despite it's limitations, fsee is fine for an embedded file system. I might implement a new file system with
dynamic allocation, though it seems a bit much considering there is only 8-32k space to manage anyway.
*/

#ifndef FRAM_H
#define FRAM_H


//type of address; note implementation assumes this is 16 bits; if there is
//need to make this bigger, the read and write implementation should be reviewed
typedef WORD FRAM_ADDR;


/**
 * Call once to setup IO lines and do initial configuration of SPI (mode
 * and data rate) for subsequent
 */
void FRAMInit(void);


/**
 * Write a contiguous chunk of data to the FRAM.
 * 
 * @param addr Address
 * @param abyData
 * @param nLen
 */
void FRAMWriteArr ( FRAM_ADDR addr, BYTE* abyData, int nLen );


/**
 * Read a contiguous chunk of data to the FRAM.
 *
 * @param addr Address
 * @param abyData
 * @param nLen
 */
void FRAMReadArr ( FRAM_ADDR addr, BYTE* abyData, int nLen );


/**
 * Sets up FRAM for writing.
 *
 * Remember that the SPI IO uses the same pins as the ethernet controller (unless you change that).
 * So between the Begin() and End() calls you may _not_ do anything with the ethernet.
 *
 * @param addr   address to where to write
 */
void FRAMBeginWrite ( FRAM_ADDR addr );


/**
 * Writes given value 'byVal' at current address. Current address is set
 * by FRAMBeginWrite() and is incremented by every FRAMWrite().
 *
 * @param byVal       Value to be written
 */
void FRAMWrite ( BYTE byVal );


/**
 * End writing to FRAM.
 */
void FRAMEndWrite(void);


/**
 * Sets up FRAM for reading.
 *
 * @param addr  Address at which read is to be performed.
 */
void FRAMBeginRead ( FRAM_ADDR addr );


/**
 * Reads next byte from FRAM; internal address is incremented by one.
 *
 * @return  Read byte
 */
BYTE FRAMRead(void);


/**
 * Ends sequential read cycle.
 */
void FRAMEndRead(void);


//testing methods during development; not for use=====
#if 0
void FRAMTest(void);
void FRAMTest2(void);
#endif

#endif	//#ifndef FRAM_H
