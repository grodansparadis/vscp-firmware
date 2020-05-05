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

#include "net\compiler.h"
#include "appcfg.h"

/////////////////////////////////////////////////
//Global variables defined in main application
#ifndef THIS_IS_STACK_APPLICATION
    //String must have format Vn.mm or Vnn.mm.
    // - n = major part, and can be 1 or 2 digets long
    // - mm is minor part, and must be 2 digets long!
    extern ROM char APP_VER_STR[];
#endif
#define APP_VER_MAJOR 2   /* Number from 1 to 99 */
#define APP_VER_MINOR 4   /* Number from 1 to 99 */


/////////////////////////////////////////////////
//Module configuration
/** @addtogroup mod_conf_projdefs
 * @section projdefs_modconf Module Configuration
 * The following section describes how to configure modules included in this project
 */


/*******************************************************
----------------- Appcfg Configuration -----------------
********************************************************/
//Define if this application has an 8 bit ADC converter, 10 bit ADC converter or none at all.
//#define APP_USE_ADC8
#define APP_USE_ADC10

//Define buffer space that is reserved for ADC conversion values. For a 8 bit ADC converted, 1 byte
//is reserved per channel. For an 10 bit ADC, 2 bytes are reserved per channel.
#define ADC_CHANNELS 9
/*******************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b Appcfg: For details on configuring the Appcfg module @ref mac_conf "click here"
 */


/*******************************************************
----------------- DHCP Configuration --------------------
********************************************************/
//Defines DHCP ports
#define DHCP_CLIENT_PORT    (68)
#define DHCP_SERVER_PORT    (67)

//The stack uses the macro STACK_IS_DHCP_ENABLED to determine if DHCP is enabled or not.
//The user can for example assign this macro to check if a flag is set. If not defined
//by the user, it will be set to true.
#define STACK_IS_DHCP_ENABLED   (appcfgGetc(APPCFG_NETFLAGS) & APPCFG_NETFLAGS_DHCP)

//Timeouts
#define DHCP_TIMEOUT        ((TICK16)(4 * TICKS_PER_SECOND))
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b DHCP: For details on configuring the DHCP module @ref dhcp_conf "click here"
 */


/*******************************************************
----------------- FTP Configuration --------------------
********************************************************/
#define FTP_COMMAND_PORT                (21)
#define FTP_DATA_PORT                   (20)
#define FTP_TIMEOUT                     ((TICK16) ((TICK16)180 * (TICK16)TICKS_PER_SECOND) )
#define MAX_FTP_ARGS                    (7)
#define MAX_FTP_CMD_STRING_LEN          (31)

//Configure FTP mudule to have PUT
#define FTP_PUT_ENABLED
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b FTP: For details on configuring the FTP module @ref ftp_conf "click here"
 */


/*******************************************************
----------------- HTTP Configuration --------------------
********************************************************/
//The following should be defined in the projdefs.h file OR on the command line

#define HTTP_PORT   (80)

//Comment this line if you do NOT want HTML files to be parsed (replace %xnn tags)
//#define HTTP_PARSE_FILETYPE_HTML

//Comment this line if you do NOT want JavaScript files to be parsed (replace %xnn tags)
//#define HTTP_PARSE_FILETYPE_JS
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b HTTP: For details on configuring the HTTP module @ref http_conf "click here"
 */


/*******************************************************
----------------- IP Configuration --------------------
********************************************************/
#define MY_IP_TTL   (100)   /* Time-To-Live in Seconds */
//When defined, the code will be compiled for optimal speed. If not defined, code is defined for smallest size.
#define IP_SPEED_OPTIMIZE
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b IP: For details on configuring the IP module @ref ip_conf "click here"
 */


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

 //NIC_DISABLE_IOCHRDY can be defined if the MAC should NOT use the IOCHRDY signal on the RTL8019AS chip. This
 //will mean that an additional PIC pin will be available for general purpose use. To use this port pin, the
 //connection to the IOCHRDY signal on RTL8019AS must be broken. This can be done via solder jumpers on certian
 //SBC boards.
 // - For SBC44EC PIC port pin B5 will be available for user IO. Solder jumper SJ5 must be opened!
 // - For SBC45EC PIC port pin A4 will be available for user IO. Solder jumper SJ5 must be opened!
 // - For SBC65EC and SBC68EC this frees up PIC pin RG4. This pin is currently however not routed to an connectors
 //#define NIC_DISABLE_IOCHRDY

 //Keep a count of CNTR1 - CNTR3 registers. This can be used for debug purposes, and can be disabled for
 //production code.
#define MAC_CNTR1_3

 //Use access RAM variables to optiomize speed and code size. There are only a limited amount of access RAM
 //registers in the PIC processor. If they are not used by any other code modules, this define should be enabled
 //seeing that it will speed up the MAC module quite a bit. If they are not available, an error message will be
 //displayed during compilation.
#define MAC_USE_ACCESSRAM
/*******************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b MAC: For details on configuring the MAC (Ethernet) module @ref mac_conf "click here"
 */


/*******************************************************
-------------- Serint Configuration --------------------
********************************************************/
#define SER_RXBUF_SIZE  32   //Size of Rx buffer, must be 8,16,32,64,128 or 256
#define SER_TXBUF_SIZE  32	//Size of TX buffer, must be 8,16,32,64,128 or 256
//Uncomment this line if the transmit routines should wait for the bytes to be send via USART if tx buffer is full
#define SER_WAIT_FOR_TXBUF
//Uncomment this line if the application does NOT configure the USART
//#define BAUD_RATE	9600	//USART BAUD rate
//Comment this line if the application does NOT configures the USART
#define APP_CONFIGURES_SERPORT  //Our application does all serial port configuration!
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b Serint: For details on configuring the Interrupt driven serial module @ref serint_conf "click here"
 */


/*********************************************************************
-------------------- UDP Configuration --------------------
*********************************************************************/
//When defined, the code will be compiled for optimal speed. If not defined, code is defined for smallest size.
#define UDP_SPEED_OPTIMIZE
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b UDP: For details on configuring the UDP module @ref udp_conf "click here"
 */


/*******************************************************
----------------- TCP Configuration --------------------
********************************************************/
//Maximum number of times a connection be retried before closing it down.
#define TCP_MAX_RETRY_COUNTS    (3)

//TCP Timeout value to begin with.
#define TCP_START_TIMEOUT_VAL   ((TICK16) ((TICK16)TICKS_PER_SECOND * (TICK16)60) )

//Define ports
#define TCP_LOCAL_PORT_START_NUMBER (1024)
#define TCP_LOCAL_PORT_END_NUMBER   (5000)

//When defined, the code will be compiled for optimal speed. If not defined, code is defined for smallest size.
#define TCP_SPEED_OPTIMIZE
/********************************************************/
/** @addtogroup mod_conf_projdefs
 * - @b TCP: For details on configuring the TCP module @ref udp_conf "click here"
 */


//*********************************************************************
//--------------------  File System Configuration --------------------
//*********************************************************************
//Defines the maximum size of a file used in the file system.
//When FSEE_FILE_SIZE_16MB is defined, the file system can handle files with a size of up to 16 Mbytes.
//When not defined, the maximum size of a file is 64 Kbyte.
//#define FSEE_FILE_SIZE_16MB

//Specifies the maximum number of files that can be open at any one time. When defined as 1, the code
//will be much faster and smaller. This value should not be much less then the the number of HTTP
//Connections, seeing that each HTTP connection can have a open file. If most web page files are
//small (below 2 kbytes) then this is not so important.
#define FSEE_MAX_FILES 6

//When this define is present, the FSEE File System is used as the primary file system. All functions
//Will be remapped to general names, for example fseeOpen() will be mapped to fileOpen. This makes switching
//between different File System much simpler.
#define FSEE_IS_PRIMARY_FS


/////////////////////////////////////////////////
//General configuration
//The following code is used for general configuration
/** @addtogroup mod_conf_projdefs
 * @section projdefs_genconf General Configuration
 * The following section describes how to configure general parameter contained in this project
 */


/** @addtogroup mod_conf_projdefs
 * @code #define HAS_BOOTLOADER @endcode
 * Include this define in the code to compiled the program to be uploaded to a device that
 * has the @ref tools_mxbootloader installed on it.
 * By doing this, this project will be compiled with the correct start of program address and
 * interrupt vector addresses. For further info in the @mxbootloader, click @ref tools_mxbootloader "here".<br>
 * The following compiler specific modifications have to be made:
 * - For HiTech compiler, remove "-A800h" option from linker
 * - For MPLAB C18 compiler, configure 18f6621.lkr file
 */
//Define the start of the program and interrupt vectors. This is used if this code is compiled for a bootloader.
#ifdef HAS_BOOTLOADER   //Bootloader that uses 0 - 0x7ff program memory
    #define RSTVECTOR_ADR   0x800
    #define HIVECTOR_ADR    0x808
    #define LOVECTOR_ADR    0x818
#else
    #define RSTVECTOR_ADR  0      /* Standard - no bootloader */
    #define HIVECTOR_ADR  0x8     /* Standard - no bootloader */
    #define LOVECTOR_ADR  0x18    /* Standard - no bootloader */
#endif


/** @addtogroup mod_conf_projdefs
 * @code #define CLOCK_FREQ (n) @endcode
 * Configure the PIC's internal clock.
 */
#define CLOCK_FREQ                      (40000000L)      // Hz
//#define CLOCK_FREQ                      (20000000L)      // Hz
//#define CLOCK_FREQ                      (10000000L)      // Hz


/** @addtogroup mod_conf_projdefs
 * @code #define DEBUG_OFF @endcode
 * Configures if debug information is written out onto the serial port. For the production version
 * of this project, this define should NOT be defined.<br>
 * Debug Configuration. When uncommenting any of the following line, remember to uncomment a debug
 * implementation in debug.h. For example, uncommend serint.h and link serint.c with project.<br>
 * For details, see @ref mod_sys_debug "Debugging" module.
 */
//Set Debug Log levels to one of the following:
// - LOG_OFF, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL
#define DEBUG_ON   //release Ensure this is not commented out!
#ifdef DEBUG_OFF
    #define DEBUG_APPCFG    	LOG_OFF
    #define DEBUG_GEN       	LOG_OFF
    #define DEBUG_HTTP      	LOG_OFF
    #define DEBUG_DHCP      	LOG_OFF
    #define DEBUG_FTP       	LOG_OFF
    #define DEBUG_FSEE      	LOG_OFF
    #define DEBUG_HTTPEXEC  	LOG_OFF
    #define DEBUG_MAC       	LOG_OFF
    #define DEBUG_MAIN      	LOG_OFF
    #define DEBUG_STACKTSK  	LOG_OFF
    #define DEBUG_UDP       	LOG_OFF
    #define DEBUG_TCP       	LOG_OFF
    #define DEBUG_IP        	LOG_OFF
    #define DEBUG_CLIENT_FTP 	LOG_OFF
    #define DEBUG_CLIENT_NTP 	LOG_OFF
    #define DEBUG_VSCP_TCP		LOG_ON
    #define DEBUG_VSCP_UDP		LOG_ON
    #define DEBUG_VSCP_TASK		LOG_ON
    #define DEBUG_VSCP_AUTO		LOG_OFF
#else
    #define DEBUG_APPCFG    	LOG_WARN    /* Debug code = 0xD0 */
    #define DEBUG_GEN       	LOG_WARN    /* Debug code = 0xA0 - 0xAF */
    #define DEBUG_HTTP      	LOG_INFO    /* Debug code = 0xA0 - 0xAF */
    #define DEBUG_DHCP      	LOG_INFO    /* Debug code = 0xD1 */
    #define DEBUG_FTP       	LOG_WARN    /* Debug code = 0xD2 */
    #define DEBUG_FSEE      	LOG_INFO    /* Debug code = 0xD3 */
    #define DEBUG_HTTPEXEC  	LOG_WARN    /* Debug code = 0xD4 */
    #define DEBUG_MAC       	LOG_WARN    /* Debug code = 0xD5 */
    #define DEBUG_MAIN      	LOG_WARN    /* Debug code = 0xD6 */
    #define DEBUG_STACKTSK  	LOG_WARN    /* Debug code = 0xD7 */
    #define DEBUG_UDP       	LOG_INFO    /* Debug code = 0x90 - 0x9F */
    #define DEBUG_TCP       	LOG_INFO    /* Debug code = 0x80 - 0x8F */
    #define DEBUG_IP        	LOG_WARN    /* Debug code = 0xD8 */
    #define DEBUG_CLIENT_FTP 	LOG_WARN
    #define DEBUG_CLIENT_NTP 	LOG_WARN
    #define DEBUG_VSCP_TCP		LOG_DEBUG
    #define DEBUG_VSCP_UDP		LOG_WARN
    #define DEBUG_VSCP_TASK		LOG_WARN
    #define DEBUG_VSCP_AUTO		LOG_DEBUG
#endif

/////////////////////////////////////////////////
//The following message macros will write a message to the "General" tab
#define debugPutGenMsg(msgCode) debugPut2Bytes(0xD9, msgCode)



/** @addtogroup mod_conf_projdefs
 * @code #define BRD_SBC65EC @endcode
 * Defines the Modtronix SBC board that this code is compiled for. Possible defines are:
 * - BRD_SBC44EC
 * - BRD_SBC45EC
 * - BRD_SBC65EC
 * - BRD_SBC68EC
 * This define is often defined in the MPLAB project file.
 */
//#define BRD_SBC65EC

/** @addtogroup mod_conf_projdefs
 * @code #define EEPROM_CONTROL (n) @endcode
 * This value is for Microchip 24LC256 - 256kb serial EEPROM
 */
#define EEPROM_CONTROL                  (0xa0)

/*
 * Number of bytes to be reserved before FSEE File System storage starts.
 *
 * These bytes can be used by the user application to store application
 * configurations data and any other required variables.
 *
 * After making any change to this variable, the file system image must be recreated
 * with correct block size.
 */
#define FSEE_RESERVE_BLOCK              (0x8000)

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_ICMP @endcode
 * Uncomment if stack is to use ICMP. For details see @ref mod_tcpip_base_icmp.
 */
#define STACK_USE_ICMP

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_HTTP_SERVER @endcode
 * Uncomment if stack is to have a HTTP server. This is usually the case, and this define is usually included.
 * For details see @ref mod_tcpip_httpsrvr.
 */
#define STACK_USE_HTTP_SERVER

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_SLIP @endcode
 * Uncomment if stack should implement the SLIP protocol. For details see @ref mod_tcpip_base_slip.
 */
//#define STACK_USE_SLIP

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_IP_GLEANING @endcode
 * Uncomment if stack should implement IP Gleaning. For details see @ref mod_tcpip_user_ipgleaning.
 */
//#define STACK_USE_IP_GLEANING

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_DHCP @endcode
 * Uncomment if stack should implement the DHCP protocol. For details see @ref mod_tcpip_user_dhcp.
 */
#define STACK_USE_DHCP

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_FTP_SERVER @endcode
 * Uncomment if stack should implement a FTP server. For details see @ref mod_tcpip_user_ftp.
 */
#define STACK_USE_FTP_SERVER

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_SNMP_SERVER @endcode
 * Uncomment if stack should implement the SNMP protocol.
 */
//#define STACK_USE_SNMP_SERVER

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_TFTP_CLIENT @endcode
 * Uncomment if stack should implement a TFTP client
 */
//#define STACK_USE_TFTP_CLIENT

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_SMTP @endcode
 * Uncomment if stack should implement SMTP
 */
//#define STACK_USE_SMTP

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_TCP @endcode
 * This define is automatically enabled/disabled based on high-level module selections.
 * If you need them with your custom application, enable it here.
 * Uncomment if stack should implement the TCP protocol. For details see @ref mod_tcpip_base_tcp.
 */
#define STACK_USE_TCP

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_USE_UDP @endcode
 * This define is automatically enabled/disabled based on high-level module selections.
 * If you need them with your custom application, enable it here.
 * Uncomment if stack should implement the UDP protocol. For details see @ref mod_tcpip_base_udp.
 */
#define STACK_USE_UDP

/*
 * When SLIP is used, DHCP is not supported.
 */
#if defined(STACK_USE_SLIP)
#undef STACK_USE_DHCP
#endif

/** @addtogroup mod_conf_projdefs
 * @code #define TCP_NO_WAIT_FOR_ACK @endcode
 * Comment following line if StackTsk should wait for acknowledgement
 * from remote host before transmitting another packet.
 * Commenting following line may reduce throughput.
 */
//#define TCP_NO_WAIT_FOR_ACK

/** @addtogroup mod_conf_projdefs
 * @code #define STACK_CLIENT_MODE @endcode
 * Uncomment following line if this stack will be used in CLIENT
 * mode.  In CLIENT mode, some functions specific to client operation
 * are enabled.
 */
#define STACK_CLIENT_MODE

/*
 * When HTTP is enabled, TCP must be enabled.
 */
#if defined(STACK_USE_HTTP_SERVER)
    #if !defined(STACK_USE_TCP)
        #define STACK_USE_TCP
    #endif
#endif

/*
 * When FTP is enabled, TCP must be enabled.
 */
#if defined(STACK_USE_FTP_SERVER)
    #if !defined(STACK_USE_TCP)
        #define STACK_USE_TCP
    #endif
#endif

#if defined(STACK_USE_FTP_SERVER) && !defined(STACK_CLIENT_MODE)
    #define STACK_CLIENT_MODE
#endif

#if defined(STACK_USE_SNMP_SERVER) && !defined(STACK_CLIENT_MODE)
    #define STACK_CLIENT_MODE
#endif

/*
 * When DHCP is enabled, UDP must also be enabled.
 */
#if defined(STACK_USE_DHCP)
    #if !defined(STACK_USE_UDP)
        #define STACK_USE_UDP
    #endif
#endif

#if defined(STACK_USE_SNMP_SERVER) && !defined(STACK_USE_UDP)
    #define STACK_USE_UDP
#endif

/*
 * When IP Gleaning is enabled, ICMP must also be enabled.
 */
#if defined(STACK_USE_IP_GLEANING)
    #if !defined(STACK_USE_ICMP)
        #define STACK_USE_ICMP
    #endif
#endif


/*
 * When TFTP Client is enabled, UDP must also be enabled.
 * And client mode must also be enabled.
 */
#if defined(STACK_USE_TFTP_CLIENT) && !defined(STACK_USE_UDP)
    #define STACK_USE_UDP
#endif

#if defined(STACK_USE_TFTP_CLIENT) && !defined(STACK_CLIENT_MODE)
    #define STACK_CLIENT_MODE
#endif


/*
 * DHCP requires unfragmented packet size of at least 328 bytes,
 * and while in SLIP mode, our maximum packet size is less than
 * 255.  Hence disallow DHCP module while SLIP is in use.
 * If required, one can use DHCP while SLIP is in use by modifying
 * C18 linker scipt file such that C18 compiler can allocate
 * a static array larger than 255 bytes.
 * Due to very specific application that would require this,
 * sample stack does not provide such facility.  Interested users
 * must do this on their own.
 */
#if defined(STACK_USE_SLIP)
    #if defined(STACK_USE_DHCP)
        #error DHCP cannot be used when SLIP is enabled.
    #endif
#endif


/** @addtogroup mod_conf_projdefs
 * @code
 * #define MY_DEFAULT_IP_ADDR_BYTE1 (n)
 * #define MY_DEFAULT_IP_ADDR_BYTE2 (n)
 * #define MY_DEFAULT_IP_ADDR_BYTE3 (n)
 * #define MY_DEFAULT_IP_ADDR_BYTE4 (n)
 * @endcode
 * Use these defines to define the default IP address of the device.
 */
#define MY_DEFAULT_IP_ADDR_BYTE1        (192)
#define MY_DEFAULT_IP_ADDR_BYTE2        (168)
#define MY_DEFAULT_IP_ADDR_BYTE3        (1)
#define MY_DEFAULT_IP_ADDR_BYTE4        (50)

/** @addtogroup mod_conf_projdefs
 * @code
 * #define MY_DEFAULT_MASK_BYTE1 (n)
 * #define MY_DEFAULT_MASK_BYTE2 (n)
 * #define MY_DEFAULT_MASK_BYTE3 (n)
 * #define MY_DEFAULT_MASK_BYTE4 (n)
 * 
 * #define MY_DEFAULT_GATE_BYTE1 (n)
 * #define MY_DEFAULT_GATE_BYTE2 (n)
 * #define MY_DEFAULT_GATE_BYTE3 (n)
 * #define MY_DEFAULT_GATE_BYTE4 (n)
 * 
 * #define MY_DEFAULT_MAC_BYTE1 (n)
 * #define MY_DEFAULT_MAC_BYTE2 (n)
 * #define MY_DEFAULT_MAC_BYTE3 (n)
 * #define MY_DEFAULT_MAC_BYTE4 (n)
 * #define MY_DEFAULT_MAC_BYTE5 (n)
 * #define MY_DEFAULT_MAC_BYTE6 (n)
 * @endcode
 * Use these defines to define the default Mask, Gateway and Ethernet MAC address of this device.
 */
#define MY_DEFAULT_MASK_BYTE1           (0xff)      // Mask
#define MY_DEFAULT_MASK_BYTE2           (0xff)
#define MY_DEFAULT_MASK_BYTE3           (0xff)
#define MY_DEFAULT_MASK_BYTE4           (0x00)

#define MY_DEFAULT_GATE_BYTE1           (192)       // Gateway to use
#define MY_DEFAULT_GATE_BYTE2           (168)
#define MY_DEFAULT_GATE_BYTE3           (1)
#define MY_DEFAULT_GATE_BYTE4           (254)

#define MY_DEFAULT_MAC_BYTE1            (0x00)      // Ethernet MAC id
#define MY_DEFAULT_MAC_BYTE2            (0x04)
#define MY_DEFAULT_MAC_BYTE3            (0xa3)
#define MY_DEFAULT_MAC_BYTE4            (0x00)
#define MY_DEFAULT_MAC_BYTE5            (0x00)
#define MY_DEFAULT_MAC_BYTE6            (0x00)

/*
 * Mac address for this node - is contained in AppConfig structure
 */
#define MY_MAC_BYTE1                    AppConfig.MyMACAddr.v[0]
#define MY_MAC_BYTE2                    AppConfig.MyMACAddr.v[1]
#define MY_MAC_BYTE3                    AppConfig.MyMACAddr.v[2]
#define MY_MAC_BYTE4                    AppConfig.MyMACAddr.v[3]
#define MY_MAC_BYTE5                    AppConfig.MyMACAddr.v[4]
#define MY_MAC_BYTE6                    AppConfig.MyMACAddr.v[5]

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


/** @addtogroup mod_conf_projdefs
 * @code #define MAC_SOCKETS @endcode
 * Number of available TCP sockets to be created. Note that each socket consumes 34 bytes of RAM.<br>
 * <b>TCP configurations</b><br> To minimize page update, match number of sockets and
 * HTTP connections with different page sources in a page. For example, if page
 * contains reference to 3 more pages, browser may try to open 4 simultaneous
 * HTTP connections, and to minimize browser delay, set HTTP connections to
 * 4, MAX_SOCKETS to 4. If you are using ICMP or other applications, you should
 * keep at least one socket available for them.
 */
#if defined(MCHP_C18)
#define MAX_SOCKETS                     (10)
#elif defined(HITECH_C18)
#define MAX_SOCKETS                     (10)
#endif

/** @addtogroup mod_conf_projdefs
 * @code #define MAC_UDP_SOCKETS @endcode
 * Number of available UDP sockets to be created.
 */
#define MAX_UDP_SOCKETS     (2)


#if (MAX_SOCKETS <= 0 || MAX_SOCKETS > 255)
#error Invalid MAX_SOCKETS value specified.
#endif

#if (MAX_UDP_SOCKETS <= 0 || MAX_UDP_SOCKETS > 255 )
#error Invlaid MAX_UDP_SOCKETS value specified
#endif


#if !defined(STACK_USE_SLIP)
    #define MAC_TX_BUFFER_SIZE          (1024)
    #define MAC_TX_BUFFER_COUNT         (1)
#else
/*
 * For SLIP, there can only be one transmit and one receive buffer.
 * Both buffer must fit in one bank.  If bigger buffer is required,
 * you must manually locate tx and rx buffer in different bank
 * or modify your linker script file to support arrays bigger than
 * 256 bytes.
 */
    #define MAC_TX_BUFFER_SIZE          (250)
    #define MAC_TX_BUFFER_COUNT         (1)
#endif
// Rests are Receive Buffers

#define MAC_RX_BUFFER_SIZE              (MAC_TX_BUFFER_SIZE)

#if (MAC_TX_BUFFER_SIZE <= 0 || MAC_TX_BUFFER_SIZE > 1500 )
#error Invalid MAC_TX_BUFFER_SIZE value specified.
#endif

#if ( (MAC_TX_BUFFER_SIZE * MAC_TX_BUFFER_COUNT) > (4* 1024) )
#error Not enough room for Receive buffer.
#endif

/** @addtogroup mod_conf_projdefs
 * @code #define HTTP_CONNECTIONS @endcode
 * Maximum numbers of simultaneous HTTP connections allowed.
 * Each connection consumes 10 bytes.
 */
#if defined(MCHP_C18)
#define MAX_HTTP_CONNECTIONS            (8)
#elif defined(HITECH_C18)
#define MAX_HTTP_CONNECTIONS            (8)
#endif

#if (MAX_HTTP_CONNECTIONS > FSEE_MAX_FILES)
    #if ((MAX_HTTP_CONNECTIONS - FSEE_MAX_FILES) > 2)
        #error "Too little file handles defined!"
        #error "For better performance of the web server, try and match HTTP Connections and file handles!"
    #endif
#endif

#if (MAX_HTTP_CONNECTIONS <= 0 || MAX_HTTP_CONNECTIONS > 255 )
#error Invalid MAX_HTTP_CONNECTIONS value specified.
#endif

#define AVAILABLE_SOCKETS (MAX_SOCKETS)
#if defined(STACK_USE_HTTP_SERVER)
    #define AVAILABLE_SOCKETS2 (AVAILABLE_SOCKETS - MAX_HTTP_CONNECTIONS)
#else
    #define AVAILABLE_SOCKETS2  (MAX_SOCKETS)
#endif

/*
 * When using FTP, you must have at least two sockets free
 */
#if defined(STACK_USE_FTP_SERVER)
    #define AVAILABLE_SOCKETS3 (AVAILABLE_SOCKETS2 - 2)
#else
    #define AVAILABLE_SOCKETS3  (AVAILABLE_SOCKETS2)
#endif

#if AVAILABLE_SOCKETS3 < 0
    #error Maximum TCP Socket count is not enough.
    #error Either increase MAX_SOCKETS or decrease module socket usage.
#endif


#define AVAILABLE_UDP_SOCKETS       (MAX_UDP_SOCKETS)
#if defined(STACK_USE_DHCP)
    #define AVAILABLE_UDP_SOCKETS2       (AVAILABLE_UDP_SOCKETS - 1)
#else
    #define AVAILABLE_UDP_SOCKETS2      AVAILABLE_UDP_SOCKETS
#endif

#if defined(STACK_USE_SNMP_SERVER)
    #define AVAILABLE_UDP_SOCKETS3      (AVAILABLE_UDP_SOCKETS2 - 1)
#else
    #define AVAILABLE_UDP_SOCKETS3      AVAILABLE_UDP_SOCKETS2
#endif

#if defined(STACK_USE_TFTP_CLIENT)
    #define AVAILABLE_UDP_SOCKETS4      (AVAILABLE_UDP_SOCKETS2)
#else
    #define AVAILABLE_UDP_SOCKETS4      AVAILABLE_UDP_SOCKETS3
#endif


#if AVAILABLE_UDP_SOCKETS4 < 0
    #error Maximum UDP Socket count is not enough.
    #error Either increase MAX_UDP_SOCKETS or decrease module UDP socket usage.
#endif



#endif  //_PROJDEFS_H_
