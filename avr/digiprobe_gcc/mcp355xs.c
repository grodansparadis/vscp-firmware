/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 Christian Olesch, Lab-Control      *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	MCP355X driver with hardware SPI support
	/CS handling of all SPI devices must be implemented in function
	spi_chip_select() contained in digiprobe_system.c
*******************************************************************************/

#include "digiprobe.inc"

//------------------------------------------------------------------------------
// Chip initialisieren
//------------------------------------------------------------------------------
uint8_t mcp355xs_init(uint8_t flags)
{
	int32_t value;
	uint8_t adstatus;

	// enable the chip
	spi_chip_select(SPI_DEVICE_MCP3550);

	// do a read out
	mcp355xs_read(&value, &adstatus, flags);

	// for single conversion mode release /CE after readout
	if (flags == MCP355X_SINGLE) {
		spi_chip_select(SPI_DEVICE_NONE);	
		}

	return adstatus;
};



//------------------------------------------------------------------------------
//Auslesen des MCP3550 AD-Wandlers
//------------------------------------------------------------------------------

uint8_t mcp355xs_read(int32_t *value, uint8_t *adstatus, uint8_t flags)
{
	uint8_t retvalue;
	int32_t dw;
	uint8_t *dw_bytes;
	uint8_t errorbits;
	
	dw_bytes=(uint8_t*) &dw;

	
	retvalue=0;

	//Prüfen ob Wandlung fertig ist
	//cbi(MCP3550_PORT,MCP3550_CS);
	_delay_us(MCP355XS_DELAY_US);

	//Wenn ja Ergebnis auslesen
	if(!spi_miso_is_high())
		{
		retvalue = 1;
		dw=0;
		
		//3Bytes (24 Bits) auslesen
		spi_transfer_array(NULL, dw_bytes, 3); 
		
		/*
		for(i=0;i<24;i++)
			{
				cbi(MCP3550_PORT,MCP3550_SCK);
				_delay_us(MCP3550_DELAY_US);
				sbi(MCP3550_PORT,MCP3550_SCK);
				_delay_us(MCP3550_DELAY_US);
				
				dw <<= 1; //Datenwort um ein Bit nach links
				if(bit_is_clear(MCP3550_PIN,MCP3550_SDO)) dw++;
			}
		*/
		
		errorbits = *(dw_bytes+2);
		errorbits >>= 6;
		*adstatus=errorbits;
		//retvalue = 1;
		
		//Errorbits aus Ergebnis herausmaskieren
		*(dw_bytes+2) &= 0b00111111;
		
		//Wenn kein OVL+
		if((errorbits&_BV(0))==0)
			{
			//Wenn Wert negativ, dann bits 22-31 negieren
			if(*(dw_bytes+2) & _BV(5))
				{
				*(dw_bytes+3)  = 255;
				*(dw_bytes+2) |= 0b11100000;
				}
			//Wenn OVL-, dann auch Bits 22-31 negieren
			if(errorbits & _BV(1))
				{
				*(dw_bytes+3)  = 255;
				*(dw_bytes+2) |= 0b11100000;
				}
			}
		
		//Wenn OVL+
		else
			{
			//Dann OVL+ -Bit aus dem Ergebnis rausmaskieren
			*(dw_bytes+2) &= ~_BV(6);
			};
			
		*value = dw;
		
		
		//Nächste Wandlung starten
		//sbi(MCP3550_PORT,MCP3550_CS);
		//_delay_us(MCP3550_DELAY_US);	
		//cbi(MCP3550_PORT,MCP3550_CS);
		//_delay_us(200);
		}
	
	//sbi(MCP3550_PORT,MCP3550_SCK);
	//sbi(MCP3550_PORT,MCP3550_CS);

	return retvalue;
};



