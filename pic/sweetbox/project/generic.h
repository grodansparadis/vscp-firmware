/*********************************************************************
 *
 *                  Generic Typedef's
 *
 *********************************************************************
 * FileName:        Generic.h
 * Dependencies:
 * Processor:       Daytona
 *                  PIC24F
 * 
 * Complier:        MPLAB C30 v1.31.00 or higher
 *                  MPLAB IDE v7.22.00 or higher
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
 *********************************************************************
 *
 * $Id: generic.h 174 2008-10-21 19:46:53Z akhe $
 *
 ********************************************************************/
#ifndef _GENERIC_H_
#define _GENERIC_H_

#define	PUBLIC
#define PROTECTED
#define PRIVATE   static

#ifndef NULL
#define NULL    0
#endif

#ifndef NUL
#define NUL     '\0'
#endif

#ifndef _WINDOWS
typedef unsigned char           BYTE;    // 8-bit
typedef unsigned short int      WORD;    // 16-bit
typedef unsigned long           DWORD;   // 32-bit

typedef void                    VOID;

typedef char                    CHAR8;
typedef unsigned char           UCHAR8;

typedef int                     INT;
typedef char                    INT8;
typedef short                   INT16;
typedef long                    INT32;

typedef unsigned int            UINT;
typedef unsigned char           UINT8;   // other name for 8-bit integer
typedef unsigned short          UINT16;  // other name for 16-bit integer
typedef unsigned long           UINT32;  // other name for 32-bit integer


typedef enum _BOOL { FALSE = 0, TRUE } BOOL;
#endif

typedef struct
{
	BYTE	b0:     1;
	BYTE	b1:     1;
	BYTE	b2:     1;
	BYTE	b3:     1;
	BYTE	b4:     1;
	BYTE	b5:     1;
	BYTE	b6:     1;
	BYTE	b7:     1;

}BYTE_BITS;
typedef struct
{
	WORD	b0:     1;
	WORD	b1:     1;
	WORD	b2:     1;
	WORD	b3:     1;
	WORD	b4:     1;
	WORD	b5:     1;
	WORD	b6:     1;
	WORD	b7:     1;
	WORD	b8:     1;
	WORD	b9:     1;
	WORD	b10:    1;
	WORD	b11:    1;
	WORD	b12:    1;
	WORD	b13:    1;
	WORD	b14:    1;
	WORD	b15:    1;
}WORD_BITS;

typedef union _BYTE_VAL
{
    BYTE_BITS bits;
    BYTE Val;
} BYTE_VAL;


typedef union _WORD_VAL
{
    WORD Val;
    WORD_BITS bits;
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    struct
    {
	BYTE_VAL low;
	BYTE_VAL high;
    }byteUnion;

    BYTE v[2];
} WORD_VAL;

typedef union _DWORD_VAL
{
    DWORD Val;
    struct
    {
        BYTE LB;
        BYTE HB;
        BYTE UB;
        BYTE MB;
    } byte;
    struct
    {
        WORD LW;
        WORD HW;
    } word;
    struct
    {
        WORD_VAL low;
        WORD_VAL high;
    }wordUnion;
    struct
    {
        BYTE_VAL lowLSB;
        BYTE_VAL lowMSB;
        BYTE_VAL highLSB;
        BYTE_VAL highMSB;
    }byteUnion;
    BYTE v[4];
    WORD w[2];	
} DWORD_VAL;

#define LSB(a)          ((a).v[0])
#define MSB(a)          ((a).v[1])

#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])

//TODO: (DF) had to remove these functions based on C18 requirements for non-inline usage
#if !defined(__18CXX)
extern inline DWORD __attribute__ ((always_inline)) _arrayToDword(BYTE *array)
{
    DWORD_VAL   data;

    data.v[0] = array[0];
    data.v[1] = array[1];
    data.v[2] = array[2];
    data.v[3] = array[3];

    return data.Val;
}

extern inline void __attribute__ ((always_inline)) _dwordToArray(DWORD data, BYTE *array)
{
    DWORD_VAL   temp;

    temp.Val    = data;
    array[0]   = temp.v[0];
    array[1]   = temp.v[1];
    array[2]   = temp.v[2];
    array[3]   = temp.v[3];

}
#endif

#endif   // _GENERIC_H_ 
