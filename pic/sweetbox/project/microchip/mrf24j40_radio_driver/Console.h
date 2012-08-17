/*********************************************************************
 *
 *                  Console Routines
 *
 *********************************************************************
 * FileName:        Console.h
 * Dependencies:
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
 *
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     7/12/04 Rel 0.9
 * Nilesh Rajbharti     11/1/04 Pre-release version
 * DF/KO                04/29/05 Microchip ZigBee Stack v1.0-2.0
 * DF/KO                07/18/05 Microchip ZigBee Stack v1.0-3.0
 * DF/KO                07/27/05 Microchip ZigBee Stack v1.0-3.1
 * DF/KO                08/19/05 Microchip ZigBee Stack v1.0-3.2
 * DF/KO                09/08/05 Microchip ZigBee Stack v1.0-3.3
 * DF/KO                01/09/06 Microchip ZigBee Stack v1.0-3.5
 ********************************************************************/
#ifndef  _CONSOLE_H_
#define _CONSOLE_H_

#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "BasicDriverDefs.h"

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
    #define c(a) ConsolePutROMString((rom char*)a);
    #define nl() ConsolePutROMString((rom char*)"\r\n");
    BYTE ConsoleInput(void);
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

#endif


