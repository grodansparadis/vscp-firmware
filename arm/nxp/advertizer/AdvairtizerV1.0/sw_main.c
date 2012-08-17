/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : sw_main.c                                              *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/
#include <global.h>
#include <uip/uip_arp.h>
#include <uip/timer.h>

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
/***************************************************************************
 * low init
 * set PLL multiplier & divisor.
 * values computed from config.h
 ***************************************************************************/
void lowInit(void){

PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL;
// enable PLL
PLLCON = PLLCON_PLLE;
PLLFEED = PLL_FEED1; // Make it happen. These two updates
PLLFEED = PLL_FEED2; // MUST occur in sequence.
// wait for PLL lock
while (!(PLLSTAT & PLLSTAT_LOCK)) continue;// enable & connect PLL
PLLCON = PLLCON_PLLE | PLLCON_PLLC;
PLLFEED = PLL_FEED1; // Make it happen. These two updates
PLLFEED = PLL_FEED2; // MUST occur in sequence.

//Watchdog
WDMOD |= WDRESET;         //Watchdog Reset Enable bit. if watchdog counter underflow will reset the microcontroller. 
WDMOD |= WDEN;            //WDEN Watchdog interrupt Enable bit 
WDMOD &= ~WDTOF;          //clear WDTOF Bit 
RSIR |= BODR;             //clear BODR Bit 
//WDTC    = 0xFFFFFFFF;     //The WDTC register determines the time-out value (32Bit)
WDTC    = 0x0BEBC200;     //The WDTC register determines the time-out value (32Bit)
WDFEED  = 0xAA;           //reloads the Watchdog timer to its preset value
WDFEED  = 0x55;           //reloads the Watchdog timer to its preset value and start it

//default Pinsels
PINSEL0 = 0x803A1555;
PINSEL1 = 0x250844A8;     // A50844A8 if USB Connect will be used
PINSEL2 = 0x00000004;     // set Debug Port on, Trace Port off & P1.16-1.25 for GPIO
PCONP   = 0x801817BE;     // Periferie Power UP

// setup the parallel port pin
SCS = BIT0 | BIT1;          // Highspeed GPIO is enabled
F_IO0CLR = PIO0_ZERO_BITS;  // clear the ZEROs output
F_IO0SET = PIO0_ONE_BITS;   // set the ONEs output
F_IO0DIR = PIO0_OUTPUT_BITS;// set the output bit direction
F_IO1CLR = PIO1_ZERO_BITS;  // clear the ZEROs output
F_IO1SET = PIO1_ONE_BITS;   // set the ONEs output
F_IO1DIR = PIO1_OUTPUT_BITS;// set the output bit direction

// setup & enable the MAM
MAMTIM = MAMTIM_CYCLES;
MAMCR = MAMCR_FULL;
// set the peripheral bus speed
// value computed from config.h
VPBDIV = VPBDIV_VALUE; // set the peripheral bus clock speed
pclk = liblpc2000_get_pclk(liblpc2000_get_cclk(OSCILLATOR_CLOCK_FREQUENCY));
}
/***************************************************************************
 * init main pwm counter (PWMMR0)
 * this is the Master PWM (Maximum FRQ for all the other PWM's)
 ***************************************************************************/
void init_pwm(void){
	PWMTCR = PWMCR_RESET;  // reset PWM counter
	PWMPR  = PWM_PCLK_DIV; // set PWM Prescaler
	PWMMR0 = PWM_MR0INIT;  // set Match0 (for the Single Edge PWM-Channels)
	PWMLER |= (1<<0);      // Enable PWM Match0 Latch
	PWMMCR |= (1<<1);      // reset PWM Timer on PWMMR0 match
}
/***************************************************************************
 * init uIP
 ***************************************************************************/
void init_uip(void){
  timer_set(&periodic_timer, HALF_SEC );// 0.5 sec
  timer_set(&arp_timer, TEN_SEC);       // 10 sec

        uip_init();
        uip_ipaddr_t ipaddr; 
        uip_ipaddr(ipaddr, 10,0,0,77);
  	uip_sethostaddr(ipaddr);        // Set the IP address of this host
  	uip_ipaddr(ipaddr, 10,0,0,254);
  	uip_setdraddr(ipaddr);          //Set the default router's IP address
  	uip_ipaddr(ipaddr, 255,0,0,0);
  	uip_setnetmask(ipaddr);

        uip_arp_init();                 // init ARP cache
        uip_router_init();
}
/***************************************************************************
 * init
 ***************************************************************************/
void init(void){
        lowInit();                                //
        init_pwm();                               //
        init_rtc();                               //
        init_SysTime();                           //
//	init_ldr_sensor();                        //
	init_speaker();                           //
	init_uart();                              //
        i2c_lpc_init(I2C_SPEED_400);              //
	enc28j60Init();                           //Ethernet Chip
        init_uip();                               //
        init_joystick();                          //to improve
//      init_rotary_encoder();                    //to improve
        lcd_init();                               //LCD Display Init
        send_pic("images/logo.bmp");
        init_ir_sensor();
        init_ir_transmitter();
        enable_RS232();
        ctl_global_interrupts_enable();
}

/**********************************************************************
* Powersave Mode & Wakeup
* Mode 1 = minimal power needed (no Ethernet, no LCD,  )
* Mode 2 = only ... shutdown
* Mode 3 .....
***********************************************************************/
bool powersave(U8 mode){
  uart0Puts("powersave: ");
  bool result = false;
  ctl_global_interrupts_disable();// Disable INTs    
    switch(mode){
      case 1:                             // Powersave Mode 1
        uart0Puts("mode 1\n");
        lcd_powersave(1);
        enc28j60_powersave(1);
        disable_RS232();
        disable_RS485();
        F_IO0SET = USB_CONNECT;           //if USB connected the PC disconnect the USB device (Enumeration, USB_Connect)
        F_IO1SET |= HB_LED;               // Heartbeat LED OFF
        result = true;
        break;
      case 2:                             // Powersave Mode 2
        uart0Puts("mode 2\n");
        enc28j60_powersave(1);
        F_IO0SET = USB_CONNECT;           //if USB connected the PC disconnect the USB device (Enumeration, USB_Connect)
        result = true;
        break;
    }
                                        // program the flash to Backup Location
//      FlashMainData();                // long call to IAP in ram complete shut down
      AD0_CR = AD0_CR&(~BIT(21));       // * clr PDN bit *
      AD1_CR = AD1_CR&(~BIT(21));       // * clr PDN bit *

//***********************************************************************
// wakup for uart, receiver must enabled and pinsel must set
// you cant use both RS232/RS485 
      enable_RS232();
//      enable_RS485();
      PINSEL0 = (U32)(0x03<<2);        // Connect P0.1 (uart0) (RS232 RX / RS485 RX) to eint0
      INTWAKE |= EXTWAKE0;             //Wakup if RXDATA (RS232 / RS485) ocur
//***********************************************************************
//    INTWAKE |= EXTWAKE1;             //used for SDA, not avail for Wakup
//***********************************************************************
// wakup for ethernet
//    PINSEL0 = (U32)(0x02<<30);       // Connect P0.15 to eint2
//    INTWAKE |= EXTWAKE2;             //Wakup if Ethernet interrupt ocur
//***********************************************************************
// wakup for IR Receiver
// dont work at the moment becurse IR receiver send a puls if powerdown ocur and
// the powerup inits
//     PINSEL1 = (U32)(0x02<<28);      // Connect P0.30 (IR_SENSOR) to eint3
//     INTWAKE |= EXTWAKE3;            //Wakeup if IR Receive interrupt ocur
//***********************************************************************
// wakup for USB not tested
//      INTWAKE |= USBWAKE
//***********************************************************************
// wakup for Brown out detection not tested
//      INTWAKE |= BODWAKE
//***********************************************************************
// wakup for Realtime Clock not tested
//      INTWAKE |= RTCWAKE
//***********************************************************************
     EXTINT |= 0x0F;                // clear all pending wakeup interrupts
     PCONP = 0x00;                  // * TURN OFF ALL PERFs * 0x80000200 Turn On USB PCLK and enable RTC
 
// if USB Wakeup Interrupt will be used
//    PLLCFG = 0x23;                    // M = 4, P = 2 48MHz 6.5mAh
//    PLLCON = PLLE ;                   // PLL enable
//    PLLFEED = 0xAA;                   // Feed Sequence 1
//    PLLFEED = 0x55;                   // Feed Sequence 2
//    while ((PLLSTAT & PLOCK) == 0);   // Wait for PLL Lock
//    PLLCON = PLLE | PLLC;             // PLL Enable & Connect
//    PLLFEED = 0xAA;                   // Feed Sequence 1
//    PLLFEED = 0x55;                   // Feed Sequence 2

  PCON = BIT(1)|BIT(2)|BIT(3)|BIT(4);   // * POWER DOWN *
  while ((PCON&BIT(1))!=0) ;
  // ==>> POWER IS NOW OFF <<== Hi->Lo Edge on EINT3 restarts
  // ....
  //sleep here until interrupt wakeup ocur  
  // ....
  //Power up rutine starts here
    PCONP   = 0x801817BE;               // Periferie Power UP
    PINSEL0 = 0x803A1555;
    PINSEL1 = 0x250844A8;               //A50844A8 if USB Connect will be used
    PINSEL2 = 0x00000004;               //set Debug Port on, Trace Port off & P1.16-1.25 for GPIO
    PLLCFG = PLLCFG_MSEL | PLLCFG_PSEL; // PLL Configuration Register
    PLLCON = PLLCON_PLLE;               // Enable PLL
    PLLFEED = 0xAA;                     // Make it happen. These two updates
    PLLFEED = 0x55;                     // MUST occur in sequence.
    while (!(PLLSTAT & PLLSTAT_LOCK)) continue; // wait for PLL lock
    PLLCON = PLLCON_PLLE | PLLCON_PLLC; // enable & connect
    PLLFEED = 0xAA;                     // Make it happen. These two updates
    PLLFEED = 0x55;                     // MUST occur in sequence.
    EXTINT |= 0x0F;                      //clear all pending wakeup interrupts
    AD0_CR= AD0_CR|BIT(21);             // set PDN bit 
    AD1_CR= AD1_CR|BIT(21);             // set PDN bit 
    enc28j60_powersave(0);              // wakeup Ethernet
    lcd_powersave(0);                   // wakeup lcd
    uart0Puts("wakeup !\n");
  ctl_global_interrupts_enable();// Enable INTs
  return result;
}

/**********************************************************************
* PCB Test Routine
* Test all the HW
***********************************************************************/
void test_hw(void){
    uart0Puts("Test Uart 0 \n\r");    //  115K
    pause(ONE_SEC);
    F_IO0SET = RELAIS1;
    F_IO1SET = IO1;
    F_IO0SET = USB_CONNECT;           //if USB connected the PC connect and disconnect the USB device (Enumeration, USB_Connect)
    F_IO0SET = IR_PWM;
    
    pause(ONE_SEC);
    F_IO0CLR = RELAIS1;
    F_IO1CLR = IO1;
    F_IO0CLR = USB_CONNECT;           //if USB connected the PC connect and disconnect the USB device (Enumeration, USB_Connect)
    F_IO0CLR = IR_PWM;
    glcdctrl_blink(5);                //Backlight blinkt 5 mal
    pause(ONE_SEC);
    fill_screen(0xffff);              //white
    fill_screen(0x0000);              //black
    fill_screen(0x001f);              //blue
    fill_screen(0xe07e);              //green
    fill_screen(0xf800);              //red
    pause(ONE_SEC);
// IR led , send some code
//    send_rc5(true, 0, 10);
    piep_ok();
}
/**********************************************************************
* main
***********************************************************************/
int main(void){
  init();

//      RTC_SetDOW(5);
//      RTC_SetDate(26,10,2007);
//      RTC_SetTime(12,10,00);
	piep_ok();
        uart0Puts("ADV V1.0\n\r");
while(1){               //main while
//*************************************
// Brownout detection BODR
// This bit is set when the 3.3 V power reaches a level below 2.6 V.
//*************************************
if (RSIR & BODR & (!POR)){            //if BODR is detected
    glcdctrl_blink(5);                //Backlight blink 5 times
    pause(ONE_SEC);
    fill_screen(0xf800);              //red
    lcd_send_string("Error 2,6V !");
    RSIR |= BODR;                     //clear BODR Bit 
  }
//*************************************
// WDTOF Watchdog Time-Out Flag.
//*************************************
if (WDMOD & WDTOF){
    WDMOD &= ~WDTOF;                            //clear WDTOF Bit
    WDMOD |= WDRESET;                           //Watchdog Reset Enable bit. if watchdog counter underflow will reset the microcontroller. 
    WDMOD |= WDEN;                              //WDEN Watchdog interrupt Enable bit
    int en = ctl_global_interrupts_disable();
    WDFEED  = 0xAA;                             //reloads the Watchdog timer to its preset value
    WDFEED  = 0x55;                             //reloads the Watchdog timer to its preset value
    ctl_global_interrupts_set(en);
    glcdctrl_blink(5);                          //Backlight blink 5 times
    pause(ONE_SEC);
    fill_screen(0xf800);                        //red
    lcd_send_string("Error WatchDog !");
  }

//*************************************
// update Watchdog Timer
// must be in main while()
    int en = ctl_global_interrupts_disable();
    WDFEED  = 0xAA;                             //reloads the Watchdog timer to its preset value
    WDFEED  = 0x55;                             //reloads the Watchdog timer to its preset value
    ctl_global_interrupts_set(en);

//*************************************
//* Routine for uip
//* ARP, UDP, TCP
//*************************************

                        if(timer_expired(&periodic_timer)) {
                          timer_reset(&periodic_timer);
                          for(U8 i = 0; i < UIP_CONNS; i++) {
                             uip_periodic(i);
                                   // If the above function invocation resulted in data that
                                   // should be sent out on the network, the global variable
                                   // uip_len is set to a value > 0.
                              if(uip_len > 0) {
                                uip_arp_out();
                                dev_send();
                              }//endif
                          }//endfor

                          #if UIP_UDP
                                  for(U8 i = 0; i < UIP_UDP_CONNS; i++) {
                                    uip_udp_periodic(i);
                                    // If the above function invocation resulted in data that
                                    //   should be sent out on the network, the global variable
                                    //   uip_len is set to a value > 0. 
                                    if(uip_len > 0) {
                                      uip_arp_out();
                                      dev_send();
                                    }//endif
                                  }//endfor
                          #endif // UIP_UDP                                   
                                  // Call the ARP timer function every 10 seconds. 
                                  if(timer_expired(&arp_timer)) {
                                    timer_reset(&arp_timer);
                                    uip_arp_timer();
                                  }//endif
                    }//endelseif

//************************************************************
//* joystick routine has to improved
//************************************************************
        if (get_stick_up() == 1){
        piep_ok();
        lcd_send_string("up");
        uart0Puts("UP");
	}
        if (get_stick_down() == 1){
        piep_ok();
        lcd_send_string("down");
        uart0Puts("Down");

	}
        if (get_stick_left() == 1){
        piep_ok();
        lcd_send_string("left");
        lcd_send_string("powersave 2");
        uart0Puts("Left");
        powersave(2);
	}
        if (get_stick_right() == 1){
        piep_ok();
        lcd_send_string("right");
        uart0Puts("Right");
        lcd_send_string("powersave 1");
        powersave(1);
	}
        if (get_stick_button() == 1){
        piep_ok();
        lcd_send_string("button");
        uart0Puts("Button");
        lcd_send_string("test Hardware");
        test_hw();
	}

/******************************************
* RC5 routine has to improved
******************************************/
   if (rc5.toggle != 0){                    //if toggle bit wars set
      piep_ok();
                                            //output
        char string[25];
        uart0Puts( "\n\r ** Toggle-Bit ** ");
        itoa (rc5.toggle, string, 10);
        uart0Puts(string);
        uart0Puts( "\n\r ** device ** ");
        itoa (rc5.addr, string, 10);
        uart0Puts(string);
        uart0Puts( "\n\r ** CMD ** ");
        itoa (rc5.code, string, 10);
        uart0Puts(string);
        uart0Puts( "\n\r");
        rc5.toggle = 0;
    if (rc5.code == 12)                     //power off received
    powersave(1);
   }
   else{
   // No RC5 code received, do somthing else with it
   }//RC5 end
/***************************************************************************
* put user code below
/***************************************************************************/

  




/***************************************************************************
* end user code
/***************************************************************************/

  }//end Main while
  return 0;

/***************************************************************************
 * The undef_handler is called  when the CPU executes an undefined instruction.
 * I'm not sure what you can do in this situation other than wait for the
 * watchdog to expire. This may work in ARM mode without optimisation applied.
 *
 * undef_handler - This is the default undefined instruction exception handler.
 * swi_handler - This is the default software interrupt exception handler.
 * pabort_handler - This is the default prefetch abort exception handler.
 * dabort_handler - This is the default data abort exception handler.
 * irq_handler - This is the default IRQ exception handler.
 * fiq_handler - This is the default FIQ exception handler.
 ***************************************************************************/
void __attribute__((interrupt("UNDEF"))) undef_handler(void){

}
void __attribute__((interrupt("UNDEF"))) swi_handler(void){

}
void __attribute__((interrupt("UNDEF"))) pabort_handler(void){

}  
void __attribute__((interrupt("UNDEF"))) dabort_handler(void){

}
void __attribute__((interrupt("UNDEF"))) fiq_handler(void){

}
void __attribute__((interrupt("UNDEF"))) irq_handler(void){

}
void __attribute__ ((interrupt)) undefined_instruction_exception()
{
//  DebugPutString("undefined_instruction_exception\r\n");
}
void __attribute__ ((interrupt)) software_interrupt_exception()
{
  // DebugPutString("I");
}
void __attribute__ ((interrupt)) prefetch_abort_exception()
{
//  DebugPutString("prefetch_abort_exception\r\n");
}
void __attribute__ ((interrupt)) data_abort_exception()
{
//  DebugPutString("data_abort_exception\r\n");
}
void __attribute__ ((interrupt)) reserved_exception()
{
//  DebugPutString("reserved_exception\r\n");
}
void __attribute__ ((interrupt)) interrupt_exception()
{
//  DebugPutString("interrupt_exception\r\n");
}
void __attribute__ ((interrupt)) fast_interrupt_exception()
{
//  DebugPutString("fast_interrupt_exception\r\n");
}

}//end main


