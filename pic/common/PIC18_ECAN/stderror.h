/******************************************************************************
*                                                                             *
*  File:                       STDERROR.H                                     *
*                                                                             *
*  Description: This file contains the global error code definitions for all  *
*               of the LDWS subsystems to reduce code size and increase error *
*               processing speed because error conversions are not required.  *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Software License Agreement                                                 *
*                                                                             *
*  The software supplied herewith by Microchip Technology Incorporated        *
*  (the “Company”) for its PICmicro® Microcontroller is intended and          *
*  supplied to you, the Company’s customer, for use solely and                *
*  exclusively on Microchip PICmicro Microcontroller products. The            *
*  software is owned by the Company and/or its supplier, and is               *
*  protected under applicable copyright laws. All rights are reserved.        *
*   Any use in violation of the foregoing restrictions may subject the        *
*  user to criminal sanctions under applicable laws, as well as to            *
*  civil liability for the breach of the terms and conditions of this         *
*  license.                                                                   *
*                                                                             *
*  THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,          *
*  WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED          *
*  TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A                *
*  PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,          *
*  IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR                 *
*  CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.                          *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Created By:  Thomas R. Shelburne 03/04/02                                  *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*    1.0   - T.R.S. - Initial release of the standard error subsystem...      *
*                                                                             *
******************************************************************************/

/* Only declare the API functions as external for calling modules... */
#ifdef _STDERROR_H_
#define GLOBAL
#else
#define GLOBAL extern
#endif

/******************************************************************************
*                        ERROR TYPEDEF (16-bit)                               *
******************************************************************************/
#ifndef STDERROR
#define STDERROR unsigned short
#endif

/******************************************************************************
*                        ERROR MACROS                                         *
******************************************************************************/
#define ERR(s,t)      (((STDERROR)s)+((STDERROR)t))
#define GET_ERROR(v)  (((STDERROR)v)&0x00FF)
#define GET_SOURCE(v) (((STDERROR)v)&0xFF00)

/******************************************************************************
*                        ERROR SOURCES (MSB ONLY)                             *
******************************************************************************/
#define GENERIC               0x0000
#define DWCAN                 0x0100
#define RAM                   0x0200
#define EEPROM                0x0300
#define FLASH                 0x0400
#define TIMER                 0x0500

/******************************************************************************
*                        ERROR TYPES (LSB ONLY)                               *
******************************************************************************/
#define SUCCESS               0x0000 /* The general OK reponse...            */
#define FAILURE               0x0001 /* The general failure response...      */
#define DISABLED              0x0002
#define READ_FAILED           0x0003
#define WRITE_FAILED          0x0004
#define QUEUE_FULL            0x0005
#define QUEUE_EMPTY           0x0006
#define INVALID_PTR           0x0007
#define INVALID_PARAM         0x0008
#define BUFFER_FULL           0x0009

#undef GLOBAL
