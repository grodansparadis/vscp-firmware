/*********************************************************************
** Title:		float.h - (ANSI) Implementation defined limits for wizC.
**
** Author:		Marcel van Lieshout
**
** Copyright:	(c) 2005, HMCS, Marcel van Lieshout
**
** License:		This software is released to the public domain and comes
**				without	warranty and/or guarantees of any kind. You have
**				the right to use, copy, modify and/or (re-)distribute the
**				software without restrictions.
*********************************************************************/
#ifndef _FLOAT_H
#define _FLOAT_H

#define	FLT_RADIX		(2)					/* radix of exponent								 */
#define	FLT_ROUNDS		(-1)				/* floating-point rounding mode for addition		 */

#define	FLT_DIG			(6)					/* decimal digits of precision						 */
#define	FLT_EPSILON		(5.364418315e-07)	/* smallest number x such that 1.0+x != 1.0			 */
#define	FLT_MANT_DIG	(24)				/* number of base FLT_RADIX in mantissa				 */
#define	FLT_MAX			(3.402823e+38)		/* maximum floating-point number					 */
#define	FLT_MAX_EXP		(127)				/* maximum n such that FLT_RADIXn-1 is representable */
#define	FLT_MIN			(6.174541e-41)		/* minimum normalized floating-point number			 */
#define	FLT_MIN_EXP		(-128)				/* minimum n such that 10n is a normalized number	 */

#define	DBL_DIG			FLT_DIG				/* decimal digits of precision						 */
#define	DBL_EPSILON		FLT_EPSILON			/* smallest number x such that 1.0+x != 1.0			 */
#define	DBL_MANT_DIG	FLT_MANT_DIG		/* number of base FLT_RADIX in mantissa				 */
#define	DBL_MAX			FLT_MAX				/* maximum double floating-point number				 */
#define	DBL_MAX_EXP		FLT_MAX_EXP			/* maximum n such that FLT_RADIXn-1 is representable */
#define	DBL_MIN			FLT_MIN				/* minimum normalized double floating-point number	 */
#define	DBL_MIN_EXP		FLT_MIN_EXP			/* minimum n such that 10n is a normalized number	 */

#endif	/* _FLOAT_H */
