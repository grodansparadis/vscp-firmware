/**
 * @brief           Project configuration
 * @file            projdefs.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @dependencies    none
 * @compiler        MPLAB C18 v2.10 or higher <br>
 *                  HITECH PICC-18 V8.35PL3 or higher
 * @ingroup         mod_conf_projdefs
 *
 * @section description Description
 **********************************
 *
 * For details on configuring the project, see @ref mod_conf_projdefs "Project Configuration" module!
 *
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
 * 2005-09-01, David Hosken (DH):
 *    - Created documentation for existing code
 *********************************************************************/

 /**
 * @defgroup mod_conf_projdefs Project Configuration
 * @ingroup mod_conf
 * The project configuration is done in the projdefs.h file. It contains a list of defines
 * that configure the project at compilation.<br>
 * It is @b very important not to do any project specific configuration in the TCP/IP
 * stack files (@e net folder), but only in the projdefs.h file.
 * This will simplify updating to newer versions of the Modtronix TCP/IP stack in
 * the future. To update to a newer version of the Modtronix TCP/IP, all that should
 * be done is replacing the files contained in the @e net folder with the newer ones.
 *********************************************************************/

#ifndef _PROJDEFS_H_
#define _PROJDEFS_H_

//Defines
#define NON_MCHP_MAC

// TEST TEST
//The DEMO Mode define is used to place the SBC65EC in demo mode. In this mode, some functions are disabled
//#define DEMO_MODE   //release - Ensure this is commented out!

//Include files
#include "compiler.h"
#include "appcfg.h"

/////////////////////////////////////////////////
//Define fast user process. It can be an external function, or a Macro. When it is an external
//function, an "extern ...." function prototype must also be defined
extern void fastUserProcess(void);
#define FAST_USER_PROCESS() fastUserProcess()

#define APP_VER_MAJOR 	0    	// Number from 1 to 99 
#define APP_VER_MINOR 	1 		// Number from 1 to 99 

#define RSTVECTOR_ADR  	0    	/* Standard - no bootloader */
#define HIVECTOR_ADR  	0x8     /* Standard - no bootloader */
#define LOVECTOR_ADR  	0x18    /* Standard - no bootloader */

/** @addtogroup mod_conf_projdefs
 * @code #define DEBUG_OFF @endcode
 * Configures if debug information is written out onto the serial port. For the production version
 * of this project, this define should NOT be defined.<br>
 * Debug Configuration. When uncommenting any of the following line, remember to uncomment a debug
 * implementation in debug.h. For example, uncommend busser1.h and link busser1.c with project.<br>
 * For details, see @ref mod_sys_debug "Debugging" module.
 */
//Set Debug Log levels to one of the following:
// - LOG_OFF, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL
#define DEBUG_OFF   //release - Ensure this is included!
#ifdef DEBUG_OFF
    #define DEBUG_ANNOUNCE  LOG_OFF
    #define DEBUG_APPCFG    LOG_OFF
    #define DEBUG_BUS       LOG_OFF
    #define DEBUG_CMD       LOG_OFF
    #define DEBUG_DHCP      LOG_OFF
    #define DEBUG_DNS       LOG_OFF
    #define DEBUG_FTP       LOG_OFF
    #define DEBUG_FSEE      LOG_OFF
    #define DEBUG_GEN       LOG_OFF
    #define DEBUG_HTTP      LOG_OFF
    #define DEBUG_HTTPEXEC  LOG_OFF
    #define DEBUG_IP        LOG_OFF
    #define DEBUG_MAC       LOG_OFF
    #define DEBUG_MAIN      LOG_OFF
    #define DEBUG_NBNS      LOG_OFF
    #define DEBUG_STACKTSK  LOG_OFF
    #define DEBUG_TCP       LOG_OFF
    #define DEBUG_TCPUTILS  LOG_OFF
    #define DEBUG_TFTPC     LOG_OFF
    #define DEBUG_UDP       LOG_OFF
    #define DEBUG_UDPUTILS  LOG_OFF
#else
    #define DEBUG_ANNOUNCE  LOG_WARN
    #define DEBUG_APPCFG    LOG_WARN
    #define DEBUG_BUS       LOG_DEBUG
    #define DEBUG_CMD       LOG_DEBUG
    #define DEBUG_DHCP      LOG_WARN
    #define DEBUG_DNS       LOG_WARN
    #define DEBUG_FTP       LOG_WARN
    #define DEBUG_FSEE      LOG_INFO
    #define DEBUG_GEN       LOG_INFO
    #define DEBUG_HTTP      LOG_INFO
    #define DEBUG_HTTPEXEC  LOG_WARN
    #define DEBUG_IP        LOG_WARN
    #define DEBUG_MAC       LOG_INFO
    #define DEBUG_MAIN      LOG_INFO
    #define DEBUG_NBNS      LOG_INFO
    #define DEBUG_STACKTSK  LOG_INFO
    #define DEBUG_TCP       LOG_INFO
    #define DEBUG_TCPUTILS  LOG_INFO
    #define DEBUG_TFTPC     LOG_WARN
    #define DEBUG_UDP       LOG_INFO
    #define DEBUG_UDPUTILS  LOG_INFO
#endif

/////////////////////////////////////////////////
//The following message macros will write a message to the "General" tab
#define debugPutGenMsg(msgCode) debugPut2Bytes(0xD9, msgCode)
//#define debugPutGenRomStr(msgCode, msgStr) debugMsgRomStr(0xD9, msgCode, msgStr)
#define debugPutGenRomStr(msgCode, msgStr) {debugPut2Bytes(0xD9, msgCode); debugPutRomString(msgStr);}


/*******************************************************
-------------- Serint Configuration --------------------
********************************************************/
//Uncomment this line if the transmit routines should wait for the bytes to be send via USART if tx buffer is full
#define SER_WAIT_FOR_TXBUF
//Uncomment this line if the application does NOT configure the USART
#define BAUD_RATE    9600ul    //USART BAUD rate
//Comment this line if the application does NOT configures the USART
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b Serint: For details on configuring the Interrupt driven serial module @ref serint_conf "click here"
 */


/** @addtogroup mod_conf_projdefs
 * @code #define EEPROM_CONTROL (n) @endcode
 * This value is for Microchip 24LC256 - 256kb serial EEPROM
 */
#define EEPROM_CONTROL                  (0xa0ul)


/*******************************************************
----------------- Mac Configuration --------------------
********************************************************/
//When STACK_USE_FAST_NIC is defined, a bit longer, but faster code is used.
#define STACK_USE_FAST_NIC

//When defined, the code will be compiled for optimal speed. If not defined, code is defined for smallest size.
#define MAC_SPEED_OPTIMIZE

//STACK_DISABLES_INTS can be defined if interrupts are to be disabled during the
//MAC access routines
//#define STACK_DISABLES_INTS

 //NIC_DISABLE_INT0 can be defined if the MAC should NOT use INT0 (connected to PIC port RB0) for it's
 //interrupt request status line. When defined, INT0 is tri-stated, and the PIC port pin connected to
 //it can be used as a general purpose user IO pin. The PIC port pin that becomes available is:
 // - For SBC44EC this has no affect - no PIC pins are connected to the interrupt pins
 // - For SBC45EC this has no affect - no PIC pins are connected to the interrupt pins
 // - For SBC65EC and SBC68EC this frees up PIC pin RB0.
#define NIC_DISABLE_INT0

 //Keep a count of CNTR1 - CNTR3 registers. This can be used for debug purposes, and can be disabled for
 //release code.
#define MAC_CNTR1_3

 //Use access RAM variables to optiomize speed and code size. There are only a limited amount of access RAM
 //registers in the PIC processor. If they are not used by any other code modules, this define should be enabled
 //seeing that it will speed up the MAC module quite a bit. If they are not available, an error message will be
 //displayed during compilation.
#define MAC_USE_ACCESSRAM

//This define must be define when using this MAC
#define MAC_RTL8019AS

/*******************************************************
-------------- Serint Configuration --------------------
********************************************************/
//Uncomment this line if the transmit routines should wait for the bytes to be send via USART if tx buffer is full
#define SER_WAIT_FOR_TXBUF
//Uncomment this line if the application does NOT configure the USART
//#define BAUD_RATE    9600ul    //USART BAUD rate
//Comment this line if the application does NOT configures the USART
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b Serint: For details on configuring the Interrupt driven serial module @ref serint_conf "click here"
 */


/** @addtogroup mod_conf_projdefs
 * @code #define CLOCK_FREQ (n) @endcode
 * Configure the PIC's internal clock.
 */
#if defined(__18F452) || defined(_18F452) || defined(__18F458) || defined(_18F458)
    #define CLOCK_FREQ                      (20000000L)      // Hz
#elif defined(__18F6621) || defined(_18F6621)       \
        || defined(__18F6527) || defined(_18F6527)  \
        || defined(__18F6627) || defined(_18F6627)  \
        || defined(__18F6680) || defined(_18F6680)
    #define CLOCK_FREQ                      (40000000L)      // Hz
#endif


#define MY_MAC_BYTE1            (0x00ul)
#define MY_MAC_BYTE2            (0x04ul)
#define MY_MAC_BYTE3            (0xa3ul)
#define MY_MAC_BYTE4            (0x00ul)
#define MY_MAC_BYTE5            (0x00ul)
#define MY_MAC_BYTE6        	(0x50ul)



/*
 * Subnet mask for this node - is contained in AppConfig structure
 * Must not be all zero's or else this node will never transmit anything !!
 */
#define MY_MASK_BYTE1                   AppConfig.MyMask.v[0]
#define MY_MASK_BYTE2                   AppConfig.MyMask.v[1]
#define MY_MASK_BYTE3                   AppConfig.MyMask.v[2]
#define MY_MASK_BYTE4                   AppConfig.MyMask.v[3]

/*
 * IP address of this node - is contained in AppConfig structure
 */
#define MY_IP_BYTE1                     AppConfig.MyIPAddr.v[0]
#define MY_IP_BYTE2                     AppConfig.MyIPAddr.v[1]
#define MY_IP_BYTE3                     AppConfig.MyIPAddr.v[2]
#define MY_IP_BYTE4                     AppConfig.MyIPAddr.v[3]

/*
 * Gateway address for this node - is contained in AppConfig structure
 */
#define MY_GATE_BYTE1                   AppConfig.MyGateway.v[0]
#define MY_GATE_BYTE2                   AppConfig.MyGateway.v[1]
#define MY_GATE_BYTE3                   AppConfig.MyGateway.v[2]
#define MY_GATE_BYTE4                   AppConfig.MyGateway.v[3]

/*
 * Primary DNS address for this node
 */
#define MY_DNS_BYTE1                    appcfgGetc(APPCFG_DNS_IP0)
#define MY_DNS_BYTE2                    appcfgGetc(APPCFG_DNS_IP1)
#define MY_DNS_BYTE3                    appcfgGetc(APPCFG_DNS_IP2)
#define MY_DNS_BYTE4                    appcfgGetc(APPCFG_DNS_IP3)

#define MY_DNS_BYTE1_SET(n)             appcfgPutc(APPCFG_DNS_IP0, n)
#define MY_DNS_BYTE2_SET(n)             appcfgPutc(APPCFG_DNS_IP1, n)
#define MY_DNS_BYTE3_SET(n)             appcfgPutc(APPCFG_DNS_IP2, n)
#define MY_DNS_BYTE4_SET(n)             appcfgPutc(APPCFG_DNS_IP3, n)


// The MAC_TX_BUFFER_COUNT must be equal to MAX_SOCKETS + 1
// (for high priority messages), or else calls to TCPPut may 
// fail when multiple TCP sockets have data pending in the 
// output buffer that hasn't been acked.  Changing this value
// is recommended only if the rammifications of doing so are 
// properly understood.  
#if defined(NON_MCHP_MAC)
	#define MAC_TX_BUFFER_SIZE          (1024ul)
    #define MAC_TX_BUFFER_COUNT         (3ul)
#else
   	#define MAC_TX_BUFFER_SIZE          (576ul)
   	#define MAC_TX_BUFFER_COUNT         (MAX_SOCKETS+1)
#endif

// Rests are Receive Buffers

#define MAC_RX_BUFFER_SIZE              (MAC_TX_BUFFER_SIZE)

#if (MAC_TX_BUFFER_SIZE <= 0 || MAC_TX_BUFFER_SIZE > 1500 )
#error Invalid MAC_TX_BUFFER_SIZE value specified.
#endif

#if ( (MAC_TX_BUFFER_SIZE * MAC_TX_BUFFER_COUNT) > (4* 1024) )
#error Not enough room for Receive buffer.
#endif


#endif  //_PROJDEFS_H_
