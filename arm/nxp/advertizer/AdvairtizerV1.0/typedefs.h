#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------

#define BIT(x) (1 << (x)) // Bitvalue define

#define FALSE 0
#define false 0
#define TRUE  1
#define true  1

#define BIT0  0x00000001
#define BIT1  0x00000002
#define BIT2  0x00000004
#define BIT3  0x00000008
#define BIT4  0x00000010
#define BIT5  0x00000020
#define BIT6  0x00000040
#define BIT7  0x00000080
#define BIT8  0x00000100
#define BIT9  0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000

#define P0_0 BIT0
#define P0_1 BIT1
#define P0_2 BIT2
#define P0_3 BIT3
#define P0_4 BIT4
#define P0_5 BIT5
#define P0_6 BIT6
#define P0_7 BIT7
#define P0_8 BIT8
#define P0_9 BIT9
#define P0_10 BIT10
#define P0_11 BIT11
#define P0_12 BIT12
#define P0_13 BIT13
#define P0_14 BIT14
#define P0_15 BIT15
#define P0_16 BIT16
#define P0_17 BIT17
#define P0_18 BIT18
#define P0_19 BIT19
#define P0_20 BIT20
#define P0_21 BIT21
#define P0_22 BIT22
#define P0_23 BIT23
#define P0_24 BIT24
#define P0_25 BIT25
#define P0_26 BIT26
#define P0_27 BIT27
#define P0_28 BIT28
#define P0_29 BIT29
#define P0_30 BIT30
#define P0_31 BIT31

#define P1_0 BIT0
#define P1_1 BIT1
#define P1_2 BIT2
#define P1_3 BIT3
#define P1_4 BIT4
#define P1_5 BIT5
#define P1_6 BIT6
#define P1_7 BIT7
#define P1_8 BIT8
#define P1_9 BIT9
#define P1_10 BIT10
#define P1_11 BIT11
#define P1_12 BIT12
#define P1_13 BIT13
#define P1_14 BIT14
#define P1_15 BIT15
#define P1_16 BIT16
#define P1_17 BIT17
#define P1_18 BIT18
#define P1_19 BIT19
#define P1_20 BIT20
#define P1_21 BIT21
#define P1_22 BIT22
#define P1_23 BIT23
#define P1_24 BIT24
#define P1_25 BIT25
#define P1_26 BIT26
#define P1_27 BIT27
#define P1_28 BIT28
#define P1_29 BIT29
#define P1_30 BIT30
#define P1_31 BIT31
//-----------------------------------------------------------------------------

/*  typedefs are here  */
typedef unsigned char       uint8_t;
typedef   signed char        int8_t;
typedef unsigned short     uint16_t;
typedef   signed short      int16_t;
typedef unsigned long      uint32_t;
typedef   signed long       int32_t;
typedef unsigned long long uint64_t;
typedef   signed long long  int64_t;


typedef enum {False, True}  boolean;

typedef signed char S8; // value range: -128..127
typedef signed char S08;
typedef signed char tS08;
typedef unsigned char U8; // value range: 0..255
typedef unsigned char U08;
typedef unsigned char tU08;
typedef unsigned char BYTE;

typedef signed short S16; // value range: -32768..32767
typedef signed short tS16;
typedef unsigned short U16; // value range: 0..65535
typedef unsigned short tU16;
typedef unsigned short	WORD;

typedef signed long S32; // value range: -2147483648..2147483647
typedef signed long tS32;
typedef unsigned long U32; // value range: 0..4294967295
typedef unsigned long tU32;
typedef unsigned long DWORD;

typedef signed long long S64;
typedef signed long long tS64;
typedef unsigned long long U64;
typedef unsigned long long tU64;

typedef S08 *pS8;
typedef S08 *pS08;
typedef U08 *pU8;
typedef U08 *pU08;
typedef S16 *pS16;
typedef U16 *pU16;
typedef S32 *pS32;
typedef U32 *pU32;
typedef S64 *pS64;
typedef U64 *pU64;
typedef float *pfloat;
typedef double *pdouble;
typedef U32 BOOL;
typedef U32 bool;
typedef BOOL *pBOOL;
typedef void(*pFUNC)(void);

typedef U08 volatile *pREG08;
typedef U16 volatile *pREG16;
typedef U32 volatile *pREG32;

typedef union uMEM08 // 8-Bit Datentyp mit Byte- und Bitzugriff
{                    // Zugriff auf Byte var.U08 = 0xFF;
  U08 U08 ;          // Zugriff auf Bit var.bit._7 = 1;

  struct
  {
    U08 _0: 1;
    U08 _1: 1;
    U08 _2: 1;
    U08 _3: 1;
    U08 _4: 1;
    U08 _5: 1;
    U08 _6: 1;
    U08 _7: 1;
  } bit;

} tMEM08;

typedef union uMEM16 // 16-Bit Datentyp mit Wort- und Bytezugriff
{                    // Zugriff auf Wort var.U16 = 0xAA55;
  U16 U16;           // Zugriff auf Byte var.U08._0 = 0xFF;

  struct
  {
    U08 _0; // LSB
    U08 _1; // MSB
  } U08;

} tMEM16;

typedef union uMEM32 // 32-Bit Datentyp mit Byte-, Wort- und Zeigerzugriff
{
  tU32 U32; // Zugriff auf DWORD var.U32 = 0x4000AA55;
  pU32 pU32; // Zeiger auf U32

  float fp; // Zugriff auf float var.fp = 123.456;
  pfloat pfp; // Zeiger auf float

  struct // Zugriff auf Word: var.U16._0 = 0xABCD;
  {
    U16 _0; // LSW
    U16 _1; // MSW
  } U16;

  pU16 pU16; // Zeiger auf U16

  struct // Zugriff auf Byte var.U08._0 = 0xFF;
  {
    U08 _0; // LSB
    U08 _1;
    U08 _2;
    U08 _3; // MSB
  } U08;

  pU08 pU08; // Zeiger auf U08

} tMEM32;

#endif
