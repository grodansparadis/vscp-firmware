/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 * File: sweetbox.h
 *
 *  SweetBox Module
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Copyright (C) 2008-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/

/*

    Resource usage
    ==============
    
    SPI
    ===
    SPI2    - CAN (Move if audio DAC is used).
    SPI1    - Ethernet.
    SPI1    - MiWi.
    SPI2    - Volume control.
    
    I2C
    ===
    I2C1    - Tempsensor.
    I2C1    - EEPROM.
    I2C2    - LED Array.
     
    Timer 1 - 1 ms timebase 
    Timer 2 - Record/Playback timer
    Timer 3 - IR Transmitter 38KHz
    Timer 4 - Humidity
    Timer 5 -
    
    External Interrupts
    ===================
    INT1 - CAN (RE8)
    INT2 - MiWi (RE9)
    INT3 - Ethernet (RA14)
    INT4 -
    
    Analog Inputs
    =============
    AN0  - (RB0) Microphone 
    AN1  - (RB1) Barometer
    AN8  - (RB8) Ambient Light
    
    Digital Inputs
    ==============
    RB2 - Digital Input UART RI
    RE4 - Alarm input
    RF5 - Proximity
    RD12 - RX433 Data
    
    Digital Outputs
    ===============
    RC2 - Siren
    RE0 - CS for CAN
    RE1 - Reset for CAN
    RE2 - CS for Ethernet
    RE3 - Reset Ethernet
    RE5 - CS MiWi
    RE6 - Reset MiWi
    RE7 - RGB /OE
    RG9 - CS Volume
    RG12 - Reset Volume
    RG13 - UART DTR
    RF0 - TX433 data
    
    Change Notification
    ===================
    RB4 - Change Notification - WAKE_MIWI
    RB4 - Change Notification - WOL Ethernet
    
    Input Capture
    =============
    RD9 - Input Capture IR Receiver
    
    Clock Input
    ===========
    RC3 - T4 Clock input
    
    PWM
    ===
    RD3 - Audio PWM left.
    RD4 - Audio PWM right
    RD0 - IR transmitter
    
    
*/

#ifndef __SWEETBOX_TYPE_DEFS_H_
#define __SWEETBOX_TYPE_DEFS_H_

#include <inttypes.h>

#ifndef TRUE
#define TRUE    (-1)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

//typedef enum _BOOL { FALSE = 0, TRUE } BOOL;	// Undefined size

#ifndef NULL
#define NULL    ((void *)0)
#endif

// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))

// Timer tick
/*
typedef union _TICK
{
    uint32_t Val;
    struct _TICK_bytes
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } byte;
    uint8_t v[4];
    struct _TICK_words
    {
        uint16_t w0;
        uint16_t w1;
    } word;
} TICK;
*/

typedef unsigned long TICK;    


// Button definitions
#define BUTTON_INIT             BIT_6
#define BUTTON_FORWARD          BIT_7
#define BUTTON_BACKWARD         BIT_13

// LED definitions
#define LED_IR_TRANSMIT         BIT_0   // Red LED
#define LED_HOST_CONNECTION     BIT_1   // Yellow LED
#define LED_STATUS              BIT_2   // Green LED

// I2C addresses
#define I2C_ADDR_9803           0x4F
#define I2C_ADDR_24A1025_1      0x51    // 0x55 for upper block
#define I2C_ADDR_24A1025_2      0x52    // 0x56 for upper block 
#define I2C_ADDR_UPPER_BLOCK    0x04    // Select the upper 512K block

//Interrupt Definition Macros
#define ISR(PRIOR,VECT) void __attribute__ ((interrupt(PRIOR))) __attribute__ ((vector(VECT))) interrupt ## VECT (void)
#define INTERRUPT(PRIOR,VECT,NAME) void __attribute__ ((interrupt(PRIOR))) __attribute__ ((vector(VECT))) NAME (void)

// Priorities/stacks for the various tasks. 
#define WEBSERVER_PRIORITY      ( tskIDLE_PRIORITY + 2 )
#define CAN_PRIORITY            ( tskIDLE_PRIORITY + 2 )
#define MIWI_PRIORITY           ( tskIDLE_PRIORITY + 2 )
#define VSCP_PRIORITY           ( tskIDLE_PRIORITY + 2 )
#define SOUND_PRIORITY          ( tskIDLE_PRIORITY + 2 )
#define RECORD_PRIORITY         ( tskIDLE_PRIORITY + 2 )
#define RGBARRAY_PRIORITY       ( tskIDLE_PRIORITY + 2 )
#define HMI_PRIORITY            ( tskIDLE_PRIORITY + 1 )
#define AMBIENTLIGHT_PRIORITY   ( tskIDLE_PRIORITY + 1 )
#define BAROMETER_PRIORITY      ( tskIDLE_PRIORITY + 1 )
#define HUMIDITY_PRIORITY       ( tskIDLE_PRIORITY + 1 )
#define TEMPERATURE_PRIORITY    ( tskIDLE_PRIORITY + 1 )
#define PROXIMITY_PRIORITY      ( tskIDLE_PRIORITY + 1 )
#define SIREN_PRIORITY          ( tskIDLE_PRIORITY + 1 )
#define ALARM_PRIORITY          ( tskIDLE_PRIORITY + 1 )
#define RX433_PRIORITY          ( tskIDLE_PRIORITY + 2 )
#define TX433_PRIORITY          ( tskIDLE_PRIORITY + 2 )
#define IRTX_PRIORITY           ( tskIDLE_PRIORITY + 2 )
#define IRRX_PRIORITY           ( tskIDLE_PRIORITY + 2 )

#define stackLEDCtrl                        70

#define timebaseDiff(a, b)      (portTickType)(a < b) ? (((portTickType)0xffff - b) + a) : (a - b)

// Registers
#define REG_DESCION_MATRIX                  0


// EEPROM storage
#define VSCP_EEPROM_ZONE                    0x0000
#define VSCP_EEPROM_SUBZONE                 0x0001
#define VSCP_EEPROM_NICKNAME                0x0002

#define VSCP_EEPROM_REG_GUID                0x1000
#define VSCP_EEPROM_REG_MANUFACTUR_ID       0x1010
#define VSCP_EEPROM_REG_USERID              0x1020
#define VSCP_EEPROM_SEGMENT_CRC             0x1030
#define VSCP_EEPROM_CONTROL                 0x1032
#define VSCP_EEPROM_BOOTLOADER_FLAG         0x1033

#define VSCP_EEPROM_END                     0x2000

//
// Sweetbox registers.
// ===================
// Should be placed in this file in there numerical order
//

typedef struct __sweetbox_regs {
    uint8_t zone;               // Module zone
    uint8_t subzone;            // Module subzone
    uint8_t temperature[2];     // Current temperature (big endian)
} sweetbox_regs;    



#endif //__SWEETBOX_TYPE_DEFS_H_

