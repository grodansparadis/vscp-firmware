/*********************************************************************
 * Module for Microchip Graphics Library
 * The header file joins all header files used in the graphics library
 * and contains compile options and defaults.
 *********************************************************************
 * FileName:        Graphics.h
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC24/PIC30/PIC32
 * Compiler:        C30 V3.00/C32
 * Company:         Microchip Technology, Inc.
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
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Anton Alkhimenok     11/12/07	Version 1.0 release
 ********************************************************************/

#ifndef _GRAPHICS_H
#define _GRAPHICS_H

//////////////////// COMPILE OPTIONS AND DEFAULTS ////////////////////

////// COMPILE OPTIONS FOR APPLICATION //////
#include <stdlib.h>
#include "GenericTypeDefs.h"
#include "GraphicsConfig.h"

//////// GRAPHICS_LIBRARY_VERSION ///////////
// MSB is version, LSB is subversion
#define GRAPHICS_LIBRARY_VERSION  0x0152

////////////////////////////// INCLUDES //////////////////////////////
#include "Primitive.h"  // Graphic primitives

#include "ScanCodes.h"  // Scan codes for AT keyboard
#include "GOL.h"        // GOL layer 
#ifdef USE_BUTTON
    #include "Button.h"
#endif
#ifdef USE_WINDOW
    #include "Window.h"
#endif
#ifdef USE_GROUPBOX
    #include "GroupBox.h"
#endif
#ifdef USE_STATICTEXT
    #include "StaticText.h"
#endif
#ifdef USE_SLIDER
    #include "Slider.h"
#endif
#ifdef USE_CHECKBOX
    #include "CheckBox.h"
#endif
#ifdef USE_RADIOBUTTON
    #include "RadioButton.h"
#endif
#ifdef USE_PICTURE
    #include "Picture.h"
#endif
#ifdef USE_PROGRESSBAR
    #include "ProgressBar.h"
#endif
#ifdef USE_EDITBOX
    #include "EditBox.h"
#endif
#ifdef USE_LISTBOX
    #include "ListBox.h"
#endif
#ifdef USE_ROUNDDIAL
    #include "RoundDial.h"
#endif
#ifdef USE_METER
    #include "Meter.h"
#endif
#ifdef USE_GRID
    #include "Grid.h"
#endif
#ifdef USE_CHART
    #include "Chart.h"
#endif
#ifdef USE_CUSTOM
    // Included for custom control demo
    #include "CustomControlDemo.h"
#endif


/*********************************************************************
* Macros: RGB565CONVERT(red, green, blue)
*
* Overview: Converts true color into 5:6:5 RGB format.
*
* PreCondition: none
*
* Input: Red, Green, Blue components.
*
* Output: 5 bits red, 6 bits green, 5 bits blue color.
*
* Side Effects: none
*
********************************************************************/
#define RGB565CONVERT(red, green, blue)\
(WORD)( (( red   >> 3 ) << 11 ) | \
(( green >> 2 ) << 5  ) | \
( blue  >> 3 ))


#endif
