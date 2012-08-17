/*****************************************************************************
 *  Module for Microchip Graphics Library 
 *  Graphic Primitives Layer
 *****************************************************************************
 * FileName:        Primitive.h
 * Dependencies:    
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
 * Anton Alkhimenok  and
 * Paolo A. Tamayo		11/12/07	Version 1.0 release
 *****************************************************************************/

#ifndef _PRIMITIVE_H
#define _PRIMITIVE_H

/*********************************************************************
* Overview: Primitive lines are drawn using line type and line thickness.
*			There are 3 line styles and 2 types of line thickness.
*
*********************************************************************/

// Solid Line Style
#define SOLID_LINE         0
// Dotted Line Style
#define DOTTED_LINE        1
// Dashed Line Style
#define DASHED_LINE        4

// Normal Line (thickness is 1 pixel)
#define NORMAL_LINE        0
// Thick Line (thickness is 3 pixels)
#define THICK_LINE         1

/*********************************************************************
* Overview: Drawing bitmaps will have two modes, normal rendering
*			and stretched rendering. Stretched rendering effectively
*			doubles the image size in the horizontal and vertical 
*			direction.
*
*********************************************************************/

// Normal image stretch code
#define IMAGE_NORMAL       1
// Stretched image stretch code
#define IMAGE_X2           2

// Current line style
extern SHORT _lineType;

// Current line thickness
extern BYTE _lineThickness;

// constants used for circle/arc computation
#define SIN45  46341  // sin(45) * 2^16)
#define ONEP25 81920  // 1.25 * 2^16

// Current cursor coordinates
extern SHORT _cursorX;
extern SHORT _cursorY;

// Font orientation
extern BYTE _fontOrientation;

#define ORIENT_HOR  0
#define ORIENT_VER  1

// Character type used 
#ifdef USE_MULTIBYTECHAR
#define XCHAR   short
#else
#define XCHAR   char
#endif

// Memory type enumeration to determine the source of data. 
// Used in interpreting bitmap and font from different memory sources.
typedef enum{
    FLASH    = 0,    // internal flash  
    EXTERNAL = 1,    // external memory
    VIDEOBUF = 2     // video buffer
}TYPE_MEMORY;

/*********************************************************************
* Overview: This structure is used to describe external memory.
*
*********************************************************************/
typedef struct {
  TYPE_MEMORY type;         // must be EXTERNAL
  WORD        ID;           // memory ID
  DWORD       address;      // bitmap or font image address
} EXTDATA;

#ifdef __PIC32MX__
#define FLASH_BYTE  const BYTE 
#define FLASH_WORD  const WORD
#else
// Flash data with 24bit pointers 
#define FLASH_BYTE  char __prog__
#define FLASH_WORD  short int __prog__
#endif

/*********************************************************************
* Overview: Structure describing the bitmap header.
*
*********************************************************************/
typedef struct {
BYTE   compression;			// Compression setting
BYTE   colorDepth;			// Color depth used
SHORT  height;				// Image height
SHORT  width;				// Image width
} BITMAP_HEADER;

/*********************************************************************
* Overview: Structure for bitmap stored in FLASH memory.
*
*********************************************************************/
typedef struct {
  TYPE_MEMORY type;         // must be FLASH
  FLASH_BYTE* address; // bitmap image address
} BITMAP_FLASH;

// Structure for bitmap stored in EXTERNAL memory
#define BITMAP_EXTERNAL EXTDATA

/*********************************************************************
* Overview: Structure describing the font header.
*
*********************************************************************/
typedef struct {
BYTE  info;					// Reserved for future use (must be set to 0).
BYTE  fontID;				// User assigned value
WORD  firstChar;			// Character code of first character (e.g. 32).
WORD  lastChar;				// Character code of last character in font (e.g. 3006).
BYTE  reserved;				// Reserved for future use (must be set to 0).
BYTE  height;				// Font characters height in pixels.
} FONT_HEADER;

// Structure describing font glyph entry
typedef struct {
BYTE  offsetMSB;
BYTE  width;
WORD  offsetLSB;
} GLYPH_ENTRY;

/*********************************************************************
* Overview: Structure for font stored in FLASH memory.
*
*********************************************************************/
typedef struct {
  TYPE_MEMORY type;         // must be FLASH
  const char* address;      // font image address
} FONT_FLASH;

// Structure for font stored in EXTERNAL memory
#define FONT_EXTERNAL   EXTDATA

/*********************************************************************
* Overview: This defines the size of the buffer used by font functions
*			to retrieve font data from the external memory. The buffer 
*			size can be increased to accommodate large font sizes. 
*			The user must be aware of the expected glyph sizes of the 
*			characters stored in the font table.
*
********************************************************************/
#define EXTERNAL_FONT_BUFFER_SIZE    200


// Pointer to the current font image
extern void*   _font;
// First and last characters in the font
extern WORD    _fontFirstChar;
extern WORD    _fontLastChar;
// Installed font height
extern SHORT  _fontHeight;

/*********************************************************************
* Function: WORD Arc(SHORT xL, SHORT yT, SHORT xR, SHORT yB, 
*					 SHORT r1, SHORT r2, BYTE octant)
*
* Overview: Draws the octant arc of a beveled figure with given centers, radii
*			and octant mask. When r1 is zero and r2 has some value, a filled 
*			circle is drawn; when the radii have values, an arc of
*			thickness (r2-r1) is drawn; when octant = 0xFF, a full ring 
*			is drawn. When r1 and r2 are zero, a rectangular object is drawn, where
*			xL, yT specifies the left top corner; xR, yB specifies the right bottom
*			corner.
*
* PreCondition: none
*
* Input: xL - x location of the upper left center in the x,y coordinate.
*		 yT - y location of the upper left center in the x,y coordinate.
*		 xR - x location of the lower right center in the x,y coordinate.
*		 yB - y location of the lower right center in the x,y coordinate.
*		 r1 - The smaller radius of the two concentric cicles that defines the thickness
*			  of the object.
*	     r2 - The larger of radius the two concentric circles that defines the thickness 
*	     	  of the object.
*		 octant - Bitmask of the octant that will be drawn.
*				  Moving in a clockwise direction from x = 0, y = +radius
*                 - bit0 : first octant 
*                 - bit1 : second octant
*                 - bit2 : third octant 
*                 - bit3 : fourth octant
*                 - bit4 : fifth octant
*                 - bit5 : sixth octant
*                 - bit6 : seventh octant
*                 - bit7 : eight octant
*
* Output: Returns the rendering status.
*		  1 - If the rendering was completed and 
*		  0 - If the rendering is not yet finished. 
*
* Side Effects: none
*
********************************************************************/
WORD Arc(SHORT xL, SHORT yT, SHORT xR, SHORT yB, SHORT r1, SHORT r2, BYTE octant);

/*********************************************************************
* Function:  void InitGraph(void)
*
* Overview: This function initializes the display controller, sets 
*			the line type to SOLID_LINE, sets the screen to all BLACK, 
*			sets the current drawing color to WHITE, sets the graphic 
*			cursor position to upper left corner of the screen, sets 
*			active and visual pages to page #0, clears the active page 
*			and disables clipping. This function should be called before 
*			using the Graphics Primitive Layer.
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
 void InitGraph(void);

/*********************************************************************
* Macros:  GetX()
*
* Overview: This macro returns the current graphic cursor x-coordinate.
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
#define GetX() _cursorX

/*********************************************************************
* Macros:  GetX()
*
* Overview: This macro returns the current graphic cursor y-coordinate.
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
#define GetY() _cursorY

/*********************************************************************
* Macros:  MoveTo(x,y)
*
* Overview: This macro moves the graphic cursor to new x,y position.
*
* PreCondition: none
*
* Input: x - Specifies the new x position of the graphic cursor.
*        y - Specifies the new y position of the graphic cursor.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define  MoveTo(x,y) _cursorX=x; _cursorY=y;

/*********************************************************************
* Macros:  MoveRel(dX,dY)
*
* Overview: This macro moves the graphic cursor relative to the 
*			current location. The given dX and dY displacement can 
*			be positive or negative numbers.
*
* PreCondition: none
*
* Input: dX - Specifies the displacement of the graphic cursor for
*			  the horizontal direction.
*		 dY - Specifies the displacement of the graphic cursor for
*			  the vertical direction.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define MoveRel(dX,dY)  _cursorX+=dX; _cursorY+=dY;

/*********************************************************************
* Macro: SetFontOrientation(orient)
*
* Overview: This macro sets font orientation vertical or horizontal.
*
* PreCondition: none
*
* Input: orient - should be non-zero if the font orientation is vertical
*
* Output: none
*
********************************************************************/
 #define SetFontOrientation(orient) _fontOrientation = orient;

/*********************************************************************
* Macro: GetFontOrientation()
*
* Overview: This macro returns font orientation (0 == horizontal, 1 == vertical).
*
* PreCondition: none
*
* Input: none
*
* Output: font orientation (0 == horizontal, 1 == vertical)
*
********************************************************************/
 #define GetFontOrientation() _fontOrientation

/*********************************************************************
* Function: void OutChar(XCHAR ch)
*
* Overview: This function outputs a character from the current graphic 
*		    cursor position. OutChar() uses the current active font 
*		    set with SetFont(). 
*
* PreCondition: none
*
* Input: ch - The character code to be displayed.
*
* Output: none
*
* Side Effects: After the function is completed, the graphic cursor 
*			    position is moved in the horizontal direction by the 
*			    character width. Vertical position of the graphic cursor 
*			    is not changed.
*
********************************************************************/
 void OutChar(XCHAR ch);

/*********************************************************************
* Function: WORD OutText(XCHAR* textString)
*
* Overview: This function outputs a string of characters starting 
*			at the current graphic cursor position. The string must 
*			be terminated by a line feed or zero. For Non-Blocking 
*			configuration, OutText() may return control to the program 
*			due to display device busy status. When this happens zero 
*			is returned and OutText() must be called again to continue 
*			the outputting of the string. For Blocking configuration, 
*			this function always returns a 1. OutText() uses the current 
*			active font set with SetFont().
*
*
* Input: textString - Pointer to the string to be displayed.
*
* Output: For NON-Blocking configuration:
*         - Returns 0 when string is not yet outputted completely.
*         - Returns 1 when string is outputted completely.
*         For Blocking configuration:
*         - Always return 1.
*
* Side Effects: Current horizontal graphic cursor position will be moved 
*				to the end of the text. The vertical graphic cursor 
*				position will not be changed.
*
********************************************************************/
 WORD OutText(XCHAR* textString);

/*********************************************************************
* Function: WORD OutTextXY(SHORT x, SHORT y, XCHAR* textString)
*
* Overview: This function outputs a string of characters starting 
*			at the given x, y position. The string must be terminated 
*			by a line feed or zero. For Non-Blocking configuration, 
*			OutTextXY() may return control to the program due to 
*			display device busy status. When this happens zero is 
*			returned and OutTextXY() must be called again to continue 
*			the outputting of the string. For Blocking configuration, 
*			this function always returns a 1. OutTextXY() uses the 
*			current active font set with SetFont().
*
* Input: x - Defines the x starting position of the string.
*		 y - Defines the y starting position of the string.
*		 textString - Pointer to the string to be displayed.
*
* Output: For NON-Blocking configuration:
*         - Returns 0 when string is not yet outputted completely.
*         - Returns 1 when string is outputted completely.
*         For Blocking configuration:
*         - Always return 1.
*
* Example:
*   <PRE> 
*	void PlaceText(void)
*	{
*		SHORT width, height;
*		static const XCHAR text[] = "Touch screen to continue";
*		
*		SetColor(BRIGHTRED);                // set color
*		SetFont(pMyFont);                   // set font to my font
*		
*		// get string width & height
*		width = GetTextWidth(text, pMyFont);
*		height = GetTextHeight(pMyFont);
*		
*		// place string in the middle of the screen
*		OutTextXY(  (GetMaxX() - width) >> 1, \
*		(GetMaxY() – height) >> 1, \
*		(char*)text);
*	}
*	</PRE> 
*
* Side Effects: Current horizontal graphic cursor position will be 
*				moved to the end of the text. The vertical graphic 
*				cursor position will not be changed.
*
********************************************************************/
 WORD OutTextXY(SHORT x, SHORT y, XCHAR* textString);

/*********************************************************************
* Function: SHORT GetTextHeight(void* font)
*
* Overview: This macro returns the height of the specified font. 
*			All characters in a given font table have a constant 
*			height.
*
* Input: font - Pointer to the font image.
*
* Output: Returns the font height.
*
* Example:
*   See OutTextXY() example.
*
* Side Effects: none
*
********************************************************************/
SHORT GetTextHeight(void* font);

/*********************************************************************
* Function: SHORT GetTextWidth(XCHAR* textString, void* font)
*
* Overview: This function returns the width of the specified string 
*			for the specified font. The string must be terminated 
*			by a line feed or zero.
*
* Input: textString - Pointer to the string.
*		 font - Pointer to the font image.
*
* Output: Returns the string width in the specified font.
*
* Example:
*   See OutTextXY() example.
*
* Side Effects: none
*
********************************************************************/
SHORT GetTextWidth(XCHAR* textString, void* font);

/*********************************************************************
* Function: void SetFont(void* font)
*
* Overview: This function sets the current font used in OutTextXY(), 
*			OutText() and OutChar() functions.
*
* Input: font - Pointer to the new font image to be used.
*
* Output: none
*
* Example:
*   See OutTextXY() example.
*
* Side Effects: none
*
********************************************************************/
void SetFont(void* font);

/*********************************************************************
* Macros: SetLineType(lnType)
*
* Overview: This macro sets the line type to draw.
*
* Input: lnType - The type of line to be used. 
*		 	Supported line types:
*				- SOLID_LINE
* 				- DOTTED_LINE
*          		- DASHED_LINE
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetLineType(lnType) _lineType=lnType;

/*********************************************************************
* Macros: SetLineThickness(lnThickness)
*
* Overview: This macro sets sets line thickness to 1 pixel or 3 pixels.
*
* Input: lnThickness - Line thickness code (0 - 1 pixel; 1 - 3 pixels)
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define SetLineThickness(lnThickness) _lineThickness=lnThickness;

/*********************************************************************
* Function: void Line(SHORT x1, SHORT y1, SHORT x2, SHORT y2)
*
* Overview: This function draws a line with the current line type 
*	        from the start point to the end point.
*
* Input: x1 - x coordinate of the start point.
*		 y1 - y coordinate of the start point.
*		 x2 - x coordinate of the end point.
*		 y2 - y coordinate of the end point.
*
* Output: none
*
* Side Effects: The graphic cursor position is moved to the end 
*				point of the line.
*
********************************************************************/
void Line(SHORT x1, SHORT y1, SHORT x2, SHORT y2);

/*********************************************************************
* Macros: LineRel(dX, dY)
*
* Overview: This macro draws a line with the current line type from 
*			the current graphic cursor position to the position defined 
*			by displacement.
*
* Input: dX - Displacement from the current x position.
*		 dY - Displacement from the current y position.
*
* Output: none
*
* Side Effects: The graphic cursor position is moved to the end 
*				point of the line.
*
********************************************************************/
#define LineRel(dX,dY) Line(GetX(),GetY(),GetX()+dX,GetY()+dY)

/*********************************************************************
* Macros: LineTo(x,y)
*
* Overview: This macro draws a line with the current line type from 
*			the current graphic cursor position to the given x, y position.
*
* Input: x - End point x position.
*		 y - End point y poisiton.
*
* Output: none
*
* Side Effects: The graphic cursor position is moved to the end 
*				point of the line.
*
********************************************************************/
#define LineTo(x,y) Line(_cursorX,_cursorY,x,y)

/*********************************************************************
* Macro: Circle(x, y, radius)
*
* Overview: This macro draws a circle with the given center and radius.
*
* Input: x - Center x position. 
*		 y - Center y position.
*		 radius - the radius of the circle.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define Circle(x, y, radius) 		Bevel(x,y,x,y,radius)

/*********************************************************************
* Function: void Bevel(SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT rad)
*
* Overview: Draws a beveled figure on the screen. 
*           For a pure circular object x1 = x2 and y1 = y2. 
*           For a rectangular object radius = 0.
*
* Input: x1 - x coordinate position of the upper left center of the circle that 
*			  draws the rounded corners.
*		 y1 - y coordinate position of the upper left center of the circle that 
*			  draws the rounded corners.
*		 x2 - x coordinate position of the lower right center of the circle that 
*			  draws the rounded corners.
*		 y2 - y coordinate position of the lower right center of the circle that 
*			  draws the rounded corners.
*        rad - defines the redius of the circle, that draws the rounded corners.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void Bevel(SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT rad); 

/*********************************************************************
* Function: void Bevel(SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT rad)
*
* Overview: Draws a filled beveled figure on the screen. 
*           For a filled circular object x1 = x2 and y1 = y2. 
*           For a filled rectangular object radius = 0.
*
* Input: x1 - x coordinate position of the upper left center of the circle that 
*			  draws the rounded corners.
*		 y1 - y coordinate position of the upper left center of the circle that 
*			  draws the rounded corners.
*		 x2 - x coordinate position of the lower right center of the circle that 
*			  draws the rounded corners.
*		 y2 - y coordinate position of the lower right center of the circle that 
*			  draws the rounded corners.
*        rad - defines the redius of the circle, that draws the rounded corners.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
WORD FillBevel(SHORT x1, SHORT y1, SHORT x2, SHORT y2, SHORT rad);

/*********************************************************************
* Macro: FillCircle(SHORT x1, SHORT y1, SHORT rad)
*
* Overview: This macro draws a filled circle. Uses the FillBevel() function.
*
* Input: x1 - x coordinate position of the center of the circle.
*		 y1 - y coordinate position of the center of the circle.
*        rad - defines the redius of the circle.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#ifndef USE_DRV_FILLCIRCLE
#define FillCircle(x1, y1, rad) FillBevel(x1, y1, x1, y1, rad)
#endif // end of USE_DRV_FILLCIRCLE

/*********************************************************************
* Macro: Rectangle(left, top, right, bottom)
*
* Overview: This macro draws a rectangle with the given left, 
*			top and right, bottom corners. Current line type is used.
*
* Input: left - x position of the left top corner.
*		 top - y position of the left top corner.
*		 right - x position of the right bottom corner.
*		 bottom - y position of the right bottom corner.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
#define Rectangle(left, top, right, bottom) Bevel(left, top, right, bottom, 0)

/*********************************************************************
* Function: void DrawPoly(SHORT numPoints, SHORT* polyPoints)
*
* Overview: This function draws a polygon with the current line 
*			type using the given number of points. The polygon points 
*			are stored in an array arranged in the following order:
*   <PRE> 
*            SHORT polyPoints[numPoints] = {x0, y0, x1, y1, x2, y2 … xn, yn};
*            Where n = numPoints - 1
*	</PRE> 
*
* Input: numPoints - Defines the number of points in the polygon.
*		 polyPoints - Pointer to the array of polygon points.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void DrawPoly(SHORT numPoints, SHORT* polyPoints);

/*********************************************************************
* Function: void Bar(SHORT left, SHORT top, SHORT right, SHORT bottom)
*
* Overview: This function draws a bar given the left, top and right, 
*			bottom corners with the current color.
*
* Input: left - x position of the left top corner.
*		 top - y position of the left top corner.
*		 right - x position of the right bottom corner.
*		 bottom - y position of the right bottom corner.
*
* Output: none
*
* Side Effects: none
*
********************************************************************/
void Bar(SHORT left, SHORT top, SHORT right, SHORT bottom);

/*********************************************************************
* Function: void ClearDevice(void)
*
* Overview: This function clears the screen with the current color 
*			and sets the graphic cursor position to (0, 0). 
*			Clipping is NOT supported by ClearDevice().
*
* Input: none
*
* Output: none
*
* Example:
*	<PRE> 
*	void ClearScreen(void)
*	{
*		SetColor(WHITE);		// set color to WHITE
*		ClearDevice();			// set screen to all WHITE
*	}
*   </PRE>
* 
* Side Effects: none
*
********************************************************************/
void ClearDevice(void);

/*********************************************************************
* Function: void PutImage(SHORT left, SHORT top, void* bitmap, BYTE stretch)
*
* Overview: This function outputs image starting from left,top coordinates.
*
* Input: left - x coordinate position of the left top corner.
*		 top - y coordinate position of the left top corner.
*        bitmap - pointer to the bitmap.
*        stretch - The image stretch factor.
*
* Output: none
* 
* Side Effects: none
*
********************************************************************/
void PutImage(SHORT left, SHORT top, void* bitmap, BYTE stretch);

/*********************************************************************
* Function: SHORT GetImageWidth(void* bitmap)
*
* Overview: This function returns the image width.
*
* Input: bitmap - Pointer to the bitmap.
*
* Output: Returns the image width in pixels.
* 
* Side Effects: none
*
********************************************************************/
SHORT GetImageWidth(void* bitmap);

/*********************************************************************
* Function: SHORT GetImageHeight(void* bitmap)
*
* Overview: This function returns the image height.
*
* Input: bitmap - Pointer to the bitmap.
*
* Output: Returns the image height in pixels.
* 
* Side Effects: none
*
********************************************************************/
SHORT GetImageHeight(void* bitmap);

/*********************************************************************
* Function: WORD ExternalMemoryCallback(EXTDATA* memory, LONG offset, WORD nCount, void* buffer)
*
* Overview: This function must be implemented in the application. 
*           The library will call this function each time when
*           the external memory data will be required. The application
*           must copy requested bytes quantity into the buffer provided.
*           Data start address in external memory is a sum of the address
*           in EXTDATA structure and offset.
*
* Input:  memory - Pointer to the external memory bitmap or font structures
*                  (FONT_EXTERNAL or BITMAP_EXTERNAL).
*         offset - Data offset.
*         nCount - Number of bytes to be transferred into the buffer.
*         buffer - Pointer to the buffer.
*
* Output: Returns the number of bytes were transferred.
* 
* Example:
*   <PRE>
*   // If there are several memories in the system they can be selected by IDs.
*   // In this example, ID for memory device used is assumed to be 0.
*   #define X_MEMORY 0
*
*   WORD ExternalMemoryCallback(EXTDATA* memory, LONG offset, WORD nCount, void* buffer) {
*   	int i;
*       long address;
*
*		// Address of the requested data is a start address of the object referred by EXTDATA structure plus offset
*		address = memory->address+offset;
*
*       if(memory->ID == X_MEMORY){
*       	// MemoryXReadByte() is some function implemented to access external memory. 
*           // Implementation will be specific to the memory used. In this example 
*			// it reads byte each time it is called. 
*           i = 0;
*           while (i < nCount) {
*           	(BYTE*)buffer = MemoryXReadByte(address++);
*               i++;
*           }
*      }
*      // return the actual number of bytes retrieved
*      return (i);
*  }
*  </PRE>
*
* Side Effects: none
*
********************************************************************/
WORD ExternalMemoryCallback(EXTDATA* memory, LONG offset, WORD nCount, void* buffer);

#endif // _PRIMITIVE_H




