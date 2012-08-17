/*******************************************************************************
* File Name          : lcd.c
* Author             : SJ
* Date First Issued  : 14/07/2003
* Description        : STR71x Demo Board LCD screen driver
*******************************************************************************/

// modified by Martin Thomas: delay-loop, EMI waitstates

#include "lcd.h"
#include "emi.h"
#include "gpio.h"

#define LCD_EMI_BANK  EMI_BANK2

/*******************************************************************************
* Function Name  : LCD_Delay
* Description    : 
* Input          : Delay factor
* Output         : 
* Return         : 
*******************************************************************************/

void LCD_Delay (u16 Factor)
{
	// mthomas: added volatile and addiditonal loop
	volatile u32 i,j,k;
	
	for (i=0; i<Factor; i++) {
		for (j=0; j<0x35; j++) {
			;//wait 100us if MCLK=4MHz
			for ( k=0; k<4; k++ ) {
				;
			}
		}
	}
}
/*******************************************************************************
* Function Name  : LCD_Command
* Description    : Send a commend to the LCD
* Input          : Command
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_Command ( u8 command )
{
  *(u8 *)(LCD_EMI_BANK) = command;
  //wait
  LCD_Delay (1);
}
/*******************************************************************************
* Function Name  : LCD_Data
* Description    : 
* Input          : Data
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_Data ( u8 data )
{
  *(u8 *)(LCD_EMI_BANK + 4) = data;
  //wait
  LCD_Delay (1);
}
/*******************************************************************************
* Function Name  : LCD_SetCharacterAddress
* Description    : 
* Input          : Address
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_SetCharacterAddress ( u8 address )
{
  LCD_Command (0x40 + 8*address);
}
/*******************************************************************************
* Function Name  : LCD_AutoScrollOn
* Description    : Activate the automatic scroll
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_AutoScrollOn(void)
{
  LCD_Command (0x07);
}
/*******************************************************************************
* Function Name  : LCD_AutoScrollOff
* Description    : Deactivate the automatic scroll.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_AutoScrollOff(void)
{
  LCD_Command (0x04);
}
/*******************************************************************************
* Function Name  : LCD_SetCursorPosition
* Description    : Position the LCD cursor at a desired row and column.
* Input          : Col, Row
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursorPosition(u8 col, u8 row)
{
  switch (row) {
    case 1: LCD_Command (0x80 + col - 1); LCD_Delay (10); break;
    case 2: LCD_Command (0xc0 + col - 1); LCD_Delay (10); break;
    default: break;
  }
}
/*******************************************************************************
* Function Name  : LCD_SetCursorHome
* Description    : Return the LCD cursor to the original position.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursorHome(void)
{
  LCD_Command (0x02);
  //wait for 1.6 ms
  LCD_Delay (16);  
}
/*******************************************************************************
* Function Name  : LCD_UnderlineCursorOn
* Description    : The cursor is displayed.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_UnderlineCursorOn(void)
{
  LCD_Command (0x0E);
}
/*******************************************************************************
* Function Name  : LCD_UnderlineCursorOff
* Description    : The cursor disappears.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_UnderlineCursorOff(void)
{
  LCD_Command (0x0C);
}
/*******************************************************************************
* Function Name  : LCD_BlockCursorOn
* Description    : The blinking is set.
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_BlockCursorOn(void)
{
  LCD_Command (0x0D);
}
/*******************************************************************************
* Function Name  : LCD_BlockCursorOff
* Description    : The blinking is over.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_BlockCursorOff(void)
{
   LCD_Command (0x0C);
}
/*******************************************************************************
* Function Name  : LCD_CursorLeft
* Description    : Move the cursor one character to the left.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CursorLeft(void)
{
   LCD_Command (0x10);
}
/*******************************************************************************
* Function Name  : LCD_CursorRight
* Description    : Move the cursor one character to the right.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CursorRight(void)
{
   LCD_Command (0x14);
}
/*******************************************************************************
* Function Name  : LCD_Init
* Description    : Initialize the LCD screen
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Init(void)
{
  GPIO_Config (GPIO2, 0x0004, GPIO_AF_PP);
  //mt  EMI_Config ( 2, EMI_ENABLE | EMI_WAITSTATE (0xA) | EMI_SIZE_8 );
  EMI_Config ( 2, EMI_ENABLE | EMI_WAITSTATE (0xD) | EMI_SIZE_8 );

  //wait for 15 ms
  LCD_Delay (150);
  //function set
  LCD_Command (0x38);

  //wait for 4.1 ms
  LCD_Delay (41);  
  //function set 
  LCD_Command (0x38);

  //wait for 100 us
  LCD_Delay (2);
  //function set 
  LCD_Command (0x38);
 
  //function set, 8 bits long, 2 display lines, 5x8 dots display font type
  LCD_Command (0x38);
  //wait 1.6ms
  LCD_Delay (16);
 
  //display off 
  LCD_Command (0x08);
  //wait 1.6ms
  LCD_Delay (16);
 
  //display clear 
  LCD_Command (0x01);
  //wait 1.6ms  
  LCD_Delay (16);

  //entry mode set, assign cursor moving direction and disable the shift of entire display 
  LCD_Command (0x06);        
  //wait 1.6ms
  LCD_Delay (1600);
}
/*******************************************************************************
* Function Name  : LCD_DisplayCharacter
* Description    : Display a character at a given position.
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_DisplayCharacter (u8 c, u8 col, u8 row)
{
  LCD_SetCursorPosition(col, row);
  LCD_Data (c);
  //wait 1.6ms
  LCD_Delay (1);
}
/*******************************************************************************
* Function Name  : LCD_CreateCharacter
* Description    : Create a character in the CGRAM (character generator RAM)
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_CreateCharacter(u8 c, u8 *pattern)
{
  int i;
  LCD_SetCharacterAddress (c);
  for (i=0; i<7; i++)
    LCD_Data (pattern[i]);
  LCD_Data (0x00);
}
/*******************************************************************************
* Function Name  : LCD_String
* Description    : Display a string at a given position
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_String(char *String, u8 col, u8 row)
{
  int i;
  for (i=0; String[i] != '\0'; i++)
    LCD_DisplayCharacter(String[i], col+i, row);
}

/*******************************************************************************
* Function Name  : LCD_ClearLine
* Description    : Clear a given line
* Input          : line number
* Output         : 
* Return         : 
*******************************************************************************/
void LCD_ClearLine (u8 line)
{
  LCD_String("                ", 1, line);
}
/*******************************************************************************/
