/*
Project specific defines
*/

#ifndef _PROJDEFS_H_
#define _PROJDEFS_H_

#include <compiler.h>

#define FIRMWARE_MAJOR_VERSION			0
#define FIRMWARE_MINOR_VERSION			0
#define FIRMWARE_SUB_MINOR_VERSION		2


/*******************************************************
Defines for "compile" module
********************************************************/
#define CLOCK_FREQ                      (40000000)      // Hz
//#define CLOCK_FREQ                    (20000000)      // Hz
//#define CLOCK_FREQ                    (10000000)      // Hz


/*******************************************************
-------------- Serint Configuration --------------------
********************************************************/
#define SER_RXBUF_SIZE  8   //Size of Rx buffer, must be 8,16,32,64,128 or 256
#define SER_TXBUF_SIZE  128	//Size of TX buffer, must be 8,16,32,64,128 or 256
//Uncomment this line if the transmit routines should wait for the bytes to be send via USART if tx buffer is full
#define SER_WAIT_FOR_TXBUF
//Uncomment this line if the application does NOT configure the USART
//#define BAUD_RATE	9600	//USART BAUD rate
//Comment this line if the application does NOT configures the USART
#define APP_CONFIGURES_SERPORT  //Our application does all serial port configuration!
/********************************************************/


/*******************************************************
----------------- Mac Configuration --------------------
********************************************************/
//When STACK_USE_FAST_NIC is defined, a bit longer, but faster code is used.
#define STACK_USE_FAST_NIC

//STACK_DISABLES_INTS can be defined if interrupts are to be disabled during the MAC access routines
#define STACK_DISABLES_INTS

//Keep a count of CNTR1 - CNTR3 registers
#define MAC_CNTR1_3

/*******************************************************
----------------- Appcfg Configuration -----------------
********************************************************/
//The following should be defined in the projdefs.h file OR on the command line

//Define if this application has an 8 bit ADC converter, 10 bit ADC converter or none at all.
//#define APP_USE_ADC8
#define APP_USE_ADC10

//Define buffer space that is reserved for ADC conversion values. For a 8 bit ADC converted, 1 byte
//is reserved per channel. For an 10 bit ADC, 2 bytes are reserved per channel.
#define ADC_CHANNELS 12

/*******************************************************/

/*
 * Debug Configuration. When uncommenting any of the following line, remember to uncomment a debug
 * implementation in debug.h. For example, uncommend serint.h and link serint.c with project
 */
//Uncomment lines to activate debugging
//#define DEBUG_HTTP
//#define DEBUG_DHCP
//#define DEBUG_FTP
//#define DEBUG_HTTPEXEC
//#define DEBUG_MAC


/*
 * This value is used by TCP to implement timeout actions.
 * If SNMP module is in use, this value should be 100 as required
 * by SNMP protocol unless main application is providing separate
 * tick which 10mS.
 */
#define TICKS_PER_SECOND               (100)        // 10ms

#if (TICKS_PER_SECOND < 10 || TICKS_PER_SECOND > 255)
#error Invalid TICKS_PER_SECONDS specified.
#endif

/*
 * Manually select prescale value to achieve necessary tick period
 * for a given clock frequency.
 */
#define TICK_PRESCALE_VALUE             (256)

#if (TICK_PRESCALE_VALUE != 2 && \
     TICK_PRESCALE_VALUE != 4 && \
     TICK_PRESCALE_VALUE != 8 && \
     TICK_PRESCALE_VALUE != 16 && \
     TICK_PRESCALE_VALUE != 32 && \
     TICK_PRESCALE_VALUE != 64 && \
     TICK_PRESCALE_VALUE != 128 && \
     TICK_PRESCALE_VALUE != 256 )
#error Invalid TICK_PRESCALE_VALUE specified.
#endif

#if defined(WIN32)
    #undef TICKS_PER_SECOND
    #define TICKS_PER_SECOND        (1)
#endif

/*
 * Use VSCP functionality
 *
 */
#define STACK_USE_VSCP


/*
 * Defines the type of Modtronix board used
 */
#define BRD_SBC65EC

/*
 * This value is for Microchip 24LC256 - 256kb serial EEPROM
 */
#define EEPROM_CONTROL                  (0xa0)

/*
 * Number of bytes to be reserved before MPFS storage is to start.
 *
 * These bytes host application configurations such as IP Address,
 * MAC Address, and any other required variables.
 *
 * After making any change to this variable, MPFS.exe must be
 * executed with correct block size.
 * See MPFS.exe help message by executing MPFS /?
 */
#define MPFS_RESERVE_BLOCK              (64)

/*
 * Comment/Uncomment following lines depending on types of modules
 * are required.
 */
#define STACK_USE_ICMP
#define STACK_USE_HTTP_SERVER

/*
 * Stack Configuration
 */
//#define STACK_USE_SLIP
#define STACK_USE_IP_GLEANING
//#define STACK_USE_DHCP
#define STACK_USE_FTP_SERVER
//#define STACK_USE_SNMP_SERVER
//#define STACK_USE_TFTP_CLIENT

/*
 * Following low level modules are automatically enabled/disabled based on high-level
 * module selections.
 * If you need them with your custom application, enable it here.
 */
//#define STACK_USE_TCP
#define STACK_USE_UDP

// Force UDP use if VSCP enabled
#if defined(STACK_USE_VSCP)
#define STACK_USE_UDP
#endif

/*
 * When SLIP is used, DHCP is not supported.
 */
#if defined(STACK_USE_SLIP)
#undef STACK_USE_DHCP
#endif

/*
 * When MPFS_USE_PGRM is used, FTP is not supported.
 */
#if defined(MPFS_USE_PGRM)
#undef STACK_USE_FTP_SERVER
#endif


/*
 * Comment following line if StackTsk should wait for acknowledgement
 * from remote host before transmitting another packet.
 * Commenting following line may reduce throughput.
 */
#define TCP_NO_WAIT_FOR_ACK


/*
 * Uncomment following line if this stack will be used in CLIENT
 * mode.  In CLIENT mode, some functions specific to client operation
 * are enabled.
 */
//#define STACK_CLIENT_MODE


/*
 * If html pages are stored in internal program memory,
 * uncomment MPFS_USE_PRGM and comment MPFS_USE_EEPROM
 * If html pages are stored in external eeprom memory,
 * comment MPFS_USE_PRGM and uncomment MPFS_USE_EEPROM
 */
//#define MPFS_USE_PGRM
#define MPFS_USE_EEPROM

#if defined(MPFS_USE_PGRM) && defined(MPFS_USE_EEPROM)
#error Invalid MPFS Storage option specified.
#endif

#if !defined(MPFS_USE_PGRM) && !defined(MPFS_USE_EEPROM)
#error You have not specified MPFS storage option.
#endif

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


/*
 * Default Address information - If not found in data EEPROM.
 */
#define MY_DEFAULT_IP_ADDR_BYTE1        (10)
#define MY_DEFAULT_IP_ADDR_BYTE2        (1)
#define MY_DEFAULT_IP_ADDR_BYTE3        (0)
#define MY_DEFAULT_IP_ADDR_BYTE4        (1)

#define MY_DEFAULT_MASK_BYTE1           (0xff)
#define MY_DEFAULT_MASK_BYTE2           (0x00)
#define MY_DEFAULT_MASK_BYTE3           (0x00)
#define MY_DEFAULT_MASK_BYTE4           (0x00)

#define MY_DEFAULT_GATE_BYTE1           MY_DEFAULT_IP_ADDR_BYTE1
#define MY_DEFAULT_GATE_BYTE2           MY_DEFAULT_IP_ADDR_BYTE2
#define MY_DEFAULT_GATE_BYTE3           MY_DEFAULT_IP_ADDR_BYTE3
#define MY_DEFAULT_GATE_BYTE4           MY_DEFAULT_IP_ADDR_BYTE4

#define MY_DEFAULT_MAC_BYTE1            (0x00)
#define MY_DEFAULT_MAC_BYTE2            (0x04)
#define MY_DEFAULT_MAC_BYTE3            (0xa3)
#define MY_DEFAULT_MAC_BYTE4            (0x00)
#define MY_DEFAULT_MAC_BYTE5            (0x00)
#define MY_DEFAULT_MAC_BYTE6            (0x00)



#define MY_MAC_BYTE1                    AppConfig.MyMACAddr.v[0]
#define MY_MAC_BYTE2                    AppConfig.MyMACAddr.v[1]
#define MY_MAC_BYTE3                    AppConfig.MyMACAddr.v[2]
#define MY_MAC_BYTE4                    AppConfig.MyMACAddr.v[3]
#define MY_MAC_BYTE5                    AppConfig.MyMACAddr.v[4]
#define MY_MAC_BYTE6                    AppConfig.MyMACAddr.v[5]

/*
 * Subnet mask for this node.
 * Must not be all zero's or else this node will never transmit
 * anything !!
 */
#define MY_MASK_BYTE1                   AppConfig.MyMask.v[0]
#define MY_MASK_BYTE2                   AppConfig.MyMask.v[1]
#define MY_MASK_BYTE3                   AppConfig.MyMask.v[2]
#define MY_MASK_BYTE4                   AppConfig.MyMask.v[3]

/*
 * Hardcoded IP address of this node
 * My IP = 10.10.5.10
 *
 * Gateway = 10.10.5.10
 */
#define MY_IP_BYTE1                     AppConfig.MyIPAddr.v[0]
#define MY_IP_BYTE2                     AppConfig.MyIPAddr.v[1]
#define MY_IP_BYTE3                     AppConfig.MyIPAddr.v[2]
#define MY_IP_BYTE4                     AppConfig.MyIPAddr.v[3]

/*
 * Harcoded Gateway address for this node.
 * This should be changed to match actual network environment.
 */
#define MY_GATE_BYTE1                   AppConfig.MyGateway.v[0]
#define MY_GATE_BYTE2                   AppConfig.MyGateway.v[1]
#define MY_GATE_BYTE3                   AppConfig.MyGateway.v[2]
#define MY_GATE_BYTE4                   AppConfig.MyGateway.v[3]


/*
 * TCP configurations
 * To minmize page update, match number of sockets and
 * HTTP connections with different page sources in a
 * page.
 * For example, if page contains reference to 3 more pages,
 * browser may try to open 4 simultaneous HTTP connections,
 * and to minimize browser delay, set HTTP connections to
 * 4, MAX_SOCKETS to 4.
 * If you are using ICMP or other applications, you should
 * keep at least one socket available for them.
 */

/*
 * Maximum sockets to be defined.
 * Note that each socket consumes 38 bytes of RAM.
 */
#if defined(MCHP_C18)
#define MAX_SOCKETS                     (10)
#elif defined(HITECH_C18)
#define MAX_SOCKETS                     (10)
#endif

/*
 * Avaialble UDP Socket
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

/*
 * Maximum numbers of simultaneous HTTP connections allowed.
 * Each connection consumes 10 bytes.
 */
#if defined(MCHP_C18)
#define MAX_HTTP_CONNECTIONS            (8)	//Changed from 2 to 8
#elif defined(HITECH_C18)
#define MAX_HTTP_CONNECTIONS            (8)	//Changed from 2 to 4
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
