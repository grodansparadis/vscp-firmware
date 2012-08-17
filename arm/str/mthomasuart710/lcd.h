/********************************************************************************
* File Name          : lcd.h
* Author             : SJ
* Date First Issued  : 14/07/2007
* Description        : STR71x Demo Board LCD screen driver
*******************************************************************************/
#ifndef __LCD_H
#define __LCD_H

#include "71x_type.h"

void LCD_Delay(u16 time);
void LCD_Init(void);
void LCD_AutoScrollOn(void);
void LCD_AutoScrollOff(void);
void LCD_SetCursorPosition(u8 col, u8 row);
void LCD_SetCursorHome(void);
void LCD_UnderlineCursorOn(void);
void LCD_UnderlineCursorOff(void);
void LCD_BlockCursorOn(void);
void LCD_BlockCursorOff(void);
void LCD_CursorLeft(void);
void LCD_CursorRight(void);

void LCD_DisplayCharacter (u8 c, u8 col, u8 row);
void LCD_CreateCharacter(u8 c, u8 *pattern);

void LCD_String(char *String, u8 col, u8 row);
void LCD_ClearLine (u8 line);

#endif // __LCD_H

/*******************************************************************************/
