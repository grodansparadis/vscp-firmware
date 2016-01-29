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

#ifndef _MCP355XS_H_
#define _MCP355XS_H_


	#define MCP355X_SINGLE		0
	#define MCP355X_CONTINOUS 	1

	#define MCP355XS_DELAY_US	200 //Verzögerung in us für Optokoppler

	#define byteof(A,byte) (*(uint8_t*)(&A + byte))

	//------------------------------------------------------------------------------
	// Chip initialisieren
	//------------------------------------------------------------------------------
	uint8_t mcp355xs_init(uint8_t flags);

	//------------------------------------------------------------------------------
	//Auslesen des MCP3550 AD-Wandlers
	//Value:		Rückgabewert int32
	//Return: 		0 Wandlung nicht fertig, 1 Wandlung fertig
	//Adstatus:		0 OK, 1 +Overflow, 2 -Overflow, 3 Comm Error
	//Werteberich:  -2.090.000 ... +2.090.000
	//
	//Diese Funktion darf nicht öfter als alle 100ms aufgerufen werden
	//------------------------------------------------------------------------------
	uint8_t mcp355xs_read(int32_t *value, uint8_t *adstatus, uint8_t flags);

#endif

