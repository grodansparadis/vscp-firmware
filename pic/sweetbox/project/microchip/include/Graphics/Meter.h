/*****************************************************************************
 *  Module for Microchip Graphics Library
 *  GOL Layer 
 *  Meter
 *****************************************************************************
 * FileName:        Meter.h
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
 * Paolo A. Tamayo		11/12/07	Version 1.0 release
 *****************************************************************************/

#ifndef _METER_H
#define _METER_H

#include <Graphics\GOL.h>

// Compile time Options for Meter
#define METER_DISPLAY_VALUES_ENABLE	// This enables the display of the values.
									// Displaying the values will have significant
									// drawing time requirement.
											
/*********************************************************************
* Object States Definition: 
*********************************************************************/
#define MTR_DISABLED    	0x0002  // Bit for disabled state.
#define MTR_RING     		0x0004  // Bit for ring type, scales are drawn over the ring
									// default is only scales drawn.
#define MTR_DRAW_UPDATE    	0x1000  // Bit to indicate an update only.
#define MTR_DRAW        	0x4000  // Bit to indicate object must be redrawn.
#define MTR_HIDE        	0x8000  // Bit to indicate object must be removed from screen.


/*********************************************************************
* Used Constants 
*********************************************************************/
#define RADIAN 				1144	// Radian definition. Equivalent to sine(1) * 2^16.
#define PIIOVER2			102944	// The constant Pii divided by two (pii/2).

// Meter types 
#define MTR_WHOLE_TYPE		0
#define MTR_HALF_TYPE		1
#define MTR_QUARTER_TYPE	2

/* ************************************************************************** */									
// Overview: This is a compile time setting to select the type if meter shape.
// There are three types: 
//	    - MTR_WHOLE_TYPE   - Meter drawn with 6 octants used.
//		- MTR_HALF_TYPE    - Meter drawn with semi circle shape.
//		- MTR_QUARTER_TYPE - Meter drawn with quarter circle shape.
// Set only one value at a time. This is done to save code space.
// Depending on the type chosen, the meter normal, critical and danger 
// ranges are defined automatically. Set the meter type in Meter.h file.
/* ************************************************************************** */	
#define METER_TYPE					MTR_WHOLE_TYPE				

//#define METER_TYPE					MTR_HALF_TYPE				// Meter drawn with semi circle shape.
//#define METER_TYPE					MTR_QUARTER_TYPE			// Meter drawn with quarter circle shape.

// These selects the other parameters of the meter that are dependent on the shape.
#if (METER_TYPE == MTR_WHOLE_TYPE) 		
	
	#define DEGREE_START	-45		// Defines the start angle to draw the meter.
	#define DEGREE_END		225		// Defines the end angle to draw the meter.

	#define CRITICAL_DEGREE	 45		// Defines the critical boundary of the angle (when to use critical color).
	#define DANGER_DEGREE	  0		// Defines the danger boundary of the angle (when to use danger color).

#elif (METER_TYPE == MTR_HALF_TYPE) 

	#define DEGREE_START	  0		// Defines the start angle to draw the meter.
	#define DEGREE_END		180		// Defines the end angle to draw the meter.

	#define CRITICAL_DEGREE	 90		// Defines the critical boundary of the angle (when to use critical color).
	#define DANGER_DEGREE	 45		// Defines the danger boundary of the angle (when to use danger color).

#elif (METER_TYPE == MTR_QUARTER_TYPE) 

	#define DEGREE_START	  0		// Defines the start angle to draw the meter.
	#define DEGREE_END		 90		// Defines the end angle to draw the meter.

	#define CRITICAL_DEGREE	 45		// Defines the critical boundary of the angle (when to use critical color).
	#define DANGER_DEGREE	 20		// Defines the danger boundary of the angle (when to use danger color).

#endif

#define SCALECHARCOUNT		  3		// Defines how many characters will be allocated for the
									// scale labels. Use this define in accordance to 
									// the range. Example: if range = 500, SCALECHARCOUNT
									// should be 3. if range = 90, SCALECHARCOUNT = 2 
#define DEGREECOUNT		 	  9		// Defines how many degrees per scale, computed per octant 
									// Example: for 5 division per octant 45/5 = 9. 
									// So every 9 degrees a scale is drawn
									// for a 5 scale division per octant.



/*********************************************************************
* Overview: Defines the parameters required for a meter Object.
*			Depending on the type selected the meter is drawn with 
*			the defined shape parameters and values set on the 
*			given fields.
*
*********************************************************************/
typedef struct {
	OBJ_HEADER      hdr;			// Generic header for all Objects (see OBJ_HEADER).
	XCHAR 		   *pText;			// The text label of the meter.
	SHORT           value;			// Current value of the meter.
	SHORT           range;        	// Range of the meter, also used to derive the scales 
									// of the meter. Minimum is always 0 and maximum = range.

	SHORT           xCenter;		// The x coordinate center position. This is computed automatically. 
	SHORT           yCenter;		// The y coordinate center position. This is computed automatically. 
	SHORT           radius;			// Radius of the meter, also defines the needle length. 
									// This is computed automatically. 

	SHORT			xPos;			// The current x position of the needle. This is computed automatically. 
	SHORT			yPos;			// The current y position of the needle. This is computed automatically. 
	
	WORD 			normalColor; 	// Normal values scale color. BRIGHTGREEN is the default color set at creation. 
	WORD 			criticalColor;  // Critical values scale color. BRIGHTYELLOW is the default color set at creation. 
	WORD 			dangerColor;    // Danger values scale color. BRIGHTRED is the default color set at creation. 
} METER;

/*********************************************************************
* Function: METER  *MtrCreate(WORD ID, SHORT left, SHORT top, SHORT right, 
*							  SHORT bottom, WORD state, SHORT value, 
*							  SHORT range, XCHAR *pText, GOL_SCHEME *pScheme)				 
*
* Overview: This function creates a METER object with the parameters given. 
*			It automatically attaches the new object into a global linked list of 
*			objects and returns the address of the object.
*
* PreCondition: none
*
* Input: ID - Unique user defined ID for the object instance.
*        left - Left most position of the object.
* 		 top - Top most position of the object. 
*		 right - Right most position of the object.
*		 bottom - Bottom most position of the object.
*        state - Sets the initial state of the object.
*		 value - Initial value set to the meter.
*		 range - Range of the meter.
*        pText - Pointer to the text label of the meter.
*        pScheme - Pointer to the style scheme used.
*
* Output: Returns the pointer to the object created.
*
* Example:
*   <PRE> 
*	#define ID_METER 101
*
*	GOL_SCHEME *pMeterScheme;
*	METER *pMtr;
*	
*		pMeterScheme = GOLCreateScheme();
*
*		pMtr = MtrCreate(
*				ID_METER,			// assign ID
*				30, 50, 150, 180,	// set dimension
*				MTR_DRAW|MTR_RING,	// draw object after creation
*				0,					// set initial value
*				100,				// set range
*				"Speed",			// Text Label
*				pMeterScheme);		// style scheme   
*			  	  
*		// check if meter was created
*		if (pMtr == NULL)		
*		   return 0;
*
*		// Change range colors: Normal values to WHITE
*		//						Critical values to BLUE
*		//						Danger values to RED
*		// assume that WHITE, BLUE and RED have been defined.
*		MtrSetScaleColors(pMtr, WHITE, BLUE, RED);				
*
*		// draw the meter
*		while(!MtrDraw(pMtr);
*
*		return 1;
*	</PRE> 
*
* Side Effects: none
*
********************************************************************/
METER  *MtrCreate(WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom,
				  WORD state, SHORT value, SHORT range, 
				  XCHAR *pText, GOL_SCHEME *pScheme);				 

/*********************************************************************
* Function: WORD MtrTranslateMsg(METER *pMtr, GOL_MSG *pMsg)
*
* Overview: This function evaluates the message from a user if the 
*			message will affect the object or not. The table below enumerates the translated 
*			messages for each event of the touch screen and keyboard inputs.
*
*	<TABLE>
*    	Translated Message   Input Source  Events         	Description
*     	##################   ############  ######         	###########
*     	MTR_MSG_SET      	 System		   EVENT_SET	    If event set occurs and the meter ID is sent in parameter 1.
*		OBJ_MSG_INVALID		 Any		   Any			  	If the message did not affect the object.							 
*	</TABLE>
*
* PreCondition: none
*
* Input: pMtr  - The pointer to the object where the message will be
*				 evaluated to check if the message will affect the object.
*        pMsg  - Pointer to the message struct containing the message from 
*        		 the user interface.
*
* Output: Returns the translated message depending on the received GOL message:
*		  - MTR_MSG_SET – Meter ID is given in parameter 1 for a TYPE_SYSTEM message.
*         - OBJ_MSG_INVALID – Meter is not affected.
*
* Side Effects: none
*
********************************************************************/
WORD MtrTranslateMsg(METER *pMtr, GOL_MSG *pMsg);

/*********************************************************************
* Function: MtrMsgDefault(WORD translatedMsg, METER *pMtr, GOL_MSG* pMsg)
*
* Overview: This function performs the actual state change 
*			based on the translated message given. Meter value is set
*			based on parameter 2 of the message given. The following state changes 
*			are supported:
*	<TABLE>
*    	Translated Message   Input Source  Set/Clear State Bit		Description
*     	##################   ############  ######     				###########
*     	MTR_MSG_SET          System  	   Set MTR_DRAW_UPDATE  	Meter will be redrawn to update the needle position and value displayed.
*	</TABLE>
*
* PreCondition: none
*
* Input: translatedMsg - The translated message.
*        pMtr          - The pointer to the object whose state will be modified.
*        pMsg          - The pointer to the GOL message.
*        
* Output: none
*
* Side Effects: none
*
********************************************************************/
void  MtrMsgDefault(WORD translatedMsg, METER *pMtr, GOL_MSG* pMsg);

/*********************************************************************
* Macros:  MtrGetVal(pMtr)
*
* Overview: This macro returns the current value of the meter.
*			Value is always in the 0-range inclusive.
*
* PreCondition: none
*
* Input: pMtr - Pointer to the object.
*
* Output: Returns current value of the meter.
*
* Side Effects: none
*
********************************************************************/
#define MtrGetVal(pMtr)  	(pMtr)->value

/*********************************************************************
* Function: MtrSetVal(METER *pMtr, SHORT newVal)
*
* Overview: This function sets the value of the meter to the passed
*			newVal. newVal is checked to be in the 0-range inclusive.
*			If newVal is not in the range, 0 or maximum (range) is 
*			assigned depending on the given newVal if less than 0 
*			or above the range.
*
* PreCondition: none
*
* Input: pMtr   - The pointer to the object.
*        newVal - New value to be set for the Meter.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void MtrSetVal(METER *pMtr, SHORT newVal);

/*********************************************************************
* Macros:  MtrIncVal(pMtr, value)
*
* Overview: This macro is used to directly increment the value. 
*			The delta change used is always the resolution setting (page).
*
* PreCondition: none
*
* Input: pMtr - Pointer to the object.
*		 value - Number to be added to the current Meter value.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define MtrIncVal(pMtr, value)		MtrSetVal(pMtr, (pMtr->val + value))

/*********************************************************************
* Macros:  MtrDecVal(pMtr)
*
* Overview: This macro is used to directly decrement the value. 
*			The delta change used is always the resolution setting (page).
*
* PreCondition: none
*
* Input: pMtr - Pointer to the object.
*        value - Number to be subtracted to the current Meter value.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define MtrDecVal(pMtr, value)		MtrSetVal(pMtr, (pMtr->pos - value))

/*********************************************************************
* Macros:  MtrSetScaleColors(pMtr, normal, critical, danger)		
*								{	pMtr->normalColor=normal;		
*									pMtr->criticalColor=critical;	
*									pMtr->dangerColor=danger	}
*
* Overview: Scale colors can be used to highlight values of the meter.
*           User can select three colors to define normal value,
*			critical values and danger values colors. Limitation is that
*			color settings are set to the following angles:
*			Color Boundaries		Type Whole		Type Half
*			Normal					46  to 225		91 to 180
*			Critical				1   to 45		46 to 90
*			Danger					-45 to 0		0  to 45
*			As the meter is drawn colors are changed depending on the 
*			angle of the scale and label being drawn.
*
* PreCondition: The object must be created (using MtrCreate()) before
*				a call to this macro is performed.
*
* Input: pMtr - Pointer to the object.
*		 normal - Normal values color. 
*		 critical - Critical values color. 
*		 danger - Danger values color. 
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define MtrSetScaleColors(pMtr, normal, critical, danger)				\
								{	pMtr->normalColor=normal;			\
									pMtr->criticalColor=critical;		\
									pMtr->dangerColor=danger;	}

/*********************************************************************
* Function: WORD MtrDraw(METER *pMtr)
*
* Overview: This function renders the object on the screen using 
* 			the current parameter settings. Location of the object is 
*			determined by the left, top, right and bottom parameters. 
*			The colors used are dependent on the state of the object. 
*			The font used is determined by the style scheme set.
*			
*			Depending on the defined settings, value of the meter
*			will displayed or hidden. Displaying the value will require
*			a little bit more rendering time depending on the size
*			of the meter and font used.
*
*			When rendering objects of the same type, each object 
*			must be rendered completely before the rendering of the 
*			next object is started. This is to avoid incomplete 
*			object rendering.
*
* PreCondition: Object must be created before this function is called.
*
* Input: pMtr - Pointer to the object to be rendered.
*        
* Output: Returns the status of the drawing 
*		  - 1 - If the rendering was completed and 
*		  - 0 - If the rendering is not yet finished. 
*		  Next call to the function will resume the 
*		  rendering on the pending drawing state.
*
* Example:
*	See MtrCreate() example.
*
* Side Effects: none
*
********************************************************************/
WORD MtrDraw(METER *pMtr);

#endif // _METER_H
