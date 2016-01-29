/*
 * This file should only be used to check if defines that are required by the stack
 * have been defined. If not, they are defined with default values. The purpose of this file
 * is to ensure that newer versions of the stack still with old projects.
 *
 * For example, the Microchip people added the NON_MCHP_MAC define to a new version of their stack. 
 * For this new version of the stack to work with the Modtronix boards, that use a non Microchip MAC,
 * this define must be defined. The correct place to define it will be in the projdefs.h file. But,
 * because older projects don't have this define, the new stack will not compile for the older
 * projects. This file simply checks if these defines have been defined, and if not, defines them
 * so they work on the Modtronix boards.
 */

#ifndef CHECKCFG_H
#define CHECKCFG_H


/////////////////////////////////////////////////
//If no MAC_XXX define has been defined, define default MAC_RTL8019AS
#if !defined(MAC_ENC28J60) && !defined(MAC_ETH97J60) && !defined(MAC_RTL8019AS)
#define MAC_RTL8019AS
#endif


/////////////////////////////////////////////////
//If NetBIOS name has not been defined by the application, use default name.
#ifndef NETBIOS_NAME_GETCHAR
    #define NETBIOS_NAME_GETCHAR(n) (NETBIOS_NAME_DEF[n])
    #define NETBIOS_NAME_GETCHAR_DEFAULT    /* Indicate that nbns.c should define default name */
#endif


/////////////////////////////////////////////////
//If no FAST_USER_PROCESS has been defined, define default one that clears the watchdog timer
#ifndef FAST_USER_PROCESS
#define FAST_USER_PROCESS() {       \
    CLRWDT();                       \
}
#endif


/////////////////////////////////////////////////
// These new defines have been defined for V2.45
#ifndef DEFAULT_HTTPSRVR_PORT
    #define DEFAULT_HTTPSRVR_PORT (80ul)
#endif
#ifndef HTTPSRVR_PORT
    #define HTTPSRVR_PORT (80ul)
#endif


/////////////////////////////////////////////////
//If MAC is RTL8019AS, ensure NON_MCHP_MAC is defined
#if defined(MAC_RTL8019AS)
    #if !defined(NON_MCHP_MAC)
    #define NON_MCHP_MAC
    #endif
#endif


/////////////////////////////////////////////////
//Debug checks
#ifndef DEBUG_OFF
    #ifndef DEBUG_ANNOUNCE
    #define DEBUG_ANNOUNCE  LOG_WARN
    #endif
    #ifndef DEBUG_CMD
    #define DEBUG_CMD       LOG_WARN
    #endif
    #ifndef DEBUG_DHCP
    #define DEBUG_DHCP      LOG_WARN
    #endif
    #ifndef DEBUG_DNS
    #define DEBUG_DNS       LOG_WARN
    #endif
    #ifndef DEBUG_FTP
    #define DEBUG_FTP       LOG_WARN
    #endif
    #ifndef DEBUG_FSEE
    #define DEBUG_FSEE      LOG_WARN
    #endif
    #ifndef DEBUG_HTTP
    #define DEBUG_HTTP      LOG_WARN
    #endif
    #ifndef DEBUG_HTTPEXEC
    #define DEBUG_HTTPEXEC  LOG_WARN
    #endif
    #ifndef DEBUG_IP
    #define DEBUG_IP        LOG_WARN
    #endif
    #ifndef DEBUG_MAC
    #define DEBUG_MAC       LOG_WARN
    #endif
    #ifndef DEBUG_NBNS
    #define DEBUG_NBNS      LOG_WARN
    #endif
    #ifndef DEBUG_STACKTSK
    #define DEBUG_STACKTSK  LOG_WARN
    #endif
    #ifndef DEBUG_TCP
    #define DEBUG_TCP       LOG_WARN
    #endif
    #ifndef DEBUG_TCPUTILS
    #define DEBUG_TCPUTILS  LOG_WARN
    #endif
    #ifndef DEBUG_TFTPC
    #define DEBUG_TFTPC     LOG_WARN
    #endif
    #ifndef DEBUG_UDP
    #define DEBUG_UDP       LOG_WARN
    #endif
    #ifndef DEBUG_UDPUTILS
    #define DEBUG_UDPUTILS  LOG_WARN
    #endif
#endif

#endif      // #ifndef CHECKCFG_H
