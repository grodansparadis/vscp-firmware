/******************************************************************************
*
* ACS SOFTWARE
*
*------------------------------------------------------------------------------
* Copyright(c)2013-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.
*
*	This Information is Proprietary to Ackerman Computer Sciences, Sarasota,
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another
* program without the express written consent of ACS. This Information or any
* portion thereof remains the property of ACS. The Information contained herein
* is believed to be accurate and ACS assumeS no responsibility or liability for
* its use in any way and conveys no license or title under any patent or 
* copyright and makes no representation or warranty that this Information is
* free from patent or copyright infringement.
*------------------------------------------------------------------------------
* Globals.h - ACS Serial I/O Software Global Variables
*------------------------------------------------------------------------------
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  6-06-13	First written.
*******************************************************************************
*/

#ifndef	_GLOBALS_H_

#define	_GLOBALS_H_

#ifdef GLOBALS_OWNER
	#define GLOBAL
#else
	#define	GLOBAL extern
#endif		

#define CONTACT_MEMORY_ALLOCATION	((MAX_CONTACTS / 8) + (MAX_CONTACTS & 7))

/* define Global variables */

GLOBAL unsigned char ContactClosures[CONTACT_MEMORY_ALLOCATION], ContactOpenings[CONTACT_MEMORY_ALLOCATION];
GLOBAL unsigned char Contacts[CONTACT_MEMORY_ALLOCATION], OldContacts[CONTACT_MEMORY_ALLOCATION], OlderContacts[CONTACT_MEMORY_ALLOCATION];
GLOBAL unsigned char OutputContacts[CONTACT_MEMORY_ALLOCATION];

GLOBAL unsigned char vscp_sendFrameTimer;

#ifdef GLOBALS_OWNER
/* define initialized variables */

GLOBAL const unsigned char GUID[16] = {0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
GLOBAL const unsigned char MDF_URL[32] = {"www.acscontrol.com/SerialIO.xml"};

#else
/* define initialized variables without initializers */

GLOBAL const unsigned char GUID[16];
GLOBAL const unsigned char MDF_URL[32];

#endif

#endif	/* _GLOBALS_H_ */
