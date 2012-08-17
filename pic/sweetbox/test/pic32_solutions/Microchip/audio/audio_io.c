/*********************************************************************
 *
 *                  Default AUDIO IO functions
 *
 *********************************************************************
 * FileName:        audio_io.c
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
#include <math.h>


#include <GenericTypeDefs.h>

#include <plib.h>

#include <audio/audio.h>
#include "audio_io.def"


// local definitions
//


#if defined (TIMER2)		
	#define	OC_TMR_SRC			OC_TIMER2_SRC						
#elif defined (TIMER3)
	#define	OC_TMR_SRC			OC_TIMER3_SRC						
#else
	#error "TIMER2 or TIMER3 has to be defined."
#endif



/* Output Compare definitions */
#if defined OUTCOMPARE1
	#define	OpenOC()		OpenOC1(OC_ON | OC_PWM_FAULT_PIN_DISABLE | OC_TMR_SRC ,0,0)
	#define	CloseOC()   		OpenOC1(OC_OFF,0,0)
	#define	SetOCDutyCycle(v)	SetDCOC1PWM((v))
	#define	OCPlayReg		OC1RS
#elif defined OUTCOMPARE2
	#define	OpenOC()		OpenOC2(OC_ON | OC_PWM_FAULT_PIN_DISABLE | OC_TMR_SRC ,0,0)
	#define	CloseOC()   		OpenOC2(OC_OFF,0,0)
	#define	SetOCDutyCycle(v)	SetDCOC2PWM((v))
	#define	OCPlayReg		OC2RS
#elif defined OUTCOMPARE3
	#define	OpenOC()		OpenOC3(OC_ON | OC_PWM_FAULT_PIN_DISABLE | OC_TMR_SRC ,0,0)
	#define	CloseOC()   		OpenOC3(OC_OFF,0,0)
	#define	SetOCDutyCycle(v)	SetDCOC3PWM((v))
	#define	OCPlayReg		OC3RS
#elif defined OUTCOMPARE4
	#define	OpenOC()		OpenOC4(OC_ON | OC_PWM_FAULT_PIN_DISABLE | OC_TMR_SRC ,0,0)
	#define	CloseOC()   		OpenOC4(OC_OFF,0,0)
	#define	SetOCDutyCycle(v)	SetDCOC4PWM((v))
	#define	OCPlayReg		OC4RS
#elif defined OUTCOMPARE5
	#define	OpenOC()		OpenOC5(OC_ON | OC_PWM_FAULT_PIN_DISABLE | OC_TMR_SRC ,0,0)
	#define	CloseOC()   		OpenOC5(OC_OFF,0,0)
	#define	SetOCDutyCycle(v)	SetDCOC5PWM((v))
	#define	OCPlayReg		OC5RS
#endif




// ADC10 definitions
#define	MAX_FREQ_TAD		5000000	// frequency corresponfing to Tad>=0.2us


#define	ADC1_WORD		(ADC_MODULE_OFF|ADC_FORMAT_FRACT|ADC_CLK_AUTO|ADC_AUTO_SAMPLING_ON)
#define	ADC2_WORD		(ADC_VREF_SOURCE|ADC_OFFSET_CAL_DISABLE|ADC_SCAN_OFF|ADC_SAMPLES_PER_INT_1|ADC_ALT_BUF_OFF|ADC_ALT_INPUT_OFF)
#define	ADC3_WORD		(ADC_CONV_CLK_PB|((ADC_SAMPLE_TAD&0x1f)<<_AD1CON3_SAMC_POSITION))
#define	CSS_WORD		0				// don't use scan

#define	OpenAdc()		(AD1CON1SET=ADC_MODULE_ON)		// turn ADC on
#define	CloseAdc()		(AD1CON1CLR=ADC_MODULE_ON)		// turn ADC off




#ifdef AUDIO_AMP
	#define	OpenAmp()			PORTSetPinsDigitalOut(AUDIO_PORT, AUDIO_PORT_BIT)
	#if AUDIO_PORT_POLARITY
		#define	EnableAmp()		PORTSetBits(AUDIO_PORT, AUDIO_PORT_BIT)
		#define DisableAmp()		PORTClearBits(AUDIO_PORT, AUDIO_PORT_BIT)
	#else
		#define	EnableAmp()		PORTClearBits(AUDIO_PORT, AUDIO_PORT_BIT)
		#define DisableAmp()		PORTSetBits(AUDIO_PORT, AUDIO_PORT_BIT)
	#endif
#else
	#define	OpenAmp()
	#define	EnableAmp()
	#define DisableAmp()
#endif



#ifdef AUDIO_SUPPORT_PLAY
static int			adpcmPwmShiftOut;		// bits to discard due to the PWM resolution
#endif


#define	LOG2	0.301029995664		// log10(2) constant


// local prototypes
//
#ifdef AUDIO_SUPPORT_RECORD
#if defined(ADC_EXCLUSIVE_MODE) || defined(ADC_EXT_MODE_POINT)
	static void			InitAdc(int adcs);
#endif
#endif


/*********************************************************************
 *                   Default audio IO supplied callbacks
 *                   General note: any of these functions can be provided by the application.
 *                   These are just the default versions supplied with the audio library.
 *                   Feel free to override them.
 **********************************************************************************************/


/****************************************************************************
 * Function:        AudioIOInitPlay
 *
 * PreCondition:    None
 *
 * Input:           sysFreq:	system running frequency, Hz
 *                  tmrFreq:	the timer frequency used to generate the sample rate, Hz and the sampling rate
 *                  sampleFreq:	the sampling frequency, Hz
 *                  bitsPerSample: the number of bits per sample
 *                  rate:	the play rate
 *                  dmaTxfer:	dma transfer requested for the play operation
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the IO devices used for playback, output amplifiers, etc.
 *                  The audio library calls this function whenever a new play starts.
 *                  The application can provide it's own AudioIOInitPlay function.
 *                  A standard IO function that uses PWM output and supports DMA transfers is provided by default.
 *                  The function should return AUDIO_RES_DMA_UNSUPPORTED if it cannot tolerate a DMA transfer.
 *                  Without DMA, AudioIOPlaySample() will be called for each sample in turn.
 *                  Otherwise, AudioIOProcessPlayBuffer() will be called for any adjustments needed by the hardware before sending the samples to
 *                  the output.
 *
 * Note:            If the AudioIOInitPlay returns AUDIO_RES_HIGH_RESOLUTION the audio library will perform retries, lowering the timer frequency,
 *                  in order to allow higher resolutions to be obtained.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES __attribute__((weak)) AudioIOInitPlay(int sysFreq, int tmrFreq, int sampleFreq, int bitsPerSample, int rate, int dmaTxfer)
{	
	int	resBits;

	if(bitsPerSample!=16)
	{
		return AUDIO_RES_BPS_UNSUPPORTED;
	}

	resBits=(int)ceil(log10((double)tmrFreq/(double)sampleFreq)/LOG2);
	if(resBits>16)
	{	// shouldn't happen for the normal values
		return AUDIO_RES_HIGH_RESOLUTION;
	}
	else if(resBits<AUDIO_MIN_RES_BITS)
	{	// need higher system frequency or lower audio resolution
		return AUDIO_RES_LOW_RESOLUTION;
	}
	
	adpcmPwmShiftOut=bitsPerSample-resBits;

	OpenAmp();	// enable audio amp
	DisableAmp();
	
	return AUDIO_RES_OK;
}
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
 *                  A standard IO function that uses PWM output is provided by default.
 * 		 
 *
 * Note:            This function should return AUDIO_RES_OK. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
AUDIO_RES __attribute__((weak)) AudioIOClosePlay(void)
{
	CloseOC();			// close the output compare
	DisableAmp();			// turn amp off
	return AUDIO_RES_OK;
}
#endif

/****************************************************************************
 * Function:        AudioIOInitRecord
 *
 * PreCondition:    None
 *
 * Input:           sysFreq:	system running frequency, Hz
 *                  tmrFreq:	the timer frequency used to generate the sample rate, Hz and the sampling rate
 *                  sampleFreq:	the sampling frequency, Hz
 *                  bitsPerSample: the number of bits per sample
 *                  rate:	the recording rate
 *                  dmaTxfer:	dma transfer requested for the record operation                  
 *
 * Output:          AUDIO_RES_OK if succes, an error code otherwise 
 *
 * Side Effects:    None
 *
 * Overview:        This function performs the initialization of the IO devices used for recording, input amplifiers, etc.
 *                  The audio library calls this function whenever a new record operation starts.
 *                  The application can provide it's own AudioIOInitRecord function.
 *                  A standard IO function that usesADC input and supports DMA transfers is provided by default.
 *                  The function should return AUDIO_RES_DMA_UNSUPPORTED if it cannot tolerate a DMA transfer.
 *                  Without DMA, AudioIOGetRecordSample() will be called for each sample in turn.
 *                  Otherwise, AudioIOProcessRecordBuffer() will be called for any adjustments needed by the hardware before storing the recorded samples.
 * 		 
 *
 * Note:            If the AudioIOInitRecord returns AUDIO_RES_HIGH_RESOLUTION the audio library will perform retries, lowering the timer frequency,
 *                  in order allow higher resolutions to be obtained.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES __attribute__((weak)) AudioIOInitRecord(int sysFreq, int tmrFreq, int sampleFreq, int bitsPerSample, int rate, int dmaTxfer)
{
#if defined(ADC_EXCLUSIVE_MODE)	
	int	adcs;
	int	pbFreq;
	
	if(bitsPerSample>16)
	{
		return AUDIO_RES_BPS_UNSUPPORTED;
	}

	pbFreq=sysFreq/(1<<mOSCGetPBDIV());	// get the PB frequency
	// calculate the conversion time
	adcs=(int)ceil((double)pbFreq/(double)MAX_FREQ_TAD)-1;	// using the ADC at the fastest rate
	InitAdc(adcs);
	return AUDIO_RES_OK;

#elif defined(ADC_EXT_MODE_POINT)

	if(dmaTxfer)
	{
		return AUDIO_RES_DMA_UNSUPPORTED;
	}
	else
	{
		InitAdc(0);
		return AUDIO_RES_OK;
	}
	
#elif defined(ADC_EXT_MODE_SCAN)
	return AUDIO_RES_OK;

#else
	#error "Unsupported ADC format"
       return AUDIO_RES_UNSUPPORTED;	
#endif
}
#endif	// AUDIO_SUPPORT_RECORD


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
 *                  A standard IO function that uses ADC input is provided by default.
 * 		 
 *
 * Note:            This function should return AUDIO_RES_OK. 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
AUDIO_RES __attribute__((weak)) AudioIOCloseRecord(void)
{
#ifdef	ADC_EXCLUSIVE_MODE	
	CloseAdc();			// turn ADC off
#endif
	return AUDIO_RES_OK;
}
#endif	// AUDIO_SUPPORT_RECORD


/****************************************************************************
 * Function:        AudioIOPlaySample
 *
 * PreCondition:    None
 *
 * Input:           sample:	the current sample to be played
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function is called when a new sample has to be sent to the playback device during a play operation.
 *                  The application can provide it's own AudioIOPlaySample function.
 *                  A standard IO function that uses PWM output is provided by default.
 * 		 
 *
 * Note:            This function is called from within the sample ISR.
 *                  Has to be as short as possible and non blocking!
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void __attribute__((weak)) AudioIOPlaySample(short sample)
{
	unsigned short pwmVal;
	int	iSample=sample;

#ifdef	AUDIO_OUT_POS
	iSample+=32768;		// make it positive output
#endif
	pwmVal=(unsigned short)((iSample)>>adpcmPwmShiftOut);
	if(pwmVal==1)
	{
		pwmVal=0;	// fix for OC bug
	}

	SetOCDutyCycle(pwmVal);	// update the duty cycle
}
#endif	// AUDIO_SUPPORT_PLAY

/****************************************************************************
 * Function:        AudioIOProcessPlayBuffer
 *
 * PreCondition:    None
 *
 * Input:           pSBuff:	pointer to the buffer containing samples to be processed
 *                  nSamples:	number of samples contained into the buffer
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function is called before data can be sent to the output by the DMA controller.
 *                  Any additional processing thatr needs to be done on the samples before sending them to the output
 *                  must be done here.
 *                  The application can provide it's own AudioIOProcessPlayBuffer function.
 *                  A standard IO function that uses PWM output is provided by default.
 * 		 
 *
 * Note:            This function is called periodically when new samples are available.
 *                  Has to be as short as possible and non blocking.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void __attribute__((weak)) AudioIOProcessPlayBuffer(short* pSBuff, int nSamples)
{
	unsigned short pwmVal;
	int	ix, iSample;

	for(ix=0; ix<nSamples; ix++)
	{
		iSample=*pSBuff;
	#ifdef	AUDIO_OUT_POS
		iSample+=32768;		// make it positive output
	#endif

		pwmVal=(unsigned short)((iSample)>>adpcmPwmShiftOut);
		if(pwmVal==1)
		{
			pwmVal=0;	// fix for OC bug
		}
		*pSBuff++=pwmVal;
	}
}
#endif	// AUDIO_SUPPORT_PLAY


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
 *                  I.e. the AudioIOProcessPlayBuffer() has to be called before the AudioIOPlaySample() can be called,
 *                  or the DMA transfer initiated, meaning that the playback device cannot use the raw 16 bit signed samples
 *                  as they are, without extra processing.
 *                    
 *                  The application can provide it's own AudioIOPlayProcessNeeded function.
 *                  A standard IO function that works with PWM output is provided by default.
 * 		 
 *
 * Note:            None
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
int __attribute__((weak)) AudioIOPlayProcessNeeded(int dmaProcess)
{
	return dmaProcess;	// AudioIOProcessPlayBuffer() needs to be called for the DMA transfers
}
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
 *                  The audio library calls this function when a DMA playback operation is started. 
 *                  The application can provide it's own AudioIOGetPlayAddress function.
 *                  A standard IO function is provided by default.
 * 		 
 *
 * Note:            The audio library calls this function only if AudioIOInitPlay() with dmaTxfer=1 returned success.  
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
volatile unsigned int* AudioIOGetPlayAddress(void)
{
	return &OCPlayReg;
}
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
 * Overview:        This function is called when a new sample has to be recorded during a record operation.
 *                  The application can provide it's own AudioIOGetRecordSample function.
 *                  A standard IO function that uses the internal ADC input is provided by default.
 * 		 
 *
 * Note:            This function is called from within the sample ISR.
 *                  Has to be as short as possible and non blocking!
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
short __attribute__((weak)) AudioIOGetRecordSample(void)
{
				
	unsigned short	adcVal;

#ifdef	ADC_EXCLUSIVE_MODE
	adcVal=(unsigned short)ReadADC10(0);	// always sample in ADC1BUF0 no matter what channel
#elif defined(ADC_EXT_MODE_SCAN)
	adcVal=(unsigned short)ReadADC10(ADC_SCAN_INDEX);	// read the scan channel
	#ifdef ADC_UNSIGNED_10
		adcVal<<=6;	// make it proper short
	#endif
#else
// punctual access
//
	pSetEvent	audioPctCB;
	
	AD1CON1=(ADC_MODULE_OFF|ADC_FORMAT_FRACT|ADC_CLK_AUTO|ADC_AUTO_SAMPLING_ON);
	AD1CON2=(ADC_VREF_SOURCE|ADC_OFFSET_CAL_DISABLE|ADC_SCAN_OFF|ADC_SAMPLES_PER_INT_1|ADC_ALT_BUF_OFF|ADC_ALT_INPUT_OFF);
	AD1CHS = (ADC_CH0_NEG_SAMPLEA_NVREF|(AUDIO_ADC_CHN << _AD1CHS_CH0SA_POSITION));	// Switch ADC channel
	AD1CON3=(ADC_CONV_CLK_PB|((ADC_SAMPLE_TAD&0x1f)<<_AD1CON3_SAMC_POSITION)|((ADC_PB_TAD_DIV&0xff)<<_AD1CON3_ADCS_POSITION));
	// start conversion
	AD1CON1SET=ADC_MODULE_ON;
	while(!(AD1CON1&_AD1CON1_DONE_MASK));	// wait conversion done
	adcVal=(unsigned short)ADC1BUF0;	// read sample
	if((audioPctCB=AudioGetADPctEvent()))
	{
		(*audioPctCB)();
	}
#endif
			
#ifdef	AUDIO_IN_POS				
	return (short)((int)adcVal-32768);
#else
	return (short)adcVal;
#endif

}
#endif	// AUDIO_SUPPORT_RECORD

/****************************************************************************
 * Function:        AudioIOProcessRecordBuffer
 *
 * PreCondition:    None
 *
 * Input:           pSBuff:	pointer to the buffer containing samples to be processed
 *                  nSamples:	number of samples contained into the buffer
 *
 * Output:          None 
 *
 * Side Effects:    None
 *
 * Overview:        This function is called before data buffer can be sent to the output stream.
 *                  Any additional processing that needs to be done on the samples to have them in proper
 *                  16 bit signed format must be done here.
 *                  The application can provide it's own AudioIOProcessRecordBuffer function.
 *                  A standard IO function that uses the internal ADC input is provided by default.
 * 		 
 *
 * Note:            This function is called periodically as new samples are recorded.
 *                  Has to be as short as possible and non blocking.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
void  __attribute__((weak)) AudioIOProcessRecordBuffer(short* pSBuff, int nSamples)
{
#if defined(ADC_EXCLUSIVE_MODE) || defined(ADC_EXT_MODE_SCAN)	
#if defined(AUDIO_IN_POS) || (defined(ADC_EXT_MODE_SCAN) && defined(ADC_UNSIGNED_10)) 
	int	ix;
	unsigned short	adcVal;
	
	for(ix=0; ix<nSamples; ix++)
	{
		adcVal=(unsigned short)*pSBuff;
	#if defined(ADC_EXT_MODE_SCAN)
		#ifdef ADC_UNSIGNED_10
			adcVal<<=6;	// make it proper short
		#endif
	#endif
		
	#ifdef	AUDIO_IN_POS				
		adcVal=(short)((int)adcVal-32768);
	#endif
		
		*pSBuff++=adcVal;
	}

#endif
#endif

}
#endif	// AUDIO_SUPPORT_RECORD


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
 * Overview:        This function returns true if samples need processing before actually stored on an external device.
 *                  I.e. the AudioIOProcessRecordBuffer() has to be called after the AudioIOGetRecordSample() was called,
 *                  or the DMA transfer was performed, meaning that the recording device doesn't actually deliver plain 16 bit
 *                  signed samples, without extra processing.
 *                    
 *                  The application can provide it's own AudioIORecordProcessNeeded function.
 *                  A standard IO function that works with the internal ADC input is provided by default.
 * 		 
 *
 * Note:            None.
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
int  __attribute__((weak)) AudioIORecordProcessNeeded(int dmaProcess)
{
	return dmaProcess;	// AudioIOProcessRecordBuffer() needs to be called for the DMA transfers
}
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
 *                  The audio library calls this function when a DMA record operation is started. 
 *                  The application can provide it's own AudioIOGetRecordAddress function.
 *                  A standard IO function is provided by default.
 * 		 
 *
 * Note:            The audio library calls this function only if AudioIOInitRecord() with dmaTxfer=1 returned success.  
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
volatile unsigned int* AudioIOGetRecordAddress(void)
{

#ifdef	ADC_EXCLUSIVE_MODE
	return &ADC1BUF0;	// always sample in ADC1BUF0 no matter what channel 
#elif defined(ADC_EXT_MODE_SCAN)
	return &ADC1BUF0+((ADC_SCAN_INDEX)*4);
#else
	return 0;
#endif
	
}
#endif	// AUDIO_SUPPORT_RECORD




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
 *                  A standard IO function is provided by default.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void __attribute__((weak)) AudioIOEnableOutAmp(void)
{
	OpenOC();
	EnableAmp();
}
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
 *                  A standard IO function is provided by default.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_PLAY
void __attribute__((weak)) AudioIODisableOutAmp(void)
{
	CloseOC();			// close the output compare
	DisableAmp();			// turn amp on
}
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
 *                  A standard IO function is provided by default.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
void __attribute__((weak)) AudioIOEnableInAmp(void)
{
#ifdef	ADC_EXCLUSIVE_MODE	
	OpenAdc();	// turn ADC on
#endif
}
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
 * Overview:        This function disables the playback output amplifier.
 *                  The audio library calls this function whenever a record operation stops/pauses.
 *                  The application can provide it's own AudioIODisableInAmp function.
 *                  A standard IO function is provided by default.
 * 		 
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
void __attribute__((weak)) AudioIODisableInAmp(void)
{
#ifdef	ADC_EXCLUSIVE_MODE	
	CloseAdc();		// turn ADC off
#endif
}
#endif


//// low level functions ///////////
//


#if defined(ADC_EXCLUSIVE_MODE) || defined(ADC_EXT_MODE_POINT)
/******************************************************************************
 * Function:        InitAdc
 *
 * PreCondition:    None
 *
 * Input:           sample and conversion times, as multiple of TAD
 *
 * Output:          None 
 *
 * Side Effects:    None.
 *
 * Overview:        This function is just a helper to initialize the ADC
 *
 * Note:            None 
 *****************************************************************************/
#ifdef AUDIO_SUPPORT_RECORD
static void InitAdc(int adcs)
{
#ifdef	ADC_EXCLUSIVE_MODE
	AD1CON1=ADC1_WORD;
	AD1CON2=ADC2_WORD;
	AD1CON3=ADC3_WORD|adcs;
	AD1CHS=(ADC_CH0_NEG_SAMPLEA_NVREF|(AUDIO_ADC_CHN << _AD1CHS_CH0SA_POSITION));
	AD1CSSL=CSS_WORD;
#endif

	mPORTBSetPinsAnalogIn((1<<AUDIO_ADC_CHN));
}
#endif	// AUDIO_SUPPORT_RECORD

#endif


