/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : cmd_server.h                                           *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef __CMD_SERVER_H__
#define __CMD_SERVER_H__

#include <uip/uipopt.h>

void cmd_server_appcall(void);

struct cmd_server_state {
  u8_t state;
}__attribute__((packed)) cmd_server_s;

U8 my_buf[10];

//#ifndef UIP_APPCALL
//#define UIP_APPCALL     cmd_server_appcall
//#endif

void cmd_server_init(void);
void cmd_server_appcall(void);
static void newdata(void);
static void cmd_senddata(void);
static void closed(void);
static void acked(void);
#endif /* __CMD_SERVER_H__ */
