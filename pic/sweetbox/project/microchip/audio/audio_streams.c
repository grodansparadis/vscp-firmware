/*********************************************************************
 *
 *                  AUDIO Library streams parser
 *
 *********************************************************************
 * FileName:        audio_streams.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
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
 *$Id: audio_streams.c 161 2008-10-15 19:30:28Z akhe $
 *
 **********************************************************************/

#include <stdlib.h>
#include <string.h>

#include <audio/audio.h>
#include "audio_streams.h"

#include "GenericTypeDefs.h"


#if defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)

#include "audio_speex.h"

typedef struct
{
	//  structure describing a particular stream could be fFmt/afmt dependent
	//  TODO: if not possible to represent all streams with one structure, create different stream formats!
	sStreamInfo	info;		// info about this stream

	pReadCB		readCb;		// stream operations
	pWriteCB	writeCb;	// Note: this stream operations are provided 
	pSeekStreamCB	inSeekCb;	// by the user of the Audio Lib!
	pSeekStreamCB	outSeekCb;	// not all of them are used
	void*		pHDcpt;		// extra handle descriptor
}sStreamHandle;	// handle of an audio stream


#define	STREAM_VERIFY_HANDLE(pHandle)		do{}while(0)

#ifdef WAVE_STREAMS
//////////////////////////////// Wave files descriptors ///////////////////////////////

typedef enum
{
	CHNKID_RIFF	= 'FFIR',	// 'RIFF', BE form
	CHNKID_WAVE	= 'EVAW',	// 'WAVE' BE. ok, this is not really a chunk Id but a format Id
	CHNKID_FMT	= ' tmf',	// 'fmt ' BE.
	CHNKID_FACT	= 'tcaf',	// 'fact' BE.
	CHNKID_DATA	= 'atad'	// 'data' BE
}eChunkId;


typedef enum
{
	RIFF_CMP_UNKNOWN	= 0,
	RIFF_CMP_PCM		= 1,
	RIFF_CMP_MS_ADPCM	= 2,
	RIFF_CMP_G711_aLAW	= 6,
	RIFF_CMP_G711_AuLAW	= 7,
	RIFF_CMP_IMA_ADPCM	= 3017,		// TODO: check this value
	RIFF_CMP_G723_ADPCM	= 20,
	RIFF_CMP_GSM_610	= 49,
	RIFF_CMP_G721_ADPCM	= 64,
	RIFF_CMP_MPEG		= 80,
	RIFF_CMP_EXPERIMENT 	= 65536
}eRiffCompression;	// compression formats for data carried within a RIFF file


typedef struct  __attribute__((packed))
{
	INT32	chunkId;	// an eChunkId
	INT32	chunkSz;
}sRiffChunk;	// general RIFF chunk


typedef struct  __attribute__((packed))
{
	INT32	chunkId;	// should be eChunkId::CHNKID_RIFF
	INT32	chunkSz;
	INT32	formatId;	// should be eChunkId::CHNKID_WAVE
}sWaveChunk;	// RIFF chunk specifying WAVE format 



typedef struct  __attribute__((packed))
{
	INT32	chunkId;		// should be eChunkId::CHNKID_FMT
	INT32	chunkSz;
	INT16	compression;		// an eRiffCompression value
	INT16	numChannels;		// 1 for mono, 2 stereo, etc
	INT32	sampleRate;		// Hz
	INT32	avgBytesPerSec;		// ==sampleRate* blockAlign
	INT16	blockAlign;		// ==bitsPerSample/8 * numChannels
	INT16	bitsPerSample;		// 8, 16, 24, etc
	INT16	extraParamSize;		// 0 when RIFF_CMP_PCM
}sWaveFmtChunk;		// default (minimum size) fmt chunk in a WAVE file.


typedef struct  __attribute__((packed))
{
	INT32	chunkId;		// should be eChunkId::CHNKID_FMT
	INT32	chunkSz;
	INT16	compression;		// an eRiffCompression value
	INT16	numChannels;		// 1 for mono, 2 stereo, etc
	INT32	sampleRate;		// Hz
	INT32	avgBytesPerSec;		// ==sampleRate* blockAlign
	INT16	blockAlign;		// ==bitsPerSample/8 * numChannels
	INT16	bitsPerSample;		// 8, 16, 24, etc
	INT16	extraParamSize;		// not used when RIFF_CMP_PCM 
	INT16	extraParams;		// space for extra params
	INT32	extra[1];		// space for extra params
}sWaveFmtExtChunk;		// extended fmt chunk in a WAVE file (we have extraParam fields here because some streams carry those even for PCM format).


typedef struct  __attribute__((packed))
{
	INT32	chunkId;	// should be eChunkId::CHNKID_FACT
	INT32	chunkSz;	// normally 4
	INT32	dataSamples;	// how many samples contained in the data chunk.
}sFactChunk;	// RIFF chunk specifying WAVE format 


typedef struct  __attribute__((packed))
{
	INT32	chunkId;			// should be eChunkId::CHNKID_DATA
	INT32	chunkSz;			// == numSamples * numChannels * bitsPerSample/8
	// char	data[2];			// interleaved samples
}sDataChunk;	// data chunk in a WAVE file 


typedef struct  __attribute__((packed))
{
	sWaveChunk	wavChunk;	// wave chunk
	sWaveFmtChunk	fmtChunk;
	sFactChunk	factChunk;
	sDataChunk	dataChunk;
}sWaveHeader;		// complete header of a wave file
	
#endif	// WAVE_STREAMS
	

#ifdef SPEEX_OGG_STREAMS
//////////////////////////////// Speex/Ogg files descriptors ///////////////////////////////

typedef enum
{
	OGG_FLAG_CONTINUATION	 = 0x1,		// continuation packet
	OGG_FLAG_BOS		 = 0x2,		// first page of the logical stream
	OGG_FLAG_EOS		 = 0x4,		// last page of the logical stream
	
}eOggHeaderFlags;	// flags in the Ogg header


typedef enum
{
	OGG_ID_MAGIC	= 'SggO',	// OggS, the magic page capture

}eOggId;


#define	OGG_ID_SPEEX			"Speex   "		// the Speex packet id
#define	SPEEX_STRING_LENGTH		8			// size of the speex string	

#define SPEEX_VERSION			"speex-1.2beta3" 	// Speex version string. 
#define	SPEEX_VERSION_LENGTH		20			// size of the speex version string	

#define	SPEEX_VERSION_ID		1			// version identifier

#define SPEEX_VENDOR_STR		"Encoded by Microchip Audio Library ver 1.0"	// comment in ogg header


typedef struct  __attribute__((packed))
{
	INT32	pageCapture;		// should be OGG_ID_MAGIC
	BYTE	struct_ver;		// version of the Ogg file format. Should be 0 (RFC3533)
	BYTE	headerFlags;		// an eOggHeaderFlags value
	INT64	granulePos;		// stream dependent position info
	INT32	streamNo;		// logical bit stream identifier
	INT32	pageNo;			// page sequence number
	INT32	pageCrc;		// CRC32 checksum of the page
       	BYTE	pageSegments;		// number of page segments to follow
	BYTE	segmentTbl[255];	// actually segmentTbl[pageSegments]; contains the lace values for all segments in the page
}sOggPageSegHdr;	// header of an Ogg page, full segment info included


typedef struct
{
	sSpxRunDcpt	runDcpt;		// global info
	sOggPageSegHdr	pageHdr;		// current page header
	int		segIx;			// current packet segment index in the current page
	int		pktIx;			// current packet index, 0 -> ...
	int		prevBytes;		// previous value of the bytes in the encoded output buffer 
}sSpxStreamDcpt;		// info needed by the stream at run time
	


typedef struct  __attribute__((packed))
{
	char	speexString[SPEEX_STRING_LENGTH];	// identify the Speex bit-stream: OGG_ID_SPEEX
	char	speexVer[SPEEX_VERSION_LENGTH];		// Speex version that encoded the file 
	INT32	speexVerId;		       		// Speex version (for checking compatibility)
	INT32	headerSize;				// sizeof(SpeexHeader)
	INT32	sampleRate;				// Sampling rate used
	INT32	wBand;					// 0 for narrowband, 1 for wideband
	INT32	modeBitsStreamVer;			// Version ID of the bit-stream
	INT32	nChannels;				// Number of channels encoded
	INT32	bitRate;				// Bit-rate used
	INT32	frameSamples;				// Size of frames, samples
	INT32	vbr;					// 1 for a VBR encoding, 0 otherwise
	INT32	framesPerPacket;			// Number of frames stored per Ogg packet
	INT32	extraHeaders;				// Number of additional headers after the comments
	INT32	reserved1;				// Reserved for future use, 0
	INT32	reserved2;				// Reserved for future use, 0
}sSpeexHeader;		// the Speex header, the first page in the Speex stream


typedef struct __attribute__((packed))
{
	INT32	strLength;			// length of the following string
	char	strUTF8[4];			// strUTF8[strLength]; the comment string itslef, not 0 terminated
}sVorbisCommentStr;		// a Vorbis comment string. Also used in Speex

typedef struct __attribute__((packed))
{
	sVorbisCommentStr	vendorStr;	// vendor string
	INT32			nComments;	// user comment list length
	sVorbisCommentStr	comment[1];	// list of comments, [nComments]. Usually "keyword=value" form
	// ....... 				// multiple comments, nComments-1 in number
	BYTE			framingBit;	// if(!framingBit || EndOfPacket) then error.
}sVorbisCommentHeader;		// Vorbis comment header, used by Speex too.


// simplified version of our comment
struct __attribute__((packed))
{
	INT32	strLength;				// length of the following string
	char	strUTF8[sizeof(SPEEX_VENDOR_STR)];	// strUTF8[strLength]; the comment string itslef, not 0 terminated
	INT32			nComments;		// =0, no comments 
}MchpComment=					// our comment header
{
	sizeof(SPEEX_VENDOR_STR),		// strLength
	SPEEX_VENDOR_STR,			// strUTF8
	0					// nComments
};


#endif	// SPEEX_OGG_STREAMS


// we support Speex encoding in FREE_STREAMS 
// this stream has a header: sSpxFreeDcpt
// also, any data chunk is formatted as sSpxFreeDataPkt (pre-fixed with a short (16 bits) size, the chunk size, in bytes) 



// local proto

// free streams functions
#ifdef FREE_STREAMS
static AUDIO_RES	AudioFreeCloseStream(sStreamHandle* pSHndl);

#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES	AudioFreeOpenInStream(sStreamHandle* pSHndl);
static AUDIO_RES	AudioFreeReadData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes);
#endif

#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES	AudioFreeOpenOutStream(sStreamHandle* pSHndl);
static AUDIO_RES	AudioFreeWriteData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes);
#endif

#endif	// FREE_STREAMS

#ifdef WAVE_STREAMS

static AUDIO_RES	AudioWavCloseStream(sStreamHandle* pSHndl);

#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES	AudioWavOpenInStream(sStreamHandle* pSHndl);
static AUDIO_RES	AudioWavFindChunk(sStreamHandle* pSHndl, eChunkId chunkId, int streamBytes, sRiffChunk* pChunk, int* pSkipChunks, eChunkId skipId);
static AUDIO_RES	AudioWavRiffToAudioCompression(eRiffCompression riffComp, AUDIO_FORMAT* pAFmt);
static AUDIO_RES	AudioWavReadData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes);
#endif	// AUDIO_SUPPORT_PLAY

#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES	AudioWavOpenOutStream(sStreamHandle* pSHndl);
static AUDIO_RES	AudioWavAudioToRiffCompression(AUDIO_FORMAT aFmt, eRiffCompression* pRiffComp);
static AUDIO_RES	AudioWavWriteData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes);
#endif	// AUDIO_SUPPORT_RECORD



#endif	// WAVE_STREAMS


#ifdef SPEEX_OGG_STREAMS

static AUDIO_RES	AudioSpxCloseStream(sStreamHandle* pSHndl);

#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES	AudioSpxOpenInStream(sStreamHandle* pSHndl);
static AUDIO_RES	AudioSpxReadData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pResult);
static AUDIO_RES	AudioOggReadPageHdr(sStreamHandle* pSHndl, sOggPageSegHdr* pOggPage, sSpxStreamDcpt* pSpxDcpt);
static AUDIO_RES	AudioOggSkipPage(sStreamHandle* pSHndl, sOggPageSegHdr* pOggPage);
static int		AudioOggPageDataSize(sOggPageSegHdr* pOggPage);
static int		AudioOggPageGetPktSize(sSpxStreamDcpt* pDcpt, int* pPktComplete);
static int		AudioOggPagePackets(sOggPageSegHdr* pOggPage);

#endif	// AUDIO_SUPPORT_PLAY

#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES	AudioSpxOpenOutStream(sStreamHandle* pSHndl);
static AUDIO_RES	AudioSpxWriteData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pResult);
static AUDIO_RES	AudioOggWritePageHdr(sStreamHandle* pSHndl, sOggPageSegHdr* pOggPage);
static AUDIO_RES	AudioOggFlushPage(sStreamHandle* pStr, void* pBuff, int nBytes, int eos);

#endif	// AUDIO_SUPPORT_RECORD


#endif	// SPEEX_OGG_STREAMS


// data
//

typedef AUDIO_RES	(*pStreamOpenFnc)(sStreamHandle* pSHndl);	// function to open an I/O stream

#ifdef AUDIO_SUPPORT_PLAY
static const pStreamOpenFnc	streamInTbl[AUDIO_STREAMS_NO]=
{
#ifdef FREE_STREAMS
	AudioFreeOpenInStream,			// AUDIO_STREAM_FREE
#endif
#ifdef WAVE_STREAMS
	AudioWavOpenInStream,			// AUDIO_STREAM_WAV	
#endif
#ifdef SPEEX_OGG_STREAMS
	AudioSpxOpenInStream,			// AUDIO_STREAM_OGG_SPEEX
#endif
};	// table of supported input stream functions
#endif	// AUDIO_SUPPORT_PLAY

#ifdef AUDIO_SUPPORT_RECORD
static const pStreamOpenFnc	streamOutTbl[AUDIO_STREAMS_NO]=
{
#ifdef FREE_STREAMS
	AudioFreeOpenOutStream,			// AUDIO_STREAM_FREE
#endif
#ifdef WAVE_STREAMS
	AudioWavOpenOutStream,			// AUDIO_STREAM_WAV
#endif
#ifdef SPEEX_OGG_STREAMS
	AudioSpxOpenOutStream,			// AUDIO_STREAM_OGG_SPEEX
#endif
	
};	// table of supported output stream functions
#endif	// AUDIO_SUPPORT_RECORD


typedef AUDIO_RES	(*pStreamCloseFnc)(sStreamHandle* pSHndl);	// function to close an I/O stream

static const pStreamCloseFnc	streamCloseTbl[AUDIO_STREAMS_NO]=
{
#ifdef FREE_STREAMS
	AudioFreeCloseStream,			// AUDIO_STREAM_FREE
#endif
#ifdef WAVE_STREAMS
	AudioWavCloseStream,			// AUDIO_STREAM_WAV
#endif
#ifdef SPEEX_OGG_STREAMS
	AudioSpxCloseStream,			// AUDIO_STREAM_OGG_SPEEX
#endif

};	// table of supported close stream functions


typedef AUDIO_RES (*pStreamDataFnc)(sStreamHandle* pStr, void* pBuff, int nBytes, void* pResult);	// function to read/write data from a stream

#ifdef AUDIO_SUPPORT_PLAY
static const pStreamDataFnc	streamReadDataTbl[AUDIO_STREAMS_NO]=
{
#ifdef FREE_STREAMS
	AudioFreeReadData,			// AUDIO_STREAM_FREE
#endif
#ifdef WAVE_STREAMS
	AudioWavReadData,			// AUDIO_STREAM_WAV
#endif
#ifdef SPEEX_OGG_STREAMS
	AudioSpxReadData,			// AUDIO_STREAM_OGG_SPEEX
#endif
	
};	// table of supported read stream functions
#endif	// AUDIO_SUPPORT_PLAY

#ifdef AUDIO_SUPPORT_RECORD
static const pStreamDataFnc	streamWriteDataTbl[AUDIO_STREAMS_NO]=
{
#ifdef FREE_STREAMS
	AudioFreeWriteData,			// AUDIO_STREAM_FREE
#endif
#ifdef WAVE_STREAMS
	AudioWavWriteData,			// AUDIO_STREAM_WAV
#endif
#ifdef SPEEX_OGG_STREAMS
	AudioSpxWriteData,			// AUDIO_STREAM_OGG_SPEEX
#endif

};	// table of supported read stream functions
#endif	// AUDIO_SUPPORT_RECORD





// interface functions


// open an audio stream
// open a stream in a specified format for PLAY/RECORD
// for PLAY op, the info is updated from the stream itself
AUDIO_RES AudioStreamOpen(sStreamInfo* pInfo, audioSHandle* pAHandle)
{
	sStreamHandle*		pSHndl;
	AUDIO_RES 		res=AUDIO_RES_OK;

	*pAHandle=0;
	if(pInfo->sFmt<0 || pInfo->sFmt>AUDIO_STREAMS_NO)
	{
		return AUDIO_RES_BAD_CALL;
	}
	
	pSHndl=(sStreamHandle*)calloc(1, sizeof(sStreamHandle));
	if(!pSHndl)
	{
		return AUDIO_RES_OUT_OF_MEM;	// don't even bother
	}
	
	pSHndl->info=*pInfo;	// copy the user passed info
	// NOTE: use the callbacks as provided by the Audio Library (set by user)!
	pSHndl->readCb=(pReadCB)AudioGetCallback(AUDIO_CB_READ);
	pSHndl->writeCb=(pWriteCB)AudioGetCallback(AUDIO_CB_WRITE);
	pSHndl->inSeekCb=(pSeekStreamCB)AudioGetCallback(AUDIO_CB_SEEK_IN);
	pSHndl->outSeekCb=(pSeekStreamCB)AudioGetCallback(AUDIO_CB_SEEK_OUT);

	
#ifdef AUDIO_SUPPORT_RECORD
	if(pInfo->aOp==RECORD)
	{
		if(pSHndl->writeCb==0)
		{
			res=AUDIO_RES_STREAM_CB_ERROR;
		}
		else if(pInfo->sFmt==AUDIO_STREAM_QUERY)
		{
			res=AUDIO_RES_BAD_CALL;
		}
		else
		{
			pSHndl->info.streamSz=0;	// brand new stream
			res=(*streamOutTbl[(int)pInfo->sFmt])(pSHndl); 
		}
	}
#endif	// AUDIO_SUPPORT_RECORD


#ifdef AUDIO_SUPPORT_PLAY
	if(pInfo->aOp==PLAY)
	{
		AUDIO_STREAM_FORMAT	ix, start, stop;

		if(pSHndl->readCb==0)
		{
			res=AUDIO_RES_STREAM_CB_ERROR;
		}
		else
		{
			if(pInfo->sFmt==AUDIO_STREAM_QUERY)
			{
			#ifdef FREE_STREAMS
				start=AUDIO_STREAM_FREE+1;	// cannot query a free format
			#else
				start=(AUDIO_STREAM_FORMAT)0;
			#endif
				stop=AUDIO_STREAMS_NO;
			}
			else
			{
				start=pInfo->sFmt;
				stop=start+1;
			}

			res=AUDIO_RES_SSTREAM_ERROR;
			for(ix=start; ix<stop; ix++)
			{
				if(ix!=start)
				{
					if(!pSHndl->inSeekCb)
					{
						res=AUDIO_RES_STREAM_CB_ERROR;
						break;
					}
				
					// reposition at the beginning
					if(!(*pSHndl->inSeekCb)(0, ORIGIN_BOF))
					{
						res=AUDIO_RES_READ_ERROR;
						break;
					}
				}

				res=(*streamInTbl[ix])(pSHndl);
				if(res==AUDIO_RES_OK)
				{
					*pInfo=pSHndl->info;	// update the stream info
					break;
				}
			}
		}
	}

#endif	// AUDIO_SUPPORT_PLAY
	
	
	if(res==AUDIO_RES_OK)
	{
		*pAHandle=pSHndl;
	}
	else
	{
		free(pSHndl);
	}
	
	return res;

	
}


// closes a currently opened stream
AUDIO_RES AudioStreamClose(audioSHandle handle)
{
	AUDIO_RES res;
	
	if(handle)
	{
		sStreamHandle*	pStr;
		STREAM_VERIFY_HANDLE(handle);
		pStr=(sStreamHandle*)handle;
		
		res=(*streamCloseTbl[(int)pStr->info.sFmt])(pStr);
		
		AudioStreamCleanup(handle);
		
	}
	else
	{
		res=AUDIO_RES_OK;
	}

	return res;
}


// discards and clean up an opened stream
AUDIO_RES AudioStreamCleanup(audioSHandle pHandle)
{
	if(pHandle)
	{
		sStreamHandle*		pStr;
		
		STREAM_VERIFY_HANDLE(pHandle);
		pStr=(sStreamHandle*)pHandle;

		free(pStr->pHDcpt);
		pStr->pHDcpt=0;
		pStr->info.sFmt=0xef;
		free(pHandle);
	}
	
	return AUDIO_RES_OK;
}


// returns stream info
const sStreamInfo* AudioStreamGetInfo(audioSHandle pHandle)
{
	STREAM_VERIFY_HANDLE(pHandle);
	
	return &((sStreamHandle*)pHandle)->info;
}


	
// read data samples
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES AudioStreamReadData(audioSHandle handle, void* pBuff, int nBytes, void* pRes)
{
	if(handle)
	{
		sStreamHandle*	pStr;
		STREAM_VERIFY_HANDLE(handle);
		pStr=(sStreamHandle*)handle;
		return (*streamReadDataTbl[(int)pStr->info.sFmt])(pStr, pBuff, nBytes, pRes);
	}
	
	return AUDIO_RES_BAD_CALL;

}
#endif	// AUDIO_SUPPORT_PLAY



// write data samples
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioStreamWriteData(audioSHandle handle, void* pBuff, int nBytes, void* pRes)
{
	if(handle)
	{
		sStreamHandle*	pStr;
		STREAM_VERIFY_HANDLE(handle);
		pStr=(sStreamHandle*)handle;
		return (*streamWriteDataTbl[(int)pStr->info.sFmt])(pStr, pBuff, nBytes, pRes);
	}
	
	return AUDIO_RES_BAD_CALL;
}
#endif	// AUDIO_SUPPORT_RECORD


// sets/gets run time info. stream specific.
#ifdef	AUDIO_SUPPORT_PLAY
AUDIO_RES AudioStreamGetRunInfo(audioSHandle handle, void* pBuff, int nBytes)
{
#ifdef SPEEX_ENCODING
	if(handle)
	{
		sStreamHandle*	pStr;
		STREAM_VERIFY_HANDLE(handle);
		pStr=(sStreamHandle*)handle;

		if(nBytes==sizeof(sSpxRunDcpt))
		{
		#ifdef SPEEX_OGG_STREAMS		
			if(pStr->info.sFmt==AUDIO_STREAM_OGG_SPEEX)
			{
				sSpxStreamDcpt* pStrDcpt=(sSpxStreamDcpt*)pStr->pHDcpt;
				sSpxRunDcpt* pExtDcpt=(sSpxRunDcpt*)pBuff;
				sSpxRunDcpt* pSrcDcpt=(sSpxRunDcpt*)&pStrDcpt->runDcpt;
				*pExtDcpt=*pSrcDcpt;	// copy info
				
				return AUDIO_RES_OK;
			}
		#endif	// SPEEX_OGG_STREAMS


		#ifdef FREE_STREAMS		
			if(pStr->info.sFmt==AUDIO_STREAM_FREE)
			{
				sSpxRunDcpt* pExtDcpt=(sSpxRunDcpt*)pBuff;
				sSpxRunDcpt* pSrcDcpt=(sSpxRunDcpt*)pStr->pHDcpt;
				*pExtDcpt=*pSrcDcpt;	// copy info
				
				return AUDIO_RES_OK;
			}
		#endif	// FREE_STREAMS

		}
	}
	
#endif	// SPEEX_ENCODING

	return AUDIO_RES_BAD_CALL;

}
#endif	// AUDIO_SUPPORT_PLAY


// implementation
//


#ifdef FREE_STREAMS

static AUDIO_RES AudioFreeCloseStream(sStreamHandle* pSHndl)
{
	// no headers, footers involved
	return AUDIO_RES_OK;
}

#ifdef AUDIO_SUPPORT_PLAY

// open a free format input audio stream
// we should be already pointing to the data part of the stream
// however, a free Speex format has to extract some data for the decoder to figure it out
// the outstream header previously written should be sSpxFreeDcpt
// then, each data chunk it's preceded by its size: sSpxFreeDataPkt 
static AUDIO_RES AudioFreeOpenInStream(sStreamHandle* pSHndl)
{
#ifdef SPEEX_ENCODING
	if(pSHndl->info.aFmt==AUDIO_SPEEX)
	{	// write the Speex free header: sStreamInfo+sSpxRunDcpt
		int		rdBytes;
		sSpxFreeDcpt	freeDcpt;
		sSpxRunDcpt*	pRunDcpt;
		AUDIO_RES	res;

		pRunDcpt=(sSpxRunDcpt*)calloc(1, sizeof(*pRunDcpt));
		if(!pRunDcpt)
		{
			return AUDIO_RES_OUT_OF_MEM;
		}
		
		rdBytes=(*pSHndl->readCb)((unsigned char*)&freeDcpt, sizeof(freeDcpt));	// read the header

		if(rdBytes!=sizeof(freeDcpt))
		{
			res=AUDIO_RES_READ_ERROR;
		}
		else if(freeDcpt.spxFreeVer!=SPEEX_FREE_PKT_VER || freeDcpt.size!=sizeof(freeDcpt))
		{
			res=AUDIO_RES_SSTREAM_ERROR;
		}
		else
		{
			pSHndl->info.sampleRate=freeDcpt.sampleRate;
			pSHndl->info.bitRate=freeDcpt.bitRate;
			
			pRunDcpt->frameSamples=freeDcpt.frameSamples;
			pRunDcpt->bitRate=freeDcpt.bitRate;
			pRunDcpt->framesPerPacket=freeDcpt.framesPerPacket;
			pRunDcpt->packetsPerPage=1;
			pRunDcpt->complexity=freeDcpt.complexity;
			pRunDcpt->qualFactor=freeDcpt.qualFactor;
			pRunDcpt->spxFlags=freeDcpt.spxFlags;
			
			pSHndl->pHDcpt=pRunDcpt;
			
			return AUDIO_RES_OK;
		}
		
		free(pRunDcpt);
		return res;
	}
#endif


	return AUDIO_RES_OK;
}

static AUDIO_RES AudioFreeReadData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes)
{
	// unknown stream size
	
	*((int*)pActBytes)=(*pStr->readCb)(pBuff, nBytes);
	return AUDIO_RES_OK;
}
#endif

#ifdef AUDIO_SUPPORT_RECORD

// open a free format output audio stream
// normally, we just write data directly, no headers or anything
// however, since we want to support a "free" format for Speex,
// some basic data needs to be stored to the output stream so that
// the decoder is able to figure it out
// we store a sSpxFreeDcpt structure at the beginning of the data
// then, each data chunk it's preceded by its size: sSpxFreeDataPkt 
static AUDIO_RES AudioFreeOpenOutStream(sStreamHandle* pSHndl)
{
#ifdef SPEEX_ENCODING
	if(pSHndl->info.aFmt==AUDIO_SPEEX)
	{	// write the Speex free header: sSpxFreeDcpt
		int		wrBytes;
		sSpxRunDcpt	runDcpt;
		sSpxFreeDcpt	freeDcpt;


		AudioSpxGetSettings(&runDcpt);

		freeDcpt.spxFreeVer=SPEEX_FREE_PKT_VER;
		freeDcpt.size=sizeof(freeDcpt);
		freeDcpt.sampleRate=(unsigned short)pSHndl->info.sampleRate;
		freeDcpt.frameSamples=runDcpt.frameSamples;
		freeDcpt.bitRate=runDcpt.bitRate;
		freeDcpt.framesPerPacket=runDcpt.framesPerPacket;
		freeDcpt.complexity=runDcpt.complexity;
		freeDcpt.qualFactor=runDcpt.qualFactor;
		freeDcpt.spxFlags=runDcpt.spxFlags;
						
		wrBytes=(*pSHndl->writeCb)((unsigned char*)&freeDcpt, sizeof(freeDcpt));		// write the sSpxFreeDcpt header

		return wrBytes==sizeof(freeDcpt)?AUDIO_RES_OK:AUDIO_RES_WRITE_ERROR;
	}
#endif
	return AUDIO_RES_OK;
}


static AUDIO_RES AudioFreeWriteData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes)
{
	int wrBytes=(*pStr->writeCb)(pBuff, nBytes);
	*((int*)pActBytes)=wrBytes;
	pStr->info.streamSz+=wrBytes;
	return wrBytes==nBytes?AUDIO_RES_OK:AUDIO_RES_WRITE_ERROR;
}
#endif

#endif	// FREE_STREAMS



#ifdef WAVE_STREAMS

// open the stream and set the stream pointer at the begining of data samples
#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES AudioWavOpenInStream(sStreamHandle* pSHndl)
{
	sWaveChunk		wChunk;
	sWaveFmtExtChunk	waveFmtExtChnk;	// the format chunk describes the data format embedded in the RIFF file
	sDataChunk		dataChunk;
	int			nBytes;
	int			skipChunks;	// true if chunks skipped
	AUDIO_FORMAT		aFmt;
	AUDIO_RES		res;

	
	while(1)
	{	
		nBytes=(*pSHndl->readCb)((BYTE*)&wChunk, sizeof(wChunk));

		if(nBytes!=sizeof(wChunk) || wChunk.chunkId!=CHNKID_RIFF || wChunk.formatId!=CHNKID_WAVE)
		{
			res=AUDIO_RES_SSTREAM_ERROR;
			break;
		}
		else if(wChunk.chunkSz < sizeof(sWaveFmtChunk)+sizeof(sDataChunk))
		{
			res=AUDIO_RES_DSTREAM_ERROR;		// missing the format and/or data chunks?
			break;
		}

		// find the "fmt " chunk
		// "fmt " before "data" is not really required by this file format
		res=AudioWavFindChunk(pSHndl, CHNKID_FMT, wChunk.chunkSz, (sRiffChunk*)&waveFmtExtChnk, &skipChunks, CHNKID_DATA);

		if(res!=AUDIO_RES_OK)
		{
			break;	// failure in the input stream
		}
	
		// ok, found the "fmt " chunk
		// see how big is it
		if(waveFmtExtChnk.chunkSz>sizeof(waveFmtExtChnk)-sizeof(sRiffChunk))
		{	// cannot read in place
			BYTE* pFmtData=(BYTE*)malloc(waveFmtExtChnk.chunkSz);
			if(!pFmtData)
			{
				return AUDIO_RES_OUT_OF_MEM;	
			}
			nBytes=(*pSHndl->readCb)(pFmtData, waveFmtExtChnk.chunkSz);	// read the remaining "fmt " chunk
		       	if(nBytes!=waveFmtExtChnk.chunkSz)
			{
				free(pFmtData);
				res=AUDIO_RES_READ_ERROR;
				break;
			}
			memcpy((BYTE*)&waveFmtExtChnk.compression, pFmtData, sizeof(waveFmtExtChnk));
			free(pFmtData);
		}
		else
		{ // read the remaining "fmt " chunk in place
			nBytes=(*pSHndl->readCb)((BYTE*)&waveFmtExtChnk.compression, waveFmtExtChnk.chunkSz);
			if(nBytes!=waveFmtExtChnk.chunkSz)
			{
				res=AUDIO_RES_READ_ERROR;
				break;
			}
		}
	
		res=AudioWavRiffToAudioCompression(waveFmtExtChnk.compression, &aFmt);
		if(res!=AUDIO_RES_OK)
		{
			break;
		}
		pSHndl->info.aFmt=(char)aFmt;
		// now it's up to the caller to see that other parameters (bits/sample, sample rate, etc) match the app needs
		// using the interface functions

		// ok, time to point to the data chunk
		if(skipChunks)
		{
			if(!(*pSHndl->inSeekCb)(sizeof(sWaveChunk), ORIGIN_BOF))
			{
				res=AUDIO_RES_READ_ERROR;
				break;
			}
			nBytes=wChunk.chunkSz;		// search from the very beginning
		}
		else
		{
			nBytes=wChunk.chunkSz-waveFmtExtChnk.chunkSz-sizeof(sRiffChunk);	// skip the format chunk	
		}
	
		res=AudioWavFindChunk(pSHndl, CHNKID_DATA, nBytes, (sRiffChunk*)&dataChunk, 0, 0);

		
		if(res==AUDIO_RES_OK)
		{
			pSHndl->info.streamSz=dataChunk.chunkSz;
			pSHndl->info.numChannels=waveFmtExtChnk.numChannels;
			pSHndl->info.sampleRate=waveFmtExtChnk.sampleRate;
			pSHndl->info.bitsPerSample=waveFmtExtChnk.bitsPerSample;
			pSHndl->info.bitRate=waveFmtExtChnk.bitsPerSample*waveFmtExtChnk.sampleRate;	// compression=1:1;
			pSHndl->info.lEndian=1;		// always in wave files

			if(pSHndl->info.activeChannel>pSHndl->info.numChannels)
			{
				pSHndl->info.activeChannel=1;		// by default
			}

			pSHndl->info.sFmt=AUDIO_STREAM_WAV;		// update the stream info if coming from a query

			if(dataChunk.chunkSz==0)
			{
				res=AUDIO_RES_STREAM_EMPTY;
			}
			
		}
		break;
	}

	return res;
		
}

// assumes proper positioning at the beginning of a chunk
// if desired chunk found, the stream pointer is just after the chunk header (sRiffChunk struct)
static AUDIO_RES AudioWavFindChunk(sStreamHandle* pSHndl, eChunkId chunkId, int streamBytes, sRiffChunk* pChunk, int* pSkipChunks, eChunkId skipId)
{
	int		nBytes;
	AUDIO_RES	res;

	if(pSkipChunks)
	{
		*pSkipChunks=0;
	}
	
	res=AUDIO_RES_SSTREAM_ERROR;
	
	while(streamBytes)
	{
		nBytes=(*pSHndl->readCb)((BYTE*)pChunk, sizeof(*pChunk));
		if(nBytes!= sizeof(*pChunk))
		{
			res=AUDIO_RES_READ_ERROR;
			break;
		}
	
		if(pChunk->chunkId==chunkId)
		{
			res=AUDIO_RES_OK; // found it
			break;
		}
		else if(pSkipChunks && pChunk->chunkId==skipId)
		{
			*pSkipChunks=1;
		}

		// have go to the following chunk
		if(!pSHndl->inSeekCb)
		{
			res=AUDIO_RES_STREAM_CB_ERROR;
			break;
		}
	
		nBytes=(pChunk->chunkSz+1)&0xfffffffe;	// should be always even
							// TODO: some files do not properly pad so that the chunk starts always on an even boundary
							// add provisions for this!
		if(!(*pSHndl->inSeekCb)(nBytes, ORIGIN_CURR))
		{
			res=AUDIO_RES_READ_ERROR;
			break;
		}

		streamBytes-=nBytes+sizeof(*pChunk);
	}


	return res;

}	

// convert a RIFF compression to our library supported format
static AUDIO_RES AudioWavRiffToAudioCompression(eRiffCompression riffComp, AUDIO_FORMAT* pAFmt)
{
	AUDIO_FORMAT	aFmt;
	AUDIO_RES	res=AUDIO_RES_OK;

	switch(riffComp)
	{
	#ifdef PCM_ENCODING
		case RIFF_CMP_PCM:
			aFmt=AUDIO_PCM;
			break;
	#endif
	#ifdef ADPCM_ENCODING
		case RIFF_CMP_IMA_ADPCM:
			aFmt=AUDIO_ADPCM;
			break;			
	#endif
			
		default:
			res=AUDIO_RES_COMPRESS_ERROR;
			aFmt=AUDIO_PCM;
			break;
	}

	if(pAFmt && res==AUDIO_RES_OK)
	{
		*pAFmt=aFmt;
	}
	
	return res;
}

static AUDIO_RES AudioWavReadData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes)
{
	if(nBytes>pStr->info.streamSz)
	{
		nBytes=pStr->info.streamSz;
	}
		
	*((int*)pActBytes)=(*pStr->readCb)(pBuff, nBytes);
	pStr->info.streamSz-=nBytes;
	return AUDIO_RES_OK;
}

#endif	// AUDIO_SUPPORT_PLAY


#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES AudioWavOpenOutStream(sStreamHandle* pSHndl)
{

	sWaveHeader	wavHeader;
	int		nBytes;
	AUDIO_RES	res;
	
	if(!pSHndl->outSeekCb)
	{
		res=AUDIO_RES_STREAM_CB_ERROR;	// we need to be able to update stream info
	}
	else
	{
		res=AudioWavAudioToRiffCompression(pSHndl->info.aFmt, 0);
	}

	if(res==AUDIO_RES_OK)
	{
		memset(&wavHeader, 0, sizeof(wavHeader));	// we'll have to update it anyway once we're done writing data
		
		nBytes=(*pSHndl->writeCb)((BYTE*)&wavHeader, sizeof(wavHeader));

		if(nBytes!=sizeof(wavHeader))
		{
			res=AUDIO_RES_WRITE_ERROR;
		}
	}

	return res;
}

// convert an audio lib compression to RIFF format
static AUDIO_RES AudioWavAudioToRiffCompression(AUDIO_FORMAT aFmt, eRiffCompression* pRiffComp)
{
	eRiffCompression	rComp;
	AUDIO_RES		res=AUDIO_RES_OK;

	switch(aFmt)
	{
	#ifdef PCM_ENCODING
		case AUDIO_PCM:
			rComp=RIFF_CMP_PCM;
			break;
	#endif
	#ifdef ADPCM_ENCODING
		case AUDIO_ADPCM:
			rComp=RIFF_CMP_IMA_ADPCM;
			break;			
	#endif
			
		default:
			res=AUDIO_RES_COMPRESS_ERROR;
			rComp=RIFF_CMP_PCM;
			break;
	}

	if(pRiffComp && res==AUDIO_RES_OK)
	{
		*pRiffComp=rComp;
	}

	return res;
}


static AUDIO_RES AudioWavWriteData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pActBytes)
{
	int wrBytes=(*pStr->writeCb)(pBuff, nBytes);
	*((int*)pActBytes)=wrBytes;
	pStr->info.streamSz+=wrBytes;
	return wrBytes==nBytes?AUDIO_RES_OK:AUDIO_RES_WRITE_ERROR;
}

#endif	// AUDIO_SUPPORT_RECORD


static AUDIO_RES AudioWavCloseStream(sStreamHandle* pSHndl)
{
	AUDIO_RES	res=AUDIO_RES_OK;
	
#ifdef AUDIO_SUPPORT_RECORD
	if(pSHndl->info.aOp==RECORD)
	{
		int			nBytes;
		eRiffCompression	rComp;
		sWaveHeader		wavHeader;
		
		// point to the beg of file, sWaveHeader
		if(!(*pSHndl->outSeekCb)(0, ORIGIN_BOF))
		{
			res=AUDIO_RES_WRITE_ERROR;
		}
		else
		{	// properly construct the header info
		
			//  update the 'RIFF' chunk
			wavHeader.wavChunk.chunkId=CHNKID_RIFF;
			wavHeader.wavChunk.chunkSz=sizeof(wavHeader.wavChunk.formatId)+sizeof(wavHeader.fmtChunk)+sizeof(wavHeader.factChunk)
						+sizeof(wavHeader.dataChunk)+pSHndl->info.streamSz;	// update the 'RIFF' chunk size
			wavHeader.wavChunk.formatId=CHNKID_WAVE;

			// update the 'fmt ' chunk
			wavHeader.fmtChunk.chunkId=CHNKID_FMT;
			wavHeader.fmtChunk.chunkSz=sizeof(wavHeader.fmtChunk)-sizeof(wavHeader.fmtChunk.chunkId)-sizeof(wavHeader.fmtChunk.chunkSz);
			AudioWavAudioToRiffCompression(pSHndl->info.aFmt, &rComp);
			wavHeader.fmtChunk.compression=rComp;
			wavHeader.fmtChunk.numChannels=pSHndl->info.numChannels;
			wavHeader.fmtChunk.sampleRate=pSHndl->info.sampleRate;
			wavHeader.fmtChunk.bitsPerSample=pSHndl->info.bitsPerSample;
			wavHeader.fmtChunk.blockAlign=(wavHeader.fmtChunk.bitsPerSample*wavHeader.fmtChunk.numChannels)/8;
			wavHeader.fmtChunk.avgBytesPerSec=wavHeader.fmtChunk.sampleRate*wavHeader.fmtChunk.blockAlign;
			wavHeader.fmtChunk.extraParamSize=0;		

			// update the 'fact' chunk
			wavHeader.factChunk.chunkId=CHNKID_FACT;
			wavHeader.factChunk.chunkSz=sizeof(wavHeader.factChunk.dataSamples);
			if(wavHeader.fmtChunk.bitsPerSample>8)
			{
				wavHeader.factChunk.dataSamples=pSHndl->info.streamSz/(wavHeader.fmtChunk.bitsPerSample/8);
			}
			else
			{
				wavHeader.factChunk.dataSamples=pSHndl->info.streamSz*(8/wavHeader.fmtChunk.bitsPerSample);
			}
		
			// update the 'data' chunk
			wavHeader.dataChunk.chunkId=CHNKID_DATA;
			wavHeader.dataChunk.chunkSz=pSHndl->info.streamSz;
		

			nBytes=(*pSHndl->writeCb)((BYTE*)&wavHeader, sizeof(wavHeader));

			if(nBytes!=sizeof(wavHeader))
			{
				res=AUDIO_RES_WRITE_ERROR;
			}
		}
	}
#endif	// AUDIO_SUPPORT_RECORD
	
	return res; 	
}

#endif	// WAVE_STREAMS


#ifdef SPEEX_OGG_STREAMS

#ifdef AUDIO_SUPPORT_PLAY
// open the stream and set the stream pointer at the begining of data samples
static AUDIO_RES AudioSpxOpenInStream(sStreamHandle* pSHndl)
{
	sSpeexHeader	spxHdr;
	int		nBytes, nPkts, pgSize;
	int		compressFact, avgPktSize;
	BOOL		commentFound;
	sSpxStreamDcpt*	pSpxDcpt;
	sOggPageSegHdr*	pOggHdr;
	eSpxFlags	spxFlags;
	AUDIO_RES	res=AUDIO_RES_OK;


	// construct the run time Speex descriptor
	pSpxDcpt=(sSpxStreamDcpt*)malloc(sizeof(sSpxStreamDcpt));
	if(!pSpxDcpt)
	{
		res=AUDIO_RES_OUT_OF_MEM;
	}
	else
	{
		pOggHdr=&pSpxDcpt->pageHdr;
	}

	
	while(res==AUDIO_RES_OK)
	{
		if((res=AudioOggReadPageHdr(pSHndl, pOggHdr, 0))!=AUDIO_RES_OK)
		{
			break;
		}
		
		// seems to be an Ogg stream
		// try to look for our Speex info in there
		if(pOggHdr->headerFlags!=OGG_FLAG_BOS || pOggHdr->granulePos!=0 || pOggHdr->pageNo!=0 ||
			       pOggHdr->pageSegments!=1 || pOggHdr->segmentTbl[0]!=sizeof(sSpeexHeader))
		{	// looking into a wrong Ogg stream
			// try to skip this page in case we have a multiple Ogg container...
			res=AudioOggSkipPage(pSHndl, pOggHdr);
			continue;	// skipped current page...
		}
		
		// read the speex header
		nBytes=(*pSHndl->readCb)((BYTE*)&spxHdr, sizeof(spxHdr));
		if(nBytes!=sizeof(spxHdr))
		{
			res=AUDIO_RES_SSTREAM_ERROR;	// could not read the Stream
			break;
		}
			
		if(strncmp(spxHdr.speexString, OGG_ID_SPEEX, SPEEX_STRING_LENGTH) || spxHdr.headerSize!=sizeof(spxHdr))
		{
			continue;	// skip current page in case we have a multiple Ogg container...
		}

		// finally, we got a valid Speex header.
		// try to get to the  data page
		// for now, we ignore the comment page

		commentFound=0;
		while(res==AUDIO_RES_OK)
		{
			if((res=AudioOggReadPageHdr(pSHndl, pOggHdr, pSpxDcpt))!=AUDIO_RES_OK)
			{
				break;		// some error
			}
			
			// valid new page from the same stream
			if(!commentFound)
			{
				// this should be the Speex cooment block
				if(pOggHdr->granulePos==0 && pOggHdr->pageNo==1 && pOggHdr->pageSegments==1)
				{	// seems to be the valid comment
					commentFound=1;
				}
				
				res=AudioOggSkipPage(pSHndl, pOggHdr);
				continue;	// just skip it anyway
			}
			else if(pOggHdr->pageNo==2 && pOggHdr->granulePos)
			{ // ok, we got the 1st data page
				break;
			}
			
			res==AudioOggSkipPage(pSHndl, pOggHdr);	// try to find another start of data page?
									// will fail, most likely
		}
		
		if(res!=AUDIO_RES_OK)
		{
			break;	// some error occurred
		}
		
		// store the info we need from the header
		pSpxDcpt->runDcpt.streamNo=pOggHdr->streamNo;
		pSpxDcpt->runDcpt.framesPerPacket=spxHdr.framesPerPacket;
		pSpxDcpt->runDcpt.frameSamples=spxHdr.frameSamples;
		spxFlags=0;
		if(spxHdr.wBand)
		{
			spxFlags|=SPX_FLAG_WB;
		}
		if(spxHdr.vbr)
		{
			spxFlags|=SPX_FLAG_VBR;
		}
		pSpxDcpt->runDcpt.spxFlags=spxFlags;
		
		pgSize=AudioOggPageDataSize(pOggHdr);
		nPkts=AudioOggPagePackets(pOggHdr);
		avgPktSize=nPkts?pgSize/nPkts:pgSize;		// TODO: this works just for CBR
								// have to be changed for VBR!

		pSpxDcpt->segIx=pSpxDcpt->pktIx=0;
		
		// we have the proper current data page info loaded
		pSHndl->pHDcpt=pSpxDcpt;	// store the needed speex state machine info
		
		// update the stream info
		pSHndl->info.sFmt=AUDIO_STREAM_OGG_SPEEX;
		pSHndl->info.aFmt=AUDIO_SPEEX;
		pSHndl->info.streamSz=0;	// not really known beforehand. 
		pSHndl->info.numChannels=spxHdr.nChannels;
		if(pSHndl->info.activeChannel>pSHndl->info.numChannels)
		{
			pSHndl->info.activeChannel=1;		// by default
		}
		pSHndl->info.sampleRate=spxHdr.sampleRate;

		// since the spxHdr.bitRate is not updated we try to estimate something:
		// NOTE: the calculation can be done using the SPEEX_GET_BITRATE.
		// However, at this moment we didn't feed any data into the Speex decoder
		// compression factor is multiplied by 100 for better approx
		compressFact=(spxHdr.framesPerPacket*spxHdr.frameSamples*200)/(avgPktSize);	// (2bytes*100)/1byte
		pSHndl->info.bitRate=(spxHdr.sampleRate*16*100)/compressFact;	// in bps
		pSHndl->info.bitsPerSample=(16*100+compressFact-1)/compressFact;		// round up
		pSHndl->info.lEndian=1;				// always in Ogg files
		


		break;
	}

	if(res!=AUDIO_RES_OK)
	{
		free(pSpxDcpt);
	}

	return res;
}

// read next data packet
// the read will extend across page boundary if the packet doesn't end on the current page
// in case of page lost, the correct packet number should be returned
static AUDIO_RES AudioSpxReadData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pResult)
{
	int		rdBytes;
	int		pktBytes;
	int		pktComplete;
	int		prevPgNo;
	int		loopCount;
	
	AUDIO_RES	res=AUDIO_RES_OK;
	sSpxStreamDcpt*	pSpxDcpt=(sSpxStreamDcpt*)pStr->pHDcpt;	// get the speex state machine info
	sSpxPktDcpt*	pPktDcpt=(sSpxPktDcpt*)pResult;


	pPktDcpt->pktBytes=0;
	loopCount=0;
	
	while(1)
	{
		pktBytes=AudioOggPageGetPktSize(pSpxDcpt, &pktComplete);
		if(pktBytes==0 && pktComplete)
		{ // if loopCount==0, we have a 0 len pkt mid page
			res=loopCount?AUDIO_RES_OK:AUDIO_RES_DSTREAM_ERROR;
			break;
		}
		
		// either pktBytes or pktComplete==0
		if(pktBytes)
		{
			if(nBytes<pktBytes)
			{
				res=AUDIO_RES_BUFF_ERROR;	// should accomodate at least a packet
				break;
			}
			rdBytes=(*pStr->readCb)(pBuff, pktBytes);
			if(rdBytes!=pktBytes)
			{
				res=AUDIO_RES_READ_ERROR;
				break;
			}
		}
		
		if(pktComplete)
		{
			pPktDcpt->pktBytes+=pktBytes;
			pPktDcpt->pktSeqNo=++pSpxDcpt->pktIx;	// was in sync
			break;
		}
		
		// read in a new page
		pBuff=(char*)pBuff+pktBytes;
		nBytes-=pktBytes;
			
		prevPgNo=pSpxDcpt->pageHdr.pageNo;
		res=AudioOggReadPageHdr(pStr, &pSpxDcpt->pageHdr, pSpxDcpt);
		if(res!=AUDIO_RES_OK)
		{
			if(res==AUDIO_RES_STREAM_END)
			{
				res=AUDIO_RES_DONE;	// eos
			}
			break;		// could not end in the middle of the packet!
		}
		pSpxDcpt->segIx=0;	// brand new page
		if(pSpxDcpt->pageHdr.pageNo!=prevPgNo+1)
		{ // we're out of sync
			// TODO: update pSpxDcpt->pktIx with lost packets: based on frameSamples, framesPerPkt and granulePos
			// for now, no op. We maintain sequential packet numbers!
		}
		loopCount++;
	}
		
	
	return res;
}


static AUDIO_RES AudioOggReadPageHdr(sStreamHandle* pSHndl, sOggPageSegHdr* pOggPage, sSpxStreamDcpt* pSpxDcpt)
{
	int		nBytes;
	int		pageBytes;
	BOOL		useStreamId;
	int		streamId=0;
	AUDIO_RES	res=AUDIO_RES_OK;

	if((useStreamId=(pSpxDcpt!=0)))
	{
		streamId=pSpxDcpt->pageHdr.streamNo;
		if((pSpxDcpt->pageHdr.headerFlags&OGG_FLAG_EOS)!=0)
		{
			res=AUDIO_RES_STREAM_END;	// end of stream
		}
	}

	while(res==AUDIO_RES_OK)
	{
		pageBytes=sizeof(*pOggPage)-sizeof(pOggPage->segmentTbl);	// page with no segment table
		nBytes=(*pSHndl->readCb)((BYTE*)pOggPage, pageBytes);
		if(nBytes!=pageBytes)
		{
			res=AUDIO_RES_READ_ERROR;
			break;
		}
		else if(pOggPage->pageCapture!=OGG_ID_MAGIC)
		{
			res=AUDIO_RES_SSTREAM_ERROR;	// not an Ogg Stream
			break;
		}
		else if((pageBytes=pOggPage->pageSegments))
		{
			nBytes=(*pSHndl->readCb)(pOggPage->segmentTbl, pageBytes);
			if(nBytes==pageBytes)
			{
				if(!useStreamId || pOggPage->streamNo==streamId)
				{	// found it
					break;
				}
				else if(!(pOggPage->headerFlags&OGG_FLAG_EOS))
				{
					res=AudioOggSkipPage(pSHndl, pOggPage);
					continue;
				}
				else
				{
					res=AUDIO_RES_STREAM_END;	// wrong stream, end of stream
					break;
				}
				
			}
			else
			{
				res=AUDIO_RES_READ_ERROR;
				break;
			}
		}
		else if(pOggPage->headerFlags&OGG_FLAG_EOS)
		{
			res=AUDIO_RES_STREAM_END;
		}
		else
		{	
			res=AUDIO_RES_SSTREAM_ERROR;	// empty page ?
			break;
		}
	}

	return res;
}


// skips over the data part of an Ogg page
static AUDIO_RES AudioOggSkipPage(sStreamHandle* pSHndl, sOggPageSegHdr* pOggPage)
{
	AUDIO_RES res;
	if(!pSHndl->inSeekCb)
	{
		res=AUDIO_RES_STREAM_CB_ERROR;
	}
	else
	{
		int	dataBytes=AudioOggPageDataSize(pOggPage);

		if(!(*pSHndl->inSeekCb)(dataBytes, ORIGIN_CURR))
		{
			res=AUDIO_RES_READ_ERROR;
		}
		else
		{
			res=AUDIO_RES_OK;
		}
	}


	return res;	// skipped current page or failed...		
}


// returns the number of data bytes in an Ogg page
static int AudioOggPageDataSize(sOggPageSegHdr* pOggPage)
{
	int	ix;
	int	dataBytes;
	
			
	for(ix=0, dataBytes=0; ix<pOggPage->pageSegments; ix++)
	{
		dataBytes+=pOggPage->segmentTbl[ix];
	}

	return dataBytes;
}


// returns the number of bytes in the current packet, completed or not
// also returns true if the packet pointed by the current segIx is completed on this page
// updates the current segIx
static int AudioOggPageGetPktSize(sSpxStreamDcpt* pDcpt, int* pPktComplete)
{
	int segIx;
	int nBytes;
	int totBytes=0;

	*pPktComplete=0;	
	for(segIx=pDcpt->segIx; segIx<pDcpt->pageHdr.pageSegments; segIx++)
	{
		totBytes+=(nBytes=pDcpt->pageHdr.segmentTbl[segIx]);
		if(nBytes<255)
		{
			*pPktComplete=1;
			segIx++;
			break;
		}
	}
	
	pDcpt->segIx=segIx;
	return totBytes;
}


// returns the number of completed packets in an Ogg page
// packets are counted on the page they actually end, not on the page they begin.
static int AudioOggPagePackets(sOggPageSegHdr* pOggPage)
{
	int	ix;
	int	nPackets=0;
	
			
	for(ix=0; ix<pOggPage->pageSegments; ix++)
	{
		if(pOggPage->segmentTbl[ix]<255)
		{
			nPackets++;
		}
	}

	return nPackets;
} 
 
#endif	// AUDIO_SUPPORT_PLAY

#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES AudioSpxOpenOutStream(sStreamHandle* pSHndl)
{
	sSpxRunDcpt*	pCtrlDcpt;
	sSpeexHeader	spxHdr;
	sSpxStreamDcpt*	pSpxDcpt;
	sOggPageSegHdr*	pOggHdr;
	AUDIO_RES	res;


	// construct the run time Speex descriptor
	pSpxDcpt=(sSpxStreamDcpt*)malloc(sizeof(sSpxStreamDcpt));
	if(!pSpxDcpt)
	{
		return AUDIO_RES_OUT_OF_MEM;
	}
	

	pOggHdr=&pSpxDcpt->pageHdr;
	pCtrlDcpt=&pSpxDcpt->runDcpt;

	AudioSpxGetSettings(pCtrlDcpt);

	
	while(1)
	{	
		memset(pOggHdr, 0, sizeof(*pOggHdr));
		pOggHdr->pageCapture=OGG_ID_MAGIC;
		pOggHdr->headerFlags=OGG_FLAG_BOS;
		pOggHdr->streamNo=pCtrlDcpt->streamNo;	
		pOggHdr->pageSegments=1;
		pOggHdr->segmentTbl[0]=sizeof(sSpeexHeader);
	
		res=AudioOggWritePageHdr(pSHndl, pOggHdr);
		if(res!=AUDIO_RES_OK)
		{
			break;
		}
		
	
		// write the segment of the first page, the speex header	
		memset(&spxHdr, 0, sizeof(spxHdr));
		strncpy(spxHdr.speexString, OGG_ID_SPEEX, SPEEX_STRING_LENGTH);
	       	strncpy(spxHdr.speexVer, SPEEX_VERSION, SPEEX_VERSION_LENGTH);
		spxHdr.speexVerId=SPEEX_VERSION_ID;
		spxHdr.headerSize=sizeof(spxHdr);
		spxHdr.sampleRate=pSHndl->info.sampleRate;
		if(pCtrlDcpt->spxFlags&SPX_FLAG_WB)
		{
			spxHdr.wBand=1;
		}
		spxHdr.modeBitsStreamVer=pCtrlDcpt->streamVer;
		spxHdr.nChannels=pSHndl->info.numChannels;
		spxHdr.bitRate=pCtrlDcpt->bitRate;
		spxHdr.frameSamples=pCtrlDcpt->frameSamples;
		if(pCtrlDcpt->spxFlags&SPX_FLAG_VBR)
		{
			spxHdr.vbr=1;
		}
		spxHdr.framesPerPacket=pCtrlDcpt->framesPerPacket;
	
		if((*pSHndl->writeCb)((BYTE*)&spxHdr, sizeof(spxHdr))!=sizeof(spxHdr))
		{
			res=AUDIO_RES_WRITE_ERROR;
			break;
		}
	
		// write the comment
		pOggHdr->headerFlags=0;
		pOggHdr->pageNo=1;
		pOggHdr->pageSegments=1;
		pOggHdr->segmentTbl[0]=sizeof(MchpComment);
		res=AudioOggWritePageHdr(pSHndl, pOggHdr);
		if(res!=AUDIO_RES_OK)
		{
			break;
		}
		
		// now write the data part
		if((*pSHndl->writeCb)((BYTE*)&MchpComment, sizeof(MchpComment))!=sizeof(MchpComment))
		{
			res=AUDIO_RES_WRITE_ERROR;
			break;
		}

		// positioned at the very beginning of the data pages	
		pOggHdr->pageNo=2;
		pOggHdr->pageSegments=0;
		pSpxDcpt->segIx=pSpxDcpt->prevBytes=pSpxDcpt->pktIx=0;
		pSHndl->pHDcpt=pSpxDcpt;
		res=AUDIO_RES_OK;
		break;
	}
	
	if(res!=AUDIO_RES_OK)
	{
		free(pSpxDcpt);
	}

	return res;
}


// write the next data packet
// the caller can specify a page flush, if needed
// however, page flush could occur by itself if there's no more room in the current page to accomodate another packet
// Note: the current packet should always fit within this page, otherwise an error is returned!
static AUDIO_RES AudioSpxWriteData(sStreamHandle* pStr, void* pBuff, int nBytes, void* pResult)
{
	int		pktSz, ix;
	int		nSegs, remBytes;
	int		eos=0;
	int		pgFlush=0;	
	AUDIO_RES	res=AUDIO_RES_OK;
	sSpxStreamDcpt*	pSpxDcpt=(sSpxStreamDcpt*)pStr->pHDcpt;	// get the speex state machine info
	sOggPageSegHdr*	pPgHdr=&pSpxDcpt->pageHdr;

	if(nBytes)
	{
		pktSz=nBytes-pSpxDcpt->prevBytes;
		pSpxDcpt->prevBytes=nBytes;
		pSpxDcpt->pktIx++;	// another packet in
	
		if(pktSz>=255)
		{
			nSegs=pktSz/255;
			remBytes=pktSz-255*nSegs;
		}
		else
		{
			nSegs=0;
			remBytes=pktSz;
		}
		nSegs++;
	
		if((int)pPgHdr->pageSegments+nSegs>255)
		{	// ooops, this should not happen...
			// the assumption here is that a packet should always fit in a page
			return AUDIO_RES_BUFF_ERROR;
		}

		for(ix=0; ix<nSegs-1; ix++)
		{
			pPgHdr->segmentTbl[pSpxDcpt->segIx++]=255;
		}
		pPgHdr->segmentTbl[pSpxDcpt->segIx++]=remBytes;		// update the last segment
		pPgHdr->pageSegments+=nSegs;

		pgFlush=*(eSpxEncCmd*)pResult==SPX_CMD_PKT_FLUSH || (pSpxDcpt->segIx+nSegs)>=255;
		// the assumption is that all packets have approx the same size
		// TODO: this has to be changed for VBR!
		eos=0;
	}

	if(*(eSpxEncCmd*)pResult==SPX_CMD_CLOSE)
	{
		pgFlush=1;
		eos=1;
	}

	
	if(pgFlush)
	{	// we have to flush this page
		
		
		res=AudioOggFlushPage(pStr, pBuff, pSpxDcpt->prevBytes, eos);

		pSpxDcpt->segIx=pSpxDcpt->prevBytes=0;
		pPgHdr->pageNo++;

		*(eSpxEncCmd*)pResult=SPX_RPLY_FLUSHED;
	}
	else
	{
		*(eSpxEncCmd*)pResult=SPX_RPLY_OK;
	}
	
	return res;
}

// writes a page header to an output stream
static AUDIO_RES AudioOggWritePageHdr(sStreamHandle* pSHndl, sOggPageSegHdr* pOggPage)
{
	int		nBytes;
	int		pageBytes;

	pageBytes=sizeof(*pOggPage)-sizeof(pOggPage->segmentTbl)+pOggPage->pageSegments;	// page header size, just the segment table
	nBytes=(*pSHndl->writeCb)((BYTE*)pOggPage, pageBytes);

	return nBytes==pageBytes?AUDIO_RES_OK:AUDIO_RES_WRITE_ERROR;
}

// update a page info and flush the contents
static AUDIO_RES AudioOggFlushPage(sStreamHandle* pStr, void* pBuff, int nBytes, int eos)
{
	int		wrBytes;
	AUDIO_RES	res;
	sSpxStreamDcpt*	pSpxDcpt=(sSpxStreamDcpt*)pStr->pHDcpt;	// get the speex state machine info
	sOggPageSegHdr*	pPgHdr=&pSpxDcpt->pageHdr;


	pPgHdr->granulePos=(INT64)pSpxDcpt->pktIx*pSpxDcpt->runDcpt.framesPerPacket*pSpxDcpt->runDcpt.frameSamples;
	if(eos)
	{
		pPgHdr->headerFlags|=OGG_FLAG_EOS;
	}
	// TODO: update the page CRC...	
	res=AudioOggWritePageHdr(pStr, pPgHdr);	// write the page header
	if(res==AUDIO_RES_OK && nBytes)
	{	// write the page data
		wrBytes=(*pStr->writeCb)(pBuff, nBytes);
		pPgHdr->pageSegments=0;
		if(wrBytes!=nBytes)
		{
			res=AUDIO_RES_WRITE_ERROR; 
		}
	}

	return res;
}



#endif	// AUDIO_SUPPORT_RECORD



static AUDIO_RES AudioSpxCloseStream(sStreamHandle* pSHndl)
{
	// nothing to do for decode
	// for encode, taken care by the encoder itself
	return AUDIO_RES_OK;
}


#endif	// SPEEX_OGG_STREAMS

#endif	// defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)

