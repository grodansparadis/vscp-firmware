
#if 1

struct {
  unsigned long x:1;
  unsigned long y:31;
} p;

void ppp(void)
{
  p.x = 1;
  p.y = 1;
}

/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.1 2006/03/23 21:29:49 akhe Exp $
 *
 */


#include <stdlib.h>   /* For system(). */
#include <stdio.h>    /* For printf(). */

#undef HTONS

#include "cs8900a.h"
#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "httpd.h"

static const struct uip_eth_addr ethaddr = {{0x00,0x00,0xe2,0x58,0xb6,0x6b}};

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

static unsigned short start, current;

#define RT_CLOCK_SECOND 2048

/*-----------------------------------------------------------------------------------*/
/**
 * \internal
 * A real-time clock.
 *
 * This example main() function uses polling of a real-time clock in
 * order to know when the periodic processing should be
 * performed. This is implemented using this function - rt_ticks(). In
 * this example unix implementation, it simply calls the unix function
 * gettimeofday() which returns the current wall clock time.
 *
 * For a micro-controller, a simple way to implement this function is
 * by having a counter that is incremented by a timer interrupt and
 * read by this function.
 * 
 * The macro RT_CLOCK_SECOND should be defined as the approximate
 * number of ticks that are elapsed during one second. 
 */
extern unsigned short rt_ticks(void);

/*-----------------------------------------------------------------------------------*/
int
main(void)
{
  u8_t i, arptimer;
  u16_t addr[2];
  int z = 3;

  ppp();

  /* Initialize the uIP TCP/IP stack. */
  uip_init();
  uip_arp_init();
  
  /* Register the Ethernet MAC address with uIP. In reality, the MAC
     address should be obtained from the EEPROM of the network
     hardware before calling this function. */
//  uip_setethaddr(ethaddr);

  /* Setup the IP address and netmask. */
//  uip_ipaddr(addr, 192,168,0,30);
//  uip_sethostaddr(addr);
//  uip_ipaddr(addr, 255,255,255,0);
//  uip_setnetmask(addr);

  /* Configure the IP address of the default router. */
//  uip_ipaddr(addr, 192,168,0,1);
//  uip_setdraddr(addr);

  /* Initialize the device driver. */ 
  cs8900a_init();

  /* Initialize the HTTP server. */
  httpd_init();
  
  start = rt_ticks();
  arptimer = 0;
  
  while(1) {
    /* Let the tapdev network device driver read an entire IP packet
       into the uip_buf. If it returns > 0, there is a packet in the
       uip_buf buffer. */
    uip_len = cs8900a_poll();
    
    if(uip_len > 0) {
      /* A packet is present in the packet buffer. We call the
	 appropriate ARP functions depending on what kind of packet we
	 have received. If the packet is an IP packet, we should call
	 uip_input() as well. */
      if(BUF->type == htons(UIP_ETHTYPE_IP)) {
	uip_arp_ipin();
	uip_input();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  cs8900a_send();
	}
      } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
	uip_arp_arpin();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */	
	if(uip_len > 0) {	
	  cs8900a_send();
	}
      }
    } else {
      /* The poll function returned 0, so no packet was
	 received. Instead we check if there is time that we do the
	 periodic processing. */
      current = rt_ticks();
      
      if((u16_t)(current - start) >= (u16_t)RT_CLOCK_SECOND / 2) {
	start = current;
	for(i = 0; i < UIP_CONNS; i++) {
	  uip_periodic(i);
	  /* If the above function invocation resulted in data that
	     should be sent out on the network, the global variable
	     uip_len is set to a value > 0. */
	  if(uip_len > 0) {
	    uip_arp_out();
	    cs8900a_send();
	  }
	}

#if UIP_UDP
	for(i = 0; i < UIP_UDP_CONNS; i++) {
	  uip_udp_periodic(i);
	  /* If the above function invocation resulted in data that
	     should be sent out on the network, the global variable
	     uip_len is set to a value > 0. */
	  if(uip_len > 0) {
	    uip_arp_out();
	    tapdev_send();
	  }
	}
#endif /* UIP_UDP */
      
	/* Call the ARP timer function every 10 seconds. */
	if(++arptimer == 20) {	
	  uip_arp_timer();
	  arptimer = 0;
	}
      	
      }    
    }    
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
void
uip_log(char *m)
{
  // printf("uIP log message: %s\n", m);
}
/*-----------------------------------------------------------------------------------*/

#else

/*
 * Copyright (c) 2001-2003, Adam Dunkels.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.  
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.1 2006/03/23 21:29:49 akhe Exp $
 *
 */


#include "uip.h"
#include "uip_arp.h"
#include "cs8900a.h"
#include "httpd.h"
#include "telnetd.h"

#include <msp430x14x.h>

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

/*-----------------------------------------------------------------------------------*/
int
main(void)
{
  u8_t i, arptimer;

  /* Turn off the watchdog. */
  WDTCTL = WDTPW + WDTHOLD;

  /* Initialize the device driver. */ 
  cs8900a_init();

  /* Initialize the uIP TCP/IP stack. */
  uip_init();

  /* Initialize the HTTP server. */
  httpd_init();

  /* Initialise the telnet server. */
//  telnetd_init();
  
  arptimer = 0;
  
  while(1) {
    /* Let the tapdev network device driver read an entire IP packet
       into the uip_buf. If it must wait for more than 0.5 seconds, it
       will return with the return value 0. If so, we know that it is
       time to call upon the uip_periodic(). Otherwise, the tapdev has
       received an IP packet that is to be processed by uIP. */
    uip_len = cs8900a_poll();
    if(uip_len == 0) {
      for(i = 0; i < UIP_CONNS; i++) {
	uip_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  cs8900a_send();
	}
      }

#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  tapdev_send();
	}
      }
#endif /* UIP_UDP */
      
      /* Call the ARP timer function every 10 seconds. */
      if(++arptimer == 20) {	
	uip_arp_timer();
	arptimer = 0;
      }
      
    } else {
      if(BUF->type == HTONS(UIP_ETHTYPE_IP)) {
	uip_arp_ipin();
	uip_input();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */
	if(uip_len > 0) {
	  uip_arp_out();
	  cs8900a_send();
	}
      } else if(BUF->type == HTONS(UIP_ETHTYPE_ARP)) {
	uip_arp_arpin();
	/* If the above function invocation resulted in data that
	   should be sent out on the network, the global variable
	   uip_len is set to a value > 0. */	
	if(uip_len > 0) {	
	  cs8900a_send();
	}
      }
    }
    
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
void
uip_log(char *m)
{
//  debug_puts(m);
}
/*-----------------------------------------------------------------------------------*/

#endif
