/********************************************************************
* FileName:		Console.h
* Dependencies: none
* Processor:	PIC18, PIC24F, PIC24H, dsPIC30, dsPIC33
*               tested with 18F4620, dsPIC33FJ256GP710	
* Hardware:		PICDEM Z, Explorer 16
* Complier:     Microchip C18 v3.04 or higher
*				Microchip C30 v2.03 or higher
* Company:		Microchip Technology, Inc.
*
* Software License Agreement
*
* This software is owned by Microchip Technology Inc. ("Microchip") 
* and is supplied to you for use exclusively as described in the 
* associated software agreement.  This software is protected by 
* software and other intellectual property laws.  Any use in 
* violation of the software license may subject the user to criminal 
* sanctions as well as civil liability.  Copyright 2006 Microchip
* Technology Inc.  All rights reserved.
*
* This software is provided "AS IS."  MICROCHIP DISCLAIMS ALL 
* WARRANTIES, EXPRESS, IMPLIED, STATUTORY OR OTHERWISE, NOT LIMITED 
* TO MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND 
* INFRINGEMENT.  Microchip shall in no event be liable for special, 
* incidental, or consequential damages.
*
*********************************************************************
* File Description:
*
*  This header file allows usage of the console functions located
*   in Console.c
*
* Change History:
*  Rev   Date         Description
*  0.1   11/09/2006   Initial revision
*  1.0   01/09/2007   Initial release
********************************************************************/
#ifndef  _CONSOLE_H_
#define  _CONSOLE_H_

/************************ HEADERS **********************************/
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "SystemProfile.h"

/************************ DEFINITIONS ******************************/
/************************ FUNCTION PROTOTYPES **********************/
#if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
    #if defined(ENABLE_CONSOLE)
        void ConsoleInit(void);
        //#define ConsoleIsPutReady()     (TRMT)
        void ConsolePut(BYTE c);
        //void ConsolePutString(BYTE *s);
        void ConsolePutROMString(ROM char* str);
    
        //#define ConsoleIsGetReady()     (RCIF)
        //BYTE ConsoleGet(void);
        //BYTE ConsoleGetString(char *buffer, BYTE bufferLen);
        void PrintChar(BYTE);
    #else
        #define ConsoleInit()
        #define ConsoleIsPutReady() 1
        #define ConsolePut(c)
        #define ConsolePutString(s)
        #define ConsolePutROMString(str)

        #define ConsoleIsGetReady() 1
        #define ConsoleGet()        'a'
        #define ConsoleGetString(buffer, bufferLen) 0
        #define PrintChar(a)
    #endif
#elif defined(__18CXX)

    #if defined(ENABLE_CONSOLE)   // Useful for disabling the console (saving power)
        void ConsoleInit(void);
        #define ConsoleIsPutReady()     (TXSTAbits.TRMT)
        void ConsolePut(BYTE c);
        void ConsolePutString(BYTE *s);
        void ConsolePutROMString(ROM char* str);
    
        #define ConsoleIsGetReady()     (PIR1bits.RCIF)
        BYTE ConsoleGet(void);
        BYTE ConsoleGetString(char *buffer, BYTE bufferLen);
        void PrintChar(BYTE);
    #else
        #define ConsoleInit()
        #define ConsoleIsPutReady() 1
        #define ConsolePut(c)
        #define ConsolePutString(s)
        #define ConsolePutROMString(str)
    
        #define ConsoleIsGetReady() 1
        #define ConsoleGet()        'a'
        #define ConsoleGetString(buffer, bufferLen) 0
        #define PrintChar(a)
    #endif
#elif defined(__PICC__)
    #if defined(ENABLE_CONSOLE)   // Useful for disabling the console (saving power)
        void ConsoleInit(void);
        #define ConsoleIsPutReady()     (TRMT)
        void ConsolePut(BYTE c);
        void ConsolePutString(BYTE *s);
        void ConsolePutROMString(ROM char* str);
    
        #define ConsoleIsGetReady()     (RCIF)
        BYTE ConsoleGet(void);
        BYTE ConsoleGetString(char *buffer, BYTE bufferLen);
        void PrintChar(BYTE);
    #else
        #define ConsoleInit()
        #define ConsoleIsPutReady() 1
        #define ConsolePut(c)
        #define ConsolePutString(s)
        #define ConsolePutROMString(str)
    
        #define ConsoleIsGetReady() 1
        #define ConsoleGet()        'a'
        #define ConsoleGetString(buffer, bufferLen) 0
        #define PrintChar(a)
    #endif
#else
#error Unknown processor.  See Compiler.h
#endif
#endif


