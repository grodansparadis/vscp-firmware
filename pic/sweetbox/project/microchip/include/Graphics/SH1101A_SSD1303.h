/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  Sino Wealth Microelectronic SH1101A OLED controller driver 
 *  Solomon Systech SSD1303 LCD controller driver 
  *****************************************************************************
 * FileName:        SH1101A_SSD1303.h
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
 * Rodger Richey		03/10/07	Original
 * Paolo Tamayo			12/20/07	Ported to PIC24 Kit
 *****************************************************************************/
#ifndef _SH1101A_SSD1303_OLED_H
#define _SH1101A_SSD1303_OLED_H

#ifdef __PIC24F__
#include <p24Fxxxx.h>
#else
#error CONTROLLER IS NOT SUPPORTED
#endif
#include "GraphicsConfig.h"
#include "GenericTypeDefs.h"

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
//#define USE_DRV_PUTIMAGE

/*********************************************************************
* Overview: Horizontal and vertical screen size.
*********************************************************************/
#ifdef  USE_PORTRAIT
#error THE DRIVER DOES SUPPORT PORTRAIT MODE
#else

/*********************************************************************
* Overview: Graphics controller chip.
*
*********************************************************************/
#define SH1101A
//#define SSD1303

// Defines the display offset in x direction. Dependent on the display 
// used and how it is connected.
#define OFFSET				2

// Defines the horizontal screen size. Dependent on the display glass used.
#define SCREEN_HOR_SIZE    128

// Defines the vertical screen size. Dependent on the display glass used.
#define SCREEN_VER_SIZE    64
#endif
/*********************************************************************
* Overview: Clipping region control codes to be used with SetClip(...)
*           function. 
*********************************************************************/
#define CLIP_DISABLE       0 	// Disables clipping.
#define CLIP_ENABLE        1	// Enables clipping.

/*********************************************************************
* Overview: Screen Saver parameters. 
*			- SSON - Means that screen saver will be enabled when 
*					 ScreenSaver(SSON) function is called with SSON as 
*					 parameter.
*			- SSOFF - Means that screen saver will be disbled when 
*					 ScreenSaver(SSOFF) function is called with SSOFF as 
*					 parameter.
*           
*********************************************************************/
#define SSON				1	// screen saver is turned on
#define	SSOFF				0	// screen saver is turned off

/*********************************************************************
* Overview: Color definitions.
*********************************************************************/

#define BLACK              (WORD)0b00000000
#define WHITE              (WORD)0b11111111

// Memory pitch for line
#define LINE_MEM_PITCH		0x100 

// Definitions for reset pin
#define RST_TRIS_BIT       TRISDbits.TRISD2
#define RST_LAT_BIT        LATDbits.LATD2

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
* Macros:  PMPDelay()
*
* Overview: Delays execution for PMP cycle time. This is dependent on 
*			processor clock. 
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
// at 32 MHZ 1 nop is at 32.25 ns. Controller needs 300 ns minimum delay from
// last CS valid to next CS valid. Thus we have the 7 nops (227.5 ns).
// PMP access at this clock frequency is 3 cycles (97.5 ns).
// 		therefore: 227.5 + 97.5 > 300 
//   			   verify: 	300 - 97.5 = 202.5
//				   			202.5/32.5 = 6.23 thus we can use 7 nops.	 				
#define PMPDelay()  Nop(); Nop(); Nop(); Nop(); Nop(); Nop(); Nop();  

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
#ifdef __PIC24F__
#define PMPWaitBusy()  Nop(); 
#else
#error CONTROLLER IS NOT SUPPORTED
#endif

/*********************************************************************
* Macros:  WriteCommand()
*
* Overview: Writes command word to the display controller. A delay
*			is inserted at the end to meet the controller requirements
*			on selected commands.
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
#define WriteCommand(cmd)		PMADDR=0x4000;PMPWaitBusy();PMDIN1=cmd;PMPDelay();

/*********************************************************************
* Macros:  WriteData(writeByte)
*
* Overview: Writes data.
*
* PreCondition: none
*
* Input: writeByte - Data byte to be written.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define WriteData(writeByte) 	PMADDR=0x4001;PMPWaitBusy();PMDIN1=writeByte;PMPDelay();

/*********************************************************************
* Macros:  ReadData(readByte)
*
* Overview: Reads a byte from the display buffer. This assumes that the 
*			page value, lower and higher column address pointers are already set.
*
* PreCondition: Page, lower and higher column address pointers are already set.
*
* Input: none.
*
* Output: ReadByte - data read from the display buffer.
*
* Side Effects: none
*
********************************************************************/
#define ReadData(readByte)		PMADDR=0x4001;PMPWaitBusy();readByte=PMDIN1;PMPDelay();

/*********************************************************************
* Macros:  SetAddress(lowerAddr,higherAddr)
*
* Overview: Sets the page and the lower and higher address pointer
*			of the display buffer. All parameters should be pre-calculated.
*
* PreCondition: none
*
* Input: page - Page value for the address.
*		 lowerAddr - Lower column address.
*		 higherAddr - Higher column address.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetAddress(page,lowerAddr,higherAddr)\
	WriteCommand(page);\
	WriteCommand(lowerAddr);\
	WriteCommand(higherAddr);

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
#define SetColor(color) _color = color;

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
BYTE GetPixel(SHORT x, SHORT y);

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

#endif // _SH1101A_SSD1303_OLED_H
