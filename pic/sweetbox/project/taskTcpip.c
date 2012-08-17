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

// Sweetbox includes
#include "sweetbox.h"
#include "helpers.h"
#include <vscp_level2.h>

// The size of the buffer in which the dynamic WEB page is created. 
#define webMAX_PAGE_SIZE	2048

// Standard GET response. 
#define webHTTP_OK	"HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

// The port on which we listen. 
#define webHTTP_PORT		( 80 )

// Delay on close error. 
#define webSHORT_DELAY		( 10 )

// Format of the dynamic page that is returned on each connection. 
#define webHTML_START \
"<html>\
<head>\
</head>\
<BODY onLoad=\"window.setTimeout(&quot;location.href='index.html'&quot;,1000)\"bgcolor=\"#CCCCff\">\
<h1>Sweetbox + lwIP + FreeRTOS</h1><br><br>Page Hits = "

#define webHTML_END \
"\r\n</pre>\
\r\n</BODY>\
</html>"

xSemaphoreHandle xSemaphore;


/* 
 * Process an incoming connection on port 80.
 *
 * This simply checks to see if the incoming data contains a GET request, and
 * if so sends back a single dynamically created page.  The connection is then
 * closed.  A more complete implementation could create a task for each 
 * connection. 
 */
static void vProcessConnection( struct netconn *pxNetCon );

///////////////////////////////////////////////////////////////////////////////
// vProcessConnection
//

static void vProcessConnection( struct netconn *pxNetCon )
{
    static portCHAR cDynamicPage[ webMAX_PAGE_SIZE ], cPageHits[ 11 ];
    struct netbuf *pxRxBuffer;
    portCHAR *pcRxString;
    unsigned portSHORT usLength;
    static unsigned portLONG ulPageHits = 0;

	// We expect to immediately get data. 
	pxRxBuffer = netconn_recv( pxNetCon );

	if ( pxRxBuffer != NULL ) {
    	
		// Where is the data? 
		netbuf_data( pxRxBuffer, ( void * ) &pcRxString, &usLength );	   
	
		// Is this a GET?  We don't handle anything else. 
		if( !strncmp( pcRxString, "GET", 3 ) ) {
			pcRxString = cDynamicPage;

			// Update the hit count. 
			ulPageHits++;
			//sprintf( cPageHits, "%lu", ulPageHits ); 

			// Write out the HTTP OK header. 
            netconn_write(pxNetCon, webHTTP_OK, (u16_t)strlen( webHTTP_OK ), NETCONN_COPY );

			/* Generate the dynamic page...

			... First the page header. */
			strcpy( cDynamicPage, webHTML_START );
			// ... Then the hit count... 
			strcat( cDynamicPage, cPageHits );
			strcat( cDynamicPage, "<p><pre>Task          State  Priority  Stack	#<br>************************************************<br>" );
			// ... Then the list of tasks and their status... 
			vTaskList( ( signed portCHAR * ) cDynamicPage + strlen( cDynamicPage ) );
		
			// ... Finally the page footer. 
			strcat( cDynamicPage, webHTML_END );

			// Write out the dynamically generated page. 
			netconn_write(pxNetCon, cDynamicPage, (u16_t)strlen( cDynamicPage ), NETCONN_COPY );
		}
 
		netbuf_delete( pxRxBuffer );
	}

	netconn_close( pxNetCon );
}


///////////////////////////////////////////////////////////////////////////////
// vProcessVSCPConnection
//

static void vProcessVSCPConnection( struct netconn *pxNetCon )
{
    struct netbuf *pxRxBuffer;
    uint8_t *pcRxString;
    uint16_t usLength;


	// We expect to immediately get data. 
	pxRxBuffer = netconn_recv( pxNetCon );

	if ( pxRxBuffer != NULL ) {
    	
		// Where is the data? 
		netbuf_data( pxRxBuffer, ( void * ) &pcRxString, &usLength );	   

	    // Write out the welcome message 
        netconn_write( pxNetCon, "Hello World\r\n", 13, NETCONN_COPY );
        netconn_write( pxNetCon, pcRxString, usLength, NETCONN_COPY );
 
		netbuf_delete( pxRxBuffer );
	}

    vTaskDelay( webSHORT_DELAY );
	netconn_close( pxNetCon );
}

///////////////////////////////////////////////////////////////////////////////
// taskHTTPListner
//

void taskHTTPListner( void *pvParameters )
{
    struct netconn *pxHTTPListener;     // Listens for HTTP connections
    struct netconn *pxNewConnection;
    
    // Create a new tcp connection handle for HTTP
    xSemaphoreTake( xSemaphore, portMAX_DELAY );
 	pxHTTPListener = netconn_new( NETCONN_TCP );
	netconn_bind( pxHTTPListener, NULL, webHTTP_PORT );
	netconn_listen( pxHTTPListener );
	
	
	// Loop forever 
	for( ;; ) {
    	
		// Wait for connection. 
		pxNewConnection = netconn_accept( pxHTTPListener );
		xSemaphoreGive( xSemaphore );

		if ( pxNewConnection != NULL ) {
		
			// Service connection. 
			vProcessConnection( pxNewConnection );
		
    		while( netconn_delete( pxNewConnection ) != ERR_OK ) {
				vTaskDelay( webSHORT_DELAY );
			}
		
		
		}
		
	}
}

///////////////////////////////////////////////////////////////////////////////
// taskVSCPListner
//

void taskVSCPListner( void *pvParameters )
{
    struct netconn *pxVSCPListener;     // Listens for VSCP connections
    struct netconn *pxNewConnection;
    
    // Create a new tcp connection handle for VSCP
    xSemaphoreTake( xSemaphore, portMAX_DELAY );
 	pxVSCPListener = netconn_new( NETCONN_TCP );
	netconn_bind( pxVSCPListener, NULL, VSCP_LEVEL2_TCP_PORT );
	netconn_listen( pxVSCPListener );
    
	// Loop forever 
	for( ;; ) {
    	
		// Wait for connection. 
		pxNewConnection = netconn_accept( pxVSCPListener );

  		if ( pxNewConnection != NULL ) {
      		xSemaphoreGive( xSemaphore );
		
			// Service connection. 
			vProcessVSCPConnection( pxNewConnection );
		
    		while( netconn_delete( pxNewConnection ) != ERR_OK ) {
				vTaskDelay( webSHORT_DELAY );
			}
		
		}
		
	}
}

///////////////////////////////////////////////////////////////////////////////
// taskWEBServer
//

void taskWEBServer( void *pvParameters )
{
    struct ip_addr xIpAddr, xNetMask, xGateway;
    extern err_t ethernetif_init( struct netif *netif );
    static struct netif EMAC_if;
    
    struct netconn *pxHTTPListener;     // Listens for HTTP connections
    struct netconn *pxVSCPListener;     // Listens for VSCP connections
    struct netconn *pxNewConnection;    

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
    // Note!!!!
    // This must be done after the scheduler has started
    // http://lwip.wikia.com/wiki/Initialization_using_tcpip.c
    netif_add( &EMAC_if, &xIpAddr, &xNetMask, &xGateway, NULL, ethernetif_init, tcpip_input );

	// make it the default interface 
    netif_set_default( &EMAC_if );    
    
    // bring it up 
    netif_set_up( &EMAC_if );
  
    xSemaphore = xSemaphoreCreateMutex();
       
    // Start the Web server task
    sys_thread_new( ( signed portCHAR *)"http", 
                        taskHTTPListner, 
                        NULL, 
                        300, 
                        WEBSERVER_PRIORITY );
                        
 /*                     
    // Start the VSCP server task
    sys_thread_new( ( signed portCHAR *)"vscp", 
                        taskVSCPListner, 
                        NULL, 
                        300, 
                        WEBSERVER_PRIORITY );                    
 */
    while( 1 ) {
        vTaskDelay( webSHORT_DELAY );
    }    
    
	
	// Create a new tcp connection handle for HTTP
 	pxHTTPListener = netconn_new( NETCONN_TCP );
	netconn_bind( pxHTTPListener, NULL, webHTTP_PORT );
	netconn_listen( pxHTTPListener );
	
	// Create a new tcp connection handle for VSCP
 	//pxVSCPListener = netconn_new( NETCONN_TCP );
	//netconn_bind( pxHTTPListener, NULL, VSCP_LEVEL2_TCP_PORT );
	//netconn_listen( pxVSCPListener );

	// Loop forever 
	for( ;; ) {
    	
		// Wait for connection. 
		pxNewConnection = netconn_accept( pxHTTPListener );

		if ( pxNewConnection != NULL ) {
		
			// Service connection. 
			vProcessConnection( pxNewConnection );
		
    		while( netconn_delete( pxNewConnection ) != ERR_OK ) {
				vTaskDelay( webSHORT_DELAY );
			}
		
		}
		
	}
	
}

