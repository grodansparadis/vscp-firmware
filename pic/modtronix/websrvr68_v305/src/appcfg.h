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
 //Define if this application has an 8 bit ADC converter, 10 bit ADC converter or none at all.
 //#define APP_USE_ADC8
 //#define APP_USE_ADC10

 //Define buffer space that is reserved for ADC conversion values. For a 8 bit ADC converted, 1 byte
 //is reserved per channel. For an 10 bit ADC, 2 bytes are reserved per channel.
 //#define ADC_CHANNELS 12
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
    IP_ADDR     MyIPAddr;        //4 bytes
    MAC_ADDR    MyMACAddr;        //6 bytes
    IP_ADDR     MyMask;            //4 bytes
    IP_ADDR     MyGateway;        //4 bytes
} APP_CONFIG;

/**
 * Mask for bits in APPCFG_NETFLAGS config byte.
 */
#define APPCFG_NETFLAGS_DHCP    0x01

#if defined(BRD_SBC65EC) || defined(BRD_SBC68EC)
//@{
/**
 * Mask for bits in APPCFG_SYSFLAGS config byte
 */
#define APPCFG_SYSFLAGS_PLLON   0x01
#define APPCFG_SYSFLAGS_BLINKB6 0x02
//@}
#endif

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
    APPCFG_ADCON0_2,                ///< Offset = 50 = 0x32

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

    /**< NetBIOS Name - Is a 16 character name, all in uppercase */
    APPCFG_NETBIOS0,                ///< Offset = 71 = 0x47
    APPCFG_NETBIOS1,                ///< Offset = 72 = 0x48
    APPCFG_NETBIOS2,                ///< Offset = 73 = 0x49
    APPCFG_NETBIOS3,                ///< Offset = 74 = 0x4A
    APPCFG_NETBIOS4,                ///< Offset = 75 = 0x4B
    APPCFG_NETBIOS5,                ///< Offset = 76 = 0x4C
    APPCFG_NETBIOS6,                ///< Offset = 77 = 0x4D
    APPCFG_NETBIOS7,                ///< Offset = 78 = 0x4E
    APPCFG_NETBIOS8,                ///< Offset = 79 = 0x4F
    APPCFG_NETBIOS9,                ///< Offset = 80 = 0x50
    APPCFG_NETBIOS10,               ///< Offset = 81 = 0x51
    APPCFG_NETBIOS11,               ///< Offset = 82 = 0x52
    APPCFG_NETBIOS12,               ///< Offset = 83 = 0x53
    APPCFG_NETBIOS13,               ///< Offset = 84 = 0x54
    APPCFG_NETBIOS14,               ///< Offset = 85 = 0x55
    APPCFG_NETBIOS15,               ///< Offset = 86 = 0x56

    /**< Primary DNS server. First entry is MSB byte of IP address, for example in "192.168.1.0" it will be 192 */
    APPCFG_DNS_IP0,                 ///< Offset = 87 = 0x57
    APPCFG_DNS_IP1,                 ///< Offset = 88 = 0x58
    APPCFG_DNS_IP2,                 ///< Offset = 89 = 0x59
    APPCFG_DNS_IP3                  ///< Offset = 90 = 0x5A
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
 * Get a specified byte from the APP_CONFIG configuration structure
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get. Use
 *        the APPCFG_X defines
 *
 * @return Returns the requested byte
 */
BYTE appcfgGetc(BYTE offset);

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
void _appcfgPutc(BYTE offset);

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
