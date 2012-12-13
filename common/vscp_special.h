//
// special_avr.h - Copyright (C) 2012, Stefan Langer, dev@datenheim.de
//
// this file is part of vscp - very simple control protocol with
// Copyright (C) 2002-2004 Ake Hedman, eurosource, akhe@eurosource.se
//
// This software is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
// General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// PLEASE DEFINE IN YOUR vscp_projdefs.h THE CONSTANT 
// VSCP_SPECIAL_PLATFORM as depending on your platform as follows:
// 1 = AVR (includes all ATiny, Atmega, Xmega)
// 2 = PIC (all PICs from 8 to 32 Bit)
// 3 = ARM (all ARM based designs)

#ifndef VSCP_SPECIAL_H_
#define VSCP_SPECIAL_H_

#ifdef VSCP_SPECIAL_PLATFORM 
	#if VSCP_SPECIAL_PLATFORM == 1
		#warning "Using optional protocol features for AVR platform"
		#include "../avr/common/special_avr.h"
	#elif VSCP_SPECIAL_PLATFORM == 2
		#warning "Using optional protocol features for PIC platform"
		#include "../pic/common/special_pic.h"
	#elif VSCP_SPECIAL_PLATFORM == 3
		#warning "Using optional protocol features for ARM platform"
		#include "../arm/common/special_arm.h"
	/*
	// can be expanded here if ARM platforms differ too much or librarys
	// provide very different features for reset and blocking wait functions
	#elif VSCP_SPECIAL_PLATFORM == 4
		#warning "Using special hardware functions vor ARM-NXP platform"
		#include "../arm/nxp/special_nxp.h"
	#elif VSCP_SPECIAL_PLATFORM == 5
		#warning "Using special hardware functions vor ARM-STR platform"
		#include "../arm/str/special_str.h"
	*/
	#else
		#warning "VSCP_SPECIAL_PLATFORM not configured. Using protocol features not used."
	#endif
#else
	#warning "VSCP_SPECIAL_PLATFORM not configured. Using protocol features not used."
#endif

#endif
