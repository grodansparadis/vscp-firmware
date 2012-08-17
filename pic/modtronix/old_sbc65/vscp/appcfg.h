/*********************************************************************
 * appcfg.h    -    Application Configuration functions
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.10PL1 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Application configuration.
 *
 *********************************************************************
 * File History
 *
 * 2004-10-19, David Hosken (DH):
 *    - Initial version
 *
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/

/*******************************************************
----------------- Appcfg Configuration -----------------
********************************************************/
//The following should be defined in the projdefs.h file OR on the command line

//Define if this application has an 8 bit ADC converter, 10 bit ADC converter or none at all.
//#define APP_USE_ADC8
//#define APP_USE_ADC10

//Define buffer space that is reserved for ADC conversion values. For a 8 bit ADC converted, 1 byte
//is reserved per channel. For an 10 bit ADC, 2 bytes are reserved per channel.
//#define ADC_CHANNELS 12

/********************************************************/

#ifndef APPCFG_H
#define APPCFG_H

#include "ip.h"
#include <projdefs.h>

/////////////////////////////////////////////////
//Do some checks on defines
#if ((ADC_CHANNELS < 0) || (ADC_CHANNELS > 12))
#error "ADC_CHANNELS has been assigned an illegal value!"
#endif

#if (ADC_CHANNELS > 0)
    #if !defined(APP_USE_ADC8) && !defined(APP_USE_ADC10)
    #define APP_USE_ADC8
    #endif
#endif

#if (ADC_CHANNELS > 0)
    #if defined(APP_USE_ADC8) && defined(APP_USE_ADC10)
    #error "APP_USE_ADC8 and APP_USE_ADC10 can not both be defined!"
    #endif
#endif

/////////////////////////////////////////////////
//CPU configuration stuff

#define BAUD_300        0
#define BAUD_1200       1
#define BAUD_2400       2
#define BAUD_4800       3
#define BAUD_9600       4
#define BAUD_19200      5
#define BAUD_38400      6
#define BAUD_57600      7
#define BAUD_115200     8
#define BAUD_230400     9
#define BAUD_460800     10
#define BAUD_921600     11

typedef struct _USART_CFG
{
    BYTE        baud;
    BYTE        cfg;
} USART_CFG;    //2 bytes long

typedef struct _ADC_CFG
{
    BYTE        adcon1;
    BYTE        adcon2;
} ADC_CFG;    //2 bytes long

typedef struct _PORT_CFG
{
    BYTE        trisa;          //NB!!! They must follow each other in structure!!!
    BYTE        trisb;
    BYTE        trisc;
    BYTE        trisd;
    BYTE        trise;
    BYTE        trisf;
} PORT_CFG;    //2 bytes long

/////////////////////////////////////////////////
//Main application config structure
typedef struct _APP_CONFIG
{
    IP_ADDR     MyIPAddr;		//4 bytes
    MAC_ADDR    MyMACAddr;		//6 bytes
    IP_ADDR     MyMask;			//4 bytes
    IP_ADDR     MyGateway;		//4 bytes
    WORD_VAL    SerialNumber;	//2 bytes
    IP_ADDR     SMTPServerAddr; //Not used - 4 bytes
    struct
    {
        unsigned int bIsDHCPEnabled : 1;
        unsigned int bPLLOn : 1;            //Indicates if the PLL is on = 40MHz clock. Else, 10MHz clock
        unsigned int bFreezeADCBuf : 1;     //Stop updating AdcValues[] ADC buffer
    } Flags;					//1 byte
    IP_ADDR     TFTPServerAddr;	//4 bytes
    USART_CFG   USART1Cfg;      //2 byte
    USART_CFG   USART2Cfg;      //2 byte
    PORT_CFG    PortCfg;        //5 bytes
    ADC_CFG     ADCCfg;         //2 bytes
} APP_CONFIG;	//4+6+4+4+2+4+1+4+1+1 = 33 bytes long. !!!!!!!!!!! Important, ensure MPFS_RESERVE_BLOCK is large enough !!!!!!!!!!!!


/////////////////////////////////////////////////
//Global variables

//This is used by other stack elements
#ifndef THIS_IS_APPCFG_MODULE

extern APP_CONFIG AppConfig;

//Array containing converted ADC values
#if defined(APP_USE_ADC8)
extern BYTE AdcValues[ADC_CHANNELS];
#elif defined(APP_USE_ADC10)
extern WORD AdcValues[ADC_CHANNELS];
#endif

#endif





/*********************************************************************
 * Function:        void appcfgInit(void)
 *
 * PreCondition:    MPFSInit() is already called.
 *
 * Input:           None
 *
 * Output:          Write/Read non-volatile config variables.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void appcfgInit(void);

/*
 * Save Application Configuartion data to EEPROM
 */
void appcfgSave(void);

/*
 * Configure USART with AppConfig data
 */
void appcfgUSART(void);

/*
 * Configure the CPU I/O ports
 */
void appcfgCpuIO(void);

/*
 * Configure the ADC unit
 */
void appcfgADC(void);

/*
 * Configure PWM
 */
void appcfgPWM( void );

#endif
