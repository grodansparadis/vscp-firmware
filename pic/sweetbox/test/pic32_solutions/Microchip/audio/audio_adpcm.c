/*********************************************************************
 *
 *                  AUDIO ADPCM wrapper
 *
 *********************************************************************
 * FileName:        audio_adpcm.c
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

#include "audio_adpcm.h"

#if defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)

#ifdef ADPCM_ENCODING	

// local definitions
//

typedef struct 
{
	int			prevsample;		/* Predicted adpcm sample */
	int			previndex;		/* Index into step size table */
}AdpcmState;



typedef struct
{
	AdpcmState	state;		// internal state of the encoder/decoder btw successive operations
	int		sLE;		// samples in LE/BE format
	void*		inBuff;		// internal buffer holding  at least nProcSamples packed ADPCM samples
	int		inBuffSize;
	int		nInBytes;	// currently processed bytes
	void*		outBuff;	// output buffer
	int		outBuffSize;
	int		nOutBytes;	// result bytes (encoding/decoding)
}AdpcmCodecDcpt;	// adpcm codec descriptor
		


/* Table of index changes */
static const int indexTable[16] = 
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8,
};

/* Quantizer step size lookup table */
static const int stepSizeTable[89] = 
{
   7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
   19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
   50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
   130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
   337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
   876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
   2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
   5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
   15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};


// local functions

#ifdef AUDIO_SUPPORT_PLAY
static short		ADPCMDecodeSample(unsigned char code, AdpcmState *state);
static void		ADPCMDecodeBuffer(AdpcmCodecDcpt* pDcpt);
#endif

#ifdef AUDIO_SUPPORT_RECORD
static unsigned char	ADPCMEncodeSample(short sample, AdpcmState *state);
static void		ADPCMEncodeBuffer(AdpcmCodecDcpt* pDcpt);
#endif

static int		AdpcmAllocateBuffers(AdpcmCodecDcpt* pDcpt, eBuffMask buffMask);


/*************************************
 * 		Interface functions
 * 		**************************************************/

#ifdef AUDIO_SUPPORT_PLAY

// returns the number of samples needed in the decoder out buffer
int AdpcmDecoderOutSamples(int procSamples, sStreamInfo* pInfo)
{
	return procSamples;
}


// init the decoder
// Note: uses the current endianess and callbacks in the audio library! 
AUDIO_RES AdpcmInitDecoder(int nProcSamples, sStreamInfo* pInfo,  eBuffMask buffMask, audioCodecHandle* pHndl)
{
	AdpcmCodecDcpt*	pDcpt;

	*pHndl=0;

	pDcpt=(AdpcmCodecDcpt*)calloc(1, sizeof(*pDcpt));
	if(!pDcpt)
	{
		return AUDIO_RES_OUT_OF_MEM;
	}
	
	pDcpt->inBuffSize=nProcSamples/2;			// 2 adpcm samples packed in a byte
	pDcpt->outBuffSize=nProcSamples*sizeof(short);	// an output sample is 16 bits	
	pDcpt->sLE=AudioGetAdpcmEndianess();

	if(!AdpcmAllocateBuffers(pDcpt, buffMask))
	{
		return AUDIO_RES_OUT_OF_MEM;
	}
		
	*pHndl=pDcpt;
	return AUDIO_RES_OK;
}


// start decoding data
AUDIO_RES AdpcmReadDecodeData(audioCodecHandle h, progressDcpt* pPrgDcpt)
{
	AUDIO_RES	res;
	AdpcmCodecDcpt*	pDcpt=(AdpcmCodecDcpt*)h;

	AudioStreamReadData(AudioGetInStream(), pDcpt->inBuff, pDcpt->inBuffSize, &pDcpt->nInBytes);

	if(pDcpt->nInBytes)
	{
		ADPCMDecodeBuffer(pDcpt);
		res=AUDIO_RES_OK;
	}
	else
	{
		res=AUDIO_RES_DONE;	// we're done
	}

	pDcpt->nOutBytes=pDcpt->nInBytes*4;
	return res;
}

// get the output work buffer size
int AdpcmGetOutBuffSize(audioCodecHandle h)
{
	return ((AdpcmCodecDcpt*)h)->outBuffSize;
}

// get the working output buffer
void* AdpcmGetOutBuff(audioCodecHandle h)
{
	return ((AdpcmCodecDcpt*)h)->outBuff;
}


// set the working output buffer
void AdpcmSetOutBuff(audioCodecHandle h, void* pBuff)
{
	((AdpcmCodecDcpt*)h)->outBuff=pBuff;
}

#endif	// AUDIO_SUPPORT_PLAY


#ifdef AUDIO_SUPPORT_RECORD
// returns the number of samples needed in the encoder input buffer
int AdpcmEncoderInSamples(int procSamples, sStreamInfo* pInfo)
{
	return procSamples; 
}


// init the encoder
// Note: uses the current endianess and callbacks in the audio library!
AUDIO_RES AdpcmInitEncoder(int nProcSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl)
{
	AdpcmCodecDcpt*	pDcpt;

	*pHndl=0;
	
	pDcpt=(AdpcmCodecDcpt*)calloc(1, sizeof(*pDcpt));
	if(!pDcpt)
	{
		return AUDIO_RES_OUT_OF_MEM;
	}

	pDcpt->outBuffSize=nProcSamples/2;			// 2 output adpcm samples packed in a byte
	pDcpt->inBuffSize=nProcSamples*sizeof(short);	// an input sample is 16 bits	
	pDcpt->sLE=AudioGetAdpcmEndianess();
	

	if(!AdpcmAllocateBuffers(pDcpt, buffMask))
	{
		return AUDIO_RES_OUT_OF_MEM;
	}

	*pHndl=pDcpt;
	return AUDIO_RES_OK;
}

// start decoding data
AUDIO_RES AdpcmEncodeWriteData(audioCodecHandle h, int nBytes, progressDcpt* pPrgDcpt)
{
	AdpcmCodecDcpt*	pDcpt=(AdpcmCodecDcpt*)h;

	if((pDcpt->nInBytes=nBytes))
	{
		ADPCMEncodeBuffer(pDcpt);
	}
	
	pDcpt->nOutBytes=nBytes/4;

	if(pDcpt->nOutBytes)
	{
		int nWritten;
		AudioStreamWriteData(AudioGetOutStream(), pDcpt->outBuff, pDcpt->nOutBytes, &nWritten);	// ask the user to save the data
		if(nWritten !=pDcpt->nOutBytes)
		{	// some error must have occurred...
			return AUDIO_RES_WRITE_ERROR;
		}
	}
	
	return AUDIO_RES_OK;	// we're done
}


// get the output work buffer size
int AdpcmGetInBuffSize(audioCodecHandle h)
{
	return ((AdpcmCodecDcpt*)h)->inBuffSize;
}

// get the working input buffer
void* AdpcmGetInBuff(audioCodecHandle h)
{
	return ((AdpcmCodecDcpt*)h)->inBuff;
}


// set the working output buffer
void AdpcmSetInBuff(audioCodecHandle h, void* pBuff)
{
	((AdpcmCodecDcpt*)h)->inBuff=pBuff;
}
#endif	// AUDIO_SUPPORT_RECORD


// get the encoded/decoded bytes and clear internal counter
int AdpcmGetOutBytes(audioCodecHandle h, int clr)
{
	int nBytes=((AdpcmCodecDcpt*)h)->nOutBytes;
	if(clr)
	{
		((AdpcmCodecDcpt*)h)->nOutBytes=0;
	}
	return nBytes;
}

// check the play/record format
AUDIO_RES AdpcmCheckFormat(sStreamInfo* pInfo)
{
	if(pInfo->numChannels!=1 || pInfo->bitsPerSample!=4 || !pInfo->lEndian)
	{
		return AUDIO_RES_UNSUPPORTED;
	}

	return AUDIO_RES_OK;
	
}

// clean-up, after done
void AdpcmCleanUpDecoder(audioCodecHandle h)
{
	AdpcmCodecDcpt*	pDcpt;
	
	if((pDcpt=(AdpcmCodecDcpt*)h))
	{
		free(pDcpt->inBuff);
		free(pDcpt->outBuff);
		free(pDcpt);
	}
}

/////////////////////////////////////////////// local functions ////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


static int AdpcmAllocateBuffers(AdpcmCodecDcpt* pDcpt, eBuffMask buffMask)
{
	int	allocFail=0;
	
	if(buffMask&CODEC_ALLOCATE_INOUT)
	{	
		if(buffMask&CODEC_ALLOCATE_IN)
		{
			if(!(pDcpt->inBuff=malloc(pDcpt->inBuffSize)))
			{
				allocFail=1;
			}
		}
		if(buffMask&CODEC_ALLOCATE_OUT)
		{
			if(!(pDcpt->outBuff=malloc(pDcpt->outBuffSize)))
			{
				allocFail=1;
			}
		}

		if(allocFail)
		{
			free(pDcpt->inBuff);
			free(pDcpt->outBuff);
			free(pDcpt);
		}
	}

	return !allocFail;	
}




/****************************************************************************
 * Function:        ADPCMDecodeBuffer
 *
 * PreCondition:    dstBuff, srcBuff valid pointers
 * 					dstBuff has to hold nSamples 16 bit samples
 *
 * Input:           pDcpt	- descriptor pointing to buffer to store the 16 bit decoded samples et all.
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function decodes a buffer of packed ADPCM samples
 * 					into 16-bit speech samples.
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
static void ADPCMDecodeBuffer(AdpcmCodecDcpt* pDcpt)
{
	short* dstBuff=(short*)pDcpt->outBuff;
	unsigned char* srcBuff=(unsigned char*)pDcpt->inBuff;
	int nBytes=pDcpt->nInBytes;
	unsigned char	pChar;	// packed ADPCM character

	if(pDcpt->sLE)
	{
		while(nBytes--)
		{
			pChar = *srcBuff++;
			*dstBuff++=ADPCMDecodeSample(pChar&0x0f, &pDcpt->state);
			*dstBuff++=ADPCMDecodeSample((pChar>>4)&0x0f, &pDcpt->state);
		}
	}
	else
	{
		while(nBytes--)
		{
			pChar = *srcBuff++;
			*dstBuff++=ADPCMDecodeSample((pChar>>4)&0x0f, &pDcpt->state);
			*dstBuff++=ADPCMDecodeSample(pChar&0x0f, &pDcpt->state);
		}
	}

	
}
#endif

/****************************************************************************
 * Function:        ADPCMEncodeBuffer
 *
 * PreCondition:    ADPCM encoder properly initialized
 *
 * Input:           pDcpt	- descriptor pointing to buffer to store the 4 bit encoded samples et all.
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function decodes a buffer of packed ADPCM samples
 * 					into 16-bit speech samples.
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
static void ADPCMEncodeBuffer(AdpcmCodecDcpt* pDcpt)
{
	unsigned char	adPCMLo;	// the encoded value
	unsigned char	adPCMHi;	// the encoded value
	unsigned char* dstBuff=(unsigned char*)pDcpt->outBuff;
	short* srcBuff= (short*)pDcpt->inBuff;
	int	nSamples=pDcpt->nInBytes/2;

	if(pDcpt->sLE)
	{	
		while(nSamples/2)
		{
			adPCMHi=ADPCMEncodeSample( *srcBuff++, &pDcpt->state);
			adPCMLo=ADPCMEncodeSample( *srcBuff++, &pDcpt->state);
	   		*dstBuff++=adPCMHi|(adPCMLo<<4);	// pack the ADPCM sample
			nSamples-=2;
		}
	}
	else
	{
		while(nSamples/2)
		{
			adPCMHi=ADPCMEncodeSample( *srcBuff++, &pDcpt->state);
			adPCMLo=ADPCMEncodeSample( *srcBuff++, &pDcpt->state);
			*dstBuff++=adPCMLo|(adPCMHi<<4);
			nSamples-=2;
		}
	}
	
}
#endif

/****************************************************************************
 * Function:        ADPCMDecodeSample
 *
 * PreCondition:    adpcm State has to configured
 *
 * Input:           unsigned char code - 8-bit number containing the 4-bit
 *                  ADPCM code
 *                  AdpcmState *state - ADPCM structure 
 *
 * Output:          short - 16-bit unsigned speech sample 
 *
 * Side Effects:    None
 *
 * Overview:        This function decodes the encoded ADPCM values. It 
 *                  recieves the 8-bit number containing 4-bit ADPCM code 
 *                  and return the 16 bit speech sample. Refer the
 *                  the comments given inline for more details of operation.
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
static short ADPCMDecodeSample(unsigned char code, AdpcmState *state)
{
   int step;		/* Quantizer step size */
   int predsample;		/* Output of ADPCM predictor */
   int diffq;		/* Dequantized predicted difference */
   int index;		/* Index into step size table */

   /* Restore previous values of predicted sample and quantizer step size index */
   predsample = state->prevsample;
   index = state->previndex;

   /* Find quantizer step size from lookup table using index */
   step = stepSizeTable[index];

   /* Inverse quantize the ADPCM code into a difference using the quantizer step size */
   diffq = step >> 3;
   if( code & 4 )
   {
      diffq += step;
   }
   if( code & 2 )
   {
      diffq += step >> 1;
   }
   if( code & 1 )
   {
      diffq += step >> 2;
   }

   /* Add the difference to the predicted sample */
   if( code & 8 )
   {
      predsample -= diffq;
   }
   else
   {
      predsample += diffq;
   }


	if(predsample>32767)
	{
		predsample=32767;
	}
	else if(predsample<-32768)
	{
		predsample=-32768;
	}
	


   /* Find new quantizer step size by adding the old index and a
      table lookup using the ADPCM code */
   index += indexTable[code];

   /* Check for overflow of the new quantizer step size index */
   if( index < 0 )
   {
      index = 0;
   }
   if( index > 88 )
   {
      index = 88;
   }


   /* Save predicted sample and quantizer step size index for next iteration */
   state->prevsample = predsample;
   state->previndex = index;

   /* Return the new speech sample */
   return (short)predsample;

}
#endif	// AUDIO_SUPPORT_PLAY


/****************************************************************************
 * Function:        ADPCMEncodeSample
 *
 * PreCondition:    adpcm State has to configured
 *
 * Input:           short sample - 16-bit signed speech sample  
 *
 * Output:          char - 8-bit number containing the 4-bit ADPCM code 
 *
 * Side Effects:    None
 *
 * Overview:        This function encode the 16-bit speech sample into
 *                  8-bit code containing 4-bit ADPCM value. Refer the
 *                  the comments given inline for more details of operation.
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
static unsigned char ADPCMEncodeSample( short sample, AdpcmState *state)
{

   int step;		/* Quantizer step size */
   int predsample;		/* Output of ADPCM predictor */
   int diffq,diff;		/* Dequantized predicted difference */
   int code;
   int index;		/* Index into step size table */

	// Restore previous values of predicted sample and quantizer step
	// size index
	predsample = state->prevsample;
	index = state->previndex;
	step = stepSizeTable[index];

	// Compute the difference between the acutal sample (sample) and the
	// the predicted sample (predsample)
	diff = sample - predsample;
	if(diff >= 0)
	{
		code = 0;
	}
	else
	{
		code = 8;
		diff = -diff;
	}

	// Quantize the difference into the 4-bit ADPCM code using the
	// the quantizer step size
	// Inverse quantize the ADPCM code into a predicted difference
	// using the quantizer step size
	diffq = step >> 3;
	if( diff >= step )
	{
		code |= 4;
		diff -= step;
		diffq += step;
	}
	step >>= 1;
	if( diff >= step )
	{
		code |= 2;
		diff -= step;
		diffq += step;
	}
	step >>= 1;
	if( diff >= step )
	{
		code |= 1;
		diffq += step;
	}

	// Fixed predictor computes new predicted sample by adding the
	// old predicted sample to predicted difference
	if( code & 8 )
	{
		predsample -= diffq;
	}
	else
	{
		predsample += diffq;
	}


	if(predsample<-32768)
	{
		predsample=-32768;
	}
	else if(predsample>32767)
	{
		predsample=32767;
	}
	
	// Find new quantizer stepsize index by adding the old index
	// to a table lookup using the ADPCM code
	index += indexTable[code];

	// Check for overflow of the new quantizer step size index
	if( index < 0 )
	{
		index = 0;
	}
	if( index > 88 )
	{
		index = 88;
	}

	// Save the predicted sample and quantizer step size index for
	// next iteration
	state->prevsample = predsample;
	state->previndex = index;

	// Return the new ADPCM code
	return ( code & 0x0f );
}
#endif	// AUDIO_SUPPORT_PLAY

#endif	// ADPCM_ENCODING	

#endif	// defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)




