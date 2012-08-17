/*****************************************************************************
 *  Module for Microchip Graphics Library 
 *  Grid control
 *****************************************************************************
 * FileName:        Grid.h
 * Dependencies:    none
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
 * Kim Otten            02/29/08    ...
 *****************************************************************************/
#ifndef _GRID_H_
#define _GRID_H_

#include <Graphics\GOL.h>

//******************************************************************************
// Section: Grid State Definitions
//******************************************************************************

#define GRID_FOCUSED                0x0001      // Bit for focused state
#define GRID_DISABLED               0x0002      // Bit for disabled state
#define GRID_SHOW_LINES             0x0004      // Display grid lines
#define GRID_SHOW_FOCUS             0x0008      // Highlight the focused cell.
#define GRID_SHOW_BORDER_ONLY       0x0010      // Draw only the outside border of the grid.
#define GRID_SHOW_SEPARATORS_ONLY   0x0020      // Draw only the lines between cells (like Tic-tac-toe)
#define GRID_DRAW_ITEMS             0x1000      // Bit to indicate that at least one item must be redrawn, but not the entire grid.
#define GRID_DRAW_ALL               0x4000      // Bit to indicate whole edit box must be redrawn
#define GRID_HIDE                   0x8000      // Bit to remove object from screen

#define GRIDITEM_SELECTED           0x0001      // The cell is selected.
#define GRIDITEM_IS_TEXT            0x0000      // The grid item is a test string.
#define GRIDITEM_IS_BITMAP          0x0008      // The grid item is a bitmap.
#define GRIDITEM_TEXTRIGHT          0x0010      // Text in the cell is right aligned.
#define GRIDITEM_TEXTLEFT           0x0020      // Text in the cell is left aligned.
#define GRIDITEM_TEXTBOTTOM         0x0040      // Bit to indicate text is top aligned.
#define GRIDITEM_TEXTTOP            0x0080      // Bit to indicate text is bottom aligned.
#define GRIDITEM_DRAW               0x0100      // Draw this cell

#define GRID_TYPE_MASK              (0x0C)

#define GRID_SUCCESS                0x0000
#define GRID_OUT_OF_BOUNDS          0x0001

#define GRID_MAX_COLUMNS(rw,cw)     ((rw - (GOL_EMBOSS_SIZE * 2) + 1) / (cw+1))
#define GRID_MAX_ROWS(rh,ch)        ((rh - (GOL_EMBOSS_SIZE * 2) + 1) / (ch+1))

#define GRID_WIDTH(c,cw)            ((GOL_EMBOSS_SIZE * 2) + (c * (cw+1)) - 1)
#define GRID_HEIGHT(r,ch)           ((GOL_EMBOSS_SIZE * 2) + (r * (ch+1)) - 1)


typedef struct
{
    void            *data;
    WORD            status;
} GRIDITEM;


typedef struct
{
	OBJ_HEADER      hdr;					// Generic header for all Objects (see OBJ_HEADER).   
    SHORT           numColumns;				// Number of columns drawn for the Grid.
    SHORT           numRows;				// Number of rows drawn for the Grid.
    SHORT           cellHeight;				// The height of each cell in pixels.
    SHORT           cellWidth;				// The width of each cell in pixels.

    SHORT           focusX;					// The x position of the cell to be focused.
    SHORT           focusY;					// The y position of the cell to be focused.
    
    GRIDITEM        *gridObjects;			// 
} GRID;


void GridClearCellState( GRID *pGrid, SHORT column, SHORT row, WORD state );
GRID *GridCreate( WORD ID, SHORT left, SHORT top, SHORT right, SHORT bottom, WORD state, SHORT numColumns, SHORT numRows,
                    SHORT cellWidth, SHORT cellHeight, GOL_SCHEME *pScheme );
WORD GridDraw( GRID *pGrid );
void GridFreeItems( GRID *pGrid );
void * GridGetCell( GRID *pGrid, SHORT column, SHORT row, WORD *cellType );
#define GridGetFocusX( pGrid )  pGrid->focusX
#define GridGetFocusY( pGrid )  pGrid->focusY
void GridMsgDefault( WORD translatedMsg, GRID *pGrid, GOL_MSG *pMsg );
WORD GridSetCell( GRID *pGrid, SHORT column, SHORT row, WORD state, void *data );
void GridSetCellState( GRID *pGrid, SHORT column, SHORT row, WORD state );
void GridSetFocus( GRID *pGrid, SHORT column, SHORT row );
WORD GridTranslateMsg( GRID *pCb, GOL_MSG *pMsg );

#endif

