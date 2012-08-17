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
#include <uip/uip.h>
#include <uip/uip_arp.h>
#include <uip/timer.h>

U08 Enc28j60Bank;
U16 NextPacketPtr;
#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

uint16_t dev_poll(void)
{
	return enc28j60PacketReceive(UIP_BUFSIZE, uip_buf);
}

void dev_send(void)
{
	enc28j60PacketSend(uip_len, uip_buf);
}

void nicGetMacAddress(U08 *macaddr)
{
    // read MAC address registers
	// NOTE: MAC address in ENC28J60 is byte-backward
    *macaddr++ = enc28j60Read(MAADR5);
    *macaddr++ = enc28j60Read(MAADR4);
    *macaddr++ = enc28j60Read(MAADR3);
    *macaddr++ = enc28j60Read(MAADR2);
    *macaddr++ = enc28j60Read(MAADR1);
    *macaddr++ = enc28j60Read(MAADR0);
}

void nicSetMacAddress(U8 *macaddr){
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
	enc28j60Write(MAADR5, *macaddr++);
	enc28j60Write(MAADR4, *macaddr++);
	enc28j60Write(MAADR3, *macaddr++);
	enc28j60Write(MAADR2, *macaddr++);
	enc28j60Write(MAADR1, *macaddr++);
	enc28j60Write(MAADR0, *macaddr++);
}

U08 enc28j60ReadOp(U08 op, U08 address)
{

    U08 data;
    // release CS
    clrb_CS_ETHERNET();
//   	pause(ONE_US*100);
    
    SPI0_SPDR = op | (address & ADDR_MASK);
    while(!(SPI0_SPSR & SPI0_SPIF)){};
    SPI0_SPDR = 0x00;
    while(!(SPI0_SPSR & SPI0_SPIF)){};
    // do dummy read if needed
	if(address & 0x80)
	{
		SPI0_SPDR = 0x00;
		while(!(SPI0_SPSR & SPI0_SPIF)){};
	}
	data = SPI0_SPDR; 
    
    setb_CS_ETHERNET(); //CS auf High
	return data;
}

void enc28j60WriteOp(U08 op, U08 address, U08 data)
{
    // release CS
    clrb_CS_ETHERNET();
//   	pause(ONE_US*100);
    // issue write command
	SPI0_SPDR = op | (address & ADDR_MASK);
	while(!(SPI0_SPSR & SPI0_SPIF)){};
	// write data
	SPI0_SPDR = data;
	while(!(SPI0_SPSR & SPI0_SPIF)){};
    // release CS
    setb_CS_ETHERNET(); //CS auf High
}

void enc28j60ReadBuffer(U16 len, U8 *data)
{
	// assert CS
        clrb_CS_ETHERNET();
//   	pause(ONE_US*100);
	// issue read command
	SPI0_SPDR = ENC28J60_READ_BUF_MEM;
	while(!(SPI0_SPSR & SPI0_SPIF)){};
	while(len--)
	{
		// read data
		SPI0_SPDR = 0x00;
		while(!(SPI0_SPSR & SPI0_SPIF)){};
		*data++ = SPI0_SPDR;
	}	
	// release CS
	setb_CS_ETHERNET();
}

void enc28j60WriteBuffer(U16 len, U08* data)
{
	// assert CS
	clrb_CS_ETHERNET(); //CS auf Low
//   	pause(ONE_US*100);
	// issue write command
	SPI0_SPDR = ENC28J60_WRITE_BUF_MEM;
	while(!(SPI0_SPSR & SPI0_SPIF)){};
	while(len--)
	{
		// write data
		SPI0_SPDR = *data++;
		while(!(SPI0_SPSR & SPI0_SPIF)){};
	}	
	// release CS
	setb_CS_ETHERNET();
}

void enc28j60SetBank(U08 address)
{
	// set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank)
	{
		// set the bank
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		Enc28j60Bank = (address & BANK_MASK);
	}
}

U08 enc28j60Read(U08 address)
{
	// set the bank
	enc28j60SetBank(address);
	// do the read
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}

void enc28j60Write(U08 address, U08 data)
{
	// set the bank
	enc28j60SetBank(address);
	// do the write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

void enc28j60PhyWrite(U08 address, U16 data)
{
	// set the PHY register address
	enc28j60Write(MIREGADR, address);
	
	// write the PHY data
	enc28j60Write(MIWRL, data);	
	enc28j60Write(MIWRH, data>>8);

	// wait until the PHY write completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY);
}
void setb_CS_ETHERNET(void){
  F_IO0SET = CS_ETHERNET;
}
void clrb_CS_ETHERNET(void){
  F_IO0CLR = CS_ETHERNET;
}
void spi_init(void){
	// SPI init
	PINSEL0 |= SPI0_IOSET_MASK;
	SPI0_SPCR = SPI0_MSTR; //Enable SPI, SPI in Master Mode	
        SPI0_SPCCR = 8;        //This register controls the frequency of a master?s SCK.
                               // 8=7,5Mhz, 10=6Mhz, 12=5Mhz, must>8 !
}

void enc28j60Init(void)
{
    U32 a;
    U32 timeout;
    // interrupt und wol enable an LPC
    PINSEL0 |= ((U32)(0x02<<28)|(0x02<<30));
    spi_init();
    
//    do{
        timeout = 0;
        // perform system reset
        enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
        // check CLKRDY bit to see if reset is complete
	// Errata workaround #2, CLKRDY check is unreliable, delay 1 mS instead
	pause(ONE_SEC*5);
//        for(a=0;a<1000000;a++);
        while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY)){
            timeout++;
            if (timeout > 100000)
//                debug_printf("enc28j60Read timeout");
                break;
            };
//        }while(timeout > 100000);
	// do bank 0 stuff
	// initialize receive buffer
	// 16-bit transfers, must write low byte first
	// set receive buffer start address
	NextPacketPtr = RXSTART_INIT;
	enc28j60Write(ERXSTL, RXSTART_INIT&0xFF);
	enc28j60Write(ERXSTH, RXSTART_INIT>>8);
	// set receive pointer address
	enc28j60Write(ERXRDPTL, RXSTART_INIT&0xFF);
	enc28j60Write(ERXRDPTH, RXSTART_INIT>>8);
	// set receive buffer end
	// ERXND defaults to 0x1FFF (end of ram)
	enc28j60Write(ERXNDL, RXSTOP_INIT&0xFF);
	enc28j60Write(ERXNDH, RXSTOP_INIT>>8);
	// set transmit buffer start
	// ETXST defaults to 0x0000 (beginnging of ram)
	enc28j60Write(ETXSTL, TXSTART_INIT&0xFF);
	enc28j60Write(ETXSTH, TXSTART_INIT>>8);

	// do bank 2 stuff
	// enable MAC receive
	enc28j60Write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
	// bring MAC out of reset
	enc28j60Write(MACON2, 0x00);
	// enable automatic padding and CRC operations
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN);

	// set inter-frame gap (non-back-to-back)
	enc28j60Write(MAIPGL, 0x12);
	enc28j60Write(MAIPGH, 0x0C);
	// set inter-frame gap (back-to-back)
	enc28j60Write(MABBIPG, 0x12);
	// Set the maximum packet size which the controller will accept
	enc28j60Write(MAMXFLL, MAX_FRAMELEN&0xFF);	
	enc28j60Write(MAMXFLH, MAX_FRAMELEN>>8);

	// do bank 3 stuff
	// write MAC address
	// NOTE: MAC address in ENC28J60 is byte-backward
        // set Mac Addr.

        volatile unsigned char mymac[6] = {0x00,0x04,0x0e,0xf8,0xb7,0xf6};
//        U8 mymac[6] = {0x00,0x04,0x0e,0xf8,0xb7,0xf6};
        nicSetMacAddress(mymac);

	// no loopback of transmitted frames
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);
        // enable interrutps
        // EIE: ETHERNET INTERRUPT ENABLE REGISTER
        // PKTIE: Receive Packet Pending Interrupt Enable bit
        // INTIE: Global INT Interrupt Enable bit
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
        enc28j60Write(ERXFCON,(CRCEN|ANDOR));//Bank1 
        enc28j60PhyWrite(PHLCON, 0x472);//Display link status and transmit/receive activity (always stretched)
        ctl_set_isr(SIR_EINT2, PRIO_INT_ENC28J60, CTL_ISR_TRIGGER_NEGATIVE_EDGE, eint2ISR, 0);// eint2ISR registrieren
        ctl_unmask_isr(SIR_EINT2);
}
/************************************************
 * enc28j60 Powersave Mode
 * mode 1 = powerdown
 * mode 0 = wakeup
 * usage:
 * bool result enc28j60_powersave(1);
 ************************************************/
bool enc28j60_powersave(U8 mode){
  bool result = false;
  switch(mode){
  case 1:
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_RXEN);
        U32 timeout = 0;
        while(!(enc28j60Read(ESTAT) & ESTAT_RXBUSY)){//Wait for any in-progress packets to finish being received by polling
            timeout++;
            if (timeout > 100000)
                break;
            };
        timeout = 0;
        while(!(enc28j60Read(ECON1) & ECON1_TXRTS)){//Wait for any current transmissions to end by confirming
            timeout++;
            if (timeout > 100000)
                break;
            };
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_VRPS);
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PWRSV);//Enter Sleep
        result = true;
  break;
  case 0:
        enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON2, ECON2_PWRSV);//Enter Sleep
        timeout = 0;
        while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY)){//Wait for PHY to stabilize (wait 300us)
            timeout++;
            if (timeout > 100000)
                break;
            };
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
//set maybee 12.1.5 Link Change Interrupt Flag (LINKIF)
        result = true;
  break;
  }
return result;
}

static void eint2ISR(void){
    ctl_global_interrupts_re_enable_from_isr();

                  uip_len = dev_poll();	// look for a packet
		  if(uip_len != 0){
//besser?                        if(ARPBUF->ethhdr.type == HTONS(UIP_ETHTYPE_IP)) {
                                  if(BUF->type == htons(UIP_ETHTYPE_IP)) {
 //                               piep_ok();
//                                uip_arp_ipin();
	         		uip_input();
	        		 	// If the above function invocation resulted in data that
	       			 	// should be sent out on the network, the global variable
	        		 	// uip_len is set to a value > 0. 
	         		if(uip_len > 0) {
	          			uip_arp_out();
	          			dev_send();
	        		 }
//besser?                        }else if(ARPBUF->ethhdr.type == HTONS(UIP_ETHTYPE_ARP)) {// process an ARP packet
                        }else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {// process an ARP packet
                                piep_ok();
                                uip_arp_arpin();
				         // If the above function invocation resulted in data that
				         // should be sent out on the network, the global variable
				         // uip_len is set to a value > 0.
				         if(uip_len > 0) {
                                           dev_send();
				         }
                                     }
                                    }
    ctl_global_interrupts_un_re_enable_from_isr();
    EXTINT |= (1<<2); //clear interrupt
}

void enc28j60PacketSend(U16 len, U8 *packet)
{
	// Set the write pointer to start of transmit buffer area
	enc28j60Write(EWRPTL, TXSTART_INIT);
	enc28j60Write(EWRPTH, TXSTART_INIT>>8);
	// Set the TXND pointer to correspond to the packet size given
	enc28j60Write(ETXNDL, (TXSTART_INIT+len));
	enc28j60Write(ETXNDH, (TXSTART_INIT+len)>>8);

	// write per-packet control byte
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(len, packet);
	
	// send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
}
unsigned int enc28j60PacketReceive(U16 maxlen, U8 *packet)
{
        U16 rxstat;
	U16 len;
        U32 timeout = 0;

	// Set the read pointer to the start of the received packet
	enc28j60Write(ERDPTL, (NextPacketPtr));
	enc28j60Write(ERDPTH, (NextPacketPtr)>>8);
	// read the next packet pointer
	NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the packet length
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the receive status
	rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;

	// limit retrieve length
	// (we reduce the MAC-reported length by 4 to remove the CRC)
	len = IFMIN(len, maxlen);

	// copy the packet from the receive buffer
	enc28j60ReadBuffer(len, packet);

	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
//	enc28j60Write(ERXRDPTL, (NextPacketPtr));
//	enc28j60Write(ERXRDPTH, (NextPacketPtr)>>8);

        // Errata workaround #13. Make sure ERXRDPT is odd
 	//

                uint16_t rs,re;
 	        rs = enc28j60Read(ERXSTH);
 	        rs <<= 8;
 	        rs |= enc28j60Read(ERXSTL);
 	        re = enc28j60Read(ERXNDH);
 	        re <<= 8;
 	        re |= enc28j60Read(ERXNDL);
 	        if (NextPacketPtr - 1 < rs || NextPacketPtr - 1 > re)
 	        {
 	            enc28j60Write(ERXRDPTL, (re));
 	            enc28j60Write(ERXRDPTH, (re)>>8);
	        }
 	        else
	        {
 	            enc28j60Write(ERXRDPTL, (NextPacketPtr-1));
 	            enc28j60Write(ERXRDPTH, (NextPacketPtr-1)>>8);
 	        }

	// decrement the packet counter indicate we are done with this packet
	// clear the PKTIF: Receive Packet Pending Interrupt Flag bit
        enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

	return len;
}

void enc28j60RegDump(void){
			
	debug_printf("RevID: 0x%x\n", enc28j60Read(EREVID));

	debug_printf ( ("Cntrl: ECON1 ECON2 ESTAT  EIR  EIE\n"));
	debug_printf ( ("         "));
	debug_printf("%02x",enc28j60Read(ECON1));
	debug_printf( ("    "));
	debug_printf("%02x",enc28j60Read(ECON2));
	debug_printf( ("    "));
	debug_printf("%02x",enc28j60Read(ESTAT));
	debug_printf( ("    "));
	debug_printf("%02x",enc28j60Read(EIR));
	debug_printf( ("   "));
	debug_printf("%02x",enc28j60Read(EIE));
	debug_printf( ("\n"));

	debug_printf( ("MAC  : MACON1  MACON2  MACON3  MACON4  MAC-Address\n"));
	debug_printf( ("        0x"));
	debug_printf("%02x",enc28j60Read(MACON1));
	debug_printf( ("    0x"));
	debug_printf("%02x",enc28j60Read(MACON2));
	debug_printf( ("    0x"));
	debug_printf("%02x",enc28j60Read(MACON3));
	debug_printf( ("    0x"));
	debug_printf("%02x",enc28j60Read(MACON4));
	debug_printf( ("   "));
	debug_printf("%02x",enc28j60Read(MAADR5));
	debug_printf("%02x",enc28j60Read(MAADR4));
	debug_printf("%02x",enc28j60Read(MAADR3));
	debug_printf("%02x",enc28j60Read(MAADR2));
	debug_printf("%02x",enc28j60Read(MAADR1));
	debug_printf("%02x",enc28j60Read(MAADR0));
	debug_printf( ("\n"));

	debug_printf( ("Rx   : ERXST  ERXND  ERXWRPT ERXRDPT ERXFCON EPKTCNT MAMXFL\n"));
	debug_printf( ("       0x"));
	debug_printf("%02x",enc28j60Read(ERXSTH));
	debug_printf("%02x",enc28j60Read(ERXSTL));
	debug_printf( (" 0x"));
	debug_printf("%02x",enc28j60Read(ERXNDH));
	debug_printf("%02x",enc28j60Read(ERXNDL));
	debug_printf( ("  0x"));
	debug_printf("%02x",enc28j60Read(ERXWRPTH));
	debug_printf("%02x",enc28j60Read(ERXWRPTL));
	debug_printf( ("  0x"));
	debug_printf("%02x",enc28j60Read(ERXRDPTH));
	debug_printf("%02x",enc28j60Read(ERXRDPTL));
	debug_printf( ("   0x"));
	debug_printf("%02x",enc28j60Read(ERXFCON));
	debug_printf( ("    0x"));
	debug_printf("%02x",enc28j60Read(EPKTCNT));
	debug_printf( ("  0x"));
	debug_printf("%02x",enc28j60Read(MAMXFLH));
	debug_printf("%02x",enc28j60Read(MAMXFLL));
	debug_printf( ("\n"));

	debug_printf( ("Tx   : ETXST  ETXND  MACLCON1 MACLCON2 MAPHSUP\n"));
	debug_printf( ("       0x"));
	debug_printf("%02x",enc28j60Read(ETXSTH));
	debug_printf("%02x",enc28j60Read(ETXSTL));
	debug_printf( (" 0x"));
	debug_printf("%02x",enc28j60Read(ETXNDH));
	debug_printf("%02x",enc28j60Read(ETXNDL));
	debug_printf( ("   0x"));
	debug_printf("%02x",enc28j60Read(MACLCON1));
	debug_printf( ("     0x"));
	debug_printf("%02x",enc28j60Read(MACLCON2));
	debug_printf( ("     0x"));
	debug_printf("%02x",enc28j60Read(MAPHSUP));
	debug_printf( ("\n"));
	debug_printf( ("PHY   : PHCON1  PHCON2  PHSTAT1 PHSTAT2\n"));
	debug_printf( ("       0x"));
	debug_printf("%02x",enc28j60Read(PHCON1));//ist 16 bit breit nicht 8 !
	debug_printf( ("     0x"));
        debug_printf("%02x",enc28j60Read(PHCON2));//ist 16 bit breit nicht 8 !
        debug_printf( ("     0x"));
        debug_printf("%02x",enc28j60Read(PHSTAT1));//ist 16 bit breit nicht 8 !
	debug_printf( ("     0x"));
        debug_printf("%02x",enc28j60Read(PHSTAT2));//ist 16 bit breit nicht 8 !
        debug_printf( ("\n"));

}



