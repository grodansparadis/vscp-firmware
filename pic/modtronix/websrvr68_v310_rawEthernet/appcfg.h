/**
 * @brief           Application Configuration functions
 * @file            appcfg.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_conf_appconfig
 *
 * @section description Description
 **********************************
 * For details on application configuration data, see @ref mod_conf_appconfig
 * "Application Configuration" module!
 *
 *
 * @subsection appcfg_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed
 * in the projdefs.h (or similar) file.
 * For details, see @ref mod_conf_projdefs "Project Configuration".
 * To configure the module, the required defines should be uncommended, and the rest commented out.
 @code
*/
 
 //*******************************************************
 //----------------- Appcfg Configuration -----------------
 //********************************************************
 //Define if this application has an 8 bit ADC converter, 10 bit ADC converter or none at all.
 //#define APP_USE_ADC8
 #define APP_USE_ADC10

 //Define buffer space that is reserved for ADC conversion values. For a 8 bit ADC converted, 1 byte
 //is reserved per channel. For an 10 bit ADC, 2 bytes are reserved per channel.
 #define ADC_CHANNELS 12

/* 
 @endcode
 *********************************************************************/

 /*********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2004-10-19, David Hosken (DH):
 *  - Initial version
 * 2009-05-29, David Hosken (DH):
 *  - Added code for MXD2R and IOR5E expansion boards
 *********************************************************************/


/////////////////////////////////////////////////
//  Application Configuration - documentation module
 /**
 * @defgroup mod_conf_appconfig Application Configuration
 * @ingroup mod_conf
 * The application configuration is stored in the PIC microcontroller's internal
 * EEPROM in the first 256 bytes of data. The structure of the data is defined
 * in the @ref APP_CONFIG_ENUM enumeration.<br>
 * The device's default configuration is set in the eeConfigArray[] array in the
 * appcfg.c file. This array will define the values for the entire APP_CONFIG_ENUM, which
 * contains all application configuration. At compilation this data is added to the hex
 * file, which is then written to the PIC's internal EEPROM at programming. Some
 * programmers will have the options during programming not to program the PIC's internal
 * EEPROM memory. It is important to enable the programming of the internal EEPROM to
 * have this default device configuration written to the EEPROM.
 *********************************************************************/

#ifndef APPCFG_H
#define APPCFG_H

#include "mac.h"

/////////////////////////////////////////////////
//CPU configuration stuff

#define BAUD_300        0
#define BAUD_1200       1
#define BAUD_2400       2
#define BAUD_4800       3
#define BAUD_9600       4
#define BAUD_19200      5
#define BAUD_38400      6
#define BAUD_57600      7
#define BAUD_115200     8
#define BAUD_230400     9
#define BAUD_460800     10
#define BAUD_921600     11

#define APPCFG_USART_ENABLE    0x01


/**
 * Structure for storing application configuration data that needs fast access.
 * This structure is created in the processors internal RAM, and is initialized with
 * the contents of the corresponding bytes in EEPROM at startup. Whenever the application
 * needs to access any of these variables, it will read them straight from this RAM structure
 * in stead of reading them from the EEPROM.
 */
typedef struct _APP_CONFIG
{
    MAC_ADDR    MyMACAddr;        //6 bytes
} APP_CONFIG;

 
 
 //@{
/**
 * Mask for bits in APPCFG_SYSFLAGS config byte
 */
#define APPCFG_SYSFLAGS_PLLON   0x01
#define APPCFG_SYSFLAGS_BLINKB6 0x02
//@}

/**
 * Define this if APP_CONFIG is smaller then 256 bytes. When this is defined
 * the code knows that it only need 1 byte to address all configuration bytes. This
 * produces smaller and faster code.
 */
#define APPCFG_SMALLER_256


/**
 * Main application config structure. This structure lives in the internal
 * EEPROM, starting at address 0.<br>
 * <b>!!! IMPORTANT - Only add to end the of this structure!!!</b><br>
 * This structure can NOT get larger then 256 bytes. If it does, some code will have to be
 * changed!
 */
enum APP_CONFIG_ENUM {
	
    APPCFG_BOOT_FLAG = 0,

    /**< Our MAC address. First entry is MSB byte of MAC address */
    APPCFG_MAC0,                    
    APPCFG_MAC1,                   
    APPCFG_MAC2,                 
    APPCFG_MAC3,                  
    APPCFG_MAC4,                   
    APPCFG_MAC5,                 
    
    /**< System flags
     *  - Bit 0: 4 x PLL enable flag
     *  - Bit 1: Blink led on B6
     */
    APPCFG_SYSFLAGS,                ///< Offset = 21 = 0x15

    /**< USART1 Configuration. Baud rate will be a BAUD_XXX define */
    APPCFG_USART1_BAUD,             ///< Offset = 31 = 0x1F
    APPCFG_USART1_CFG,              ///< Offset = 32 = 0x20

    /**< Port direction configuration */
    APPCFG_TRISA,                   ///< Offset = 35 = 0x23
    APPCFG_TRISB,                   ///< Offset = 36 = 0x24
    APPCFG_TRISC,                   ///< Offset = 37 = 0x25
    APPCFG_TRISD,                   ///< Offset = 38 = 0x26
    APPCFG_TRISE,                   ///< Offset = 39 = 0x27
    APPCFG_TRISF,                   ///< Offset = 40 = 0x28
    APPCFG_TRISG,                   ///< Offset = 41 = 0x29

    /**< Port default values */
    APPCFG_PORTA,                   ///< Offset = 42 = 0x2A
    APPCFG_PORTB,                   ///< Offset = 43 = 0x2B
    APPCFG_PORTC,                   ///< Offset = 44 = 0x2C
    APPCFG_PORTD,                   ///< Offset = 45 = 0x2D
    APPCFG_PORTE,                   ///< Offset = 46 = 0x2E
    APPCFG_PORTF,                   ///< Offset = 47 = 0x2F
    APPCFG_PORTG,                   ///< Offset = 48 = 0x30

    /**< ADC configuration */
    APPCFG_ADCON01,                  ///< Offset = 49 = 0x31
    APPCFG_ADCON02,                  ///< Offset = 50 = 0x32

    /**< Serial number */
    APPCFG_SERIAL_NUMBER0,          ///< Offset = 51 = 0x33
    APPCFG_SERIAL_NUMBER1,          ///< Offset = 52 = 0x34
    
        /**< <hr><b>PWM Channel 1-8 Enable</b> Register.
     * 0=off, 1=on. All Off by default. The LSB (bit 0) controls PWM Channel 1.
     */
    APPCFG_PWM_ENABLE,              ///< Offset = 99 = 0x63
    
    
    /**< <hr>
     *  <table border="0" cellpadding="0" cellspacing="0" width="100%">
     *  <tr><td><b>PWM Frequency</b> for all channels. The frequency is different for 8 bit 10 bit mode! </td></tr>
     *  <tr><td><i>Frequencies when in 10 bit mode:</i></td></tr>
     *  <tr><td class="ind1">- 0 = 2.441 kHz</td></tr>
     *  <tr><td class="ind1">- 1 = 9.766 kHz</td></tr>
     *  <tr><td class="ind1">- 2 and 3 = 39.062 kHz</td></tr>
     *  <tr><td><i>Frequencies when in 8 bit mode:</i></td></tr>
     *  <tr><td class="ind1">- 0 and 1 = 9.766 kHz</td></tr>
     *  <tr><td class="ind1">- 2 = 39.062 kHz</td></tr>
     *  <tr><td class="ind1">- 3 = 156.25 kHz</td></tr>
     *  </table>
     */
    APPCFG_PWM_FREQ,                ///< Offset = 100 = 0x64
    
    /**< <hr><b>PWM Flags:</b>
     *  - Bit 0: Configures for 8 or 10 bit PWM. 0 = 8 bit, 1 = 10 bit.
     */
    APPCFG_PWM_FLAGS,               ///< Offset = 101 = 0x65

    /**< <hr><b>PWM Channel 1</b> (C1) default value (10 bit value) */
    APPCFG_PWM_1L,                  ///< Offset = 102 = 0x66
    APPCFG_PWM_1H,                  ///< Offset = 103 = 0x67

    /**< <hr><b>PWM Channel 2</b> (C2) default value (10 bit value) */
    APPCFG_PWM_2L,                  ///< Offset = 104 = 0x68
    APPCFG_PWM_2H,                  ///< Offset = 105 = 0x69

    /**< <hr><b>PWM Channel 3</b> (G0) default value (10 bit value) */
    APPCFG_PWM_3L,                  ///< Offset = 106 = 0x6A
    APPCFG_PWM_3H,                  ///< Offset = 107 = 0x6B

    /**< <hr><b>PWM Channel 4</b> (G3) default value (10 bit value) */
    APPCFG_PWM_4L,                  ///< Offset = 108 = 0x6C
    APPCFG_PWM_4H,                  ///< Offset = 109 = 0x6D

    /**< <hr><b>PWM Channel 5</b> (G4) default value (10 bit value) */
    APPCFG_PWM_5L,                  ///< Offset = 110 = 0x6E
    APPCFG_PWM_5H,                  ///< Offset = 111 = 0x6F
 
    
    APPCFG_VSCP_EEPROM_CONTROL,
    APPCFG_VSCP_EEPROM_REG_USERID,
    APPCFG_VSCP_EEPROM_REG_USERID1,
    APPCFG_VSCP_EEPROM_REG_USERID2,
    APPCFG_VSCP_EEPROM_REG_USERID3,
    APPCFG_VSCP_EEPROM_REG_USERID4,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID0,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID1,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID2,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID3,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID0,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID1,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID2,
    APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID,
    APPCFG_VSCP_EEPROM_REG_MODULE_ZONE,
    APPCFG_VSCP_EEPROM_REG_MODULE_SUBZONE,
    APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL,
    APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL,
    APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL,
    APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_LOW,
    APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_HIGH,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL0,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL2,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL3,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL4,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL5,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL6,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL7,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL0,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL1,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL2,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL3,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL4,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL5,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL6,
    APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL7,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME0_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME0_LSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME1_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME1_LSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME2_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME2_LSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME3_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME3_LSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME4_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME4_LSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME5_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME5_LSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME6_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME6_LSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME7_MSB,
    APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME7_LSB,
    APPCFG_VSCP_EEPROM_REG_PWM_MSB,
    APPCFG_VSCP_EEPROM_REG_PWM_LSB,
    APPCFG_VSCP_EEPROM_REG_RESERVED1,
    APPCFG_VSCP_EEPROM_REG_SERIAL_CONTROL,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN0_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN0_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN1_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN1_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN2_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN2_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN3_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN3_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN4_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN4_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN5_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN5_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN6_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN6_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN7_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MIN7_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX0_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX0_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX1_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX1_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX2_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX2_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX3_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX3_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX4_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX4_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX5_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX5_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX6_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX6_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX7_MSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_MAX7_LSB,
    APPCFG_VSCP_EEPROM_REG_ANALOG_HYSTERESIS,
    APPCFG_VSCP_EEPROM_REG_CAN_CONTROL,             // CAN control setup
    APPCFG_VSCP_EEPROM_REG_CAN_CLASS_MASK_MSB,      // Class mask for CAN interface
    APPCFG_VSCP_EEPROM_REG_CAN_CLASS_MASK_LSB,
    APPCFG_VSCP_EEPROM_REG_CAN_TYPE_MASK_MSB,       // Type mask for CAN interface
    APPCFG_VSCP_EEPROM_REG_CAN_TYPE_MASK_LSB,
    APPCFG_VSCP_EEPROM_REG_CAN_CLASS_FILTER_MSB,    // Class filter for CAN interface
    APPCFG_VSCP_EEPROM_REG_CAN_CLASS_FILTER_LSB,
    APPCFG_VSCP_EEPROM_REG_CAN_TYPE_FILTER_MSB,     // Type filter for CAN interface
    APPCFG_VSCP_EEPROM_REG_CAN_TYPE_FILTER_LSB,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID0,           // GUID for CAN interface
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID1,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID2,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID3,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID4,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID5,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID6,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID7,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID8,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID9,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID10,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID11,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID12,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID13,
    APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID14
};
 


/**
 * Second application config structure. Contains configuration for SPI, I2C, USART, CAN,
 * USB serial buses. This structure lives in the internal EEPROM, starting at address 256.
 * <br><b>!!! IMPORTANT - Only add to end the of this structure!!!</b><br>
 * This structure can NOT get larger then 256 bytes. If it does, some code will have to be
 * changed!
 */
enum BUS_CONFIG_ENUM {

    BUSCFG_SER1_TXBUFSIZE=256,          ///< <b>Serial Bus 1 Tx Buffer size</b> Offset = 0 = 0x00

    /**< <hr> <b>Serial Bus 1 Rx Buffer size</b> */
    BUSCFG_SER1_RXBUFSIZE,              ///< Offset = 1 = 0x01

    /**< <hr> <b>Serial Bus 2 Tx Buffer size</b> */
    BUSCFG_SER2_TXBUFSIZE,              ///< Offset = 2 = 0x02
    /**< <hr> <b>Serial Bus 2 Rx Buffer size</b> */
    BUSCFG_SER2_RXBUFSIZE,              ///< Offset = 3 = 0x03

    /**< <hr> <b>I2C Bus 1 Tx Buffer size</b> */
    BUSCFG_I2C1_TXBUFSIZE,              ///< Offset = 4 = 0x04
    /**< <hr> <b>I2C Bus 1 Rx Buffer size</b> */
    BUSCFG_I2C1_RXBUFSIZE,              ///< Offset = 5 = 0x05

    /**< <hr> <b>SPI Bus 1 Tx Buffer size</b> */
    BUSCFG_SPI1_TXBUFSIZE,              ///< Offset = 6 = 0x06
    /**< <hr> <b>SPI Bus 1 Rx Buffer size</b> */
    BUSCFG_SPI1_RXBUFSIZE,              ///< Offset = 7 = 0x07

    /**< <hr> <b>Reserved for Serial Bus 5 to 8</b> */
    BUSCFG_BUS5_TXBUFSIZE,              ///< Offset = 8  = 0x08
    BUSCFG_BUS5_RXBUFSIZE,              ///< Offset = 9  = 0x09
    BUSCFG_BUS6_TXBUFSIZE,              ///< Offset = 10 = 0x0A
    BUSCFG_BUS6_RXBUFSIZE,              ///< Offset = 11 = 0x0B
    BUSCFG_BUS7_TXBUFSIZE,              ///< Offset = 12 = 0x0C
    BUSCFG_BUS7_RXBUFSIZE,              ///< Offset = 13 = 0x0D
    BUSCFG_BUS8_TXBUFSIZE,              ///< Offset = 14 = 0x0E
    BUSCFG_BUS8_RXBUFSIZE,              ///< Offset = 15 = 0x0F

    /**< <hr> <b>UDP 1 Port</b> */
    BUSCFG_UDP1_PORT0,                  ///< Offset = 16 = 0x10
    BUSCFG_UDP1_PORT1,                  ///< Offset = 17 = 0x11

    /**< <hr><b>UDP Port 1 Flags</b>
     *  - Bit 0: Enable UDP Port 1. When set, UDP Port 1 is enabled.
     */
    BUSCFG_UDP1_CFG,                    ///< Offset = 18 = 0x12

    /**< <hr><b>UDP 2 Port</b> */
    BUSCFG_UDP2_PORT0,                  ///< Offset = 19 = 0x13
    BUSCFG_UDP2_PORT1,                  ///< Offset = 20 = 0x14

    /**< <hr><b>UDP Port 2 Flags</b>
     *  - Bit 0: Enable UDP Port 2. When set, UDP Port 2 is enabled.
     */
    BUSCFG_UDP2_CFG,                    ///< Offset = 21 = 0x15

    /**< <hr><b>BUS that UDP Port 1 is linked to</b>, 0xff is non */
    BUSCFG_UDP1_LINK,                   ///< Offset = 22 = 0x16

    /**< <hr><b>BUS that UDP Port 2 is linked to</b>, 0xff is non */
    BUSCFG_UDP2_LINK,                   ///< Offset = 23 = 0x17

    /**< <hr><b>Software SPI buses enabled</b>. Bits 0 to 7 indicate which software SPI buses have
    been enabled. */
    BUSCFG_SWSPI_ENABLED,               ///< Offset = 24 = 0x18

    /**< <hr><b>Software SPI 1 CLK pin</b>. Bits 4-7 point to port (0=a, 1=b..), and bits 0-3 the bit */
    BUSCFG_SWSPI1_CLKPIN,               ///< Offset = 25 = 0x19
    /**< <hr><b>Software SPI 1 SDI pin</b>. Bits 4-7 point to port (0=a, 1=b..), and bits 0-3 the bit */
    BUSCFG_SWSPI1_SDIPIN,               ///< Offset = 26 = 0x1A
    /**< <hr><b>Software SPI 1 SDO pin</b>. Bits 4-7 point to port (0=a, 1=b..), and bits 0-3 the bit */
    BUSCFG_SWSPI1_SDOPIN,               ///< Offset = 27 = 0x1B
    /**< <hr><b>Software SPI 1 CS pin</b>. Bits 4-7 point to port (0=a, 1=b..), and bits 0-3 the bit */
    BUSCFG_SWSPI1_CSPIN,                ///< Offset = 28 = 0x1C
    /**< <hr><b>Software SPI 1 config</b>.
    - Bits 0-1 = SPI Mode<br>
        00 = Mode 0<br>
        01 = Mode 1<br>
        10 = Mode 2<br>
        11 = Mode 3
    - Bits 4-7 = Max clock speed<br>
        0 = Maximum clock speed, no delays<br>
        1 = About 500kbit / second maximum speed<br>
        2 = About 250kbit / second maximum speed<br>
        3 = About 125kbit / second maximum speed<br>
        4 = About 62kbit / second maximum speed<br>
        5 = About 31kbit / second maximum speed<br>
        6 = About 15kbit / second maximum speed<br>
        7 = About 8kbit / second maximum speed<br>
        8 = About 4kbit / second maximum speed<br>
        9 = About 2kbit / second maximum speed<br>
    */
    BUSCFG_SWSPI1_CONF,             ///< Offset = 29 = 0x1D
};

#define BUSCFG_BUS1_TXBUFSIZE BUSCFG_SER1_TXBUFSIZE

/////////////////////////////////////////////////
//Global variables

//This is used by other stack elements
#ifndef THIS_IS_APPCFG_MODULE

extern APP_CONFIG AppConfig;

#endif


/**
 * @preCondition  fsysInit() is already called.
 */
void appcfgInit(void);

/**
 * Save Application Configuartion data to EEPROM
 */
void appcfgSave(void);

/**
 * Configure USART with AppConfig data
 */
void appcfgUSART(void);

/**
 * Configure USART2 with AppConfig data
 */
void appcfgUSART2(void);

/**
 * Configure the CPU I/O ports
 */
void appcfgCpuIO(void);

/**
 * Configure the CPU I/O ports to have the configured default values
 */
void appcfgCpuIOValues(void);

/**
 * Configure the ADC unit
 */
void appcfgADC(void);

/*
 * Configure the PWM ports with default power up values
 */
void appcfgPWM(void);

/**
 * Configure the Expansion Board
 */
void appcfgXboard(void);

/**
 * Sets the PWM frequency to the given value
 *
 * @param freq New frequency, a value from 0-3.
 *             For 8 bit mode:  0 and 1=9.766kHz, 2=39.062kHz, 3=156.25kHz
 *             For 10 bit mode: 0=2.441kHz, 1=9.766kHz, 2 and 3=39.062kHz
 * @param remember If 1, store in EEPROM and remember
 */
void pwmSetFreq(BYTE freq, BOOL remember);

/**
 * Gets a constant value representing the current PWM frequency
 *
 * @return Frequency constant, a value from 0-3.
 *          For 8 bit mode:  1=9.766kHz, 2=39.062kHz, 3=156.25kHz
 *          For 10 bit mode: 0=2.441kHz, 1=9.766kHz, 2=39.062kHz
 */
BYTE pwmGetFreq(void);

/**
 * Return TRUE if the PWM is currently in 10-bit mode
 */
#define pwmIs10BitMode() (PR2 == 255)

/**
 * Return TRUE if the PWM is currently in 10-bit mode
 */
#define pwmIs8BitMode() (PR2 == 63)


/**
 * Set the given PWM channel to the given value
 *
 * @param ch PWM channnel, is a value from 1 to 5
 * @param val The value to set the channel.
 *            - For 8 bit mode, is a value from 0-255
 *            - For 10 bit mode, is a value from 0-1023
 * @param remember If 1, store in EEPROM and remember
 *
 */
void pwmSetValue(BYTE ch, WORD_VAL val, BOOL remember);


/**
 * Get the value of the given PWM channel
 *
 * @param ch PWM channnel, is a value from 1 to 5
 *
 * @return The given PWM channels value
 *
 */
WORD pwmGetValue(BYTE ch);


/**
 * Enables/Disables given PWM channels
 *
 * @param chan Bits 0-4 indicate if PWM channels 1-5 must be enabled or disabled
 * @param remember If 1, store in EEPROM and remember
 */
void pwmEnable(BYTE chan, BOOL remember);

/**
 * Get a specified byte from the APP_CONFIG configuration structure
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get. Use
 *        the APPCFG_X defines
 *
 * @return Returns the requested byte
 */
BYTE appcfgGetc(WORD offset);

/**
 * Gets a specified byte from the APP_CONFIG configuration structure
 *
 * @param field The byte to get from the APP_CONFIG structure. For example "MyMask.v[0]"
 *
 * @return Returns the requested byte
 */
#define appcfgGetcAtField(field) appcfgGetc(offsetof(APP_CONFIG, field))

/**
 * Set a specified byte from the APP_CONFIG configuration structure to a given value.
 *
 * @param field The byte to set in the APP_CONFIG structure. For example "MyMask.v[0]"
 * @param value Contains the value to set the given byte to
 *
 * @return Returns the requested byte
 */
#define appcfgPutcAtField(field, value) EEDATA = (value); _appcfgPutc(offsetof(APP_CONFIG, field))

/**
 * Set a specified byte from the APP_CONFIG configuration structure with the data contained
 * in the EEDATA byte.
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get
 *
 * @return Returns the requested byte
 */
void _appcfgPutc(WORD offset);

/**
 * Set a specified byte in the APP_CONFIG configuration structure to a given value.
 * This function will only update the EEPROM if the given new value is different from the
 * current old value. This saves the EEPROM, and increases it's live!
 *
 * @param offset The byte to set in the APP_CONFIG structure. Use the APPCFG_X defines
 * @param value Contains the value to set the given byte to
 *
 * @return Returns the requested byte
 */
#define appcfgPutc(offset, value) EEDATA = (value); _appcfgPutc(offset)



#endif
