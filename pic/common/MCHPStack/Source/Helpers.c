/*********************************************************************
 *
 *                  Helper Functions for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        Helpers.C
 * Dependencies:    compiler.h
 *                  helpers.h
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * HiTech PICC18 Compiler Options excluding device selection:
 *                  -FAKELOCAL -G -E -C
 *
 *
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/17/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     6/25/02 Rewritten CalcIPChecksum() to avoid
 *                              multi-byte shift operation.
 ********************************************************************/

#include "compiler.h"
#include "helpers.h"


WORD swaps(WORD v)
{
    WORD_VAL t;
    BYTE b;

    t.Val   = v;
    b       = t.v[1];
    t.v[1]  = t.v[0];
    t.v[0]  = b;

    return t.Val;
}


DWORD swapl(DWORD v)
{
    BYTE b;
    DWORD myV;
    DWORD_VAL *myP;

    myV     = v;
    myP     = (DWORD_VAL*)&myV;

    b       = myP->v[3];
    myP->v[3] = myP->v[0];
    myP->v[0] = b;

    b       = myP->v[2];
    myP->v[2] = myP->v[1];
    myP->v[1] = b;

    return myV;

}

WORD CalcIPChecksum(BYTE* buffer, WORD count)
{
    WORD i;
    WORD *val;

    union
    {
        DWORD Val;
        struct
        {
            WORD_VAL LSB;
            WORD_VAL MSB;
        } words;
    } tempSum, sum;

    sum.Val = 0;

    i = count >> 1;
    val = (WORD *)buffer;

    while( i-- )
        sum.Val += *val++;

    if ( count & 1 )
        sum.Val += *(BYTE *)val;

    tempSum.Val = sum.Val;

    while( (i = tempSum.words.MSB.Val) != 0 )
    {
        sum.words.MSB.Val = 0;
        sum.Val = (DWORD)sum.words.LSB.Val + (DWORD)i;
        tempSum.Val = sum.Val;
    }

    return (~sum.words.LSB.Val);
}


#if defined(HITECH_C18)
char *strupr (char *s)
{
    char c;
    char *t;

    t = s;
    while( (c = *t) )
    {
        if ( (c >= 'a' && c <= 'z') )
            *t -= ('a' - 'A');
    t++;
    }
    return s;
}
#endif


#if defined(HITECH_C18)
void *mymemcpy(void * d1, const void * s1, register unsigned char n)
{

    register char *     d;
    register const char *   s;

    s = s1;
    d = d1;
    while(n--)
        *d++ = *s++;
    return d1;
}
#endif
