/******************************************************************************                
*                                                                                              
* ACS SOFTWARE                                                                      
*                                                                                              
*------------------------------------------------------------------------------                
* Copyright(c)2013-2014 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.                                  
*                                                                                              
*       This Information is Proprietary to Ackerman Computer Sciences, Sarasota,               
* Florida (ACS) and MAY NOT be copied by any method or incorporated into another               
* program without the express written consent of ACS. This Information or any                  
* portion thereof remains the property of ACS. The Information contained herein                
* is believed to be accurate and ACS assumes no responsibility or liability for                
* its use in any way and conveys no license or title under any patent or                       
* copyright and makes no representation or warranty that this Information is                   
* free from patent or copyright infringement.                                                  
*------------------------------------------------------------------------------                
* SerialIO.h - ACS Serial I/O Software Function definitions
*------------------------------------------------------------------------------                
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  6-04-13	First written for Serial IO.
*******************************************************************************                
*/                                                                                             

#ifndef	_SERIALIO_H_
#define	_SERIALIO_H_

// uncomment the following to enable 18.432MHz clock instead of 20MHz
#define	CLOCK_18_432MHz

#define	MAX_CONTACTS		56

#define	FIRMWARE_VERSION_MAJOR		0
#define	FIRMWARE_VERSION_MINOR		0
#define	FIRMWARE_VERSION_SUBMINOR	1

#include "sfr_r8m12a.h"	/* R8CM12A Special Function Registers */
#include "Types.h"		/* ACS datatypes */
#include "Defines.h"	/* */
#include "IO.h"
#include "Macros.h"

BYTE ReadDipSwitch(void);
void SetLEDs(BYTE leds);
void main(void);

#endif	// _SERIALIO_H_

