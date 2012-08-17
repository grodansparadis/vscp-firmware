/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : uip_app_router.h                                       *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *                  routes the incomming receive app_call to udp or tcp    *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef __UIP_APP_ROUTER_H__
#define __UIP_APP_ROUTER_H__

#include <uip/uipopt.h>

  struct router_state {
    u8_t state;
  }__attribute__((packed)) uip_router_s;

#ifdef   UIP_APPCALL                                                    /* If UIP_APPCALL is already defined from another uIP App   */    
#undef   UIP_APPCALL                                                    /* then undefine it and force uIP apps to register          */
#endif

#ifndef UIP_APPCALL
#define UIP_APPCALL     uip_router_tcp_appcall
#endif

#ifdef   UIP_UDP_APPCALL                                                /* If UIP_UDP_APPCALL is already defined from an uIP App    */    
#undef   UIP_UDP_APPCALL                                                /* then undefine it and redefine it as specified below      */
#endif

#ifndef UIP_UDP_APPCALL
//#define UIP_UDP_APPCALL     uip_router_tcp_appcall
#define UIP_UDP_APPCALL     uip_router_udp_appcall
#endif

/* Finally we define the application function to be called by uIP. */
void uip_router_tcp_appcall(void);
void uip_router_udp_appcall(void);

void uip_router_init(void);
#endif /* __TELNETD_H__ */
