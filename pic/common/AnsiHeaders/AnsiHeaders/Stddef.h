/*********************************************************************
** Title:		stddef.h - (ANSI) Standard Definitions for wizC.
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
#ifndef _STDDEF_H
#define _STDDEF_H

typedef	int				ptrdiff_t;
typedef	unsigned int	size_t;
typedef	unsigned char	wchar_t;

#ifndef NULL
	#define	NULL	(0)
#endif

#define	offsetof(s,m)	((size_t)&(((s *)0)->m))

#endif	/* _STDDEF_H */
