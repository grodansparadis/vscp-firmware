/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  GOL Layer 
 *****************************************************************************
 * FileName:        GOL.h
 * Dependencies:    None 
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, MPLAB C32
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
 * Anton Alkhimenok and
 * Paolo A. Tamayo	
 *                      11/12/07	Version 1.0 release
 *****************************************************************************/

#ifndef _GOL_H
#define _GOL_H

#include "GraphicsConfig.h"

/*********************************************************************
* Overview: GOL scheme defines the style scheme to be used by an object. 
*
*********************************************************************/
typedef struct {
	WORD  EmbossDkColor;		// Emboss dark color used for 3d effect.
    WORD  EmbossLtColor;		// Emboss light color used for 3d effect.
    WORD  TextColor0;			// Character color 0 used for objects that supports text.
    WORD  TextColor1;			// Character color 1 used for objects that supports text.
    WORD  TextColorDisabled;	// Character color used when object is in a disabled state.
    WORD  Color0;				// Color 0 usually assigned to an Object state.
    WORD  Color1;				// Color 1 usually assigned to an Object state.
    WORD  ColorDisabled;		// Color used when an Object is in a disabled state.
    WORD  CommonBkColor;		// Background color used to hide Objects.
	void  *pFont;				// Font selected for the scheme.
} GOL_SCHEME;

// Pointer to the GOL default scheme.
// This scheme is created in GOLInit() function.
extern GOL_SCHEME  *_pDefaultGolScheme;

/*********************************************************************
* Overview: This structure defines the Object types used in the library. 
*
*********************************************************************/
typedef enum {
		OBJ_BUTTON,				// Type defined for Button Object.
		OBJ_WINDOW,				// Type defined for Window Object.
		OBJ_CHECKBOX,			// Type defined for Check Box Object.
		OBJ_RADIOBUTTON,		// Type defined for Radio Button Object.
		OBJ_EDITBOX,			// Type defined for Edit Box Object.
		OBJ_LISTBOX,			// Type defined for List Box Object.
		OBJ_SLIDER,				// Type defined for Slider and/or Scroll Bar Object.
		OBJ_PROGRESSBAR,		// Type defined for Progress Object.
		OBJ_STATICTEXT,			// Type defined for Static Text Object.
		OBJ_PICTURE,			// Type defined for Picture or Bitmap Object.
		OBJ_GROUPBOX,			// Type defined for Group Box Object.
    	OBJ_CUSTOM,				// Type defined for Custom Object.
    	OBJ_ROUNDDIAL,			// Type defined for Dial Object.
    	OBJ_METER,				// Type defined for Meter Object.
        OBJ_GRID,               // Type defined for Grid Object.
        OBJ_CHART,              // Type defined for Chart Object.
        OBJ_UNKNOWN
} GOL_OBJ_TYPE;

/*********************************************************************
* Overview: This structure defines the GOL message used in the library. 
*	- The types must be one of the INPUT_DEVICE_TYPE:
*		- TYPE_UNKNOWN
*		- TYPE_KEYBOARD
*		- TYPE_TOUCHSCREEN
*		- TYPE_MOUSE
*	- uiEvent must be one of the INPUT_DEVICE_EVENT. 
*   	- for touch screen:
*			- EVENT_INVALID 
*			- EVENT_MOVE 
*			- EVENT_PRESS 
*			- EVENT_RELEASE 
*   	- for keyboard:
*			- EVENT_KEYSCAN (param2 contains scan code)
*   		- EVENT_KEYCODE (param2 contains character code)
* 	- param1:
*	 	- for touch screen is the x position
*	    - for keyboard ID of object receiving the message
*	- param2
*	 	- for touch screen y position
*       - for keyboard scan or key code
*
*********************************************************************/
typedef struct {
	BYTE 			type;		// Type of input device.
    BYTE 			uiEvent;	// The generic events for input device.
    SHORT 			param1;		// Parameter 1 meaning is dependent on the type of input device.
    SHORT 			param2; 	// Parameter 2 meaning is dependent on the type of input device.
} GOL_MSG;

/*********************************************************************
* Overview: This structure defines the types of input devices 
*			used in the library. 
*
*********************************************************************/
typedef enum {                  
    TYPE_UNKNOWN = 0,			// Unknown device.
	TYPE_KEYBOARD,     			// Keyboard.
    TYPE_TOUCHSCREEN,  			// Touchscreen.
    TYPE_MOUSE,        			// Mouse.
    TYPE_TIMER,	       			// Timer.
    TYPE_SYSTEM	       			// System Messages.
} INPUT_DEVICE_TYPE;

/*********************************************************************
* Overview: This structure defines the types of GOL message events 
*  			used in the library. 
*
*********************************************************************/
typedef enum {                  
    EVENT_INVALID = 0,			// An invalid event.
    EVENT_MOVE,					// A move event.
    EVENT_PRESS,				// A press event.
    EVENT_RELEASE,				// A release event.
    EVENT_KEYSCAN,				// A keyscan event, parameters has the object ID keyboard scan code.
    EVENT_CHARCODE,				// Character code is presented at the parameters.
    EVENT_SET,					// A generic set event.
    EVENT_SET_STATE,			// A set state event.	
    EVENT_CLR_STATE				// A clear state event.
} INPUT_DEVICE_EVENT;

/*********************************************************************
* Overview: This structure defines the list of translated messages for 
*  			GOL Objects used in the library. 
*
*********************************************************************/
typedef enum {
	OBJ_MSG_INVALID = 0,		// Invalid message response.

    CB_MSG_CHECKED,				// Check Box check action ID.
    CB_MSG_UNCHECKED,			// Check Box un-check action ID.

    RB_MSG_CHECKED,				// Radio Button check action ID.

    WND_MSG_CLIENT,				// Window client area selected action ID.
    WND_MSG_TITLE,				// Window title bar selected action ID.
	
    BTN_MSG_PRESSED,			// Button pressed action ID.
    BTN_MSG_RELEASED,			// Button released action ID.

    PICT_MSG_SELECTED,			// Picture selected action ID.

    GB_MSG_SELECTED,			// Group Box selected action ID.

    CC_MSG_SELECTED,			// Custom Control selected action ID.

    SLD_MSG_INC,				// Slider or Scroll Bar increment action ID.
    SLD_MSG_DEC,				// Slider or Scroll Bar decrement action ID.

    ST_MSG_SELECTED,			// Static Text selected action ID.

    PB_MSG_SELECTED,			// Progress Bar selected action ID.
    
	RD_MSG_CLOCKWISE,			// Dial move clockwise action ID.
	RD_MSG_CTR_CLOCKWISE,		// Dial move counter clockwise action ID.

	MTR_MSG_SET,				// Meter set value action ID.

    EB_MSG_CHAR,				// Edit Box insert character action ID.
    EB_MSG_DEL,					// Edit Box remove character action ID.
    EB_MSG_TOUCHSCREEN,			// Edit Box touchscreen selected action ID.

    LB_MSG_SEL,					// List Box item select action ID.
    LB_MSG_MOVE,				// List Box item move action ID.
    LB_MSG_TOUCHSCREEN,			// List Box touchscreen selected action ID.
	
	GRID_MSG_TOUCHED,           // Grid item touched action ID.
	GRID_MSG_ITEM_SELECTED,     // Grid item selected action ID.
	GRID_MSG_UP,                // Grid up action ID.
	GRID_MSG_DOWN,              // Grid down action ID.
	GRID_MSG_LEFT,              // Grid left action ID.
	GRID_MSG_RIGHT,             // Grid right action ID.
	
	CH_MSG_SELECTED,			// Chart selected action ID
	
} TRANS_MSG;

/*********************************************************************
* Overview: This structure defines the first nine fields of the 
*			Objects structure. This allows generic operations on 
*			library Objects. 
*
*********************************************************************/
typedef struct {
	WORD		   ID;			// Unique id assigned for referencing.
	void          *pNxtObj;     // A pointer to the next object.
	GOL_OBJ_TYPE   type;	    // Identifies the type of GOL object.
    WORD           state;       // State of object.
	SHORT          left;        // Left position of the Object.
	SHORT          top;			// Top position of the Object.
	SHORT          right;       // Right position of the Object.
	SHORT          bottom;		// Bottom position of the Object.
	GOL_SCHEME    *pGolScheme;  // Pointer to the scheme used.
} OBJ_HEADER;


/*********************************************************************
* Overview: The following are the common Objcet States.
*
*********************************************************************/

// Focus state bit.
#define FOCUSED			 0x0001		
// Disabled state bit.
#define DISABLED    	 0x0002		
// Object hide state bit. Object will be hidden from the 
// screen by drawing over it the common background color.
#define HIDE        	 0x8000	
// Object redraw state bit. The whole Object must be redrawn.	
#define DRAW        	 0x7C00	
// Focus redraw state bit. The focus rectangle must be redrawn.	
#define DRAW_FOCUS     	 0x2000		
// Partial Object redraw state bit. A part or parts of
// of the Object must be redrawn to show updated state.
#define DRAW_UPDATE      0x3C00  	

/*********************************************************************
* Overview: The following are the global variables of GOL.
*
*********************************************************************/

// Pointer to the current linked list of objects displayed and receiving messages.
extern OBJ_HEADER  *_pGolObjects;
	                   
// Pointer to the object receiving keyboad input.
extern OBJ_HEADER  *_pObjectFocused;

// Line type used for focus mark.
#define FOCUS_LINE       2

/*********************************************************************
* Overview: Emboss size set for 3-D effect.
*
*********************************************************************/
#ifndef GOL_EMBOSS_SIZE
#define GOL_EMBOSS_SIZE                 3
#endif

/*********************************************************************
* Overview: The following are the style scheme default settings.
*
*********************************************************************/

// Default GOL font.
extern const FONT_FLASH GOLFontDefault; 	

// Brown color scheme
#define SADDLEBROWN        RGB565CONVERT(139,  69,  19)		// Saddle Color
#define SIENNA             RGB565CONVERT(160,  82,  45)		// Sienna Color
#define PERU               RGB565CONVERT(205, 133,  63)		// Peru Color
#define BURLYWOOD          RGB565CONVERT(222, 184, 135)		// Burly wood Color
#define WHEAT              RGB565CONVERT(245, 245, 220)		// Wheat Color
#define TAN                RGB565CONVERT(210, 180, 140)		// Tan Color
#define GRAY80             RGB565CONVERT(204, 204, 204)		// Gray80 Color
#define GRAY90             RGB565CONVERT(229, 229, 229)		// Gray90 Color
#define GRAY95             RGB565CONVERT(242, 242, 242)		// Gray95 Color

#if defined (USE_MONOCHROME)

	/* default settings for a monochrome display */ 

	// Emboss dark color default value.
	#define EMBOSSDKCOLORDEFAULT            0x00	
	// Emboss light color default value.	
	#define EMBOSSLTCOLORDEFAULT            0xFF	
	// Text color 0 default value.	
	#define TEXTCOLOR0DEFAULT               0xFF
	// Text color 1 default value.		
	#define TEXTCOLOR1DEFAULT               0x00	
	// Text color disabled default value.							
	#define TEXTCOLORDISABLEDDEFAULT        0xFF
	// Color 0 default value.								
	#define COLOR0DEFAULT                   0x00	
	// Color 1 default value.	
	#define COLOR1DEFAULT                   0xFF	
	// Disabled color default value.	
	#define COLORDISABLEDDEFAULT            0x00	
	// Common background color default value.	
	#define COMMONBACKGROUNDCOLORDEFAULT    0x00	
	// Default Font assignement.	
	#define FONTDEFAULT                     GOLFontDefault						

#else

	#if (GRAPHICS_PICTAIL_VERSION == 1)
		/* default settings for Microtips display */ 
	
		// Emboss dark color default value.
		#define EMBOSSDKCOLORDEFAULT            RGB565CONVERT(0x2B, 0x55, 0x87)	
		// Emboss light color default value.	
		#define EMBOSSLTCOLORDEFAULT            RGB565CONVERT(0xD4, 0xE4, 0xF7)	
		// Text color 0 default value.	
		#define TEXTCOLOR0DEFAULT               RGB565CONVERT(0x07, 0x1E, 0x48)
		// Text color 1 default value.		
		#define TEXTCOLOR1DEFAULT               WHITE	
		// Text color disabled default value.							
		#define TEXTCOLORDISABLEDDEFAULT        WHEAT
		// Color 0 default value.								
		#define COLOR0DEFAULT                   RGB565CONVERT(0x93, 0xEF, 0xFF)	
		// Color 1 default value.	
		#define COLOR1DEFAULT                   RGB565CONVERT(0x26, 0xC7, 0xF2)	
		// Disabled color default value.	
		#define COLORDISABLEDDEFAULT            RGB565CONVERT(0xB6, 0xD2, 0xFB)	
		// Common background color default value.	
		#define COMMONBACKGROUNDCOLORDEFAULT    RGB565CONVERT(0xEF, 0xFE, 0xFF)	
		// Default Font assignement.	
		#define FONTDEFAULT                     GOLFontDefault			
	
	#elif (GRAPHICS_PICTAIL_VERSION == 2)
		/* default settings for TRULY display */
		
		// Emboss dark color default value.
		#define EMBOSSDKCOLORDEFAULT            RGB565CONVERT(0x2B, 0x55, 0x87)		
		// Emboss light color default value.
		#define EMBOSSLTCOLORDEFAULT            RGB565CONVERT(0xD4, 0xE4, 0xF7)		
		// Text color 0 default value.
		#define TEXTCOLOR0DEFAULT               RGB565CONVERT(0x07, 0x1E, 0x48)		
		// Text color 1 default value.
		#define TEXTCOLOR1DEFAULT               WHITE								
		// Text color disabled default value.
		#define TEXTCOLORDISABLEDDEFAULT        WHEAT
		// Color 0 default value.								
		#define COLOR0DEFAULT                   RGB565CONVERT(0xA9, 0xDB, 0xEF)		
		// Color 1 default value.
		#define COLOR1DEFAULT                   RGB565CONVERT(0x26, 0xC7, 0xF2)		
		// Disabled color default value.
		#define COLORDISABLEDDEFAULT            RGB565CONVERT(0xB6, 0xD2, 0xFB)		
		// Common background color default value.
		#define COMMONBACKGROUNDCOLORDEFAULT    RGB565CONVERT(0xD4, 0xED, 0xF7)		
		// Default Font assignement.
		#define FONTDEFAULT                     GOLFontDefault						
	#endif // GRAPHICS_PICTAIL_VERSION
#endif // USE_MONOCHROME

/*********************************************************************
* Function: WORD GOLCanBeFocused(OBJ_HEADER* object)
*
* Overview: This function returns non-zero if the object can 
*			be focused. Only button, check box, radio button, 
*			slider, edit box, list box, scroll bar can accept 
*			focus. If the object is disabled it cannot be set 
*			to focused state.
*
* PreCondition: none
*
* Input: object - Pointer to the object of interest.
*
* Output: This returns a non-zero if the object can be focused 
*		  and zero if not.
*
* Side Effects: none
*
********************************************************************/
WORD GOLCanBeFocused(OBJ_HEADER* object);

/*********************************************************************
* Function: OBJ_HEADER *GOLGetFocusNext()
*
* Overview: This function returns the pointer to the next object
*		 	in the active linked list which is able to receive 
*		 	keyboard input.
*
* PreCondition: none
*
* Input: none
*
* Output: This returns the pointer of the next object in the 
*		  active list capable of receiving keyboard input. If 
*		  there is no object capable of receiving keyboard 
*		  inputs (i.e. none can be focused) NULL is returned.
*
* Side Effects: none
*
********************************************************************/
OBJ_HEADER *GOLGetFocusNext(void);

/*********************************************************************
* Function: void GOLSetFocus(OBJ_HEADER* object)
*
* Overview: This function sets the keyboard input focus to the  
*			object. If the object cannot accept keyboard messages 
*			focus will not be changed. This function resets FOCUSED 
*			state for the object was in focus previously, set 
*			FOCUSED state for the required object and marks the 
*			objects to be redrawn.
*
* PreCondition: none
*
* Input: object - Pointer to the object of interest.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void GOLSetFocus(OBJ_HEADER* object);

/*********************************************************************
* Macros:  GOLGetFocus()
*
* Overview: This macro returns the pointer to the current object 
*			receiving keyboard input.
*
* PreCondition: none
*
* Input: none
*
* Output: Returns the pointer to the object receiving keyboard input. 
*		  If there is no object in focus NULL is returned.
*
* Side Effects: none
*
********************************************************************/
#define GOLGetFocus() _pObjectFocused

/*********************************************************************
* Macros: GetObjType(pObj)
*
* Overview: This macro returns the object type.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*
* Output: Returns the OBJ_TYPE of the object.
*
* Side Effects: none
*
********************************************************************/
#define GetObjType(pObj)   		((OBJ_HEADER*)pObj)->type

/*********************************************************************
* Macros: GetObjID(pObj)
*
* Overview: This macro returns the object ID.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*
* Output: Returns the ID of the object.
*
* Example:
*   <PRE> 
*	void UseOfGetObjID(OBJ_HEADER *pObj) {
*		WORD id;
*			switch(id = GetObjID(pObj)) {
*		    	case ID_WINDOW1:
*		            // do something
*		    	case ID_WINDOW2:
*		            // do something else
*		    	case ID_WINDOW3:
*		            // do something else
*		    	default:
*		            // do something else
*			}
*	}
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define GetObjID(pObj)     		((OBJ_HEADER*)pObj)->ID

/*********************************************************************
* Macros: GetObjNext(pObj)
*
* Overview: This macro returns the next object after the specified 
*			object.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*
* Output: Returns the pointer of the next object.
*
* Example:
*   <PRE> 
*	// This is the same example for the GetObjType() macro
*	// We just replaced one line
*	void RedrawButtons(void) {
*		OBJ_HEADER *pCurr;
*		
*		pCurr = GOLGetList();				// get active list
*		while(pCurr->pNxtObj != NULL) {
*			if (GetObjType(pCurr) == BUTTON)
*				pCurr->state = BTN_DRAW;	// set button to be redrawn
*			pCurr = GetObjNext(pCurr);		// Use of GetObjNext() macro
*			// replaces the old line
*		}
*		GolDraw();                          // redraw all buttons in the
*		                                    // active list
*	}
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define GetObjNext(pObj)   		((OBJ_HEADER*)pObj)->pNxtObj

/*********************************************************************
* Macros: IsObjUpdated(pObj)
*
* Overview: This macro tests if the object is pending to 
*			be redrawn. This is done by testing the 6 MSBits 
*			of object’s state to detect if the object must 
*			be redrawn.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*
* Output: Returns a nonzero value if the object needs 
*		  to be redrawn. Zero if not.
*
* Example:
*   <PRE> 
*	int DrawButtonWindowOnly() {
*		static OBJ_HEADER *pCurrentObj = NULL;
*		SHORT done = 0;
*
*		if (pCurrentObj == NULL)
*			pCurrentObj = GOLGetList();         // get current list
*
*		while(pCurrentObj != NULL){
*			if(IsObjUpdated(pCurrentObj)){
*				switch(pCurrentObj->type){
*					case OBJ_BUTTON:
*						done = BtnDraw((BUTTON*)pCurrentObj);
*						break;
*					case OBJ_WINDOW:
*						done = WndDraw((WINDOW*)pCurrentObj);
*						break;
*				}
*				// reset state of object if done
*				if (done)
*					GOLDrawComplete(pCurrentObj)
*				// Return if not done. This means that BtnDraw()
*				// was terminated prematurely by device busy status
*				// and must be recalled to finish rendering of
*				// objects in the list that have new states.
*				else
*					return 0;
*			}
*			// go to the next object in the list
*			pCurrentObj = pCurrentObj->pNxtObj;
*		}
*		return 1;
*	}
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define IsObjUpdated(pObj)   	(((OBJ_HEADER*)pObj)->state&0xfc00)

/*********************************************************************
* Macros: GOLDrawComplete(pObj)
*
* Overview: This macro resets the drawing states of the object 
*			(6 MSBits of the object’s state).
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*
* Output: none
*
* Example:
*   <PRE> 
*	// This function should be called again whenever an incomplete
*	// rendering (done = 0) of an object occurs.
*	// internal states in the BtnDraw() or WndDraw() should pickup
*	// on the state where it left off to continue rendering.
*	void GOLDraw() {
*		static OBJ_HEADER *pCurrentObj = NULL;
*		SHORT done;
*
*		if(pCurrentObj == NULL) {
*			if(GOLDrawCallback()) {
*				// If it's last object jump to head
*				pCurrentObj = GOLGetList();
*			} else {
*				return;
*			}
*		}
*		done = 0;
*
*		while(pCurrentObj != NULL) {
*			if(IsObjUpdated(pCurrentObj)) {
*				switch(pCurrentObj->type) {
*					case OBJ_BUTTON:
*						done = BtnDraw((BUTTON*)pCurrentObj);
*						break;
*					case OBJ_WINDOW:
*						done = WndDraw((WINDOW*)pCurrentObj);
*						break;
*				}
*				if(done){
*					GOLDrawComplete(pCurrentObj);
*				}else{
*					return;
*				}
*			}
*			pCurrentObj = pCurrentObj->pNxtObj;
*		}
*	}
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define GOLDrawComplete(pObj) ((OBJ_HEADER*)pObj)->state &= 0x03ff

/*********************************************************************
* Macros:  SetState(pObj, stateBits)
*
* Overview: This macro sets the state bits of an object. 
*			Object must be redrawn to display the changes. 
*			It is possible to set several state bits with 
*			this macro.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*		 stateBits - Defines which state bits are to be 
*				set. Please refer to specific objects for object 
*				state bits definition for details
*
* Output: none
*
* Example:
*   <PRE> 
*	void BtnMsgDefault(WORD msg, BUTTON* pB){
*		switch(msg){
*			case BTN_MSG_PRESSED:
*				// set pressed and redraw
*				SetState(pB, BTN_PRESSED|BTN_DRAW);
*				break;
*			case BTN_MSG_RELEASED:
*				ClrState(pB, BTN_PRESSED);       // reset pressed
*				SetState(pB, BTN_DRAW);          // redraw
*				break;
*		}
*	}
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define SetState(pObj, stateBits)     ((OBJ_HEADER*)pObj)->state |= (stateBits)

/*********************************************************************
* Macros:  ClrState(pObj, stateBits)
*
* Overview: This macro clear the state bits of an object. 
*			Object must be redrawn to display the changes. 
*			It is possible to clear several state bits with 
*			this macro.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*		 stateBits - Defines which state bits are to be 
*				cleared. Please refer to specific objects for object 
*				state bits definition for details
*
* Output: none
*
* Example:
*   See example for SetState().
*
* Side Effects: none
*
********************************************************************/
#define ClrState(pObj, stateBits)    ((OBJ_HEADER*)pObj)->state &= (~(stateBits))

/*********************************************************************
* Macros:  GetState(pObj, stateBits)
*
* Overview: This macro retrieves the current value of 
*			the state bits of an object. It is possible 
*			to get several state bits.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*		 stateBits - Defines which state bits are requested. 
*				Please refer to specific objects for object 
*				state bits definition for details
*
* Output: Macro returns a non-zero if any bit in the 
*		  stateBits mask is set.
*
* Example:
*   <PRE> 
*	#define BTN_HIDE 0x8000
*	BUTTON *pB;
*	// pB is created and initialized
*	// do something here to set state
*
*	// Hide the button (remove from screen)
*	if (GetState(pB,BTN_HIDE)) {
*		SetColor(pB->pGolScheme->CommonBkColor);
*		Bar(pB->left,pB->top,pB->right,pB->bottom);
*
*	}
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define GetState(pObj, stateBits)    (((OBJ_HEADER*)pObj)->state&(stateBits))

/*********************************************************************
* Function: GOL_SCHEME *GOLCreateScheme()
*
* Overview: This function creates a new style scheme object 
*			and initializes the parameters to default values.
*
* PreCondition: none
*
* Input: none
*
* Output: Pointer to the new GOL_SCHEME created.
*
* Example:
*   <PRE> 
*	extern const char Font22[] __attribute__((aligned(2)));
*	extern const char Font16[] __attribute__((aligned(2)));
*
*	GOLSCHEME *pScheme1, *pScheme2;
*	pScheme1 = GOLCreateScheme();
*	pScheme2 = GOLCreateScheme();
*	
*	pScheme1->pFont = (BYTE*)Font22;
*	pScheme2->pFont = (BYTE*)Font16;
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
GOL_SCHEME *GOLCreateScheme(void);

/*********************************************************************
* Macros:  GOLSetScheme(pObj, pScheme)
*
* Overview: This macro sets the GOL scheme to be used for the object.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*		 pScheme - Pointer to the style scheme to be used. 
*
* Output: none
*
* Example:
*   <PRE> 
*	extern FONT_FLASH Gentium12;
*	GOLSCHEME *pScheme1;
*	BUTTON *pButton;
*
*	pScheme1 = GOLCreateScheme();
*	pScheme1->pFont = &Gentium12;
*
*	// assume button is created and initialized
*
*	// reassign the scheme used by pButton to pScheme1
*	GOLSetScheme(pButton, pScheme1);
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define GOLSetScheme(pObj, pScheme)  ((OBJ_HEADER*)pObj)->pGolScheme = pScheme

/*********************************************************************
* Macros:  GOLGetScheme(pObj)
*
* Overview: This macro gets the GOL scheme used by the given object.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object of interest.
*
* Output: Returns the style scheme used by the given object.
*
* Example:
*   <PRE> 
*	GOLSCHEME *pScheme2;
*	BUTTON *pButton;
*
*	// assume button is created and initialized
*	// get the scheme assigned to pButton
*	pScheme2 = GOLGetScheme(pButton);
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
#define GOLGetScheme(pObj)  ((OBJ_HEADER*)pObj)->pGolScheme

/*********************************************************************
* Macros:  GOLGetSchemeDefault()
*
* Overview: This macro returns the default GOL scheme pointer.
*
* PreCondition: none
*
* Input: none
*
* Output: Returns the pointer to the default style scheme.
*
* Side Effects: none
*
********************************************************************/
#define GOLGetSchemeDefault()  _pDefaultGolScheme

/*********************************************************************
* Function: void GOLInit()
*
* Overview: This function initializes the graphics library and 
*			creates a default style scheme with default settings 
*			referenced by the global scheme pointer. GOLInit() 
*			function must be called before GOL functions can be 
*			used. It is not necessary to call GraphInit() 
*			function if this function is used.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: This sets the line type to SOLID_LINE, sets the 
*				screen to all BLACK, sets the current drawing color 
*				to WHITE, sets the graphic cursor position to upper 
*				left corner of the screen, sets active and visual 
*				pages to page #0, clears the active page and disables 
*				clipping. This also creates a default style scheme.
*
********************************************************************/
void  GOLInit();

/*********************************************************************
* Function: void GOLAddObject(OBJ_HEADER* object)
*
* Overview: This function adds an object to the tail of the 
*			active list pointed to by _pGolObjects. The new list 
*			tail is set to point to NULL.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object to be added on the current 
*			    active list.
*
* Output: none
*
* Example:
*   <PRE> 
*	void MoveObject(OBJ_HEADER *pSrcList, OBJ_HEADER *pDstList,
*		OBJ_HEADER *pObjtoMove) {
*		OBJ_HEADER *pTemp = pSrcList;
*		
*		if(pTemp != pObjtoMove) {
*			while(pTemp->pNxtObj != pObjtoMove)
*				pTemp = pTemp->pNxtObj;
*		}
*
*		pTemp->pNxtObj = pObjtoMove ->pNxt; // remove object from list
*		GOLSetList(pDstList);               // destination as active list
*		GOLAddObject(pObjtoMove);           // add object to active list
*	}
*	</PRE>
*
* Side Effects: none
*
********************************************************************/
void  GOLAddObject(OBJ_HEADER* object);

/*********************************************************************
* Macros: void GOLNewList()
*
* Overview: This macro starts a new linked list of objects and 
*			resets the keyboard focus to none. This macro assigns 
*			the current active list _pGolObjects and current receiving 
*			keyboard input _pObjectFocused object pointers to NULL. 
*			Any keyboard inputs at this point will be ignored. 
*			Previous active list must be saved in another pointer if 
*			to be referenced later. If not needed anymore memory used 
*			by that list should be freed by GOLFree() function.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Example:
*   <PRE> 
*	OBJ_HEADER *pSave;
*
*	pSave = GOLGetList();            // save current list
*	GOLNewList();                    // start the new list
*	                                 // current list is now NULL
*
*	// assume that objects are already created
*	// you can now add objects to the new list
*	GOLAddObject(pButton);
*	GOLAddObject(pWindow);
*	GOLAddObject(pSlider);
*	</PRE>
*
* Side Effects: This macro sets the focused object pointer 
*				(_pObjectFocused) to NULL.
*
********************************************************************/
#define  GOLNewList()  _pGolObjects = NULL;  _pObjectFocused = NULL

/*********************************************************************
* Macros: GOLGetList()
*
* Overview: This macro gets the current active list.
*
* PreCondition: none
*
* Input: none
*
* Output: Returns the pointer to the current active list.
*
* Example:
*   See GOLNewList() example.
*
* Side Effects: none
*
********************************************************************/
#define  GOLGetList()  _pGolObjects

/*********************************************************************
* Macros: GOLSetList(objsList)
*
* Overview: This macro sets the given object list as the active 
*			list and resets the keyboard focus to none. This macro 
*			assigns the receiving keyboard input object _pObjectFocused 
*			pointer to NULL. If the new active list has an object’s 
*			state set to focus, the _pObjectFocused pointer must be 
*			set to this object or the object’s state must be change 
*			to unfocused. This is to avoid two objects displaying a 
*			focused state when only one object in the active list must 
*			be set to a focused state at anytime.
*
* PreCondition: none
*
* Input: objsList - The pointer to the new active list.
*
* Output: none
*
* Example:
*	<PRE> 
* 	OBJ_HEADER *pSave;
*	pSave = GOLGetList();           // save current list
*	GOLNewList();               	// start the new list
*	                                // current list is now NULL
*
*	// you can now add objects to the current list
*	// assume that objects are already created
*	GOLAddObject(pButton);
*	GOLAddObject(pWindow);
*	GOLAddObject(pSlider);
*
*	// do something here on the new list
*	// return the old list
*	GOLSetList(pSave);
*	</PRE>	
*
* Side Effects: This macro sets the focused object pointer 
*				(_pObjectFocused) to NULL. Previous active list 
*				should be saved if needed to be referenced later. 
*				If not, use GOLFree() function to free the memory 
*				used by the objects before calling GOLSetList().
*
********************************************************************/
#define  GOLSetList(objsList)  _pGolObjects = objsList; _pObjectFocused = NULL									

/*********************************************************************
* Function: OBJ_HEADER* GOLFindObject(WORD ID)
*
* Overview: This function finds an object in the active list 
*			pointed to by _pGolObjects using the given object ID.
*
* PreCondition: none
*
* Input: ID - User assigned value set during the creation of the object.
*
* Output: Pointer to the object with the given ID.
*
* Example:
*	<PRE> 
* 	void CopyObject(OBJ_HEADER *pSrcList, OBJ_HEADER *pDstList, WORD ID) 
*	{
*		OBJ_HEADER *pTemp;
*		
*		pTemp = GOLFindObject(ID);          	// find the object
*		if (pTemp != NULL) {
*			GOLSetList(pDstList);            	// destination as active list
*			GOLAddObject(pObj);              	// add object to active list
*		}
*	}
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
OBJ_HEADER* GOLFindObject(WORD ID);

/*********************************************************************
* Function: void GOLFree()
*
* Overview: This function frees all the memory used by objects in 
*			the active list and initializes _pGolObjects pointer 
*			to NULL.
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Example:
*	<PRE> 
* 	void DeletePage(OBJ_HEADER *pPage) {
*		OBJ_HEADER *pTemp;
*		
*		// assuming pPage is different from the current active list
*		pTemp = GOLGetList();               // save the active list
*		GOLSetList(pPage);                  // set list as active list
*		GolFree();                          // pPage objects are deleted
*		
*		GOLSetList(pTemp);                  // restore the active list
*	}
*	</PRE>	
*
* Side Effects: All objects in the active list are deleted from memory.
*
********************************************************************/
void GOLFree();
/*********************************************************************
* Function: BOOL GOLDeleteObject(OBJ_HEADER * object)
*
* PreCondition: none
*
* Input: pointer to the object
*
* Output: none
*
* Side Effects: none
*
* Overview: deletes an object to the linked list objects for the current screen.
*
* Note: none
*
********************************************************************/
BOOL  GOLDeleteObject(OBJ_HEADER * object);
/*********************************************************************
* Function: BOOL GOLDeleteObjectByID(WORD ID)
*
* PreCondition: none
*
* Input: ID of the object.
*
* Output: none
*
* Side Effects: none
*
* Overview: Deletes an object in the current active linked list of objects using the ID parameter
*			of the object.
*
* Note: none
*
********************************************************************/
BOOL  GOLDeleteObjectByID(WORD ID);

/*********************************************************************
* Function: WORD GOLDraw()
*
* Overview: This function loops through the active list and redraws 
*			objects that need to be redrawn. Partial redrawing or 
*			full redraw is performed depending on the drawing states 
*			of the objects.
*			GOLDrawCallback() function is called by GOLDraw() 
*			when drawing of objects in the active list is completed.
*
* PreCondition: none
*
* Input: none
*
* Output: Non-zero if the active link list drawing is completed.
*
* Example:
*	<PRE> 
* 	// Assume objects are created & states are set to draw objects
*	while(1){
*		if(GOLDraw()){              // parse active list and redraw objects that needs to be redrawn
	
*	    	// here GOL drawing is completed
*	      	// it is safe to modify objects states and linked list
*
*			TouchGetMsg(&msg);      // evaluate messages from touch screen device
*
* 			GOLMsg(&msg);           // evaluate each object is affected by the message
*      	}
*	}
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
WORD GOLDraw();

/*********************************************************************
* Function: void GOLRedrawRec(SHORT left, SHORT top, SHORT right, SHORT bottom)
*
* Overview: This function marks all objects in the active 
*			list intersected by the given rectangular area 
*			to be redrawn.
*
* PreCondition: none
*
* Input: left - Defines the left most border of the rectangle area.
*		 top - Defines the top most border of the rectangle area.
*		 right - Defines the right most border of the rectangle area.
*		 bottom - Defines the bottom most border of the rectangle area.
*
* Output: none
*
* Example:
*	<PRE> 
* 	OBJ_HEADER *pTemp;
*	OBJ_HEADER *pAllObjects;
*
*	// assume *pAllObjects points to a list of all existing objects
*	// created and initialized
*
*	// mark all objects inside the rectangle to be redrawn
*	GOLRedrawRec(10,10,100,100);
*
*	pTemp = pAllObjects;					
*	GOLStartNewList();                     	// reset active list
*	while(pTemp->pNxtObj != NULL) {
*		if (pTemp->state&0x7C00)            // add only objects to be
*		GOLAddObject(pTemp);          		// redrawn to the active list
*		pTemp = pTemp->pNxtObj;
*	}
*	GOLDraw();                             	// redraw active list
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
void  GOLRedrawRec(SHORT left, SHORT top, SHORT right, SHORT bottom);

/*********************************************************************
* Macros: GOLRedraw(pObj)
*
* Overview: This macro sets the object to be redrawn. For the 
*			redraw to be effective, the object must be in the 
*			current active list. If not, the redraw action will 
*			not be performed until the list where the object is 
*			currently inserted will be set to be the active list.
*
* PreCondition: none
*
* Input: pObj - Pointer to the object to be redrawn.
*
* Output: none
*
* Example:
*	<PRE> 
*	void GOLRedrawRec(SHORT left, SHORT top, SHORT right, SHORT bottom) {  
*		// set all objects encompassed by the rectangle to be redrawn
*		OBJ_HEADER *pCurrentObj;
*		
*		pCurrentObj = GOLGetList();
*		while(pCurrentObj != NULL){
*			if (
*				((pCurrentObj->left >= left) && (pCurrentObj->left <= right)) ||
*				((pCurrentObj->right >= left) && (pCurrentObj->right <= right))||
*				((pCurrentObj->top >= top) && (pCurrentObj->top <= bottom)) ||
*				((pCurrentObj->bottom >= top) && (pCurrentObj->bottom <= bottom))){
*					GOLRedraw(pCurrentObj);
*			}
*			pCurrentObj = pCurrentObj->pNxtObj;           
*		}//end of while
*	}                             	
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
#define GOLRedraw(pObj) ((OBJ_HEADER*)pObj)->state |= 0x7c00

/*********************************************************************
* Function: void GOLMsg(GOL_MSG *pMsg)
*
* Overview: This function receives a GOL message from user and 
*			loops through the active list of objects to check 
*			which object is affected by the message. For affected 
*			objects the message is translated and GOLMsgCallback() 
*			is called. In the call back function, user has the 
*			ability to implement action for the message. If the 
*			call back function returns non-zero OBJMsgDefault() 
*			is called to process message for the object by default. 
*			If zero is returned OBJMsgDefault() is not called. 
*			Please refer to GOL Messages section for deatils.
*
* 			This function should be called when GOL drawing 
* 			is completed. It can be done when GOLDraw() returns 
* 			non-zero value or inside GOLDrawCallback() function.
*
* PreCondition: none
*
* Input: pMsg - Pointer to the GOL message from user.
*
* Output: none
*
* Example:
*	<PRE> 
*	// Assume objects are created & states are set to draw objects
*	while(1){
*		if(GOLDraw()){
*		   	// GOL drawing is completed here
*  			// it is safe to change objects
*			TouchGetMsg(&msg);         // from user interface module
*			GOLMsg(&msg);
*		}
*	}                       	
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
void  GOLMsg(GOL_MSG *pMsg);

/*********************************************************************
* Function: WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)
*
* Overview: The user MUST implement this function. GOLMsg() calls 
*			this function when a valid message for an object in the 
*			active list is received. User action for the message should 
*			be implemented here. If this function returns non-zero, 
*			the message for the object will be processed by default. 
*			If zero is returned, GOL will not perform any action.
*
* PreCondition: none
*
* Input: objMsg - Translated message for the object or the action ID response from the object.
*		 pObj - Pointer to the object that processed the message.
*		 pMsg - Pointer to the GOL message from user.
*
* Output: Return a non-zero if the message will be processed by default.
*		  If a zero is returned, the message will not be processed by GOL.
*
* Example:
*	<PRE> 
*	WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG *pMsg){
*		static char focusSwitch = 1;
*
*		switch(GetObjID(pObj)){
*	        case ID_BUTTON1:
*	            // Change text and focus state
*	            if(objMsg == BTN_MSG_RELEASED){
*	                focusSwitch ^= 1;
*	            	if(focusSwitch){
*                   	BtnSetText((BUTTON*)pObj, "Focused");
*                    	SetState(pObj,BTN_FOCUSED);
*                	}else{
*                    	BtnSetText((BUTTON*)pObj, "Unfocused");
*                    	ClrState(pObj,BTN_FOCUSED);
*                	}
*            	}
*            	// Process by default
*            	return 1;
*	        case ID_BUTTON2:
*	            // Change text
*	            if(objMsg == BTN_MSG_PRESSED){
*	                BtnSetText((BUTTON*)pObj, "Pressed");
*            	}
*	            if(objMsg == BTN_MSG_RELEASED){
*	                BtnSetText((BUTTON*)pObj, "Released");
*	            }
*	            // Process by default
*	            return 1;
*	        case ID_BUTTON3:
*	            // Change face picture
*	            if(objMsg == BTN_MSG_PRESSED){
*	                BtnSetBitmap(pObj,arrowLeft);
*	            }
*	            if(objMsg == BTN_MSG_RELEASED){
*	                BtnSetBitmap(pObj,(char*)arrowRight);
*	            }
*	            // Process by default
*	            return 1;
*	        case ID_BUTTON_NEXT:
*	            if(objMsg == BTN_MSG_RELEASED){
*	                screenState = CREATE_CHECKBOXES;
*	            }
*	            // Process by default
*	            return 1;
*	        case ID_BUTTON_BACK:
*	            return 1;
*	        default:
*	            return 1;
*   	}
*	}              	
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg);

/*********************************************************************
* Function: WORD GOLDrawCallback()
*
* Overview: GOLDrawCallback() function MUST BE implemented by 
*			the user. This is called inside the GOLDraw() 
*			function when the drawing of objects in the active 
*			list is completed. User drawing must be done here. 
*			Drawing color, line type, clipping region, graphic 
*			cursor position and current font will not be changed 
*			by GOL if this function returns a zero. To pass 
*			drawing control to GOL this function must return 
*			a non-zero value. If GOL messaging is not using 
*			the active link list, it is safe to modify the 
*			list here.
*
* PreCondition: none
*
* Input: none
*
* Output: Return a one if GOLDraw() will have drawing control 
*		  on the active list. Return a zero if user wants to 
*		  keep the drawing control.
*
* Example:
*	<PRE> 
*	#define SIG_STATE_SET   0
*	#define SIG_STATE_DRAW  1
*	WORD GOLDrawCallback(){
*		static BYTE state = SIG_STATE_SET;
*		if(state == SIG_STATE_SET){
*			// Draw the button with disabled colors
*			GOLPanelDraw(SIG_PANEL_LEFT,SIG_PANEL_TOP,
*						 SIG_PANEL_RIGHT,SIG_PANEL_BOTTOM, 0,
*						 WHITE, altScheme->EmbossLtColor,
*						 altScheme->EmbossDkColor,
*						 NULL, GOL_EMBOSS_SIZE);
*
*			state = SIG_STATE_DRAW;
*		}
*		
*		if(!GOLPanelDrawTsk()){
*	    	// do not return drawing control to GOL
*      		// drawing is not complete
*			return 0;
*		}else{
*			state = SIG_STATE_SET;
*			// return drawing control to GOL, drawing is complete
*			return 1;
*		}
*	}        	
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
WORD GOLDrawCallback();

/*********************************************************************
* Variables for rounded panel drawing. Used by GOLRndPanelDraw and GOLRndPanelDrawTsk
********************************************************************/
extern SHORT _rpnlX1;				// Panel center x position of upper left corner
extern SHORT _rpnlY1;				// Panel center y position of upper left corner
extern SHORT _rpnlX2;				// Panel center x position of lower right corner
extern SHORT _rpnlY2;				// Panel center y position of lower right corner
extern SHORT _rpnlR;				// radius (defines the rounded corner size)
extern WORD  _rpnlFaceColor;		// Panel face color
extern WORD  _rpnlEmbossLtColor;	// Panel emboss light color
extern WORD  _rpnlEmbossDkColor;	// Panel emboss dark color
extern WORD  _rpnlEmbossSize;      	// Size of panel emboss (Usually uses GOL_EMBOSS_SIZE but maybe different 
                        			// for some objects)emboss size 
	  
extern void* _pRpnlBitmap;      	// Bitmap used in the panel

/*********************************************************************
* Macros: GOLPanelDraw(	left, top, right, bottom,
*						radius,
*						faceClr,
*						embossLtClr, embossDkClr,
*						pBitmap,
*						embossSize)
*
* Overview: This macro sets the parameters to draw a panel. 
*			Panel is not an object. It will not be added to 
*			the active list. Use GOLPanelDrawTsk() to display 
*			the panel on the screen.
* 			The following relationships of the parameters determines
*			the general shape of the button:
* 			1. Panel width is determined by right - left.
*			2. Panel height is determined by top - bottom.
*			3. Panel radius - specifies if the panel will have a rounded 
*						edge. If zero then the panel will have 
*						sharp (cornered) edge.
*			4. If 2*radius = height = width, the panel is circular. 	
*
* PreCondition: none
*
* Input: left - Panel's left most position.
*		 top - Panel's top most position.
*		 right - Panel's right most position.
*		 bottom - Panel's bottom most position.
*		 radius - The radius of the rounded corner of the panel.
*		 faceClr - Defines the face color of the panel.
*		 embossLtClr - Defines the emboss light color.
*		 embossDkClr - Defines the emboss dark color.
*		 pBitmap - Defines the bitmap used in the face of the panel.
*		 embossSize - Defines the emboss size of the panel.
*
* Output: none
*
* Example:
*	<PRE> 
*	// Dimensions for signature box
*	#define SIG_PANEL_LEFT      10
*	#define SIG_PANEL_RIGHT    310
*	#define SIG_PANEL_TOP       50
*	#define SIG_PANEL_BOTTOM   170
*
*	#define SIG_STATE_SET   0
*	#define SIG_STATE_DRAW  1
*	
*	GOL_SCHEME *altScheme;           // style scheme
*	
*	// Draws box for signature
*	WORD PanelSignature() {
*		static BYTE state = SIG_STATE_SET;
*
*		if(state == SIG_STATE_SET){
*			// set data for panel drawing (radius = 0)
*			// assume altScheme is defined
*			GOLPanelDraw(SIG_PANEL_LEFT,SIG_PANEL_TOP,
*						 SIG_PANEL_RIGHT,SIG_PANEL_BOTTOM,0,
*						 WHITE,
*						 altScheme->EmbossLtColor,
*						 altScheme->EmbossDkColor,
*						 NULL, GOL_EMBOSS_SIZE);
*
*			state = SIG_STATE_DRAW; // change state
*		}
*
*		if(!GOLPanelDrawTsk())
*			return 0; // drawing is not completed
*		} else {
*			state = SIG_STATE_SET; // set state to initial
*			return 1; // drawing is done
*		}
*	}
*	</PRE>	
*
* Side Effects: none
*
********************************************************************/
#define GOLPanelDraw(left,top,right,bottom,radius,faceClr,embossLtClr,embossDkClr,pBitmap,embossSize) _rpnlX1=left;\
_rpnlY1 = top;\
_rpnlX2 = right;\
_rpnlY2 = bottom;\
_rpnlR = radius;\
_rpnlFaceColor = faceClr;\
_rpnlEmbossLtColor = embossLtClr;\
_rpnlEmbossDkColor = embossDkClr;\
_pRpnlBitmap = pBitmap;\
_rpnlEmbossSize = embossSize;

/*********************************************************************
* Function: WORD GOLPanelDrawTsk()
*
* Overview: This function draws a panel on the screen with parameters 
*			set by GOLPanelDraw() macro. This function must be called 
*			repeatedly (depending on the return value) for a successful 
*			rendering of the panel. 	
*
* PreCondition: Parameters of the panel must be set by GOLPanelDraw() macro.
*
* Input: none
*
* Output: Returns the status of the panel rendering
*	<PRE> 
*          0 – Rendering of the panel is not yet finished.
*          1 – Rendering of the panel is finished.
*	</PRE>	
*
* Example:
*	See GOLPanelDraw() example.
*
* Side Effects: none
*
********************************************************************/
WORD GOLPanelDrawTsk(void); 

#endif // _GOL_H
