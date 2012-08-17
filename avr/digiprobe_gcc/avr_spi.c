/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2009 by Christian Olesch, lab-control        *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Generic AVR SPI functions
*******************************************************************************/

#include"digiprobe.inc"


//-----------------------------------------------------------------------------------------------------------------
// SPI als Master initialisieren
//-----------------------------------------------------------------------------------------------------------------
void 	spi_master_init(uint8_t freq_div, bool lsb_first, uint8_t spi_mode)
{
	
	//Power einschalten für das SPI-Interface:
	//PRSPI-Bit im PRR0-Register auf 0 setzen
	// now use the avr-lib for this!
	// Not all Atmels support this!
	#ifdef PRSPI
	power_spi_enable();
	#endif
	
	//Portpins des SPI initialisieren
	SPI_DDR |= _BV(SPI_CLK_PIN) | _BV(SPI_MOSI_PIN);
	SPI_DDR &= ~_BV(SPI_MISO_PIN);
	
	//SPI als Master
	SPCR = (1<<SPE)|(1<<MSTR);
	
	//Frequenzteiler einstellen
	SPCR |= freq_div;
	
	//Übertragungsmodus 0...3
	SPCR |= spi_mode << 2;
	
	//LSB oder MSB first
	if(lsb_first) SPCR |= (1<<DORD);

}



//-----------------------------------------------------------------------------------------------------------------
// Ein Byte übertragen (senden / empfangen bidirektional)
//-----------------------------------------------------------------------------------------------------------------
void 	spi_transfer(uint8_t byte_send, uint8_t* byte_receive)
{
	//Start transmission
	SPDR = byte_send;
	
	//Wait for transmission complete
	while(!(SPSR & (1<<SPIF)));
	
	//Receive byte
	if(byte_receive) *byte_receive=SPDR;
}



//-----------------------------------------------------------------------------------------------------------------
// Array übertragen
//-----------------------------------------------------------------------------------------------------------------
void	spi_transfer_array(uint8_t* send, uint8_t* receive, uint8_t bytes)
{
	uint8_t tx,rx,i;
	for(i=0; i<bytes; i++)
	{
		if(send) tx=send[i];
		else tx=0;
		
		spi_transfer(tx, &rx);
		
		if(receive) receive[bytes-i-1]=rx;
	}
}					


//-----------------------------------------------------------------------------------------------------------------
//Liest den Status der MISO-Leitung
//-----------------------------------------------------------------------------------------------------------------
bool 	spi_miso_is_high(void)
{
	bool r=false;
	if(SPI_PIN & _BV(SPI_MISO_PIN)) r=true;
	return r;
}												
