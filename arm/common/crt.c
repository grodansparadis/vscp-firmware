/***************************************************************************
   Copyright ARM Limited 1998 - 2000.  All rights reserved.
****************************************************************************

    crt.c

 	$Id: crt.c,v 1.1 2005/12/19 11:45:57 akhe Exp $

    Some C run time code, for example printf(), sprintf() etc.
    If the normal C-Library is included, this isn't required.
  
****************************************************************************/

#include "uhal.h"
#include "except_h.h"

void *uHALr_memset(char *s, int c, int n)
{
    int i = 0;

    while (n--)
    {
	s[i++] = c;
    }

    return s;
}


int uHALr_memcmp(char *cs, char *ct, int n)
{
    while (n--)
    {
	if (*cs > *ct)
	    return 1;
	if (*cs < *ct)
	    return -1;
	cs++;
	ct++;
    }

    return 0;
}


void *uHALr_memcpy(char *s, char *ct, int n)
{
    void *result = (void *)s;

    while (n--)
	*s++ = *ct++;

    return result;
}


int uHALr_strlen(const char *s)
{
    int i;

    i = 0;
    while (s[i] != '\0')
	i++;

    return i;
}


/*---------------------printf and support routines ---------------------*/

/* print c count times */
void PutRepChar(char c, int count)
{
    while (count--)
	uHALr_putchar(c);
}


/* put string reverse */
void PutStringReverse(char *s, int index)
{
    while ((index--) > 0)
	uHALr_putchar(s[index]);
}


/*-------------------------------------------------------------------------*/
/* 
   prints value in radix, in a field width width, with fill
   character fill
   if radix is negative, print as signed quantity
   if width is negative, left justify
   if width is 0, use whatever is needed
   if fill is 0, use ' '
 */

void PutNumber(int value, int radix, int width, char fill)
{
    char buffer[40];
    int bi = 0;
    int unsigned uvalue;
    short int digit;
    short int left = FALSE;
    short int negative = FALSE;

    if (fill == 0)
	fill = ' ';

    if (width < 0)
    {
	width = -width;
	left = TRUE;
    }

    if (width < 0 || width > 80)
	width = 0;

    if (radix < 0)
    {
	radix = -radix;
	if (value < 0)
	{
	    negative = TRUE;
	    value = -value;
	}
    }

    uvalue = value;

    do
    {
	if (radix != 16)
	{
	    digit = uvalue % radix;
	    uvalue = uvalue / radix;
	}
	else
	{
	    digit = uvalue & 0xf;
	    uvalue = uvalue >> 4;
	}
	buffer[bi] = digit + ((digit <= 9) ? '0' : ('A' - 10));
	bi++;

	if (uvalue != 0)
	{
	    if ((radix == 10)
		&& ((bi == 3) || (bi == 7) || (bi == 11) | (bi == 15)))
	    {
		buffer[bi++] = ',';
	    }
	}
    }
    while (uvalue != 0);

    if (negative)
    {
	buffer[bi] = '-';
	bi += 1;
    }

    if (width <= bi)
	PutStringReverse(buffer, bi);
    else
    {
	width -= bi;
	if (!left)
	    PutRepChar(fill, width);
	PutStringReverse(buffer, bi);
	if (left)
	    PutRepChar(fill, width);
    }
}


/*-------------------------------------------------------------------------*/
char *FormatItem(char *f, int a)
{
    char c;
    int fieldwidth = 0;
    int leftjust = FALSE;
    int radix = 0;
    char fill = ' ';

    if (*f == '0')
	fill = '0';

    while ((c = *f++) != 0)
    {
	if (c >= '0' && c <= '9')
	{
	    fieldwidth = (fieldwidth * 10) + (c - '0');
	}
	else
	    switch (c)
	    {
		case '\000':
		    return (--f);
		case '%':
		    uHALr_putchar('%');
		    return (f);
		case '-':
		    leftjust = TRUE;
		    break;
		case 'c':
		    {
			if (leftjust)
			    uHALr_putchar(a & 0x7f);
			if (fieldwidth > 0)
			    PutRepChar(fill, fieldwidth - 1);
			if (!leftjust)
			    uHALr_putchar(a & 0x7f);
			return (f);
		    }
		case 's':
		    {
			if (leftjust)
			    PutString((char *)a);
			if (fieldwidth > uHALr_strlen((char *)a))
			    PutRepChar(fill,
				       fieldwidth - uHALr_strlen((char *)a));
			if (!leftjust)
			    PutString((char *)a);
			return (f);
		    }
		case 'd':
		case 'i':
		    radix = -10;
		    break;
		case 'u':
		    radix = 10;
		    break;
		case 'x':
		    radix = 16;
		    break;
		case 'X':
		    radix = 16;
		    break;
		case 'o':
		    radix = 8;
		    break;
		default:
		    radix = 3;
		    break;	/* unknown switch! */
	    }
	if (radix)
	    break;
    }

    if (leftjust)
	fieldwidth = -fieldwidth;

    PutNumber(a, radix, fieldwidth, fill);

    return (f);
}


#define vaStart(list, param) list = (char*)((int)&param + sizeof(param))
#define vaArg(list, type) ((type *)(list += sizeof(type)))[-1]
#define vaEnd(list)

void uHALir_Defaultprintf(char *f, ...)	/* variable arguments */
{
    unsigned int mode;
    char *argP;

    /* disable IRQs and FIQs */
    mode = uHALir_ReadMode();
    uHALir_WriteMode(mode | NoFIQ | NoIRQ);

	vaStart(argP, f);	/* point at the end of the format string */
	while (*f)
	{			/* this works because args are all ints */
	    if (*f == '%')
		f = FormatItem(f + 1, vaArg(argP, int));

	    else
		uHALr_putchar(*f++);
	}
	vaEnd(argP);

	/* restore the previous mode */
	uHALir_WriteMode(mode);
}

// End of file - crt.c

