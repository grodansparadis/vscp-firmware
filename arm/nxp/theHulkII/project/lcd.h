//lcd.h

#ifndef _lcd_h_
#define _lcd_h_

#define		LCD_DD_RAM_ADDR1   0x00000080	// LCD 1 row
#define		LCD_DD_RAM_ADDR2   0x000000C0	// LCD 2 row
#define		LCD_CLR_DISP	   0x00000001	
#define		LCD_DISP_ON		   0x0000000C	
#define		LCD_DISP_OFF	   0x00000008	
#define		LCD_CUR_HOME       0x00000002
#define		LCD_CUR_OFF 	   0x0000000C
#define     LCD_CUR_ON_UNDER   0x0000000E
#define     LCD_CUR_ON_BLINK   0x0000000F
#define     LCD_CUR_BACKSPACE  0x00000010
#define     LCD_CUR_SPACE      0x00000014
#define		LCD_CUR_UP  	   0x00000080	
#define		LCD_CUR_DOWN	   0x000000C0
#define     LCD_ENTER          0x000000C0
#define     LCD_TWO_ROWS       0x00000028

void E_Pulse( void );
void LCDInit( void );
void LCDSendCommand( unsigned long a );
void LCDSendChar( unsigned long a );
void LCDSendTxt( char* a );

#endif
