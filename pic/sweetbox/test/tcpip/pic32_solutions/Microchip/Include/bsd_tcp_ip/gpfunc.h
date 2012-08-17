/*********************************************************************
 *
 *                  Helper Function Defs for Microchip eTCP/IP Stack
 *
 *********************************************************************
 * FileName:        gpFunc.h
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC30F and PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's dsPIC30F and PICmicro Microcontroller products. 
 * The software is owned by the Company and/or its supplier, and is
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
 * $Id: gpFunc.h,v 1.1 2006/09/28 22:10:29 C12770 Exp $
 *
 ********************************************************************/

#ifndef HELPERS_H
#define HELPERS_H

#include "bsd_tcp_ip\etcpcfg.h"

WORD swaps(WORD v);
void swapl(DWORD dw, DWORD*pd);

void IP_StrToDWord( char* szDot, DWORD* pdwIP );
void MAC_StrToBin( char* szMAC, BYTE* pMAC );
void itoa(unsigned int dat, char* str);

WORD fold_checksum( DWORD csum );
void checksum16_partial( BYTE* pkt, int len, DWORD* sum );
WORD checksum16( BYTE* pkt, int len );
void Delay( unsigned int delay_count );
char *strupr(char*);

#define LOWORD(a) ((WORD)a)
#define HIWORD(a) ((WORD)(a >> 16))

#endif
