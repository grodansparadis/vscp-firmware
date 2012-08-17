/*********************************************************************
 *
 *                  AUDIO PCM wrapper
 *
 *********************************************************************
 * FileName:        audio_pcm.c
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

#include <stdlib.h>
#include <GenericTypeDefs.h>

#include <audio/audio.h>
#include "audio_streams.h"

#include "audio_pcm.h"



#if defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)


#ifdef PCM_ENCODING	

// local definitions
//


typedef struct
{
	int		buffSize;
	int		nBytes;		// currently processed bytes
	void*		inBuff;		// internal buffer holding  in/out pcm samples
	void*		outBuff;		// internal buffer holding  in/out pcm samples
}pcmCodecDcpt;	// pcm codec descriptor
		

/*************************************
 * 		Interface functions
 * 		**************************************************/

#ifdef AUDIO_SUPPORT_PLAY

// returns the number of samples needed in the decoder out buffer
int PcmDecoderOutSamples(int procSamples, sStreamInfo* pInfo)
{
	return procSamples;
}


// init the decoder
// Note: uses the current callbacks in the audio library! 
AUDIO_RES PcmInitDecoder(int nProcSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl)
{
	pcmCodecDcpt*	pDcpt;

	*pHndl=0;

	pDcpt=(pcmCodecDcpt*)calloc(1, sizeof(*pDcpt));
	if(!pDcpt)
	{
		return AUDIO_RES_OUT_OF_MEM;
	}

	pDcpt->buffSize=nProcSamples*sizeof(short);	// a sample is 16 bits	

	if(buffMask&CODEC_ALLOCATE_OUT)
	{	
		pDcpt->outBuff=malloc(pDcpt->buffSize);

		if(!pDcpt->outBuff)
		{
			free(pDcpt);
			return AUDIO_RES_OUT_OF_MEM;
		}
	}
	
	*pHndl=pDcpt;
	return AUDIO_RES_OK;
}

// start decoding data
AUDIO_RES PcmReadDecodeData(audioCodecHandle h, progressDcpt* pPrgDcpt)
{
	pcmCodecDcpt*	pDcpt=(pcmCodecDcpt*)h;
	
	AudioStreamReadData(AudioGetInStream(), pDcpt->outBuff, pDcpt->buffSize, &pDcpt->nBytes);

	return pDcpt->nBytes?AUDIO_RES_OK:AUDIO_RES_DONE;
}


// get the decoder output buffer
void* PcmGetOutBuff(audioCodecHandle h)
{
	return ((pcmCodecDcpt*)h)->outBuff;
}

// set the working decoder output buffer
void PcmSetOutBuff(audioCodecHandle h, void* pBuff)
{
	((pcmCodecDcpt*)h)->outBuff=pBuff;
}
#endif	// AUDIO_SUPPORT_PLAY



#ifdef AUDIO_SUPPORT_RECORD
// set the working encoder input buffer
void PcmSetInBuff(audioCodecHandle h, void* pBuff)
{
	((pcmCodecDcpt*)h)->inBuff=pBuff;
}


// returns the number of samples needed in the encoder input buffer
int PcmEncoderInSamples(int procSamples, sStreamInfo* pInfo)
{
	return procSamples;
}

// get the encoder input buffer
void* PcmGetInBuff(audioCodecHandle h)
{
	return ((pcmCodecDcpt*)h)->inBuff;
}


// init the decoder
// Note: uses the current callbacks in the audio library!
AUDIO_RES PcmInitEncoder(int nProcSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl)
{
	pcmCodecDcpt*	pDcpt;

	*pHndl=0;
	
	pDcpt=(pcmCodecDcpt*)calloc(1, sizeof(*pDcpt));
	if(!pDcpt)
	{
		return AUDIO_RES_OUT_OF_MEM;
	}
	
	pDcpt->buffSize=nProcSamples*sizeof(short);	// an input sample is 16 bits	
	pDcpt->nBytes=0;
	
	if(buffMask&CODEC_ALLOCATE_IN)
	{	
		pDcpt->inBuff=malloc(pDcpt->buffSize);

		if(!pDcpt->inBuff)
		{
			free(pDcpt);
			return AUDIO_RES_OUT_OF_MEM;
		}
	}

	*pHndl=pDcpt;
	return AUDIO_RES_OK;
}

// start decoding data
AUDIO_RES PcmEncodeWriteData(audioCodecHandle h, int nBytes, progressDcpt* pPrgDcpt)
{
	pcmCodecDcpt*	pDcpt=(pcmCodecDcpt*)h;
	
	if((pDcpt->nBytes=nBytes))
	{
		int nWritten;
		AudioStreamWriteData(AudioGetOutStream(), pDcpt->inBuff, nBytes, &nWritten);	// ask the stream to save the data
		if(nWritten !=nBytes)
		{	// some error must have occurred...
			return AUDIO_RES_WRITE_ERROR;
		}
	}
	
	return AUDIO_RES_OK;	// we're done
}
#endif	// AUDIO_SUPPORT_RECORD


// get the in/out work buffer size
int PcmGetBuffSize(audioCodecHandle h)
{
	return ((pcmCodecDcpt*)h)->buffSize;
}

// get the encoded/decoded bytes and clear internal counter
int PcmGetBytes(audioCodecHandle h, int clr)
{
	int nBytes=((pcmCodecDcpt*)h)->nBytes;
	if(clr)
	{
		((pcmCodecDcpt*)h)->nBytes=0;
	}
	return nBytes;
}


// check the play/record format
AUDIO_RES PcmCheckFormat(sStreamInfo* pInfo)
{
	if(pInfo->numChannels!=1 || pInfo->bitsPerSample!=16 || !pInfo->lEndian)
	{
		return AUDIO_RES_UNSUPPORTED;
	}

	return AUDIO_RES_OK;
	
}

// clean-up, after done
void PcmCleanUpDecoder(audioCodecHandle h)
{
	pcmCodecDcpt* pDcpt;

	if((pDcpt=(pcmCodecDcpt*)h))
	{
		free(pDcpt->inBuff);
		free(pDcpt->outBuff);
		free(pDcpt);
	}
}

#endif	// ADPCM_ENCODING	

#endif	// defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)




