/*************************************************************
* nRF2401A interface code
*
* Contains functions to operate the nRF in Shockburts mode.
*
* (c) Lieven Hollevoet
* most recent version on http://boostc.lika.be
*************************************************************/

#include "nrf2401a.h"
#include "spi.h"
#include "serial.h"

// Global variable to this function, contains all the settings
nrf2401_t nrf_conf;

//////////////////////////////////////////////////////////////
// nRF init function
// 
// Configures the SPi interface and the basic settings for 
// the nRF
//////////////////////////////////////////////////////////////
void nrf_init() {
	// Required for Tpd2cfgm
	delay_ms(3);
	// Init the SPI interface
	spi_init();
	
	nrf_ce = 0;
	nrf_cs = 0;
	
	// Init control pins data direction
	nrf_ce_tris     = 0;
	nrf_cs_tris	    = 0; 
	nrf_pwr_up_tris = 0;
	nrf_dr1_tris    = 1;  
	nrf_pwr_up  = 1;
	
	nrf_basic_config();
	
	return;
}

//////////////////////////////////////////////////////////////
// Basic configuration settings
// 
// Change parameters like channel and datawidth here
//////////////////////////////////////////////////////////////
void nrf_basic_config() {

	// Program the settings for the nrf
	nrf_conf.txrx          = NRF_RX;
	nrf_conf.channel       = 32;
	nrf_conf.power         = NRF_POW0dBm;
	nrf_conf.xtal          = NRF_XTAL16;	
	nrf_conf.rate          = NRF_RATE250k;
	nrf_conf.mode          = NRF_SHOCKBURST;
	nrf_conf.rxmode        = NRF_SINGLERX;
	nrf_conf.crcmode       = NRF_CRCON;
	nrf_conf.crclength     = NRF_CRC8BIT;
	nrf_conf.addrwidth     = NRF_ADDR24;  
	nrf_conf.addr_ch1[0]   = 0x99; 
	nrf_conf.addr_ch1[1]   = 0x99;  
	nrf_conf.addr_ch1[2]   = 0x99; 
	nrf_conf.addr_ch1[3]   = 0x99;
	nrf_conf.addr_ch1[4]   = 0x99;     
	nrf_conf.datawidth_ch1 = 0x10; // in bits!!
	nrf_conf.addr_ch2[0]   = 0x99;  
	nrf_conf.addr_ch2[1]   = 0x99;  
	nrf_conf.addr_ch2[2]   = 0x99;  
	nrf_conf.addr_ch2[3]   = 0x99;  
	nrf_conf.addr_ch2[4]   = 0x99;                  
	nrf_conf.datawidth_ch2 = 0xE0; // in bits!!
	

 	// configure the device
	nrf_send_config(15);

	return;
	
}

//////////////////////////////////////////////////////////////
// Program the nRF in receive mode, and start reception
//////////////////////////////////////////////////////////////
void nrf_rx_mode(){

	// Config
	nrf_conf.txrx = NRF_RX;
	nrf_send_config(1);
	
	// Make sure we don't drive the output of the spi bus
	spi_dout_tris = 1;

	// Enable reception
	nrf_ce = 1;
	
	return;	
}

//////////////////////////////////////////////////////////////
// Program the nRF in transmit mode
//////////////////////////////////////////////////////////////
void nrf_tx_mode(){
	nrf_conf.txrx = NRF_TX;
	nrf_send_config(1);

	//nrf_ce = 1;
	return;	
}

//////////////////////////////////////////////////////////////
// Send the configuration word to the nRF
//  This function will pack the config into 15 bytes
//  and will transmit it over the SPI to the nRF2401A
// The user can also choose to only transmit the first
// or the first 2 bytes for quick config changes
//////////////////////////////////////////////////////////////
void nrf_send_config(char nr_of_bytes){

	char helper;
	
	// PIC drives the SPI bus
	spi_dout_tris = 0;
	
	// Put the nrf in config mode
	nrf_ce = 0;
	delay_10us(1);
	nrf_cs = 1;
			
	// Required for Tcs2data
	delay_10us(1);
	
	// Load the spi buffer (contents -> see nRF2401A manual p. 18)
	// Note: the lack of break statement in the following switch-statement
	//  is intentional
	switch (nr_of_bytes) {
		case 15:
			spi_load_byte(nrf_conf.datawidth_ch2);
			spi_load_byte(nrf_conf.datawidth_ch1);
			spi_load_byte(nrf_conf.addr_ch2[4]);
			spi_load_byte(nrf_conf.addr_ch2[3]);
			spi_load_byte(nrf_conf.addr_ch2[2]);
			spi_load_byte(nrf_conf.addr_ch2[1]);
			spi_load_byte(nrf_conf.addr_ch2[0]);
			spi_load_byte(nrf_conf.addr_ch1[4]);
			spi_load_byte(nrf_conf.addr_ch1[3]);
			spi_load_byte(nrf_conf.addr_ch1[2]);
			spi_load_byte(nrf_conf.addr_ch1[1]);
			spi_load_byte(nrf_conf.addr_ch1[0]);
	
			helper   = nrf_conf.addrwidth << 2;
			helper.1 = nrf_conf.crclength;
			helper.0 = nrf_conf.crcmode;
			spi_load_byte(helper);

		case 2:
			helper   = nrf_conf.xtal << 2;
			helper  |= nrf_conf.power & (0x03);
			helper.5 = nrf_conf.rate;
			helper.6 = nrf_conf.mode;
			helper.7 = nrf_conf.rxmode;
			spi_load_byte(helper);
	
		case 1:
			helper   = nrf_conf.channel << 1;
			helper.0 = nrf_conf.txrx;
			spi_load_byte(helper);
	}
	
	spi_exchange();
	spi_wait_for_completion();
	
	// Extra delay
	delay_10us(1);
	
	// Leave config mode
	nrf_cs = 0;

	// Extra delay
	delay_10us(1);
			
	return;
	 
}

//////////////////////////////////////////////////////////////
// Read the received data from the nRF into the receive 
// buffer
//////////////////////////////////////////////////////////////
void nrf_read(){

	spi_read(nrf_conf.datawidth_ch1 / 8);

	return;
}

//////////////////////////////////////////////////////////////
// Read a byte from the nrf receive buffer
//////////////////////////////////////////////////////////////
char nrf_get_byte(char index){
	return spi_get_byte(index);
}

//////////////////////////////////////////////////////////////
// Write data to the nRF and transmit
//////////////////////////////////////////////////////////////
void nrf_write(char* addr, char* data){

	char count = 0;
	
	// Load data and address
	spi_init_buffer();
	
	for (count = 0; count < (nrf_conf.addrwidth / 8); count++){
		spi_load_byte(*addr++);
	}
	
	for (count = 0; count < (nrf_conf.datawidth_ch1 / 8); count++){
		spi_load_byte(*data++);
	}
	
	// Wake up sleepyhead
	nrf_ce = 1;
	
	spi_exchange();
	spi_wait_for_completion();

	nrf_ce = 0;

}

