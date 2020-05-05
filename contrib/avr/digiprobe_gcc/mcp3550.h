/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 Christian Olesch, Lab-Control      *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	MCP355X driver using portpins
*******************************************************************************/

#ifndef _MCP355X_H_
#define _MCP355X_H_

	#include <inttypes.h>
	#include <avr/io.h>
	#include <util/delay.h>

	//Pinkonfiguration
	#define MCP355X_SDO			3
	#define MCP355X_CS			0
	#define MCP355X_SCK			1
	#define MCP355X_PORT		PORTB
	#define MCP355X_PIN			PINB
	#define MCP355X_DDR			DDRB
	#define MCP355X_DELAY_US	200 //Verzögerung in us für Optokoppler

	#define sbi(portn, bitn) ((portn)|=(1<<(bitn)))
	#define cbi(portn, bitn) ((portn)&=~(1<<(bitn)))

	#define NOP __asm__ __volatile__ ("NOP" ::)

	#define MCP355X_SINGLE		0
	#define MCP355X_CONTINOUS 	1

	void 		mcp355x_init(uint8_t flags);

	void 	mcp355x_trigger(void);

	//Auslesen des MCP355X AD-Wandlers
	//Value:		Rückgabewert int32
	//Return: 		0 Wandlung nicht fertig, 1 Wandlung fertig
	//Adstatus:		0 OK, 1 +Overflow, 2 -Overflow, 3 Comm Error
	//Werteberich:  -2.090.000 ... +2.090.000
	uint8_t 	mcp355x_read(int32_t *value, uint8_t *adstatus, uint8_t flags);

#endif

