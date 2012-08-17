#ifndef _DEFINE_H_
#define _DEFINE_H_

#ifndef BITFIELD
#define BITFIELD unsigned char
#endif

#ifndef BYTE
#define BYTE char
#endif

#ifndef UBYTE
#define UBYTE unsigned char
#endif

#ifndef WORD
#define WORD short
#endif

#ifndef UWORD
#define UWORD unsigned short
#endif

#ifndef LONG
#define LONG long
#endif

#ifndef ULONG
#define ULONG unsigned long
#endif

#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef FALSE
#undef  TRUE
#define FALSE   0
#define TRUE    1
#define OFF     FALSE
#define ON      TRUE
#endif

#ifndef NULL
#define NULL 0
#endif

#endif
