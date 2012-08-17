/*********************************************************************
 *
 *                  AUDIO Library Implementation
 *
 *********************************************************************
 * FileName:        audio.c
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
 *$Id: audio.c 161 2008-10-15 19:30:28Z akhe $
 *
 **********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include <audio/audio.h>

#include "audio_formats.h"
#include "audio_streams.h"

#include "audio_pcm.h"

#ifdef ADPCM_ENCODING
#include "audio_adpcm.h"
#endif

#ifdef SPEEX_ENCODING
#include "audio_speex.h"
#endif


#include <plib.h>


#if defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)


#define DEBUG_SYNC				// use RD8 as sync output
	

#ifdef DEBUG_SYNC
	#define	DEBUG_SYNC_HIGH()		mPORTDSetBits(0x100)
	#define	DEBUG_SYNC_LOW()		mPORTDClearBits(0x100)
#else
	#define	DEBUG_SYNC_HIGH()
	#define	DEBUG_SYNC_LOW()
#endif

#define AUDIO_BITS_PER_SAMPLE		16	// number of bits per sample for unencoded PCM (RAW) samples
// NOTE: the current build of the Audio library is designed to work with 16 bit signed samples
// No support for other sample size is available yet.

// constant data


typedef enum
{
	ISR_READY,
	ISR_RUN
}ISRStat;	// status of the ISR processing


typedef struct
{
	int			isrProcDone;		// flag signalling the isr has no more need of this buffer
	short*			pBuff;			// pointer to the buffer containing samples to be processed
	short*			pEnd;			// end of buffer marker
}ISRBuffDcpt;	// descriptor for an ISR buffer

typedef struct
{
	ISRBuffDcpt		bDcpt[2];		// use a double buffer scheme: ISR toggles from one buffer to the other
	int			dcptIx;			// index of the current ISR buffer
	int			isrStop;		// stop after processing the current buffer, don't start a new one
	ISRStat			stat;			// current ISR procesing status
	int			isrCnt;			// if using higher freq than 8KHz
	int			isrRate;		// reload rate
	int			overrun;		// performance counter
}ISRDcpt;	// descriptor of the ISR needed data

// global data
//

static volatile ISRDcpt		isrDcpt = {{{0}}};	// ISR descriptor
static volatile int		audioEvFlag;		// flag to be set when the player needs data

static AUDIO_STATUS		audioCurrStat = STOP;	// current state

static short*			sampleBuff[2] = {0, 0};		// samples buffer holding at least audioProcSamples (AUDIO_BITS_PER_SAMPLE bit samples)

static pReadCB			usrReadCb;
static pWriteCB			usrWriteCb;			// callbacks for data retrieval/storage
static pSeekStreamCB		usrInSeekCb;
static pSeekStreamCB		usrOutSeekCb;			// callbacks for repositioning in the in/out stream

static pSetEvent		usrNotifyCB;			// notification callback
static pSetEvent		audioPctCB=0;			// punctual record notification callback


static int			audioTmrDiv;			// value to divide the PB clock to obtain the timer tick
static int			audioTmrPeriod;			// the generator period, derived from the AUDIO_SAMPLE_FREQ


static int			audioSysFreq;				// system running frequency

#ifdef AUDIO_SUPPORT_PLAY
static int			audioPlayRate=AUDIO_PLAY_RATE;		// player rate
static AUDIO_STREAM_FORMAT	audioStreamInFmt=AUDIO_STREAM_QUERY;	// default value for the input stream
static audioCodecHandle		audioDecHandle=0;			// current decoder handle
#endif

#ifdef AUDIO_SUPPORT_RECORD
static int			audioRecordRate=AUDIO_RECORD_RATE;	// recorder rate
static AUDIO_STREAM_FORMAT	audioStreamOutFmt=(AUDIO_STREAM_FORMAT)0;	// default value for the output stream
static audioCodecHandle		audioEncHandle=0;			// current encoder handle
#endif

#ifdef ADPCM_ENCODING
static int			audioAdpcmLe=AUDIO_ADPCM_NIBBLES_LE;	// default value for the ADPCM nibbles endianess
#endif

static sStreamInfo		audioStrInfo=
{
	AUDIO_STREAM_QUERY,				// sFmt
	(AUDIO_FORMAT)0,				// aFmt
	PLAY,						// aOp
	AUDIO_BITS_PER_SAMPLE,				// bitsPerSample, corresponding to RAW (unencoded PCM)
	//
	1,						// numChannels
	1,						// play active channel
	1,						// lEndian: always LE
	0,						// reserved
	//
	0,						// streamSz
	AUDIO_SAMPLE_FREQ,				// sampleRate
	AUDIO_BITS_PER_SAMPLE*AUDIO_SAMPLE_FREQ,	// bitRate, no compression by default
};	// default/current values for the audio stream




static sStreamInfo		audioCurrInfo;		// the info for the current played/recorded stream
							// when playing this could get updated from the stream

static audioSHandle		audioInHandle=0;	// current opened istream handle
static audioSHandle		audioOutHandle=0;	// current opened ostream handle


static int			audioProcSamples=((AUDIO_SAMPLES_BLK+127)/128)*128;	// default value for the number of samples.	
static int			audioMinFreq=AUDIO_MIN_WORK_FREQ;	// default value for the minumum work frequency


static int			audioDmaEnabled=AUDIO_DMA_ENABLED;
static int			audioDmaChn=AUDIO_DMA_CHN;
static int			audioDmaPri=AUDIO_DMA_CHN_PRI;
static int			audioDmaIpl=AUDIO_SAMPLE_IPL;		// default values for DMA usage

static volatile int*		audioDmaAdd=0;				// DMA play/record device address

static AUDIO_SDEV		audioSamplingDev=AUDIO_SAMPLE_DEVICE;
static int			audioSamplingIpl=AUDIO_SAMPLE_IPL;	// default sampling device settings



#ifdef AUDIO_SUPPORT_PLAY
static const AudioDecDcpt	audioDecDcpt[AUDIO_FORMATS]=
{
#ifdef PCM_ENCODING
	// AUDIO_PCM
	{1, PcmDecoderOutSamples, PcmInitDecoder, PcmReadDecodeData, PcmCleanUpDecoder, PcmGetBuffSize, PcmGetOutBuff, PcmGetBytes,  PcmSetOutBuff, PcmCheckFormat},
#endif

#ifdef ADPCM_ENCODING	
	// AUDIO_ADPCM
	{1, AdpcmDecoderOutSamples, AdpcmInitDecoder, AdpcmReadDecodeData, AdpcmCleanUpDecoder, AdpcmGetOutBuffSize, AdpcmGetOutBuff, AdpcmGetOutBytes, AdpcmSetOutBuff,
	       AdpcmCheckFormat},
#endif
	// AUDIO_SPEEX	
#ifdef SPEEX_ENCODING
	{1, SpxDecoderOutSamples, SpxInitDecoder, SpxReadDecodeData, SpxCleanUpDecoder, SpxGetOutBuffSize, SpxGetOutBuff, SpxGetOutBytes, SpxSetOutBuff, SpxCheckPlayFormat},
#endif

};
#endif	// AUDIO_SUPPORT_PLAY

#ifdef AUDIO_SUPPORT_RECORD
static const AudioEncDcpt	audioEncDcpt[AUDIO_FORMATS]=
{
#ifdef PCM_ENCODING
	// AUDIO_PCM
	{1, PcmEncoderInSamples, PcmInitEncoder, PcmEncodeWriteData, 0, PcmCleanUpEncoder, PcmGetBuffSize, PcmGetInBuff, PcmSetInBuff, PcmGetBytes, PcmCheckFormat},
#endif

#ifdef ADPCM_ENCODING
	// AUDIO_ADPCM
	{1, AdpcmEncoderInSamples, AdpcmInitEncoder, AdpcmEncodeWriteData, 0, AdpcmCleanUpEncoder, AdpcmGetInBuffSize, AdpcmGetInBuff, AdpcmSetInBuff, AdpcmGetOutBytes, AdpcmCheckFormat},
#endif

	// AUDIO_SPEEX	
#ifdef SPEEX_ENCODING
	{1, SpxEncoderInSamples, SpxInitEncoder, SpxEncodeWriteData, SpxEncoderStop, SpxCleanUpEncoder, SpxGetInBuffSize, SpxGetInBuff, SpxSetInBuff, SpxGetOutBytes, SpxCheckRecordFormat}, 
#endif

};
#endif	// AUDIO_SUPPORT_RECORD



// local prototypes

static void			AudioStartIsrProcess(AUDIO_STATUS currStat, volatile ISRBuffDcpt* pDcpt);

static void			AudioStartSmplDevice(void);
static void			AudioStopSmplDevice(void);

static AUDIO_RES		AudioStopProcessing(int abort);

#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES		AudioInitPlay(sStreamInfo* pInfo);
static AUDIO_RES		AudioLibPlay(void);
static AUDIO_RES		AudioInitDecoder(sStreamInfo* pInfo, int nSamples, int bPlayBuff, audioCodecHandle* pDecHndl);
static AUDIO_RES		AudioCheckPlayFormat(sStreamInfo* pInfo);
#endif

#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES		AudioInitRecord(sStreamInfo* pInfo);
static AUDIO_RES		AudioLibRecord(void);
static AUDIO_RES		AudioInitEncoder(sStreamInfo* pInfo, int nSamples, int bRecBuff, audioCodecHandle* pEncHndl);
static AUDIO_RES		AudioCheckRecordFormat(sStreamInfo* pInfo);
#endif


static int			AudioStartDmaProcess(AUDIO_STATUS currStat, volatile ISRBuffDcpt* pDcpt);

static void			AudioStallDmaProcess(AUDIO_STATUS currStat, volatile ISRBuffDcpt* pDcpt);

static void			AudioInitDma(void);

#if PROFILING
	static unsigned int		audioProfStart=0;
	static unsigned long long	audioProfCycles=0;
	static int			audioProfOn=0;
	static int			audioProfIo=0;

	static unsigned int		audioProfIntStart=0;
	static unsigned long long	audioProfIntCycles=0;
	static unsigned long long	audioProfOvlapCycles=0;


	static pProgressFnc		audioProgressFnc=0;

	
	extern __inline__ void __attribute((always_inline)) ProfileIntStart(void)
	{
		audioProfIntStart=ReadCoreTimer();
	}

	extern __inline__ void __attribute((always_inline)) ProfileIntUpdate(void)
	{
		unsigned int ticks=ReadCoreTimer()-audioProfIntStart;
		audioProfIntCycles+=ticks;
		if(audioProfOn)
		{
			audioProfOvlapCycles+=ticks;
		}
	}


	extern __inline__ void __attribute((always_inline)) AudioProfilingEnable(void)
	{
		if(!audioProfOn)
		{
			audioProfStart=ReadCoreTimer();
			audioProfOn=1;
		}
		else
		{
			__asm __volatile ("teq $0, $0");
		}
	}

	extern __inline__ void __attribute((always_inline)) AudioProfilingDisable(void)
	{
		if(audioProfOn)
		{
			unsigned int ticks=ReadCoreTimer();
			audioProfOn=0;
			audioProfCycles+=(ticks-audioProfStart);
		}
		else
		{
			__asm __volatile ("teq $0, $0");
		}
	}


	extern __inline__ void __attribute((always_inline)) AudioProfilingRefresh(void)
	{
		if(audioProfOn)
		{
			unsigned int currTicks=ReadCoreTimer();
			audioProfCycles+=(currTicks-audioProfStart);
			audioProfStart=currTicks;
		}
		else
		{
			__asm __volatile ("teq $0, $0");
		}
	}


	void AudioProfilingInit(void)
	{
		audioProfOn=0;
		audioProfCycles=audioProfIntCycles=audioProfOvlapCycles=0;
	}

	void AudioSetProfileIo(int profIo)
	{
		audioProfIo=profIo;
	}

	int AudioGetProfileIo(void)
	{
		return audioProfIo;
	}	
	
	
	unsigned long long AudioProfilingCycles(void)
	{
		return audioProfCycles*2;
	}

	unsigned long long AudioProfilingIntCycles(void)
	{
		return audioProfIntCycles*2;
	}


	unsigned long long AudioProfilingOvlapCycles(void)
	{
		return audioProfOvlapCycles*2;
	}


	int GetReadCb(unsigned char* samplesBuff, int bSize)
	{
		int	n;
		
		if(!audioProfIo)
		{
			AudioProfilingDisable();
			n=(*usrReadCb)(samplesBuff, bSize);
			AudioProfilingEnable();
			return n;
		}
		else
		{
			n=(*usrReadCb)(samplesBuff, bSize);
			AudioProfilingRefresh();
		}

		return n;
	}

	int GetWriteCb(unsigned char* samplesBuff, int nSamples)
	{
		int	n;
		
		if(!audioProfIo)
		{
			AudioProfilingDisable();
			n=(*usrWriteCb)(samplesBuff, nSamples);
			AudioProfilingEnable();
			return n;
		}
		else
		{
			n=(*usrWriteCb)(samplesBuff, nSamples);
			AudioProfilingRefresh();
		}

		return n;
		
	}
	

	int GetInSeekCb(long offset, ORIGIN_SEEK origin)
	{
		int	res;
		
		if(!audioProfIo)
		{
			AudioProfilingDisable();
			res=(*usrInSeekCb)(offset, origin);
			AudioProfilingEnable();
		}
		else
		{
			res=(*usrInSeekCb)(offset, origin);
			AudioProfilingRefresh();
		}

		return res;
	}


	int GetOutSeekCb(long offset, ORIGIN_SEEK origin)
	{
		int	res;
		
		if(!audioProfIo)
		{
			AudioProfilingDisable();
			res=(*usrOutSeekCb)(offset, origin);
			AudioProfilingEnable();
		}
		else
		{
			res=(*usrOutSeekCb)(offset, origin);
			AudioProfilingRefresh();
		}

		return res;
		
	}

	void GetNotifyCb(void)
	{
		if(!audioProfIo)
		{
			AudioProfilingDisable();
			(*usrNotifyCB)();
			AudioProfilingEnable();
		}
		else
		{
			(*usrNotifyCB)();
			AudioProfilingRefresh();
		}
	}		


	void AudioProfileProgress(int nBytes)
	{
		AudioProfilingDisable();
		(*audioProgressFnc)(nBytes);
		AudioProfilingEnable();
	}		


#else
	#define AudioProfilingEnable()
	#define AudioProfilingDisable()
	#define	ProfileIntStart()
	#define ProfileIntUpdate()

	#define	GetReadCb	usrReadCb
	#define	GetWriteCb	usrWriteCb
	#define	GetInSeekCb	usrInSeekCb
	#define	GetOutSeekCb	usrOutSeekCb
	#define	GetNotifyCb	usrNotifyCB
#endif



/*********************************
 * some local inlined functions
 * *********************************************/



/************************************************************************
 * Function:       AudioDisableSmplDeviceInt 
 *
 * PreCondition:   None 
 *
 * Input:          None 
 *
 * Output:         None.  
 *
 * Side Effects:   None
 *
 * Overview:       Disables the sampling timer device interrupt. 
 *
 * Note:           None 
 *****************************************************************************/
extern __inline__ void __attribute((always_inline)) AudioDisableSmplDeviceInt(void)
{
	if(audioDmaEnabled)
	{
		DmaChnIntDisable(audioDmaChn);
	}
	else if(audioSamplingDev==AUDIO_SDEV_TMR2)
	{
		mT2IntEnable(0);
	}
	else
	{
		mT3IntEnable(0);
	}			
}


/************************************************************************
 * Function:       AudioEnableSmplDeviceInt 
 *
 * PreCondition:   None 
 *
 * Input:          None 
 *
 * Output:         None.  
 *
 * Side Effects:   None
 *
 * Overview:       Enables the sampling timer device interrupt. 
 *
 * Note:           None 
 *****************************************************************************/
extern __inline__ void __attribute((always_inline)) AudioEnableSmplDeviceInt(void)
{
	if(audioDmaEnabled)
	{
		DmaChnIntEnable(audioDmaChn);
	}	
	else if(audioSamplingDev==AUDIO_SDEV_TMR2)
	{
		mT2IntEnable(1);
	}
	else
	{
		mT3IntEnable(1);
	}			
}


/************************************************************************
 * Function:       AudioAckSmplDeviceInt 
 *
 * PreCondition:   None 
 *
 * Input:          None 
 *
 * Output:         None.  
 *
 * Side Effects:   None
 *
 * Overview:       Acknowledges the sampling timer device interrupt. 
 *
 * Note:           None 
 *****************************************************************************/
extern __inline__ void __attribute((always_inline)) AudioAckSmplDeviceInt(void)
{
	if(audioDmaEnabled)
	{
		DmaChnClrEvFlags(audioDmaChn, DMA_EV_BLOCK_DONE);
		DmaChnClrIntFlag(audioDmaChn);
	}	
	else if(audioSamplingDev==AUDIO_SDEV_TMR2)
	{
		mT2ClearIntFlag();
	}
	else
	{
		mT3ClearIntFlag();
	}			
}


/************************************************************************
 * Function:       AudioStartSmplDevice 
 *
 * PreCondition:   None 
 *
 * Input:          None 
 *
 * Output:         None.  
 *
 * Side Effects:   None
 *
 * Overview:       Stops the sampling timer device. 
 *
 * Note:           None 
 *****************************************************************************/
static void AudioStartSmplDevice(void)
{
	
	// set the proper timer range
	if(audioSamplingDev==AUDIO_SDEV_TMR2)
	{
		if(audioDmaEnabled)
		{
			ConfigIntTimer2(T2_INT_OFF);
		}
		else
		{
			ConfigIntTimer2(T2_INT_ON | audioSamplingIpl | T2_INT_SUB_PRIOR_3);			
		}
		
		OpenTimer2(T2_ON | T2_IDLE_CON | ((audioTmrDiv) << _T2CON_TCKPS_POSITION), (audioTmrPeriod-1));

	
	}
	else
	{
		if(audioDmaEnabled)
		{
			ConfigIntTimer3(T3_INT_OFF);
		}
		else
		{
			ConfigIntTimer3(T3_INT_ON | audioSamplingIpl | T3_INT_SUB_PRIOR_3);			
		}		
		
		OpenTimer3(T3_ON | T3_IDLE_CON | ((audioTmrDiv) << _T3CON_TCKPS_POSITION), (audioTmrPeriod-1));
	}

	if(audioDmaEnabled)
	{
		DmaChnEnable(audioDmaChn);
	}
}
			
	
/************************************************************************
 * Function:       AudioStopSmplDevice 
 *
 * PreCondition:   None 
 *
 * Input:          None 
 *
 * Output:         None.  
 *
 * Side Effects:   None
 *
 * Overview:       Stops the sampling timer device. 
 *
 * Note:           None 
 *****************************************************************************/
static void AudioStopSmplDevice(void)
{
	AudioDisableSmplDeviceInt();		// Disable sampling interrupt

	if(audioSamplingDev==AUDIO_SDEV_TMR2)
	{
		OpenTimer2(T2_OFF, 0);
	}
	else
	{
		OpenTimer3(T3_OFF, 0);
	}

	if(audioDmaEnabled)
	{
		DmaChnDisable(audioDmaChn);
	}	
}



/*********************************************
 * 		interface functions
 *******************************************************/


/****************************************************************************
 * Function:        AudioInit
 *
 * PreCondition:    None
 *
 * Input:           system running frequency, read and write callbacks, notification callback
 *
 * Output:          always AUDIO_RES_OK 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the audio library. 
 *
 * Note:            This has to be called by the user prior to any other 
 *                  audio functions.
 *****************************************************************************/
AUDIO_RES AudioInit(int sysFreq, pReadCB readCb, pWriteCB writeCB, pSetEvent eventCB)
{	

#if (AUDIO_SAMPLE_FREQ/AUDIO_PLAY_RATE)<AUDIO_MIN_WORK_FREQ || (AUDIO_SAMPLE_FREQ/AUDIO_RECORD_RATE)<AUDIO_MIN_WORK_FREQ
	#error "Sample frequency too low. Increase the AUDIO_SAMPLE_FREQ"
#endif

	if(audioCurrStat!=STOP)
	{
		AudioClose();
	}
	
	audioSysFreq=sysFreq;

	
	usrReadCb=readCb;
	usrWriteCb=writeCB;
	usrNotifyCB=eventCB;		// save the event flag callback
	audioEvFlag=0;			// clear data request

#ifdef DEBUG_SYNC
	// init the debug pin	
	mPORTDSetPinsDigitalOut(0x100);		
	DEBUG_SYNC_LOW();
#endif
	
   return AUDIO_RES_OK;
   
}

/****************************************************************************
 * Function:        AudioSetCallback
 *
 * PreCondition:    None
 *
 * Input:           callback type and callback value
 *
 * Output:          AUDIO_RES_OK if ok, AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function sets a callback used for playing/recording and/or encoding/decoding.
 *
 * Note:            If the necessary callback is not provided with the AudioInit(),
 * 		    this function has to be called before calling the audio functions that might need the callbacks.
 *****************************************************************************/
AUDIO_RES AudioSetCallback(AUDIO_CB cbType, void* cbFnc)
{
	AUDIO_RES	res;


	if(audioCurrStat==STOP)
	{
		res=AUDIO_RES_OK;
		
		switch(cbType)
		{
			case AUDIO_CB_READ:
				usrReadCb=(pReadCB)cbFnc;			
				break;			
			case AUDIO_CB_WRITE:
				usrWriteCb=(pWriteCB)cbFnc;			
				break;			
			case AUDIO_CB_SEEK_IN:
				usrInSeekCb=(pSeekStreamCB)cbFnc;			
				break;			
			case AUDIO_CB_SEEK_OUT:
				usrOutSeekCb=(pSeekStreamCB)cbFnc;			
				break;			
			case AUDIO_CB_EVENT:
				usrNotifyCB=(pSetEvent)cbFnc;			
				break;
			default:
				res=AUDIO_RES_UNSUPPORTED;
				break;
		}
	}
	else
	{
		res=AUDIO_RES_BAD_CALL;
	}

	return res;

}

/****************************************************************************
 * Function:        AudioGetCallback
 *
 * PreCondition:    None
 *
 * Input:           callback type
 *
 * Output:          a callback value or 0 if not supported callback type 
 *
 * Side Effects:    None
 *
 * Overview:        This function gets the value of a callback used for playing/recording and/or encoding/decoding.
 *
 * Note:            None
 *****************************************************************************/
void* AudioGetCallback(AUDIO_CB cbType)
{
	void*	cbFnc=0;
	
	switch(cbType)
	{
		case AUDIO_CB_READ:
			cbFnc=GetReadCb;			
			break;			
		case AUDIO_CB_WRITE:
			cbFnc=GetWriteCb;			
			break;			
		case AUDIO_CB_SEEK_IN:
			cbFnc=GetInSeekCb;			
			break;			
		case AUDIO_CB_SEEK_OUT:
			cbFnc=GetOutSeekCb;			
			break;			
		case AUDIO_CB_EVENT:
			cbFnc=GetNotifyCb;			
			break;
		default:
			break;
	}

	return cbFnc;
}


/****************************************************************************
 * Function:        AudioClose
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          always AUDIO_RES_OK 
 *
 * Side Effects:    None
 *
 * Overview:        Clean-up of the resorces allocated by the AudioInit
 *
 * Note:            This has to be called by the user when done with the 
 *                  audio library.
 *****************************************************************************/
AUDIO_RES AudioClose(void)
{
	AudioStopProcessing(1);	// stop everything

	return AUDIO_RES_OK;	
}



/******************************************************************************
 * Function:        AudioPlay
 *
 * PreCondition:    AudioInit has to be called.
 *
 * Input:           None
 *
 * Output:          return AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function starts the audio play using the current selected audio format.
 * 					When the player needs data, the registered notification callback is called.
 * 					For actually filling the buffer with data, the registered read callback is called.
 *
 * Note:            This has to be called from the user application when the notification is set.
 * 		    The rate of call is indirectly set by audioProcSamples. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES AudioPlay(void)
{
	AUDIO_RES res;

	AudioProfilingEnable();

	audioEvFlag=0;		// clear data event flag. we've just been called
	
	if(audioCurrStat==PAUSE_PLAY)
	{
		res=AudioResume();
		AudioProfilingDisable();
		return res;
	}
	else if(audioCurrStat!=STOP && audioCurrStat!=PLAY )
	{
		AudioProfilingDisable();
		return AUDIO_RES_BAD_CALL;	//  error
	}
	
	if(audioCurrStat==STOP)
	{	// time to start processing
		sStreamInfo*	pInfo=&audioCurrInfo;
		
		*pInfo=audioStrInfo;		// update what caller asks for
		pInfo->aOp=PLAY;		// make sure it's play
		pInfo->sFmt=audioStreamInFmt;	// update the input stream format

		audioDecHandle=0;

		while(1)
		{
			if((res=AudioStreamOpen(pInfo, &audioInHandle))!=AUDIO_RES_OK)
			{
				break;
			}
			
			if((res=AudioCheckPlayFormat(pInfo))!=AUDIO_RES_OK)
			{
				break;
			}

			if((res=AudioInitDecoder(pInfo, audioProcSamples, 1, &audioDecHandle))!=AUDIO_RES_OK)
			{
				break;
			}

			res=AudioInitPlay(pInfo);
		
			break;
		}

		if(res==AUDIO_RES_OK)
		{	// success
			memset((void*)&isrDcpt, 0, sizeof(isrDcpt));
			isrDcpt.bDcpt[0].isrProcDone=isrDcpt.bDcpt[1].isrProcDone=1;	// show buffers 0 is available
			isrDcpt.dcptIx=1;						// mimic buffer 1 is used by the ISR
			isrDcpt.isrRate=isrDcpt.isrCnt=audioPlayRate;
		}
		else
		{	// fail
			AudioStreamCleanup(audioInHandle);
			audioInHandle=0;
			if(audioDecHandle)
			{
				const AudioDecDcpt*	pDDcpt=audioDecDcpt+pInfo->aFmt;	
				(*pDDcpt->decoderCleanup)(audioDecHandle);
				audioDecHandle=0;
			}
		}
	}
	else
	{	// check if we have to stop
		if(isrDcpt.isrStop)
		{
			AudioStopProcessing(0);		// be sure to abort the current operation
			res=AUDIO_RES_DONE;
		}
		else
		{
			res=AUDIO_RES_OK;
		}
	}


	if(res==AUDIO_RES_OK)
	{
		res=AudioLibPlay();
	}

	AudioProfilingDisable();
	return res;
}
#endif

/******************************************************************************
 * Function:        AudioRecord
 *
 * PreCondition:    AudioInit has to be called.
 *
 * Input:           None
 *
 * Output:          return AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function starts the audio recording using the current selected audio format.
 * 					When the recorder has available data, the registered notification callback is called.
 * 					To actually pass the data buffer to the application for storage, the registered write callback is called.
 *
 * Note:            This has to be called from the user application when the notification is set.
 * 		    The rate of call is indirectly set by audioProcSamples. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioRecord(void)
{
	AUDIO_RES res;
	AudioProfilingEnable();

	audioEvFlag=0;			// clear data event flag; we've just been called
	
	if(audioCurrStat==PAUSE_RECORD)
	{
		res=AudioResume();
		AudioProfilingDisable();
		return res;
	}
	
	if(audioCurrStat!=STOP && audioCurrStat!=RECORD )
	{
		AudioProfilingDisable();
		return AUDIO_RES_BAD_CALL;	//  error
	}

	
	if(audioCurrStat==STOP)
	{	// some initialization
		int			nSamples;
		const AudioEncDcpt*	pEDcpt;
		sStreamInfo*		pInfo=&audioCurrInfo;

		*pInfo=audioStrInfo;		// update what caller asks for
		pInfo->aOp=RECORD;		// make sure it's record
		pInfo->sFmt=audioStreamOutFmt;	// update the output stream format

		audioEncHandle=0;

		while(1)
		{
			if((res=AudioCheckRecordFormat(pInfo))!=AUDIO_RES_OK)
			{
				break;
			}
					
			if((res=AudioInitEncoder(pInfo, audioProcSamples, 1, &audioEncHandle))!=AUDIO_RES_OK)
			{
				break;
			}

			if((res=AudioStreamOpen(pInfo, &audioOutHandle))!=AUDIO_RES_OK)
			{
				break;
			}
			
			res=AudioInitRecord(pInfo);
			break;
		}

		pEDcpt=audioEncDcpt+audioCurrInfo.aFmt;	
		if(res==AUDIO_RES_OK)
		{	// success
			memset((void*)&isrDcpt, 0, sizeof(isrDcpt));
	
			nSamples=(*pEDcpt->encoderGetInSize)(audioEncHandle)/sizeof(short);
			
			
			// prepare for the ISR use
			isrDcpt.bDcpt[0].isrProcDone=0;		// show buffer not filled with data
			isrDcpt.bDcpt[0].pBuff=sampleBuff[0];
			isrDcpt.bDcpt[0].pEnd=sampleBuff[0]+nSamples;
			isrDcpt.bDcpt[1].isrProcDone=0;		// show buffer not filled with data
			isrDcpt.bDcpt[1].pBuff=sampleBuff[1];
			isrDcpt.bDcpt[1].pEnd=sampleBuff[1]+nSamples;
		
			isrDcpt.dcptIx=1;					// mimic buffer 1 was last used by the ISR
			isrDcpt.isrRate=isrDcpt.isrCnt=audioRecordRate;

			AudioStartIsrProcess((audioCurrStat=RECORD), isrDcpt.bDcpt); 	// Now enable the ISR processing
		}
		else
		{
			AudioStreamCleanup(audioOutHandle);
			audioOutHandle=0;
			if(audioEncHandle)
			{
				(*pEDcpt->encoderCleanup)(audioEncHandle);
				audioEncHandle=0;
			}
		}
	}
	else
	{
		res=AUDIO_RES_OK;
	}
		
		
	if(res==AUDIO_RES_OK)
	{
		res=AudioLibRecord();
	}

	AudioProfilingDisable();	
	return res;
}
#endif	// AUDIO_SUPPORT_RECORD



/************************************************************************
 * Function:        AudioStatus
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          return the current state of the audio library.
 *
 * Side Effects:    None
 *
 * Overview:        Helper to return the current play state.
 *
 * Note:            None
 *****************************************************************************/
AUDIO_STATUS AudioStatus(void)
{
	return audioCurrStat;
}


/************************************************************************
 * Function:       AudioPause 
 *
 * PreCondition:   None 
 *
 * Input:          None
 *
 * Output:         AUDIO_RES_OK if succes, an error code otherwise.
 *
 * Side Effects:   None
 *
 * Overview:       This is normally be called while playing/recording an audio.
 *                 It will suspend the current audio operation.
 * 					The play/record can be resumed using AudioResume().
 *
 * Note:           None
 **************************************************************************/
AUDIO_RES AudioPause(void)
{
	AUDIO_RES	res=AUDIO_RES_OK;
	
	switch(audioCurrStat)
	{
 		case PAUSE_PLAY:
		case PAUSE_RECORD:
			break;
	#ifdef AUDIO_SUPPORT_PLAY
		case PLAY:
			AudioDisableSmplDeviceInt();	//Disable sample interrupt
			AudioIODisableOutAmp();
			audioCurrStat=PAUSE_PLAY;
			break;
	#endif

	#ifdef AUDIO_SUPPORT_RECORD
		case RECORD:
			AudioDisableSmplDeviceInt();	//Disable sample interrupt
			AudioIODisableInAmp();
			audioCurrStat=PAUSE_RECORD;
			break;
	#endif
		default:	// STOP
			res= AUDIO_RES_BAD_CALL;
			break;
	}
   
	return res;
}


/************************************************************************
 * Function:       AudioStop 
 *
 * PreCondition:   None 
 *
 * Input:          None 
 *
 * Output:         AUDIO_RES_OK.  
 *
 * Side Effects:   None
 *
 * Overview:       calling this function will stop the current audio operation, record/play. 
 *
 * Note:           None 
 *****************************************************************************/
AUDIO_RES AudioStop(void)
{
	AUDIO_RES res;

	AudioProfilingEnable();
	
	res=AudioStopProcessing(0);

	AudioProfilingDisable();

	return res;
}


/************************************************************************
 * Function:       AudioResume 
 *
 * PreCondition:   None 
 *
 * Input:          None
 *
 * Output:         AUDIO_RES_OK if succes, an error code otherwise.
 *
 * Side Effects:   None
 *
 * Overview:       This function is called to resume the play from the paused state.
 *
 * Note:           None
 **************************************************************************/
AUDIO_RES AudioResume(void)
{   
   
	AUDIO_RES	res=AUDIO_RES_OK;
	int			resume=0;
	
	switch(audioCurrStat)
	{
	#ifdef AUDIO_SUPPORT_PLAY
 		case PAUSE_PLAY:
			audioCurrStat=PLAY;
			AudioIOEnableOutAmp();
			resume=1;
			break;
	#endif

	#ifdef AUDIO_SUPPORT_RECORD
		case PAUSE_RECORD:
			audioCurrStat=RECORD;
			AudioIOEnableInAmp();
			resume=1;
			break;
	#endif
		default:
			res= AUDIO_RES_BAD_CALL;
			break;
	}

	if(resume)
	{
		AudioEnableSmplDeviceInt();
	}
   
	return res;
}



/************************************************************************
 * Function:       AudioSetProcSamples 
 *
 * PreCondition:   nSamples>0 
 *
 * Input:          the number of samples
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the number of the processed samples per block used by the audio library when
 * 		   playing or recording. The buffer for reading the samples will be allocated when the play/record starts
 * 		   and freed at the end of operation.
 * 		   It basically decides the amount of data requested/handed from/to the application when the read/write
 * 		   callbacks are called.
  *
 * Note:           - When playing/recording non-raw formats (ADPCM, speex, etc) extra buffering is done internally by the
 * 		   audio library.
 * 		   - The default value is audio.def::AUDIO_SAMPLES_BLK. 
 **************************************************************************/
AUDIO_RES AudioSetProcSamples(int nSamples)
{

	audioProcSamples=((nSamples+127)/128)*128;	// minimize the DMA transfers ISR's
	return AUDIO_RES_OK; 
}


/************************************************************************
 * Function:       AudioGetProcSamples 
 *
 * PreCondition:   None 
 *
 * Input:          
 *
 * Output:         the number of processed samples
 *
 * Side Effects:   None
 *
 * Overview:       This function gets the number of the processed samples per block used by the audio library when
 * 		   playing or recording.
  *
 * Note:           None 
 **************************************************************************/
int AudioGetProcSamples(void)
{
	return audioProcSamples;
}
	



/************************************************************************
 * Function:       AudioSetPlayRate 
 *
 * PreCondition:   playRate>0 
 *
 * Input:          the play rate
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the sampling frequency versus the play frequency rate.
 * 		   If playRate is 1, the play frequency equals the audio sampling frequency.
 * 					Else the play frequency is playRate times slower than the audio sampling frequency.
 *
 * Note:           The default value for the play rate is set by AUDIO_PLAY_RATE in the audio.def file.
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES AudioSetPlayRate(int playRate)
{
	audioPlayRate=playRate;
	return AUDIO_RES_OK;
}
#endif

/************************************************************************
 * Function:       AudioGetPlayRate 
 *
 * PreCondition:   None 
 *
 * Input:          None
 *
 * Output:         the current play rate
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the sampling frequency versus the play frequency rate.
 * 		   If the rate is 1, the play frequency equals the audio sampling frequency.
 * 					Else the play frequency is the play rate times slower than the audio sampling frequency.
 *
 * Note:           None
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
int	AudioGetPlayRate(void)
{
	return audioPlayRate;
}
#endif


/************************************************************************
 * Function:       AudioSetRecordRate 
 *
 * PreCondition:   recordRate>0 
 *
 * Input:          the record rate
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the sampling frequency versus the record frequency rate.
 * 		   If recordRate is 1, the record frequency equals the audio sampling frequency.
 * 					Else the record frequency is recordRate times slower than the audio sampling frequency.
 *
 * Note:           The default value for the play rate is set by AUDIO_RECORD_RATE in the audio.def file.
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioSetRecordRate(int recordRate)
{
	audioRecordRate=recordRate;
	return AUDIO_RES_OK;
}
#endif

/************************************************************************
 * Function:       AudioGetRecordRate 
 *
 * PreCondition:   None 
 *
 * Input:          None
 *
 * Output:         the record rate
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the sampling frequency versus the record frequency rate.
 * 		   If the rate is 1, the record frequency equals the audio sampling frequency.
 * 					Else the record frequency is the record rate times slower than the audio sampling frequency.
 *
 * Note:           None
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
int	AudioGetRecordRate(void)
{
	return 	audioRecordRate;
}
#endif


/************************************************************************
 * Function:       AudioSetSamplingFreq 
 *
 * PreCondition:   sampleFreq>=AUDIO_MIN_WORK_FREQ 
 *
 * Input:          The sampling frequency, in Hz.
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the audio sampling rate.
 * 					After this adjustment the play and record rate have to 
 * 					be greater than AUDIO_MIN_WORK_FREQ, otherwise the call to AudioPlay/AudioRecord will fail.	
 *
 * Note:           The default value for the play rate is set by AUDIO_SAMPLE_FREQ in the audio.def file.
 **************************************************************************/
AUDIO_RES	AudioSetSamplingFreq(int sampleFreq)
{
	audioStrInfo.sampleRate=sampleFreq;
	return AUDIO_RES_OK;
}


/************************************************************************
 * Function:       AudioGetSamplingFreq 
 *
 * PreCondition:   None 
 *
 * Input:          None
 *
 * Output:         The sampling frequency, in Hz.
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the audio sampling rate.
 *
 * Note:           None
 **************************************************************************/
int AudioGetSamplingFreq(void)
{
	return audioStrInfo.sampleRate; 	
}


/************************************************************************
 * Function:       AudioSetMinFreq 
 *
 * PreCondition:   None 
 *
 * Input:          The min frequency, in Hz
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the minimum working frequency.
 *
 * Note:           None	
 **************************************************************************/
AUDIO_RES AudioSetMinFreq(int minFreq)
{
	audioMinFreq=minFreq;
	return AUDIO_RES_OK;
}

/************************************************************************
 * Function:       AudioGetMinFreq 
 *
 * PreCondition:   None 
 *
 * Input:          None
 *
 * Output:         The min frequency, in Hz
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the minimum working frequency.
 *
 * Note:           None	
 **************************************************************************/
int AudioGetMinFreq(void)
{
	return audioMinFreq;
}



/************************************************************************
 * Function:       AudioSetEncoding 
 *
 * PreCondition:   None
 *
 * Input:          the working audio format
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current audio format for play and record.
 *
 * Note:           None	
 **************************************************************************/
AUDIO_RES AudioSetEncoding(AUDIO_FORMAT format)
{
	int		bitsPerSample;
	AUDIO_RES	res=AUDIO_RES_OK;
	
	switch(format)
	{
	#ifdef PCM_ENCODING
		case AUDIO_PCM:
			bitsPerSample=16;
			break;
	#endif

	#ifdef ADPCM_ENCODING
		case AUDIO_ADPCM:
			bitsPerSample=4;
			break;
	#endif

	#ifdef SPEEX_ENCODING
		case AUDIO_SPEEX:
			bitsPerSample=0;	// not known yet
			break;
	#endif

		default:
			res=AUDIO_RES_UNSUPPORTED;
			break;
	}

	if(res==AUDIO_RES_OK)
	{
		audioStrInfo.aFmt=format;
		audioStrInfo.bitsPerSample=bitsPerSample;
	}
	
	return res;
}


/************************************************************************
 * Function:       AudioGetEncoding 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the working audio format
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the audio format for play and record.
 *
 * Note:           None	
 **************************************************************************/
AUDIO_FORMAT	AudioGetEncoding(void)
{		
	return audioStrInfo.aFmt;	
}


/************************************************************************
 * Function:       AudioSetInStreamFormat 
 *
 * PreCondition:   sFmt a proper AUDIO_STREAM_FORMAT value
 *
 * Input:          the stream structure format
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the current stream format to be found in the incoming stream.
 *
 * Note:           None	
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES AudioSetInStreamFormat(AUDIO_STREAM_FORMAT sFmt)
{
	audioStreamInFmt=sFmt;
	return AUDIO_RES_OK;
}
#endif

/************************************************************************
 * Function:       AudioGetInStreamFormat 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the stream structure format
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the input stream format.
 *
 * Note:           None	
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_STREAM_FORMAT	AudioGetInStreamFormat(void)
{
	return audioStreamInFmt;
}
#endif

/************************************************************************
 * Function:       AudioSetOutStreamFormat 
 *
 * PreCondition:   sFmt a proper AUDIO_STREAM_FORMAT value
 *
 * Input:          the stream structure format
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the current stream format to be recorded in the output stream.
 *
 * Note:           None	
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioSetOutStreamFormat(AUDIO_STREAM_FORMAT sFmt)
{
	audioStreamOutFmt=sFmt;
	return AUDIO_RES_OK;
}
#endif

/************************************************************************
 * Function:       AudioGetOutStreamFormat 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the stream structure format
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the stream format recorded in the output stream.
 *
 * Note:           None	
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_STREAM_FORMAT AudioGetOutStreamFormat(void)
{
	return audioStreamOutFmt;
}
#endif

#ifdef ADPCM_ENCODING
/************************************************************************
 * Function:       AudioSetAdpcmEndianess 
 *
 * PreCondition:   None
 *
 * Input:          the samples endianess
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current setting for the ADPCM nibbles endianess.
 *
 * Note:           The default value is set by audio.def::AUDIO_ADPCM_NIBBLES_LE.
 *                 The ADPCM nibbles endianess is bot consistent across file formats.
 *                 Some store nibbles in BE order, some in LE order.	
 **************************************************************************/
AUDIO_RES AudioSetAdpcmEndianess(int isLE)
{
	audioAdpcmLe=isLE?1:0;
	return AUDIO_RES_OK;
}


/************************************************************************
 * Function:       AudioGetAdpcmEndianess 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the ADPCM nibbles endianess
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the ADPCM nibbles endianess:
 * 			- 1 for LE
 * 			- 0 for BE.
 *
 * Note:           The default ADPCM nibbles endianess value is set by audio.def::AUDIO_ADPCM_NIBBLES_LE.	
 **************************************************************************/
int AudioGetAdpcmEndianess(void)
{
	return audioAdpcmLe;
}
#endif	// ADPCM_ENCODING



/************************************************************************
 * Function:       AudioSetActiveChannel 
 *
 * PreCondition:   None
 *
 * Input:          the active channel
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current active channel to be played for streams supporting multiple channels.
 *
 * Note:           The default value is 1.	
 **************************************************************************/
AUDIO_RES AudioSetActiveChannel(int chn)
{
	audioStrInfo.activeChannel=chn;
	return AUDIO_RES_OK;
}


/************************************************************************
 * Function:       AudioGetActiveChannel 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the current active channel
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the active channel.
 *
 * Note:           None	
 **************************************************************************/
int AudioGetActiveChannel(void)
{
	return audioStrInfo.activeChannel; 
}


/************************************************************************
 * Function:       AudioGetInStreamInfo 
 *
 * PreCondition:   None
 *
 * Input:          pInfo - pointer to address to store the stream information
 *
 * Output:         AUDIO_RES_OK for success, an error code otherwise
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the information for the currently opened istream.
 *                 If no input stream is currently opened, an error is returned.
 *
 * Note:           None
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES AudioGetInStreamInfo(sStreamInfo* pInfo)
{
	if(audioInHandle)
	{
		const sStreamInfo* pStrInfo=AudioStreamGetInfo(audioInHandle);
		if(pStrInfo)
		{
			*pInfo=*pStrInfo;
			return AUDIO_RES_OK;
		}
	}
	
	return AUDIO_RES_BAD_CALL;	// no stream currently opened
}
#endif

/************************************************************************
 * Function:       AudioGetOutStreamInfo 
 *
 * PreCondition:   None
 *
 * Input:          pInfo - pointer to address to store the stream information
 *
 * Output:         AUDIO_RES_OK for success, an error code otherwise
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the information for the currently opened ostream.
 *                 If no output stream is currently opened, an error is returned.
 *
 * Note:           None
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioGetOutStreamInfo(sStreamInfo* pInfo)
{
	if(audioOutHandle)
	{
		const sStreamInfo* pStrInfo=AudioStreamGetInfo(audioOutHandle);
		if(pStrInfo)
		{
			*pInfo=*pStrInfo;
			return AUDIO_RES_OK;
		}
	}
	
	return AUDIO_RES_BAD_CALL;	// no stream currently opened
}
#endif

/************************************************************************
 * Function:       AudioGetInStream 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the current istream
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the currently opened istream.
 *
 * Note:           None
 **************************************************************************/
audioSHandle AudioGetInStream(void)
{
	return audioInHandle;
}


/************************************************************************
 * Function:       AudioGetOutStream 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the current ostream
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the currently opened ostream.
 *
 * Note:           None
 **************************************************************************/
audioSHandle AudioGetOutStream(void)
{
	return audioOutHandle;
}



/************************************************************************
 * Function:       AudioSetDmaEnable 
 *
 * PreCondition:   None
 *
 * Input:          boolean to turn the DMA use on/off
 *
 * Output:         AUDIO_RES_OK for success, an error code otherwise
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current setting for the DMA use by the audio library.
 *
 * Note:           The default value is set by audio.def::AUDIO_DMA_ENABLED.	
 **************************************************************************/
AUDIO_RES AudioSetDmaEnable(int enableDma)
{
	AUDIO_RES res;
	if(audioCurrStat==STOP)
	{
		audioDmaEnabled=enableDma;
		res=AUDIO_RES_OK;
	}
	else
	{
		res=AUDIO_RES_BAD_CALL;
	}
	
	return res;
}

/************************************************************************
 * Function:       AudioGetDmaEnable 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         current setting for the DMA use by the audio library
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current setting for the DMA use by the audio library.
 *
 * Note:           The default value is set by audio.def::AUDIO_DMA_ENABLED.	
 **************************************************************************/
int AudioGetDmaEnable(void)
{
	return audioDmaEnabled!=0; 
}

/************************************************************************
 * Function:       AudioSetDmaChannel 
 *
 * PreCondition:    valid DMA channel, priority, ipl
 *
 * Input:          chn       - the DMA channel to use
 *                 chnPri    - the priority of that channel
 *                 dmaIpl    - the IPL for the DMA channel
 *
 * Output:         AUDIO_RES_OK for success, an error code otherwise
 *
 * Side Effects:   None
 *
 * Overview:       This function enables the DMA use and sets the current DMA channel, DMA priority and ipl to be used by the audio library.
 *
 * Note:           - The default value is set by audio.def::AUDIO_DMA_CHN, audio:def::AUDIO_DMA_CHN_PRI.
 *                 - The application needs to call AudioSetDmaChannel() at run time to match the settings used in the SETUP_AUDIO_SAMPLE_INT.
 **************************************************************************/
AUDIO_RES AudioSetDmaChannel(int chn, int chnPri, int dmaIpl)
{
	AUDIO_RES	res;

	if(audioCurrStat==STOP)
	{
		audioDmaEnabled=1;
		audioDmaChn=chn;
		audioDmaPri=chnPri;
		audioDmaIpl=dmaIpl;
		res=AUDIO_RES_OK;
	}
	else
	{
		res=AUDIO_RES_BAD_CALL;
	}
	
	return res;
}


/************************************************************************
 * Function:       AudioGetDmaChannel 
 *
 * PreCondition:   None
 *
 * Input:          pChn       - address to store the DMA channel used. Can be NULL.
 *                 pChnPri    - address to store the priority of the channel. Can be NULL.
 *                 pDmaIpl    - address to store the ipl used for the DMA. Can be NULL.
 *
 * Output:         true if the audio library has the DMA enabled, false otherwise
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current use of the DMA, the DMA channel, priority and ipl used by the audio library.
 *
 * Note:           None	
 **************************************************************************/
int AudioGetDmaChannel(int* pChn, int* pChnPri, int* pDmaIpl)
{
	
	if(pChn)
	{
		*pChn=audioDmaChn;
	}	
	if(pChnPri)
	{
		*pChnPri=audioDmaPri;
	}
	if(pDmaIpl)
	{
		*pDmaIpl=audioDmaIpl;
	}
	return audioDmaEnabled;
}


/****************************************************************************
 * Function:        AudioSetADPctEvent
 *
 * PreCondition:    None
 *
 * Input:           event notification callback
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the notification event for recording.
 * 			Whenever a punctual recording occurs, the library will execute this callback, if provided.
 * 			Relevant just for punctual mode recordings.
 * 			The application is notified that a punctual recording just took place and the record ADC
 * 			channel have been altered.
 *
 * Note:            - The handler should be as short as possible ((like signalling an event flag, smaphore, mux, etc) and NON Blocking!
 * 			Called from within the ISR!
 *                  - The ipl of the audio ISR (AUDIO_IPL in audio.def) is usually selected high enough,
 *                  so that the audio output is not distorted by other interrupts active within the system. 
 *****************************************************************************/
void AudioSetADPctEvent(pSetEvent eventCB)
{
	audioPctCB=eventCB;
}



/****************************************************************************
 * Function:        AudioGetADPctEvent
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          event notification callback 
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the notification event that's set for recording.
 *
 * Note:            None 
 *****************************************************************************/
pSetEvent AudioGetADPctEvent(void)
{
	return audioPctCB;
}



/************************************************************************
 * Function:       AudioGetOverrun 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the overrun count
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current number of overrun counts that occurred at the last play/record operation.
 *                 Overrun occurs:
 *                 - when playing: there is no available sample to be played (the read samples callback was slow).
 *                 The previous sample will be played.
 *                 - when recording: there's no buffer available to store the recorded sample (the write samples callback was slow).
 *                 The recorded sample will be discarded.
 *
 * Note:           The overrun count is cleared at the beginning of any play/record operation.	
 **************************************************************************/
int AudioGetOverrun(void)
{
	return isrDcpt.overrun;
}


/****************************************************************************
 * Function:        AudioSetSamplingDevice
 *
 * PreCondition:    valid sampling device and ipl.
 *
 * Input:           sDev:           the device to be used by the audio library to generate the sampling rate.
 *                  samplingIpl:    the IPL to be used when programming the sampling device
 *
 * Output:          AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the device to be used by the audio library when generating the sampling rate.
 *
 * Note:            For the time being, the application cannot change dinamically the sampling device
 *                  because the ISR is placed in ROM and the ISR's ipl is compile time defined.
 *                  - The application needs to call AudioSetSamplingDevice() at run time to match the settings used in the SETUP_AUDIO_SAMPLE_INT.
 *                  - The samplingIpl is relevant when the Audio Library won't use DMA. Otherwise, the dmaIpl in the AudioSetDmaChannel
 *                  is used.  
 *****************************************************************************/
AUDIO_RES AudioSetSamplingDevice(AUDIO_SDEV sDev, int samplingIpl)
{
	AUDIO_RES	res;

	if(audioCurrStat==STOP)
	{
		audioSamplingDev=sDev;
		audioSamplingIpl=samplingIpl;
		res=AUDIO_RES_OK;
	}
	else
	{
		res=AUDIO_RES_BAD_CALL;
	}
	return res;
}



/****************************************************************************
 * Function:        AudioGetSamplingDevice
 *
 * PreCondition:    None
 *
 * Input:           pSamplingIpl: pointer to store the current sampling Ipl used. Can be NULL.
 *
 * Output:          the device currently used by the audio library to generate the sampling rate. 
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the device currently used by the audio library to generate the sampling rate and the IPL used.
 *
 * Note:            None 
 *****************************************************************************/
AUDIO_SDEV AudioGetSamplingDevice(int* pSamplingIpl)
{
	if(pSamplingIpl)
	{
		*pSamplingIpl=audioSamplingIpl;
	}

	return audioSamplingDev;
}


/******************************************************************************
 * Function:        AudioConvert
 *
 * PreCondition:    the callbacks needed by the encoding/decoding operation have to be properly set prior to this operation
 *
 * Input:           pPrgDcpt: progress descriptor
 *
 * Output:          AUDIO_RES_OK if operation succeeded,
 * 		    an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function converts an input stream to raw samples and then encodes the resulted data to an output data stream.
 *                  If the encoding in the input stream cannot be automatially detected (for example the stream is AUDIO_STREAM_FREE)
 *                  the user can specify this using AudioSetInEncoding() function.
 *                  The output encoding format is the one set with AudioSetEncoding().
 *
 * Note:            pPrgDcpt could be null if no progress indication needed
 * 	            This function is not implemented yet. 
 *****************************************************************************/
#if defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)
AUDIO_RES AudioConvert(progressDcpt* pPrgDcpt)
{
	AUDIO_RES 		res;
	int			encSamples, decSamples;
	int			commSamples;
	const AudioEncDcpt*	pEDcpt;
	const AudioDecDcpt*	pDDcpt;
	sStreamInfo		decInfo;
	sStreamInfo		encInfo;
	
	if(audioCurrStat!=STOP)
	{
		return AUDIO_RES_BAD_CALL;	//  error
	}

	audioEncHandle=audioDecHandle=0;

	while(1)
	{
		decInfo=encInfo=audioStrInfo;	// update what caller asks for
		decInfo.aOp=PLAY;		// make sure it's play
		decInfo.sFmt=audioStreamInFmt;	// update the input stream format
		
		if((res=AudioStreamOpen(&decInfo, &audioInHandle))!=AUDIO_RES_OK)
		{
			break;
		}

		pDDcpt=audioDecDcpt+decInfo.aFmt;	
	
		res=(*pDDcpt->decoderCheckFormat)(&decInfo);
		if(res!=AUDIO_RES_OK)
		{	// unsupported?
			break;
		}

		decSamples=(*pDDcpt->decoderOutSamples)(audioProcSamples, &decInfo);

		encInfo.aOp=RECORD;		// make sure it's record
		encInfo.sFmt=audioStreamOutFmt;	// update the output stream format
		pEDcpt=audioEncDcpt+encInfo.aFmt;	
		
		res=(*pEDcpt->encoderCheckFormat)(&encInfo);
		if(res!=AUDIO_RES_OK)
		{	// unsupported?
			break;
		}

		encSamples=(*pEDcpt->encoderInSamples)(audioProcSamples, &encInfo);

		commSamples=encSamples>decSamples?encSamples:decSamples;	// choose the maxnumber of samples
	
		res=(*pDDcpt->decoderInit)(commSamples, &decInfo, CODEC_ALLOCATE_INOUT, &audioDecHandle);
		if(res!=AUDIO_RES_OK)
		{	// failed to init the decoder
			break;
		}

		
		res=(*pEDcpt->encoderInit)(commSamples, &encInfo, CODEC_ALLOCATE_OUT, &audioEncHandle);
		if(res!=AUDIO_RES_OK)
		{	// failed to init the encoder
			break;
		}		
	
		// finally, open the output stream	
		if((res=AudioStreamOpen(&encInfo, &audioOutHandle))!=AUDIO_RES_OK)
		{
			break;
		}

		// set the decoder output buffer as the encoder input buff
		(*pEDcpt->encoderSetInBuff)(audioEncHandle, (*pDDcpt->decoderGetOutBuff)(audioDecHandle));
		
		res=(*pDDcpt->decoderRProcData)(audioDecHandle, 0);	// TODO: add the progress dcpt here!

		res=(*pEDcpt->encoderProcWData)(audioEncHandle, (*pDDcpt->decoderGetOutBytes)(audioDecHandle, 1), 0);
		
		break;
	}


	/*else
		{
			AudioStreamCleanup(audioOutHandle);
			audioOutHandle=0;
			if(audioEncHandle)
			{
				(*pEDcpt->encoderCleanup)(audioEncHandle);
				audioEncHandle=0;
			}
		}
		* {	// fail
			AudioStreamCleanup(audioInHandle);
			audioInHandle=0;
			if(audioDecHandle)
			{
				const AudioDecDcpt*	pDDcpt=audioDecDcpt+pInfo->aFmt;	
				(*pDDcpt->decoderCleanup)(audioDecHandle);
				audioDecHandle=0;
			}
		}}*/
	
		
	return res;
}
#endif // AUDIO_SUPPORT_PLAY && AUDIO_SUPPORT_RECORD


#ifdef SPEEX_ENCODING
// codec routines
/******************************************************************************
 * Function:        AudioSetEncoderQuality
 *
 * PreCondition:    valid quality supported by encoder
 *
 * Input:           the desired encoder quality
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the encoder quality. It overrides any previous encoder bit rate setting. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioSetEncoderQuality(int qFactor)
{
	if(audioStrInfo.aFmt==AUDIO_SPEEX)
	{
		AudioSpxSetQuality(qFactor);
		return AUDIO_RES_OK;
	}
	return AUDIO_RES_UNSUPPORTED;
}
#endif

/******************************************************************************
 * Function:        AudioSetEncoderBitrate
 *
 * PreCondition:    valid bitrate supported by encoder
 *
 * Input:           the desired bitrate, bps
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the encoder bit rate and, therefore, quality. It overrides any previous encoder quality setting. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioSetEncoderBitrate(int bRate)
{
	if(audioStrInfo.aFmt==AUDIO_SPEEX)
	{
		AudioSpxSetBitRate(bRate);
		return AUDIO_RES_OK;
	}
	return AUDIO_RES_UNSUPPORTED;
}
#endif


/******************************************************************************
 * Function:        AudioSetEncoderComplexity
 *
 * PreCondition:    valid complexity supported by encoder
 *
 * Input:           the desired encoder quality
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the encoder quality. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioSetEncoderComplexity(int cFactor)
{
	if(audioStrInfo.aFmt==AUDIO_SPEEX)
	{
		AudioSpxSetComplexity(cFactor);
		return AUDIO_RES_OK;
	}
	return AUDIO_RES_UNSUPPORTED;
}
#endif

/******************************************************************************
 * Function:        AudioSetEncoderFrames
 *
 * PreCondition:    None
 *
 * Input:           the desired number of frames
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the encoder number of frames per packet. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioSetEncoderFrames(int nFrames)
{
	if(audioStrInfo.aFmt==AUDIO_SPEEX)
	{
		AudioSpxSetFramesNo(nFrames);
		return AUDIO_RES_OK;
	}
	return AUDIO_RES_UNSUPPORTED;
}
#endif

/******************************************************************************
 * Function:        AudioSetEncoderPackets
 *
 * PreCondition:    None
 *
 * Input:           the desired number of packets
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the encoder number of packets per Ogg page. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES AudioSetEncoderPackets(int nPackets)
{
	if(audioStrInfo.aFmt==AUDIO_SPEEX)
	{
		AudioSpxSetPacketsNo(nPackets);
		return AUDIO_RES_OK;
	}
	return AUDIO_RES_UNSUPPORTED;
}
#endif

/******************************************************************************
 * Function:        AudioSetDecoderEnhancement
 *
 * PreCondition:    None
 *
 * Input:           boolean to turn the enhancement on/off
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function turn on/off decoder perceptual enhancement. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES AudioSetDecoderEnhancement(int eVal)
{
	if(audioStrInfo.aFmt==AUDIO_SPEEX)
	{
		AudioSpxSetPrcptEnh(eVal);
		return AUDIO_RES_OK;
	}
	return AUDIO_RES_UNSUPPORTED;
}
#endif

/******************************************************************************
 * Function:        AudioSetCodecBand
 *
 * PreCondition:    valid band supported by the codec
 *
 * Input:           
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the desired codec band (narrow, wide, etc). 
 *
 * Note:            None 
 *****************************************************************************/
AUDIO_RES AudioSetCodecBand(AUDIO_BAND band)
{
	int	res=0;
	
	if(audioStrInfo.aFmt==AUDIO_SPEEX)
	{
		int wBand=band==AUDIO_BAND_NARROW?0:1;
		if(band==AUDIO_BAND_NARROW || band==AUDIO_BAND_WIDE)
		{
			res=AudioSpxSetBandMode(wBand);
		}
	}
	
	return res?AUDIO_RES_OK:AUDIO_RES_UNSUPPORTED;
}


#endif	// SPEEX_ENCODING

//////////////////// low level working routines //////////////////


/************************************************************************
 * Function:       AudioStopProcessing 
 *
 * PreCondition:   None 
 *
 * Input:          abort - stop gracefully or abort recording 
 *
 * Output:         AUDIO_RES_OK if ok, an error code otherwise.  
 *
 * Side Effects:   None
 *
 * Overview:       calling this function will stop the current audio operation, record/play.
 * 		   It can also abort the current record operation.
 *
 * Note:           None 
 *****************************************************************************/
static AUDIO_RES AudioStopProcessing(int abort)
{
	AUDIO_RES res=AUDIO_RES_OK;

	AudioStopSmplDevice();
#ifdef AUDIO_SUPPORT_PLAY
	
	if(audioCurrStat==PLAY || audioCurrStat==PAUSE_PLAY)
	{
		const AudioDecDcpt* pDDcpt=audioDecDcpt+audioCurrInfo.aFmt;

		AudioIOClosePlay();
		AudioStreamClose(audioInHandle);
		audioInHandle=0;
		// free decoder internal buffers
		(*pDDcpt->decoderSetOutBuff)(audioDecHandle, 0);
		(*pDDcpt->decoderCleanup)(audioDecHandle);
		audioDecHandle=0;


	}
#endif

#ifdef AUDIO_SUPPORT_RECORD

	if(audioCurrStat==RECORD || audioCurrStat==PAUSE_RECORD)
	{
		const AudioEncDcpt* pEDcpt=audioEncDcpt+audioCurrInfo.aFmt;
		AudioIOCloseRecord();
		if(!abort && pEDcpt->encoderStop)
		{	// encoder needs to flush out data
			res=(*pEDcpt->encoderStop)(audioEncHandle);
		}
		AudioStreamClose(audioOutHandle); 
		audioOutHandle=0;
		// free encoder internal buffers
		(*pEDcpt->encoderSetInBuff)(audioEncHandle, 0);
		(*pEDcpt->encoderCleanup)(audioEncHandle);
		audioEncHandle=0;

	}
#endif

	// free our own buffers
	free(sampleBuff[0]);
	free(sampleBuff[1]);
	sampleBuff[0]=sampleBuff[1]=0;
	
	audioCurrStat = STOP;

	return res;
}

/******************************************************************************
 * Function:        AudioInitDecoder
 *
 * PreCondition:    None
 *
 * Input:           the current stream info 
 *
 * Output:          AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function initializes the codec responsible for the decoding operation.
 *                  It also allocates the necessary memory resources needed for playing. 
 *
 * Note:            Resources should be freed when AudioStop()/AudioStopProcessing() is called. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES AudioInitDecoder(sStreamInfo* pInfo, int nSamples, int bPlayBuff, audioCodecHandle* pDecHndl)
{
	AUDIO_RES		res;
	audioCodecHandle	hndl;
	
	const AudioDecDcpt* pDDcpt=audioDecDcpt+pInfo->aFmt;
	*pDecHndl=0;
	
	res=(*pDDcpt->decoderInit)(nSamples, pInfo, CODEC_ALLOCATE_INOUT, &hndl);

	if(res==AUDIO_RES_OK && bPlayBuff)
	{
		sampleBuff[1]=malloc((*pDDcpt->decoderGetOutSize)(hndl));	// extra buffer for our double buffered play
		if(!sampleBuff[1])
		{
			res=AUDIO_RES_OUT_OF_MEM;
		}
		else
		{
			sampleBuff[0]=(short*)(*pDDcpt->decoderGetOutBuff)(hndl);
		}
	}

	if(res==AUDIO_RES_OK)
	{
		*pDecHndl=hndl;
	}
	else
	{
		(*pDDcpt->decoderCleanup)(hndl);
	}
	
	return res;
}
#endif

/******************************************************************************
 * Function:        AudioInitEncoder
 *
 * PreCondition:    None
 *
 * Input:           the current stream info, storage for the encoder handle 
 *
 * Output:          AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function initializes the codec responsible for the encoding operation.
 *                  It also allocates the necessary memory resources needed for recording. 
 *
 * Note:            Resources should be freed when AudioStop()/AudioStopProcessing() is called. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES AudioInitEncoder(sStreamInfo* pInfo, int nSamples, int bRecBuff, audioCodecHandle* pEncHndl)
{
	AUDIO_RES		res;
	audioCodecHandle	hndl;
	
	const AudioEncDcpt* pEDcpt=audioEncDcpt+pInfo->aFmt;

	*pEncHndl=0;

	res=(*pEDcpt->encoderInit)(nSamples, pInfo, CODEC_ALLOCATE_INOUT, &hndl);
	
	if(res==AUDIO_RES_OK && bRecBuff)
	{
		sampleBuff[1]=malloc((*pEDcpt->encoderGetInSize)(hndl));	// extra buffer for our double buffered record
		if(!sampleBuff[1])
		{
			res=AUDIO_RES_OUT_OF_MEM;
		}
		else
		{
			sampleBuff[0]=(short*)(*pEDcpt->encoderGetInBuff)(hndl);
		}
	}

	if(res==AUDIO_RES_OK)
	{
		*pEncHndl=hndl;
	}
	else
	{
		(*pEDcpt->encoderCleanup)(hndl);		
	}
	
	return res;
}
#endif	// AUDIO_SUPPORT_RECORD


/******************************************************************************
 * Function:        AudioCheckPlayFormat
 *
 * PreCondition:    None
 *
 * Input:           stream specifier
 *
 * Output:          AUDIO_RES_OK if the desired format and all is supported for the play operation,
 * 		    AUDIO_RES_UNSUPPORTED otherwise
 *   
 *
 * Side Effects:    None.
 *
 * Overview:        This function checks if the provided stream is suitable for playing. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES AudioCheckPlayFormat(sStreamInfo* pInfo)
{
	AUDIO_FORMAT audioFmt=pInfo->aFmt;

	if(audioFmt>=0 && audioFmt<sizeof(audioDecDcpt)/sizeof(*audioDecDcpt))
	{
		if(audioDecDcpt[audioFmt].decodeWhilePlay)
		{
			return (*audioDecDcpt[audioFmt].decoderCheckFormat)(pInfo);
		}
	}

	return AUDIO_RES_UNSUPPORTED;
}
#endif

/******************************************************************************
 * Function:        AudioCheckRecordFormat
 *
 * PreCondition:    None
 *
 * Input:           stream specifier
 *
 * Output:          AUDIO_RES_OK if the desired format and all is supported for the record operation,
 * 		    AUDIO_RES_UNSUPPORTED otherwise
 *   
 *
 * Side Effects:    None.
 *
 * Overview:        This function checks if the provided stream is suitable for recording. 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES AudioCheckRecordFormat(sStreamInfo* pInfo)
{
	AUDIO_FORMAT audioFmt=pInfo->aFmt;

	if(audioFmt>=0 && audioFmt<sizeof(audioEncDcpt)/sizeof(*audioEncDcpt))
	{
		if(audioEncDcpt[audioFmt].encodeWhileRecord)
		{
			return (*audioEncDcpt[audioFmt].encoderCheckFormat)(pInfo);
		}
	}

	return AUDIO_RES_UNSUPPORTED;
}
#endif



/****************************************************************************
 * Function:        AudioInitPlay
 *
 * PreCondition:    None
 *
 * Input:           stream specifier
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the audio output chain based on the current stream settings:
 * 		    - initializes the timer used to give the audio rate (AUDIO_SDEV_TMR2/AUDIO_SDEV_TMR3)
 * 		    by using the current sampleRate
 * 		    - calls the application supplied AudioIOInitPlay 
 *
 * Note:            This function is called by the library before starting the play operation. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES AudioInitPlay(sStreamInfo* pInfo)
{
	int		pbFreq;
	int		tmrFreq;	
	AUDIO_RES	res;
	int		retries=AUDIO_LOW_RES_RETRIES;
	
	if(pInfo->sampleRate/audioPlayRate<audioMinFreq)
	{
		return AUDIO_RES_MIN_FREQ;	
	}
	
	pbFreq=audioSysFreq/(1<<mOSCGetPBDIV());	// get the PB frequency

	audioTmrDiv=0;
	tmrFreq=pbFreq;

	while(retries--)
	{
		res=AudioIOInitPlay(audioSysFreq, tmrFreq, pInfo->sampleRate, AUDIO_BITS_PER_SAMPLE, audioPlayRate, audioDmaEnabled);
		if(res!=AUDIO_RES_HIGH_RESOLUTION)
		{
			break;
		}
		audioTmrDiv++;
		tmrFreq>>=1;		// retry with lower frequency		
	}

	if(res==AUDIO_RES_OK)
	{
		audioTmrPeriod=tmrFreq/pInfo->sampleRate;
		AudioIODisableOutAmp();		// just in case

		if(audioDmaEnabled)
		{
			AudioInitDma();
		}
	}


	
	return res;	
}
#endif


/****************************************************************************
 * Function:        AudioInitRecord
 *
 * PreCondition:    None
 *
 * Input:           stream specifier
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the audio input chain based on the defines in the audio.def file:
 * 		    - initializes the timer used to give the audio rate (AUDIO_SDEV_TMR2/AUDIO_SDEV_TMR3)
 * 		    by using the current audioSampleFreq,
 * 		    - calls the application supplied AudioIOInitRecord 
 *
 * Note:            This function is called by the library before starting the record operation. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES AudioInitRecord(sStreamInfo* pInfo)
{
	int		pbFreq;
	int		tmrFreq;	
	AUDIO_RES	res;
	int		retries=AUDIO_LOW_RES_RETRIES;
	
	if(pInfo->sampleRate/audioRecordRate<audioMinFreq)
	{
		return AUDIO_RES_MIN_FREQ;	
	}

	pbFreq=audioSysFreq/(1<<mOSCGetPBDIV());	// get the PB frequency

	audioTmrDiv=0;
	tmrFreq=pbFreq;

	while(retries--)
	{
		res=AudioIOInitRecord(audioSysFreq, tmrFreq, pInfo->sampleRate, AUDIO_BITS_PER_SAMPLE, audioRecordRate, audioDmaEnabled);
		if(res!=AUDIO_RES_HIGH_RESOLUTION)
		{
			break;
		}
		audioTmrDiv++;
		tmrFreq>>=1;		// retry with lower frequency		
	}

	if(res==AUDIO_RES_OK)
	{
		audioTmrPeriod=tmrFreq/pInfo->sampleRate;
		if(audioDmaEnabled)
		{
			AudioInitDma();
		}
	}

	return res;
}
#endif	// AUDIO_SUPPORT_RECORD



/******************************************************************************
 * Function:        AudioLibPlay
 *
 * PreCondition:    AudioInit has to be called.
 *
 * Input:           None
 *
 * Output:          return AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function kick start the audio play.
 *                  It will process audioProcSamples samples at a time (if needed).
 *
 * Note:            This has to be called from the user frequently enough.
 * 		    The rate of call is given by audioProcSamples. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
static AUDIO_RES AudioLibPlay(void)
{
	int		dcptIx;
	AUDIO_RES	res=AUDIO_RES_OK;
		
	// see if we have some buffer available
	dcptIx=isrDcpt.dcptIx^1;	// look to the buffer not used in the ISR
	if(isrDcpt.bDcpt[dcptIx].isrProcDone)
	{	// ok, we have a buffer available
		int bRead, nSamples;
		const AudioDecDcpt* pDDcpt=audioDecDcpt+audioCurrInfo.aFmt;

		(*pDDcpt->decoderSetOutBuff)(audioDecHandle, sampleBuff[dcptIx]);
		res=(*pDDcpt->decoderRProcData)(audioDecHandle, 0);
		if(res==AUDIO_RES_OK || res==AUDIO_RES_DONE)
		{
			bRead=(*pDDcpt->decoderGetOutBytes)(audioDecHandle, 1);
			nSamples=bRead/2;

			if(nSamples)
			{	// got some new data
				if(AudioIOPlayProcessNeeded(audioDmaEnabled))
				{	// have samples ready for the output
					AudioIOProcessPlayBuffer(sampleBuff[dcptIx], nSamples);
				}
				isrDcpt.bDcpt[dcptIx].pBuff=sampleBuff[dcptIx];
				isrDcpt.bDcpt[dcptIx].pEnd=sampleBuff[dcptIx]+nSamples;
				isrDcpt.bDcpt[dcptIx].isrProcDone=0;	// show it's ready for the ISR
				if(audioCurrStat==STOP)
				{	// ok, it's the 1st time we're running it
					AudioStartIsrProcess((audioCurrStat=PLAY), isrDcpt.bDcpt+dcptIx); 	// Now enable the ISR processing
				}
			}

			if(nSamples==0 || res==AUDIO_RES_DONE)
			{	// ok, that's it, no more.
				if(audioCurrStat==PLAY)
				{	// allow the current buffer to end...
					isrDcpt.isrStop=1;
					res=AUDIO_RES_OK;	// we'll change it to done once we'he taken the stop
				}
				else
				{
					audioCurrStat=PLAY;	// have to know the status to stop...
					AudioStopProcessing(0);
				}
			}
		}
		else
		{	// some error
			AudioStopProcessing(1);
		}
	}

	return res;
}
#endif	// AUDIO_SUPPORT_PLAY


/******************************************************************************
 * Function:        AudioLibRecord
 *
 * PreCondition:    AudioInit has to be called.
 *
 * Input:           None
 *
 * Output:          return AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function kick start the audio play.
 *                  It will process audioProcSamples samples at a time (if needed).
 *
 * Note:            This has to be called from the user frequently enough.
 * 		    The rate of call is given by audioProcSamples. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
static AUDIO_RES AudioLibRecord(void)
{
	int			dcptIx;
	volatile ISRBuffDcpt*	pDcpt;
	AUDIO_RES		res=AUDIO_RES_OK;
		
	// see if we have some buffer with data
	dcptIx=isrDcpt.dcptIx^1;	// look to the buffer last used by the ISR
	pDcpt=isrDcpt.bDcpt+dcptIx;
	if(pDcpt->isrProcDone)
	{	// ok, we have a buffer full
		const AudioEncDcpt* pEDcpt=audioEncDcpt+audioCurrInfo.aFmt;

		if(AudioIORecordProcessNeeded(audioDmaEnabled))
		{	// have samples ready for the output
			AudioIOProcessRecordBuffer(sampleBuff[dcptIx], pDcpt->pEnd-sampleBuff[dcptIx]);
		}		
		
		(*pEDcpt->encoderSetInBuff)(audioEncHandle, sampleBuff[dcptIx]);
		res=(*pEDcpt->encoderProcWData)(audioEncHandle, (*pEDcpt->encoderGetInSize)(audioEncHandle), 0);
		if(res!=AUDIO_RES_OK)
		{	// some error must have occurred...
			AudioStopProcessing(1);	// abort in case of error
		}
		
		pDcpt->pBuff=sampleBuff[dcptIx];
		pDcpt->isrProcDone=0;	// show we're done and it's ready for the ISR
	}
	
	return res;
}
#endif	// AUDIO_SUPPORT_RECORD



/******************************************************************************
 * Function:        AudioStartIsrProcess
 *
 * PreCondition:    None
 *
 * Input:           boolean to enable ADC
 *                  pointer to buffer descriptor to be used if the DMA processing is enabled
 *
 * Output:          None 
 *
 * Side Effects:    None.
 *
 * Overview:        This function is just a helper to enable the timer ISR processing
 *
 * Note:            None 
 *****************************************************************************/
static void AudioStartIsrProcess(AUDIO_STATUS currStat, volatile ISRBuffDcpt* pDcpt)
{
#ifdef AUDIO_SUPPORT_RECORD
	if(currStat==RECORD)
	{
		AudioIOEnableInAmp();
		if(audioDmaEnabled)
		{
			audioDmaAdd=AudioIOGetRecordAddress();
			AudioStartDmaProcess(currStat, pDcpt);
		}
	}
#endif
	
#ifdef AUDIO_SUPPORT_PLAY
	
	if(currStat==PLAY)
	{
		AudioIOEnableOutAmp();

		if(audioDmaEnabled)
		{
			audioDmaAdd=AudioIOGetPlayAddress();
			AudioStartDmaProcess(currStat, pDcpt);
		}
	}
#endif

	AudioStartSmplDevice();
	
}



/****************************************************************************
 * Function:        AudioInitDma
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the DMA controller for an audio transfer
 *
 * Note:            This function is called by the library before starting the DMA playback/record operation. 
 *****************************************************************************/
static void AudioInitDma(void)
{

	DmaEvCtrlFlags	ctrlFlags;

	if(audioSamplingDev==AUDIO_SDEV_TMR2)
	{
		ctrlFlags=DMA_EV_START_IRQ(_TIMER_2_IRQ);
	}
	else
	{
		ctrlFlags=DMA_EV_START_IRQ(_TIMER_3_IRQ);
	}
	
		
	DmaChnOpen(audioDmaChn, audioDmaPri, DMA_OPEN_DEFAULT);
	DmaChnSetEventControl(audioDmaChn, ctrlFlags);

	DmaChnSetEvEnableFlags(audioDmaChn, DMA_EV_BLOCK_DONE|DMA_EV_ERR|DMA_EV_ABORT);	
	DmaChnSetIntPriority(audioDmaChn, audioDmaIpl, INT_SUB_PRIORITY_LEVEL_3);
	DmaChnIntEnable(audioDmaChn);
}


/******************************************************************************
 * Function:        AudioStartDmaProcess
 *
 * PreCondition:    None
 *
 * Input:           currStat: if record/playback operation
 *                  pDcpt:    pointer to buffer descriptor to be used if the DMA processing is enabled
 *
 * Output:          number of bytes the DMA was re-started with. 0 if no more bytes left to transfer
 *
 * Side Effects:    None.
 *
 * Overview:        This function is just a helper to program the DMA controller
 *
 * Note:            None 
 *****************************************************************************/
static int AudioStartDmaProcess(AUDIO_STATUS currStat, volatile ISRBuffDcpt* pDcpt)
{
	int	nBytes;
	
	if((nBytes=(char*)pDcpt->pEnd-(char*)pDcpt->pBuff))
	{
		if(nBytes>256)
		{	// DMA transfer limit!
			nBytes=256;
		}

		if(currStat==RECORD)
		{
			DmaChnSetTxfer(audioDmaChn, (void*)audioDmaAdd, pDcpt->pBuff, 2, nBytes, 2);
		}
		else
		{
			DmaChnSetTxfer(audioDmaChn, pDcpt->pBuff, (void*)audioDmaAdd, nBytes, 2, 2);
		}
		
		pDcpt->pBuff+=nBytes/2;		// store what's processed	
	}

	return nBytes;
}

/******************************************************************************
 * Function:        AudioStallDmaProcess
 *
 * PreCondition:    None
 *
 * Input:           currStat: if record/playback operation
 *                  pDcpt:    pointer to buffer descriptor to be used if the DMA processing is enabled
 *
 * Output:          None
 *
 * Side Effects:    None.
 *
 * Overview:        This function is just a helper to program the DMA controller
 *
 * Note:            None 
 *****************************************************************************/
static void AudioStallDmaProcess(AUDIO_STATUS currStat, volatile ISRBuffDcpt* pDcpt)
{
	if(currStat==RECORD)
	{	// just overwrite the last available record	
		DmaChnSetTxfer(audioDmaChn, (void*)audioDmaAdd, pDcpt->pBuff-1, 2, 2, 2);
	}
	else
	{	// just output the last available sample	
		DmaChnSetTxfer(audioDmaChn, pDcpt->pBuff-1, (void*)audioDmaAdd, 2, 2, 2);
	}
}



/************************************************************************
 * Interrupt handler:   __AudioSampleRoutine
 *
 * PreCondition:        The desired sample interrupt has to be configured and enabled by the user application
 *                      using the SETUP_AUDIO_SAMPLE_INT.
 *
 * Overview:            For each sample interrupt the ISR will call the AudioIO layer to perform the D/A or A/D conversions.
 *
 * Note:                This ISR is reached when the sampling interrupt occurs.
 *                      If the play/record is done using a DMA transfer, the ISR is reached when a DMA block is completed.
 *****************************************************************************/
void __AudioSampleRoutine(void)
{
	int	newIx;		// new buffer index
	int	toStop;
	int	toSignal;

	ProfileIntStart();
	
	toStop=0;
	
	toSignal=(isrDcpt.bDcpt[0].isrProcDone || isrDcpt.bDcpt[1].isrProcDone)!=0;
	
	if(!audioDmaEnabled)
	{	
		if(!--isrDcpt.isrCnt)
		{
			isrDcpt.isrCnt=isrDcpt.isrRate;		// reload

			while(isrDcpt.stat==ISR_READY)
			{	// don't have a buffer to work into
				if(isrDcpt.isrStop)
				{	// ok, that's it
					toStop=1;
					break;
				}
				
				newIx=isrDcpt.dcptIx^1;	// look into the other buffer to see if it's ready
				if(isrDcpt.bDcpt[newIx].isrProcDone)
				{
					isrDcpt.overrun++;		// no buffer available
				}
				else
				{
					isrDcpt.dcptIx=newIx;	// switch buffers
					isrDcpt.stat=ISR_RUN;	// we got the buffer, continue processing
				}
				break;
			}

			while(isrDcpt.stat==ISR_RUN)
			{	// ok, we're in the middle of a buffer
				volatile ISRBuffDcpt* pDcpt=isrDcpt.bDcpt+isrDcpt.dcptIx;
				if(audioCurrStat==PLAY)
				{
					DEBUG_SYNC_HIGH();	
				#ifdef AUDIO_SUPPORT_PLAY
					AudioIOPlaySample(*pDcpt->pBuff++);
				#endif
					DEBUG_SYNC_LOW();
				}
				else
				{	// RECORD
					DEBUG_SYNC_HIGH();
				#ifdef AUDIO_SUPPORT_RECORD
					*pDcpt->pBuff++=AudioIOGetRecordSample();
				#endif
					DEBUG_SYNC_LOW();	// done
				}
				
				if(pDcpt->pBuff==pDcpt->pEnd)
				{	// done with this buffer
					pDcpt->isrProcDone=1;
					// need another buffer?
					if(isrDcpt.isrStop)
					{	// ok, that's it
						toStop=1;
						break;
					}
					
					newIx=isrDcpt.dcptIx^1;	// look into the other buffer to see if it's ready
					if(isrDcpt.bDcpt[newIx].isrProcDone)
					{	// flag not cleared, buffer not ready
						isrDcpt.stat=ISR_READY;	// wait for buffer in the next ISR
					}
					else
					{
						isrDcpt.dcptIx=newIx;	// we're still ISR_RUN, switch buffers
					}
				}
				break;
			}
		}
	}
	else
	{	// DMA transfer
		volatile ISRBuffDcpt* pDcpt=isrDcpt.bDcpt+isrDcpt.dcptIx;

		while(isrDcpt.stat==ISR_READY)
		{	// don't have a buffer to work into
			if(isrDcpt.isrStop)
			{	// ok, that's it
				toStop=1;
				break;
			}
			
			newIx=isrDcpt.dcptIx^1;	// look into the other buffer to see if it's ready
			if(isrDcpt.bDcpt[newIx].isrProcDone)
			{
				isrDcpt.overrun++;		// no buffer available
				AudioStallDmaProcess(audioCurrStat, pDcpt);
				DmaChnEnable(audioDmaChn);
			}
			else
			{
				isrDcpt.dcptIx=newIx;		// switch buffers
				pDcpt=isrDcpt.bDcpt+newIx;	// switch descriptors				
				isrDcpt.stat=ISR_RUN;		// we got the buffer, continue processing
			}
			break;
		}

		while(isrDcpt.stat==ISR_RUN)
		{	// ok, we're in the middle of a buffer
			int nBytes;
			nBytes=AudioStartDmaProcess(audioCurrStat, pDcpt);
			
			if(!nBytes)
			{	// done with this buffer
				pDcpt->isrProcDone=1;
				// need another buffer?
				if(isrDcpt.isrStop)
				{	// ok, that's it
					toStop=1;
					break;
				}
				
				newIx=isrDcpt.dcptIx^1;	// look into the other buffer to see if it's ready
				if(isrDcpt.bDcpt[newIx].isrProcDone)
				{	// flag not cleared, buffer not ready
					AudioStallDmaProcess(audioCurrStat, pDcpt);
					isrDcpt.stat=ISR_READY;	// wait for buffer in the next ISR
				}
				else
				{
					isrDcpt.dcptIx=newIx;	// we're still ISR_RUN, switch buffers
					AudioStartDmaProcess(audioCurrStat, isrDcpt.bDcpt+newIx);
				}
			}
				
			DmaChnEnable(audioDmaChn);
			break;
		}
	}



	if(toStop)
	{
		AudioStopSmplDevice();
	}

	if(toSignal || toStop)
	{
		if(!audioEvFlag)
		{
			audioEvFlag=1;
			(*usrNotifyCB)();	// show you need attention
		}
	}

	
	AudioAckSmplDeviceInt(); // Clear the interrupt flag	

	ProfileIntUpdate();
}

#endif	// defined(AUDIO_SUPPORT_RECORD) || defined(AUDIO_SUPPORT_PLAY)

