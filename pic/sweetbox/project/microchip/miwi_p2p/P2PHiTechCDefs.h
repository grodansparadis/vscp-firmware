/********************************************************************
* FileName:		P2PHiTechCDefs.h
* Dependencies: none
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher	
* Company:		Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for P2P Software:
*
* Copyright © 2006-2007 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and 
* distribute Software only when embedded on a Microchip
* microcontroller or digital signal controller and used with a 
* Microchip radio frequency transceiver, which are integrated into
* your product or third party product (pursuant to the sublicense 
* terms in the accompanying license agreement).  
*
* You should refer to the license agreement accompanying this 
* Software for additional information regarding your rights 
* and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION,
* ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND
* FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR
* ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE,
* STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL
* EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
* INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT,
* PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST
* OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
* CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
* THEREOF), OR OTHER SIMILAR COSTS.
*
*********************************************************************
* File Description:
*
*  This file creates definitions used by the P2P stack to force the
*   Hi-Tech C compiler to look like the C30 compiler
*
* Change History:
*  Rev   Date         Description
*  0.1   11/09/2006   Initial revision
*  1.0   01/09/2007   Initial release
********************************************************************/

#ifndef P2P_HITECHC_DEFS_H
#define P2P_HITECHC_DEFS_H

typedef struct
{
   unsigned char SSPM:4;
   unsigned char CKP:1;
   unsigned char SSPEN:1;
   unsigned char SSPOV:1;
   unsigned char WCOL:1;
} SSPCON1bits;
typedef struct
{
   unsigned char BF:1;
   unsigned char UA:1;
   unsigned char R_W:1;
   unsigned char S:1;
   unsigned char P:1;
   unsigned char D_A:1;
   unsigned char CKE:1;
   unsigned char SMP:1;
} SSPSTATbits;
typedef struct 
{
   unsigned char RBIF:1;
   unsigned char INT0IF:1;
   unsigned char TMR0IF:1;
   unsigned char RBIE:1;
   unsigned char INT0IE:1;
   unsigned char TMR0IE:1;
   unsigned char GIEL:1;
   unsigned char GIEH:1;
} INTCONbits;
typedef struct 
{
   unsigned char RBIP:1;
   unsigned char INT3IP:1;
   unsigned char TMR0IP:1;
   unsigned char INTEDG3:1;
   unsigned char INTEDG2:1;
   unsigned char INTEDG1:1;
   unsigned char INTEDG0:1;
   unsigned char RBPU:1;
} INTCON2bits;
typedef struct 
{
   unsigned char ADON:1;
   unsigned char GO:1;
   unsigned char CHS0:1;
   unsigned char CHS1:1;
   unsigned char CHS2:1;
   unsigned char CHS3:1;
} ADCON0bits;
typedef struct 
{
unsigned char ADCS0:1;
unsigned char ADCS1:1;
unsigned char ADCS2:1;
unsigned char ACQT0:1;
unsigned char ACQT1:1;
unsigned char ACQT2:1;
unsigned char :1;
unsigned char ADFM:1;
} ADCON2bits;
typedef struct 
{
   unsigned char TMR1IF:1;
   unsigned char TMR2IF:1;
   unsigned char CCP1IF:1;
   unsigned char SSPIF:1;
   unsigned char TXIF:1;
   unsigned char RCIF:1;
   unsigned char ADIF:1;
   unsigned char PSPIF:1;
} PIR1bits;
typedef struct 
{
   unsigned char TMR1IE:1;
   unsigned char TMR2IE:1;
   unsigned char CCP1IE:1;
   unsigned char SSPIE:1;
   unsigned char TXIE:1;
   unsigned char RCIE:1;
   unsigned char ADIE:1;
   unsigned char PSPIE:1;
} PIE1bits;
typedef struct
{
   unsigned char T0PS0:1;
   unsigned char T0PS1:1;
   unsigned char T0PS2:1;
   unsigned char PSA:1;
   unsigned char T0SE:1;
   unsigned char T0CS:1;
   unsigned char T08BIT:1;
   unsigned char TMR0ON:1;
} T0CONbits;
typedef struct
{
   unsigned char TX9D:1;
   unsigned char TRMT:1;
   unsigned char BRGH:1;
   unsigned char SENDB:1;
   unsigned char SYNC:1;
   unsigned char TXEN:1;
   unsigned char TX9:1;
   unsigned char CSRC:1;
} TXSTAbits;
typedef struct
{
   unsigned char RX9D:1;
   unsigned char OERR:1;
   unsigned char FERR:1;
   unsigned char ADDEN:1;
   unsigned char CREN:1;
   unsigned char SREN:1;
   unsigned char RX9:1;
   unsigned char SPEN:1;
} RCSTAbits;
typedef struct
{
  	unsigned char :7;
  	unsigned char IPEN;
  } RCONbits;

#define TXSTA				TXSTA1
#define RCSTA				RCSTA1
#define SPBRG				SPBRG1
#define RCREG				RCREG1
#define TXREG				TXREG1
#define SSPCON              SSPCON1

#define INTCONbits			(*((INTCONbits*)&INTCON))
#define INTCON2bits			(*((INTCON2bits*)&INTCON2))
#define ADCON0bits			(*((ADCON0bits*)&ADCON0))
#define ADCON2bits			(*((ADCON2bits*)&ADCON2))
#define PIR1bits			(*((PIR1bits*)&PIR1))
#define PIE1bits			(*((PIE1bits*)&PIE1))
#define T0CONbits			(*((T0CONbits*)&T0CON))
#define TXSTAbits			(*((TXSTAbits*)&TXSTA1))
#define RCSTAbits			(*((RCSTAbits*)&RCSTA1))
#define SSPCON1bits			(*((SSPCON1bits*)&SSPCON1))
#define RCONbits			(*((RCONbits*)&RCON))

#endif
