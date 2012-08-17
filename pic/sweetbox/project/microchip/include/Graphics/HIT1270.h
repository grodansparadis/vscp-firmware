/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  Densitron HIT1270 LCD controller driver 
 *  Landscape orientation only
 *****************************************************************************
 * FileName:        HIT1270.h
 * Dependencies:    p24Fxxxx.h
 * Processor:       PIC24
 * Compiler:       	MPLAB C30
 * Linker:          MPLAB LINK30
 * Company:         Microchip Technology Incorporated
 *
 * Software License Agreement
 *
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * Microchip licenses to you the right to use, modify, copy and distribute
 * Software only when embedded on a Microchip microcontroller or digital
 * signal controller, which is integrated into your product or third party
 * product (pursuant to the sublicense terms in the accompanying license
 * agreement).  
 *
 * You should refer to the license agreement accompanying this Software
 * for additional information regarding your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
 * OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
 * BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
 * COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
 * CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
 * OR OTHER SIMILAR COSTS.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok     11/12/07	Version 1.0 release
 *****************************************************************************/
#ifndef _HIT1270L_H
#define _HIT1270L_H

#include <p24Fxxxx.h>
#include "GraphicsConfig.h"
#include "GenericTypeDefs.h"

// Additional hardware-accelerated functions implemented in the driver.
// These definitions exclude the PutPixel()-based functions from compilation 
// in the primitives layer (Primitive.c file).

//#define USE_DRV_FONT
//#define USE_DRV_LINE
//#define USE_DRV_CIRCLE
//#define USE_DRV_FILLCIRCLE
#define USE_DRV_BAR
#define USE_DRV_CLEARDEVICE
#define USE_DRV_PUTIMAGE


// Horizontal and vertical screen size
#define SCREEN_HOR_SIZE    320
#define SCREEN_VER_SIZE    234

// Memory pitch for line
#define LINE_MEM_PITCH     320
// Video buffer offset
#define BUF_MEM_OFFSET    (DWORD)0x18000 

// Definitions for reset pin
#define RST_TRIS_BIT       TRISCbits.TRISC1
#define RST_LAT_BIT        LATCbits.LATC1

// Definitions for CS pin
#define CS_TRIS_BIT        TRISDbits.TRISD8
#define CS_LAT_BIT         LATDbits.LATD8

// Definitions for wait pin
#define WAIT_TRIS_BIT      TRISCbits.TRISC2
#define WAIT_PORT_BIT      PORTCbits.RC2

// Definitions for FLASH CS pin 
#define CS_FLASH_LAT_BIT   LATDbits.LATD9
#define CS_FLASH_TRIS_BIT  TRISDbits.TRISD9

// Clipping region control codes
#define CLIP_DISABLE       0 
#define CLIP_ENABLE        1

// HIT1270 commands
#define SET_DATA_POINTER   0x50
#define SRAM_WRITE         0x51
#define STAMP_ICON         0x52
#define FILL_BLOCK         0x53

#define FLASH_WRITE        0x60
#define FLASH_READ         0x61

// Color codes

#define BLACK              (WORD)0x0000
#define BRIGHTBLUE         (WORD)0x001f
#define BRIGHTGREEN        (WORD)0x07e0
#define BRIGHTCYAN         (WORD)0x07ff
#define BRIGHTRED          (WORD)0xf800
#define BRIGHTMAGENTA      (WORD)0xf81f
#define BRIGHTYELLOW       (WORD)0xffe0
#define BLUE               (WORD)0x0010
#define GREEN              (WORD)0x0400
#define CYAN               (WORD)0x0410
#define RED                (WORD)0x8000
#define MAGENTA            (WORD)0x8010
#define BROWN              (WORD)0xfc00
#define LIGHTGRAY          (WORD)0x8410
#define DARKGRAY           (WORD)0x4208
#define LIGHTBLUE          (WORD)0x841f
#define LIGHTGREEN         (WORD)0x87f0
#define LIGHTCYAN          (WORD)0x87ff
#define LIGHTRED           (WORD)0xfc10
#define LIGHTMAGENTA       (WORD)0xfc1f
#define YELLOW             (WORD)0xfff0
#define WHITE              (WORD)0xffff

#define GRAY0       	   (WORD)0xe71c
#define GRAY1         	   (WORD)0xc618
#define GRAY2              (WORD)0xa514
#define GRAY3              (WORD)0x8410
#define GRAY4              (WORD)0x630c
#define GRAY5              (WORD)0x4208
#define GRAY6	           (WORD)0x2104

// Color
extern WORD_VAL _color;

// Clipping region control
extern SHORT _clipRgn;

// Clipping region borders
extern SHORT _clipLeft;
extern SHORT _clipTop;
extern SHORT _clipRight;
extern SHORT _clipBottom;

/*********************************************************************
* Macros:  PMPWaitBusy()
*
* Overview: Delays execution for PMP cycle time.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define PMPWaitBusy()  while(PMMODEbits.BUSY);

/*********************************************************************
* Macros:  WriteData(byte1, byte0)
*
* PreCondition: none
*
* Input: byte1,byte0 - data bytes
*
* Output: none
*
* Side Effects: none
*
* Overview: writes S6D0129 data
*
* Note: none
*
********************************************************************/
#define WriteData(byte1,byte0)\
PMADDR=0x0000;PMDIN1=SRAM_WRITE;PMPWaitBusy();\
PMADDR=0x0001;PMDIN1=byte0;PMPWaitBusy();PMDIN1=byte1;PMPWaitBusy();

/*********************************************************************
* Macros:  SetAddress(addr2,addr1,addr0)
*
* PreCondition: none
*
* Input: addr0,addr1,addr2 - address bytes
*
* Output: none
*
* Side Effects: none
*
* Overview: writes S6D0129 address pointer
*
* Note: none
*
********************************************************************/
#define SetAddress(addr2,addr1,addr0)\
PMADDR=0x0000;PMDIN1=SET_DATA_POINTER;PMPWaitBusy();\
PMADDR=0x0001;PMDIN1=addr0;PMPWaitBusy();\
PMDIN1=addr1;PMPWaitBusy();PMDIN1=addr2;PMPWaitBusy();

/*********************************************************************
* Function:  void ResetDevice()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: resets device, initialize PMP
*
* Note: none
*
********************************************************************/
void ResetDevice(void);

/*********************************************************************
* Macros:  GetMaxX()
*
* PreCondition: none
*
* Input: none
*
* Output: maximum horizontal coordinate
*
* Side Effects: none
*
* Overview: returns maximum horizontal coordinate
*
* Note: none
*
********************************************************************/
#define GetMaxX() (SCREEN_HOR_SIZE-1)

/*********************************************************************
* Macros:  GetMaxY()
*
* PreCondition: none
*
* Input: none
*
* Output: maximum vertical coordinate
*
* Side Effects: none
*
* Overview: returns maximum vertical coordinate
*
* Note: none
*
********************************************************************/
#define GetMaxY() (SCREEN_VER_SIZE-1)

/*********************************************************************
* Function:  void SetColor(WORD color)
*
* PreCondition: none
*
* Input: color coded in format:
*           bits 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00 
*          color  R  R  R  R  R  G  G  G  G  G  G  B  B  B  B  B
*
* Output: none
*
* Side Effects: none
*
* Overview: sets current color
*
* Note: none
*
********************************************************************/
void SetColor(WORD color);

/*********************************************************************
* Macros:  GetColor()
*
* PreCondition: none
*
* Input: none
*
* Output:  color coded in format:
*           bits 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00 
*          color  R  R  R  R  R  G  G  G  G  G  G  B  B  B  B  B
*
* Side Effects: none
*
* Overview: returns current color
*
* Note: none
*
********************************************************************/
#define GetColor() _color.Val

/*********************************************************************
* Macros:  SetActivePage(page)
*
* PreCondition: none
*
* Input: graphic page number
*
* Output: none
*
* Side Effects: none
*
* Overview: sets active graphic page 
*
* Note: the controller has only page
*
********************************************************************/
#define SetActivePage(page)

/*********************************************************************
* Macros: SetVisualPage(page)
*
* PreCondition: none
*
* Input: graphic page number
*
* Output: none
*
* Side Effects: none
*
* Overview: sets graphic page to display
*
* Note: the controller has only page
*
********************************************************************/
#define SetVisualPage(page)

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* PreCondition: none
*
* Input: x,y - pixel coordinates 
*
* Output: none
*
* Side Effects: none
*
* Overview: puts pixel
*
* Note: none
*
********************************************************************/
void PutPixel(SHORT x, SHORT y);

/*********************************************************************
* Macros: GetPixel(x, y)
*
* PreCondition: none
*
* Input: x,y - pixel coordinates 
*
* Output: pixel color
*
* Side Effects: none
*
* Overview: returns pixel color at x,y position
*
* Note: this glass doesn't support reading
*
********************************************************************/
#define GetPixel(x, y) 0

/*********************************************************************
* Macros: SetClipRgn(left, top, right, bottom)
*
* PreCondition: none
*
* Input: left,top,right,bottom - clipping region borders
*
* Output: none
*
* Side Effects: none
*
* Overview: sets clipping region 
*
* Note: none
*
********************************************************************/
#define SetClipRgn(left,top,right,bottom) _clipLeft=left; _clipTop=top; _clipRight=right; _clipBottom=bottom;

/*********************************************************************
* Macros: GetClipLeft()
*
* PreCondition: none
*
* Input: none
*
* Output: left clipping border
*
* Side Effects: none
*
* Overview: returns left clipping border
*
* Note: none
*
********************************************************************/
#define GetClipLeft() _clipLeft

/*********************************************************************
* Macros: GetClipRight()
*
* PreCondition: none
*
* Input: none
*
* Output: right clipping border
*
* Side Effects: none
*
* Overview: returns right clipping border
*
* Note: none
*
********************************************************************/
#define GetClipRight() _clipRight

/*********************************************************************
* Macros: GetClipTop()
*
* PreCondition: none
*
* Input: none
*
* Output: top clipping border
*
* Side Effects: none
*
* Overview: returns top clipping border
*
* Note: none
*
********************************************************************/
#define GetClipTop() _clipTop

/*********************************************************************
* Macros: GetClipBottom()
*
* PreCondition: none
*
* Input: none
*
* Output: bottom clipping border
*
* Side Effects: none
*
* Overview: returns bottom clipping border
*
* Note: none
*
********************************************************************/
#define GetClipBottom() _clipBottom

/*********************************************************************
* Macros: SetClip(control)
*
* PreCondition: none
*
* Input: control - 0 disable/ 1 enable
*
* Output: none
*
* Side Effects: none
*
* Overview: enables/disables clipping 
*
* Note: none
*
********************************************************************/
#define SetClip(control) _clipRgn=control;

/*********************************************************************
* Macros: IsDeviceBusy()
*
* PreCondition: none
*
* Input: none
*
* Output: busy status
*
* Side Effects: none
*
* Overview:  returns non-zero if LCD controller is busy 
* (previous drawing operation is not complete).
*
*
********************************************************************/
#define IsDeviceBusy()  (WAIT_PORT_BIT == 0) 

/*********************************************************************
* Macros: SetPalette(colorNum, color)
*
* PreCondition: none
*
* Input: colorNum - register number, color - color
*
* Output: none
*
* Side Effects: none
*
* Overview:  sets palette register
*
* Note: S6D0129 has no palette
*
********************************************************************/
#define SetPalette(colorNum, color)

/*********************************************************************
* Function:  void  DelayMs(WORD time)
*
* PreCondition: none
*
* Input: time - delay in ms
*
* Output: none
*
* Side Effects: none
*
* Overview: delays execution on time specified in ms
*
* Note: delay is defined for 16MIPS
*
********************************************************************/
void DelayMs(WORD time);


#endif // HIT1270L.h
