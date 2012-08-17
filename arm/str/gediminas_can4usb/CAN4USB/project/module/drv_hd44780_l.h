/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : drv_hd44780_l.h
 *    Description : STR750 HD44780 low level I/O function driver
 *
 *    History :
 *    1. Date        : June 3, 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 16170 $
 **************************************************************************/
#include "includes.h"

#ifndef  __DRV_HD44780_L_H
#define  __DRV_HD44780_L_H

#define LCD_LIGHT                 GPIO_Pin_2
#define LCD_LIGHT_PORT            GPIO2

#define LCD_RS                    GPIO_Pin_1
#define LCD_RS_PORT               GPIO2

#define LCD_E                     GPIO_Pin_0
#define LCD_E_PORT                GPIO2

#define LCD_DATA                  0xFFUL
#define LCD_DATA_SHIFT            10
#define LCD_DATA_PORT             GPIO2

#define LCD_LIGHT_ON()            GPIO_WriteBit(LCD_LIGHT_PORT,LCD_LIGHT,Bit_SET)
#define LCD_LIGHT_OFF()           GPIO_WriteBit(LCD_LIGHT_PORT,LCD_LIGHT,Bit_RESET)

/* Delay macro */
#define HD44780_BUS_DLY()         for(volatile int dly = 30;dly;--dly)

void HD44780_IO_Init (void);
void HD44780SetRS (Boolean Data);
void HD44780SetE (Boolean Data);
void HD44780WrIO (Int32U);

#endif  /* __DRV_HD44780_L_H */
