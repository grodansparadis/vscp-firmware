/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  ST7529 LCD controllers driver
 *****************************************************************************
 * FileName:        ST7529.h
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
 * Anton Alkhimenok     03/20/08
 *****************************************************************************/
#ifndef _ST7529_H
#define _ST7529_H

#include <p24Fxxxx.h>

#include "GraphicsConfig.h"
#include "GenericTypeDefs.h"

/*********************************************************************
* Overview: Uncomment this line to rotate image on 180 deg
*********************************************************************/
#define ROTATE_180

/*********************************************************************
* Overview: Additional hardware-accelerated functions can be implemented
*           in the driver. These definitions exclude the PutPixel()-based
*           functions in the primitives layer (Primitive.c file) from compilation.
*********************************************************************/

// Define this to implement Font related functions in the driver.
//#define USE_DRV_FONT

// Define this to implement Line function in the driver.
//#define USE_DRV_LINE

// Define this to implement Circle function in the driver.
//#define USE_DRV_CIRCLE

// Define this to implement FillCircle function in the driver.
//#define USE_DRV_FILLCIRCLE

// Define this to implement Bar function in the driver.
//#define USE_DRV_BAR

// Define this to implement ClearDevice function in the driver.
#define USE_DRV_CLEARDEVICE

// Define this to implement PutImage function in the driver.
#define USE_DRV_PUTIMAGE

/*********************************************************************
* Overview: Horizontal and vertical screen size.
*********************************************************************/
// Defines the horizontal screen size. Dependent on the display glass used.
#define SCREEN_HOR_SIZE    256
// Defines the vertical screen size. Dependent on the display glass used.
#define SCREEN_VER_SIZE    128

// Norm commands

#define EXTIN       0x30
#define EXTOUT      0x31
#define DISON       0xaf
#define DISOFF      0xae
#define DISNOR      0xa6
#define DISINV      0xa7
#define COMSCN      0xbb
#define DISCTRL     0xca
#define SLPIN       0x95
#define SLPOUT      0x94
#define LASET       0x75
#define CASET       0x15
#define DATSDR      0xbc
#define RAMWR       0x5c
#define RAMRD       0x5d
#define PTLIN       0xa8
#define PTLOUT      0xa9
#define RMWIN       0xe0
#define RMWOUT      0xee
#define ASCSET      0xaa
#define SCSTART     0xab
#define OSCON       0xd1
#define OSCOFF      0xd2
#define PWRCTRL     0x20
#define VOLCTRL     0x81
#define VOLUP       0xd6
#define VOLDOWN     0xd7
#define EPSRRD1     0x7c
#define EPSRRD2     0x7d
#define NOP         0x25
#define EPINT       0x07

// Ext commands

#define GRAY1SET    0x20
#define GRAY2SET    0x21
#define ANASET      0x32
#define SWINT       0x34
#define EPCTIN      0xcd
#define EPCOUT      0xcc
#define EPMWR       0xfc
#define EPMRD       0xfd


/*********************************************************************
* Overview: Clipping region control codes to be used with SetClip(...)
*           function. 
*********************************************************************/
#define CLIP_DISABLE       0 	// Disables clipping.
#define CLIP_ENABLE        1	// Enables clipping.

/*********************************************************************
* Overview: Some basic colors definitions.
*********************************************************************/

#define BLACK                0
#define GRAY1         	    (2<<3)
#define GRAY2               (4<<3)
#define GRAY3               (6<<3)
#define GRAY4               (8<<3)
#define GRAY5               (10<<3)
#define GRAY6	            (12<<3)
#define GRAY7       	    (14<<3)
#define GRAY8         	    (16<<3)
#define GRAY9               (18<<3)
#define GRAY10              (20<<3)
#define GRAY11              (22<<3)
#define GRAY12              (24<<3)
#define GRAY13	            (26<<3)
#define GRAY14	            (28<<3)
#define GRAY15	            (30<<3)
#define WHITE               (31<<3)
                            
// Definitions for reset pin
#define RST_TRIS_BIT       TRISCbits.TRISC1
#define RST_LAT_BIT        LATCbits.LATC1

// Definitions for RS pin
#define RS_TRIS_BIT        TRISBbits.TRISB15
#define RS_LAT_BIT         LATBbits.LATB15

// Definitions for CS pin
#define CS_TRIS_BIT        TRISDbits.TRISD8
#define CS_LAT_BIT         LATDbits.LATD8

// Color
extern BYTE _color;

/*********************************************************************
* Overview: Clipping region control and border settings.
*
*********************************************************************/
// Clipping region enable control
extern SHORT _clipRgn;

// Left clipping region border
extern SHORT _clipLeft;
// Top clipping region border
extern SHORT _clipTop;
// Right clipping region border
extern SHORT _clipRight;
// Bottom clipping region border
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
* Macros:  WriteCmd(command)
*
* Overview: Writes a command.
*
* PreCondition: none
*
* Input: command
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define WriteCmd(command)  RS_LAT_BIT=0;PMDIN1=command;PMPWaitBusy();RS_LAT_BIT=1;

/*********************************************************************
* Macros:  WriteData(data)
*
* Overview: Writes data.
*
* PreCondition: none
*
* Input: data
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define WriteData(data)   PMDIN1=data;PMPWaitBusy();

/*********************************************************************
* Macros:  ReadData()
*
* Overview: Reads data.
*
* PreCondition: none
*
* Input: none
*
* Output: data
*
* Side Effects: none
*
********************************************************************/
#define ReadData()   PMDIN1;PMPWaitBusy();

/*********************************************************************
* Function:  void ResetDevice()
*
* Overview: Initializes LCD module.
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
void ResetDevice(void);

/*********************************************************************
* Macros:  GetMaxX()
*
* Overview: Returns maximum horizontal coordinate.
*
* PreCondition: none
*
* Input: none
*
* Output: Maximum horizontal coordinate.
*
* Side Effects: none
*
********************************************************************/
#define GetMaxX() (SCREEN_HOR_SIZE-1)

/*********************************************************************
* Macros:  GetMaxY()
*
* Overview: Returns maximum vertical coordinate.
*
* PreCondition: none
*
* Input: none
*
* Output: Maximum vertical coordinate.
*
* Side Effects: none
*
********************************************************************/
#define GetMaxY() (SCREEN_VER_SIZE-1)

/*********************************************************************
* Macros:  SetColor(color)
*
* Overview: Sets current drawing color.
*
* PreCondition: none
*
* Input: color - Color coded in 5:6:5 RGB format.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetColor(color) _color = (BYTE)color;

/*********************************************************************
* Macros:  GetColor()
*
* Overview: Returns current drawing color.
*
* PreCondition: none
*
* Input: none
*
* Output: Color coded in 5:6:5 RGB format.
*
* Side Effects: none
*
********************************************************************/
#define GetColor() _color

/*********************************************************************
* Macros:  SetActivePage(page)
*
* Overview: Sets active graphic page.
*
* PreCondition: none
*
* Input: page - Graphic page number.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetActivePage(page)

/*********************************************************************
* Macros: SetVisualPage(page)
*
* Overview: Sets graphic page to display.
*
* PreCondition: none
*
* Input: page - Graphic page number
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetVisualPage(page)

/*********************************************************************
* Function: void PutPixel(SHORT x, SHORT y)
*
* Overview: Puts pixel with the given x,y coordinate position.
*
* PreCondition: none
*
* Input: x - x position of the pixel.
*		 y - y position of the pixel.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void PutPixel(SHORT x, SHORT y);

/*********************************************************************
* Function: WORD GetPixel(SHORT x, SHORT y)
*
* Overview: Returns pixel color at the given x,y coordinate position.
*
* PreCondition: none
*
* Input: x - x position of the pixel.
*		 y - y position of the pixel.
*
* Output: pixel color
*
* Side Effects: none
*
********************************************************************/
WORD GetPixel(SHORT x, SHORT y);

/*********************************************************************
* Macros: SetClipRgn(left, top, right, bottom)
*
* Overview: Sets clipping region.
*
* PreCondition: none
*
* Input: left - Defines the left clipping region border.
*		 top - Defines the top clipping region border.
*		 right - Defines the right clipping region border.
*	     bottom - Defines the bottom clipping region border.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetClipRgn(left,top,right,bottom) _clipLeft=left; _clipTop=top; _clipRight=right; _clipBottom=bottom;

/*********************************************************************
* Macros: GetClipLeft()
*
* Overview: Returns left clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Left clipping border.
*
* Side Effects: none
*
********************************************************************/
#define GetClipLeft() _clipLeft

/*********************************************************************
* Macros: GetClipRight()
*
* Overview: Returns right clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Right clipping border.
*
* Side Effects: none
*
********************************************************************/
#define GetClipRight() _clipRight

/*********************************************************************
* Macros: GetClipTop()
*
* Overview: Returns top clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Top clipping border.
*
* Side Effects: none
*
********************************************************************/
#define GetClipTop() _clipTop

/*********************************************************************
* Macros: GetClipBottom()
*
* Overview: Returns bottom clipping border.
*
* PreCondition: none
*
* Input: none
*
* Output: Bottom clipping border.
*
* Side Effects: none
*
********************************************************************/
#define GetClipBottom() _clipBottom

/*********************************************************************
* Macros: SetClip(control)
*
* Overview: Enables/disables clipping.
*
* PreCondition: none
*
* Input: control - Enables or disables the clipping.
*			- 0: Disable clipping
*			- 1: Enable clipping
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetClip(control) _clipRgn=control;

/*********************************************************************
* Macros: IsDeviceBusy()
*
* Overview: Returns non-zero if LCD controller is busy 
*           (previous drawing operation is not completed).
*
* PreCondition: none
*
* Input: none
*
* Output: Busy status.
*
* Side Effects: none
*
********************************************************************/
#define IsDeviceBusy()  0

/*********************************************************************
* Macros: SetPalette(colorNum, color)
*
* Overview:  Sets palette register.
*
* PreCondition: none
*
* Input: colorNum - Register number.
*        color - Color.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetPalette(colorNum, color)

/*********************************************************************
* Function:  void  DelayMs(WORD time)
*
* Overview: Delays execution on time specified in milliseconds.
*           The delay is correct only for 16MIPS.
*
* PreCondition: none
*
* Input: time - Delay in milliseconds.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void DelayMs(WORD time);

/*********************************************************************
* Function: void ContrastSet(WORD contrast)
*
* PreCondition: none
*
* Input: contrast value (LSB 10-0 are valid, MSB 15-11 are not used)
*
* Output: none
*
* Side Effects: none
*
* Overview: sets contrast
*
* Note: none
*
********************************************************************/
void ContrastSet(WORD contrast);

/*********************************************************************
* Function: void ContrastUp(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: increases contrast
*
* Note: none
*
********************************************************************/
void ContrastUp(void);

/*********************************************************************
* Function: void ContrastDown(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: decreases contrast
*
* Note: none
*
********************************************************************/
void ContrastDown(void);

/*********************************************************************
* Function: void DisplayOn(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: turns on display
*
* Note: none
*
********************************************************************/
void DisplayOn(void);

/*********************************************************************
* Function: void DisplayOn(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: turns off display
*
* Note: none
*
********************************************************************/
void DisplayOff(void);

/*********************************************************************
* Function: void SleepIn(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: enters sleep mode
*
* Note: none
*
********************************************************************/
void SleepIn(void);

/*********************************************************************
* Function: void WakeUp(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: wakes up from sleep
*
* Note: none
*
********************************************************************/
void WakeUp(void);

#endif // _ST7529_H
