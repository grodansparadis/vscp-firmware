/******************************************************************************
*                                                                             *
*  File:                        SYSTEM.C                                      *
*                                                                             *
*  Description: This file contains the API for the PIC18F258 system           *
*               initialization, test, and control functions.                  *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Software License Agreement                                                 *
*                                                                             *
*  The software supplied herewith by Microchip Technology Incorporated        *
*  (the “Company”) for its PICmicro® Microcontroller is intended and          *
*  supplied to you, the Company’s customer, for use solely and                *
*  exclusively on Microchip PICmicro Microcontroller products. The            *
*  software is owned by the Company and/or its supplier, and is               *
*  protected under applicable copyright laws. All rights are reserved.        *
*   Any use in violation of the foregoing restrictions may subject the        *
*  user to criminal sanctions under applicable laws, as well as to            *
*  civil liability for the breach of the terms and conditions of this         *
*  license.                                                                   *
*                                                                             *
*  THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,          *
*  WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED          *
*  TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A                *
*  PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,          *
*  IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR                 *
*  CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.                          *
*                                                                             *
*******************************************************************************
*                                                                             *
*  Created By:  Thomas R. Shelburne 09/30/02                                  *
*                                                                             *
*  Versions:                                                                  *
*                                                                             *
*  MAJ.MIN - AUTHOR - CHANGE DESCRIPTION                                      *
*                                                                             *
*    1.0   - T.R.S. - Initial release of the system API definitions...        *
*                                                                             *
******************************************************************************/

#define  _SYSTEM_H_
#include "system.h"
#undef   _SYSTEM_H_

/* Local macro and constant definitions... */
#define EEPROM_START_ADDR    0x0000 /* KLU - These all need to be updated for the PIC18F4680... */
#define EEPROM_END_ADDR      0x00FF
#define FLASH_START_ADDR     0x0000
#define FLASH_END_ADDR       0x7FFF
#define RAM_START_ADDR       0x0000
#define RAM_END_ADDR         0x05FF

/* PIC18F4680 configuration bit settings... */
#pragma romdata CONFIG1H=0x300001
#ifdef _HS_MODE_
const rom unsigned char config1H=0b10000010; /* IESO enabled, clock HS mode... */
#else
const rom unsigned char config1H=0b10000100; /* IESO enabled, EC oscillator CLKOUT function on RA6 */
#endif
#pragma romdata CONFIG2L=0x300002
const rom unsigned char config2L=0b00011011; /* 2.1V brownout reset enabled, disabled powerup timer */
#pragma romdata CONFIG2H=0x300003
const rom unsigned char config2H=0b00011110; /* Watchdog disabled... */
#pragma romdata CONFIG3H=0x300005
const rom unsigned char config3H=0b10000100; /* MCLR enable, timer 1 low power, port B I/O by default... */
#pragma romdata CONFIG4L=0x300006
#ifdef _ENABLE_DEBUG_
const rom unsigned char config4L=0b01000001; /* Background debug enabled, extended instruction set, stack reset enabled */
#else
const rom unsigned char config4L=0b11000001; /* Background debug disabled, extended instruction set, stack reset enabled */
#endif
#pragma romdata CONFIG5L=0x300008
const rom unsigned char config5L=0b00001111; /* No code protection on blocks 0-3... */
#pragma romdata CONFIG5H=0x300009
const rom unsigned char config5H=0b11000000; /* No EEPROM protection, no boot block protection... */
#pragma romdata CONFIG6L=0x30000A
const rom unsigned char config6L=0b00001111; /* No write protection on blocks 0-3... */
#pragma romdata CONFIG6H=0x30000B
const rom unsigned char config6H=0b11100000; /* EEPROM/boot/config not write protected... */
#pragma romdata CONFIG7L=0x30000C
const rom unsigned char config7L=0b00001111; /* No table read protection on blocks 0-3... */
#pragma romdata CONFIG7H=0x30000D
const rom unsigned char config7H=0b01000000; /* No table read protection on boot block... */
#pragma romdata

/* Local function declarations... */
static STDERROR eeprom_test(void);
static STDERROR flash_test(void);
static STDERROR ram_test(void);

/* Local variable declarations... */

/******************************************************************************
*                      GLOBAL FUNCTION DEFINITIONS                            *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: void system_init(void)                                           *
*  Description: This function initializes all required system registers in    *
*               the PIC18F258 for proper operation. Global interrupts are     *
*               left in the disabled state so the application can re-enable   *
*               them at the appropriate time.                                 *
*  Returns: None                                                              *
*                                                                             *
******************************************************************************/
void system_init(void)
{
  /* Disable global ints in case of re-init... */
  GBL_HI_INTR_DISABLE;
  GBL_LO_INTR_DISABLE;

  /* Setup system registers... */
  RCONbits.IPEN = 1; /* Enable interrupt prioritization!!! */
  RCONbits.SBOREN = 0; /* Disable brown out reset... */

  /* Configure PORTB push button input pins... */
  TRISB |= INCR_SWITCH | DECR_SWITCH;
  
  /* Configure PORTC RC2 as an output... */
  TRISC &= ~(LED_8);
  LED_8_LOW;
  
  /* Configure Port D for all outputs... */
  CMCON = 0x07; /* Make RD0-RD3 digital pins!!! */
  TRISD = 0x00;
  PORTD = 0x00; /* Drive a default state out onto the LED's to start... */

  /* Enable global interrupts... */
  GBL_LO_INTR_ENABLE;
  GBL_HI_INTR_ENABLE;
}

/******************************************************************************
*                                                                             *
*  Function: void system_test(void)                                           *
*  Description: This function will execute a suite of non-destructive tests   *
*               on the PIC18F258 to test various hardware components...       *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
STDERROR system_test(void)
{
  STDERROR erReturnCode;

  /* Execute RAM test... */
  erReturnCode = ram_test();
  if ( GET_ERROR(erReturnCode) == SUCCESS )
  {
    /* Execute FLASH test... */
    erReturnCode = flash_test();
    if ( GET_ERROR(erReturnCode) == SUCCESS )
    {
      /* Execute EEPROM test... */
      erReturnCode = eeprom_test();
    }
  }

  return erReturnCode;
}

/******************************************************************************
*                       LOCAL FUNCTION DEFINITIONS                            *
******************************************************************************/

/******************************************************************************
*                                                                             *
*  Function: STDERROR eeprom_test(void)                                       *
*  Description: This function performs an internal EEPROM test...             *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
static STDERROR eeprom_test(void)
{
  /* KLU - Just stubbed out for now... */
  return ERR(EEPROM,SUCCESS);
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR flash_test(void)                                        *
*  Description: This function performs an internal FLASH test...              *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
static STDERROR flash_test(void)
{
  /* KLU - Just stubbed out for now... */
  return ERR(FLASH,SUCCESS);
}

/******************************************************************************
*                                                                             *
*  Function: STDERROR ram_test(void)                                          *
*  Description: This function performs an internal RAM test...                *
*  Returns: STDERROR                                                          *
*                                                                             *
******************************************************************************/
static STDERROR ram_test(void)
{
  /* KLU - Just stubbed out for now... */
  return ERR(RAM,SUCCESS);
}
