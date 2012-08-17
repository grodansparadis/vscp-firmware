/*********************************************************************
 *
 *                  AUDIO Library - PIC32 definitions
 *
 *********************************************************************
 * FileName:        audio.h
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
 *$Id: $
 ********************************************************************/
#ifndef _AUDIO_H_

#define _AUDIO_H_

#include <audio.def>


typedef enum
{
	AUDIO_SDEV_TMR2,	// TMR2 used as a sampling device
	AUDIO_SDEV_TMR3,	// TMR3 used as a sampling device

	AUDIO_SDEV_NO		// total number of devices supported
}AUDIO_SDEV;	// device to be used by the audio library to generate the sampling rate

typedef enum
{
	STOP,			// the audio player is idle
	PLAY,			// the audio player is currently playing some data
	RECORD,			// the audio player is curently recording some data
	PAUSE_PLAY,		// the play operation is paused
	PAUSE_RECORD,		// the record operation is paused
}AUDIO_STATUS;		// The current status of the audio state machine


#define	AUDIO_OP	AUDIO_STATUS	// an audio operation: PLAY and RECORD used only

typedef enum 
{
#ifdef PCM_ENCODING
	AUDIO_PCM,		// unencoded 16bit PCM data
	AUDIO_RAW = AUDIO_PCM,	// backward  compatible name for PCM data
#endif
#ifdef ADPCM_ENCODING	
	AUDIO_ADPCM,		// IMA ADPCM format
#endif
#ifdef SPEEX_ENCODING
	AUDIO_SPEEX,		// SPEEX format
#endif

	AUDIO_FORMATS		// number of supported formats
}AUDIO_FORMAT;		// The formats supported by the audio library

#ifdef SPEEX_ENCODING
typedef enum
{
	AUDIO_BAND_NARROW,	// codec in narrow band mode
	AUDIO_BAND_WIDE,	// codec in wide band mode
				// ultra wide band not supprted yet 

}AUDIO_BAND;	// supported codec band
#endif


typedef enum
{
#ifdef FREE_STREAMS
	AUDIO_STREAM_FREE,	// free format, just samples, no particular structure. Always FIRST in this list, if defined!
#endif
#ifdef WAVE_STREAMS
	AUDIO_STREAM_WAV,	// MS-WAVE RIFF streams
#endif
#ifdef SPEEX_OGG_STREAMS
	AUDIO_STREAM_OGG_SPEEX,	// Speex in Ogg streams
#endif

	// add other supported file structures here
	// 
	AUDIO_STREAMS_NO,	// number of supported stream structures
	//
	// query should always be the last in this list!
	AUDIO_STREAM_QUERY=AUDIO_STREAMS_NO,			// unknown format but try to detect if supported
	
}AUDIO_STREAM_FORMAT;	// stream formats supported by the Audio Library

// some sanity checks

#if !defined(AUDIO_SUPPORT_PLAY) && !defined(AUDIO_SUPPORT_RECORD)
	#error " Either Play or Record operations have to be defined!"
#endif

#if !defined(PCM_ENCODING) && !defined(ADPCM_ENCODING) && !defined(SPEEX_ENCODING)
	#error " At least one encoding format is needed!"
#endif

#if defined(PCM_ENCODING) && !(defined(FREE_STREAMS) || defined(WAVE_STREAMS))
	#error " PCM encoding needs either free or wave streams!"
#endif

#if defined(ADPCM_ENCODING) && !(defined(FREE_STREAMS) || defined(WAVE_STREAMS))
	#error " ADPCM encoding needs either free or wave streams!"
#endif

#if defined(SPEEX_ENCODING) && !(defined(FREE_STREAMS) || defined(SPEEX_OGG_STREAMS))
	#error "Speex encoding needs either free or Speex Ogg streams!" 
#endif



typedef enum
{
	AUDIO_RES_OK,			// success
	AUDIO_RES_UNSUPPORTED,		// unsupported format, operation, etc.
	AUDIO_RES_BAD_CALL,		// calling PLAY while recording, etc.
	AUDIO_RES_OUT_OF_MEM,		// not enough memory (lower the number of processed samples?)
	AUDIO_RES_LOW_RESOLUTION,	// not enough resolution could be obtained. increase the system frequency, lower the audio sample frequency
	AUDIO_RES_MIN_FREQ,		// AUDIO_MIN_WORK_FREQ violated. Increase sampling frequency or lower the play/record rate.
	AUDIO_RES_WRITE_ERROR,		// some write operation could not be completed
	AUDIO_RES_READ_ERROR,		// some read operation could not be completed
	AUDIO_RES_DSTREAM_ERROR,	// data error in the input stream
	AUDIO_RES_SSTREAM_ERROR,	// structure error in the input stream
	AUDIO_RES_BUFF_ERROR,		// allocated buffer too small, increase.
	//      
	AUDIO_RES_HIGH_RESOLUTION,	// the work frequency is too high, lower it down
	AUDIO_RES_STREAM_CB_ERROR,	// stream callback needed but not supported
	AUDIO_RES_STREAM_EMPTY,		// missing data in the input stream
	AUDIO_RES_STREAM_END,		// end of stream
	AUDIO_RES_STREAM_ID_ERROR,	// different stream, etc
	AUDIO_RES_COMPRESS_ERROR,	// compression not supported error
	AUDIO_RES_DMA_UNSUPPORTED,	// DMA transfer not supported
	AUDIO_RES_BPS_UNSUPPORTED,	// unsupported bits per sample
	AUDIO_RES_DONE,			// operation completed
	
}AUDIO_RES;	// The result of an audio operation



typedef enum
{
	AUDIO_CB_READ,			// read data callback
	AUDIO_CB_WRITE,			// write data callback
	AUDIO_CB_SEEK_IN,		// input data seek callback
	AUDIO_CB_SEEK_OUT,		// output data seek callback
	AUDIO_CB_EVENT,			// event notification callback

}AUDIO_CB;	// audio library callbacks

typedef	int (*pReadCB)(unsigned char* samplesBuff, int bSize);		// Callback function for user to provide samples in a buffer to be played.
									// Returns the number of bytes actually put in the buffer
typedef	int (*pWriteCB)(unsigned char* samplesBuff, int nSamples);	// Callback function for user to store an encoded samples buffer.
									// Returns the number of bytes written

typedef enum
{
	ORIGIN_CURR	= 0,		// current position
	ORIGIN_BOF	= -1,		// beginning of file
	ORIGIN_EOF	= 1,		// end of file
}ORIGIN_SEEK;	// operations for moving around in the in/out streams


typedef int (*pSeekStreamCB)(long offset, ORIGIN_SEEK origin);		// callback for moving the read pointer in the i/o stream
									// returns true if succeeded, false otherwise



typedef void (*pSetEvent)(void);					// Function to set a notification that the player needs to be called
									// (AudioPlay if playing, AudioRecord if recording)
									// Could be a semaphore, event flag, etc.
									// Note: called from within the ISR. NON BLOCKING!


typedef struct __attribute__((packed))
{
	char			sFmt;		// AUDIO_STREAM_FORMAT: the stream format: AUDIO_STREAM_FREE, AUDIO_STREAM_WAV, etc	
	char			aFmt;		// AUDIO_FORMAT: the encoding data format in the stream. 
	char			aOp;		// AUDIO_OP: PLAY/RECORD type of operation
	char			bitsPerSample;	// 8/16/24/32. Always 16 in this implementation.
	//
	char			numChannels;	// 1 for mono, 2 stereo, etc
	char			activeChannel;	// 1, 2, etc playing mono, 0 for all of them. Just one channel supported in this implementation.
	char			lEndian;	// true if LE, false if BE
	char			reserved;	// padding
	//
	int			streamSz;	// how many total (i.e. for all channels) data bytes available in the stream
	int			sampleRate;	// play/record rate, Hz
	int			bitRate;	// compressed bps
}sStreamInfo;	// an audio stream info data


typedef void(*pProgressFnc)(int nBytes);			// progress display function
								// during an lengthy operation (AudioConvert)
								// the library will use this callback
								// so that caller can regain control

typedef struct
{
	int		progressStep;		// no of bytes to process before calling the progress callback
	int		progressCnt;		// current counter
	pProgressFnc	progressFnc;		// progress callback
}progressDcpt;	// encoder/decoder progress activity descriptor



/****************************************************************************
 * Function:        AudioInit
 *
 * PreCondition:    None
 *
 * Input:           sysFreq	- system running frequency
 *                  readCb	- read callback
 *                  writeCb	- write callback
 *                  eventCb	- notification callback
 *
 * Output:          always AUDIO_RES_OK 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the audio library based. 
 *
 * Note:            This has to be called by the user prior to any other audio functions.
 *****************************************************************************/
AUDIO_RES	AudioInit(int sysFreq, pReadCB readCb, pWriteCB writeCb, pSetEvent eventCb);


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
 * Overview:        Clean-up of the allocated resorces.
 *
 * Note:            This has to be called by the user when done with the audio library.
 *****************************************************************************/
AUDIO_RES	AudioClose(void);


/****************************************************************************
 * Function:        AudioSetCallback
 *
 * PreCondition:    None
 *
 * Input:           cbType: callback type
 *                  cbFnc: callback value
 *
 * Output:          AUDIO_RES_OK if ok, AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function sets a callback used for playing/recording and/or encoding/decoding.
 *
 * Note:            Intended for callbacks that cannot be set by AudioInit(), or for callbacks that have to be changed dynamically.
 * 		    This function has to be called before calling the audio functions that might need the callbacks.
 *****************************************************************************/
AUDIO_RES	AudioSetCallback(AUDIO_CB cbType, void* cbFnc);


/****************************************************************************
 * Function:        AudioGetCallback
 *
 * PreCondition:    None
 *
 * Input:           cbType	- callback type
 *
 * Output:          a callback value or 0 if not supported callback type 
 *
 * Side Effects:    None
 *
 * Overview:        This function gets the value of a callback used for playing/recording and/or encoding/decoding.
 *
 * Note:            None
 *****************************************************************************/
void*		AudioGetCallback(AUDIO_CB cbType);

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
 * Overview:        This function starts the audio play using the current selected audio format and stream structure.
 *                  When the player needs AudioPlay to be called again, the registered notification callback is called.
 *                  For actually filling the buffer with data, the registered read callback is called.
 *
 * Note:            - The user application has to call this function when the notification is set in play mode.
 *                  - The rate of call is indirectly set by the number of processed samples per buffer, AudioSetProcSamples(). 
 *                  - If the encoding in the input stream cannot be automatically detected (for example the stream is AUDIO_STREAM_FREE)
 *                    the user can specify the encoding using AudioSetEncoding() function.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES	AudioPlay(void);
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
 * Overview:        This function starts the audio recording using the current selected audio format and stream structure.
 *                  When the recorder needs the AUdioRecord to be called again, the registered notification callback is called.
 *                  To actually pass the data buffer to the application for storage, the registered write callback is called.
 *
 * Note:            The user application has to call this function when the notification is set in record mode.
 *                  The rate of call is indirectly set by the number of processed samples per buffer, AudioSetProcSamples().
 *                  The output encoding format is the one set with AudioSetEncoding().
 *                  The oustput stream format is set with AudioSetOutStreamFormat().
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES	AudioRecord(void);
#endif



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
AUDIO_STATUS AudioStatus(void);

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
 * Overview:       The function will suspend the current audio play/record operation.
 *                 The play/record can be resumed using AudioResume().
 *
 * Note:           None
 **************************************************************************/
AUDIO_RES	AudioPause(void);

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
 * Overview:       The function will stop/cancel the current audio operation, record/play. 
 *
 * Note:           None 
 *****************************************************************************/
AUDIO_RES AudioStop(void);

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
 * Overview:       This function will resume a previously paused play/record operation.
 *
 * Note:           None
 **************************************************************************/
AUDIO_RES	AudioResume(void);



/************************************************************************
 * Function:       AudioSetProcSamples 
 *
 * PreCondition:   nSamples>0 
 *
 * Input:          nSamples - the number of samples
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the number of the processed samples per buffer used by the audio library when
 * 		   playing or recording. Two sample buffers will be allocated when the play/record starts
 * 		   and freed at the end of operation.
 * 		   It basically decides the amount of data requested/handed from/to the application when the read/write
 * 		   callbacks are called.
 *
 * Note:           - When playing/recording encoded formats extra buffering can be done internally by the audio library.
 * 		   - The default value is audio.def::AUDIO_SAMPLES_BLK.
 * 		   - The setting takes effect once a new play/record operation is started. 
 **************************************************************************/
AUDIO_RES	AudioSetProcSamples(int nSamples);



/************************************************************************
 * Function:       AudioGetProcSamples 
 *
 * PreCondition:   None 
 *
 * Input:          None
 *
 * Output:         the number of processed samples
 *
 * Side Effects:   None
 *
 * Overview:       This function gets the number of the processed samples per buffer used by the audio library when
 * 		   playing or recording.
  *
 * Note:           None 
 **************************************************************************/
int		AudioGetProcSamples(void);


/************************************************************************
 * Function:       AudioSetPlayRate 
 *
 * PreCondition:   playRate>0 
 *
 * Input:          playRate	- the play rate
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the sampling frequency per the play frequency rate.
 * 		   If playRate is 1, the play frequency equals the audio sampling frequency.
 * 		   Else the play frequency is play rate times slower than the audio sampling frequency.
 *
 * Note:           The default value for the play rate is set by audio.def::AUDIO_PLAY_RATE.
 * 		   The setting takes effect once a new play operation is started. 
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES	AudioSetPlayRate(int playRate);
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
 * Overview:       This function returns the sampling frequency per the play frequency rate.
 * 		   If the rate is 1, the play frequency equals the audio sampling frequency.
 * 		   Else the play frequency is the play rate times slower than the audio sampling frequency.
 *
 * Note:           None
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
int		AudioGetPlayRate(void);
#endif

/************************************************************************
 * Function:       AudioSetRecordRate 
 *
 * PreCondition:   recordRate>0 
 *
 * Input:          recordRate	- the record rate
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the sampling frequency per the record frequency rate.
 * 		   If recordRate is 1, the record frequency equals the audio sampling frequency.
 * 		   Else the record frequency is record rate times slower than the audio sampling frequency.
 *
 * Note:           The default value for the record rate is set by audio.def::AUDIO_RECORD_RATE.
 * 		   The setting takes effect once a new record operation is started. 
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES	AudioSetRecordRate(int recordRate);
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
 * Overview:       This function returns the sampling frequency per the record frequency rate.
 * 		   If the rate is 1, the record frequency equals the audio sampling frequency.
 * 		   Else the record frequency is the record rate times slower than the audio sampling frequency.
 *
 * Note:           None
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
int		AudioGetRecordRate(void);
#endif

/************************************************************************
 * Function:       AudioSetSamplingFreq 
 *
 * PreCondition:   sampleFreq>=AUDIO_MIN_WORK_FREQ 
 *
 * Input:          sampleFreq	- the sampling frequency, in Hz.
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the audio sampling rate.
 *                 After this adjustment the play and record frequencies have to
 *                 be greater than AUDIO_MIN_WORK_FREQ, otherwise the call to AudioPlay/AudioRecord will fail.	
 *
 * Note:           The default value for the sampling rate is set by  audio.def::AUDIO_SAMPLE_FREQ.
 * 		   The setting takes effect once a new play/record operation is started. 
 **************************************************************************/
AUDIO_RES	AudioSetSamplingFreq(int sampleFreq);

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
int		AudioGetSamplingFreq(void);


/************************************************************************
 * Function:       AudioSetMinFreq 
 *
 * PreCondition:   None 
 *
 * Input:          minFreq	- the min frequency, in Hz
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the minimum frequency acceptable for play/record.
 *
 * Note:           The default value is set by audio.def::AUDIO_MIN_WORK_FREQ.
 * 		   The setting takes effect once a new play/record operation is started. 
 **************************************************************************/
AUDIO_RES	AudioSetMinFreq(int minFreq);

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
 * Overview:       This function returns the minimum acceptable play/record frequency.
 *
 * Note:           None	
 **************************************************************************/
int		AudioGetMinFreq(void);


/************************************************************************
 * Function:       AudioSetEncoding 
 *
 * PreCondition:   format  a valid AUDIO_FORMAT value
 *
 * Input:          format	- the working audio format
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current play/record audio format
 *
 * Note:           The setting takes effect once a new play/record operation is started. 
 **************************************************************************/
AUDIO_RES	AudioSetEncoding(AUDIO_FORMAT format);

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
 * Overview:       This function returns the current setting for the  play/record audio format.
 *
 * Note:           None	
 **************************************************************************/
AUDIO_FORMAT	AudioGetEncoding(void);

/************************************************************************
 * Function:       AudioSetInStreamFormat 
 *
 * PreCondition:   sFmt a proper AUDIO_STREAM_FORMAT value
 *
 * Input:          sFmt	- the in stream structure format
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the current stream format to be read from the input stream.
 *
 * Note:           The default value is AUDIO_STREAM_QUERY.
 *                 The setting takes effect once a new play operation is started. 
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES	AudioSetInStreamFormat(AUDIO_STREAM_FORMAT sFmt);
#endif

/************************************************************************
 * Function:       AudioGetInStreamFormat 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the in stream structure format
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the input stream format.
 *
 * Note:           None	
 **************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_STREAM_FORMAT	AudioGetInStreamFormat(void);
#endif

/************************************************************************
 * Function:       AudioSetOutStreamFormat 
 *
 * PreCondition:   sFmt a proper AUDIO_STREAM_FORMAT value
 *
 * Input:          sFmt	- the out stream structure format
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function sets the current stream format to be used for the output stream.
 *
 * Note:           The default value is the first AUDIO_STREAM_FORMAT enumerated value.
 *                 The setting takes effect once a new record operation is started. 
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES	AudioSetOutStreamFormat(AUDIO_STREAM_FORMAT sFmt);
#endif
/************************************************************************
 * Function:       AudioGetOutStreamFormat 
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         the out stream structure format
 *
 * Side Effects:   None
 *
 * Overview:       This function returns the current setting for the output stream format.
 *
 * Note:           None	
 **************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_STREAM_FORMAT	AudioGetOutStreamFormat(void);
#endif

/************************************************************************
 * Function:       AudioSetAdpcmEndianess 
 *
 * PreCondition:   None
 *
 * Input:          isLE	- boolean to specify the samples little endianess
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current setting for the ADPCM nibbles endianess.
 *
 * Note:           The default value is set by audio.def::AUDIO_ADPCM_NIBBLES_LE.
 *                 The setting takes effect once a new play/record operation is started. 
 *                 The ADPCM nibbles endianess is not consistent across file formats.
 *                 Some store nibbles in BE order, some in LE order.	
 **************************************************************************/
AUDIO_RES	AudioSetAdpcmEndianess(int isLE);


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
 * Note:           None	
 **************************************************************************/
int		AudioGetAdpcmEndianess(void);



/************************************************************************
 * Function:       AudioSetActiveChannel 
 *
 * PreCondition:   None
 *
 * Input:          chn	- the active channel
 *
 * Output:         AUDIO_RES_OK
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current active channel to be played for streams supporting multiple channels.
 *
 * Note:           The default value is 1.
 *                 The current implementation supports just one channel play/record.	
 **************************************************************************/
AUDIO_RES	AudioSetActiveChannel(int chn);


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
int		AudioGetActiveChannel(void);


/************************************************************************
 * Function:       AudioGetInStreamInfo 
 *
 * PreCondition:   None
 *
 * Input:          pInfo	-  pointer to address to store the stream information
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
AUDIO_RES	AudioGetInStreamInfo(sStreamInfo* pInfo);
#endif

/************************************************************************
 * Function:       AudioGetOutStreamInfo 
 *
 * PreCondition:   None
 *
 * Input:          pInfo	- pointer to address to store the stream information
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
AUDIO_RES	AudioGetOutStreamInfo(sStreamInfo* pInfo);
#endif

/************************************************************************
 * Function:       AudioSetDmaEnable 
 *
 * PreCondition:   None
 *
 * Input:          enableDma	- boolean to turn the DMA use on/off
 *
 * Output:         AUDIO_RES_OK for success, an error code otherwise
 *
 * Side Effects:   None
 *
 * Overview:       This function modifies the current setting for the DMA use by the audio library.
 *
 * Note:           The default value is set by audio.def::AUDIO_DMA_ENABLED.	
 **************************************************************************/
AUDIO_RES	AudioSetDmaEnable(int enableDma); 


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
 * Note:           None	
 **************************************************************************/
int		AudioGetDmaEnable(void);

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
 * Overview:       This function enables the DMA use and sets the current DMA channel, priority and ipl to be used by the audio library.
 *
 * Note:           - The default value is set by audio.def::AUDIO_DMA_CHN, audio:def::AUDIO_DMA_CHN_PRI.
 *                 - The application needs to call AudioSetDmaChannel() at run time to match the settings used in the SETUP_AUDIO_SAMPLE_INT.
 *                 - The settings take effect once a new play/record operation is started. 
 **************************************************************************/
AUDIO_RES	AudioSetDmaChannel(int chn, int chnPri, int dmaIpl);

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
int		AudioGetDmaChannel(int* pChn, int* pChnPri, int* pDmaIpl);


/****************************************************************************
 * Function:        AudioSetADPctEvent
 *
 * PreCondition:    None
 *
 * Input:           eventCB - event notification callback
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the notification event for recording.
 *                  Whenever a punctual recording occurs, the AudioIOGetRecordSample() can call this callback, if provided.
 *                  The application is thus notified that a record sample was taken and the record ADC
 *                  channel could have been altered.
 *
 * Note:            - The handler should be as short as possible ((like signalling an event flag, smaphore, mux, etc) and NON Blocking!
 *                  - Called from within the ISR!
 *                  - The ipl of the audio ISR (AUDIO_IPL in audio.def) should be selected high enough, so that the audio output is not distorted by other interrupts active within the system. 
 *                 - The setting take effect once a new record operation is started. 
 *****************************************************************************/
void		AudioSetADPctEvent(pSetEvent eventCB);


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
pSetEvent	AudioGetADPctEvent(void);



/****************************************************************************
 * Function:        AudioSetSamplingDevice
 *
 * PreCondition:    valid sampling device and ipl.
 *
 * Input:           sDev	- the device to be used by the audio library to generate the sampling rate.
 *                  samplingIpl	- the IPL to be used when programming the sampling device
 *
 * Output:          AUDIO_RES_OK for success, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function sets the device to be used by the audio library when generating the sampling rate.
 *
 * Note:            - For the time being, the application cannot change dinamically the sampling device because the ISR is placed in ROM and the ISR's ipl is compile time defined.
 *                  - However, the application needs to call AudioSetSamplingDevice() at run time to match the settings used in the SETUP_AUDIO_SAMPLE_INT.
 *                  - The samplingIpl is relevant when the Audio Library won't use DMA. Otherwise, the dmaIpl in the AudioSetDmaChannel is used.
 *                  - The audio library has to be idle when this call is made. 
 *                  - The setting takes effect once a new play/record operation is started. 
 *****************************************************************************/
AUDIO_RES	AudioSetSamplingDevice(AUDIO_SDEV sDev, int samplingIpl);



/****************************************************************************
 * Function:        AudioGetSamplingDevice
 *
 * PreCondition:    None
 *
 * Input:           pSamplingIpl - pointer to store the current sampling Ipl used. Can be NULL.
 *
 * Output:          the device currently used by the audio library to generate the sampling rate. 
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the device currently used by the audio library to generate the sampling rate and the IPL used.
 *
 * Note:            None 
 *****************************************************************************/
AUDIO_SDEV	AudioGetSamplingDevice(int* pSamplingIpl);


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
 * Overview:       This function returns the current number of overrun counts that occurred during the last play/record operation.
 *                 Overrun occurs:
 *                 - when playing: there is no available sample to be played (the read samples callback was slow). The previous sample will be played.
 *                 - when recording: there's no buffer available to store the recorded sample (the write samples callback was slow). The recorded sample will be discarded.
 *
 * Note:           The overrun count is cleared at the beginning of any play/record operation.
 *                 The application can use this value to properly adjust the buffers used by the library with AudioSetProcSamples().
 **************************************************************************/
int		AudioGetOverrun(void);


/******************************************************************************
 * Function:        AudioConvert
 *
 * PreCondition:    the callbacks needed by the encoding/decoding operation have to be properly set prior to this operation
 *
 * Input:           pPrgDcpt - progress descriptor
 *
 * Output:          AUDIO_RES_OK if operation succeeded,
 * 		    an error code otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function converts an input stream to raw samples and then encodes the resulted data to an output data stream.
 *
 * Note:            pPrgDcpt could be null if no progress indication needed
 *                  This functionality is not implemented yet. 
 *****************************************************************************/
#if defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)
AUDIO_RES	AudioConvert(progressDcpt* pPrgDcpt);
#endif

#ifdef SPEEX_ENCODING
/******************************************************************************
 * Function:        AudioSetEncoderQuality
 *
 * PreCondition:    valid quality supported by encoder
 *
 * Input:           qFactor - the desired encoder quality
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
AUDIO_RES	AudioSetEncoderQuality(int qFactor);
#endif

/******************************************************************************
 * Function:        AudioSetEncoderBitrate
 *
 * PreCondition:    valid bitrate supported by encoder
 *
 * Input:           bRate - the desired bitrate, bps
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
AUDIO_RES	AudioSetEncoderBitrate(int bRate);
#endif

/******************************************************************************
 * Function:        AudioSetEncoderComplexity
 *
 * PreCondition:    valid complexity supported by encoder
 *
 * Input:           cFactor - the desired encoder quality
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
AUDIO_RES	AudioSetEncoderComplexity(int cFactor);
#endif

/******************************************************************************
 * Function:        AudioSetEncoderFrames
 *
 * PreCondition:    None
 *
 * Input:           nFrames - the desired number of frames
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the encoder number of frames per packet. 
 *
 * Note:            Can be meaningfull just for some output streams.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES	AudioSetEncoderFrames(int nFrames);
#endif

/******************************************************************************
 * Function:        AudioSetEncoderPackets
 *
 * PreCondition:    None
 *
 * Input:           nPackets - the desired number of packets
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function sets the encoder number of packets per page. 
 *
 * Note:            Can be meaningfull just for some output streams.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES	AudioSetEncoderPackets(int nPackets);
#endif

/******************************************************************************
 * Function:        AudioSetDecoderEnhancement
 *
 * PreCondition:    None
 *
 * Input:           eVal - boolean to turn the enhancement on/off
 *
 * Output:          AUDIO_RES_OK if this parameter is supported by the current encoder format,
 * 		    AUDIO_RES_UNSUPPORTED otherwise 
 *
 * Side Effects:    None.
 *
 * Overview:        This function turn on/off decoder perceptual enhancement. 
 *
 * Note:            Note all decoders support this feature. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES	AudioSetDecoderEnhancement(int eVal);
#endif

/******************************************************************************
 * Function:        AudioSetCodecBand
 *
 * PreCondition:    valid band supported by the codec
 *
 * Input:           None 
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
AUDIO_RES	AudioSetCodecBand(AUDIO_BAND band);


#endif		// SPEEX_ENCODING

/*********************************************************************
 *                   Application IO supplied callbacks
 *                   General note: any of these functions can be provided by the application.
 *                   However, default versions are supplied with the audio library.
 **********************************************************************************************/

/****************************************************************************
 * Function:        AudioIOInitPlay
 *
 * PreCondition:    None
 *
 * Input:           sysFreq	- system running frequency, Hz
 *                  tmrFreq	- the timer frequency used to generate the sample rate, Hz
 *                  sampleFreq	- the sampling frequency, Hz
 *                  bitsPerSample - the number of bits per sample
 *                  rate	- the play rate
 *                  dmaTxfer	- dma transfer requested for the play operation
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the IO devices used for playback, output amplifiers, etc.
 *                  The audio library calls this function whenever a new play starts.
 *                  The application can provide it's own AudioIOInitPlay function.
 *                  A default IO function that uses PWM output and supports DMA transfers is provided.
 *                  The function should return AUDIO_RES_DMA_UNSUPPORTED if it cannot tolerate a DMA transfer.
 *                  With no DMA transfers, AudioIOPlaySample() will be called for each sample in turn.
 *                  When DMA is used, AudioIOProcessPlayBuffer() will be called for any adjustments needed before the DMA engine sends
 *                  the samples to the output.
 *
 * Note:            If the AudioIOInitPlay returns AUDIO_RES_HIGH_RESOLUTION the audio library will perform retries, lowering the timer frequency,
 *                  in order to allow higher resolutions to be obtained.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES	AudioIOInitPlay(int sysFreq, int tmrFreq, int sampleFreq, int bitsPerSample, int rate, int dmaTxfer);
#endif

/****************************************************************************
 * Function:        AudioIOClosePlay
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the clean up of the IO devices used for playback.
 *                  The audio library calls this function whenever a play operation ended.
 *                  The application can provide it's own AudioIOClosePlay function.
 *                  A default IO function that uses PWM output is provided.
 * 		 
 *
 * Note:            This function should return AUDIO_RES_OK. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES	AudioIOClosePlay(void);
#endif

/****************************************************************************
 * Function:        AudioIOInitRecord
 *
 * PreCondition:    None
 *
 * Input:           sysFreq	- system running frequency, Hz
 *                  tmrFreq	- the timer frequency used to generate the sample rate, Hz
 *                  sampleFreq	- the sampling frequency, Hz
 *                  bitsPerSample - the number of bits per sample
 *                  rate	- the recording rate
 *                  dmaTxfer	- dma transfer requested for the record operation                  
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the IO devices used for recording, input amplifiers, etc.
 *                  The audio library calls this function whenever a new record operation starts.
 *                  The application can provide it's own AudioIOInitRecord function.
 *                  A default IO function that uses the ADC input and supports DMA transfers is provided.
 *                  The function should return AUDIO_RES_DMA_UNSUPPORTED if it cannot tolerate a DMA transfer.
 *                  With no DMA transfers, AudioIOGetRecordSample() will be called for each sample in turn.
 *                  When DMA is used, AudioIOProcessRecordBuffer() will be called for any adjustments needed after the DMA engine
 *                  transferred the data from the input and before storage for the recorded samples is called.
 * 		 
 *
 * Note:            If the AudioIOInitRecord returns AUDIO_RES_HIGH_RESOLUTION the audio library will perform retries, lowering the timer frequency,
 *                  in order allow higher resolutions to be obtained.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES	AudioIOInitRecord(int sysFreq, int tmrFreq, int sampleFreq, int bitsPerSample, int rate, int dmaTxfer);
#endif

/****************************************************************************
 * Function:        AudioIOCloseRecord
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the clean up of the IO devices used for recording.
 *                  The audio library calls this function whenever a record operation ended.
 *                  The application can provide it's own AudioIOCloseRecord function.
 *                  A default IO function that uses ADC input is provided.
 * 		 
 *
 * Note:            This function should return AUDIO_RES_OK. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES	AudioIOCloseRecord(void);
#endif

/****************************************************************************
 * Function:        AudioIOPlaySample
 *
 * PreCondition:    None
 *
 * Input:           sample	- the current sample to be played
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when a new sample has to be sent to the playback device during a play operation
 *                  that does not use DMA transfers.
 *                  The application can provide it's own AudioIOPlaySample function.
 *                  A default IO function that uses PWM output is provided.
 * 		 
 *
 * Note:            This function is called from within the sample ISR.
 *                  Has to be as short as possible and non blocking!
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void		AudioIOPlaySample(short sample);
#endif

/****************************************************************************
 * Function:        AudioIOPlayProcessNeeded
 *
 * PreCondition:    None
 *
 * Input:           dmaProcess - true if a DMA transfer will be used to play the data, false otherwise 
 *
 * Output:          boolean specifying if samples processing needed  
 *
 * Side Effects:    None
 *
 * Overview:        This function returns true if samples need processing before actually played.
 *                  I.e. the AudioIOProcessPlayBuffer() has to be called before the AudioIOPlaySample()/DMA transfer
 *                  can proceed.
 *                  It means that the playback device cannot use the 16 bit signed PCM samples
 *                  as they are, without extra processing.
 *                    
 *                  The application can provide it's own AudioIOPlayProcessNeeded function.
 *                  A default IO function that works with PWM output is provided.
 * 		 
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
int		AudioIOPlayProcessNeeded(int dmaProcess);
#endif

/****************************************************************************
 * Function:        AudioIOProcessPlayBuffer
 *
 * PreCondition:    None
 *
 * Input:           pSBuff	- pointer to the buffer containing samples to be processed
 *                  nSamples	- number of samples contained into the buffer
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        If AudioIOPlayProcessNeeded() returned true, this function is called just before data can be sent
 *                  to the output to be played.
 *                  Any additional processing that needs to be done on the samples before sending them to the output
 *                  must be done here.
 *                  The application can provide it's own AudioIOProcessPlayBuffer function.
 *                  A default IO function that uses PWM output is provided.
 * 		 
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void		AudioIOProcessPlayBuffer(short* pSBuff, int nSamples);
#endif



/****************************************************************************
 * Function:        AudioIOGetRecordSample
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          The new sample to be recorded 
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when a new sample is needed during a record operation.
 *                  The application can provide it's own AudioIOGetRecordSample function.
 *                  A default IO function that uses the internal ADC input is provided.
 * 		 
 *
 * Note:            This function is called from within the sample ISR.
 *                  Has to be as short as possible and non blocking!
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
short		AudioIOGetRecordSample(void);
#endif

/****************************************************************************
 * Function:        AudioIORecordProcessNeeded
 *
 * PreCondition:    None
 *
 * Input:           dmaProcess - true if a DMA transfer was used to get the record data, false otherwise 
 *
 * Output:          boolean specifying if samples processing needed  
 *
 * Side Effects:    None
 *
 * Overview:        This function returns true if samples need extra processing after they've been stored into an 
 *                  internal buffer and before they can be actually stored on an external device.
 *                  I.e. the AudioIOProcessRecordBuffer() has to be called after the AudioIOGetRecordSample()/DMA transfer
 *                  was performed.
 *                  It usually means that the recording device doesn't actually deliver plain 16 bit
 *                  signed samples, without extra processing.
 *                    
 *                  The application can provide it's own AudioIORecordProcessNeeded function.
 *                  A default IO function that works with the internal ADC input is provided.
 * 		 
 *
 * Note:            None.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
int		AudioIORecordProcessNeeded(int dmaProcess);
#endif

/****************************************************************************
 * Function:        AudioIOProcessRecordBuffer
 *
 * PreCondition:    None
 *
 * Input:           pSBuff	- pointer to the buffer containing samples to be processed
 *                  nSamples	- number of samples contained into the buffer
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        If AudioIORecordProcessNeeded() returned true, this function is called before data buffer
 *                  can be sent to the output stream for storage.
 *                  Any additional processing that needs to be done on the samples to have them in proper
 *                  16 bit signed format must be done here.
 *                  The application can provide it's own AudioIOProcessRecordBuffer function.
 *                  A default IO function that uses the internal ADC input is provided.
 * 		 
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
void		AudioIOProcessRecordBuffer(short* pSBuff, int nSamples);
#endif


/****************************************************************************
 * Function:        AudioIOGetPlayAddress
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          the address of the playback device 
 *
 * Side Effects:    None
 *
 * Overview:        This function has to return the address of the playback device used by the
 *                  audio IO device.
 *                  The audio library calls this function when a playback operation using DMA is started. 
 *                  The application can provide it's own AudioIOGetPlayAddress function.
 *                  A default IO function is provided.
 * 		 
 *
 * Note:            The audio library calls this function only if AudioIOInitPlay() with dmaTxfer=1 returned success.  
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
volatile unsigned int*		AudioIOGetPlayAddress(void);
#endif

/****************************************************************************
 * Function:        AudioIOGetRecordAddress
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          the address of the recording device 
 *
 * Side Effects:    None
 *
 * Overview:        This function has to return the address of the recording device used by the
 *                  audio IO device.
 *                  The audio library calls this function when a record operation using DMA is started. 
 *                  The application can provide it's own AudioIOGetRecordAddress function.
 *                  A default IO function is provided.
 * 		 
 *
 * Note:            The audio library calls this function only if AudioIOInitRecord() with dmaTxfer=1 returned success.  
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
volatile unsigned int*		AudioIOGetRecordAddress(void);
#endif

/****************************************************************************
 * Function:        AudioIOEnableOutAmp
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function enables the playback output amplifier.
 *                  The audio library calls this function whenever a play operation starts/resumes.
 *                  The application can provide it's own AudioIOEnableOutAmp function.
 *                  A default IO function is provided.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void		AudioIOEnableOutAmp(void);
#endif

/****************************************************************************
 * Function:        AudioIODisableOutAmp
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function disables the playback output amplifier.
 *                  The audio library calls this function whenever a play operation stops/pauses.
 *                  The application can provide it's own AudioIODisableOutAmp function.
 *                  A default IO function is provided.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void		AudioIODisableOutAmp(void);
#endif


/****************************************************************************
 * Function:        AudioIOEnableInAmp
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function enables the record input amplifier.
 *                  The audio library calls this function whenever a record operation starts/resumes.
 *                  The application can provide it's own AudioIOEnableInAmp function.
 *                  A default IO function is provided.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
void		AudioIOEnableInAmp(void);
#endif

/****************************************************************************
 * Function:        AudioIODisableInAmp
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function disables the record input amplifier.
 *                  The audio library calls this function whenever a record operation stops/pauses.
 *                  The application can provide it's own AudioIODisableInAmp function.
 *                  A default IO function is provided.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
void		AudioIODisableInAmp(void);
#endif


/****************************************************************************
 * Function:        SETUP_AUDIO_SAMPLE_INT
 *
 * PreCondition:    proper definitions for AUDIO_SAMPLE_DEVICE_ and AUDIO_SAMPLE_IPL in the audio.def file
 *
 * Input:           None
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This macro is a helper to be inserted in the application using the audio library.
 *                  It allows the flexibility of choosing the desired sampling device at run time
 *                  (i.e. an interrupt vector and ipl) without the need to re-build the audio library.
 *
 * Note:            - The Audio Library provides functions for setting the sampling device at run time
 *                  like AudioSetSamplingDevice(). However, for the time being, the functionality doesn't allow
 *                  the application to change dinamically the sampling device and it's ipl, because the sampling ISR
 *                  is placed in ROM.
 *                  - The Audio Library uses the default sampling device defined in the audio.def
 *                  There's no real need to call AudioSetSamplingDevice() at run time unless you're setting
 *                  a different sampling device than the one specified in the audio.def.
 *
 * Example:         SETUP_AUDIO_SAMPLE_INT
 *****************************************************************************/
#define AUDIO_SDEV_TMR2 0
#define AUDIO_SDEV_TMR3 1

#if AUDIO_DMA_ENABLED
	#if AUDIO_DMA_CHN==0
		#define __AUDIO_ISR_VECTOR	_DMA0_VECTOR
	#elif AUDIO_DMA_CHN==1
		#define __AUDIO_ISR_VECTOR	_DMA1_VECTOR
	#elif AUDIO_DMA_CHN==2
		#define __AUDIO_ISR_VECTOR	_DMA2_VECTOR
	#elif AUDIO_DMA_CHN==3
		#define __AUDIO_ISR_VECTOR	_DMA3_VECTOR
	#else
		#error "AUDIO_DMA_CHN: 0 to 3 valid values only."
	#endif
#elif AUDIO_SAMPLE_DEVICE==AUDIO_SDEV_TMR2
	#define	__AUDIO_ISR_VECTOR		_TIMER_2_VECTOR
#elif AUDIO_SAMPLE_DEVICE==AUDIO_SDEV_TMR3
	#define	__AUDIO_ISR_VECTOR		_TIMER_3_VECTOR
#else
	#error "AUDIO_SAMPLE_DEVICE: either AUDIO_SDEV_TMR2 or AUDIO_SDEV_TMR3 have to be used."
#endif	

#if AUDIO_SAMPLE_IPL == 7
	#define	__AUDIO_ISR_IPL	ipl7
#elif AUDIO_SAMPLE_IPL ==6
	#define	__AUDIO_ISR_IPL	ipl6
#elif AUDIO_SAMPLE_IPL ==5
	#define	__AUDIO_ISR_IPL	ipl5
#elif AUDIO_SAMPLE_IPL ==4
	#define	__AUDIO_ISR_IPL	ipl4
#elif AUDIO_SAMPLE_IPL ==3
	#define	__AUDIO_ISR_IPL	ipl3
#elif AUDIO_SAMPLE_IPL ==2
	#define	__AUDIO_ISR_IPL	ipl2
#else
	#define	__AUDIO_ISR_IPL	ipl1
#endif	


#define SETUP_AUDIO_SAMPLE_INT()	extern void __AudioSampleRoutine(void);						\
				void __ISR(__AUDIO_ISR_VECTOR, __AUDIO_ISR_IPL) __AudioSampleIntHandler(void)	\
				{										\
					__AudioSampleRoutine();							\
				}	// macro to set up the Audio Library sampling interrupt routine




#if PROFILING
// some profiling functions
// The profiling is done by instrumenting the code

/****************************************************************************
 * Function:        AudioProfilingInit
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function clears all the internal maintained profile counters.
 *
 * Note:            None 
 *****************************************************************************/
void			AudioProfilingInit(void);


/****************************************************************************
 * Function:        AudioSetProfileIo
 *
 * PreCondition:    None
 *
 * Input:           profIo - boolean to turn on/off profiling during i/o operations
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function changes the profiling during read/write i/o operations.
 *                  When profIo is set, the time taken for the i/o operations is also counted.
 *                  These i/o operations are not part of the Audio Library, they are supplied by
 *                  the user by the respective callbacks. 
 *  counters.
 *
 * Note:            None 
 *****************************************************************************/
void			AudioSetProfileIo(int profIo);


/****************************************************************************
 * Function:        AudioGetProfileIo
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          current status of the i/o profiling 
 *
 * Side Effects:    None
 *
 * Overview:        This function is a helper to return the current status of the i/o profiling.
 *
 * Note:            None 
 *****************************************************************************/
int			AudioGetProfileIo(void);

/****************************************************************************
 * Function:        AudioProfilingCycles
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          current number of profiling cycles 
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the number of clock cycles spent in the Audio Library processing functions,
 *                  as counted by the internal profiling mechanism.
 *
 * Note:            None 
 *****************************************************************************/
unsigned long long	AudioProfilingCycles(void);


/****************************************************************************
 * Function:        AudioProfilingIntCycles
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          current number of profiling cycles 
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the number of clock cycles spent in the Audio Library ISR,
 *                  as counted by the internal profiling mechanism.
 *
 * Note:            None 
 *****************************************************************************/
unsigned long long	AudioProfilingIntCycles(void);

/****************************************************************************
 * Function:        AudioProfilingOvlapCycles
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          current number of profiling cycles 
 *
 * Side Effects:    None
 *
 * Overview:        This function returns the number of clock cycles that were counted as spent
 *                  in both Audio Library processing functions and Audio Library ISR.
 *                  The ISRs occurring while already in the Audio Library are considered
 *                  to be overlapping.
 *
 * Note:            None 
 *****************************************************************************/
unsigned long long	AudioProfilingOvlapCycles(void);


#endif	// PROFILING


#endif // _AUDIO_H_



