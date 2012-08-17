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
 * Copyright (C) 2008-2011 Ake Hedman, eurosource, <akhe@eurosource.se>
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

#include <p32xxxx.h>
#include <plib.h> 
#include <inttypes.h>
#include "GenericTypedefs.h"
#include "rtc.h"

///////////////////////////////////////////////////////////////////////////////
// initRTCC
//

uint8_t initRTCC( void )
{
    rtccTime	tm;
    rtccDate	dt;
    
    RtccInit();            // init the RTCC

    // wait for the SOSC to be  
    //actually running and RTCC to have its clock source, 
    // could wait here at most 32ms
    while ( RTCC_CLK_ON != RtccGetClkStat() );    
        
    // let's set the current date
    {
        // Set clock to epoch 1970-01-01 00:00:00
        tm.l = 0;               // Why do clocks always start at 12:00??
        tm.sec = 0x00;          // Set the Seconds
        tm.min = 0x00;          // set the Minutes
        tm.hour = 0x00;         // Set the Hour

        dt.wday = 0;            
        dt.mday = 0x01;         
        dt.mon = 0x01;          
        dt.year = 0x70;        
        
        RtccSetTimeDate( tm.l, dt.l );  // Set time and date into registers....
    }
    
    //mRtccSelectSecPulseOutput();      // select the seconds clock pulse as the function of the RTCC output pin
    //mRtccSelectAlarmPulseOutput();      // select the alarm pulse as the function of the RTCC output pin
    //mRtccOutputEnable();                // enable the Output pin of the RTCC
    
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// openRTCC
//

uint8_t openRTCC( uint32_t lTime, uint32_t lDate )
{
    // set time, date and calibration in a single operation
    //RtccOpen( tm.l, dt.l, 0 );   
    RtccOpen( lTime, lDate, 0 );      
       
    //check to see if the RTCC is running: check the SYNC bit
    while( mRtccGetSync() );            // wait sync to be low
    while( !mRtccGetSync() );           // wait to be high
    while( mRtccGetSync() );            // wait sync to be low again
    
    // other RTCC operations
    
    // adjust the RTCC timing
    RtccSetCalibration( 200 );          // value to calibrate with at each minut    
    // enabling the RTCC output pin
    //mRtccSelectSecPulseOutput();      // select the seconds clock pulse as the function of the RTCC output pin
    //mRtccSelectAlarmPulseOutput();      // select the alarm pulse as the function of the RTCC output pin
    //mRtccOutputEnable();                // enable the Output pin of the RTCC
    //Enable Interrupts
    mRtccEnableInt();
    
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// alarmRTCC
//

uint8_t alarmRTCC( uint32_t lTime, uint32_t lDate, uint16_t AlrmMinutes )    
{
    rtccTime	tAlrm;	
	rtccDate	dAlrm;
	
    tAlrm.l = lTime;
    dAlrm.l = lDate;
    tAlrm.min += AlrmMinutes;           // Set for nn Mins
    RtccChimeDisable();                 // Disable Chime, no rollover allowed!!
    RtccSetAlarmRptCount( 5 );          // 10 Alarms
    RtccSetAlarmRpt( RTCC_RPT_MIN );    // repeat every minute
    RtccSetAlarmTimeDate( tAlrm.l, dAlrm.l );
    RtccAlarmEnable();
    
    return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CalibrateRTCC
//

/*    the following will update the RTCC calibration value.*/
void CalibrateRTCC( void )
{
    //int cal = 0x3FD;                // 10 Bits adjustment, -3 in value
    if ( RTCCON & 0x8000 )          // if RTCC is On
    {
        uint16_t t0,t1;
        do
        {
            t0 = RTCTIME;
            t1 = RTCTIME;
        } while( t0 != t1 );  
        //Read valid time value
            
        if ( ( t0 & 0xFF ) == 00 )
        {                               // were at 2nd 00, wait for auto-adjust to be performed
            while( !( RTCCON & 0x2 ) ); // wait until second half
        } 
    }
}

