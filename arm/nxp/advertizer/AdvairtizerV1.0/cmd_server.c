/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : cmd_server.c                                           *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *                  receive ofer udp port 85 and 86 commands and do staff  *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>

#define STATE_NORMAL 0
#define STATE_IAC    1
#define STATE_WILL   2
#define STATE_WONT   3
#define STATE_DO     4
#define STATE_DONT   5
#define STATE_CLOSE  6
U16 counter = 0;
/*---------------------------------------------------------------------------*/
void cmd_server_init(void){
  uip_listen(HTONS(85));
  uip_listen(HTONS(86));
}
/*---------------------------------------------------------------------------*/
static void newdata(void){
//debug_printf("new data !");
 if (uip_conn->lport == HTONS(85)){
 
        if (my_buf[0] == 'U'){
        uip_send("Up\r", 3);
        my_buf[0] = 0;
        }
        if (my_buf[0] == 'D'){
        uip_send("Down\r", 5);
        my_buf[0] = 0;
        }

    char *dataptr;
    dataptr = (char *)uip_appdata;
    if (*dataptr == 'B'){ 
//debug_printf("start_pic");     
counter = 0;
    }
  }

 if (uip_conn->lport == HTONS(86)){

 if (counter <= 0 ){
  start_pic();
  counter = 46464;
  }
  counter -= uip_datalen();
     send_pic_data();
  }
}
/*---------------------------------------------------------------------------*/
void cmd_server_appcall(void){

  if(uip_connected()) {
  }

  if(cmd_server_s.state == STATE_CLOSE) {
    cmd_server_s.state = STATE_NORMAL;
    uip_close();
    return;
  }
  
  if(uip_closed() || uip_aborted() || uip_timedout()) {
    closed();
  }
  
  if(uip_acked()) {
    acked();
  }
  
  if(uip_newdata()) {
    newdata();
  }
  
  if(uip_rexmit() || uip_newdata() || uip_acked() || uip_connected() || uip_poll()) {
 }
  
//    lcd_send_string("new data! ");
//    piep_ok();
    //senddata();
}
/*---------------------------------------------------------------------------*/
static void cmd_senddata(void){
    uip_send("Welcome!\n", 9);
    uip_close();
}
static void closed(void){
}
static void acked(void){
}
