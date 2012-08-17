/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol) 
 * 	http://www.vscp.org
 *
 *  File: main.c
 *
 *  The SweetBox
 * 	Version information in version.h
 * 	akhe@eurosource.se
 *
 * Copyright (C) 2008 Ake Hedman, eurosource, <akhe@eurosource.se>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 *	This file is part of VSCP - Very Simple Control Protocol 	
 *	http://www.vscp.org
 *
 * ******************************************************************************
*/


#ifndef ___RTC_H
#define ___RTC_H


/*!
    Initialize the realtime clock
    \return TRUE on success.
*/
uint8_t initRTCC( void );

/*!
    Open realtime clock
    \param lTime Current Time
    \param lDate Current Date
    \return TRUE on success
*/
uint8_t openRTCC( uint32_t l1, uint32_t l2 );

/*!
    Set an alarm for the RTCC
    \param lTime Current time
    \parm lDate Current data
    \param alarmMinutes Minutes in future for alarm
    \return TRUE on success.
*/
uint8_t alarmRTCC( uint32_t lTime, uint32_t lDate, uint16_t alarmMinutes ); 

/*!
    Calibrate RTC
*/
void CalibrateRTCC( void );

#endif // ___RTC_H

