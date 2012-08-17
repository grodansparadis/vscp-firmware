/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack
 *
 * $Id: clock-arch.c,v 1.2 2006/06/12 08:00:31 adam Exp $
 */

/**
 * \file
 *         Implementation of architecture-specific clock functionality
 * \author
 *         Adam Dunkels <adam@sics.se>
 */
#include <global.h>
#include "clock-arch.h"
//#include <rtc.h>

/*---------------------------------------------------------------------------*/
clock_time_t clock_time(void){

struct timeval { 
long    tv_sec;         /* Sekunden */
long    tv_usec;        /* Mikrosekunden */
};
struct timezone {
int	tz_minuteswest;	/* minutes west of Greenwich */
int	tz_dsttime;	/* type of dst correction */
};
struct timeval tv;
struct timezone tz;

//  gettimeofday(&tv, &tz);
tz.tz_minuteswest = -60;
tz.tz_dsttime = 0;
//tv.tv_sec = RTC_GetSEC();
tv.tv_sec = getSysTICs();
//debug_printf("tv.tv_sec %d\n", tv.tv_sec);
tv.tv_usec = 0;
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;//original
//  return tv.tv_sec + tv.tv_usec;
}
/*---------------------------------------------------------------------------*/
