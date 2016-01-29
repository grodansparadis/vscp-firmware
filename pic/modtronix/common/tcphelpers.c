#include <projdefs.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

/*
 * These headers must be included for required defs.
 */

#include "net/stacktsk.h"
#include "net/tick.h"
#include "net/mac.h"

#if defined(STACK_USE_DHCP)
#include "net/dhcp.h"
#endif

#if defined(STACK_USE_HTTP_SERVER)
#include "net/http.h"
#endif

#if defined(STACK_USE_FTP_SERVER) && defined(MPFS_USE_EEPROM)
#include "net/ftp.h"
#endif

#if defined(MPFS_USE_EEPROM)
#include "net/xeeprom.h"
#endif


// For debug only.
#include "net/ARPTsk.h"
#include "net/arp.h"
#include "net/tcp.h"
#include "net/icmp.h"
#include "net/delay.h"
#include "debug.h"

#include "tcphelpers.h"

////////////////////////////////////////////////////////////////////////////////////////////
// writeRomString2Socket
//

void writeRomString2Socket( TCP_SOCKET sock, ROM char* p, uint8_t bFlush ) 
{
	BYTE c;

   	while( ( c = *p++ ) ) {
		TCPPut( sock, c );
   	}
   	
   	if ( bFlush ) TCPFlush( sock );
}

////////////////////////////////////////////////////////////////////////////////////////////
// writeString2Socket
//

void writeString2Socket( TCP_SOCKET sock, ROM char* p, uint8_t bFlush ) 
{
   BYTE c;

   while( ( c = *p++ ) ) {
      TCPPut( sock, c );
   }
   
   if ( bFlush ) TCPFlush( sock );
}
