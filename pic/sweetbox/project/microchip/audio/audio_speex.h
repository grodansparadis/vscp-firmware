/*********************************************************************
 *
 *                  AUDIO speex wrapper header
 *
 *********************************************************************
 * FileName:        audio_speex.h
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
 *$Id: audio_speex.h 161 2008-10-15 19:30:28Z akhe $
 ********************************************************************/

#ifndef _AUDIO_SPEEX_H_

#define _AUDIO_SPEEX_H_ 

#include <audio/audio.h>
#include "audio_formats.h"

#ifdef SPEEX_ENCODING	


typedef enum
{
	SPX_FLAG_WB		= 0x1,		// wide/narrow band
	SPX_FLAG_VBR		= 0x2,		// VBR
	SPX_FLAG_BRATE_OVR	= 0x4,		// bit rate setting overrides the encoder quality
	SPX_FLAG_PRCPT_ENH	= 0x8,		// decoder perceptual enhancement on/off
}eSpxFlags;		// Speex flags
// Note: at most 8 flags are supported right now! 

typedef struct  __attribute__((packed))
{
	int		streamNo;		// stream number inside the container
	int		streamVer;		// stream version
	//	
	unsigned short	frameSamples;		// Size of frames, in samples
	unsigned short	bitRate;		// encoder bit rate 
	//
	unsigned char	framesPerPacket;	// Number of frames stored per Ogg packet, <10
	unsigned char	packetsPerPage;		// number of packets in an Ogg page <= 255
	unsigned char	complexity;		// encoder complexity 1-10
	unsigned char	qualFactor;		// encoder quality factor 1-10
	// 
	char		spxFlags;		// eSpxFlags: run time flags
	char		reserved[3];		// future expansion/padding
}sSpxRunDcpt;	// run time speex descriptor obtained from the stream with AudioStreamGetRunInfo()

typedef struct
{
	int		pktBytes;		// how many bytes in this packet
	int		pktSeqNo;		// packet sequence number
						// there's always framesPerPacket frames in a packet
}sSpxPktDcpt;	// decoder data packet descriptor	


typedef enum
{
	SPX_RPLY_OK,		// executed the previous command
	SPX_RPLY_FLUSHED,	// flush occurred (forced or not)
	SPX_CMD_PKT_FLUSH,	// new packet and flush output buffer
	SPX_CMD_PKT,		// new packet arrived
	SPX_CMD_CLOSE,		// end of stream
}eSpxEncCmd;	// commands/ reply returned to the Speex encoder


#define	SPEEX_FREE_PKT_VER	0x10	// the version of the current Speex free packet format

typedef struct  __attribute__((packed))
{
	unsigned char	spxFreeVer;		// SPEEX_FREE_PKT_VER: version of this header
	unsigned char	size;			// size of this header
	//	
	unsigned short	sampleRate;		// sample rate, HZ
	unsigned short	frameSamples;		// Size of frames, in samples
	unsigned short	bitRate;		// encoder bit rate 
	//
	unsigned char	framesPerPacket;	// Number of frames stored per data packet, <10
	unsigned char	complexity;		// encoder complexity 1-10
	unsigned char	qualFactor;		// encoder quality factor 1-10
	unsigned char	spxFlags;		// eSpxFlags: run time flags
}sSpxFreeDcpt;	// free stream Speex decriptor, stored within the stream

typedef struct __attribute__((packed))
{
	unsigned short	pktSize;
	char		spxData[2];
}sSpxFreeDataPkt;		// layout of an Speex free format data packet
/*
 *  |pktSz| frm1, frm2,..., frmN|;	layout for a data packet consisting of multiple framesPerPacket
 *  
*/


int			AudioSpxSetBandMode(int wBand);


int			AudioSpxSetComplexity(int cplxN);


int			AudioSpxSetQuality(int newQual);

int			AudioSpxSetBitRate(int bRate);

int			AudioSpxSetFramesNo(int nFrames);

int			AudioSpxSetPacketsNo(int nPackets);

// turn on/off speex perceptual enhancement
int			AudioSpxSetPrcptEnh(int enh);


// get the user settings
void			AudioSpxGetSettings(sSpxRunDcpt* pRunDcpt);



// low level access functions
//

// returns the number of samples needed in the decoder out buffer
int			SpxDecoderOutSamples(int procSamples, sStreamInfo* pInfo);

// init the decoder
AUDIO_RES		SpxInitDecoder(int procSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl);

// start decoding data
AUDIO_RES		SpxReadDecodeData(audioCodecHandle h, progressDcpt* pPrgDcpt);

// clean-up, after done
void			SpxCleanUpDecoder(audioCodecHandle h);

// get the output work buffer size
int			SpxGetOutBuffSize(audioCodecHandle h);

// get the working output buffer
void*			SpxGetOutBuff(audioCodecHandle h);

// set the working output buffer
void			SpxSetOutBuff(audioCodecHandle h, void* pBuff);

// get the decoded bytes and clear internal counter
int			SpxGetOutBytes(audioCodecHandle h, int clr);

// returns the number of samples needed in the encoder input buffer
int			SpxEncoderInSamples(int procSamples, sStreamInfo* pInfo);

// init the encoder
AUDIO_RES		SpxInitEncoder(int procSamples, sStreamInfo* pInfo, eBuffMask buffMask, audioCodecHandle* pHndl);

// read input data and encode to the output buffer
AUDIO_RES		SpxEncodeWriteData(audioCodecHandle h, int nBytes, progressDcpt* pPrgDcpt);

// clean-up, when done
void			SpxCleanUpEncoder(audioCodecHandle h);

// get the working input buffer
void*			SpxGetInBuff(audioCodecHandle h);

// get the input buffer size
int			SpxGetInBuffSize(audioCodecHandle h);

// set the working input buffer
void			SpxSetInBuff(audioCodecHandle h, void* pBuff);

// stop decoder and flush out some data that's already in the internal buffers
AUDIO_RES		SpxEncoderStop(audioCodecHandle h);

// check the play format
AUDIO_RES		SpxCheckPlayFormat(sStreamInfo* pInfo);


// check the record format
AUDIO_RES		SpxCheckRecordFormat(sStreamInfo* pInfo);




#endif	// SPEEX_ENCODING	


#endif	// _AUDIO_SPEEX_H_

