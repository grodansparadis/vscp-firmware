/*********************************************************************
 *
 *                  Explorer 16 LCD
 *
 *********************************************************************
 * FileName:        mstimer.c
 * Processor:       PIC24 /  Daytona
 * Complier:        MPLAB C30/C32
 *
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement:
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its dsPIC30F and PICmicro® Microcontroller is intended 
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip's dsPIC30F and PICmicro Microcontroller products. 
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date      Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *
 ********************************************************************/
#include "ex16lcd.h"
#include "mstimer.h"
#include "peripheral\pmp.h"

#define LCD_CLEAR_DISPALY_CMD           0x01
                                        
#define LCD_CURSOR_HOME_CMD             0x02
                                        
#define LCD_ENTRY_MODE_CMD              0x04
#define LCD_ENTRY_MODE_INCREASE         0x02
#define LCD_ENTRY_MODE_DECREASE         0x00
#define LCD_ENTRY_MODE_DISPLAY          0x01
#define LCD_ENTRY_MODE_CURSOR           0x00
                                        
#define LCD_DISPLAY_CTRL_CMD            0x08
#define LCD_DISPLAY_CTRL_DISPLAY_ON     0x04
#define LCD_DISPLAY_CTRL_DISPLAY_OFF    0x00
#define LCD_DISPLAY_CTRL_CURSOR_ON      0x02
#define LCD_DISPLAY_CTRL_CURSOR_OFF     0x00
#define LCD_DISPLAY_CTRL_BLINK_ON       0x01
#define LCD_DISPLAY_CTRL_BLINK_OFF      0x00

#define LCD_CUR_DISPLAY_SHIFT_CMD       0x10
#define LCD_CUR_DISPLAY_SHIFT_DISPLAY   0x08
#define LCD_CUR_DISPLAY_SHIFT_CURSOR    0x00
#define LCD_CUR_DISPLAY_SHIFT_RIGHT     0x04
#define LCD_CUR_DISPLAY_SHIFT_LEFT      0x00

#define LCD_FUNCTION_SET_CMD            0x20
#define LCD_FUNCTION_SET_8_BITS         0x10
#define LCD_FUNCTION_SET_4_BITS         0x00
#define LCD_FUNCTION_SET_2_LINES        0x08
#define LCD_FUNCTION_SET_1_LINE         0x00
#define LCD_FUNCTION_SET_LRG_FONT       0x04
#define LCD_FUNCTION_SET_SML_FONT       0x00

#define LCD_CGRAM_ADDR_CMD              0x40

#define LCD_DDRAM_ADDR_CMD              0x80

#define LCD_NEW_LINE                    0xC0
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
inline void __attribute__((always_inline)) Ex16LCDCmd(BYTE cmd, UINT wait)
{
	PMPSetAddress(0x0000);
	PMPMasterWrite(cmd);
    MSTimerWait(wait);                
  
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
inline void __attribute__((always_inline)) Ex16LCDPutChar(BYTE data)
{
    PMPSetAddress(0x0001);
	PMPMasterWrite(data);
    MSTimerWait(1);                // wait 1ms
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
inline BYTE __attribute__((always_inline)) Ex16LCDGetChar(void)
{
    BYTE data;

    PMPSetAddress(0x0001);
	data = PMPMasterRead();
    MSTimerWait(1);                // wait 1ms

}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/

void Ex16LCDInit(UINT pb_clk)
{
    MSTimerInit(pb_clk);

	
	mPMPOpen((PMP_ON | PMP_READ_WRITE_EN | PMP_CS2_CS1_EN | PMP_LATCH_POL_HI | PMP_CS2_POL_HI | PMP_CS1_POL_HI | PMP_WRITE_POL_HI | PMP_READ_POL_HI),
			(PMP_MODE_MASTER1 | PMP_WAIT_BEG_4 | PMP_WAIT_MID_15 | PMP_WAIT_END_4),
			PMP_PEN_0,
			0);
    
    MSTimerWait(30);                // wait 30ms

	
    // set up function
    Ex16LCDCmd(LCD_FUNCTION_SET_CMD | LCD_FUNCTION_SET_8_BITS | LCD_FUNCTION_SET_2_LINES | LCD_FUNCTION_SET_LRG_FONT, 1);

    // turn display on
    Ex16LCDCmd(LCD_DISPLAY_CTRL_CMD | LCD_DISPLAY_CTRL_DISPLAY_ON, 1);

    // clear display
    Ex16LCDCmd(LCD_CLEAR_DISPALY_CMD, 2);

    // increase the cursor
    Ex16LCDCmd(LCD_ENTRY_MODE_CMD | LCD_ENTRY_MODE_INCREASE, 2);
}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
void Ex16LCDWriteLine(UINT number, const BYTE *line)
{
    BYTE i;

    if(number == 2)
        Ex16LCDCmd(LCD_NEW_LINE, 2);
    else
        Ex16LCDCmd(LCD_CURSOR_HOME_CMD, 2);

    for(i = 0; i < EX16LCD_LINE_SIZE; i++)
    {
        if(!line[i])
            break;

        Ex16LCDPutChar(line[i]);
    }

    for(; i < EX16LCD_LINE_SIZE; i++)
        Ex16LCDPutChar(' ');

}
/*********************************************************************
 * Function:       
 *
 * PreCondition:    
 *
 * Input:           
 *
 * Output:          
 *
 * Side Effects:    
 *
 * Overview:        
 *
 * Note:           
 ********************************************************************/
void Ex16LCDReadLine(UINT number, BYTE *line)
{
    BYTE i;

    if(number == 2)
        Ex16LCDCmd(LCD_NEW_LINE, 1);
    else
        Ex16LCDCmd(LCD_CURSOR_HOME_CMD, 1);

    for(i = 0; i < EX16LCD_LINE_SIZE; i++)
    {
        line[i] = Ex16LCDGetChar();
    }

    line[i] ='\0';
}
