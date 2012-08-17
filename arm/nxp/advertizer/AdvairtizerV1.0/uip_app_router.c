/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : uip_app_router.c                                       *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *                  routes the incomming receive app_call to udp or tcp    *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>
//#include <uip/uip.h>

void uip_router_init(void){
  telnetd_init();
  cmd_server_init();
}

void uip_router_udp_appcall(void){
//add here your udp connection application
   switch(uip_conn->lport) {
   case HTONS(85):
      cmd_server_appcall();
      break;
   case HTONS(86):
      cmd_server_appcall();
      break;
   }
}

void uip_router_tcp_appcall(void){
//add here your tcp connection application
   switch(uip_conn->lport) {
   case HTONS(23):
      telnetd_appcall();
      break;
   }
}
