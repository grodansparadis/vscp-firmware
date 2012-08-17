
#include "types.h"
#include "mtutil.h"

#define __MAX_INT_CHARS 7

char* itoa(int16_t i)
{
	static char a[__MAX_INT_CHARS];
	
	char *b = a + sizeof(a) - 1;
	uint8_t   sign = (i < 0);

	if (sign) i = -i;
	*b = 0;
	do {
		*--b = '0' + (i % 10);
		i /= 10;
	} while (i);
	if (sign) *--b = '-';
	return b;
}
