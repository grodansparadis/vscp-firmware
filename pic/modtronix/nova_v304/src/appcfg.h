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
 //*******************************************************
 //----------------- Appcfg Configuration -----------------
 //********************************************************


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
 *    - Initial version
 *********************************************************************/


/////////////////////////////////////////////////
//  Application Configuration - documentation module
 /**
 * @defgroup mod_conf_appconfig Application Configuration
 * @ingroup mod_conf
 * The application configuration is stored in the PIC microcontroller's internal
 * EEPROM in the first 256 bytes of data. The structure of the data is defined
 * in the @ref APP_CONFIG_ENUM enumeration.<br>
 * The device's default configuration is set in the eeconfigArray[] array in the
 * appcfg.c file. This array will define the values for the entire APP_CONFIG_ENUM, which
 * contains all application configuration. At compilation this data is added to the hex
 * file, which is then written to the PIC's internal EEPROM at programming. Some
 * programmers will have the options during programming not to program the PIC's internal
 * EEPROM memory. It is important to enable the programming of the internal EEPROM to
 * have this default device configuration written to the EEPROM.
 *********************************************************************/

#ifndef APPCFG_H
#define APPCFG_H

#include "net\ip.h"


/////////////////////////////////////////////////
//Do some checks on defines
#if ((ADC_CHANNELS < 0) || (ADC_CHANNELS > 12))
#error "ADC_CHANNELS has been assigned an illegal value!"
#endif

#if (ADC_CHANNELS > 0)
    #if !defined(APP_USE_ADC8) && !defined(APP_USE_ADC10)
    #define APP_USE_ADC8
    #endif
#endif

#if (ADC_CHANNELS > 0)
    #if defined(APP_USE_ADC8) && defined(APP_USE_ADC10)
    #error "APP_USE_ADC8 and APP_USE_ADC10 can not both be defined!"
    #endif
#endif

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

/** Structure for storing USART configuration data. Data is stored in internal EEPROM */
typedef struct _USART_CFG
{
    BYTE        baud;
    BYTE        cfg;
} USART_CFG;    //2 bytes long

/** Structure for storing ADC configuration data. Data is stored in internal EEPROM */
typedef struct _ADC_CFG
{
    BYTE        adcon1;
    BYTE        adcon2;
} ADC_CFG;    //2 bytes long

/** Structure for storing port configuration data. Data is stored in internal EEPROM */
typedef struct _PORT_CFG
{
    /** This is a test */
    BYTE        trisa;          //NB!!! They must follow each other in structure!!!
    BYTE        trisb;          
    BYTE        trisc;
    BYTE        trisd;
    BYTE        trise;
    BYTE        trisf;
    BYTE        trisg;
} PORT_CFG;    //7 bytes long

/** Structure for storing default port value. Data is stored in internal EEPROM */
typedef struct _PORT_VALUES     //Port startup values
{
    BYTE        porta;          //NB!!! They must follow each other in structure!!!
    BYTE        portb;
    BYTE        portc;
    BYTE        portd;
    BYTE        porte;
    BYTE        portf;
    BYTE        portg;
} PORT_VALUE;    //7 bytes long

//Define maximum username length. This is the lengh of the characters + NULL terminating char. Thus, a value
//of 9 = 8 char string + NULL
#define APPCFG_USERNAME_LEN 9

/**
 * Structure for storing application configuration data that needs fast access.
 * This structure is created in the processors internal RAM, and is initialized with
 * the contents of the corresponding bytes in EEPROM at startup. Whenever the application
 * needs to access any of these variables, it will read them straight from this RAM structure
 * in stead of reading them from the EEPROM.
 */
typedef struct _APP_CONFIG
{
    IP_ADDR     MyIPAddr;		//4 bytes
    MAC_ADDR    MyMACAddr;		//6 bytes
    IP_ADDR     MyMask;			//4 bytes
    IP_ADDR     MyGateway;		//4 bytes
     
} APP_CONFIG;

/**
 * Mask for bits in APPCFG_NETFLAGS config byte.
 */
#define APPCFG_NETFLAGS_DHCP    0x01

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
    APPCFG_IP0 = 0,                 ///< Offset = 0 = 0x00. Our IP address. First entry is MSB byte of IP address, for example in "192.168.1.0" it will be 192
    APPCFG_IP1,                     ///< Offset = 1 = 0x01
    APPCFG_IP2,                     ///< Offset = 2 = 0x02
    APPCFG_IP3,                     ///< Offset = 3 = 0x03

    /**< Our MAC address. First entry is MSB byte of MAC address */
    APPCFG_MAC0,                    ///< Offset = 4 = 0x04
    APPCFG_MAC1,                    ///< Offset = 5 = 0x05
    APPCFG_MAC2,                    ///< Offset = 6 = 0x06
    APPCFG_MAC3,                    ///< Offset = 7 = 0x07
    APPCFG_MAC4,                    ///< Offset = 8 = 0x08
    APPCFG_MAC5,                    ///< Offset = 9 = 0x09

    /**< Our Mask. First entry is MSB of MASK address, for example if mask is 255.0.0.0 it will be 255 */
    APPCFG_MASK0,                   ///< Offset = 10 = 0x0A
    APPCFG_MASK1,                   ///< Offset = 11 = 0x0B
    APPCFG_MASK2,                   ///< Offset = 12 = 0x0C
    APPCFG_MASK3,                   ///< Offset = 13 = 0x0D

    /**< Our Gateway address. First entry is MSB byte of IP address, for example in "192.168.1.254" it will be 192 */
    APPCFG_GATEWAY0,                ///< Offset = 14 = 0x0E
    APPCFG_GATEWAY1,                ///< Offset = 15 = 0x0F
    APPCFG_GATEWAY2,                ///< Offset = 16 = 0x10
    APPCFG_GATEWAY3,                ///< Offset = 17 = 0x11

    /**< Delay how long we wait for the bootloader, value from 0-n. Time = 0.8sec x n */
    APPCFG_WAIT4BL,                 ///< Offset = 18 = 0x12

    /**< If 0, the FLASH has not been programmed yet, or does NOT contain a valid program. In this case we can
     *  not exit bootloader mode! */
    APPCFG_FLASH_HAS_PROGRAM,       ///< Offset = 19 = 0x13

    /**< Time to delay (n x 50ms) on startup and give chance to press key and enter serial config. If 0, skip */
    APPCFG_STARTUP_SER_DLY,         ///< Offset = 20 = 0x14

    /**< System flags
     *  - Bit 0: 4 x PLL enable flag
     *  - Bit 1: Blink led on B6
     */
    APPCFG_SYSFLAGS,                ///< Offset = 21 = 0x15

    /**<
     * Network flags.
     * - Bit 0: DHCP enable flag
     */
    APPCFG_NETFLAGS,                ///< Offset = 22 = 0x16

    /**< SMTP Server address */
    APPCFG_SMTP_SRVR_ADR0,          ///< Offset = 23 = 0x17
    APPCFG_SMTP_SRVR_ADR1,          ///< Offset = 24 = 0x18
    APPCFG_SMTP_SRVR_ADR2,          ///< Offset = 25 = 0x19
    APPCFG_SMTP_SRVR_ADR3,          ///< Offset = 26 = 0x1A

    /**< TFTP Server address */
    APPCFG_TFTP_SRVR_ADR0,          ///< Offset = 27 = 0x1B
    APPCFG_TFTP_SRVR_ADR1,          ///< Offset = 28 = 0x1C
    APPCFG_TFTP_SRVR_ADR2,          ///< Offset = 29 = 0x1D
    APPCFG_TFTP_SRVR_ADR3,          ///< Offset = 30 = 0x1E

    /**< USART1 Configuration. Baud rate will be a BAUD_XXX define */
    APPCFG_USART1_BAUD,             ///< Offset = 31 = 0x1F
    APPCFG_USART1_CFG,              ///< Offset = 32 = 0x20

    /**< USART2 Configuration. Baud rate will be a BAUD_XXX define */
    APPCFG_USART2_BAUD,             ///< Offset = 33 = 0x21
    APPCFG_USART2_CFG,              ///< Offset = 34 = 0x22

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
    APPCFG_ADCON1,                  ///< Offset = 49 = 0x31
    APPCFG_ADCON2,                  ///< Offset = 50 = 0x32

    /**< Serial number */
    APPCFG_SERIAL_NUMBER0,          ///< Offset = 51 = 0x33
    APPCFG_SERIAL_NUMBER1,          ///< Offset = 52 = 0x34

    /**< Username - Is a null terminated string. Max 8 char + null termination char*/
    APPCFG_USERNAME0,               ///< Offset = 53 = 0x35
    APPCFG_USERNAME1,               ///< Offset = 54 = 0x36
    APPCFG_USERNAME2,               ///< Offset = 55 = 0x37
    APPCFG_USERNAME3,               ///< Offset = 56 = 0x38
    APPCFG_USERNAME4,               ///< Offset = 57 = 0x39
    APPCFG_USERNAME5,               ///< Offset = 58 = 0x3A
    APPCFG_USERNAME6,               ///< Offset = 59 = 0x3B
    APPCFG_USERNAME7,               ///< Offset = 60 = 0x3C
    APPCFG_USERNAME8,               ///< Offset = 61 = 0x3D

    /**< Password - Is a null terminated string. Max 8 char + null termination char*/
    APPCFG_PASSWORD0,               ///< Offset = 62 = 0x3E
    APPCFG_PASSWORD1,               ///< Offset = 63 = 0x3F
    APPCFG_PASSWORD2,               ///< Offset = 64 = 0x40
    APPCFG_PASSWORD3,               ///< Offset = 65 = 0x41
    APPCFG_PASSWORD4,               ///< Offset = 66 = 0x42
    APPCFG_PASSWORD5,               ///< Offset = 67 = 0x43
    APPCFG_PASSWORD6,               ///< Offset = 68 = 0x44
    APPCFG_PASSWORD7,               ///< Offset = 69 = 0x45
    APPCFG_PASSWORD8,               ///< Offset = 70 = 0x46
    
    /**< VSCP TCP Server - First entry is MSB byte of IP address, for example in "192.168.1.6" it will be 192 */
    APPCFG_VSCP_SERVER_IP0,         ///< Offset = 71 = 0x47. 
    APPCFG_VSCP_SERVER_IP1,         ///< Offset = 72 = 0x48
    APPCFG_VSCP_SERVER_IP2,         ///< Offset = 73 = 0x49
    APPCFG_VSCP_SERVER_IP3,         ///< Offset = 74 = 0x4A
    
    // VSCP Stack function flags
    APPCFG_VSCP_SERVER_FLAGS,       ///< Offset = 75 = 0x4B
    
    /**< VSCP TCP Auto Discovered Server 0 */
    APPCFG_VSCP_SERVER_AUTO0_IP0,	///< Offset = 76 = 0x4C. 
    APPCFG_VSCP_SERVER_AUTO0_IP1,   ///< Offset = 77 = 0x4D
    APPCFG_VSCP_SERVER_AUTO0_IP2,   ///< Offset = 78 = 0x4E
    APPCFG_VSCP_SERVER_AUTO0_IP3,   ///< Offset = 79 = 0x4F
    
    /**< VSCP TCP Auto Discovered Server 1 */
    APPCFG_VSCP_SERVER_AUTO1_IP0,	///< Offset = 80 = 0x50 
    APPCFG_VSCP_SERVER_AUTO1_IP1,   ///< Offset = 81 = 0x51
    APPCFG_VSCP_SERVER_AUTO1_IP2,   ///< Offset = 82 = 0x52
    APPCFG_VSCP_SERVER_AUTO1_IP3,   ///< Offset = 83 = 0x53
    
    /**< VSCP TCP Auto Discovered Server 2 */
    APPCFG_VSCP_SERVER_AUTO2_IP0,	///< Offset = 84 = 0x54 
    APPCFG_VSCP_SERVER_AUTO2_IP1,   ///< Offset = 85 = 0x55
    APPCFG_VSCP_SERVER_AUTO2_IP2,   ///< Offset = 86 = 0x56
    APPCFG_VSCP_SERVER_AUTO2_IP3,   ///< Offset = 87 = 0x57
    
    /**< VSCP TCP Auto Discovered Server 0 port */
    APPCFG_VSCP_SERVER_AUTO0_PORT_MSB,	///< Offset = 88 = 0x58 
    APPCFG_VSCP_SERVER_AUTO0_PORT_LSB, 	///< Offset = 89 = 0x59
    
    /**< VSCP TCP Auto Discovered Server 1 port */
    APPCFG_VSCP_SERVER_AUTO1_PORT_MSB,	///< Offset = 90 = 0x5A 
    APPCFG_VSCP_SERVER_AUTO1_PORT_LSB, 	///< Offset = 91 = 0x5B
    
    /**< VSCP TCP Auto Discovered Server 2 port */
    APPCFG_VSCP_SERVER_AUTO2_PORT_MSB,	///< Offset = 92 = 0x5C 
    APPCFG_VSCP_SERVER_AUTO2_PORT_LSB, 	///< Offset = 93 = 0x5D
    
    /**< Number of found VSCP TCP Auto Discovered Servers */
    APPCFG_VSCP_CNT_SERVER_AUTO,		///< Offset = 94 = 0x5E
    
    APPCFG_VSCP_EEPROM_CONTROL,							///< Offset = 95 = 0x5F
	APPCFG_VSCP_EEPROM_REG_USERID,						///< Offset = 96 = 0x60
	APPCFG_VSCP_EEPROM_REG_USERID1,						///< Offset = 97 = 0x61
	APPCFG_VSCP_EEPROM_REG_USERID2,						///< Offset = 98 = 0x62
	APPCFG_VSCP_EEPROM_REG_USERID3,						///< Offset = 99 = 0x63
	APPCFG_VSCP_EEPROM_REG_USERID4,						///< Offset = 100 = 0x64
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID0,				///< Offset = 101 = 0x65
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID1,				///< Offset = 102 = 0x66
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID2,				///< Offset = 103 = 0x67
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_ID3,				///< Offset = 104 = 0x68
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID0,			///< Offset = 105 = 0x69
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID1,			///< Offset = 106 = 0x6A
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID2,			///< Offset = 107 = 0x6B
	APPCFG_VSCP_EEPROM_REG_MANUFACTUR_SUBID,			///< Offset = 108 = 0x6C
	APPCFG_VSCP_EEPROM_REG_MODULE_ZONE,					///< Offset = 109 = 0x6D
	APPCFG_VSCP_EEPROM_REG_MODULE_SUBZONE,				///< Offset = 110 = 0x6E
	APPCFG_VSCP_EEPROM_REG_MODULE_CONTROL,				///< Offset = 111 = 0x6F
	APPCFG_VSCP_EEPROM_REG_EVENT_INTERVAL,				///< Offset = 112 = 0x70
	APPCFG_VSCP_EEPROM_REG_ADCTRL_EVENT_PERIODCTRL,		///< Offset = 113 = 0x71
	APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_LOW,			///< Offset = 114 = 0x72
	APPCFG_VSCP_EEPROM_REG_ADCTRL_ALARM_HIGH,			///< Offset = 115 = 0x73
	APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL0,				///< Offset = 116 = 0x74
	APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL2,				///< Offset = 117 = 0x75
	APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL3,				///< Offset = 118 = 0x76
	APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL4,				///< Offset = 119 = 0x77
	APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL5,				///< Offset = 120 = 0x78
	APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL6,				///< Offset = 121 = 0x79
	APPCFG_VSCP_EEPROM_REG_OUTPUT_CONTROL7,				///< Offset = 122 = 0x7A
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL0,				///< Offset = 123 = 0x7B
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL1,				///< Offset = 124 = 0x7C
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL2,				///< Offset = 125 = 0x7D
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL3,				///< Offset = 126 = 0x7E
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL4,				///< Offset = 127 = 0x7F
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL5,				///< Offset = 128 = 0x80
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL6,				///< Offset = 129 = 0x81
	APPCFG_VSCP_EEPROM_REG_INPUT_CONTROL7,				///< Offset = 130 = 0x82
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME0_MSB,	///< Offset = 131 = 0x83
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME0_LSB,	///< Offset = 132 = 0x84
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME1_MSB,	///< Offset = 133 = 0x85
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME1_LSB,	///< Offset = 134 = 0x86
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME2_MSB,	///< Offset = 135 = 0x87
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME2_LSB,	///< Offset = 136 = 0x88
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME3_MSB,	///< Offset = 137 = 0x89
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME3_LSB,	///< Offset = 138 = 0x8A
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME4_MSB,	///< Offset = 139 = 0x8B
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME4_LSB,	///< Offset = 140 = 0x8C
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME5_MSB,	///< Offset = 141 = 0x8D
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME5_LSB,	///< Offset = 142 = 0x8E
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME6_MSB,	///< Offset = 143 = 0x8F
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME6_LSB,	///< Offset = 144 = 0x90
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME7_MSB,	///< Offset = 145 = 0x91
	APPCFG_VSCP_EEPROM_REG_OUTPUT_PROTECTION_TIME7_LSB,	///< Offset = 146 = 0x92
	APPCFG_VSCP_EEPROM_REG_PWM_MSB,						///< Offset = 147 = 0x93
	APPCFG_VSCP_EEPROM_REG_PWM_LSB,						///< Offset = 148 = 0x94
	APPCFG_VSCP_EEPROM_REG_RESERVED1,					///< Offset = 149 = 0x95
	APPCFG_VSCP_EEPROM_REG_SERIAL_CONTROL,				///< Offset = 150 = 0x96
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN0_MSB,				///< Offset = 151 = 0x97
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN0_LSB,				///< Offset = 152 = 0x98
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN1_MSB,				///< Offset = 153 = 0x99
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN1_LSB,				///< Offset = 154 = 0x9A
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN2_MSB,				///< Offset = 155 = 0x9B
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN2_LSB,				///< Offset = 156 = 0x9C
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN3_MSB,				///< Offset = 157 = 0x9D
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN3_LSB,				///< Offset = 158 = 0x9E
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN4_MSB,				///< Offset = 159 = 0x9F
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN4_LSB,				///< Offset = 160 = 0xA0
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN5_MSB,				///< Offset = 161 = 0xA1
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN5_LSB,				///< Offset = 162 = 0xA2
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN6_MSB,				///< Offset = 163 = 0xA3
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN6_LSB,				///< Offset = 164 = 0xA4
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN7_MSB,				///< Offset = 165 = 0xA5
	APPCFG_VSCP_EEPROM_REG_ANALOG_MIN7_LSB,				///< Offset = 166 = 0xA6
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX0_MSB,				///< Offset = 167 = 0xA7
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX0_LSB,				///< Offset = 168 = 0xA8
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX1_MSB,				///< Offset = 169 = 0xA9
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX1_LSB,				///< Offset = 170 = 0xAA
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX2_MSB,				///< Offset = 171 = 0xAB
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX2_LSB,				///< Offset = 172 = 0xAC
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX3_MSB,				///< Offset = 173 = 0xAD
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX3_LSB,				///< Offset = 174 = 0xAE
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX4_MSB,				///< Offset = 175 = 0xAF
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX4_LSB,				///< Offset = 176 = 0xB0
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX5_MSB,				///< Offset = 177 = 0xB1
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX5_LSB,				///< Offset = 178 = 0xB2
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX6_MSB,				///< Offset = 179 = 0xB3
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX6_LSB,				///< Offset = 180 = 0xB4
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX7_MSB,				///< Offset = 181 = 0xB5
	APPCFG_VSCP_EEPROM_REG_ANALOG_MAX7_LSB,				///< Offset = 182 = 0xB6
	APPCFG_VSCP_EEPROM_REG_ANALOG_HYSTERESIS,			///< Offset = 183 = 0xB7
	APPCFG_VSCP_EEPROM_REG_CAN_CONTROL,					///< Offset = 184 = 0xB8
	APPCFG_VSCP_EEPROM_REG_CAN_CLASS_MASK_MSB,			///< Offset = 185 = 0xB9
	APPCFG_VSCP_EEPROM_REG_CAN_CLASS_MASK_LSB,			///< Offset = 186 = 0xBA
	APPCFG_VSCP_EEPROM_REG_CAN_TYPE_MASK_MSB,			///< Offset = 187 = 0xBB
	APPCFG_VSCP_EEPROM_REG_CAN_TYPE_MASK_LSB,			///< Offset = 188 = 0xBC
	APPCFG_VSCP_EEPROM_REG_CAN_CLASS_FILTER_MSB,		///< Offset = 189 = 0xBD
	APPCFG_VSCP_EEPROM_REG_CAN_CLASS_FILTER_LSB,		///< Offset = 190 = 0xBE
	APPCFG_VSCP_EEPROM_REG_CAN_TYPE_FILTER_MSB,			///< Offset = 191 = 0xBF
	APPCFG_VSCP_EEPROM_REG_CAN_TYPE_FILTER_LSB,			///< Offset = 192 = 0xC0
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID0,				///< Offset = 193 = 0xC1
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID1,				///< Offset = 194 = 0xC2
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID2,				///< Offset = 195 = 0xC3
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID3,				///< Offset = 196 = 0xC4
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID4,				///< Offset = 197 = 0xC5
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID5,				///< Offset = 198 = 0xC6
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID6,				///< Offset = 199 = 0xC7
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID7,				///< Offset = 200 = 0xC8
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID8,				///< Offset = 201 = 0xC9
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID9,				///< Offset = 202 = 0xCA
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID10,				///< Offset = 203 = 0xCB
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID11,				///< Offset = 204 = 0xCC
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID12,				///< Offset = 205 = 0xCD
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID13,				///< Offset = 206 = 0xCE
	APPCFG_VSCP_EEPROM_REG_REG_CAN_GUID14,				///< Offset = 207 = 0xDF
};


/////////////////////////////////////////////////
//Global variables

//This is used by other stack elements
#ifndef THIS_IS_APPCFG_MODULE

extern APP_CONFIG AppConfig;

/**
 * Global Array containing converted ADC values. This array can be accessed from
 * anywhere to obtain the current value of the ADC inputs. For example, to get
 * the current value of AN2 ADC, use AdcValues[2]
 */
#if defined(APP_USE_ADC8)
extern BYTE AdcValues[ADC_CHANNELS];
#elif defined(APP_USE_ADC10)
extern WORD AdcValues[ADC_CHANNELS];
#endif

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

/**
 * Configure PWM
 */
void appcfgPWM(void);

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
 *
 * @param offset The byte to set in the APP_CONFIG structure. Use the APPCFG_X defines
 * @param value Contains the value to set the given byte to
 *
 * @return Returns the requested byte
 */
#define appcfgPutc(offset, value) EEDATA = (value); _appcfgPutc(offset)


#endif
