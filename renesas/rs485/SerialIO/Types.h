/******************************************************************************
*
* ACS SOFTWARE
*
*------------------------------------------------------------------------------
* Copyright(c)1997-2011 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.
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
* types.h - ACS Data Type definitions
*------------------------------------------------------------------------------
* written by:
*	Steven J. Ackerman
*	Consultant
*	ACS
*	6233 E. Sawgrass Rd.
*	Sarasota, FL 34240
*
*   date		Comments
* --------------------------
* 12-31-97	First created.
*******************************************************************************
*/

#ifndef  _TYPES_H_
#define  _TYPES_H_

/* datatypes */
typedef unsigned long	ULONG;
typedef	signed long		LONG;
typedef	unsigned int	UINT;
typedef signed int		SINT;
typedef unsigned short	USHORT;
typedef unsigned short int WORD;
typedef	signed short	SHORT;
typedef unsigned char	BYTE;
typedef	signed char		SBYTE;

typedef unsigned char	BOOLEAN;
#define	FALSE	0
#define	TRUE	1

/* bit definitions */
#define B0		0x01
#define B1		0x02
#define B2		0x04
#define B3		0x08
#define B4		0x10
#define B5		0x20
#define B6		0x40
#define B7		0x80

#endif	/* _TYPES_H_ */

