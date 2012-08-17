 /*
 * Copyright (c) 2003, Adam Dunkels.
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
 * $Id: shell.c,v 1.1 2006/06/07 09:43:54 adam Exp $
 *
 */
 #include <global.h>
// #include <uip/uip.h>
// #include <shell.h>
// #include <string.h>
/*---------------------------------------------------------------------------*/
static void parse(register char *str, struct ptentry *t){
  struct ptentry *p;

   /* Loop over the parse table entries in t in order to find one that
   matches the str. */
  for(p = t; p->commandstr != NULL; ++p) {
    if(strncmp(p->commandstr, str, strlen(p->commandstr)) == 0) {
      break;
    }
  }
/* Did not find matching entry in parse table. We just call the
   efault handler supplied by the caller and return. */
  p->pfunc(str);
}
/*---------------------------------------------------------------------------*/
char inttostr(register char *str, unsigned int num){
   int k;
   char c, flag, *ostr;

   if (num < 0) {
      num = -num;
      *str++ = '-';
   }
   k = 10000;
   ostr = str;
   flag = 0;
   while (k) {
      c = num / k;
      if (c || k == 1 || flag) {
         num %= k;
         c += '0';
         *str++ = c;
         flag = 1;
      }
      k /= 10;
   }
   *str = '\0';
   return (U32)ostr;
}
/*---------------------------------------------------------------------------*/
 void help(char *str){
  shell_output("--------- Help -----------", "");
  shell_output("Available commands:", "");
  shell_output("stats   - show network statistics", "");
  shell_output("conn    - show TCP connections", "");
  shell_output("setup   - setup the Network Interface", "");
  shell_output("lcd     - LCD Utilities", "");
  shell_output("speaker - future staff", "");
  shell_output("help, ? - show help", "");
  shell_output("exit    - exit shell", "");
 shell_output("---------------------------", "");
  shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void unknown(char *str){
  if(strlen(str) > 0) {
    shell_output("Unknown command: ", str);
    piep_error();
  }
  shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void stats(char *str){
  strcpy (shell_pmt , "stats> ");
  shell_output("--------- Stats -----------", "");
  inttostr(str, uip_stat.ip.recv);
  shell_output("IP   packets received ", str);
  inttostr(str, uip_stat.ip.sent);
  shell_output("IP   packets sent     ", str);
  inttostr(str, uip_stat.ip.drop);
  shell_output("IP   packets dropped ", str);

  inttostr(str, uip_stat.icmp.recv);
  shell_output("ICMP packets received ", str);
  inttostr(str, uip_stat.icmp.sent);
  shell_output("ICMP packets sent     ", str);
  inttostr(str, uip_stat.icmp.drop);
  shell_output("ICMP packets dropped  ", str);

  inttostr(str, uip_stat.tcp.recv);
  shell_output("TCP  packets received  ", str);
  inttostr(str, uip_stat.tcp.sent);
  shell_output("TCP  packets sent     ", str);
  inttostr(str, uip_stat.tcp.drop);
  shell_output("TCP  packets dropped ", str);
  inttostr(str, uip_stat.tcp.rexmit);
  shell_output("TCP  packets retransmitted ", str);
  inttostr(str, uip_stat.tcp.synrst);
  shell_output("TCP  connection attempts ", str);
 shell_output("---------------------------", "");
 shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void conn(char *str){
    strcpy (shell_pmt , "/conn> ");
  shell_prompt(shell_pmt);

}
/*---------------------------------------------------------------------------*/
/*
  debug_printf("arp_update:\n" );
  debug_printf("IP: %d", (ipaddr[0])& 0x00ff );
  debug_printf(".%d", (ipaddr[0]>>8)& 0x00ff );
  debug_printf(".%d", (ipaddr[1])& 0x00ff );
  debug_printf(".%d", (ipaddr[1]>>8)& 0x00ff);
  debug_printf(" MAC: %#x", ethaddr->addr[0]);
  debug_printf(":%#x", ethaddr->addr[1]);
  debug_printf(":%#x", ethaddr->addr[2]);
  debug_printf(":%#x", ethaddr->addr[3]);
  debug_printf(":%#x", ethaddr->addr[4]);
  debug_printf(":%#x\n", ethaddr->addr[5]);
  */
 void setup(char *str){
 parsetab = &setup_parsetab;
 strcpy (shell_pmt , "/setup> ");
 shell_output("--------- Setup -----------", "");
 shell_output("Type '?' and return for help", "");
 uip_ipaddr_t ipaddr; 
 uip_gethostaddr(ipaddr);
 inttostr(str, ipaddr[0]&0x00ff);
 shell_output2("IP adress ", str);
 inttostr(str, ipaddr[0]>>8&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]>>8&0x00ff);
 shell_output(".", str);

 uip_getnetmask(ipaddr);
 inttostr(str, ipaddr[0]&0x00ff);
 shell_output2("Subnetmask ", str);
 inttostr(str, ipaddr[0]>>8&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]>>8&0x00ff);
 shell_output(".", str);

 uip_getdraddr(ipaddr);
 inttostr(str, ipaddr[0]&0x00ff);
 shell_output2("Router ", str);
 inttostr(str, ipaddr[0]>>8&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]>>8&0x00ff);
 shell_output(".", str);
//nicGetMacAddress(U8 pointer);
 shell_output("---------------------------", "");
 shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void setup_help(char *str){
 shell_output("--------- Help ------------", "");
 shell_output("Available commands:", "");
 shell_output("i - ", "change IP adress");
 shell_output("m - ", "change MAC adress");
 shell_output("s - ", "change Subnetmask");
 shell_output("r - ", "change Router");
 shell_output("f_U - ", "firmware update");
 shell_output("fv - ", "firmware version");
 shell_output("R_R - ", "reboot");
 shell_output("x - ", "exit setup");
 shell_output("---------------------------", "");
 shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void setup_R_R(char *str){
 shell_prompt("rebooting.... ");
}
/*---------------------------------------------------------------------------*/
 void setup_i(char *str){
 //shell_prompt("new IP (192.168.0.1) : ");
 uip_ipaddr_t ipaddr; 
 uip_gethostaddr(ipaddr);
 inttostr(str, ipaddr[0]&0x00ff);
 shell_output2("old IP (", str);
 inttostr(str, ipaddr[0]>>8&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]>>8&0x00ff);
 shell_output2(".", str);
 shell_output2(")",": ");
}
/*---------------------------------------------------------------------------*/
 void setup_m(char *str){
//  shell_prompt("new MAC (00:ef:fe:00:00:00) :  ");

//  register struct uip_eth_addr *ethaddr;
//  nicGetMacAddress(ethaddr);
//  inttostr(str, ethaddr->addr[0]);
  shell_output2("old MAC (", str);
//  inttostr(str, ethaddr->addr[1]);
//  shell_output2(":", str); 
//  inttostr(str, ethaddr->addr[2]);
//  shell_output2(":", str);
//  inttostr(str, ethaddr->addr[3]);
//  shell_output2(":", str);
//  inttostr(str, ethaddr->addr[4]);
//  shell_output2(":", str);
//  inttostr(str, ethaddr->addr[5]);

  shell_output2(")",": ");
}
/*---------------------------------------------------------------------------*/
 void setup_s(char *str){
//  shell_prompt("new Subnetmask (255.255.255.0) :  ");
 uip_ipaddr_t ipaddr; 
 uip_getnetmask(ipaddr);
 inttostr(str, ipaddr[0]&0x00ff);
 shell_output2("old Subnetmask (", str);
 inttostr(str, ipaddr[0]>>8&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]>>8&0x00ff);
 shell_output2(".", str);
 shell_output2(")",": ");
}
/*---------------------------------------------------------------------------*/
 void setup_r(char *str){
  //shell_prompt("new default Gateway (192.168.0.254) :  ");
 uip_ipaddr_t ipaddr; 
 uip_getdraddr(ipaddr);
 inttostr(str, ipaddr[0]&0x00ff);
 shell_output2("old Gateway (", str);
 inttostr(str, ipaddr[0]>>8&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]&0x00ff);
 shell_output2(".", str);
 inttostr(str, ipaddr[1]>>8&0x00ff);
 shell_output2(".", str);
 shell_output2(")",": ");
}
/*---------------------------------------------------------------------------*/
 void lcd(char *str){
   strcpy (shell_pmt , "/lcd> ");
  shell_prompt(shell_pmt);
  send_pic_dir();
}
/*---------------------------------------------------------------------------*/
 void speaker(char *str){
   strcpy (shell_pmt , "/speaker> ");
  shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void shell_exit(char *str){
 shell_output("good by", "");
 shell_quit("");
}
/*---------------------------------------------------------------------------*/
 void shell_init(char *str){
  parsetab = &root_parsetab;
  strcpy (shell_pmt , "ADV 1.0> ");
  shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void shell_start(void){
  lcd_send_string("Telenet login !");
  piep_error();
  shell_output("Advairtizer command shell", "");
  shell_output("Type '?' and return for help", "");
  shell_prompt(shell_pmt);
}
/*---------------------------------------------------------------------------*/
 void shell_input(char *cmd){
//debug_printf("input: ", cmd);
  parse(cmd, parsetab);
}
/*---------------------------------------------------------------------------*/
