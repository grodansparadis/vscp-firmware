/*! \file nmea.c \brief NMEA protocol function library. */
//*****************************************************************************
//
// File Name	: 'nmea.c'
// Title		: NMEA protocol function library
// Author		: Pascal Stang - Copyright (C) 2002
// Created		: 2002.08.27
// Revised		: 2002.08.27
// Version		: 0.1
// Target MCU	: Atmel AVR Series
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#ifndef WIN32
	#include <avr/io.h>
	#include <avr/signal.h>
	#include <avr/interrupt.h>
	#include <avr/pgmspace.h>
	#include <string.h>
	#include <stdlib.h>
#endif

#include "global.h"
#include "buffer.h"
#include "rprintf.h"
#include "gps.h"

#include "nmea.h"

// Program ROM constants

// Global variables
extern GpsInfoType GpsInfo;
#define BUFFERSIZE	80
u08 NmeaPacket[BUFFERSIZE];
u08 debug;

void nmeaInit(void)
{
	debug = 0;
}

u08 nmeaProcess(cBuffer* rxBuffer)
{
	u08 foundpacket = FALSE;
	u08 startFlag = FALSE;
	//u08 data;
	u08 i,j;

	// process the receive buffer
	// go through buffer looking for packets
	while(rxBuffer->datalength)
	{
		// look for a start of NMEA packet
		if(bufferGetAtIndex(rxBuffer,0) == '$')
		{
			// found start
			startFlag = TRUE;
			// done looking for start
			break;
		}
		else
			bufferGetFromFront(rxBuffer);
	}
	
	// if we detected a start, look for end of packet
	if(startFlag)
	{
		for(i=1; i<(rxBuffer->datalength)-1; i++)
		{
			// check for end of NMEA packet <CR><LF>
			if((bufferGetAtIndex(rxBuffer,i) == '\r') && (bufferGetAtIndex(rxBuffer,i+1) == '\n'))
			{
				// have a packet end
				// dump initial '$'
				bufferGetFromFront(rxBuffer);
				// copy packet to NmeaPacket
				for(j=0; j<(i-1); j++)
					NmeaPacket[j] = bufferGetFromFront(rxBuffer);
				// null terminate it
				NmeaPacket[j] = 0;
				// dump <CR><LF> from rxBuffer
				bufferGetFromFront(rxBuffer);
				bufferGetFromFront(rxBuffer);

				if(debug)
				{
					rprintf("Rx NMEA packet type: ");
					rprintfStrLen(NmeaPacket, 0, 5);
					rprintfStrLen(NmeaPacket, 5, (i-1)-5);
					rprintfCRLF();
				}
				// found a packet
				// done with this processing session
				foundpacket = TRUE;
				break;
			}
		}
	}

	if(foundpacket)
	{
		// check message type and process appropriately
		if(!strncmp(NmeaPacket, "GPGGA", 5))
			nmeaProcessGPGGA(NmeaPacket);
		else if(!strncmp(NmeaPacket, "GPVTG", 5))
			nmeaProcessGPVTG(NmeaPacket);
	}
	return foundpacket;
}

void nmeaProcessGPGGA(u08* packet)
{
	u08 i;
	char* endptr;

	if(debug)
	{
		rprintf("NMEA: ");
		rprintfStr(packet);
		rprintfCRLF();
	}

	// start parsing just after "GPGGA,"
	i = 6;

	// get UTC time [hhmmss.sss]
	GpsInfo.PosLLA.TimeOfFix.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: latitude
	
	// get latitude [ddmm.mmmmm]
	GpsInfo.PosLLA.lat.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: N/S indicator
	
	// correct latitute for N/S
	if(packet[i] == 'S') GpsInfo.PosLLA.lat.f = -GpsInfo.PosLLA.lat.f;
	while(packet[i++] != ',');				// next field: longitude
	
	// get longitude [ddmm.mmmmm]
	GpsInfo.PosLLA.lon.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: E/W indicator

	// correct latitute for E/W
	if(packet[i] == 'W') GpsInfo.PosLLA.lon.f = -GpsInfo.PosLLA.lon.f;
	while(packet[i++] != ',');				// next field: position fix status

	// position fix status
	// 0 = Invalid, 1 = Valid SPS, 2 = Valid DGPS, 3 = Valid PPS
	// check for good position fix
	if( (packet[i] != '0') && (packet[i] != ',') )
		GpsInfo.PosLLA.updates++;
	while(packet[i++] != ',');				// next field: satellites used
	
	// get number of satellites used in GPS solution
	GpsInfo.numSVs = atoi(&packet[i]);
	while(packet[i++] != ',');				// next field: HDOP (horizontal dilution of precision)
	while(packet[i++] != ',');				// next field: altitude
	
	// get altitude (in meters)
	GpsInfo.PosLLA.alt.f = strtod(&packet[i], &endptr);

	while(packet[i++] != ',');				// next field: altitude units, always 'M'
	while(packet[i++] != ',');				// next field: geoid seperation
	while(packet[i++] != ',');				// next field: seperation units
	while(packet[i++] != ',');				// next field: DGPS age
	while(packet[i++] != ',');				// next field: DGPS station ID
	while(packet[i++] != '*');				// next field: checksum
}

void nmeaProcessGPVTG(u08* packet)
{
	u08 i;
	char* endptr;

	if(debug)
	{
		rprintf("NMEA: ");
		rprintfStr(packet);
		rprintfCRLF();
	}

	// start parsing just after "GPVTG,"
	i = 6;
	// get course (true north ref) in degrees [ddd.dd]
	GpsInfo.VelHS.heading.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'T'
	while(packet[i++] != ',');				// next field: course (magnetic north)

	// get course (magnetic north ref) in degrees [ddd.dd]
	//GpsInfo.VelHS.heading.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'M'
	while(packet[i++] != ',');				// next field: speed (knots)

	// get speed in knots
	//GpsInfo.VelHS.speed.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'N'
	while(packet[i++] != ',');				// next field: speed (km/h)

	// get speed in km/h
	GpsInfo.VelHS.speed.f = strtod(&packet[i], &endptr);
	while(packet[i++] != ',');				// next field: 'K'
	while(packet[i++] != '*');				// next field: checksum

	GpsInfo.VelHS.updates++;
}

