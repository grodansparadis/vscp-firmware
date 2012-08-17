/*********************************************************************
 *
 *                  AUDIO demo application
 *
 *********************************************************************
 * FileName:        audio_demo_speex_rs232.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB Cxx
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *$Id: $
 ********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <GenericTypeDefs.h>
#include <audio/audio.h>

#include "f_system.def"

#include <plib.h>

#if MEM_PROFILING
	#include "mhook.h"
#endif


#if !defined(AUDIO_SUPPORT_RECORD) && !defined(AUDIO_SUPPORT_PLAY)
	#error "one of AUDIO_SUPPORT_PLAY or AUDIO_SUPPORT_RECORD have to be defined"
#endif


// Configuration Bit settings
//  System Clock = 80 MHz,  Peripherial Bus = 40 MHz
//  Primary Osc w/PLL (XT+,HS+,EC+PLL)
//  Input Divider    2x Divider
//  Multiplier      20x Multiplier
//  WDT disabled
//	Other options are don't care
//

    #pragma config FPLLMUL  = MUL_20    // PLL Multiplier
    #pragma config FPLLIDIV = DIV_2     // PLL Input Divider
    #pragma config FPLLODIV = DIV_1	// PLL Output Divider
    #pragma config FPBDIV   = DIV_2     // PBUS Divider
    #pragma config FWDTEN   = OFF       // Watchdog Timer
    #pragma config POSCMOD  = HS        // Primary Oscillator
    #pragma config FNOSC    = PRIPLL    // Oscillator Selection
    #pragma config CP       = OFF       // Code Protect
    #pragma config BWP      = OFF       // Boot Flash Write Protect
    #pragma config PWP      = OFF       // Program Flash Write Protect

#ifdef USE_USB_MSD
    #pragma config UPLLEN   = ON        // USB PLL Enabled
	#pragma config UPLLIDIV = DIV_2     // USB PLL Input Divider
#endif

#define	SYS_FREQ	80000000	// what we're running at

#define BAUDRATE     	9600		//The desired BaudRate




/*
 * This demo software is designed to run on an board with an RS232 interface (Explorer 16 board with PICTail plus playback board, for ex).
 Start an Hyper terminal on the PC side proir to starting the demo application, with the following settings.
 Baud Rate : 9600
 Data Bits :  8 
 Parity : None 
 Stop Bits: 1
 Flow Control: None
 Connect the PC to the demo board via a UART cable.
 Once you download and start the application it sends some message onto 
 your hyper terminal. It will pop up couple of options to the user.
 The application will  behave according to the command send from the Hyper terminal.
 For Eg; If you send 'p' command then it starts playing the Audio.
 For more information refer the Application note. 
*/


#define menu_str	\
"***    Audio Demo Application     *** \r\n \
*** Press 'p C:\\fname' for Play the file fname  *** \r\n \
*** Press 'r C:\\fname' for Recording the file fname  *** \r\n \
*** Press 's' for Stop the Audio  *** \r\n \
*** Press 'u' for Pause the Audio *** \r\n \
*** Press 'c' for Continue the Audio *** \r\n \
*** Press 'q' for quitting the Audio *** "


// some data
static AUDIO_STATUS			playerStat;		// current status
static FILE*				playerInFile;		// current file played
static FILE*				playerOutFile;		// current file recorded
static char*				playerInName;		// current in file name
static char*				playerOutName;		// current out file name
static int				playerEof;		// current player file ended
volatile int				playerNotify;		// flag telling the player needs attention
static AUDIO_FORMAT			playerFmt;		// playing/recording raw/encoded data

static AUDIO_STREAM_FORMAT		playerStreamType;	// stream format

/*static*/ FILE*			playerDir;		// current directory

int					playerDiscardMode=0;	// to discard recorded data

int					playerProcSamples=1024;	// default number of processed samples

int					playerProgressBytes=5*1024;	// default no of bytes for the codec progress indication 

static char				playerOperation[20];		// ascii name of the current operation

#define					MAX_CMD_LENGTH	50
static char				cmdBuff[MAX_CMD_LENGTH+1];	// buffer to assemble the command
static char*				cmdPnt;				// current character pointer in the cmd buffer
static char*				cmdEnd;				// end of command pointer
static int				cmdContinue;			// command can continue or abort

static char				promptStr[FILE_NAME_SPACE_SIZE];	// prompt string
static int				promptNeed;				// display prompt needed
static int				promptRfsh;				// refresh prompt

static int				currPbDiv;


#define					FILE_ATTRIB_TAB_SZ		12	// space btw file name and attributes
#define					FILE_LENGTH_SZ			9	// characters for file size. >3
static char				fNameBuff[FILE_NAME_SPACE_SIZE+FILE_ATTRIB_TAB_SZ+FILE_LENGTH_SZ];	// process file names buffer
														// also used as message storage for various
														// display functions
static const char*			term="\r\n";			// line terminator
static const char*			msgParamSep=", ";		// message parameter separator
static const char*			tokenSep=", \r\n";		// token separator


typedef void (*pCmdFnc)(void);				// command function

typedef struct
{
	const char*		cmdStr;
	pCmdFnc			cmdFnc;
	const char*		cmdDescr;
}cmdDcpt;		// a simple command descriptor

#ifdef AUDIO_SUPPORT_PLAY
static void		CommandPlay(void);				// play command
static void		CommandPlayRate(void);				// set the play rate
#endif

#ifdef AUDIO_SUPPORT_RECORD
static void		CommandRecord(void);				// record command
static void		CommandRecordRate(void);			// set the record rate
#endif

#if defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)
static void		CommandConvert(void);				// convert data from one format to another
#endif

static void		CommandStop(void);				// stop command
static void		CommandPause(void);				// pause command
static void		CommandContinue(void);				// continue command
static void		CommandAudioFormat(void);			// set the encoding type
static void		CommandAudioInStream(void);			// set the in stream type
static void		CommandAudioOutStream(void);			// set the out stream type
static void		CommandToggleDiscard(void);			// command toggle record discard mode
static void		CommandAudioFreq(void);				// set the audio freq, Hz
static void		CommandProcSamples(void);			// set the number of processing samples
static void		CommandCodecSetProgress(void);			// set the bytes for the codec progress indication
static void		CommandPbDiv(void);				// set the audio freq, KHz
static void		CommandLs(void);				// directory listing
static void		CommandCd(void);				// change directory
static void		CommandMd(void);				// make directory
static void		CommandRmd(void);				// remove directory
static void		CommandRm(void);				// remove file
static void		CommandHelp(void);				// help
static void		CommandOverrun(void);				// get the overrun count
static void		CommandQuit(void);				// command quit
static void		CommandDmaEnable(void);				// enable/disable DMA
static void		CommandDmaChannel(void);			// set/get the DMA channel

#if PROFILING
static void		CommandProfCycles(void);
static void		CommandProfTime(void);
static void		CommandProfOverhead(void);
static void		CommandAProfIo(void);
#endif // PROFILING

#if MEM_PROFILING
static void		CommandMProfStatReset(void);
static void		CommandMProfPoolStat(void);
static void		CommandMProfDynStat(void);
#endif

// added speex commands
#ifdef SPEEX_ENCODING

static void		CommandSpxSetNb(void);				// set speex narrow band mode
static void		CommandSpxSetWb(void);				// set speex wide band mode
#ifdef AUDIO_SUPPORT_RECORD
static void		CommandSpxSetComplexity(void);			// set speex encoding complexity
static void		CommandSpxSetQuality(void);			// set speex encoding quality
static void		CommandSpxSetFrames(void);			// set speex encoding frames per packet
static void		CommandSpxSetPackets(void);			// set speex encoding packets per page
static void		CommandSpxSetBitrate(void);			// set speex encoding bitrate
#endif

#ifdef AUDIO_SUPPORT_PLAY
static void		CommandSpxSetPrcptEnh(void);			// turn on/off speex perceptual enhancement
#endif

#endif	// SPEEX_ENCODING


static const cmdDcpt	cmdTbl[]=
{
#ifdef AUDIO_SUPPORT_PLAY
	{"p", CommandPlay, ": play file"},
#endif
#ifdef AUDIO_SUPPORT_RECORD
	{"r", CommandRecord, ": record file"},
#endif
	{"s", CommandStop, ": stop"},
	{"u", CommandPause, ": pause"},
	{"c", CommandContinue, ": continue"},
	{"afmt", CommandAudioFormat, ": set the audio encoding format: raw, adpcm, speex"},
	{"istream", CommandAudioInStream, ": set the audio in stream: wave, free, query"},
	{"ostream", CommandAudioOutStream, ": set the audio in stream: wave, free"},
	{"tdr", CommandToggleDiscard, ": toggle discard record"},
#ifdef AUDIO_SUPPORT_PLAY
	{"pr", CommandPlayRate, ": set play rate"}, 
#endif
#ifdef AUDIO_SUPPORT_RECORD
	{"rr", CommandRecordRate, ": set record rate"}, 
#endif
	{"afreq", CommandAudioFreq, ": set audio frequency, Hz"},
	{"asamples", CommandProcSamples, ": set/get number of processing samples"},
	{"c-prg", CommandCodecSetProgress, ": set codec progress indicator, bytes"},
	{"pbd", CommandPbDiv, ": set peripheral bus frequency divider, 0-3"},
	{"ls", CommandLs, ": directory list"},
	{"cd", CommandCd, ": directory change"},
	{"md", CommandMd, ": directory make"},
	{"rmd", CommandRmd, ": directory remove"},
	{"rm", CommandRm, ": file remove"},	
	{"h", CommandHelp,": help"},
	{"ovr", CommandOverrun, ": returns the play/record overrun count"},
	{"q", CommandQuit, ": quit"},
	{"dma-en", CommandDmaEnable, ": enables/disables the DMA usage, 0/1"},
	{"dma-chn", CommandDmaChannel, ": sets/gets the DMA settings - chn, pri, ipl"},
#if defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)
	{"convert", CommandConvert, ": converts data - infile, outfile"},
#endif
	// profiling stuff
#if PROFILING
	{"prof-c", CommandProfCycles, ": cycle profiling statistics (cycles)"},
	{"prof-t", CommandProfTime, ": time profiling statistics (msec)"},
	{"prof-ovhd", CommandProfOverhead, ": overhead profiling statistics (%)"},
	{"prof-io", CommandAProfIo, ": audio profile io on/off"},
#endif // PROFILING

#if MEM_PROFILING
	{"mprof-clr", CommandMProfStatReset, ": cleared memory profiling pool statistics"},
	{"mprof-pool", CommandMProfPoolStat, ": memory profiling pool statistics"},
	{"mprof-dyn", CommandMProfDynStat, ": memory profiling dynamic statistics"},
#endif

	// Speex specific stuff
#ifdef SPEEX_ENCODING
	{"spx-nb", CommandSpxSetNb, ": set speex library in narrow band mode"},
	{"spx-wb", CommandSpxSetWb, ": set speex library in wide band mode"},
#ifdef AUDIO_SUPPORT_RECORD
	{"spx-cplx", CommandSpxSetComplexity, ": set speex encoder complexity factor, 1-10"},
	{"spx-qual", CommandSpxSetQuality, ": set speex encoder quality factor, 0-10"},
	{"spx-frames", CommandSpxSetFrames, ": set speex encoder frames per packet, 1-10"},
	{"spx-pkts", CommandSpxSetPackets, ": set speex encoder packets per page, 1-255"},
	{"spx-rate", CommandSpxSetBitrate, ": set speex encoder bit rate, bps"},
#endif
#ifdef AUDIO_SUPPORT_PLAY
	{"spx-enh", CommandSpxSetPrcptEnh, ": speex turn decoder perceptual enhancement on/off"},
#endif	
#endif	// SPEEX_ENCODING	

	
};


typedef struct
{
	AUDIO_FORMAT	aFmt;		// player/recorder audio format
	const char*	fmtStr;		// format string
}fmtDcpt;	// format descriptor


static const fmtDcpt	playerFmtTbl[]=
{
#ifdef PCM_ENCODING
	{AUDIO_PCM,		"pcm"},
#endif
#ifdef ADPCM_ENCODING
	{AUDIO_ADPCM,		"adpcm"},
#endif
#ifdef SPEEX_ENCODING
	{AUDIO_SPEEX,		"speex"},
#endif
};	// supported formats



typedef struct
{
	AUDIO_STREAM_FORMAT	sFmt;		// player/recorder audio format
	const char*		streamStr;	// stream string
}streamDcpt;	// format descriptor

static const streamDcpt	playerStreamTbl[]=
{
#ifdef FREE_STREAMS
	{AUDIO_STREAM_FREE,	"free"},
#endif
#ifdef WAVE_STREAMS
	{AUDIO_STREAM_WAV,	"wave"},
#endif
#ifdef SPEEX_OGG_STREAMS
	{AUDIO_STREAM_OGG_SPEEX, "speex-ogg"},
#endif

	{AUDIO_STREAM_QUERY,	"query"},
};	// supported streams


#define	COMMAND_DEPTH	4

typedef struct _tagCmdNode
{
	struct _tagCmdNode*	next;
	struct _tagCmdNode*	prev;
	char	cmdBuff[MAX_CMD_LENGTH+1];	// command itself
}cmdNode;	// simple command history	
	

typedef struct
{
	cmdNode*	head;
	cmdNode*	tail;
}cmdDlList;

static cmdDlList	cmdList={0, 0};		// root of the command history

static cmdNode*		pCurrCmdN;		// history pointer

#define	ESC_SEQ_SIZE	2
static const char	seqUpArrow[]="[A";
static const char	seqDownArrow[]="[B";
static const char	seqRightArrow[]="[C";
static const char	seqLeftArrow[]="[D";	// standard VT100 escape sequences


#if PROFILING

	static unsigned int		tProfStart=0;
	static unsigned long long	tProfCycles;
	static unsigned int		iProfStart=0;
	static unsigned long long	iProfCycles;

	extern __inline__ void __attribute((always_inline)) tiProfilingStart(void)
	{
		iProfStart=tProfStart=ReadCoreTimer();
	}

	extern __inline__ void __attribute((always_inline)) tProfilingStart(void)
	{
		tProfStart=ReadCoreTimer();
	}

	extern __inline__ void __attribute((always_inline)) iProfilingStart(void)
	{
		iProfStart=ReadCoreTimer();
	}


	extern __inline__ void __attribute((always_inline)) tiProfilingStop(void)
	{
		unsigned int currTicks=ReadCoreTimer();
		tProfCycles+=(currTicks-tProfStart);
		iProfCycles+=(currTicks-iProfStart);			
	}


	extern __inline__ void __attribute((always_inline)) tProfilingRfshIStop(void)
	{
		unsigned int currTicks=ReadCoreTimer();
		tProfCycles+=(currTicks-tProfStart);
		iProfCycles+=(currTicks-iProfStart);
		tProfStart=currTicks;			
	}


	extern __inline__ void __attribute((always_inline)) tProfilingStop(void)
	{
		tProfCycles+=(ReadCoreTimer()-tProfStart);
	}

	extern __inline__ void __attribute((always_inline)) tProfilingInit(void)
	{
		tProfCycles=iProfCycles=0;
	}
	
	unsigned __inline__ long long __attribute((always_inline)) tProfilingCycles(void)
	{
		return tProfCycles*2;
	}

	unsigned __inline__ long long __attribute((always_inline)) iProfilingCycles(void)
	{
		return iProfCycles*2;
	}

	#define	ProfilingTIStart()		do{AudioProfilingInit(); tProfilingInit(); tiProfilingStart();}while(0)
	#define	ProfilingTStart()		do{AudioProfilingInit(); tProfilingInit(); tProfilingStart();}while(0)
	#define	ProfilingTIStop()		do{tiProfilingStop();}while(0)
	#define	ProfilingTStop()		do{tProfilingStop();}while(0)
	#define ProfilingTIResume()		do{tiProfilingStart();}while(0)
	#define	ProfilingTRefreshIStop()	do{tProfilingRfshIStop();}while(0)
	#define	ProfilingIResume()		do{iProfilingStart();}while(0)


#else

	#define	ProfilingTIStart()
	#define	ProfilingTStart()
	#define	ProfilingTIStop()
	#define	ProfilingTStop()
	#define	ProfilingTIResume()
	#define	ProfilingTRefreshIStop()
	#define	ProfilingIResume()
#endif // PROFILING




// local proto

static int		DemoReadCB(unsigned char* samplesBuff, int bSz);	// callback function to fill a samples buffer
static int		DemoWriteCB(unsigned char* samplesBuff, int nSamples);	// callback function for user to store an encoded samples buffer
static int		DemoOutSeekCB(long offset, ORIGIN_SEEK origin);		// callback for re-positioning the pointer in a stream
static int		DemoInSeekCB(long offset, ORIGIN_SEEK origin);		// callback for re-positioning the pointer in a stream

static void		DemoSetFlag(void);					// callback function to notify that audio library function has to be called


static int		DemoInit(void);					// demo resources init
static void		DemoCleanup(void);				// demo resources clean-up
static int		ProcessCmd(void);				// processing a serial line command
static void		ParseCmdBuffer(char* str);			// parse the command buffer

static void		ProcessEscSequence(void);			// process an escape sequence


static void		DisplayPrompt(void);


static void		DisplayMsgParams(const char* str, int params, ...);

static void		DisplayMsgOpenErr(const char* fName);

#if defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)
static void		DisplayMsgCloseErr(const char* fName);
static void		DisplayMsgSuccess(const char* operation);
static void		DisplayProgress(int byteCnt);		// progress indication
#endif

#ifdef AUDIO_SUPPORT_PLAY
static void		DisplayMsgEmptyErr(const char* fName);
#endif

static void		DisplayMsgNoFName(void);

static void		DisplayMsgRdErr(const char* fName);

static void		DisplayMsgWrErr(const char* fName);

static void		DisplayMsgMemErr(void);

static void		DisplayMsgDataErr(const char* fName);

static void		DisplayMsgBuffErr(const char* bName);

static void		DisplayUnsuportErr(void);

static void		DisplayMsgUnknown(void);

static void		DisplayMsgNoCmd(void);

static void		DisplayMsgGenErr(void);

static int		GetCmdIntParam(void);

static char*		GetCmdStrParam(void);

static void		itoa(int n, char s[]);				// local implementation

static void		lltoa(unsigned long long n, char s[]);		// local implementation

static void		SetAudioStream(int isOut);			// set the stream type

static void		CloseAudioInFile(void);
	
static void		CloseAudioOutFile(void);

static void		CloseAudioFiles(void);			// close the currently open files

static void		DisplayOperationResult(AUDIO_RES res);

static void		printLineMsg(const char* str);

static void		printMsg(const char* str);


static void		DisplayMsgLLParams(const char* str, int params, ...);

static void		CmdAddHead(cmdDlList* pL, cmdNode* pN);
static cmdNode*		CmdRemoveTail(cmdDlList* pL);

static void		DisplayNodeMsg(cmdNode* pNext);

#ifdef	USE_USB_MSD
void            MonitorMedia( void );            // USB Mass Storage Media
void		WaitMs(int nMs);		// wait a number of ms

// USB MSD Media data //////////////////
typedef struct _VOLUME_INFO
{
    char        label[12];
    BYTE        valid;
} VOLUME_INFO;

VOLUME_INFO         volume;
BYTE                mediaPresent;
// USB MSD Media data //////////////////

#endif	// USE_USB_MSD


int main(void)
{
	AUDIO_RES	res;
	int		ix;
	
	SYSTEMConfigPerformance(SYS_FREQ);	// configure the proper PB frequency and the number of wait states
	currPbDiv=mOSCGetPBDIV();				// read current PB divider					

	playerInFile=playerOutFile=0;
	playerFmt=(AUDIO_FORMAT)0;			// default audio format
	playerStreamType=(AUDIO_STREAM_FORMAT)0;	// default stream format


	if(!DemoInit())			// init resources
	{
		return 0;
	}
	

	cmdPnt=cmdEnd=cmdBuff;

	promptNeed=promptRfsh=1;

	// construct the command history list
	
	for(ix=0; ix<COMMAND_DEPTH; ix++)
	{
		cmdNode* pN;
		pN=(cmdNode*)malloc(sizeof(*pN));

		if(!pN)
		{
			DisplayMsgMemErr();
			return 0;
		}
		pN->cmdBuff[0]='\0';
		CmdAddHead(&cmdList, pN);
	}
	pCurrCmdN=0;
	
	while(ProcessCmd())
	{
	#ifdef	USE_USB_MSD
        	MonitorMedia();
	#endif
		if(playerNotify)
		{	// time to do smth...
			ProfilingTRefreshIStop();	// just to avoid core timer overrun				
			
			playerNotify=0;
			switch(playerStat)
			{
			#ifdef AUDIO_SUPPORT_PLAY
				case PLAY:
					res=AudioPlay();
					break;
			#endif
			#ifdef AUDIO_SUPPORT_RECORD
				case RECORD:
					res=AudioRecord();
					break;
			#endif
				default:
					res=AUDIO_RES_OK;
					break;
			}
			ProfilingIResume();

			if(res!=AUDIO_RES_OK)
			{
				ProfilingTIStop();
				promptNeed=1;
				if(playerStat==PLAY && res==AUDIO_RES_DONE)
				{
					playerStat=STOP;
					CloseAudioInFile();
					printLineMsg("*** The audio is stopped now. *** ");
				}
				else
				{
					DisplayOperationResult(res);
					CommandStop();
				}
			}
		}
	}

	DemoCleanup();

	return 1;

}


// init demo resources
static int DemoInit(void)
{
	int	pbFreq;
	
	if(playerInFile || playerOutFile)
	{
		AudioStop();
		CloseAudioFiles();
	}

	playerStat=STOP;
	playerDir=0;


	mOSCSetPBDIV(currPbDiv);		// set proper PB frequency     

	pbFreq=SYS_FREQ/(1<<mOSCGetPBDIV());
	
	OpenUART2(UART_EN, UART_RX_ENABLE|UART_TX_ENABLE, (pbFreq/16/(BAUDRATE))-1);


	if(AudioInit(SYS_FREQ, DemoReadCB, DemoWriteCB, DemoSetFlag)!=AUDIO_RES_OK)
	{
		printLineMsg("*** Cannot initialize the Audio Library. Aborting....*** ");
		return 0;
	}

	printLineMsg(menu_str);
	
	AudioSetCallback(AUDIO_CB_SEEK_IN, DemoInSeekCB);
	AudioSetCallback(AUDIO_CB_SEEK_OUT, DemoOutSeekCB);
	
	playerFmt=(AUDIO_FORMAT)0;		// first supported format
	AudioSetEncoding(playerFmt);

	
	INTEnableSystemMultiVectoredInt();		// Enable Multi-vectored interrupts.



#ifdef USE_USB_MSD
	
	// Initialize USB layers
	if (USBHostInit())
	{
		printLineMsg("*** Cannot initialize the USB Host. Aborting....*** ");
		return 0;
	}
   
	if (USBHostMSDInit())
	{
		printLineMsg("*** Cannot initialize the USB Host MSD. Aborting....*** ");
		return 0;
	}

	// Initialize IO pins used to control VBus power chip (5v, VBus on).
	#ifdef USB_HOST_PIM
		mPORTBSetPinsDigitalIn(VBUS_POWER_GOOD);
		mPORTBClearBits(VBUS_SELECT_3V3);
		mPORTBSetBits(VBUS_SHUTDOWN_N);
		mPORTBSetPinsDigitalOut(VBUS_SELECT_3V3|VBUS_SHUTDOWN_N);
	#else
		mPORTCClearBits(BIT_4);
		mPORTCSetPinsDigitalOut(BIT_4);
   
		mPORTGSetBits(VBUS_SHUTDOWN_N);
		mPORTGSetPinsDigitalOut(VBUS_SHUTDOWN_N);
	#endif // USB_HOST_PIM
	
#else	// FAT16
	MSTimerInit(pbFreq);
	if (!FSInit(pbFreq))
	{
		printLineMsg("*** Cannot initialize the FAT16 Library. Aborting....*** ");
		return 0;
	}
	
#endif	// USE_USB_MSD/FAT16


	playerNotify=0;
	
	return 1;
	
}


// define the sampling routine for the Audio Library
SETUP_AUDIO_SAMPLE_INT();



// exit clean-up
static void DemoCleanup(void)
{
	CloseUART2(); /* Close the serial port */
}



#ifdef	USE_USB_MSD

/*******************************************************************************
Function:       void MonitorMedia( void )

Precondition:   None

Overview:       This function calls the background tasks necessary to support
                USB Host operation.  Upon initial insertion of the media, it
                initializes the file system support and reads the volume label.
                Upon removal of the media, the volume label is marked invalid.

Input:          None

Output:         None
*******************************************************************************/

void MonitorMedia( void )
{
    BYTE            mediaPresentNow;
    BYTE            mountTries;
    SearchRec       searchRecord;

    USBHostTasks();
    USBHostMSDTasks();

    mediaPresentNow = USBHostMSDSCSIMediaDetect();
    if (mediaPresentNow != mediaPresent)
    {
        if (mediaPresentNow)
        {
            mountTries = 10;
            while(!FSInit() && mountTries--);
            if (!mountTries)
            {
                printLineMsg( "APP: Could not mount media" );
                mediaPresent = FALSE;
            }
            else
            {
                mediaPresent = TRUE;

                // Find the volume label.  We need to do this here while we are at the
                // root directory.
                if (!FindFirst( "*.*", ATTR_VOLUME, &searchRecord ))
                {
                    strcpy( volume.label, searchRecord.filename );
                    volume.valid = TRUE;
                }
            }
        }
        else
        {
            mediaPresent = FALSE;
            volume.valid = FALSE;
        }
    }
}

#endif	// USE_USB_MSD


// callback function for user to provide a samples buffer
// in this demo we just provide our own buffer, for now.
static int DemoReadCB(unsigned char* samplesBuff, int bSize)
{
	int	nChars=0;

	if(playerInFile)
	{
		nChars=fread(samplesBuff, 1, bSize, playerInFile);
	}
	
	if(!nChars)
	{	// done the input file...
		playerEof=1;
	}
	
	return nChars;
}



// callback function for user to store an encoded samples buffer
static int DemoWriteCB(unsigned char* samplesBuff, int nSamples)
{
	return playerOutFile?fwrite(samplesBuff, 1, nSamples, playerOutFile):nSamples;
}


static int DemoOutSeekCB(long offset, ORIGIN_SEEK origin)
{
	return playerOutFile?!fseek(playerOutFile, offset, origin==ORIGIN_CURR?SEEK_CUR:origin==ORIGIN_EOF?SEEK_END:SEEK_SET):1;
}

static int DemoInSeekCB(long offset, ORIGIN_SEEK origin)
{
	return !fseek(playerInFile, offset, origin==ORIGIN_CURR?SEEK_CUR:origin==ORIGIN_EOF?SEEK_END:SEEK_SET);
}


// callback function to notify that audio library function has to be called
static void DemoSetFlag(void)
{
	playerNotify=1;		// just set a variable
}


// command to play a file
#ifdef AUDIO_SUPPORT_PLAY
static void CommandPlay(void)
{

	if(playerStat!=STOP)
	{
		printLineMsg("*** Could not play. Use STOP first...*** ");
	}
	else
	{
		playerInName=GetCmdStrParam();
		
		if(!playerInName)
		{
			DisplayMsgNoFName();
		}
		else
		{	// open the file, get the fp, it will be used in the read CB
			if(!(playerInFile=fopen(playerInName, "r")))
			{
				DisplayMsgOpenErr(playerInName);
			}
			else if(!playerInFile->size)
			{
				CloseAudioInFile();
				DisplayMsgEmptyErr(playerInName);
			}
			else
			{
				AUDIO_RES res;
				AudioSetProcSamples(playerProcSamples);
				
				ProfilingTIStart();

				res=AudioPlay();	// start the play
				if(res==AUDIO_RES_OK)
				{
					playerStat=PLAY;
					playerEof=0;
					printLineMsg("*** Playing the audio *** ");
					strncpy(playerOperation, "Playing ", sizeof(playerOperation));					
				}
				else
				{
					ProfilingTIStop();
					CloseAudioInFile();
					DisplayOperationResult(res);
				}
			}
		}
	}
}
#endif	// AUDIO_SUPPORT_PLAY


// command to record a file
#ifdef AUDIO_SUPPORT_RECORD
static void CommandRecord(void)
{
	AUDIO_RES	res;
	
	if(playerStat!=STOP)
	{
		printLineMsg("*** Could not play. Use STOP first...*** ");
		return;
	}

	if(!playerDiscardMode)
	{
		playerOutName=GetCmdStrParam();
	
		if(!playerOutName)
		{
			DisplayMsgNoFName();
		}
		else
		{	// open the file, get the fp, it will be used in the write CB
			if(!(playerOutFile=fopen(playerOutName, "w")))
			{
				DisplayMsgOpenErr(playerOutName);
			}
		}

		if(!playerOutFile)
		{
			return;
		}
	}
	

	AudioSetProcSamples(playerProcSamples);

	ProfilingTIStart();
				
       	res=AudioRecord();	// start the recording
	if(res==AUDIO_RES_OK)
	{
		playerStat=RECORD;
		printLineMsg("*** Start recording. *** ");
		strncpy(playerOperation, "Recording ", sizeof(playerOperation));
	}
	else
	{
		ProfilingTIStop();
		CloseAudioOutFile();
		DisplayOperationResult(res);		
	}
}
#endif	// AUDIO_SUPPORT_RECORD



// stop
static void CommandStop(void)
{
	AudioStop();
	
	ProfilingTIStop();
	
	CloseAudioFiles();
	
	playerStat=STOP;
	printLineMsg("*** Stopped the audio *** ");
	
}

// pause
static void CommandPause(void)
{
	ProfilingTIStop();
	
	if(playerStat!=PLAY && playerStat!=RECORD)
	{
		printLineMsg("*** Could not pause. Use PLAY/RECORD first...*** ");
	}
	else
	{
		AudioPause();
		playerStat=playerStat==PLAY?PAUSE_PLAY:PAUSE_RECORD;

		printLineMsg("*** Paused the audio *** ");

	}
}

// continue
static void CommandContinue(void)
{
	if(playerStat!=PAUSE_PLAY && playerStat!=PAUSE_RECORD)
	{
		printLineMsg("*** Could not Continue. Use PAUSE first...*** ");
	}
	else
	{
		playerStat=playerStat==PAUSE_PLAY?PLAY:RECORD;
		AudioResume();

		printLineMsg("*** Continued the audio *** ");
		ProfilingTIResume();
 	}
}


static void CommandAudioFormat(void)
{
	int		ix;
	char*		fmtStr=GetCmdStrParam();

	if(fmtStr)
	{
		for(ix=0; ix<sizeof(playerFmtTbl)/sizeof(*playerFmtTbl); ix++)
		{
			if(!strcmp(fmtStr, playerFmtTbl[ix].fmtStr))
			{
				playerFmt=playerFmtTbl[ix].aFmt;
				AudioSetEncoding(playerFmt);
				strcpy(fNameBuff, "Audio format set to ");
				strcat(fNameBuff, fmtStr);
				printLineMsg(fNameBuff);
				return;
			}
		}
	}
			
	printLineMsg("*** Audio player: unsupported format. *** ");
}


static void SetAudioStream(int isOut)
{
#if defined(AUDIO_SUPPORT_PLAY) || defined(AUDIO_SUPPORT_RECORD)
	int		ix;
	char*		streamStr=GetCmdStrParam();
	
	if(streamStr)
	{
		for(ix=0; ix<sizeof(playerStreamTbl)/sizeof(*playerStreamTbl); ix++)
		{
			if(!strcmp(streamStr, playerStreamTbl[ix].streamStr))
			{
				playerStreamType=playerStreamTbl[ix].sFmt;
		#ifdef AUDIO_SUPPORT_RECORD
				if(isOut)
				{
					AudioSetOutStreamFormat(playerStreamType);
					strcpy(fNameBuff, "Audio output stream type set to ");
				}
		#endif
		#ifdef AUDIO_SUPPORT_PLAY
				if(!isOut)
				{
					AudioSetInStreamFormat(playerStreamType);
					strcpy(fNameBuff, "Audio input stream type set to ");
				}
		#endif
				strcat(fNameBuff, streamStr);
				printLineMsg(fNameBuff);
				return;
			}
		}
	}

#endif	// defined(AUDIO_SUPPORT_PLAY) || defined(AUDIO_SUPPORT_RECORD)
	
	printLineMsg("*** Audio player: unsupported stream type. *** ");
}


static void CommandAudioInStream(void)
{

	SetAudioStream(0);
}


static void CommandAudioOutStream(void)
{

	SetAudioStream(1);
}

static void CommandToggleDiscard(void)
{
	playerDiscardMode^=1;
	if(playerDiscardMode)
	{
		printLineMsg("*** Audio player in record discard mode. *** ");
	}
	else
	{
		printLineMsg("*** Audio player in record save mode. *** ");
	}
	
}

#ifdef AUDIO_SUPPORT_PLAY
static void CommandPlayRate(void)
{
	int pRate=GetCmdIntParam();
	
	if(pRate<=0 || pRate>16)
	{
		printLineMsg("*** Invalid play rate. Retry *** ");
	}
	else
	{
		DisplayMsgParams("Player rate changed to ", 1, pRate);
		AudioSetPlayRate(pRate);
	}	
}
#endif

#ifdef AUDIO_SUPPORT_RECORD
static void CommandRecordRate(void)
{
	int rRate=GetCmdIntParam();
	
	if(rRate<=0 || rRate>16)
	{
		printLineMsg("*** Invalid play rate. Retry *** ");
	}
	else
	{
		DisplayMsgParams("Recorder rate changed to ", 1, rRate);		
		AudioSetRecordRate(rRate);
	}	
}
#endif

// audio freq, KHz
static void CommandAudioFreq(void)
{
	int aFreq=GetCmdIntParam();
	
	if(aFreq<=0 || aFreq>64000)
	{
		printLineMsg("*** Invalid audio frequency. Retry *** ");
	}
	else
	{
		DisplayMsgParams("Audio sampling rate changed to ", 1, aFreq);
		AudioSetSamplingFreq(aFreq);
	}	
}


static void CommandProcSamples(void)
{
	int nSamples=GetCmdIntParam();
	
	if(nSamples<0)
	{
		nSamples=AudioGetProcSamples();
		DisplayMsgParams("The current number of processing samples is ", 1, nSamples);
	}
	else
	{
		AudioSetProcSamples(playerProcSamples=nSamples);
		DisplayMsgParams("Updated the processing samples to ", 1, nSamples);
	}		
}

static void CommandCodecSetProgress(void)
{
	int progressBytes=GetCmdIntParam();
	if(progressBytes<0)
	{
		printLineMsg("*** Invalid progress indicator. Retry ***");
	}
	else
	{
		playerProgressBytes=progressBytes;
		DisplayMsgParams("Updated the progress indicator to", 1, progressBytes);		
	}		
}


static void CommandPbDiv(void)
{
	unsigned int pbDiv=GetCmdIntParam();
	
	if(pbDiv<0 || pbDiv>3)
	{
		printLineMsg("*** Invalid PB Divider. Retry ***");
	}
	else
	{
		printLineMsg("*** Invalid PB Divider. Retry ***");	
		currPbDiv=pbDiv;
		if(!DemoInit())		// re-initialize everything
		{
			cmdContinue=0; // failed, exit
		}
	}	
}


static void CommandLs(void)
{
	BOOL	isDir;
	FILE*	currFile;
	int	fIx;
	int	ix;

	if(!playerDir)
	{
		printLineMsg("Could not list the directory contents ");
		return;
	}

	fIx=0;
	currFile=playerDir;
	
	while(currFile)
	{
		if(!GetFileNameIdx(currFile, fIx, &isDir))
		{
			break;	// done
		}
		
		
		strncpy(fNameBuff, currFile->name, FILE_NAME_SPACE_SIZE);

		for(ix=strlen(fNameBuff); ix<FILE_NAME_SPACE_SIZE+FILE_ATTRIB_TAB_SZ; ix++)
		{
			fNameBuff[ix]=' ';	// clear rest of display line
		}
		fNameBuff[ix]=0;	// end string properly
		
		if(isDir)
		{
			strcat(fNameBuff, "<DIR>");
		}
		else
		{
			itoa(currFile->size, fNameBuff+strlen(fNameBuff));
		}
		printLineMsg(fNameBuff);
		fIx++;
	}

	printLineMsg(term);
}


static void CommandCd(void)
{
	FILE*	currDir;	
	char*	dirName;
	BOOL	isDir;	
	int	fIx;
	int	res=0;

	
	dirName=GetCmdStrParam();

	currDir=playerDir;
	fIx=0;
	
	while(currDir && dirName)
	{
		if(!GetFileNameIdx(currDir, fIx, &isDir))
		{
			break;	// done
		}

		if(isDir)
		{
			if(!strcmp(currDir->name, dirName))
			{	// found it
				if(ChangeDirectory(currDir, dirName))
				{
					promptRfsh=1;
					res=1;
				}
				break;	// no matter what		
			}
		}
		fIx++;		
	}

	if(!res)
	{
		printLineMsg("Invalid directory name.");
		return;
	}
}


static void CommandMd(void)
{
	char*	newDirName;
	FILE*	newDir;
	int	res=0;
	
	newDirName=GetCmdStrParam();

	if(newDirName)
	{
		newDir=CreateDirectory(newDirName);
		if(newDir)
		{
			promptRfsh=1;
			res=1;
		}
	}

	
	if(!res)
	{
		printLineMsg("Could not create new directory.");
		return;
	}
}

static void CommandRmd(void)
{
	char*	delDirName;
	int	res=0;
	
	delDirName=GetCmdStrParam();

	if(delDirName)
	{
		res=EraseDirectory(playerDir, delDirName);
	}

	
	if(!res)
	{
		printLineMsg("Could not remove the directory.");
		return;
	}
}

static void CommandRm(void)
{
	char*	delFName;
	int	res=0;
	
	delFName=GetCmdStrParam();

	if(delFName)
	{
		res=FSremove(delFName);
	}

	
	if(res)
	{
		printLineMsg("Could not remove the file.");
		return;
	}
}

static void CommandHelp(void)
{
	int 			ix;
	const cmdDcpt*	pDcpt;
	
	for(ix=0, pDcpt=cmdTbl; ix<sizeof(cmdTbl)/sizeof(*cmdTbl); ix++, pDcpt++)
	{
		printMsg("  *** ");
		printMsg(pDcpt->cmdStr);
		printMsg(pDcpt->cmdDescr);
		printLineMsg("***");
	}
	printLineMsg("");

}


static void CommandOverrun(void)
{
	DisplayMsgParams("Audio player overrun count: ", 1, AudioGetOverrun());
}


// quit
static void CommandQuit(void)
{
	AudioClose();

	CloseAudioFiles();
	

	printLineMsg("*** Quitting the Audio player *** ");
	cmdContinue=0;		// done
}


// enable/disable DMA
static void CommandDmaEnable(void)
{
	int dmaEnabled=GetCmdIntParam();
	
	if(dmaEnabled<0)
	{
		dmaEnabled=AudioGetDmaEnable();
	}
	else
	{
		AudioSetDmaEnable(dmaEnabled);
	}

	if(dmaEnabled)
	{
		printLineMsg("Audio library: DMA transfer is on.");
	}
	else
	{
		printLineMsg("Audio library: DMA transfer is off.");
	}		
}

// set/get the DMA channel
static void CommandDmaChannel(void)
{
	int	dmaEnabled, dmaChn, dmaPri, dmaIpl;

	dmaChn=GetCmdIntParam();

	if(dmaChn<0)
	{
		dmaEnabled=AudioGetDmaChannel(&dmaChn, &dmaPri, &dmaIpl);
	}
	else
	{
		dmaPri=GetCmdIntParam();
		dmaIpl=GetCmdIntParam();
		if(dmaPri<0 || dmaIpl<0)
		{
			printLineMsg("Audio DMA: please enter valid params for DMA pri and IPL.");
			return;
		}
	
		AudioSetDmaChannel(dmaChn, dmaPri, dmaIpl);
		dmaEnabled=1;
	}

	if(dmaEnabled)
	{
		printLineMsg("Audio library: DMA transfer is on.");
	}
	else
	{
		printLineMsg("Audio library: DMA transfer is off.");		
	}
	
	DisplayMsgParams("Audio DMA chn, pri, ipl: ", 3, dmaChn, dmaPri, dmaIpl);			
		
}

// convert data from one format to another
#if defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)
static void CommandConvert(void)
{
	progressDcpt	prgDcpt;
	AUDIO_RES	res;

	if(playerStat!=STOP)
	{
		printLineMsg("*** Could not convert. Use STOP first...*** ");
	}

	playerInFile=playerOutFile=0;
	
	playerInName=GetCmdStrParam();

	if(!playerInName)
	{
		DisplayMsgNoFName();
	}
	else
	{
		if(!(playerInFile=fopen(playerInName, "r")))
		{
			DisplayMsgOpenErr(playerInName);
		}
		else if(!playerInFile->size)
		{
			DisplayMsgEmptyErr(playerInName);
		}
	}
	
	if(!playerInFile)
	{
		return;
	}
	
	if(!playerDiscardMode)
	{
		playerOutName=GetCmdStrParam();
		if(!playerOutName)
		{
			DisplayMsgNoFName();
		}
		else
		{
			if(!(playerOutFile=fopen(playerOutName, "w")))	
			{
				DisplayMsgOpenErr(playerOutName);
			}
		}

		if(!playerOutFile)
		{
			return;
		}
	}



	// success
	prgDcpt.progressStep=playerProgressBytes;
	prgDcpt.progressFnc=DisplayProgress;

	strncpy(playerOperation, "Converting ", sizeof(playerOperation));
	ProfilingTStart();
	res=AudioConvert(&prgDcpt);
	ProfilingTStop();
	
	if(res!=AUDIO_RES_OK)
	{
		DisplayOperationResult(res);
	}
	else if(playerOutFile)
	{
		if(fclose(playerOutFile))
		{
			res=AUDIO_RES_WRITE_ERROR;
			DisplayMsgCloseErr(playerOutName);
		}
		playerOutFile=0;
	}

	if(res==AUDIO_RES_OK)
	{
		DisplayMsgSuccess(playerOperation);		
	}

	CloseAudioFiles();
}
#endif	// defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)



#if PROFILING
static void CommandProfCycles(void)
{
	DisplayMsgLLParams("Total cycles:                   ", 1, tProfilingCycles());
	DisplayMsgLLParams("Idle cycles:                    ", 1, iProfilingCycles());
	DisplayMsgLLParams("Audio main cycles:              ", 1, AudioProfilingCycles());	
	DisplayMsgLLParams("Audio interrupt cycles:         ", 1, AudioProfilingIntCycles());
	DisplayMsgLLParams("Audio interrupt overlap cycles: ", 1, AudioProfilingOvlapCycles());
}

static void CommandProfTime(void)
{
	float totTime=(float)tProfilingCycles()/(SYS_FREQ/1000);
	float idleTime=(float)iProfilingCycles()/(SYS_FREQ/1000);
	float mainTime=(float)(AudioProfilingCycles())/(SYS_FREQ/1000);
	float intTime=(float)AudioProfilingIntCycles()/(SYS_FREQ/1000);
	float ovTime=(float)AudioProfilingOvlapCycles()/(SYS_FREQ/1000);
	
	DisplayMsgParams("Total time (msec):            ", 1, (int)(totTime+0.5));
	DisplayMsgParams("Idle time (msec):             ", 1, (int)(idleTime+0.5));
	DisplayMsgParams("Audio main time:              ", 1, (int)(mainTime+0.5));
	DisplayMsgParams("Audio interrupt time:         ", 1, (int)(intTime+0.5));
	DisplayMsgParams("Audio interrupt overlap time: ", 1, (int)(ovTime+0.5));
}


static void CommandProfOverhead(void)
{
	unsigned long long	totCycles=tProfilingCycles();
	unsigned long long	idleCycles=iProfilingCycles();
	unsigned long long	mainCycles=AudioProfilingCycles();
	unsigned long long	intCycles=AudioProfilingIntCycles();
	unsigned long long	ovCycles=AudioProfilingOvlapCycles();

	
	if(totCycles)
	{
		float	totPerc=(float)(mainCycles+intCycles-ovCycles)*100.0/totCycles;
		float	mainPerc=(float)(mainCycles)*100.0/totCycles;
		float	idlePerc=(float)(idleCycles)*100.0/totCycles;
		float	intPerc=(float)(intCycles)*100.0/totCycles;
		float	intAdjPerc=(float)(intCycles-ovCycles)*100.0/totCycles;		


		DisplayMsgParams("Audio total overhead ((main+int-ovlp)/tot) (%):     ", 1, (int)(totPerc+0.5));
		DisplayMsgParams("Audio main overhead (main/tot):                     ", 1, (int)(mainPerc+0.5));
		DisplayMsgParams("Idle percentage (idle/tot):                         ", 1, (int)(idlePerc+0.5));
		DisplayMsgParams("Audio interrupt overhead (int/tot):                 ", 1, (int)(intPerc+0.5));
		DisplayMsgParams("Audio interrupt adjusted overhead ((int-ovlp)/tot): ", 1, (int)(intAdjPerc+0.5));		
	}
	else
	{
		printLineMsg("No data available to print the audio overhead statistics");
	}
}



static void CommandAProfIo(void)
{
	int profIo=GetCmdIntParam();

	if(profIo<0)
	{
		profIo=AudioGetProfileIo();
	}
	else
	{
		AudioSetProfileIo(profIo);
	}
	
	if(profIo)
	{
		printLineMsg("Audio I/O profiling on");
	}
	else
	{
		printLineMsg("Audio I/O profiling off");
	}
}	

#endif // PROFILING



#if MEM_PROFILING
static void CommandMProfStatReset(void)
{
	if(MemHookStatReset())
	{
		printLineMsg("Mem profiling statistics cleared");
	}
	else
	{
		printLineMsg("Error when clearing Mem profiling statistics");
	}

}

static void CommandMProfPoolStat(void)
{
	sMPoolStat	poolStat;

	MemHookPoolStat(&poolStat);

	DisplayMsgParams("Number of slots in the pool:            ", 1, MemHookPoolSlots());

	DisplayMsgParams("Current pool depth:                     ", 1, poolStat.mHkPoolCurrDepth);
	DisplayMsgParams("Maximum pool depth:                     ", 1, poolStat.mHkPoolMaxDepth);
	DisplayMsgParams("Failed recording pool slots:            ", 1, poolStat.mHkPoolFailCnt);
	DisplayMsgParams("Failed recording pool bytes:            ", 1, poolStat.mHkPoolFailBytes);
}

static void CommandMProfDynStat(void)
{
	sMDynStat	dynStat;

	MemHookDynStat(&dynStat);

	DisplayMsgParams("Max dynamic memory request:             ", 1, dynStat.dynMaxMem);
	DisplayMsgParams("Current dynamic memory request:         ", 1, dynStat.dynCurrMem);
	DisplayMsgParams("Failed dynamic memory requests:         ", 1, dynStat.dynMemFailCnt);
	DisplayMsgParams("Failed dynamic memory bytes:            ", 1, dynStat.dynMemFailBytes);

	DisplayMsgParams("Stack depth bytes:                      ", 1, MemHookStackAlloc());
	
}



#endif



// added speex stuff
//

#ifdef SPEEX_ENCODING


static void CommandSpxSetNb(void)
{
	if(AudioSetCodecBand(AUDIO_BAND_NARROW)==AUDIO_RES_OK)
	{
		printLineMsg("Speex codec in narrow band mode now.");
	}
}


static void CommandSpxSetWb(void)
{
	if(AudioSetCodecBand(AUDIO_BAND_WIDE)==AUDIO_RES_OK)
	{	
		printLineMsg("Speex codec in wide band mode now.");
	}
	else
	{
		printLineMsg("Speex codec does not support wide band mode.");	
	}
}


#ifdef AUDIO_SUPPORT_RECORD
static void CommandSpxSetComplexity(void)
{
	int cmplx=GetCmdIntParam();
	if(cmplx<1 || cmplx>10)
	{
		printLineMsg("*** Invalid complexity required. Retry ***");
	}
	else
	{
		if(AudioSetEncoderComplexity(cmplx)==AUDIO_RES_OK)
		{
			DisplayMsgParams("Speex encoder complexity set to ", 1, cmplx);		
		}
	}
}

static void CommandSpxSetQuality(void)
{
	int qFactor=GetCmdIntParam();
	if(qFactor<0 || qFactor>10)
	{
		printLineMsg("*** Invalid quality factor. Retry ***");
	}
	else
	{
		if(AudioSetEncoderQuality(qFactor)==AUDIO_RES_OK)
		{
			DisplayMsgParams("Speex encoder quality factor set to ", 1, qFactor);
		}
	}
}

static void CommandSpxSetFrames(void)
{
	int nFrames=GetCmdIntParam();
	if(nFrames<0 || nFrames>10)
	{
		printLineMsg("*** Invalid number of frames. Retry ***");
	}
	else
	{
		if(AudioSetEncoderFrames(nFrames)==AUDIO_RES_OK)
		{
			DisplayMsgParams("Speex encoder frames per packet set to ", 1, nFrames);
		}
	}
}

static void CommandSpxSetPackets(void)
{
	int nPackets=GetCmdIntParam();
	if(nPackets<0 || nPackets>255)
	{
		printLineMsg("*** Invalid number of packets. Retry ***");
	}
	else
	{
		if(AudioSetEncoderPackets(nPackets)==AUDIO_RES_OK)
		{
			DisplayMsgParams("Speex encoder packets per page set to ", 1, nPackets);
		}
	}

}

static void CommandSpxSetBitrate(void)
{
	int bRate=GetCmdIntParam();
	if(bRate<0 || bRate>44100)
	{
		printLineMsg("*** Invalid bit rate. Retry ***");
	}
	else
	{
		if(AudioSetEncoderBitrate(bRate)==AUDIO_RES_OK)
		{
			DisplayMsgParams("Speex encoder bit rate set to ", 1, bRate);
		}
	}
}
#endif

// turn on/off speex perceptual enhancement
#ifdef AUDIO_SUPPORT_PLAY
static void CommandSpxSetPrcptEnh(void)
{
	int prcptEnh=GetCmdIntParam();
	
	if(prcptEnh<0)
	{
		printLineMsg("*** Invalid perceptual enhancement. Retry ***");
	}
	else if(AudioSetDecoderEnhancement(prcptEnh)==AUDIO_RES_OK)
	{
		if(prcptEnh)
		{
			printLineMsg("Speex decoder perceptual enhancement on.");
		}
		else
		{
			printLineMsg("Speex decoder perceptual enhancement off.");
		}
	}
}
#endif	// AUDIO_SUPPORT_PLAY

#endif	// SPEEX_ENCODING

///////////////////////////// end of speex stuff //////////////////////////



// processing command received by the serial line
static int ProcessCmd(void)
{
	char	data;

	cmdContinue=1;
	
	DisplayPrompt();
	
	if(DataRdyUART2())
	{	// some data available
		data = (char)ReadUART2(); /* Read data from Rx. */
		if(data=='\n')
		{	// new command assembled
			printMsg(term);
			*cmdPnt=0;	// end the command string
			cmdPnt=cmdEnd=cmdBuff;
			if(strlen(cmdBuff))
			{
				ParseCmdBuffer(cmdBuff);
			}
			else
			{
				DisplayMsgNoCmd();
				promptNeed=1;
			}		
		}
		else if(data=='\b')
		{
			if(cmdPnt>cmdBuff)
			{
				printMsg("\b \b");
				cmdPnt--; cmdEnd--;
			}
		}
		else if(data==0x1b)
		{ // escape sequence
			ProcessEscSequence();
		}	
		else if(cmdEnd-cmdBuff<MAX_CMD_LENGTH)
		{
			putcUART2(data);
			*cmdPnt++=data;
			if(cmdPnt>cmdEnd)
			{
				cmdEnd=cmdPnt;
			}
		}
		else
		{
			printLineMsg("*** Audio player command buffer exceeded. Retry. ***");
			cmdPnt=cmdEnd=cmdBuff;
			promptNeed=1;
		}
	}

	return cmdContinue;
}

static void DisplayPrompt(void)
{
	
	if(promptNeed)
	{
		if(promptRfsh)
		{
			promptRfsh=0;
			// construct prompt string
			if(!playerDir)
			{
				playerDir=GetRootDirectory("C:\\");
			}
			
			if(playerDir && strlen(playerDir->name))
			{
				strncpy(promptStr, playerDir->name, sizeof(promptStr)-1);
			}
			else
			{
				strcpy(promptStr, "/");
			}
			strcat(promptStr, ">");
		}

		printMsg(promptStr);
		promptNeed=0;		
	}
}

// dummy implementation
static void ParseCmdBuffer(char* str)
{
static char	saveCmd[MAX_CMD_LENGTH+1];

	int	ix;
	char*	cmdStr;
	char*	lineStr;

	promptNeed=1;

	strncpy(saveCmd, str, sizeof(saveCmd));		// make a copy of the command
	lineStr=strtok(saveCmd, term);			// remove line term
	
	cmdStr=strtok(str, tokenSep);	
	if(cmdStr)
	{	// ok, there's smth here

		// add it to the history list
		cmdNode* pN=CmdRemoveTail(&cmdList);
		strcpy(pN->cmdBuff, lineStr);
		CmdAddHead(&cmdList, pN);
		pCurrCmdN=0;	
		
		for(ix=0; ix<sizeof(cmdTbl)/sizeof(*cmdTbl); ix++)
		{
			if(!strcmp(cmdStr, cmdTbl[ix].cmdStr))
			{	// command found
				(*cmdTbl[ix].cmdFnc)();		// call command handler
				return;
			}
		}
		
		// command not found
		DisplayMsgUnknown();
	}
	else
	{
		DisplayMsgNoCmd();
	}

}


static void ProcessEscSequence(void)
{
	cmdNode	*pNext;
	char	seq[ESC_SEQ_SIZE+1];
	int	nChar=0;

	while(nChar<ESC_SEQ_SIZE)
	{
		while(!DataRdyUART2());
		seq[nChar++]=ReadUART2();
	}
	seq[nChar]='\0';

	if(!strcmp(seq, seqUpArrow))
	{ // up arrow
		if(pCurrCmdN)
		{
			pNext=pCurrCmdN->next;
			if(pNext==cmdList.head)
			{
				return;	// reached the end of list
			}
		}
		else
		{
			pNext=cmdList.head;
		}
		
		DisplayNodeMsg(pNext);
	}
	else if(!strcmp(seq, seqDownArrow))
	{ // down arrow
		if(pCurrCmdN)
		{
			pNext=pCurrCmdN->prev;
			if(pNext!=cmdList.tail)
			{
				DisplayNodeMsg(pNext);
			}
		}
	}
	else if(!strcmp(seq, seqRightArrow))
	{ // right arrow
		if(cmdPnt<cmdEnd)
		{
			printMsg(seqRightArrow);	
			cmdPnt++;
		}
	}	
	else if(!strcmp(seq, seqLeftArrow))
	{ // left arrow
		if(cmdPnt>cmdBuff)
		{
			cmdPnt--;
			printMsg(seqLeftArrow);
		}
	}	
	else
	{
		DisplayMsgUnknown();
		promptNeed=1;
	}
	
}


static void DisplayNodeMsg(cmdNode* pNext)
{
	int	oCmdLen, nCmdLen;
	
	if((nCmdLen=strlen(pNext->cmdBuff)))
	{	// something there
		oCmdLen=cmdEnd-cmdBuff;
		while(oCmdLen>nCmdLen)
		{
			printMsg("\b \b");		// clear the old command
			oCmdLen--;
		}
		while(oCmdLen--)
		{
			printMsg("\b");			
		}
		strcpy(cmdBuff, pNext->cmdBuff);
		printMsg(cmdBuff);
		cmdPnt=cmdEnd=cmdBuff+nCmdLen;
		pCurrCmdN=pNext;
	}
}

// helper to display a message, followed by a number and line terminator (avoiding expensive sprintf)
// uses fNameBuff as a message buffer
static void DisplayMsgParams(const char* str, int params, ...)
{
	int	p;
	int	ix;
	va_list	ap;

	strcpy(fNameBuff, str);
	
	va_start(ap, params);
	for(ix=0; ix<params; ix++)
	{
		p=va_arg(ap, int);
		itoa(p, fNameBuff+strlen(fNameBuff));
		if(ix!=params-1)
		{
			strcat(fNameBuff, msgParamSep);
		}
	}
	strcat(fNameBuff, term);
	printLineMsg(fNameBuff);
	va_end(ap);
}

// helper to display a message, followed by a long long number and line terminator (avoiding expensive sprintf)
// uses fNameBuff as a message buffer
static void DisplayMsgLLParams(const char* str, int params, ...)
{
	unsigned long long	ll;
	int	ix;
	va_list	ap;

	strcpy(fNameBuff, str);
	
	va_start(ap, params);
	for(ix=0; ix<params; ix++)
	{
		ll=va_arg(ap, long long);
		lltoa(ll, fNameBuff+strlen(fNameBuff));
		if(ix!=params-1)
		{
			strcat(fNameBuff, msgParamSep);
		}
	}
	strcat(fNameBuff, term);
	printLineMsg(fNameBuff);
	va_end(ap);
}


// helpers to display file operations error messages
// 

static void DisplayMsgOpenErr(const char* fName)
{
	strcpy(fNameBuff, "*** Could not open ");
	strcat(fNameBuff, fName);
	strcat(fNameBuff, ". Please retry.***");
	printLineMsg(fNameBuff);
}

#if defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)
static void DisplayMsgCloseErr(const char* fName)
{
	strcpy(fNameBuff, "*** Could not close ");
	strcat(fNameBuff, fName);
	strcat(fNameBuff, ". Aborting...");
	printLineMsg(fNameBuff);
}

static void DisplayMsgSuccess(const char* operation)
{
	strcpy(fNameBuff, operation);
	strcat(fNameBuff, " done successfully!");
	printLineMsg(fNameBuff);	
}

static void DisplayProgress(int byteCnt)
{
	printMsg(".");
}

#endif	// defined(AUDIO_SUPPORT_PLAY) && defined(AUDIO_SUPPORT_RECORD)

#ifdef AUDIO_SUPPORT_PLAY
static void DisplayMsgEmptyErr(const char* fName)
{
	
	strcpy(fNameBuff, "*** File ");
	strcat(fNameBuff, fName);
	strcat(fNameBuff, " is empty. Please retry.***");
	printLineMsg(fNameBuff);
}	
#endif

static void DisplayMsgNoFName(void)
{
	printLineMsg("*** Audio player: specify a file name with the command.*** ");
}

static void DisplayMsgRdErr(const char* fName)
{
	strcpy(fNameBuff, "  An error occurred while reading from the ");
	strcat(fNameBuff, fName);
	strcat(fNameBuff, " file. Aborting...");
	printLineMsg(fNameBuff);	
}

static void DisplayMsgWrErr(const char* fName)
{
	strcpy(fNameBuff, "  An error occurred while writing to the ");
	strcat(fNameBuff, fName);
	strcat(fNameBuff, " file. Aborting...");
	printLineMsg(fNameBuff);	
}


static void DisplayMsgMemErr(void)
{
	printLineMsg("  Not enough memory. Aborting...");
}


static void DisplayMsgDataErr(const char* fName)
{
	strcpy(fNameBuff, "  Invalid data in the ");
	strcat(fNameBuff, fName);
	strcat(fNameBuff, " file. Aborting...");
	printLineMsg(fNameBuff);	
}


static void DisplayMsgBuffErr(const char* bName)
{
	strcpy(fNameBuff, bName);
	strcat(fNameBuff, " buffer too small. Aborting...");
	printLineMsg(fNameBuff);
}
	

static void DisplayUnsuportErr(void)
{
	printLineMsg("  Operation not supported...");
}

static void DisplayMsgNoCmd(void)
{
	printLineMsg("*** Audio player: Please type in a command***");
}



static void DisplayMsgUnknown(void)
{
	printLineMsg("*** Audio player: unknown command.*** ");
}

static void DisplayMsgGenErr(void)
{
	printLineMsg("An unspecified error has occurred. Aborting...");
}

static int GetCmdIntParam(void)
{
	char* pStr=strtok(0, tokenSep);

	return pStr?atoi(pStr):-1;
}


static char* GetCmdStrParam(void)
{
	return strtok(0, tokenSep);
}



/* reverse:  reverse string s in place */
void reverse(char s[])
{
    int c, i, j;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

	
/* itoa:  convert n to characters in s */
void itoa(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
} 

	
/* lltoa:  convert n to characters in s */
void lltoa(unsigned long long n, char s[])
{
	int i;

	i = 0;
	do
	{       /* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0);     /* delete it */
    
	s[i] = '\0';
	reverse(s);
} 


static void CloseAudioInFile(void)
{
	if(playerInFile)
	{
		fclose(playerInFile);
		playerInFile=0;
	}
}


static void CloseAudioOutFile(void)
{
	if(playerOutFile)
	{
		fclose(playerOutFile);
		playerOutFile=0;
	}	
}


static void CloseAudioFiles(void)
{
	CloseAudioInFile();
	CloseAudioOutFile();
	
}



static void DisplayOperationResult(AUDIO_RES res)
{
	switch(res)
	{
		case AUDIO_RES_OUT_OF_MEM:
			DisplayMsgMemErr();
			break;

		case AUDIO_RES_WRITE_ERROR:
			DisplayMsgWrErr(playerOutName);
			break;

		case AUDIO_RES_READ_ERROR:
			DisplayMsgRdErr(playerInName);
			break;

		case AUDIO_RES_DSTREAM_ERROR:
			DisplayMsgDataErr(playerInName);
			break;

		case AUDIO_RES_SSTREAM_ERROR:
			DisplayMsgDataErr(playerInName);
			break;

		case AUDIO_RES_BUFF_ERROR:
			DisplayMsgBuffErr(playerOperation);
			break;

		case AUDIO_RES_UNSUPPORTED:
			DisplayUnsuportErr();
			break;			

		default:
			DisplayMsgGenErr();
			break;
	}				

}


static void printLineMsg(const char* str)
{
	putsUART2(str);
	putsUART2(term);
}

static void printMsg(const char* str)
{
	putsUART2(str);
}


static void CmdAddHead(cmdDlList* pL, cmdNode* pN)
{
	if(pL->head==0)
	{ // empty list, first node
		pL->head=pL->tail=pN;
		pN->next=pN->prev=pN;
	}
	else
	{
		pN->next=pL->head;
		pN->prev=pL->tail;
		pL->tail->next=pN;
		pL->head->prev=pN;
		pL->head=pN;
	}		
}


static cmdNode*	CmdRemoveTail(cmdDlList* pL)
{
	cmdNode* pN;
	if(pL->head==pL->tail)
	{
		pN=pL->head;
		pL->head=pL->tail=0;
	}
	else
	{
		pN=pL->tail;
		pL->tail=pN->prev;
		pL->tail->next=pL->head;
		pL->head->prev=pL->tail;
	}
	return pN;
}




