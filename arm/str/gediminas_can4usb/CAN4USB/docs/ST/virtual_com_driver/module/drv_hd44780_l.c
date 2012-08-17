/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : drv_hd44780_l.c
 *    Description : I/O function
 *
 *    History :
 *    1. Date        : August 10, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 16170 $
**************************************************************************/
#include "drv_hd44780_l.h"

/*************************************************************************
 * Function Name: HD44780_IO_Init
 * Parameters: none
 * Return: none
 * Description: Init IO ports directions and level
 *
 *************************************************************************/
void HD44780_IO_Init (void)
{
GPIO_InitTypeDef  GPIO_InitStructure;

  // Enable GPIO clocks
  MRCC_PeripheralClockConfig(MRCC_Peripheral_GPIO, ENABLE);
  // Release GPIO reset
  MRCC_PeripheralSWResetConfig(MRCC_Peripheral_GPIO,DISABLE);

  /* IO configure */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Pin = LCD_DATA << LCD_DATA_SHIFT;
  GPIO_Init(LCD_DATA_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LCD_RS;
  GPIO_Init(LCD_RS_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LCD_E;
  GPIO_Init(LCD_E_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = LCD_LIGHT;
  GPIO_Init(LCD_LIGHT_PORT, &GPIO_InitStructure);

  // RS Output - Low
  HD44780SetRS(0);
  // E  Output - Low
  HD44780SetE(0);
  // Backlight off
  LCD_LIGHT_OFF();
}

/*************************************************************************
 * Function Name: HD44780SetRS
 * Parameters: Boolean Data
 * Return: none
 * Description: Set RS signal
 *
 *************************************************************************/
void HD44780SetRS (Boolean Data)
{
  GPIO_WriteBit(LCD_RS_PORT,LCD_RS,Data?Bit_SET:Bit_RESET);
}

/*************************************************************************
 * Function Name: HD44780SetE
 * Parameters: Boolean Data
 * Return: none
 * Description: Set E signal
 *
 *************************************************************************/
void HD44780SetE (Boolean Data)
{
  GPIO_WriteBit(LCD_E_PORT,LCD_E,Data?Bit_SET:Bit_RESET);
}

/*************************************************************************
 * Function Name: HD44780WrIO
 * Parameters: Int32U Data
 * Return: none
 * Description: Write to HD44780 I/O
 *
 *************************************************************************/
void HD44780WrIO (Int32U Data)
{
  // Write Data
  HD44780SetE(1);
  Data = (GPIO_Read(LCD_DATA_PORT) & ~(LCD_DATA << LCD_DATA_SHIFT)) | ((Data & LCD_DATA) << LCD_DATA_SHIFT);
  GPIO_Write(LCD_DATA_PORT,Data);
  HD44780_BUS_DLY();
  HD44780SetE(0);
}
