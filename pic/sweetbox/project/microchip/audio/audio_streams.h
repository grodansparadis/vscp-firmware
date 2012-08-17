/*********************************************************************
 *
 *                  AUDIO streams header
 *
 *********************************************************************
 * FileName:        audio_streams.h
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB Cxx
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 * Microchip Audio Library – PIC32 Software.
 * Copyright © 2008 Microchip Technology Inc.  All rights reserved.
 * 
 * Microchip licenses the Software for your use with Microchip microcontrollers
 * and Microchip digital signal controllers pursuant to the terms of the
 * Non-Exclusive Software License Agreement accompanying this Software.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION,
 * ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS
 * FOR A PARTICULAR PURPOSE.
 * MICROCHIP AND ITS LICENSORS ASSUME NO RESPONSIBILITY FOR THE ACCURACY,
 * RELIABILITY OR APPLICATION OF THE SOFTWARE AND DOCUMENTATION.
 * IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED
 * UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH
 * OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL,
 * SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS
 * OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY,
 * SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED
 * TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *
 *$Id: audio_streams.h 161 2008-10-15 19:30:28Z akhe $
 ********************************************************************/

#ifndef _AUDIO_STREAMS_H_
#define _AUDIO_STREAMS_H_ 

#include <audio/audio.h>

typedef void*	audioSHandle;		// handle to an audio stream



// open an audio stream
// NOTE: Multiple streams are handled.
// Use AudioStreamCleanup() to discard and clean-up an already opened stream.

// open a stream in a specified format for PLAY/RECORD
// for PLAY op, the info is updated from the stream itself
AUDIO_RES	AudioStreamOpen(sStreamInfo* pInfo, audioSHandle* pHandle);

// closes a currently opened stream
AUDIO_RES	AudioStreamClose(audioSHandle handle);
	
// discards and clean up an opened stream
AUDIO_RES	AudioStreamCleanup(audioSHandle handle);

// returns stream info
const sStreamInfo*	AudioStreamGetInfo(audioSHandle pHandle);

// read data samples
AUDIO_RES	AudioStreamReadData(audioSHandle handle, void* pBuff, int nBytes, void* pRes);


// write data samples
AUDIO_RES	AudioStreamWriteData(audioSHandle handle, void* pBuff, int nBytes, void* pRes);


// returns run time info. stream specific.
AUDIO_RES	AudioStreamGetRunInfo(audioSHandle handle, void* pBuff, int nBytes);



// have a way of getting the current streams from the audio library
extern audioSHandle	AudioGetInStream(void);

extern audioSHandle	AudioGetOutStream(void);


#endif	// _AUDIO_STREAMS_H_

