/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	hardware specific settings and functions
*******************************************************************************/

#ifndef _DIGIPROBE_SYSTEM_H_
#define _DIGIPROBE_SYSTEM_H_

	// System clock in kHz
	#define F_CPU	8000000	// CPU Clock in Hz

	// SPI device selection enumerator
	#define SPI_DEVICE_NONE		0		// disable all devices
	#define SPI_DEVICE_MCP3550	1		// select MCP3550
	void spi_chip_select (uint8_t spidevice);

	// Prototypes
	void initTimer();
	void init_avr_ports(void);
	void array_write (uint8_t* array, uint16_t pos,  uint16_t bytes, void* data);
	void array_read (uint8_t* array, uint16_t pos,  uint16_t bytes, void* data);

#endif
