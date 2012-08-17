// Copyright (C) 2002 Jaroslaw Karwik, 
//   Mente Marine Oy <jaroslaw.karwik@mente-marine.com>
//
// CANDIP
// Changes by akhe@eurosouce.se http://www.eursource.se
// 
// This file is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option) any
// later version.
// 
// In addition to the permissions in the GNU General Public License, the
// author gives you unlimited permission to link the
// compiled version of this file into combinations with other programs,
// and to distribute those combinations without any restriction coming
// from the use of this file.  (The General Public License restrictions
// do apply in other respects; for example, they cover modification of
// the file, and distribution when not linked into a combine
// executable.)
// 
// This file is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.  
// 


// General AVR definitions
typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned long int   u32;
typedef signed char         int8;
typedef short int           int16;
typedef long int            int32;
typedef unsigned char       BOOL;

#define TRUE    1
#define FALSE   0

/* SPMCR */
/* Store Program Memory Control Register */
#define    SPMIE        7
#define    RWWSB        6
#define    RWWSRE       4
#define    BLBSET       3
#define    PGWRT        2
#define    PGERS        1
#define    SPMEN        0

// Compiler specific macros
//#define  SET_PORT_BIT(PORT_,BIT_)  sbi(PORT_,BIT_)
//#define  CLR_PORT_BIT(PORT_,BIT_)  cbi(PORT_,BIT_)
//#define  CHK_PORT_BIT(PORT_,BIT_)  bit_is_set(PORT_,BIT_)
//#define  SET_PORT(PORT_,VAL_)      *((volatile unsigned char *)PORT_)  = (VAL_)
//#define  GET_PORT(PORT_)           *((volatile unsigned char *)PORT_)
//#define  WATCHDOG_RESET()          wdt_reset()

// Prototypes
void writePage( u16 addr, u8 function);
u8 readProgramMemory( u16 addr);
void fillTempBuffer( u16 data, u16 adr);
void runApplication( void );
void lowFlashBank( void );
void highFlashBank( void );





