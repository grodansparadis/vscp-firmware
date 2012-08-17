/**
 * @brief           Application Configuration functions
 * @file            appcfg.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.10PL1 or higher
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

#include "net/ip.h"

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
    APPCFG_FLASH_HAS_PROGRAM        ///< Offset = 19 = 0x13
};


#endif
