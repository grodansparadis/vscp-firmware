/**  
 * @brief           Compiler specific defines
 * @file            compiler.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 *
 *
 * @section description Description
 **********************************
 * This module is an interface between the code and different compilers. It defines
 * generic names for compiler specific defines and variables.
 *
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith by Modtronix Engineering is based on v2.20.04.01
 * of the Microchip TCP/IP stack. The original code base is owned by Microchip
 * Technology Incorporated (the Company), and is protected under applicable
 * copyright laws. The modifications are owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller based products manufactured
 * by Modtronix Engineering. The code may be modified and can be used free of charge
 * for non commercial and commercial applications. All rights are reserved. Any use
 * in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 ********************************************************************
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 2006-02-03, David Hosken (DH): Added defines for Port F
 * 2004-09-09, David Hosken (DH): Added projdef support
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     11/14/01 Original        (Rev 1.0)
 *
 *******************************************************
 *The following should be defined in the projdefs.h file OR on the command line
 ********************************************************
 * #define CLOCK_FREQ  (20000000l)      // Hz
 ********************************************************************/

#ifndef COMPILER_H
#define COMPILER_H

#if defined(HI_TECH_C)
    #if defined(_MPC_)
        #define HITECH_C18
    #else
        #error "Unknown part is selected."
    #endif
#else
    #if !defined(_WIN32)
        #define MCHP_C18
    #endif
#endif

#if defined(MCHP_C18) && defined(HITECH_C18)
#error "Invalid Compiler selection."
#endif

#if !defined(MCHP_C18) && !defined(HITECH_C18) && !defined(_WIN32)
#error "Compiler not supported."
#endif

#if defined(MCHP_C18)
    #include <p18cxxx.h>    // p18cxxx.h must have current processor
                            // defined.
    #include <stddef.h>     //For offsetof macro
#endif

#if defined(HITECH_C18)
    #include <pic18.h>
    #include <stdio.h>
    #include <stddef.h>     //For offsetof macro
#endif

#if defined(__18CXX)        //Microchip C18 compiler
    #define PERSISTENT
#elif defined(HITECH_C18)   //HiTech PICC18 Compiler
    #define PERSISTENT persistent
#endif

#include <stdlib.h>


/////////////////////////////////////////////////
//Set some defines that are processor related
#if defined(__18F6621) || defined(_18F6621) \
        || defined(__18F6527) || defined(_18F6527) \
        || defined(__18F6627) || defined(_18F6627) \
        || defined(__18F6680) || defined(_18F6680)
#else
    #error "Stack can not be compiled for this processor!"
#endif


/////////////////////////////////////////////////
//Modify following macros depending on your interrupt usage
#define ENABLE_INTERRUPTS()             INTCON_GIEH = 1
#define DISBALE_INTERRUPTS()            INTCON_GIEH = 0


/////////////////////////////////////////////////
//Data Types
#undef BOOL
#undef TRUE
#undef FALSE
#undef BYTE
#undef WORD
#undef DWORD

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;
typedef unsigned char BYTE;                 // 8-bit
typedef unsigned short int WORD;            // 16-bit
#ifndef _WIN32
typedef unsigned short long SWORD;          // 24-bit
#else
typedef short int SWORD;
#endif
typedef unsigned long DWORD;                // 32-bit

typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    BYTE Val;
} BYTE_VAL;

typedef union _SWORD_VAL
{
    SWORD Val;
    struct
    {
        BYTE LSB;
        BYTE MSB;
        BYTE USB;
    } byte;
    BYTE v[3];
} SWORD_VAL;

typedef struct _WORD_ARRAY
{
    BYTE offset0;
    BYTE offset1;
} WORD_ARRAY;

#define WORD_LSB(a) ( (unsigned char) ((WORD_ARRAY)a).offset0 )
#define WORD_MSB(a) ( (unsigned char) ((WORD_ARRAY)a).offset1 )

typedef union _WORD_VAL
{
    WORD Val;
    struct
    {
        BYTE LSB;
        BYTE MSB;
    } byte;
    BYTE v[2];
} WORD_VAL;

#define LSB(a)          ((a).v[0])
#define MSB(a)          ((a).v[1])

typedef union _DWORD_VAL
{
    DWORD Val;
    struct
    {
        BYTE LOLSB;
        BYTE LOMSB;
        BYTE HILSB;
        BYTE HIMSB;
    } byte;
    struct
    {
        WORD LSW;
        WORD MSW;
    } word;
    BYTE v[4];
} DWORD_VAL;
#define LOWER_LSB(a)    ((a).v[0])
#define LOWER_MSB(a)    ((a).v[1])
#define UPPER_LSB(a)    ((a).v[2])
#define UPPER_MSB(a)    ((a).v[3])

#define CLEAR_BIT(theByte, mask)    (theByte &= (~mask))
#define SET_BIT(theByte, mask)      (theByte |= mask)
#define IS_BIT_SET(theByte, mask)   (theByte & mask)
#define IS_BIT_CLEAR(theByte, mask) ((theByte & mask) == 0)

typedef BYTE BUFFER;

#if defined(MCHP_C18)
    #define CLRWDT()        ClrWdt()
    #define NOP()           Nop()
    #define RESET()         Reset()
    #define SLEEP()         Sleep()

    #define ROM rom

    #define PORTA_RA0           PORTAbits.RA0
    #define PORTA_RA1           PORTAbits.RA1
    #define PORTA_RA2           PORTAbits.RA2
    #define PORTA_RA3           PORTAbits.RA3
    #define PORTA_RA4           PORTAbits.RA4
    #define PORTA_RA5           PORTAbits.RA5
    #define LATA0               LATAbits.LATA0
    #define LATA1               LATAbits.LATA1
    #define LATA2               LATAbits.LATA2
    #define LATA3               LATAbits.LATA3
    #define LATA4               LATAbits.LATA4
    #define LATA5               LATAbits.LATA5
    #define TRISA_RA0           TRISAbits.TRISA0
    #define TRISA_RA1           TRISAbits.TRISA1
    #define TRISA_RA2           TRISAbits.TRISA2
    #define TRISA_RA3           TRISAbits.TRISA3
    #define TRISA_RA4           TRISAbits.TRISA4
    #define TRISA_RA5           TRISAbits.TRISA5

    #define PORTB_RB0           PORTBbits.RB0
    #define PORTB_RB1           PORTBbits.RB1
    #define PORTB_RB2           PORTBbits.RB2
    #define PORTB_RB3           PORTBbits.RB3
    #define PORTB_RB4           PORTBbits.RB4
    #define PORTB_RB5           PORTBbits.RB5
    #define PORTB_RB6           PORTBbits.RB6
    #define PORTB_RB7           PORTBbits.RB7
    #define LATB0               LATBbits.LATB0
    #define LATB1               LATBbits.LATB1
    #define LATB2               LATBbits.LATB2
    #define LATB3               LATBbits.LATB3
    #define LATB4               LATBbits.LATB4
    #define LATB5               LATBbits.LATB5
    #define LATB6               LATBbits.LATB6
    #define LATB7               LATBbits.LATB7
    #define TRISB_RB0           TRISBbits.TRISB0
    #define TRISB_RB1           TRISBbits.TRISB1
    #define TRISB_RB2           TRISBbits.TRISB2
    #define TRISB_RB3           TRISBbits.TRISB3
    #define TRISB_RB4           TRISBbits.TRISB4
    #define TRISB_RB5           TRISBbits.TRISB5
    #define TRISB_RB6           TRISBbits.TRISB6
    #define TRISB_RB7           TRISBbits.TRISB7

    #define PORTC_RC0           PORTCbits.RC0
    #define PORTC_RC1           PORTCbits.RC1
    #define PORTC_RC2           PORTCbits.RC2
    #define PORTC_RC3           PORTCbits.RC3
    #define PORTC_RC4           PORTCbits.RC4
    #define PORTC_RC5           PORTCbits.RC5
    #define PORTC_RC6           PORTCbits.RC6
    #define PORTC_RC7           PORTCbits.RC7
    #define LATC0               LATCbits.LATC0
    #define LATC1               LATCbits.LATC1
    #define LATC2               LATCbits.LATC2
    #define LATC3               LATCbits.LATC3
    #define LATC4               LATCbits.LATC4
    #define LATC5               LATCbits.LATC5
    #define LATC6               LATCbits.LATC6
    #define LATC7               LATCbits.LATC7
    #define TRISC_RC0           TRISCbits.TRISC0
    #define TRISC_RC1           TRISCbits.TRISC1
    #define TRISC_RC2           TRISCbits.TRISC2
    #define TRISC_RC3           TRISCbits.TRISC3
    #define TRISC_RC4           TRISCbits.TRISC4
    #define TRISC_RC5           TRISCbits.TRISC5
    #define TRISC_RC6           TRISCbits.TRISC6
    #define TRISC_RC7           TRISCbits.TRISC7

    #define PORTD_RD0           PORTDbits.RD0
    #define PORTD_RD1           PORTDbits.RD1
    #define PORTD_RD2           PORTDbits.RD2
    #define PORTD_RD3           PORTDbits.RD3
    #define PORTD_RD4           PORTDbits.RD4
    #define PORTD_RD5           PORTDbits.RD5
    #define PORTD_RD6           PORTDbits.RD6
    #define PORTD_RD7           PORTDbits.RD7
    #define TRISD_RD0           TRISDbits.TRISD0
    #define TRISD_RD1           TRISDbits.TRISD1
    #define TRISD_RD2           TRISDbits.TRISD2
    #define TRISD_RD3           TRISDbits.TRISD3
    #define TRISD_RD4           TRISDbits.TRISD4
    #define TRISD_RD5           TRISDbits.TRISD5
    #define TRISD_RD6           TRISDbits.TRISD6
    #define TRISD_RD7           TRISDbits.TRISD7

    #define PORTE_RE0           PORTEbits.RE0
    #define PORTE_RE1           PORTEbits.RE1
    #define PORTE_RE2           PORTEbits.RE2
    #define PORTE_RE3           PORTEbits.RE3
    #define PORTE_RE4           PORTEbits.RE4
    #define PORTE_RE5           PORTEbits.RE5
    #define PORTE_RE6           PORTEbits.RE6
    #define PORTE_RE7           PORTEbits.RE7
    #define LATE0               LATEbits.LATE0
    #define LATE1               LATEbits.LATE1
    #define LATE2               LATEbits.LATE2
    #define LATE3               LATEbits.LATE3
    #define LATE4               LATEbits.LATE4
    #define LATE5               LATEbits.LATE5
    #define LATE6               LATEbits.LATE6
    #define LATE7               LATEbits.LATE7
    #define TRISE_RE0           TRISEbits.TRISE0
    #define TRISE_RE1           TRISEbits.TRISE1
    #define TRISE_RE2           TRISEbits.TRISE2
    #define TRISE_RE3           TRISEbits.TRISE3
    #define TRISE_RE4           TRISEbits.TRISE4
    #define TRISE_RE5           TRISEbits.TRISE5
    #define TRISE_RE6           TRISEbits.TRISE6
    #define TRISE_RE7           TRISEbits.TRISE7

    #define PORTF_RF0           PORTFbits.RF0
    #define PORTF_RF1           PORTFbits.RF1
    #define PORTF_RF2           PORTFbits.RF2
    #define PORTF_RF3           PORTFbits.RF3
    #define PORTF_RF4           PORTFbits.RF4
    #define PORTF_RF5           PORTFbits.RF5
    #define PORTF_RF6           PORTFbits.RF6
    #define PORTF_RF7           PORTFbits.RF7
    #define LATF0               LATFbits.LATF0
    #define LATF1               LATFbits.LATF1
    #define LATF2               LATFbits.LATF2
    #define LATF3               LATFbits.LATF3
    #define LATF4               LATFbits.LATF4
    #define LATF5               LATFbits.LATF5
    #define LATF6               LATFbits.LATF6
    #define LATF7               LATFbits.LATF7
    #define TRISF_RF0           TRISFbits.TRISF0
    #define TRISF_RF1           TRISFbits.TRISF1
    #define TRISF_RF2           TRISFbits.TRISF2
    #define TRISF_RF3           TRISFbits.TRISF3
    #define TRISF_RF4           TRISFbits.TRISF4
    #define TRISF_RF5           TRISFbits.TRISF5
    #define TRISF_RF6           TRISFbits.TRISF6
    #define TRISF_RF7           TRISFbits.TRISF7

    #define PORTG_RG0           PORTGbits.RG0
    #define PORTG_RG1           PORTGbits.RG1
    #define PORTG_RG2           PORTGbits.RG2
    #define PORTG_RG3           PORTGbits.RG3
    #define PORTG_RG4           PORTGbits.RG4
    #define PORTG_RG5           PORTGbits.RG5
    #define LATG0               LATGbits.LATG0
    #define LATG1               LATGbits.LATG1
    #define LATG2               LATGbits.LATG2
    #define LATG3               LATGbits.LATG3
    #define LATG4               LATGbits.LATG4
    #define LATG5               LATGbits.LATG5
    #define TRISG_RG0           TRISGbits.TRISG0
    #define TRISG_RG1           TRISGbits.TRISG1
    #define TRISG_RG2           TRISGbits.TRISG2
    #define TRISG_RG3           TRISGbits.TRISG3
    #define TRISG_RG4           TRISGbits.TRISG4
    #define TRISG_RG5           TRISGbits.TRISG5

    #define INTCON_TMR0IE       INTCONbits.TMR0IE
    #define INTCON_TMR0IF       INTCONbits.TMR0IF
    #define INTCON2_RBPU        INTCON2bits.RBPU

    #define T0CON_TMR0ON        T0CONbits.TMR0ON
    #define T1CON_RD16          T1CONbits.RD16
    #define T2CON_TMR2ON        T2CONbits.TMR2ON
    #define T4CON_TMR4ON        T4CONbits.TMR4ON

    #define SSPCON1_WCOL        SSPCON1bits.WCOL
    #define SSPCON1_SSPOV       SSPCON1bits.SSPOV

    #define SSPCON2_SEN         SSPCON2bits.SEN
    #define SSPCON2_ACKSTAT     SSPCON2bits.ACKSTAT
    #define SSPCON2_RSEN        SSPCON2bits.RSEN
    #define SSPCON2_RCEN        SSPCON2bits.RCEN
    #define SSPCON2_ACKEN       SSPCON2bits.ACKEN
    #define SSPCON2_PEN         SSPCON2bits.PEN
    #define SSPCON2_ACKDT       SSPCON2bits.ACKDT

    #define SSPSTAT_R_W         SSPSTATbits.R_W
    #define SSPSTAT_BF          SSPSTATbits.BF

    #define INTCON_GIEH         INTCONbits.GIEH
    #define INTCON_GIEL         INTCONbits.GIEL

    #define PIE1_TXIE           PIE1bits.TXIE
    #define PIR1_TXIF           PIR1bits.TXIF
    #define PIE1_RCIE           PIE1bits.RCIE
    #define PIR1_RCIF           PIR1bits.RCIF
    #define PIE3_TX2IE          PIE3bits.TX2IE
    #define PIR3_TX2IF          PIR3bits.TX2IF
    #define PIE3_RC2IE          PIE3bits.RC2IE
    #define PIR3_RC2IF          PIR3bits.RC2IF
    #define PIR2_BCLIF          PIR2bits.BCLIF
    #define PIR3_TMR4IF         PIR3bits.TMR4IF

    #define WDTCON_SWDTEN       WDTCONbits.SWDTEN

    #if defined(__18F6621)
    #define BAUDCON             BAUDCON1
    #define SPBRGH              SPBRGH1
    #endif

    #define TXSTA_TXEN          TXSTAbits.TXEN
    #define TXSTA1_TXEN         TXSTA1bits.TXEN
    #define TXSTA2_TXEN         TXSTA2bits.TXEN
    #define TXSTA_TRMT          TXSTAbits.TRMT
    #define TXSTA_BRGH          TXSTAbits.BRGH
    #define TXSTA1_TRMT         TXSTA1bits.TRMT
    #define TXSTA1_BRGH         TXSTA1bits.BRGH
    #define TXSTA2_TRMT         TXSTA2bits.TRMT
    #define TXSTA2_BRGH         TXSTA2bits.BRGH

    #define RCSTA_CREN          RCSTAbits.CREN
    #define RCSTA1_CREN         RCSTA1bits.CREN
    #define RCSTA2_CREN         RCSTA2bits.CREN
    #define RCSTA_SPEN          RCSTAbits.SPEN
    #define RCSTA1_SPEN         RCSTA1bits.SPEN
    #define RCSTA2_SPEN         RCSTA2bits.SPEN

    #define ADCON0_GO           ADCON0bits.GO
    #define ADCON0_ADON         ADCON0bits.ADON

    #define RCON_POR            RCONbits.POR

    #define EECON1_WREN         EECON1bits.WREN
    #define EECON1_WR           EECON1bits.WR
    #define EECON1_RD           EECON1bits.RD
    #define EECON1_EEPGD        EECON1bits.EEPGD
    #define EECON1_CFGS         EECON1bits.CFGS
    #define EECON1_FREE         EECON1bits.FREE

    #define OSCCON_PLLEN        OSCCONbits.PLLEN
    #define OSCCON_LOCK         OSCCONbits.LOCK
    #define OSCCON_SCS1         OSCCONbits.SCS1
    #define OSCCON_SCS0         OSCCONbits.SCS0
#endif

#if defined(HITECH_C18)
    #define ROM const

    //Macros
    #define memcmppgm2ram(a, b, c)      memcmp(a, b, c)

    #define memcpypgm2ram(a, b, c)      mymemcpy(a, b, c)

    #define strcmp(a, b)                strcmp(a, b)
    #define strcmppgm(a, b)             strcmp(a, b)
    #define strcmppgm2ram(a, b)         strcmp(a, b)
    #define strcmpram2pgm(a, b)         strcmp(a, b)

    #define strcpy(dst, src)            strcpy(dst, src)
    #define strcpypgm(dst, src)         strcpy(dst, src)
    #define strcpypgm2ram(dst, src)     strcpy(dst, src)
    #define strcpyram2pgm(dst, src)     strcpy(dst, src)


    extern void *mymemcpy(void * d1, const void * s1, unsigned char n);
    extern char *strupr(char*);

    /* Fix for HITECH C */
    #define TXREG       _TXREG
    static volatile near unsigned char       _TXREG       @ 0xFAD;


    // CPUs with dual SPI units
    #if defined(_18F6527) || defined(_18F6627)
        #ifndef SSPCON1
            #define SSPCON1     SSP1CON1
        #endif
        
        #ifndef SSPCON2
            #define SSPCON2     SSP1CON2
        #endif
        
        #ifndef SSPBUF
            #define SSPBUF     SSP1BUF
        #endif
        
        #ifndef BCLIF
            #define BCLIF   BCL1IF
        #endif
        
        #ifndef BAUDCON
            #define BAUDCON     BAUDCON1
        #endif
    
        #ifndef SPBRGH
            #define SPBRGH      SPBRGH1
        #endif
    
        #ifndef TXSTA
            #define TXSTA      TXSTA1
        #endif
    
        #ifndef RCSTA
            #define RCSTA      RCSTA1
        #endif
    
        #ifndef RCREG
            #define RCREG      RCREG1
        #endif
    
        #ifndef SPBRG
            #define SPBRG      SPBRG1
        #endif
    
        #ifndef SSPSTAT
            #define SSPSTAT      SSP1STAT
        #endif
    
        #ifndef SSPADD
            #define SSPADD      SSP1ADD
        #endif
    #endif


    /* Bit Definitions */
    #define PORTA_RA0           RA0
    #define PORTA_RA1           RA1
    #define PORTA_RA2           RA2
    #define PORTA_RA3           RA3
    #define PORTA_RA4           RA4
    #define PORTA_RA5           RA5
    #define TRISA_RA0           TRISA0
    #define TRISA_RA1           TRISA1
    #define TRISA_RA2           TRISA2
    #define TRISA_RA3           TRISA3
    #define TRISA_RA4           TRISA4
    #define TRISA_RA5           TRISA5

    #define PORTB_RB0           RB0
    #define PORTB_RB1           RB1
    #define PORTB_RB2           RB2
    #define PORTB_RB3           RB3
    #define PORTB_RB4           RB4
    #define PORTB_RB5           RB5
    #define PORTB_RB6           RB6
    #define PORTB_RB7           RB7
    #define TRISB_RB0           TRISB0
    #define TRISB_RB1           TRISB1
    #define TRISB_RB2           TRISB2
    #define TRISB_RB3           TRISB3
    #define TRISB_RB4           TRISB4
    #define TRISB_RB5           TRISB5
    #define TRISB_RB6           TRISB6
    #define TRISB_RB7           TRISB7

    #define PORTC_RC0           RC0
    #define PORTC_RC1           RC1
    #define PORTC_RC2           RC2
    #define PORTC_RC3           RC3
    #define PORTC_RC4           RC4
    #define PORTC_RC5           RC5
    #define PORTC_RC6           RC6
    #define PORTC_RC7           RC7
    #define TRISC_RC0           TRISC0
    #define TRISC_RC1           TRISC1
    #define TRISC_RC2           TRISC2
    #define TRISC_RC3           TRISC3
    #define TRISC_RC4           TRISC4
    #define TRISC_RC5           TRISC5
    #define TRISC_RC6           TRISC6
    #define TRISC_RC7           TRISC7

    #define PORTD_RD0           RD0
    #define PORTD_RD1           RD1
    #define PORTD_RD2           RD2
    #define PORTD_RD3           RD3
    #define PORTD_RD4           RD4
    #define PORTD_RD5           RD5
    #define PORTD_RD6           RD6
    #define PORTD_RD7           RD7
    #define TRISD_RD0           TRISD0
    #define TRISD_RD1           TRISD1
    #define TRISD_RD2           TRISD2
    #define TRISD_RD3           TRISD3
    #define TRISD_RD4           TRISD4
    #define TRISD_RD5           TRISD5
    #define TRISD_RD6           TRISD6
    #define TRISD_RD7           TRISD7

    #define PORTE_RE0           RE0
    #define PORTE_RE1           RE1
    #define PORTE_RE2           RE2
    #define PORTE_RE3           RE3
    #define PORTE_RE4           RE4
    #define PORTE_RE5           RE5
    #define PORTE_RE6           RE6
    #define PORTE_RE7           RE7
    #define TRISE_RE0           TRISE0
    #define TRISE_RE1           TRISE1
    #define TRISE_RE2           TRISE2
    #define TRISE_RE3           TRISE3
    #define TRISE_RE4           TRISE4
    #define TRISE_RE5           TRISE5
    #define TRISE_RE6           TRISE6
    #define TRISE_RE7           TRISE7

    #define PORTF_RF0           RF0
    #define PORTF_RF1           RF1
    #define PORTF_RF2           RF2
    #define PORTF_RF3           RF3
    #define PORTF_RF4           RF4
    #define PORTF_RF5           RF5
    #define PORTF_RF6           RF6
    #define PORTF_RF7           RF7
    #define TRISF_RF0           TRISF0
    #define TRISF_RF1           TRISF1
    #define TRISF_RF2           TRISF2
    #define TRISF_RF3           TRISF3
    #define TRISF_RF4           TRISF4
    #define TRISF_RF5           TRISF5
    #define TRISF_RF6           TRISF6
    #define TRISF_RF7           TRISF7

    #define PORTG_RG0           RG0
    #define PORTG_RG1           RG1
    #define PORTG_RG2           RG2
    #define PORTG_RG3           RG3
    #define PORTG_RG4           RG4
    #define PORTG_RG5           RG5
    #define TRISG_RG0           TRISG0
    #define TRISG_RG1           TRISG1
    #define TRISG_RG2           TRISG2
    #define TRISG_RG3           TRISG3
    #define TRISG_RG4           TRISG4
    #define TRISG_RG5           TRISG5

    #define INTCON_TMR0IE       TMR0IE
    #define INTCON_TMR0IF       TMR0IF
    #define INTCON2_RBPU        RBPU

    #define T0CON_TMR0ON        TMR0ON
    #define T1CON_RD16          TMR1RD16
    #define T2CON_TMR2ON        TMR2ON
    #define T4CON_TMR4ON        TMR4ON

    #define SSPCON1_WCOL        WCOL
    #define SSPCON1_SSPOV       SSPOV

    #define SSPCON2_SEN         SEN
    #define SSPCON2_ACKSTAT     ACKSTAT
    #define SSPCON2_RSEN        RSEN
    #define SSPCON2_RCEN        RCEN
    #define SSPCON2_ACKEN       ACKEN
    #define SSPCON2_PEN         PEN
    #define SSPCON2_ACKDT       ACKDT

    #define SSPSTAT_R_W         RW
    #define SSPSTAT_BF          BF

    #define INTCON_GIEH         GIEH
    #define INTCON_GIEL         GIEL

    // CPUs with dual SPI units
    #if defined(_18F6527) || defined(_18F6627)
        #ifndef BCLIF
            #define BCLIF           BCL1IF
        #endif
    
        #ifndef TXIE
            #define TXIE            TX1IE
        #endif
    
        #ifndef RCIE
            #define RCIE            RC1IE
        #endif
    
        #ifndef TXIF
            #define TXIF            TX1IF
        #endif
    
        #ifndef RCIF
            #define RCIF            RC1IF
        #endif
    #endif

    #define PIE1_TXIE           TXIE
    #define PIE1_RCIE           RCIE
    #define PIR1_TXIF           TXIF
    #define PIR1_RCIF           RCIF
    #define PIE3_TX2IE          TX2IE
    #define PIE3_RC2IE          RC2IE
    #define PIR3_TX2IF          TX2IF
    #define PIR3_RC2IF          RC2IF
    #define PIR2_BCLIF          BCLIF
    #define PIR3_TMR4IF         TMR4IF

    #define TXSTA_TRMT          TRMT
    #define TXSTA_BRGH          BRGH
    #define TXSTA1_TRMT         TRMT1
    #define TXSTA1_BRGH         BRGH1
    #define TXSTA2_TRMT         TRMT2
    #define TXSTA2_BRGH         BRGH2

    #define RCSTA_CREN          CREN
    #define RCSTA1_CREN         CREN1
    #define RCSTA2_CREN         CREN2

    #define ADCON0_GO           GODONE
    #define ADCON0_ADON         ADON

    #define Nop()               asm("NOP");

    #define RCON_POR            POR

    #define EECON1_WREN         WREN
    #define EECON1_WR           WR
    #define EECON1_RD           RD
    #define EECON1_EEPGD        EEPGD
    #define EECON1_CFGS         CFGS
    #define EECON1_FREE         FREE

    #define WDTCON_SWDTEN       SWDTEN

    #if defined(_18F6621) || defined(_18F6680)
        static          near bit    SCS0    @ ((unsigned)&OSCCON*8)+0;
        static          near bit    SCS1    @ ((unsigned)&OSCCON*8)+1;
        static          near bit    LOCK    @ ((unsigned)&OSCCON*8)+2;
        static          near bit    PLLEN    @ ((unsigned)&OSCCON*8)+3;
    #endif

    #define OSCCON_PLLEN        PLLEN
    #define OSCCON_LOCK         LOCK
    #define OSCCON_SCS1         SCS1
    #define OSCCON_SCS0         SCS0

#endif


#endif
