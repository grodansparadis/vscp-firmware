/*********************************************************************
 *
 *                  Compiler specific defs.
 *
 *********************************************************************
 * FileName:        Compiler.h
 * Dependencies:    None
 * Processor:       PIC32
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-32 V8.10PL1 or higher
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
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     7/12/04     Rel 0.9
 * Nilesh Rajbharti     11/1/04     Pre-release version
 * DF/KO                04/29/05 Microchip ZigBee Stack v1.0-2.0
 * DF/KO                07/18/05 Microchip ZigBee Stack v1.0-3.0
 * DF/KO                07/27/05 Microchip ZigBee Stack v1.0-3.1
 * DF/KO                01/09/06 Microchip ZigBee Stack v1.0-3.5
 ********************************************************************/


#include <stdlib.h>

#ifndef _COMPILER_H_
#define _COMPILER_H_

	#define TXSTAbits           TXSTA2bits
 	#define TXSTA               TXSTA2
   	#define TXREG               TXREG2
    #define RCSTA           	RCSTA2
	#define SPBRG           	SPBRG2
	#define RCREG           	RCREG2
	#define SPICON				SSP2CON1
	#define SPISTATbits			SPI2STATbits
    
    #define ROM                 rom

    #define SWDTEN              WDTCONbits.SWDTEN

    #define DC                  STATUSbits.DC

    #define EEWR                EECON1bits.WR
    #define EEWREN              EECON1bits.WREN

    #define LATA0               LATAbits.LATA0
    #define LATA1               LATAbits.LATA1
    #define LATA2               LATAbits.LATA2
    #define LATA3               LATAbits.LATA3
    #define LATA4               LATAbits.LATA4
    #define LATA5               LATAbits.LATA5
    #define LATA6               LATAbits.LATA6
    #define LATA7               LATAbits.LATA7

    #define RB0                 PORTBbits.RB0
    #define RB1                 PORTBbits.RB1
    #define RB2                 PORTBbits.RB2
    #define RB3                 PORTBbits.RB3
    #define RB4                 PORTBbits.RB4
    #define RB5                 PORTBbits.RB5
    #define RB6                 PORTBbits.RB6
    #define RB7                 PORTBbits.RB7

    #define TRISB0              TRISBbits.TRISB0
    #define TRISB1              TRISBbits.TRISB1
    #define TRISB2              TRISBbits.TRISB2
    #define TRISB3              TRISBbits.TRISB3
    #define TRISB4              TRISBbits.TRISB4
    #define TRISB5              TRISBbits.TRISB5
    #define TRISB6              TRISBbits.TRISB6
    #define TRISB7              TRISBbits.TRISB7

    #define RA0                 PORTAbits.RA0
    #define RA1                 PORTAbits.RA1
    #define RA2                 PORTAbits.RA2
    #define RA3                 PORTAbits.RA3
    #define RA4                 PORTAbits.RA4
    #define RA5                 PORTAbits.RA5
    #define RA6                 PORTAbits.RA6
    #define RA7                 PORTAbits.RA7

    #define TRISA0              TRISAbits.TRISA0
    #define TRISA1              TRISAbits.TRISA1
    #define TRISA2              TRISAbits.TRISA2
    #define TRISA3              TRISAbits.TRISA3
    #define TRISA4              TRISAbits.TRISA4
    #define TRISA5              TRISAbits.TRISA5
    #define TRISA6              TRISAbits.TRISA6
    #define TRISA7              TRISAbits.TRISA7

    #define LATC0               LATCbits.LATC0
    #define LATC1               LATCbits.LATC1
    #define LATC2               LATCbits.LATC2
    #define LATC3               LATCbits.LATC3
    #define LATC4               LATCbits.LATC4
    #define LATC5               LATCbits.LATC5
    #define LATC6               LATCbits.LATC6
    #define LATC7               LATCbits.LATC7

    #define TRISC0              TRISCbits.TRISC0
    #define TRISC1              TRISCbits.TRISC1
    #define TRISC2              TRISCbits.TRISC2
    #define TRISC3              TRISCbits.TRISC3
    #define TRISC4              TRISCbits.TRISC4
    #define TRISC5              TRISCbits.TRISC5
    #define TRISC6              TRISCbits.TRISC6
    #define TRISC7              TRISCbits.TRISC7

    #define RC0                 PORTCbits.RC0
    #define RC1                 PORTCbits.RC1
    #define RC2                 PORTCbits.RC2
    #define RC3                 PORTCbits.RC3
    #define RC4                 PORTCbits.RC4
    #define RC5                 PORTCbits.RC5
    #define RC6                 PORTCbits.RC6
    #define RC7                 PORTCbits.RC7

    #define TRISD7              TRISDbits.TRISD7
    #define TRISD6              TRISDbits.TRISD6
    #define TRISD5              TRISDbits.TRISD5
    #define TRISD4              TRISDbits.TRISD4
    #define TRISD3              TRISDbits.TRISD3
    #define TRISD2              TRISDbits.TRISD2
    #define TRISD1              TRISDbits.TRISD1
    #define TRISD0              TRISDbits.TRISD0

    #define LATD0               LATDbits.LATD0
    #define LATD1               LATDbits.LATD1
    #define LATD2               LATDbits.LATD2
    #define LATD3               LATDbits.LATD3
    #define LATD4               LATDbits.LATD4
    #define LATD5               LATDbits.LATD5
    #define LATD6               LATDbits.LATD6
    #define LATD7               LATDbits.LATD7


    #define RD7                 PORTDbits.RD7
    #define RD6                 PORTDbits.RD6
    #define RD5                 PORTDbits.RD5
    #define RD4                 PORTDbits.RD4
    #define RD3                 PORTDbits.RD3
    #define RD2                 PORTDbits.RD2
    #define RD1                 PORTDbits.RD1
    #define RD0                 PORTDbits.RD0

    #define RE0                 PORTEbits.RE0
    #define RE1                 PORTEbits.RE1
    #define RE2                 PORTEbits.RE2
    #define RE3                 PORTEbits.RE3
    #define RE4                 PORTEbits.RE4
    #define RE5                 PORTEbits.RE5
    #define RE6                 PORTEbits.RE6
    #define RE7                 PORTEbits.RE7

    #define TMR0IE              INTCONbits.TMR0IE
    #define TMR0IF              INTCONbits.TMR0IF
    #define INT0IE              INTCONbits.INT0IE
    #define INT0IF              INTCONbits.INT0IF
    #define RBPU                INTCON2bits.RBPU
    #define RBIF                INTCONbits.RBIF
    #define RBIE                INTCONbits.RBIE

    #define CCP2IF              PIR2bits.CCP2IF
    #define CCP2IE              PIE2bits.CCP2IE
    #define CCP2IP              IPR2bits.CCP2IP

    #define INT2IE              INTCON3bits.INT2IE
    #define INT2IF              INTCON3bits.INT2IF
    #define INT0IF              INTCONbits.INT0IF

    #define INT2IP              INTCON3bits.INT2IP
    #define INTEDG2             INTCON2bits.INTEDG2
    #define TMR0IP              INTCON2bits.TMR0IP

    #define TMR0ON              T0CONbits.TMR0ON

    #define TMR1ON              T1CONbits.TMR1ON

    #define TMR3IE              PIE2bits.TMR3IE
    #define TMR3IF              PIR2bits.TMR3IF
    #define TMR3ON              T3CONbits.TMR3ON


    #define SEN                 SSPCON2bits.SEN
    #define ACKSTAT             SSPCON2bits.ACKSTAT
    #define RSEN                SSPCON2bits.RSEN
    #define RCEN                SSPCON2bits.RCEN
    #define ACKEN               SSPCON2bits.ACKEN
    #define PEN                 SSPCON2bits.PEN
    #define ACKDT               SSPCON2bits.ACKDT

    #define SSPRW               SSPSTATbits.R_W
    #define SSPBF               SSPSTATbits.BF
    #define CKE                 SSPSTATbits.CKE
    #define SMP                 SSPSTATbits.SMP

    #define GIEH                INTCONbits.GIEH
    #define GIEL                INTCONbits.GIEL

    #define BCLIF               PIR2bits.BCLIF

    #define TXIE                PIE1bits.TXIE
    #define RCIE                PIE1bits.RCIE
    #define TMR1IE              PIE1bits.TMR1IE

    
    #define TMR1IF              PIR1bits.TMR1IF

    #define RCIP                IPR1bits.RCIP


    #define TRMT                TXSTAbits.TRMT
    #define BRGH                TXSTAbits.BRGH


  	#define RCIF                PIR1bits.RCIF
   	#define TXIF                PIR1bits.TXIF
        
  	#define SSPIF               PIR1bits.SSPIF
  	#define WCOL                SSPCON1bits.WCOL
            
   	#define CREN                RCSTAbits.CREN
    #define OERR                RCSTAbits.OERR

    
    #define ADGO                ADCON0bits.GO
    #define ADON                ADCON0bits.ADON

    #define RI                  RCONbits.RI
    #define TO                  RCONbits.TO
    #define PD                  RCONbits.PD
    #define POR                 RCONbits.POR
    #define BOR                 RCONbits.BOR
    #define IPEN                RCONbits.IPEN

    #define WUE                 BAUDCONbits.WUE

    #define STKFUL              STKPTRbits.STKFUL
    #define STKUNF              STKPTRbits.STKUNF

    #define NOP()               Nop()
    #define CLRWDT()            ClrWdt()
    #define RESET()             Reset()
    #define SLEEP()             Sleep()
    #define DISABLE_WDT()       (WDTCONbits.SWDTEN = 0)
    #define ENABLE_WDT()        (WDTCONbits.SWDTEN = 1)

    #define TBLWTPOSTINC()      _asm tblwtpostinc _endasm


#endif
