/*********************************************************************
 *
 *                  AUDIO formats header
 *
 *********************************************************************
 * FileName:        audio_formats.h
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

#ifndef _AUDIO_FORMATS_H_
#define _AUDIO_FORMATS_H_ 

#include <audio/audio.h>


// common definitions accross supported formats

typedef void*	audioCodecHandle;	// handle to an audio codec


typedef enum
{
	CODEC_ALLOCATE_NONE	= 0,
	CODEC_ALLOCATE_IN	= 0x1,
	CODEC_ALLOCATE_OUT	= 0x2,
	CODEC_ALLOCATE_INOUT	= CODEC_ALLOCATE_IN|CODEC_ALLOCATE_OUT
}eBuffMask;


// decoding/playing stuff

// number of samples needed in the decoder out buffer
typedef int		(*pDecoderOutSamples)(int procSamples, sStreamInfo* pInfo);

// init the decoder function
typedef AUDIO_RES	(*pInitDecoder)(int nProcSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl);

// read and decode data
typedef AUDIO_RES	(*pReadDecodeData)(audioCodecHandle h, progressDcpt* pPrgDcpt);

// clean-up, after done
typedef void		(*pDecoderCleanup)(audioCodecHandle h);

// get the output work buffer size
typedef int		(*pGetOutBuffSize)(audioCodecHandle h);

// get the working output buffer
typedef void*		(*pGetOutBuff)(audioCodecHandle h);

// set the working output buffer
typedef void		(*pSetOutBuff)(audioCodecHandle h, void* pBuff);

// get the number of encoded/decoded bytes and clear internal counter
typedef int		(*pGetOutBytes)(audioCodecHandle h, int clr);

// check if options are supported
typedef AUDIO_RES	(*pCheckCodecFormat)(sStreamInfo* pInfo);




// encoding/recording stuff

// number of samples needed in the encoder input buffer
typedef int		(*pEncoderInSamples)(int procSamples, sStreamInfo* pInfo);

// init the encoder
typedef AUDIO_RES	(*pInitEncoder)(int nProcSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl);

// start encoding data
typedef AUDIO_RES	(*pEncodeWriteData)(audioCodecHandle h, int nBytes, progressDcpt* pPrgDcpt);


// get the input work buffer size
typedef int		(*pGetInBuffSize)(audioCodecHandle h);

// get the working input buffer
typedef void*		(*pGetInBuff)(audioCodecHandle h);


// set the working output buffer
typedef void		(*pSetInBuff)(audioCodecHandle h, void* pBuff);

// clean-up, after done
typedef void		(*pEncoderCleanup)(audioCodecHandle h);

// stop decoder. last chance to flush out some data that's already in the internal buffers
typedef AUDIO_RES	(*pEncoderStop)(audioCodecHandle h);


// in order to support multiple formats we try to standardize the functions we need
//

typedef struct
{
	int			decodeWhilePlay;	// real time decoding capability
	pDecoderOutSamples	decoderOutSamples;
	pInitDecoder		decoderInit;
	pReadDecodeData		decoderRProcData;
	pDecoderCleanup		decoderCleanup;
	pGetOutBuffSize		decoderGetOutSize;
	pGetOutBuff		decoderGetOutBuff;
	pGetOutBytes		decoderGetOutBytes;	
	pSetOutBuff		decoderSetOutBuff;
	pCheckCodecFormat	decoderCheckFormat;
}AudioDecDcpt;		// decoder descriptor


typedef struct
{
	int			encodeWhileRecord;	// real time encoding capability
	pEncoderInSamples	encoderInSamples;
	pInitEncoder		encoderInit;
	pEncodeWriteData	encoderProcWData;
	pEncoderStop		encoderStop;
	pEncoderCleanup		encoderCleanup;
	pGetInBuffSize		encoderGetInSize;
	pGetInBuff		encoderGetInBuff;
	pSetInBuff		encoderSetInBuff;
	pGetOutBytes		encoderGetOutBytes;
	pCheckCodecFormat	encoderCheckFormat;
}AudioEncDcpt;		// encoder descriptor



#endif	// _AUDIO_FORMATS_H_

