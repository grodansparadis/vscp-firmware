/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  Solomon Systech. SSD1906 LCD controllers driver.
 *****************************************************************************
 * FileName:        SSD1906.h
 * Dependencies:    p24Fxxxx.h or plib.h
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
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
 * Anton Alkhimenok     01/29/08
 * Anton Alkhimenok     01/31/08    combined portrait and landscape, PIC32 support
 * Anton Alkhimenok     08/22/08    driver configuration is simplified,
 *                                  new image rotation modes
 *****************************************************************************/

#ifndef _SSD1906_H
#define _SSD1906_H

#ifdef __PIC32MX
#include <plib.h>
#define PMDIN1   PMDIN
#else
#ifdef __PIC24F__
#include <p24Fxxxx.h>
#else
#error CONTROLLER IS NOT SUPPORTED
#endif
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
#define USE_DRV_BAR

// Define this to implement ClearDevice function in the driver.
#define USE_DRV_CLEARDEVICE

// Define this to implement PutImage function in the driver.
#define USE_DRV_PUTIMAGE

/*********************************************************************
* Overview: Horizontal and vertical display resolution
*                  (from the glass datasheet).
*********************************************************************/
#define DISP_HOR_RESOLUTION				240
#define DISP_VER_RESOLUTION				320

/*********************************************************************
* Overview: Image orientation (can be 0, 90, 180, 270 degrees).
*********************************************************************/
#ifdef USE_PORTRAIT
#define DISP_ORIENTATION				0
#else
#define DISP_ORIENTATION				90
#endif

/*********************************************************************
* Overview: Horizontal synchronization timing in pixels
*                  (from the glass datasheet).
*********************************************************************/
#define DISP_HOR_PULSE_WIDTH		    24
#define DISP_HOR_BACK_PORCH				8
#define DISP_HOR_FRONT_PORCH			8

/*********************************************************************
* Overview: Vertical synchronization timing in lines
*                  (from the glass datasheet).
*********************************************************************/
#define DISP_VER_PULSE_WIDTH		    3
#define DISP_VER_BACK_PORCH				1
#define DISP_VER_FRONT_PORCH			1

/*********************************************************************
* PARAMETERS VALIDATION
*********************************************************************/
#if COLOR_DEPTH != 16
#error This driver supports 16 BPP only.
#endif

#if (DISP_HOR_RESOLUTION % 8) != 0
#error Horizontal resolution must be divisible by 8.
#endif

#if ((DISP_HOR_PULSE_WIDTH+DISP_HOR_BACK_PORCH+DISP_HOR_FRONT_PORCH)%8) != 0
#error The sum of horizontal synchronization pulse width, front and back porches must be divisible by 8.
#endif

#if (DISP_ORIENTATION != 0) && (DISP_ORIENTATION != 180) && (DISP_ORIENTATION != 90) && (DISP_ORIENTATION != 270)
#error The display orientation selected is not supported. It can be only 0,90,180 or 270.
#endif

/*********************************************************************
* Overview: Clipping region control codes to be used with SetClip(...)
*           function. 
*********************************************************************/
#define CLIP_DISABLE       0 	// Disables clipping.
#define CLIP_ENABLE        1	// Enables clipping.

/*********************************************************************
* Overview: Some basic colors definitions.
*********************************************************************/

#define BLACK               RGB565CONVERT(0,    0,      0)
#define BRIGHTBLUE          RGB565CONVERT(0,    0,      255)
#define BRIGHTGREEN         RGB565CONVERT(0,    255,    0)
#define BRIGHTCYAN          RGB565CONVERT(0,    255,    255)
#define BRIGHTRED           RGB565CONVERT(255,  0,      0)
#define BRIGHTMAGENTA       RGB565CONVERT(255,  0,      255)
#define BRIGHTYELLOW        RGB565CONVERT(255,  255,    0)
#define BLUE                RGB565CONVERT(0,    0,      128)
#define GREEN               RGB565CONVERT(0,    128,    0)
#define CYAN                RGB565CONVERT(0,    128,    128)
#define RED                 RGB565CONVERT(128,  0,      0)
#define MAGENTA             RGB565CONVERT(128,  0,      128)
#define BROWN               RGB565CONVERT(255,  128,    0)
#define LIGHTGRAY           RGB565CONVERT(128,  128,    128)
#define DARKGRAY            RGB565CONVERT(64,   64,     64)
#define LIGHTBLUE           RGB565CONVERT(128,  128,    255)
#define LIGHTGREEN          RGB565CONVERT(128,  255,    128)
#define LIGHTCYAN           RGB565CONVERT(128,  255,    255)
#define LIGHTRED            RGB565CONVERT(255,  128,    128)
#define LIGHTMAGENTA        RGB565CONVERT(255,  128,    255)
#define YELLOW              RGB565CONVERT(255,  255,    128)
#define WHITE               RGB565CONVERT(255,  255,    255)
                            
#define GRAY0       	    RGB565CONVERT(224,  224,    224)
#define GRAY1         	    RGB565CONVERT(192,  192,    192)   
#define GRAY2               RGB565CONVERT(160,  160,    160)   
#define GRAY3               RGB565CONVERT(128,  128,    128)
#define GRAY4               RGB565CONVERT(96,   96,     96)
#define GRAY5               RGB565CONVERT(64,   64,     64)
#define GRAY6	            RGB565CONVERT(32,   32,     32)

// Development boards I/O definition

// Definitions for reset line
#define RST_TRIS_BIT       TRISCbits.TRISC1
#define RST_LAT_BIT        LATCbits.LATC1

// Definitions for RS line
#define RS_TRIS_BIT        TRISCbits.TRISC2
#define RS_LAT_BIT         LATCbits.LATC2

// Definitions for CS line
#define CS_TRIS_BIT        TRISDbits.TRISD10
#define CS_LAT_BIT         LATDbits.LATD10

// Definitions for A0 line
#define A0_LAT_BIT         LATDbits.LATD3
#define A0_TRIS_BIT        TRISDbits.TRISD3

// Definitions for A17 line 
#define A17_LAT_BIT        LATGbits.LATG14
#define A17_TRIS_BIT       TRISGbits.TRISG14

// SSD1906 registers names definitions

#define REG_DISP_BUFFER_SIZE           0x01
#define REG_CONFIG_READBACK            0x02
#define REG_REVISION_CODE              0x03
#define REG_MEMCLK_CONFIG       	   0x04
#define REG_PCLK_CONFIG                0x05
#define REG_LUT_BLUE_WRITE_DATA        0x08
#define REG_LUT_GREEN_WRITE_DATA       0x09
#define REG_LUT_RED_WRITE_DATA         0x0a
#define REG_LUT_WRITE_ADDR             0x0b
#define REG_LUT_BLUE_READ_DATA         0x0c
#define REG_LUT_GREEN_READ_DATA        0x0d
#define REG_LUT_RED_READ_DATA          0x0e
#define REG_LUT_READ_ADDR              0x0f
#define REG_PANEL_TYPE                 0x10
#define REG_MOD_RATE                   0x11
#define REG_HORIZ_TOTAL                0x12
#define REG_HDP                        0x14
#define REG_HDP_START_POS0             0x16
#define REG_HDP_START_POS1             0x17
#define REG_VERT_TOTAL0                0x18
#define REG_VERT_TOTAL1                0x19
#define REG_VDP0                       0x1c
#define REG_VDP1                       0x1d
#define REG_VDP_START_POS0             0x1e
#define REG_VDP_START_POS1             0x1f
#define REG_HSYNC_PULSE_WIDTH          0x20
#define REG_HSYNC_PULSE_START_POS0     0x22
#define REG_HSYNC_PULSE_START_POS1     0x23
#define REG_VSYNC_PULSE_WIDTH          0x24
#define REG_VSYNC_PULSE_START_POS0     0x26
#define REG_VSYNC_PULSE_START_POS1     0x27
 
#define REG_FPFRAME_START_OFFSET0	   0x30
#define REG_FPFRAME_START_OFFSET1	   0x31
#define REG_FPFRAME_STOP_OFFSET0	   0x34
#define REG_FPFRAME_STOP_OFFSET1	   0x35
#define REG_HRTFT_SPECIAL_OUTPUT	   0x38
#define REG_GPIO1_PULSE_START		   0x3c
#define REG_GPIO1_PULSE_STOP		   0x3e
#define REG_GPIO2_PULSE_DELAY 		   0x40
#define REG_STN_COLOR_DEPTH			   0x45
#define REG_DYN_DITHER_CONTROL		   0x50

#define REG_DISPLAY_MODE               0x70
#define REG_SPECIAL_EFFECTS            0x71
#define REG_MAIN_WIN_DISP_START_ADDR0  0x74
#define REG_MAIN_WIN_DISP_START_ADDR1  0x75
#define REG_MAIN_WIN_DISP_START_ADDR2  0x76
#define REG_MAIN_WIN_ADDR_OFFSET0      0x78
#define REG_MAIN_WIN_ADDR_OFFSET1      0x79
#define REG_FLOAT_WIN_DISP_START_ADDR0 0x7c
#define REG_FLOAT_WIN_DISP_START_ADDR1 0x7d
#define REG_FLOAT_WIN_DISP_START_ADDR2 0x7e
#define REG_FLOAT_WIN_ADDR_OFFSET0     0x80
#define REG_FLOAT_WIN_ADDR_OFFSET1     0x81
#define REG_FLOAT_WIN_X_START_POS0     0x84
#define REG_FLOAT_WIN_X_START_POS1     0x85
#define REG_FLOAT_WIN_Y_START_POS0     0x88
#define REG_FLOAT_WIN_Y_START_POS1     0x89
#define REG_FLOAT_WIN_X_END_POS0       0x8c
#define REG_FLOAT_WIN_X_END_POS1       0x8d
#define REG_FLOAT_WIN_Y_END_POS0       0x90
#define REG_FLOAT_WIN_Y_END_POS1       0x91
#define REG_POWER_SAVE_CONFIG          0xa0
#define REG_SOFTWARE_RESET             0xa2
#define REG_SCRATCH_PAD0               0xa4
#define REG_SCRATCH_PAD1               0xa5
#define REG_GPIO_CONFIG0               0xa8
#define REG_GPIO_CONFIG1               0xa9
#define REG_GPIO_STATUS_CONTROL0       0xac
#define REG_GPIO_STATUS_CONTROL1       0xad
#define REG_PWM_CV_CLOCK_CONTROL       0xb0
#define REG_PWM_CV_CLOCK_CONFIG        0xb1
#define REG_CV_CLOCK_BURST_LENGTH      0xb2
#define REG_PWM_CLOCK_DUTY_CYCLE       0xb3

#define REG_CURSOR_FEATURE			   0xc0
#define REG_CURSOR1_BLINK_TOTAL0	   0xc4
#define REG_CURSOR1_BLINK_TOTAL1	   0xc5
#define REG_CURSOR1_BLINK_ON0		   0xc8
#define REG_CURSOR1_BLINK_ON1		   0xc9
#define REG_CURSOR1_MEM_START0		   0xcc
#define REG_CURSOR1_MEM_START1		   0xcd
#define REG_CURSOR1_MEM_START2		   0xce
#define REG_CURSOR1_POSX0			   0xd0
#define REG_CURSOR1_POSX1			   0xd1
#define REG_CURSOR1_POSY0			   0xd4
#define REG_CURSOR1_POSY1			   0xd5
#define REG_CURSOR1_HORIZ_SIZE		   0xd8
#define REG_CURSOR1_VERT_SIZE		   0xdc
#define REG_CURSOR1_COL_IND1_0		   0xe0
#define REG_CURSOR1_COL_IND1_1		   0xe1
#define REG_CURSOR1_COL_IND2_0		   0xe4
#define REG_CURSOR1_COL_IND2_1		   0xe5
#define REG_CURSOR1_COL_IND3_0		   0xe8
#define REG_CURSOR1_COL_IND3_1		   0xe9
#define REG_CURSOR2_BLINK_TOTAL0	   0xec
#define REG_CURSOR2_BLINK_TOTAL1	   0xed
#define REG_CURSOR2_BLINK_ON0		   0xf0
#define REG_CURSOR2_BLINK_ON1		   0xf1
#define REG_CURSOR2_MEM_START0		   0xf4
#define REG_CURSOR2_MEM_START1		   0xf5
#define REG_CURSOR2_MEM_START2		   0xf6
#define REG_CURSOR2_POSX0			   0xf8
#define REG_CURSOR2_POSX1			   0xf9
#define REG_CURSOR2_POSY0			   0xfc
#define REG_CURSOR2_POSY1			   0xfd
#define REG_CURSOR2_HORIZ_SIZE		   0x100
#define REG_CURSOR2_VERT_SIZE		   0x104
#define REG_CURSOR2_COL_IND1_0		   0x108
#define REG_CURSOR2_COL_IND1_1		   0x109
#define REG_CURSOR2_COL_IND2_0		   0x10c
#define REG_CURSOR2_COL_IND2_1		   0x10d
#define REG_CURSOR2_COL_IND3_0		   0x110
#define REG_CURSOR2_COL_IND3_1		   0x111

// Color
extern WORD _color;

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
* Macros or function:  WriteData(data)
*
* PreCondition: none
*
* Input: data - 16 bit value to be written to RAM
*
* Output: none
*
* Side Effects: none
*
* Overview: writes data into controller's RAM
*
* Note: none
*
********************************************************************/
#ifdef __PIC32MX

void  WriteData(WORD value);

#else

#define WriteData(data) RS_LAT_BIT=1;\
PMDIN1=data; PMPWaitBusy();\
if(PMADDR == 0)\
if(A17_LAT_BIT) A17_LAT_BIT=0; else A17_LAT_BIT = 1;

#endif

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
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)

#define	GetMaxX() (DISP_VER_RESOLUTION-1)

#elif (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)

#define	GetMaxX() (DISP_HOR_RESOLUTION-1)

#endif

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
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)

#define	GetMaxY() (DISP_HOR_RESOLUTION-1)

#elif (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)

#define	GetMaxY() (DISP_VER_RESOLUTION-1)

#endif

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
#define SetColor(color) _color = color

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

#endif // _SSD1906_H
