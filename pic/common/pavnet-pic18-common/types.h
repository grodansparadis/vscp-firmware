/**************************************************************************
 *
 * FILE NAME:			types.h
 * FILE DESCRIPTION:	Extended types header
 *
 * FILE CREATION DATE:	24-05-2004
 *
 *==========================================================================
 *
 * Written by Eran "Pavius" Duchan; www.pavius.net; Freely distributed.
 * 
 ***************************************************************************
 *
 * Modification history:
 * --------------------
 * 01a,24may03 erd	written
 *
 ***************************************************************************/

#ifndef __TYPES_H_
#define __TYPES_H_

// basic types
typedef char 				int_8;
typedef int 				int_16;
typedef unsigned char 		uint_8;
typedef unsigned int 		uint_16;
typedef unsigned long 		uint_32;

// boolean type
#define FALSE				(0)
#define TRUE				(1)
typedef uint_8				BOOL;
#define NULL				(0)

#endif /* __TYPES_H_ */
