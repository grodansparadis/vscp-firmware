/*
	FreeRTOS.org V5.0.4 - Copyright (C) 2003-2008 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and 
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety 
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting, 
	licensing and training services.
*/

/*
	Implements a simplistic WEB server.  Every time a connection is made and
	data is received a dynamic page that shows the current TCP/IP statistics
	is generated and returned.  The connection is then closed.

	This file was adapted from a FreeRTOS lwIP slip demo supplied by a third
	party.
*/

/*
	Changes from V3.2.2

	+ Changed the page returned by the lwIP WEB server demo to display the 
	  task status table rather than the TCP/IP statistics.
*/


//
// Adopted from FreeRTOS demo lwIP_Demo_Rowley_ARM7 eample by AKHE
// 2009-10-10, akhe@eurosource.se - http://www.vscp.org
//

// Standard includes. 
//#include <stdio.h>
#include <string.h>
#include <p32xxxx.h>

// Scheduler includes. 
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// 
#include "taskTcpip.h"
#include "ethernet.def"


// lwIP includes. 
#include "lwip/api.h" 
#include "lwip/tcpip.h"
#include "lwip/memp.h" 
#include "lwip/stats.h"
#include "netif/loopif.h"

#include "vscp_level2.h"

// Sweetbox includes
#include "helpers.h"


/* The port on which we listen. */
#define VSCP_PORT		( VSCP_LEVEL2_TCP_PORT )

/* Delay on close error. */
#define webSHORT_DELAY		( 10 )

/* 
 * Process an incoming connection on the VSCP port.
 */
void taskVSCPServer( void *pvParameters );

///////////////////////////////////////////////////////////////////////////////
// taskVSCPServer
//

void taskVSCPServer( void *pvParameters )
{
    struct netconn *pxNetCon = (struct netconn *)pvParameters;
    struct netbuf *pxRxBuffer;

    // Write out welcome message. 
    netconn_write( pxNetCon, webHTTP_OK, (u16_t)strlen( webHTTP_OK ), NETCONN_COPY );

	// We expect to immediately get data. 
	pxRxBuffer = netconn_recv( pxNetCon );

	if( pxRxBuffer != NULL )
	{
		// Where is the data? 
		netbuf_data( pxRxBuffer, ( void * ) &pcRxString, &usLength );	   
	
		// Is this a GET?  We don't handle anything else. 
		if( !strncmp( pcRxString, "GET", 3 ) )
		{

			// Write out the HTTP OK header. 
            netconn_write(pxNetCon, webHTTP_OK, (u16_t)strlen( webHTTP_OK ), NETCONN_COPY );


			// Write out the dynamically generated page. 
			netconn_write(pxNetCon, cDynamicPage, (u16_t)strlen( cDynamicPage ), NETCONN_COPY );
		}
 
		netbuf_delete( pxRxBuffer );
	}

	netconn_close( pxNetCon );
}



///////////////////////////////////////////////////////////////////////////////
// taskVSCPServerListner
//

void taskVSCPServerListner( void *pvParameters )
{
    struct netconn *pxVSCPListener, *pxNewConnection;
    struct ip_addr xIpAddr, xNetMask, xGateway;
    extern err_t ethernetif_init( struct netif *netif );
    static struct netif EMAC_if;

	// Parameters are not used - suppress compiler error. 
	( void ) pvParameters;

	// Create and configure the EMAC interface. 
	IP4_ADDR( &xIpAddr, 192, 168, 1, 201 );
	IP4_ADDR( &xNetMask, 255, 255, 255, 0 );
	IP4_ADDR( &xGateway, 192, 169, 1, 254 );
    
    //IPStr2Bin( DEFAULT_IP_ADDR, &xIpAddr.addr );
    //IPStr2Bin( DEFAULT_IP_MASK, &xNetMask.addr );
    //IPStr2Bin( DEFAULT_IP_GATEWAY, &xGateway.addr );
    
    //MACStrToBin( DEFAULT_MAC_ADDR, );

    // Add the network interface
    netif_add( &EMAC_if, &xIpAddr, &xNetMask, &xGateway, NULL, ethernetif_init, tcpip_input );

	// make it the default interface 
    netif_set_default( &EMAC_if );

	// bring it up 
    netif_set_up( &EMAC_if );
	
	// Create a new tcp connection handle 
 	pxVSCPListener = netconn_new( NETCONN_TCP );
	netconn_bind( pxVSCPListener, NULL, VSCP_LEVEL2_TCP_PORT );
	netconn_listen( pxVSCPListener );

	// Loop forever 
	while ( TRUE )  {
    	
		// Wait for connection. 
		pxNewConnection = netconn_accept( pxVSCPListener );

		if ( pxNewConnection != NULL ) {
		
			// Service connection. 
			vProcessConnection( pxNewConnection );
		
    		while( netconn_delete( pxNewConnection ) != ERR_OK ) {
				vTaskDelay( webSHORT_DELAY );
			}
		
		}
		
	}
	
}

