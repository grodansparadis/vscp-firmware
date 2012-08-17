/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Main application
*******************************************************************************/


#ifndef _DIGIPROBE_GCC_H_
	#define _DIGIPROBE_GCC_H_

	//#define DIGIPROBE_USE_FLOAT

	////
	#define SENS_TMP_ADCCHN 0

	// I want to use floating points
	//#define DIGIPROBE_USE_FLOAT



	uint8_t STD_REG_GUID[16];
	uint8_t STD_REG_MANUFACTID[8];
	uint8_t STD_REG_MDF_URL[32];

	uint8_t STD_REG_NICKNAME;
	uint8_t STD_REG_SEGMENT_CRC;
	uint8_t STD_REG_CONTROL;
	uint8_t STD_REG_ZONE;
	uint8_t STD_REG_SUBZONE;
	uint8_t STD_REG_USERID[5];
	uint8_t P0_USER_REGS[PAGE0_USER_REGISTERS];



	//////////////////////////////////////////////////////////
	// Variables
	volatile uint16_t clock_current;	// timer for current readings
	volatile uint16_t clock_temp;		// timer for temperature readings

	int32_t sens_current_raw;
	int16_t sens_temp_raw;
	uint8_t samples;
	uint8_t average;

	uint8_t adstatus;

char txt [20];


#endif
