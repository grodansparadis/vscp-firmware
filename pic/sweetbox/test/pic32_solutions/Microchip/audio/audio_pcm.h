/*********************************************************************
 *
 *                  AUDIO pcm wrapper header
 *
 *********************************************************************
 * FileName:        audio_pcm.h
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
 *$Id: $
 ********************************************************************/

#ifndef _AUDIO_RAW_H_
#define _AUDIO_RAW_H_ 

#include <audio/audio.h>
#include "audio_formats.h"

// returns the number of samples needed in the decoder out buffer
int			PcmDecoderOutSamples(int procSamples, sStreamInfo* pInfo);

// init the decoder
AUDIO_RES		PcmInitDecoder(int nProcSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl);

// start decoding data
AUDIO_RES		PcmReadDecodeData(audioCodecHandle h, progressDcpt* pPrgDcpt);

// clean-up, after done
void			PcmCleanUpDecoder(audioCodecHandle h);

// get the output work buffer size
int			PcmGetBuffSize(audioCodecHandle h);

// get the decoder output buffer
void*			PcmGetOutBuff(audioCodecHandle h);

// set the working decoder output buffer
void			PcmSetOutBuff(audioCodecHandle h, void* pBuff);

// get the encoded/decoded bytes and clear internal counter
int			PcmGetBytes(audioCodecHandle h, int clr);


// returns the number of samples needed in the encoder input buffer
int			PcmEncoderInSamples(int procSamples, sStreamInfo* pInfo);

// init the encoder
AUDIO_RES		PcmInitEncoder(int nProcSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl);

// start decoding data
AUDIO_RES		PcmEncodeWriteData(audioCodecHandle h, int nBytes, progressDcpt* pPrgDcpt);

// get the encoder input buffer
void*			PcmGetInBuff(audioCodecHandle h);

// set the working encoder input buffer
void			PcmSetInBuff(audioCodecHandle h, void* pBuff);


// clean-up, when done
#define			PcmCleanUpEncoder	PcmCleanUpDecoder


// check the play/record format
AUDIO_RES	PcmCheckFormat(sStreamInfo* pInfo);



#endif	// _AUDIO_RAW_H_

