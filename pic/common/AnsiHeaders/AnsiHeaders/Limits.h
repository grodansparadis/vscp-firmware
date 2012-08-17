/*********************************************************************
** Title:		limits.h - (ANSI) Implementation defined limits for wizC.
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
#ifndef _LIMITS_H
#define _LIMITS_H

#define	CHAR_BIT	(8)				/* bits in a char						*/

#define	SCHAR_MIN	(-128)			/* minimum value of signed char			*/
#define	SCHAR_MAX	(127)			/* maximum value of signed char			*/

#define	CHAR_MIN	SCHAR_MIN		/* mimimum value of char				*/
#define	CHAR_MAX	SCHAR_MAX		/* maximum value of char				*/
#define	UCHAR_MAX	(0xFFu)			/* maximum value of unsigned char		*/

#define	INT_MIN		(-32768)		/* minimum value of a (signed) int		*/
#define	INT_MAX		(32767)			/* maximum value of a (signed) int		*/
#define	UINT_MAX	(0xFFFFu)    	/* maximum value of a unsigned int		*/

#define	SHRT_MIN	(-32768)		/* minimum value of a (signed) short	*/
#define	SHRT_MAX	(32767)			/* maximum value of a (signed) short	*/
#define	USHRT_MAX	(0xFFFFu)    	/* maximum value of a unsigned short	*/

#define	LONG_MIN	(-2147483648l)	/* minimum value of a (signed) long		*/
#define	LONG_MAX	(2147483647l)	/* maximum value of a (signed) long		*/
#define	ULONG_MAX	(0xFFFFFFFFul) 	/* maximum value of a unsigned long		*/

#endif	/* _LIMITS_H */
