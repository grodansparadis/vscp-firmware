/*
*******************************************************************************
* 	      digiprobe, Copyright 2009, 2010 Stefan Langer, datenheim            *
*      based on work copyrighted 2006-2009 by Ake Hedman, eurosource          *
*	Read the instructions in the licence.txt file. No warrenties at all       *
*******************************************************************************
 	Mapping of application registers inside EEPROM
*******************************************************************************/

#ifndef 	_DIGIPROBE_REGISTERS_H_
	#define _DIGIPROBE_REGISTERS_H_

	// Register offset for local storage

	// This are the offsets of the user registers within the user register space

	// Photocurrent offset
	#define P0_REG_IP_OFF				0		//overlay for access as float
	#define P0_REG_IP_OFF0				0
	#define P0_REG_IP_OFF1				1
	#define P0_REG_IP_OFF2				2
	#define P0_REG_IP_OFF3				3

	// Photo current calibration
	#define P0_REG_IP_CAL				4		//overlay for access as float
	#define P0_REG_IP_CAL_C0			4
	#define P0_REG_IP_CAL_C1			5
	#define P0_REG_IP_CAL_C2			6
	#define P0_REG_IP_CAL_C3			7

	// number of used registers in page0
	#define PAGE0_USER_REGISTERS		8		//number of user registers

#endif
