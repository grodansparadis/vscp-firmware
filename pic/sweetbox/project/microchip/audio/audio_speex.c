/*********************************************************************
 *
 *                  AUDIO speex wrapper
 *
 *********************************************************************
 * FileName:        audio_speex.c
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
 *$Id: audio_speex.c 161 2008-10-15 19:30:28Z akhe $
 ********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <GenericTypeDefs.h>

#include <audio/audio.h>

#include "audio_formats.h"
#include "audio_streams.h"

#include "audio_speex.h"

#if defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)

#ifdef SPEEX_ENCODING	


#include <speex/speex.h>



#define	SPX_CODEC_BUFF_MUL		6	// values to adjust the average encoder/decoder buffer size 
#define	SPX_CODEC_BUFF_DIV		5	// 20% will do
						// TODO: make it user selectable?

#define	SPX_MAX_FRAMES_PER_PACKET	10	// encoder setting. compatible with the speex library

#define	SPX_DEFAULT_FRAMES_PER_PACKET	1	// default setting

#define	SPX_STREAM_NO			1	// the speex stream when we encode in Ogg pages

#define	SPX_DEFAULT_PACKETS_PER_PAGE	10	// default number of packets per Ogg page


typedef struct
{
	// speex info
	SpeexBits	spxBits;		// codec control structure
	void*		spxState;		// codec state
	// input info	
	char*		inBuff;			// the input buffer
	int		inBuffSize;		// input buffer size	
	// output info
	char*		outBuff;		
	int		outBuffSize;
	char*		pOutFrame;		// current decoder pointer in the output buffer
	int		nOutBytes;		// bytes available in the output buffer 
}spxCdcDcpt;	// Speex codec descriptor
		// common part of encoder/decoder
		// since we don't take the trouble to have a type to distinguish between descriptors,
		// we have to have a common part


typedef struct
{
	spxCdcDcpt	cdc;			// common part
	// speex
	int		framesPerPacket;	// frames per ogg packet
	int		frameSize;		// size of the frames
	int		processedPktNo;		// counter of processed packets
	int		currPktNo;		// number of the currently received packet from the stream
	// input info	
	int		nInBytes;		// bytes available in the input buffer	
	
	// output info
	int		outFrameSize;
}spxDecDcpt;
// speex codec descriptor
// and global data for the Speex state machine

typedef struct
{
	spxCdcDcpt	cdc;			// common part
	// speex
	int		framesPerPacket;	// frames per ogg packet
	int		frameNo;		// encoded number of frames in the current output buffer, 0 - framesPerPacket
	int		currPktNo;		// number of the currently received packet from the stream

	// input info	
	int		inFrameSize;		// size of a frame	
	int		nInBytes;		// bytes available in the input buffer	
}spxEncDcpt;
// speex encoder descriptor
// and global data for the Speex encoder state machine




// static data
// 
static sSpxRunDcpt	SpxCtrl=
{
	SPX_STREAM_NO,				// streamNo
	0,					// streamVer
	//	
	0,					// frameSamples
	-1,					// bitRate
	//
	SPX_DEFAULT_FRAMES_PER_PACKET,		// framesPerPacket
	SPX_DEFAULT_PACKETS_PER_PAGE,		// packetsPerPage
	2,					// complexity
	3,					// qualFactor
	//
	0,					// WB|VBR|BRATE_OVR|PRCPT_ENH
};


//
//
// local prototypes

static int		SpxAllocateBuffers(spxCdcDcpt* pDcpt, eBuffMask buffMask);


#ifdef AUDIO_SUPPORT_RECORD
#ifdef SPEEX_OGG_STREAMS
static AUDIO_RES	SpxEncodeWriteOggData(spxEncDcpt* pDcpt, int avlBytes, progressDcpt* pPrgDcpt, audioSHandle ostream);
#endif	// SPEEX_OGG_STREAMS

#ifdef FREE_STREAMS
static AUDIO_RES	SpxEncodeWriteFreeData(spxEncDcpt* pDcpt, int avlBytes, progressDcpt* pPrgDcpt, audioSHandle ostream);
#endif	// FREE_STREAMS
#endif	// AUDIO_SUPPORT_RECORD


/*****************************************
 *              Interface functions
 ************************************************************/

#ifdef AUDIO_SUPPORT_PLAY
// turn on/off speex perceptual enhancement
int AudioSpxSetPrcptEnh(int enh)
{
	int oldEnh=SpxCtrl.spxFlags&SPX_FLAG_PRCPT_ENH;
	if(enh)
	{
		SpxCtrl.spxFlags|=SPX_FLAG_PRCPT_ENH;
	}
	else
	{
		SpxCtrl.spxFlags&=~SPX_FLAG_PRCPT_ENH;
	}
	return oldEnh;
}

// get the output work buffer size
int SpxGetOutBuffSize(audioCodecHandle h)
{
	return ((spxCdcDcpt*)h)->outBuffSize;
}
// get the working output buffer
void* SpxGetOutBuff(audioCodecHandle h)
{
	return ((spxCdcDcpt*)h)->outBuff;
}

// set the working output buffer
void SpxSetOutBuff(audioCodecHandle h, void* pBuff)
{
	((spxCdcDcpt*)h)->outBuff=(char*)pBuff;
	((spxCdcDcpt*)h)->pOutFrame=(char*)pBuff;
}


// check the play format
// no free streams supported for Speex. Frame info needed!
AUDIO_RES SpxCheckPlayFormat(sStreamInfo* pInfo)
{
	AUDIO_RES	res;
	int		fmtFail;

#if defined(FREE_STREAMS) && defined(SPEEX_OGG_STREAMS)
	fmtFail=(pInfo->sFmt!=AUDIO_STREAM_OGG_SPEEX && pInfo->sFmt!=AUDIO_STREAM_FREE);
#elif defined(SPEEX_OGG_STREAMS)
	fmtFail=(pInfo->sFmt!=AUDIO_STREAM_OGG_SPEEX);
#elif defined(FREE_STREAMS)
	fmtFail=(pInfo->sFmt!=AUDIO_STREAM_FREE);
#endif

	fmtFail|=(pInfo->numChannels!=1 || pInfo->lEndian==0);
	
	if(!fmtFail)
	{	
		sSpxRunDcpt runDcpt;
		
		res=AudioStreamGetRunInfo(AudioGetInStream(), &runDcpt, sizeof(runDcpt));
	
		if(res==AUDIO_RES_OK)
		{
		#if defined(SPEEX_WIDEBAND)	
			fmtFail|=((runDcpt.spxFlags&SPX_FLAG_VBR)!=0);
		#else	
			fmtFail|=((runDcpt.spxFlags&(SPX_FLAG_VBR|SPX_FLAG_WB))!=0);
		#endif // SPEEX_WIDEBAND
		}
		else
		{
			fmtFail=1;
		}
	}

	return fmtFail?AUDIO_RES_UNSUPPORTED:AUDIO_RES_OK;	
}


AUDIO_RES SpxInitDecoder(int procSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl)
{
	sSpxRunDcpt	runDcpt;
	int		outFrameSamples;
	int		sampleRate;
	int		inBuffSize;
	int		compression;
	int		prcptEnh;
	spxDecDcpt*	pDDcpt;

	*pHndl=0;

	pDDcpt=(spxDecDcpt*)calloc(1, sizeof(*pDDcpt));
	if(!pDDcpt)
	{
		return AUDIO_RES_OUT_OF_MEM;		
	}

	AudioStreamGetRunInfo(AudioGetInStream(), &runDcpt, sizeof(runDcpt));
	
	speex_bits_init(&pDDcpt->cdc.spxBits);
#ifdef SPEEX_WIDEBAND	
	pDDcpt->cdc.spxState=speex_decoder_init((runDcpt.spxFlags&SPX_FLAG_WB)?&speex_wb_mode:&speex_nb_mode);
#else
	pDDcpt->cdc.spxState=speex_decoder_init(&speex_nb_mode);	
#endif	// SPEEX_WIDEBAND

	if(!pDDcpt->cdc.spxState)
	{	// allocation problems
		SpxCleanUpDecoder(pDDcpt);
		return AUDIO_RES_OUT_OF_MEM;		
	}
		
	
	prcptEnh=(runDcpt.spxFlags&SPX_FLAG_PRCPT_ENH)!=0;
	speex_decoder_ctl(pDDcpt->cdc.spxState, SPEEX_SET_ENH, &prcptEnh);

	sampleRate=pInfo->sampleRate;
	speex_decoder_ctl(pDDcpt->cdc.spxState, SPEEX_SET_SAMPLING_RATE, &sampleRate);


	compression=(pInfo->sampleRate*16)/pInfo->bitRate;
	inBuffSize=(runDcpt.framesPerPacket*runDcpt.frameSamples*sizeof(short))/compression;
	inBuffSize=(inBuffSize*SPX_CODEC_BUFF_MUL)/SPX_CODEC_BUFF_DIV;
	pDDcpt->cdc.inBuffSize=(inBuffSize+7)&0xfffffff8;	// 8 multiple

	outFrameSamples=runDcpt.framesPerPacket*runDcpt.frameSamples;	// Always process full packets
	// round up input size to an integer number of frames taken by the encoder
	pDDcpt->cdc.outBuffSize=((procSamples+outFrameSamples-1)/outFrameSamples)*outFrameSamples;
	
	pDDcpt->cdc.outBuffSize*=sizeof(short);
	pDDcpt->outFrameSize=outFrameSamples*sizeof(short);	// convert to bytes


	if(!SpxAllocateBuffers(&pDDcpt->cdc, buffMask))
	{
		SpxCleanUpDecoder(pDDcpt);
		return AUDIO_RES_OUT_OF_MEM;
	}	

	pDDcpt->nInBytes=pDDcpt->cdc.nOutBytes=0;	// show input buffer empty
	pDDcpt->cdc.pOutFrame=pDDcpt->cdc.outBuff;	// set the out pointers

	pDDcpt->framesPerPacket=runDcpt.framesPerPacket;
	pDDcpt->frameSize=runDcpt.frameSamples*sizeof(short);
	pDDcpt->processedPktNo=pDDcpt->currPktNo=0;
	
	*pHndl=pDDcpt;

	return AUDIO_RES_OK;

}


// read input data and decode to output buffer
AUDIO_RES SpxReadDecodeData(audioCodecHandle h, progressDcpt* pPrgDcpt)
{
const	sStreamInfo*	pInfo;
	audioSHandle	istream;
	sSpxPktDcpt	pktDcpt;
	int		decFault;
	int		frameIx;
	spxDecDcpt*	pDcpt=(spxDecDcpt*)h;
	AUDIO_RES	res=AUDIO_RES_OK;

	pInfo=AudioStreamGetInfo((istream=AudioGetInStream()));
	pDcpt->cdc.nOutBytes=0;
	
	while(res==AUDIO_RES_OK)
	{
		if(pDcpt->processedPktNo==pDcpt->currPktNo)
		{	// finished what we had to do; get a new packet
		#ifdef SPEEX_OGG_STREAMS
			if(pInfo->sFmt==AUDIO_STREAM_OGG_SPEEX)
			{	
				res=AudioStreamReadData(istream, pDcpt->cdc.inBuff, pDcpt->cdc.inBuffSize, &pktDcpt);
				if(res!=AUDIO_RES_OK)
				{
					break;
				}
				pDcpt->currPktNo=pktDcpt.pktSeqNo;
			}
		#endif	// SPEEX_OGG_STREAMS

		#ifdef FREE_STREAMS
			if(pInfo->sFmt==AUDIO_STREAM_FREE)
			{
				unsigned short	pktSize;
				int		nBytes;
				
				res=AudioStreamReadData(istream, &pktSize, sizeof(pktSize), &nBytes);
				if(res==AUDIO_RES_OK)
				{
					if(!nBytes)
					{
						res=AUDIO_RES_DONE;	// eos
					}
					else if(nBytes!=sizeof(pktSize))
					{
						res=AUDIO_RES_READ_ERROR;
					}
					else
					{
						res=AudioStreamReadData(istream, pDcpt->cdc.inBuff, pktSize, &nBytes);
						if(res==AUDIO_RES_OK && nBytes!=pktSize)
						{
							res=AUDIO_RES_READ_ERROR;
						}
						pktDcpt.pktBytes=nBytes;
						pDcpt->currPktNo++;	// no sequence control in free stream
					}
				}

				if(res!=AUDIO_RES_OK)
				{
					break;
				}
			}
		#endif	// FREE_STREAMS
			speex_bits_read_from(&pDcpt->cdc.spxBits, pDcpt->cdc.inBuff, pktDcpt.pktBytes);
		}
		// else we miss some packets, no need to read new data, just try to catch up
		
		for(frameIx=0; frameIx<pDcpt->framesPerPacket; frameIx++)
		{
			// decode a frame
			if(pDcpt->currPktNo==pDcpt->processedPktNo+1)
			{ // in sync
				decFault=speex_decode_int(pDcpt->cdc.spxState, &pDcpt->cdc.spxBits, (short*)pDcpt->cdc.pOutFrame);
			}
			else
			{ // lost frames, let decoder guess
				decFault=speex_decode_int(pDcpt->cdc.spxState, 0, (short*)pDcpt->cdc.pOutFrame);
			}
			
			if(decFault==-1)
			{
				res=AUDIO_RES_DONE;	// eos
				break;
			}
			else if(decFault==-2)
			{	//error in the input stream ?
				res=AUDIO_RES_DSTREAM_ERROR;
				break;
			}

			/*if(speex_bits_remaining(pDcpt->cdc.spxState)<0)
			{	// overflow...
				res=AUDIO_RES_DSTREAM_ERROR;	// TODO
				break;
			}*/				

			pDcpt->cdc.pOutFrame+=pDcpt->frameSize;
			pDcpt->cdc.nOutBytes+=pDcpt->frameSize;	// update the bytes in the output buffer

		}

		if(res==AUDIO_RES_OK)
		{
			pDcpt->processedPktNo++;	// packet processed
		
			if(pDcpt->cdc.nOutBytes+pDcpt->outFrameSize>pDcpt->cdc.outBuffSize)			
			{	// no room for another frame, output buffer has to be flushed
				pDcpt->cdc.pOutFrame=pDcpt->cdc.outBuff;	// reset pointers
				break;
			}
		
			if(pPrgDcpt)	
			{
				if(++pPrgDcpt->progressCnt==pPrgDcpt->progressStep)
				{
					(*pPrgDcpt->progressFnc)(pPrgDcpt->progressCnt);
					pPrgDcpt->progressCnt=0;
				}
			}
		}
	}

	return res;

}

int SpxDecoderOutSamples(int procSamples, sStreamInfo* pInfo)
{
	SpeexBits	spxBits;		// codec control structure
	void*		spxState;		// codec state
	int		sampleRate;
	sSpxRunDcpt	runDcpt;
	int		outSamples;
	int		prcptEnh;

	AudioStreamGetRunInfo(AudioGetInStream(), &runDcpt, sizeof(runDcpt));
	
	speex_bits_init(&spxBits);
#ifdef SPEEX_WIDEBAND	
	spxState=speex_decoder_init(runDcpt.spxFlags&SPX_FLAG_WB?&speex_wb_mode:&speex_nb_mode);
#else
	spxState=speex_decoder_init(&speex_nb_mode);	
#endif	// SPEEX_WIDEBAND

	prcptEnh=(SpxCtrl.spxFlags&SPX_FLAG_PRCPT_ENH)!=0;
	speex_decoder_ctl(spxState, SPEEX_SET_ENH, &prcptEnh);

	sampleRate=pInfo->sampleRate;
	speex_decoder_ctl(spxState, SPEEX_SET_SAMPLING_RATE, &sampleRate);

	outSamples=runDcpt.framesPerPacket*runDcpt.frameSamples;	// Always process full packets
	// round up input size to an integer number of frames taken by the encoder
	outSamples=((procSamples+outSamples-1)/outSamples)*outSamples;
	
	speex_bits_destroy(&spxBits);
	speex_decoder_destroy(spxState);

	return outSamples;

}



void SpxCleanUpDecoder(audioCodecHandle h)
{
	spxDecDcpt* pDDcpt;
	
	if((pDDcpt=(spxDecDcpt*)h))
	{
		speex_bits_destroy(&pDDcpt->cdc.spxBits);
		if(pDDcpt->cdc.spxState)
		{
			speex_decoder_destroy(pDDcpt->cdc.spxState);
		}

		free(pDDcpt->cdc.inBuff);
		free(pDDcpt->cdc.outBuff);
		free(pDDcpt);
	}
}


#endif	// AUDIO_SUPPORT_PLAY


#ifdef AUDIO_SUPPORT_RECORD

int AudioSpxSetFramesNo(int nFrames)
{
	int oldFrames=SpxCtrl.framesPerPacket;
	SpxCtrl.framesPerPacket=nFrames>SPX_MAX_FRAMES_PER_PACKET?SPX_MAX_FRAMES_PER_PACKET:nFrames;
	return oldFrames;
}

int AudioSpxSetPacketsNo(int nPackets)
{
	int oldPackets=SpxCtrl.packetsPerPage;
	SpxCtrl.packetsPerPage=nPackets;
	return oldPackets;
}

int AudioSpxSetComplexity(int cplxN)
{
	int oldN=SpxCtrl.complexity;

	SpxCtrl.complexity=cplxN;	
	
	return oldN;
}

int AudioSpxSetQuality(int newQual)
{
	int qFactor=SpxCtrl.qualFactor;
	SpxCtrl.qualFactor=(char)newQual;
	SpxCtrl.spxFlags&=~SPX_FLAG_BRATE_OVR;

	return qFactor;
}

int AudioSpxSetBitRate(int bRate)
{
	int oldRate=SpxCtrl.bitRate;
	SpxCtrl.bitRate=(short)bRate;
	SpxCtrl.spxFlags|=SPX_FLAG_BRATE_OVR;

	return oldRate;
}


// get the working input buffer
void* SpxGetInBuff(audioCodecHandle h)
{
	return ((spxCdcDcpt*)h)->inBuff;	
}

// get the input buffer size
int SpxGetInBuffSize(audioCodecHandle h)
{
	return ((spxCdcDcpt*)h)->inBuffSize;
}
	
// set the working input buffer
void SpxSetInBuff(audioCodecHandle h, void* pBuff)
{
	((spxCdcDcpt*)h)->inBuff=pBuff;	
}


// get the user settings
void AudioSpxGetSettings(sSpxRunDcpt* pRunDcpt)
{
	*pRunDcpt=SpxCtrl;
}


// check the record format
// no free streams supported for Speex. Frame info needed!
AUDIO_RES SpxCheckRecordFormat(sStreamInfo* pInfo)
{
	int		fmtFail;

#if defined(FREE_STREAMS) && defined(SPEEX_OGG_STREAMS)
	fmtFail=(pInfo->sFmt!=AUDIO_STREAM_OGG_SPEEX && pInfo->sFmt!=AUDIO_STREAM_FREE);
#elif defined(SPEEX_OGG_STREAMS)
	fmtFail=(pInfo->sFmt!=AUDIO_STREAM_OGG_SPEEX);
#elif defined(FREE_STREAMS)
	fmtFail=(pInfo->sFmt!=AUDIO_STREAM_FREE);
#endif

	fmtFail|=(pInfo->numChannels!=1 || pInfo->lEndian==0);

#if defined(SPEEX_WIDEBAND)
	fmtFail|=((SpxCtrl.spxFlags&SPX_FLAG_VBR)!=0);	
#else
	fmtFail|=((SpxCtrl.spxFlags&(SPX_FLAG_WB|SPX_FLAG_VBR))!=0);
#endif // SPEEX_WIDEBAND
	

	return fmtFail?AUDIO_RES_UNSUPPORTED:AUDIO_RES_OK;	
}



int SpxEncoderInSamples(int procSamples, sStreamInfo* pInfo)
{
	SpeexBits	spxBits;		// codec control structure
	void*		spxState;		// codec state
	int		frameSamples;
	int		inSamples;
	int		spxCtl;
	
	
	speex_bits_init(&spxBits);
#ifdef SPEEX_WIDEBAND
	spxState=speex_encoder_init((SpxCtrl.spxFlags&SPX_FLAG_WB)?&speex_wb_mode:&speex_nb_mode);
#else
	spxState=speex_encoder_init(&speex_nb_mode);		
#endif	// SPEEX_WIDEBAND

	spxCtl=SpxCtrl.complexity;
	speex_encoder_ctl(spxState, SPEEX_SET_COMPLEXITY, &spxCtl);
		
	spxCtl=SpxCtrl.qualFactor;
	speex_encoder_ctl(spxState, SPEEX_SET_QUALITY, &spxCtl);

	speex_encoder_ctl(spxState, SPEEX_SET_SAMPLING_RATE, &pInfo->sampleRate); // TODO: make sure 8000/16000 for nb/wb is enforced!
	
	speex_encoder_ctl(spxState, SPEEX_GET_FRAME_SIZE, &frameSamples);

	if(SpxCtrl.spxFlags&SPX_FLAG_BRATE_OVR)
	{		
		spxCtl=SpxCtrl.bitRate;
		speex_encoder_ctl(spxState, SPEEX_SET_BITRATE, &spxCtl);
	}
	else
	{
		speex_encoder_ctl(spxState, SPEEX_GET_BITRATE, &spxCtl);
		SpxCtrl.bitRate=spxCtl;
	}
	
	// input buffer holds just an integer number of frames
	inSamples=((procSamples+frameSamples-1)/frameSamples)*frameSamples;

	speex_bits_destroy(&spxBits);
	speex_encoder_destroy(spxState);
	
	return inSamples;

}


AUDIO_RES SpxInitEncoder(int procSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl)
{
	int		frameSamples;
	int		compression;
	int		outBuffSize;
	int		packetsPerPage;
	spxEncDcpt*	pEDcpt;
	int		spxCtl;

	
	*pHndl=0;
	
	pEDcpt=(spxEncDcpt*)calloc(1, sizeof(*pEDcpt));
	if(!pEDcpt)
	{
		return AUDIO_RES_OUT_OF_MEM;		
	}

	
	speex_bits_init(&pEDcpt->cdc.spxBits);
#ifdef SPEEX_WIDEBAND
	pEDcpt->cdc.spxState=speex_encoder_init((SpxCtrl.spxFlags&SPX_FLAG_WB)?&speex_wb_mode:&speex_nb_mode);
#else
	pEDcpt->cdc.spxState=speex_encoder_init(&speex_nb_mode);		
#endif	// SPEEX_WIDEBAND

	if(!pEDcpt->cdc.spxState)
	{	// allocation problems
		SpxCleanUpEncoder(pEDcpt);
		return AUDIO_RES_OUT_OF_MEM;		
	}
		
	spxCtl=SpxCtrl.complexity;
	speex_encoder_ctl(pEDcpt->cdc.spxState, SPEEX_SET_COMPLEXITY, &spxCtl);
		
	spxCtl=SpxCtrl.qualFactor;
	speex_encoder_ctl(pEDcpt->cdc.spxState, SPEEX_SET_QUALITY, &spxCtl);

	speex_encoder_ctl(pEDcpt->cdc.spxState, SPEEX_SET_SAMPLING_RATE, &pInfo->sampleRate); // TODO: make sure 8000/16000 for nb/wb is enforced!
	
	speex_encoder_ctl(pEDcpt->cdc.spxState, SPEEX_GET_FRAME_SIZE, &frameSamples);
	SpxCtrl.frameSamples=frameSamples;

	if(SpxCtrl.spxFlags&SPX_FLAG_BRATE_OVR)
	{
		spxCtl=SpxCtrl.bitRate;
		speex_encoder_ctl(pEDcpt->cdc.spxState, SPEEX_SET_BITRATE, &spxCtl);
	}
	else
	{
		speex_encoder_ctl(pEDcpt->cdc.spxState, SPEEX_GET_BITRATE, &spxCtl);
		SpxCtrl.bitRate=spxCtl;
	}

	
	// input buffer holds just an integer number of frames
	pEDcpt->cdc.inBuffSize=((procSamples+frameSamples-1)/frameSamples)*frameSamples*sizeof(short);	// convert to bytes

/*#ifdef FREE_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_FREE)*/
	{
		packetsPerPage=1;	// just framesPerPacket matter in free mode
	}
/*#endif	// FREE_STREAMS*/	// use 1 as default

	// however, the output buffer has to store a packetsPerPage packets, each framesPerPacket frames
#ifdef SPEEX_OGG_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_OGG_SPEEX)
	{
		packetsPerPage=SpxCtrl.packetsPerPage;
	}
#endif	// SPEEX_OGG_STREAMS
		

	outBuffSize=packetsPerPage*SpxCtrl.framesPerPacket*frameSamples*sizeof(short);
	compression=(pInfo->sampleRate*16)/SpxCtrl.bitRate;
	outBuffSize=((outBuffSize/compression)*SPX_CODEC_BUFF_MUL)/SPX_CODEC_BUFF_DIV;
	pEDcpt->cdc.outBuffSize=(outBuffSize+7)&0xfffffff8;	// 8 multiple
#ifdef FREE_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_FREE)
	{
		pEDcpt->cdc.outBuffSize+=sizeof(((sSpxFreeDataPkt*)pEDcpt)->pktSize);	// save room for packet size
	}
#endif	// FREE_STREAMS
	

	if(!SpxAllocateBuffers(&pEDcpt->cdc, buffMask))
	{
		SpxCleanUpEncoder(pEDcpt);
		return AUDIO_RES_OUT_OF_MEM;
	}	

	pEDcpt->inFrameSize=frameSamples*sizeof(short);	// size of an input frame
	pEDcpt->frameNo=pEDcpt->currPktNo=0;
	pEDcpt->framesPerPacket=SpxCtrl.framesPerPacket;

	pEDcpt->nInBytes=pEDcpt->cdc.nOutBytes=0;	// show input buffer empty

#ifdef SPEEX_OGG_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_OGG_SPEEX)
	{
		pEDcpt->cdc.pOutFrame=pEDcpt->cdc.outBuff;	// set the out pointer
	}
#endif	// SPEEX_OGG_STREAMS
	
#ifdef FREE_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_FREE)
	{
		pEDcpt->cdc.pOutFrame=((sSpxFreeDataPkt*)pEDcpt->cdc.outBuff)->spxData;	// set the out pointer
	}
#endif	// FREE_STREAMS
	

	
	SpxCtrl.streamNo=SPX_STREAM_NO;
#ifdef SPEEX_WIDEBAND
	SpxCtrl.streamVer=(SpxCtrl.spxFlags&SPX_FLAG_WB)?speex_wb_mode.bitstream_version:speex_nb_mode.bitstream_version;
#else
	SpxCtrl.streamVer=speex_nb_mode.bitstream_version;
#endif	// SPEEX_WIDEBAND

	*pHndl=pEDcpt;
	
	return AUDIO_RES_OK;

}


// read input data and encode to the output buffer
AUDIO_RES SpxEncodeWriteData(audioCodecHandle h, int avlBytes, progressDcpt* pPrgDcpt)
{	
const	sStreamInfo*	pInfo;
	audioSHandle	ostream;

	pInfo=AudioStreamGetInfo((ostream=AudioGetOutStream()));

#ifdef FREE_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_FREE)
	{
		return SpxEncodeWriteFreeData((spxEncDcpt*)h, avlBytes, pPrgDcpt, ostream);
	}
#endif	// FREE_STREAMS

#ifdef SPEEX_OGG_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_OGG_SPEEX)
	{
		return SpxEncodeWriteOggData((spxEncDcpt*)h, avlBytes, pPrgDcpt, ostream);
	}
#endif	// SPEEX_OGG_STREAMS

	return AUDIO_RES_BAD_CALL;	// shouldn't happen
}

#ifdef SPEEX_OGG_STREAMS
// read input data and encode to the output buffer
static AUDIO_RES SpxEncodeWriteOggData(spxEncDcpt* pDcpt, int avlBytes, progressDcpt* pPrgDcpt, audioSHandle ostream)
{	
	int		nBytes, leftBuffSpace;
	char*		pInFrame;
	eSpxEncCmd	outFlush;
	AUDIO_RES	res=AUDIO_RES_OK;

	
	pInFrame=pDcpt->cdc.inBuff;	// input buffer should contain whole frames	

	pDcpt->nInBytes=avlBytes;	
	while(pDcpt->nInBytes >= pDcpt->inFrameSize)
	{	// we encode but whole frames
		speex_bits_reset(&pDcpt->cdc.spxBits);
		speex_encode_int(pDcpt->cdc.spxState, (short*)pInFrame, &pDcpt->cdc.spxBits);
		
		nBytes=speex_bits_nbytes(&pDcpt->cdc.spxBits);	// read the required bytes after compression
		
		leftBuffSpace=pDcpt->cdc.outBuffSize-pDcpt->cdc.nOutBytes;	// space left in the output buffer
		
		if(nBytes>leftBuffSpace)
		{	// not enough room left in this buffer. shouldn't happen
			res=AUDIO_RES_BUFF_ERROR;
			break;
		}

		// extract the encoded frame to the output buffer and...another frame processed	
		pDcpt->cdc.nOutBytes+=(nBytes=speex_bits_write(&pDcpt->cdc.spxBits, pDcpt->cdc.pOutFrame, leftBuffSpace));
		pDcpt->cdc.pOutFrame+=nBytes;	// advance the output pointers

		if(++pDcpt->frameNo==pDcpt->framesPerPacket)
		{	// another packet assembled
			if(++pDcpt->currPktNo==SpxCtrl.packetsPerPage)
			{ // normally the page should be able to hold all the number of packets
				outFlush=SPX_CMD_PKT_FLUSH;
			}
			else
			{
				outFlush=SPX_CMD_PKT;
			}

			if((res=AudioStreamWriteData(ostream, pDcpt->cdc.outBuff, pDcpt->cdc.nOutBytes, &outFlush))!=AUDIO_RES_OK)
			{
				break;
			}

			if(outFlush==SPX_RPLY_FLUSHED)
			{	// this could happen because we specifically reached the number of packets per page
				// however, the stream itself could decide to flush the page if there were no more room...
				pDcpt->cdc.nOutBytes=pDcpt->currPktNo=0;
				pDcpt->cdc.pOutFrame=pDcpt->cdc.outBuff;	// reset the out pointers
			}

			pDcpt->frameNo=0;	
		}
		
		pInFrame+=pDcpt->inFrameSize;
		pDcpt->nInBytes-=pDcpt->inFrameSize;	// advance the input pointers
			
		if(pPrgDcpt)
		{
			if(++pPrgDcpt->progressCnt==pPrgDcpt->progressStep)
			{
				(*pPrgDcpt->progressFnc)(pPrgDcpt->progressCnt);
				pPrgDcpt->progressCnt=0;					
			}
		}
	}

	return res;

}
#endif	// SPEEX_OGG_STREAMS

#ifdef FREE_STREAMS
// read input data and encode to the output buffer
static AUDIO_RES SpxEncodeWriteFreeData(spxEncDcpt* pDcpt, int avlBytes, progressDcpt* pPrgDcpt, audioSHandle ostream)
{	
	int		nBytes, leftDataSpace;
	char*		pInFrame;
	AUDIO_RES	res=AUDIO_RES_OK;


	pInFrame=pDcpt->cdc.inBuff;	// input buffer should contain whole frames	

	pDcpt->nInBytes=avlBytes;	
	while(pDcpt->nInBytes >= pDcpt->inFrameSize)
	{	// we encode but whole frames
		speex_bits_reset(&pDcpt->cdc.spxBits);
		speex_encode_int(pDcpt->cdc.spxState, (short*)pInFrame, &pDcpt->cdc.spxBits);
		
		nBytes=speex_bits_nbytes(&pDcpt->cdc.spxBits);		// read the required bytes after compression
		
		// check the space left in the output buffer
		leftDataSpace=pDcpt->cdc.outBuffSize-sizeof(((sSpxFreeDataPkt*)0)->pktSize)-pDcpt->cdc.nOutBytes;
		
		if(nBytes>leftDataSpace)
		{	// not enough room left in this buffer. shouldn't happen
			res=AUDIO_RES_BUFF_ERROR;
			break;
		}

		// extract the encoded frame to the output buffer and...another frame processed	
		pDcpt->cdc.nOutBytes+=(nBytes=speex_bits_write(&pDcpt->cdc.spxBits, pDcpt->cdc.pOutFrame, leftDataSpace));
		pDcpt->cdc.pOutFrame+=nBytes;	// advance the output pointers

		if(++pDcpt->frameNo==pDcpt->framesPerPacket)
		{	// another packet assembled
			sSpxFreeDataPkt* pDataPkt=(sSpxFreeDataPkt*)pDcpt->cdc.outBuff;

			pDataPkt->pktSize=(unsigned short)pDcpt->cdc.nOutBytes;
						
			res=AudioStreamWriteData(ostream, (char*)pDataPkt, pDataPkt->pktSize+sizeof(pDataPkt->pktSize), &nBytes);
			if(res!=AUDIO_RES_OK)
			{
				break;
			}

			pDcpt->cdc.nOutBytes=0;
			pDcpt->cdc.pOutFrame=pDataPkt->spxData;	// reset the out pointers
			pDcpt->frameNo=0;	
		}
		
		pInFrame+=pDcpt->inFrameSize;
		pDcpt->nInBytes-=pDcpt->inFrameSize;	// advance the input pointers
			
		if(pPrgDcpt)
		{
			if(++pPrgDcpt->progressCnt==pPrgDcpt->progressStep)
			{
				(*pPrgDcpt->progressFnc)(pPrgDcpt->progressCnt);
				pPrgDcpt->progressCnt=0;					
			}
		}
	}

	return res;

}
#endif	// FREE_STREAMS




// stop decoder and flush out some data that's already in the internal buffers
AUDIO_RES SpxEncoderStop(audioCodecHandle h)
{
	const sStreamInfo*	pInfo=AudioStreamGetInfo(AudioGetOutStream());

#ifdef SPEEX_OGG_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_OGG_SPEEX)
	{	// normally we shouldn't have any data lingering in the input buffer
		// we'll just flush
		eSpxEncCmd	outFlush=SPX_CMD_CLOSE;

		return AudioStreamWriteData(AudioGetOutStream(), ((spxEncDcpt*)h)->cdc.outBuff, 0, &outFlush);
	}
#endif	// SPEEX_OGG_STREAMS

#ifdef FREE_STREAMS
	if(pInfo->sFmt==AUDIO_STREAM_FREE)
	{
		return AUDIO_RES_OK;
	}
#endif	// FREE_STREAMS

	return AUDIO_RES_BAD_CALL;	// shouldn't happen	
	
}


void SpxCleanUpEncoder(audioCodecHandle h)
{
	spxEncDcpt* pEDcpt;
	
	if((pEDcpt=(spxEncDcpt*)h))
	{
		speex_bits_destroy(&pEDcpt->cdc.spxBits);
		if(pEDcpt->cdc.spxState)
		{
			speex_encoder_destroy(pEDcpt->cdc.spxState);
		}

		free(pEDcpt->cdc.inBuff);
		free(pEDcpt->cdc.outBuff);
		free(pEDcpt);
	}
}


#endif	// AUDIO_SUPPORT_RECORD


// get the decoded bytes and clear internal counter
int SpxGetOutBytes(audioCodecHandle h, int clr)
{
	int nBytes=((spxCdcDcpt*)h)->nOutBytes;
	if(clr)
	{
		((spxCdcDcpt*)h)->nOutBytes=0;
	}
	return nBytes;
}

int AudioSpxSetBandMode(int wBand)
{
#ifdef SPEEX_WIDEBAND
	if(wBand)
	{
		SpxCtrl.spxFlags|=SPX_FLAG_WB;
	}
	else
	{
		SpxCtrl.spxFlags&=~SPX_FLAG_WB;
	}
	return 1;
#else
	SpxCtrl.spxFlags&=~SPX_FLAG_WB;
	return !wBand;
#endif
}

/////////////////////////////////////////////// local functions ////////////////////////
////////////////////////////////////////////////////////////////////////////////////////


static int SpxAllocateBuffers(spxCdcDcpt* pDcpt, eBuffMask buffMask)
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
			if(buffMask&CODEC_ALLOCATE_IN)
			{
				free(pDcpt->inBuff);
				pDcpt->inBuff=0;
			}
			
			if(buffMask&CODEC_ALLOCATE_OUT)
			{
				free(pDcpt->outBuff);
				pDcpt->outBuff=0;
			}
		}
	}

	return !allocFail;	
}


#endif	// SPEEX_ENCODING	


#endif	// defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)

