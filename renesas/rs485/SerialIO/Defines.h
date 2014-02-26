/******************************************************************************
*
* ACS SOFTWARE
*
*------------------------------------------------------------------------------
* Copyright(c)1993-2013 by ACS, Sarasota, FL. ALL RIGHTS RESERVED.
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
* Defines.h - ACS Serial I/O Software Global Defines
*------------------------------------------------------------------------------
* written by:                                                                                  
*		Steven J. Ackerman
*		Consultant
*		ACS, Sarasota, FL
*                                         
*  date		comments                                                               
* --------------------------------------------                                                 
*  6-04-13	First written.
*******************************************************************************
*/

/* cpu clock */
#ifdef	CLOCK_18_432MHz
#define	CPU_CLOCK	18432000ul		/* 18.432MHz */
#else
#define	CPU_CLOCK	20000000ul		/* 20.000MHz */
#endif

