//
//  $Id: lcd.h 156 2008-10-15 07:05:54Z akhe $
//  $Revision: 156 $
//  $Author: akhe $
//  $Date: 2008-10-15 09:05:54 +0200 (on, 15 okt 2008) $
//  $HeadURL: http://m2m.svn.sourceforge.net/svnroot/m2m/trunk/firmware/arm/lpc/common/LPC2148_Demo/lcd/lcd.h $
//

#ifndef _LCD_H_
#define _LCD_H_

//
//
//
void lcdWriteChar (char c);
void lcdWriteText (const char *s);
void lcdCursorXY (int x, int y);
void lcdInit (void);
portTASK_FUNCTION (vLCDTask, pvParameters);

#endif
