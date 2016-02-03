/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*   based on work copyrighted 2006-2009 by Christian Olesch, Lab-Control      *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	generic AVR ADC functions
*******************************************************************************/


#ifndef _AVR_SPI_H_
#define _AVR_SPI_H_

	#define SPI_PORT			PORTB
	#define SPI_PIN				PINB
	#define SPI_DDR				DDRB
	#define SPI_CLK_PIN			1
	#define SPI_MOSI_PIN		2
	#define SPI_MISO_PIN		3

	//Schnittstelleneinstellungen
	//Frequenzteiler (vom Master-Clock)
	#define FREQ_DIV_4			0
	#define FREQ_DIV_16			1
	#define FREQ_DIV_64			2
	#define FREQ_DIV_128		3

	//SPI-Modi 0..3 (siehe Datenblatt)
	void 	spi_master_init(uint8_t freq_div, bool lsb_first, uint8_t spi_mode);	//SPI initialisieren
	void 	spi_transfer(uint8_t byte_send, uint8_t* byte_receive);					//Ein Byte übertragen (senden / empfangen bidirektional)
	void	spi_transfer_array(uint8_t* send, uint8_t* receive, uint8_t bytes);	//Array übertragen
	bool 	spi_miso_is_high(void);													//Liest den Status der MISO-Leitung

#endif


