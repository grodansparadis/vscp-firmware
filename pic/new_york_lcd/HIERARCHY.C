/* 
----------------------------------------------------

HIERARCHY:-
A PROGRAM TO ALLOW THE CASCADING OF MENU SELECTIONS
USING A STANDARD LCD DISPLAY

----------------------------------------------------
*/

/*
------------------------------------------
CONSTANTS/MACROS
------------------------------------------
*/
#define VERSION_STRING "1.00"
//used in introductory screen for software version number

//USER PARAMETERS
//names and locations for all the different parameters
//stored in iParameters[]
//limit this to 16 to simplify the article
#define BURNER1 0x00
#define BURNER2 0x01
#define BURNER3 0x02
#define BURNER4 0x03
#define BURNER5 0x04
#define BURNER6 0x05
#define INPUT_TYPE 0x06
#define PBAND_CHAN1 0x07
#define INTEGRAL_CHAN1 0x08
#define DIFFRNTL_CHAN1 0x09
#define RPM 0x0a
#define INTEGRAL_CHAN2 0x0b
#define DIFFRNTL_CHAN2 0x0c
#define SIGMIN 0x0d
#define SIGMAX 0x0e
#define LANGUAGE 0x0f

#define NUMBER_OF_PARAMETERS 32
//to allow easy change to more parameters.

//LCD signal lines
#define RS 5
#define RW 4
#define E1 6
#define BF 3

//LCD addresses as built into the LCD command
#define LINE1_START 0x80
#define LINE2_START 0xc0
#define LINE3_START 0x94
#define LINE4_START 0xd4
#define COLON_IN_BUFFER 11
//number of characters over for the flashing colon
#define COLON_PNT (0xd4+COLON_IN_BUFFER)
//TIMER CONSTANTS
#define HUNDREDmS 100		//100mS delay

#define NUMBER_OF_TIMERS 3
//the system measures 100mS periods based on HUNDREDmS above.
//the timers measure multiples of this 100mS
//the following is the array allocation in periodTimers[NUMBER_OF_TIMERS]
#define T_ACTIVITY 0	//flash colon
#define T_KEYBOARD_REPEAT 1 //timer for auto repeat of keyboard
#define T_KEYBOARD_TIMEOUT 2 //timeout of keyboard
#define ACTIVITY_TIME 5	//every 500mS

//External Logic pin addresses
#define EN_PB 12 //keyboard enable
#define ACTIVITY_LED 20	//activity LED address

//KEYBOARD CONSTANTS
#define TYPEMATIC_INITIAL 10	//1 sec before first repeat
#define TYPEMATIC 2				//200mS for every incrment thereafter
#define TYPEMATIC_STEP 10		//10 incremntts before going up a multiplier
#define K_ENTER 1					//Keyboard Code associated with a key
#define K_MENU 2					//Keyboard Code associated with a key
#define K_UP 3					//Keyboard Code associated with a key
#define K_DOWN 4					//Keyboard Code associated with a key
#define K_PASSWORD 5					//Keyboard Code associated with a key

#define INITIATE_DISPLAY 5
//offset to allow wait for keyboard interface to
//go before it to allow easier debugging
#define MENU_LEVEL_ENTRY INITIATE_DISPLAY+2
//2 InterPhases further on
#define USER_LEVEL_ENTRY MENU_LEVEL_ENTRY+1
//next InterPhase on
#define FACTORY_LEVEL_ENTRY USER_LEVEL_ENTRY+1
//next InterPhase on
#define BURNER_SELECTION_ENTRY FACTORY_LEVEL_ENTRY+1
#define BURNER_ENABLE_ENTRY BURNER_SELECTION_ENTRY+1
#define RPM_TOLERANCE_ENTRY BURNER_ENABLE_ENTRY+1

#define UNUSED_PARAMETER 0xff
//unused to allow detetction of unused display line


// ----------------------------


/*
------------------------------------------
VARIABLES
------------------------------------------
*/
unsigned int iPhase;
//cooperative multitasking selection device
unsigned int iParameter[32];
unsigned char cBufferLine1[21],cBufferLine2[21],cBufferLine3[21],cBufferLine4[21];
//lines to be written to display
unsigned char cBufferTime[21];	//"time line"

unsigned long lLCDtimer,lLCDtimerFreeze;


unsigned int iStatus;
//status byte with the following bit allocations
#define WRITE_LINE1 0x0001		/*request write to line 1*/
#define WRITE_LINE2 0x0002		/*request write to line 2*/
#define WRITE_LINE3 0x0004		/*request write to line 3*/
#define WRITE_LINE4 0x0008		/*request write to line 4*/
#define TIME_DISABLE 0x0010	/*disable time update on screen */
#define COLON_REQUEST 0x0020	/*write colon to screen */
#define MINUTES_CHANGED 0x0040	/*in order to update time on display */
#define PB_DELTA 0x0080	/*valid pushbutton detected */
#define KEYBOARD_VALID 0x0100	/* keyboard available for analysis */
#define TRANSFER_DISPLAY_ENABLE 0x0200	/* enable TransferMessage procedure */
unsigned int iLatchedStatus;
//to allow display write to complete

#define KEYBOARD_TIMEOUT 1200	//2 minutes for keyboad before timeout

unsigned int iLCDphase;	//for subtasks in writing data to LCD hardware
unsigned int iDisplayReadyPhase;	//for subtasks waiting for display to go ready
unsigned int iWriteLinePhase; //write line 
unsigned int iUpdateDisplayPhase;	//send all 4 lines to display as necessary
unsigned int iInterPhase;		//in DisplayInterfaceManager

unsigned char cColon;				//used for colon symbol
unsigned int periodTimers[NUMBER_OF_TIMERS];
//multiple timers based on 100mS intervals
unsigned long mSTimer0;
unsigned long mSTimer0_Freeze;

unsigned char cActivity;	//used to determine state of LED and colon
struct tm rtc,rtcSave;							//real time clock structure
struct tm FaultOcc;							//clock structure for fault occurrence
unsigned int iPreviousMinutes;		// to detect minute change
unsigned int iPreviousSeconds;		// to detect second change
unsigned int iDynamicDisplay;			//output to demonstarte update on screen
unsigned char cPB[3];	//for debounce of pushbuttons
unsigned char cPushButton;	//pushbutton input
unsigned char cKeyboardCode;		//keyboard output code
unsigned char iKeyboardIncrement;	//amount added to step
unsigned char cKeyboardLoopCounter;	//number of times a particular value is
//used before incrment.

unsigned int iFactoryLevelEnable;	//used to simulate a jumper
unsigned int iControlInputType;		//in place of a parameter
unsigned int iModel;			//model type


/*
------------------------------------------
CONSTANTS IN FLASH
------------------------------------------
*/
const unsigned int iDefaultParameters[NUMBER_OF_PARAMETERS]= {
1,1,1,1,1,1,0,6000,4000,2000,150,4000,2000,0,100,0, //0-15
100,0,0x1200,25,0,10,100,30,30,100,30,30,0,0,0,0 //16-31

};

const unsigned int iMaximumParameters[NUMBER_OF_PARAMETERS]= {
1,1,1,1,1,1,1,65535,65535,65535,500,65535,65535,100,100,2, //0-15
3000,400,0x2459,50,1,100,100,100,100,100,100,100,0,0,0,0 //16-31
};

const unsigned int iMinimumParameters[NUMBER_OF_PARAMETERS]= {
0,0,0,0,0,0,0,0,0,0,100,0,0,0,0,0, //0-15
0,0,0x0000,5,0,0,0,0,0,25,0,0,0,0,0,0 //16-31

};

const unsigned int iStepParameters[NUMBER_OF_PARAMETERS]= {
1,1,1,1,1,1,1,1,1,110,10,1,1,1,1,1, //0-15
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 //16-31

};

#define MAX_SCREEN_SEL 10
//maximum number of screen selections (lines on the screen)
//to move arrow up and down
struct ScreenType {
unsigned char cNumberOfLines;
unsigned char cArrowsRequired;	//need to display selection arrows
unsigned char cMsgNo[MAX_SCREEN_SEL]; //message to be displayed
unsigned char cProcNum[MAX_SCREEN_SEL]; //process to be done on message
unsigned char cParamNum[MAX_SCREEN_SEL];	//parameter associated with line
unsigned char cNextAction[MAX_SCREEN_SEL];	//where to go when enter is pressed
unsigned char cAssociatedDisplay[MAX_SCREEN_SEL];	//what to display when enter is pressed
//this can carry a number associated with the line, so that the lines can be packed
//and still correctly identified (it could be eeprom address when the line is actually a parameter
};

#define MAX_SCREENS 10		//number of different screens possible
#define INITIAL_DISPLAY 0	//first display
#define FIRST_MENU 1
//when enter is pressed  for the 1st time Screen[1] in diags
#define USER_LEVEL_SCREEN 2	//USER LEVEL INPUT SCREEN Screen[2]
#define FACTORY_LEVEL_SCREEN 3	//FACTORY LEVEL INPUT SCREEN Screen[3]
#define BURNER_SCREEN 4	//BURNER SUMMARY SCREEN Screen[4]
#define BURNER_ENABLE_SCREEN 5 //enable disable channel Screen[5]
#define RPM_TOLERANCE_SCREEN 6 //change the RPM tolerance

#define BLANK_MESSAGE 25
//25th message is blank

struct ScreenType const Screen[MAX_SCREENS]={
//This is the definition of all the different screens

//INITIAL_DISPLAY
4,							//0-INITIAL_DISPLAY=4 lines
0,							//
0,1,2,BLANK_MESSAGE,0,0,0,0,0,0,	//message number
1,5,0,4,0,0,0,0,0,0,	//processes
0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,	//associated parameters- 0xff used to denote line is to be blanked
MENU_LEVEL_ENTRY,MENU_LEVEL_ENTRY,MENU_LEVEL_ENTRY,MENU_LEVEL_ENTRY,0,0,0,0,0,0,	//next process
FIRST_MENU,FIRST_MENU,FIRST_MENU,FIRST_MENU,0,0,0,0,0,0,	//and the associated display for that next process

//FIRST_MENU 1-> Screen[1]
4,							//=4 lines
1,							//arrows required
3,4,5,6,0,0,0,0,0,0,	//message number
0,0,2,0,0,0,0,0,0,0,	//processes
0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,	//associated parameters- 0xff used to denote line is to be blanked
0,USER_LEVEL_ENTRY,FACTORY_LEVEL_ENTRY,0,0,0,0,0,0,0,	//next process, should have SYSTEM STATUS and DIAGNOSTICS when completed
0,USER_LEVEL_SCREEN,FACTORY_LEVEL_SCREEN,0,0,0,0,0,0,0,	//and the associated display for that next process

//USER_LEVEL_SCREEN 2-> Screen[2]
8,							//=8 lines
1,							//arrows required
7,8,9,10,11,12,13,14,0,0,	//message number
0,0,0,0,0,0,3,0,0,0,	//processes
0,0,0,0,0,0,0,0,0xff,0xff,	//associated parameters- 0xff used to denote line is to be blanked
BURNER_SELECTION_ENTRY,0,0,0,0,0,0,0,0,0,	//
BURNER_SCREEN,0,0,0,0,0,0,0,0,0,	//

//FACTORY_LEVEL_SCREEN 3-> Screen[3]
5,							//=5 lines
1,							//arrows required
18,19,20,21,22,0,0,0,0,0,	//message number
0,0,0,0,0,0,0,0,0,0,	//processes
0,RPM,0,0,0,0xff,0xff,0xff,0xff,0xff,	//associated parameters- 0xff used to denote line is to be blanked
0,RPM_TOLERANCE_ENTRY,0,0,0,0,0,0,0,0,	//next process, should have SYSTEM STATUS and DIAGNOSTICS when completed
0,RPM_TOLERANCE_SCREEN,0,0,0,0,0,0,0,0,	//and the associated display for that next process

//BURNER_SCREEN 4-> Screen[4]
6,							//=6 lines
1,							//arrows required
15,15,15,15,15,15,0,0,0,0,	//message number
6,6,6,6,6,6,0,0,0,0,	//processes
BURNER1,BURNER2,BURNER3,BURNER4,BURNER5,BURNER6,0xff,0xff,0xff,0xff,	//associated parameters- 0xff used to denote line is to be blanked
BURNER_ENABLE_ENTRY,BURNER_ENABLE_ENTRY,BURNER_ENABLE_ENTRY,BURNER_ENABLE_ENTRY,BURNER_ENABLE_ENTRY,BURNER_ENABLE_ENTRY,0,0,0,0,	//next process, should have SYSTEM STATUS and DIAGNOSTICS when completed
BURNER_ENABLE_SCREEN,BURNER_ENABLE_SCREEN,BURNER_ENABLE_SCREEN,BURNER_ENABLE_SCREEN,BURNER_ENABLE_SCREEN,BURNER_ENABLE_SCREEN,0,0,0,0,	//and the associated display for that next process

//BURNER_ENABLE_SCREEN 5-> Screen[5]
6,							//=4 lines
0,							//no arrows required
15,BLANK_MESSAGE,BLANK_MESSAGE,BLANK_MESSAGE,0,0,0,0,0,0,	//message number
7,8,0,0,0,0,0,0,0,0,	//processes
0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,	//associated parameters- 0xff used to denote line is to be blanked
0,0,0,0,0,0,0,0,0,0,	//next process, should have SYSTEM STATUS and DIAGNOSTICS when completed
0,0,0,0,0,0,0,0,0,0,	//and the associated display for that next process

//RPM_TOLERANCE_SCREEN 6-> Screen[6]
4,							//=4 lines
0,							//no arrows required
23,24,BLANK_MESSAGE,BLANK_MESSAGE,0,0,0,0,0,0,	//message number
0,9,0,0,0,0,0,0,0,0,	//processes
0,0,0,0,0xff,0xff,0xff,0xff,0xff,0xff,	//associated parameters- 0xff used to denote line is to be blanked
0,0,0,0,0,0,0,0,0,0,	//next process, should have SYSTEM STATUS and DIAGNOSTICS when completed
0,0,0,0,0,0,0,0,0,0,	//and the associated display for that next process



};

unsigned char cActualNumberOfLines;
//for the current display
unsigned char cLineLimit;
//for displays < 4 lines
unsigned char cTopLine,cPointerLine;
//for arrow on side
unsigned char cFocusParameter;
//when the parameter is finally selected, the address is carried on this
//variable- modified every time the Enter is pressed on a menu selection
unsigned int iModifiableValue;
//parameter stored to allow parameter to be modified
//or cancelled by just pressing menu and not enter
unsigned char cParameterScreen;
//when a parameter is being changed it needs to be refreshed
//the screen is saved here

struct DisplayBufferType {
//This is the memory for the displays sized
//for the maximum number of rows

unsigned char cArrowsRequired;	//need to display selection arrows
unsigned char cScreenCreatedToGetHere; //in order to recreate later
unsigned char cLine[MAX_SCREEN_SEL][21];
//unsigned char cMsgNo[MAX_SCREEN_SEL]; //message to be displayed NEEDED?
//unsigned char cProcNum[MAX_SCREEN_SEL]; //process to be done on message NEEDED?
unsigned char cParamNum[MAX_SCREEN_SEL];	//parameter associated with line
unsigned char cNextAction[MAX_SCREEN_SEL];	//where to go when enter is presssed
unsigned char cAssociatedDisplay[MAX_SCREEN_SEL];	//display to invoke when enter is preesed

//this can carry a number associated with the line, so that the lines can be packed
//and still correctly identified (it could be eeprom address when the line is actually a parameter

}DisplayBuffer;

struct StackType {
unsigned char cStackPointer;
unsigned char cScreenCreatedToGetHere[MAX_SCREEN_SEL];
unsigned char cTopLine[MAX_SCREEN_SEL];
unsigned char cPointerLine[MAX_SCREEN_SEL];
unsigned int iCallingInterPhase[MAX_SCREEN_SEL];
} ProcessStack;



//originally the following was a 3 dimensional array, but because of the Rabbit constraint
//that the const declaration must be in root memory, this was shifted to
//the xstring format. 
xstring English {
//there are 30 messages below of 20 characters in 3 languages
	"HIERARCHY Versn     ",//0
	"MODEL X100          ",//1
	"Dynamic Reading     ",//2
	"SYSTEM STATUS       ",//3
	"USER LEVEL          ",//4
	"FACTORY LEVEL       ",//5
	"DIAGNOSTICS         ",//6
	"BURNER CONTROLS     ",//7
	"CONTROL INPUT TYPE  ",//8
	"LANGUAGE CONTROLS   ",//9
	"SERVICE REMINDER    ",//10
	"FULL TANK BLOWDOWN  ",//11
	"BLOWDOWN CONTROLS   ",//12
	"LOW/HIGH RH ALARMS  ",//13
	"DATE/TIME           ",//14
	"BURNER @:           ",//15
	"ENABLED             ",//16
	"DISABLED            ",//17
	"FACTORY LOCATION    ",//18
	"BLOWER RPM FAULT    ",//19
	"BLOWER RESPONSE     ",//20
	"RPM TRANSITIONS     ",//21
	"CLEAR FAULT HISTRY  ",//22
	"RPM TOLERANCE       ",//23
	"RPM +/-: @          ",//24
	"                    ",//25
	"MODEL X2000         ",//26
	"SPARE               ",//27
	"SPARE               ",//28
	"SPARE               "//29
};
xstring French {
//french- not actually translated
//but 1st character changed to an f just to see if it works
	"fIERARCHY           ",//0
	"fODEL G100          ",//1
	"fnitial Display     ",//2
	"fYSTEM STATUS       ",//3
	"fSER LEVEL          ",//4
	"fACTORY LEVEL       ",//5
	"fIAGNOSTICS         ",//6
	"fURNER CONTROLS     ",//7
	"fONTROL INPUT TYPE  ",//8
	"fANGUAGE CONTROLS   ",//9
	"fERVICE REMINDER    ",//10
	"fULL TANK BLOWDOWN  ",//11
	"fLOWDOWN CONTROLS   ",//12
	"fOW/HIGH RH ALARMS  ",//13
	"fATE/TIME           ",//14
	"fURNER @            ",//15
	"fNABLED             ",//16
	"fISABLED            ",//17
	"fACTORY LOCATION    ",//18
	"fLOWER RPM FAULT    ",//19
	"fLOWER RESPONSE     ",//20
	"fPM TRANSITIONS     ",//21
	"fLEAR FAULT HISTRY  ",//22
	"fPM TOLERANCE       ",//23
	"fPM +/-: @          ",//24
	"fPARE               ",//25
	"fPARE               ",//26
	"fPARE               ",//27
	"fPARE               ",//28
	"fPARE               "//29
};
xstring German {
//german- not actually translated
//but 1st character changed to an g just to see if it works
	"gIERARCHY           ",//0
	"gODEL G100          ",//1
	"gnitial Display     ",//2
	"gYSTEM STATUS       ",//3
	"gSER LEVEL          ",//4
	"gACTORY LEVEL       ",//5
	"gIAGNOSTICS         ",//6
	"gURNER CONTROLS     ",//7
	"gONTROL INPUT TYPE  ",//8
	"gANGUAGE CONTROLS   ",//9
	"gERVICE REMINDER    ",//10
	"gULL TANK BLOWDOWN  ",//11
	"gLOWDOWN CONTROLS   ",//12
	"gOW/HIGH RH ALARMS  ",//13
	"gATE/TIME           ",//14
	"gURNER @            ",//15
	"gNABLED             ",//16
	"gISABLED            ",//17
	"gACTORY LOCATION    ",//18
	"gLOWER RPM FAULT    ",//19
	"gLOWER RESPONSE     ",//20
	"gPM TRANSITIONS     ",//21
	"gLEAR FAULT HISTRY  ",//22
	"gPM TOLERANCE       ",//23
	"gPM +/-: @          ",//24
	"gPARE               ",//25
	"gPARE               ",//26
	"gPARE               ",//27
	"gPARE               ",//28
	"gPARE               "//29
};

/*
------------------------------------------
PROTOTYPES
------------------------------------------
*/
void InitialisePorts(void);
void write259s(unsigned char outputNo, int outputValue);
void InitialiseParameters(void);
void ClearBuffer (void *BufferStart, unsigned char BlankChar, unsigned char NumberOfCharacters);
int WaitDisplayReady (void);
void WriteControlNybble (unsigned char DisplayCmd);
int WriteControlByte (int DisplayCmd);
int WriteDataByte (int DisplayData);
void DisplayInitialise(void);
void mSecInitialise(void *data1, void *data2, unsigned long timerValue);
unsigned char mSecTimeup (void *data1,void *data2);
void UpdateDisplay (void);
int WriteLine (void *BufferStart, unsigned char DisplayStartAddress, unsigned char NumberOfCharacters);
void OverallProcess (void);
void Keyboard (void);
void DisplayInterfaceManager(void);
void TransferMsg(unsigned char MessageNumber,void *Destination);
void CreateDisplay (unsigned char MessageNumber);
void DisplayInterfaceManager (void);
void ProcessLine (unsigned char LineProcess, unsigned char LineNumber);
void TransferDisplay( void);
void PerCent (unsigned int PC, void *LineNum);
void FlattenStack (void);
void PopStack (void);
void PushStack (void);
void PointerUp (void);
void PointerDown (void);
void ClearPointers(void);
void TimeoutNoKeyEntry(void);
void GoToDisplaySelected(void);
void ObliterateLine(unsigned char LineNumber);
void IncreaseParameter(void);
void DecreaseParameter(void);
void PasteLeft (void *Dest,unsigned int Conv);
void PasteRight (void *Dest,unsigned int Conv);

/*
------------------------------------------
"MAIN" ROUTINE
------------------------------------------
*/
main ()
{
	auto int i;
	
	InitialisePorts();
	//set up the microcomputer ports
	InitialiseParameters();
	//in this example the parameters are always initialised-
	//in reality they would be initialised under a particular set of conditions
	//and then preserved by battery backup in RAM or in Flash or EEPROM.
	
	//Dynamic C does not initialise variables.
	
	iStatus=0;
	iLCDphase=0;
	iDisplayReadyPhase=0;
	iPhase=0;
	iUpdateDisplayPhase=0;
	iWriteLinePhase=0;
	iInterPhase=INITIATE_DISPLAY;
	for (i=0;i<NUMBER_OF_TIMERS;i++)
	{//decrment all timers hereevery 100mS
		periodTimers[i]=0;
	}
	iPreviousMinutes=0xff;		//a non existent minutes number, to ensure initial time message
	iPreviousSeconds=100;		//a non existent seconds number, to ensure initial time message
	iDynamicDisplay=120;		//a non existent seconds number, to ensure initial time message
	cPB[0]=cPB[1]=cPB[2]=0;
	FlattenStack();
	//Dynamic C does not initialise variables.
	
	DisplayInitialise();
	mSecInitialise(&mSTimer0,&mSTimer0_Freeze,HUNDREDmS);
	while (1)
	{
		switch (iPhase)
		{
			case 0:
				UpdateDisplay();
				break;
			case 1:
				Keyboard();
				break;
			case 2:
				OverallProcess();
				break;
			case 3:
				DisplayInterfaceManager();
				break;
			default:
				iPhase=0;
				break;
		}
	}
}

/*
------------------------------------------
SUPPORTING ROUTINEs
------------------------------------------
*/
void InitialisePorts(void)
{
	//sets the microcomputer ports including I/O on the host board
	//to the desired condition and state
	unsigned char i;
	const unsigned char cInitial259[24]=
	{		//U9
	 0,	//unused
	 1,	//	irrelevant
	 1,	// irrelevant
	 1, 	// irrelevant
	 0,	// unused
	 0,	// irrelevant
	 0,	// irrelevant
	 0,	// irrelevant
			// U12
	 1,	// SEL1 high
	 1,	// SEL2 high
	 1,	// SEL3 high
	 1,	// SEL4 high
	 1,	// SEL5 high
	 0,	// irrelevant
	 0,	// irrelevant
	 0,	// irrelevant
	 		//U14
	 0,	// irrelevant
	 0,	// irrelevant
	 0,	// irrelevant
	 0,	// irrelevant
	 1,	// irrelevant
	 1,	// irrelevant
	 0,	//unused
	 0		//irrelevant
	 };
	 
	WrPortI(SPCR, &SPCRShadow, 0x80);
	// disable slave port &
	//portA is all input

	//PB0-5 are inputs and PB6,7 are outputs,by default
	WrPortI(PBDR, &PBDRShadow, 0x40);	//EN-C is high
	//PC is used for the serial ports
	//PD0-3 is for I/O- initialise to input
	//PD4-6 are outputs PD7 is unused
	WrPortI(PDCR, &PDCRShadow, 0x00);	// outputs always updated on pclk/2
	WrPortI(PDDDR, &PDDDRShadow, 0xf0);	// upper bits are output, bal are input
	WrPortI(PDDCR, &PDDCRShadow, 0x00); // all active high
	WrPortI(PDFR, &PDFRShadow, 0x00);	// no alternate funtions
	WrPortI(PDDR, &PDDRShadow, 0x00);	//all outputs at zero

	//PORT E are all outputs except for PE0 which is input
	WrPortI(PECR, &PECRShadow, 0x00);	// outputs always updated on pclk/2
	WrPortI(PEDDR, &PEDDRShadow, 0xfe);	// only bit 0=input
	WrPortI(PEDR, &PEDRShadow, 0xc0);	// set outputs
	WrPortI(PEFR, &PEFRShadow, 0x00);	// no strobes

	//and now to set up the 24 outputs on the 259s
	//additional outputs are set up on 74HC259 and are individually
	//addressable.
	//obviously you design will employ different techniques.
	//The only use of these outputs, as far as this article is concerned
	//is to enable the keyboard in order to read it.
	//this still needs to be in the program so that other hardware functions
	//do not conflict.
	//
	for (i=0;i<24;i++)
	{
		write259s(i,(int)cInitial259[i]);
	}

}

// --------------------------------------------------------
void write259s(unsigned char outputNo, int outputValue)
{// writes to bit 0-23 based on outputNo, using outputValue
	//based on input set up out combination

	//this procedure woulud be replaced by one to drive your peripherals

	int iTempOut;

	//fetch the current output
	iTempOut=PEDRShadow & 0xc7;
	//clear all the associated outputs
	switch (outputNo)
	{
		case 0:
		case 8:
		case 16:
		//set outputs to 000
		//tempOutput is unchnaged
			break;
		case 1:
		case 9:
		case 17:
			iTempOut |= 0x8;
			break;

		case 2:
		case 10:
		case 18:
			iTempOut |= 0x20;
			break;

		case 3:
		case 11:
		case 19:
			iTempOut |= 0x28;
			break;

		case 4:
		case 12:
		case 20:
			iTempOut |= 0x10;
			break;

		case 5:
		case 13:
		case 21:
			iTempOut |= 0x18;
			break;

		case 6:
		case 14:
		case 22:
			iTempOut |= 0x30;
			break;

		default:
		//case 7:
		//case 15:
		//case 23:
			iTempOut |= 0x38;
			break;
	}
	//set the output address
	WrPortI(PEDR, &PEDRShadow, iTempOut);	// set outputs
	
	//now to set the output to the desired value
	if (outputValue !=0) outputValue=1;
	//non zero forces a 1.
	BitWrPortI(PBDR,&PBDRShadow,outputValue,7);
	
	if (outputNo<8)
	{
		BitWrPortI(PEDR,&PEDRShadow,0,7);
	}
	else {
		if (outputNo<16)
		{
			BitWrPortI(PEDR,&PEDRShadow,0,6);
		}
		else {
			BitWrPortI(PBDR,&PBDRShadow,0,6);
		}
	}
	//and now take them all high
	BitWrPortI(PEDR,&PEDRShadow,1,7);
	BitWrPortI(PEDR,&PEDRShadow,1,6);
	BitWrPortI(PBDR,&PBDRShadow,1,6);
}

//-------------------------------------------------
void InitialiseParameters(void)
{//set all the parameters to their default values.
	int i;
	for (i=0;i<NUMBER_OF_PARAMETERS;i++)
	{
		iParameter[i]=iDefaultParameters[i];
	}
}
//-------------------------------------------------
void DisplayInitialise(void)
{//initialise the display
	mSecInitialise(&lLCDtimer,&lLCDtimerFreeze,15);
	while (mSecTimeup(&lLCDtimer,&lLCDtimerFreeze)==0)
	{
		;//15mS delay to allow stabilization
	}

	WriteControlNybble(0x03);
	mSecInitialise(&lLCDtimer,&lLCDtimerFreeze,15);
	while (mSecTimeup(&lLCDtimer,&lLCDtimerFreeze)==0)
	{
		;//5mS delay,
		//>4.1mS as per LCD spec
	}
	WriteControlNybble(0x03);
	mSecInitialise(&lLCDtimer,&lLCDtimerFreeze,2);
	while (mSecTimeup(&lLCDtimer,&lLCDtimerFreeze)==0)
	{
		;//1mS delay
		//>100uS as per LCD spec
	}
	WriteControlNybble(0x03);
	WriteControlNybble(0x02);

	while (WriteControlByte(0x28)==0){;}//2 line 5x8
	//while (WriteControlByte(0x0f)==0){;}//dispaly on, cursor on, cursor flash
	//uncomment the above for help in setting up display
	while (WriteControlByte(0x0c)==0){;}//display on, cursor off
	//uncomment the above for display without cursor
	while (WriteControlByte(0x01)==0){;}//clear
	while (WriteControlByte(0x06)==0){;}//increment from left to right

	//set up graphics
	//set cursor position to 0x00 in Char Geneartor

	while (WriteControlByte(0x40)==0){;}//increment from left to right
	
		//setup null character as blank
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}
		
		//now to set up the up arrow
		//byte by byte
		//for the following bit pattern
		
		//--x--  =0x04
		//-xxx-  =0x0e
		//x-x-x  =0x15
		//--x--  =0x04
		//--x--  =0x04
		//--x--  =0x04
		//-----  =0x00
		//-----  =0x0

		//placing the character 0x01 in the display buffer will bring up this character
		
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x0e)==0){;}
	while (WriteDataByte(0x15)==0){;}
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x00)==0){;}

		//Down arrow, derived as above
		//placing the character 0x02 in the display buffer will bring up this character
	while (WriteDataByte(0x00)==0){;}
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x15)==0){;}
	while (WriteDataByte(0x0e)==0){;}
	while (WriteDataByte(0x04)==0){;}
	while (WriteDataByte(0x00)==0){;}


	//point to start of display
	while (WriteControlByte(0x01)==0){;}//clear
	while (WriteControlByte(0x01)==0){;}//clear
	while (WriteControlByte(0x81)==0){;}//clear
	
		//now to prepare the buffers
	ClearBuffer(&cBufferLine1[0],0x20,20);
	ClearBuffer(&cBufferLine2[0],0x20,20);
	ClearBuffer(&cBufferLine3[0],0x20,20);
	ClearBuffer(&cBufferLine4[0],0x20,5);
		
	//
	//set up initial buffer	
	//

	/* the following is only used in development
	
	strcpy(cBufferLine1,"1 - This is Line 1");
	strcpy(cBufferLine2,"2 - This is line 2");
	strcpy(cBufferLine3,"3 - This is line 3");
	//strcpy(BufferLine4,"4 - This is line 4");
	cBufferLine1[2]=0x01; //up arrow
	cBufferLine2[2]=0x02; //down arrow
	cBufferLine3[2]=0x01; //up arrow
	*/
		
	iStatus |= (WRITE_LINE1 | WRITE_LINE2 | WRITE_LINE3 | WRITE_LINE4);

}

//-------------------------------------------------
//a utility to clear the designated display buffer
void ClearBuffer (void *BufferStart, unsigned char BlankChar, unsigned char NumberOfCharacters)
{
	auto unsigned char i;
	unsigned char *p1;
	p1=BufferStart;
	for (i=0;i<NumberOfCharacters;i++)
	{
		*p1=BlankChar;
		p1++;
	}
}
//-------------------------------------------------

//a utility to wait until display is ready
int WaitDisplayReady (void)
{
//returns a 1 when display is ready, else returns a 0
	unsigned int i,j;
	
	j=0;
	//prepare return
	switch (iDisplayReadyPhase)
	{
		case 0:
			BitWrPortI(PDDR,&PDDRShadow,0,RS);	//control
			//set up R/!W
			BitWrPortI(PDDR,&PDDRShadow,1,RW);
			BitWrPortI(PDDR,&PDDRShadow,1,E1);
			iDisplayReadyPhase++;
			break;
		case 1:
			if (BitRdPortI(PDDR,BF)==0)
			{
				j=1;
				//set up return
				iDisplayReadyPhase=0;
				BitWrPortI(PDDR,&PDDRShadow,0,E1);
				//take E1 back down again
			}
			break;
		default:
			iDisplayReadyPhase=0;
			break;
	}
/*
	while (BitRdPortI(PDDR,BF)!=0)
	{
		;//wait for ready
	}
		j=1;
		//set up return
*/
	return j;
}
//-------------------------------------------------

void WriteControlNybble (unsigned char DisplayCmd)
{
	//write the parameter as a control word to the display module
	unsigned char i1;
	//set up cmd, RS=0
	BitWrPortI(PDDR,&PDDRShadow,0,RS);
	//set up R/!W
	BitWrPortI(PDDR,&PDDRShadow,0,RW);
	WrPortI(PDDDR, &PDDDRShadow, 0xff);	// upper bits are output, bal are output now
	//take E1 high
	BitWrPortI(PDDR,&PDDRShadow,1,E1);
	//manipulate the data
	i1=(DisplayCmd) & 0x0f;
	i1 |= (PDDRShadow &0xf0);	//clear bits
	//i1 has the upper nibble
	WrPortI(PDDR, &PDDRShadow, (int)i1);// data on bits PD0 to PD3
	BitWrPortI(PDDR,&PDDRShadow,0,E1); //take E1 low
	WrPortI(PDDDR, &PDDDRShadow, 0xf0);	// upper bits are cofigured to output, bal are input now

}
//-------------------------------------------------

int WriteControlByte (int DisplayCmd)
{
	int i2,j2;
	//sends 2 niibbles to display module.
	//the first time this is called and the time process has not elapsed
	//will initiate the process
	//it will return a 0 while executing and a 1 when complete
	j2=0; //to prime the return value
	switch (iLCDphase)
	{
		case 0:
			//in this state, the write process must be started
			iLCDphase++;
			break;
		case 1:
			//set up cmd, RS=0
			BitWrPortI(PDDR,&PDDRShadow,0,RS);
			//set up R/!W
			BitWrPortI(PDDR,&PDDRShadow,0,RW);
			WrPortI(PDDDR, &PDDDRShadow, 0xff);	// upper bits are output, bal are output now
			//take E1 high
			BitWrPortI(PDDR,&PDDRShadow,1,E1);
			//manipulate the data
			i2=(DisplayCmd>>4);
			i2 &= 0x0f;
			i2 |= (PDDRShadow &0xf0);	//clear bits
			//i2 has the upper nibble
			WrPortI(PDDR, &PDDRShadow, i2);// data on bits PD0 to PD3
			BitWrPortI(PDDR,&PDDRShadow,0,E1); //take E1 low
			WrPortI(PDDDR, &PDDDRShadow, 0xf0);	// upper bits are output, bal are input now
			iLCDphase++;
			break;
		case 2:
		case 4:
			//wait for the nibble to be written
			if (WaitDisplayReady())
			{
				iLCDphase++;
			}
			break;
		case 3:
			//2nd nibble
			//set up cmd, RS=0
			BitWrPortI(PDDR,&PDDRShadow,0,RS);
			//set up R/!W
			BitWrPortI(PDDR,&PDDRShadow,0,RW);
			WrPortI(PDDDR, &PDDDRShadow, 0xff);	// upper bits are output, bal are output now
			//take E1 high
			BitWrPortI(PDDR,&PDDRShadow,1,E1);
			//manipulate the data
			i2=(DisplayCmd) & 0x0f;
			i2 |= (PDDRShadow &0xf0);	//clear bits
			//i has the upper nibble
			WrPortI(PDDR, &PDDRShadow, i2);// data on bits PD0 to PD3
			BitWrPortI(PDDR,&PDDRShadow,0,E1); //take E1 low
			WrPortI(PDDDR, &PDDDRShadow, 0xf0);	// upper bits are output, bal are input now
			iLCDphase++;
			break;
		case 5:
		default:
			//at this point the process is over
			j2=1;
			iLCDphase=0;
			break;
			
			
			
	}
	return j2;
}
//-------------------------------------------------

int WriteDataByte (int DisplayData)
{
	int i2,j2;
	j2=0;	//return value for wait

	switch (iLCDphase)
	{
		case 0:
			//in this state, the write process is started
			iLCDphase++;
			break;
		case 1:
			//set up cmd, RS=1
			BitWrPortI(PDDR,&PDDRShadow,1,RS);
			//set up R/!W
			BitWrPortI(PDDR,&PDDRShadow,0,RW);
			WrPortI(PDDDR, &PDDDRShadow, 0xff);	// upper bits are output, bal are output now
			//take E1 high
			BitWrPortI(PDDR,&PDDRShadow,1,E1);
			//manipulate the data
			i2=(DisplayData>>4);
			i2 &= 0x0f;
			i2 |= (PDDRShadow &0xf0);	//clear bits
			//i has the upper nibble
			WrPortI(PDDR, &PDDRShadow, i2);// data on bits PD0 to PD3
			//just for delay
			BitWrPortI(PDDR,&PDDRShadow,0,E1); //take E1 low
			WrPortI(PDDDR, &PDDDRShadow, 0xf0);	// upper bits are output, bal are input now
			iLCDphase++;
			break;
		case 2:
		case 4:
			//wait for the nibble to be written
			if (WaitDisplayReady())
			{
				iLCDphase++;
			}
			break;
		case 3:
			//2nd nibble
			//set up data, RS=1
			BitWrPortI(PDDR,&PDDRShadow,1,RS);
			//set up R/!W
			BitWrPortI(PDDR,&PDDRShadow,0,RW);
			WrPortI(PDDDR, &PDDDRShadow, 0xff);	// upper bits are output, bal are output now
			//take E1 high
			BitWrPortI(PDDR,&PDDRShadow,1,E1);
			//manipulate the data
			i2=(DisplayData) & 0x0f;
			i2 |= (PDDRShadow &0xf0);	//clear bits
			//i has the upper nibble
			WrPortI(PDDR, &PDDRShadow, i2);// data on bits PD0 to PD3
			BitWrPortI(PDDR,&PDDRShadow,0,E1); //take E1 low
			WrPortI(PDDDR, &PDDDRShadow, 0xf0);	// upper bits are output, bal are input now
			//wait for ready
			iLCDphase++;
			break;
		case 5:
		default:
			//at this point the process is over
			j2=1;
			iLCDphase=0;
			break;
	}
	return j2;
}
//-------------------------------------------------

unsigned char mSecTimeup (void *data1,void *data2)
{

	//takes the data (time delay period) at pointer *data1,
	//adds it to the start time pointed at by *data2.
	//If the the timer MS_TIMER is greater than the total
	//	then the time has expired.
	//It is convoluted to allow for the rollover from ff to 0
	//of MS_TIMER.				
	//This function is non-reentrant.

	//PARAMETER1:    the address of the timer (so that there can be a multitude of timers)
	//PARAMETER2:    the address of the time the procedure was invoked
	
		//simply save the length (in mS) and the time the process is initailised			
		unsigned long temp;
		//unsigned long temp2;
		unsigned long *p1;
		unsigned long *p2;
		unsigned char i;
		
		p1=data1;
		p2=data2;

		temp=MS_TIMER;
		//temp2=*p1+*p2;
		
		if (*p2 > (*p1 + *p2)) //<<------------
		{//then a rollover condition will occur here
			//wait for rollover
			if (temp<*p2)
			{//rollover has occurred
				if (temp>(*p1  -(0xffffffff-*p2)))
				{//the time above 0 is calculated from
				 //the initial time to 0 and the to the delay
					i=1;
				}
				else {
					i=0;
				}
			}
			else {
				i=0;
			}
			
		}
		else {//here there will be no rollover
			if ( temp > (*p1 + *p2))
			{
				i=1;
			}
			else {
				i=0;
			}
		}
		return i;
}
//-------------------------------------------------


void mSecInitialise(void *data1, void *data2, unsigned long timerValue)
{
	//Take parameter2 and create the desired time by adding
	//it to the current mS timer. If there will be a rollover
	//compensation is provided
					
	//This function is non-reentrant.

	//PARAMETER1:    the address of the timer (so that there can be a multitude of timers)
	//PARAMETER2:    the address of the time the procedure was invoked
	//PARAMETER2:		the time in mS
	//simply save the length (in mS) and the time the process is initailised			
		
		unsigned long *p1;
		unsigned long *p2;
		p1=data1;
		p2=data2;

		*p2=MS_TIMER;
		*p1=timerValue;
}
//-------------------------------------------------
//write a message line to the display
int WriteLine (void *BufferStart, unsigned char DisplayStartAddress, unsigned char NumberOfCharacters)
{	//allows to write a line of any length, down to 1 character
	//returns a 1 when complete, otherwise 0
	
	static unsigned char cCharacterCount,cCounter1;
	static unsigned char *cLinePoint;
	auto int i;

	i=0; //return value set to 0 as incomplete
	switch (iWriteLinePhase)
	{
		case 0:
			cLinePoint=BufferStart;
			cCharacterCount=NumberOfCharacters;
			cCounter1=0;//initialise counter for number of characters
			iWriteLinePhase++;
			break;
		case 1:
			if (WriteControlByte(DisplayStartAddress))
			{
				iWriteLinePhase++;
			}
			break;
		case 2:
			if ((WriteDataByte ((int)*cLinePoint)))
			{
				cCounter1++;	//increase the number of characters written
				cLinePoint++;//bump pointer
				if (cCounter1>=cCharacterCount)
				{
					iWriteLinePhase=0;
					i=1;	
				}
			}
			break;
		default:
			iWriteLinePhase=0;
			break;
		
	}

	return i;
}
//-------------------------------------------------

void UpdateDisplay (void)
{
	//update the display based on several requests in status
	
	switch (iUpdateDisplayPhase)
	{
		case 0:
		//check if any update is needed
		//including the flashing of the colon in the time display
			iLatchedStatus=iStatus;
			iStatus &= ~((WRITE_LINE1 | WRITE_LINE2 | WRITE_LINE3 |WRITE_LINE4));
			//so a request is not lost when this process is writing a line
			//and a new request is made.
			if ((iLatchedStatus & (WRITE_LINE1 | WRITE_LINE2 | WRITE_LINE3 |WRITE_LINE4)) ||
			     ((iStatus & (TIME_DISABLE | COLON_REQUEST))==COLON_REQUEST))
			     iUpdateDisplayPhase++;
			     break;
		case 1:
			if (iLatchedStatus & WRITE_LINE1)
			{
				if (WriteLine(&cBufferLine1[0],LINE1_START,20))
				{
					iLatchedStatus &= ~WRITE_LINE1;
					iUpdateDisplayPhase++;
				}
				
			}
			else {
				iUpdateDisplayPhase++;
				//no line1 look at next line
			}
			break;
		case 2:
			if (iLatchedStatus & WRITE_LINE2)
			{
				if (WriteLine(&cBufferLine2[0],LINE2_START,20))
				{
					iLatchedStatus &= ~WRITE_LINE2;
					iUpdateDisplayPhase++;
				}
				
			}
			else {
				iUpdateDisplayPhase++;
				//no line1 look at next line
			}
			break;
		case 3:
			if (iLatchedStatus & WRITE_LINE3)
			{
				if (WriteLine(&cBufferLine3[0],LINE3_START,20))
				{
					iLatchedStatus &= ~WRITE_LINE3;
					iUpdateDisplayPhase++;
				}
				
			}
			else {
				iUpdateDisplayPhase++;
				//no line1 look at next line
			}
			break;
		case 4:
			if (iLatchedStatus & WRITE_LINE4)
			{
				if (WriteLine(&cBufferLine4[0],LINE4_START,20))
				{
					iLatchedStatus &= ~WRITE_LINE4;
					iStatus &= ~COLON_REQUEST;
					iUpdateDisplayPhase++;
				}
				
			}
			else {
				iUpdateDisplayPhase++;
				//no line1 look at next line
			}
			break;
		case 5:
		//needed to flash colon in minutes display,
		//but only when time is needed on the display
			if ((iStatus & (TIME_DISABLE | COLON_REQUEST))==COLON_REQUEST)
			{
				if (WriteLine(&cColon,COLON_PNT,1))
				{
					iStatus &= ~COLON_REQUEST;
					iUpdateDisplayPhase=0;
				}
			}
			else {
				iUpdateDisplayPhase=0;
				//no line1 look at next line
			}
			break;
		default:
			iUpdateDisplayPhase=0;
			break;
	}
	iPhase++;
	
}
//-------------------------------------------------
void OverallProcess (void)
{
	auto unsigned char i;
	struct tm rtc;
	auto unsigned long temp1;
	
	if (mSecTimeup(&mSTimer0,&mSTimer0_Freeze))
	{
		mSecInitialise(&mSTimer0,&mSTimer0_Freeze,HUNDREDmS);
		//refresh timer for next period	
		for (i=0;i<NUMBER_OF_TIMERS;i++)
		{//decrment all timers hereevery 100mS
			if ( periodTimers[i] !=0)
			{
				periodTimers[i]--;
			}
		}
		
	}
	//check for activity indicator
	if ((periodTimers[T_ACTIVITY]==0)/* && ((status & TIME_DISABLE)==0)*/)
	{
		periodTimers[T_ACTIVITY]=ACTIVITY_TIME;
		cActivity++;
		//activity is essentially a divide by 2 counter
		//the LSB is used to flash the colon in the display
		 
		write259s(ACTIVITY_LED,(cActivity & 0x1));
		//on board health LED- irrelevant to this application
		
		tm_rd(&rtc);
		//fetched the time
		//now has the time changed
		iDynamicDisplay=rtc.tm_sec;
		if (rtc.tm_min!=iPreviousMinutes)
		{
			iPreviousMinutes=rtc.tm_min;
			iStatus|=MINUTES_CHANGED;
		}
		if ((iStatus & TIME_DISABLE)==0)
		{
		//now has the time changed
		if (iStatus & MINUTES_CHANGED)
		{
			//flag operational time update
			iStatus &= ~MINUTES_CHANGED;
			
			ClearBuffer(&cBufferTime[0],0x20,20);
			if (rtc.tm_mon<10)
			{
				cBufferTime[0]='0';
				itoa(rtc.tm_mon,&cBufferTime[1]);
			}
			else {
				itoa(rtc.tm_mon,&cBufferTime[0]);
			}
			cBufferTime[2]='.';
			
			if (rtc.tm_mday<10)
			{
				cBufferTime[3]='0';
				itoa(rtc.tm_mday,&cBufferTime[4]);
			}
			else {
				itoa(rtc.tm_mday,&cBufferTime[3]);
			}
			cBufferTime[5]='.';

			if ((rtc.tm_year-100)<10)
			{
				cBufferTime[6]='0';
				itoa((rtc.tm_year-100),&cBufferTime[7]);
			}
			else {
				itoa((rtc.tm_year-100),&cBufferTime[6]);
			}
			cBufferTime[8]=' ';

			if (rtc.tm_hour<10)
			{
				cBufferTime[9]='0';
				itoa(rtc.tm_hour,&cBufferTime[10]);
			}
			else {
				itoa(rtc.tm_hour,&cBufferTime[9]);
			}
			
			if (cActivity & 0x1)
			{
				cBufferTime[COLON_IN_BUFFER]=':';
			}
			else {
				cBufferTime[COLON_IN_BUFFER]=' ';
			}
			if (rtc.tm_min<10)
			{
				cBufferTime[12]='0';
				itoa(rtc.tm_min,&cBufferTime[13]);
			}
			else {
				itoa(rtc.tm_min,&cBufferTime[12]);
			}
			cBufferTime[14]=0x0;

			//can move later
			
			strcpy(&cBufferLine4[0],&cBufferTime[0]);
			iStatus |= WRITE_LINE4;
			
		}
		else {
			iStatus |= COLON_REQUEST;
			if (cActivity & 0x1)
			{
				cColon=':';
			}
			else {
				cColon=' ';
			}
		}
		}
	}
	iPhase++;

}

/* *********************************** */
void Keyboard (void)
{
	write259s(EN_PB,(int)0);
	//enable keyboard buffer
	//shunt last readings
	cPB[0]=cPB[1];
	cPB[1]=cPB[2];
	cPB[2]= (unsigned char)RdPortI(PADR) & 0xf8;
	//3 scans for debounce
	write259s(EN_PB,(int)1);
	//disable keyboard buffer
	if ((cPB[0]==cPB[1]) && (cPB[0]==cPB[2]))
	{
		if (cPushButton != cPB[2])
		{
			iStatus |= PB_DELTA;
			cPushButton=cPB[2];
		}
	}
	
	if (iStatus & PB_DELTA)
	{//here there is a keyboard input
		iStatus &= ~PB_DELTA;
		//clear the callin flag
		//and strt the timer with the initial period, i.e. for the 1st 1 sec nothing happens
		periodTimers[T_KEYBOARD_REPEAT]=TYPEMATIC_INITIAL;
		iKeyboardIncrement=1;		//inititlise the increment 
		//(it can be 1, 10, 100, & 1000 & 10000)
		cKeyboardLoopCounter=1;
		//number of loops before the KeyboardIncrement is incrmented
		iStatus|=KEYBOARD_VALID;
		periodTimers[T_KEYBOARD_TIMEOUT]= KEYBOARD_TIMEOUT;
		//refresh timer
		switch (cPushButton)
		{
			case 0x78:
				cKeyboardCode=K_ENTER;
				break;
			case 0xE8:
				cKeyboardCode=K_MENU;
				break;
			case 0xD8:
				cKeyboardCode=K_UP;
				break;
			case 0xB8:
				cKeyboardCode=K_DOWN;
				break;
			case 0x98:
				cKeyboardCode=K_PASSWORD;
				break;
			default:
				cKeyboardCode=0;
				iStatus&=~KEYBOARD_VALID;
				//not a valid code
				break;
				
		}
		
	}
	else {
	//no keyoard input, check for typematic increases
		if (((cKeyboardCode==K_UP) || (cKeyboardCode==K_DOWN)) && (cPushButton !=0xf8))
		{//only if up or down
			if (periodTimers[T_KEYBOARD_REPEAT]==0)
			{// only when time is up
				cKeyboardLoopCounter++;
				if (cKeyboardLoopCounter>TYPEMATIC_STEP)
				{
					cKeyboardLoopCounter=1;
					if (iKeyboardIncrement<10000)
					{
						iKeyboardIncrement *=10;
					}
				}
				periodTimers[T_KEYBOARD_REPEAT]=TYPEMATIC;
				iStatus|=KEYBOARD_VALID;
			}
		}
	}

	iPhase++;
}
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void DisplayInterfaceManager (void)
{
	switch (iInterPhase)
	{
		case 0:
			break;
		case INITIATE_DISPLAY:
			CreateDisplay(INITIAL_DISPLAY);
			iInterPhase++;
			break;
		case INITIATE_DISPLAY+1:
			TransferDisplay();
			//not internal flag prevents refresh
			//every time it is called
			if ((iPreviousSeconds!=iDynamicDisplay))
			{//UPDATE WHEN SECONDS CHANGE
				iPreviousSeconds=iDynamicDisplay;
				TransferMsg(2,cBufferLine3);
				//copy message 2 to the buffer line to refresh
				//and add the number
				PerCent(iPreviousSeconds,cBufferLine3);	
				//and now for the % demand
				iStatus |= (WRITE_LINE3);
			}
			if (iStatus & KEYBOARD_VALID)
			{
				iStatus &= ~KEYBOARD_VALID;
				if (cKeyboardCode==K_MENU)
				{//only in Menu input
					GoToDisplaySelected();
				}
			}
			break;
		case MENU_LEVEL_ENTRY:
			TransferDisplay();
			if (iStatus & KEYBOARD_VALID)
			{
				iStatus &= ~KEYBOARD_VALID;
				switch (cKeyboardCode)
				{
					case K_ENTER:
						GoToDisplaySelected();
						break;
					case K_MENU:
						PopStack();
						break;
					case K_UP:
						PointerUp();
						break;
					case K_DOWN:
						PointerDown();
						break;
					case K_PASSWORD:
						break;
					default:
						break;
				}
			}
			else {//check timeout
				TimeoutNoKeyEntry();
			}
			break;
			
		//TYPICAL MENU SCAN
		case USER_LEVEL_ENTRY:
		case FACTORY_LEVEL_ENTRY:
		case BURNER_SELECTION_ENTRY:
			TransferDisplay();
			if (iStatus & KEYBOARD_VALID)
			{
				iStatus &= ~KEYBOARD_VALID;
				switch (cKeyboardCode)
				{
					case K_ENTER:
						GoToDisplaySelected();
						break;
					case K_MENU:
						PopStack();
						break;
					case K_UP:
						PointerUp();
						break;
					case K_DOWN:
						PointerDown();
						break;
					case K_PASSWORD:
						break;
					default:
						break;
				}
			}
			else {//check timeout
				TimeoutNoKeyEntry();
			}
			break;

		//TYPICAL PARAMTER CHANGE
		case BURNER_ENABLE_ENTRY:
		case RPM_TOLERANCE_ENTRY:
			TransferDisplay();
			if (iStatus & KEYBOARD_VALID)
			{
				iStatus &= ~KEYBOARD_VALID;
				switch (cKeyboardCode)
				{
					case K_ENTER:
						iParameter[cFocusParameter]=iModifiableValue;
						//update the parametr
						PopStack();
						break;
					case K_MENU:
						PopStack();
						//no update of the parameter
						break;
					case K_UP:
						IncreaseParameter();
						break;
					case K_DOWN:
						DecreaseParameter();
						break;
					case K_PASSWORD:
						break;
					default:
						break;
				}
			}
			else {//check timeout
				TimeoutNoKeyEntry();
			}
			break;
		default:
			iInterPhase=INITIATE_DISPLAY;
			break;
	}
	iPhase++;
}

/* *********************************** */
void CreateDisplay (unsigned char MessageNumber)
{
	auto unsigned char i,j,k,cDetect;
	
	for (i=0;i<MAX_SCREEN_SEL;i++)
	{// clearing the parameter type- 0xff
	 //indicates that the line is to be blanked
		DisplayBuffer.cParamNum[i]=UNUSED_PARAMETER;
		//set up for unused line
		TransferMsg(BLANK_MESSAGE,&DisplayBuffer.cLine[i][0]);
		//for unused line-set to blank
		//blanking the rows
	
	}
	//transferring all relevant information from the screen
	//to the display buffer
	DisplayBuffer.cArrowsRequired=Screen[MessageNumber].cArrowsRequired;
	DisplayBuffer.cScreenCreatedToGetHere=MessageNumber;
	for (i=0;i<(Screen[MessageNumber].cNumberOfLines);i++)
	{//do for number of lines in the particular display
		DisplayBuffer.cNextAction[i]=Screen[MessageNumber].cNextAction[i];
		//so we know what to do next
		DisplayBuffer.cAssociatedDisplay[i]=Screen[MessageNumber].cAssociatedDisplay[i];
		//so we know what and display
		DisplayBuffer.cParamNum[i]=Screen[MessageNumber].cParamNum[i];
		//save the parameter
		TransferMsg(Screen[MessageNumber].cMsgNo[i],&DisplayBuffer.cLine[i][0]);
		//copy the associated message across
		ProcessLine(Screen[MessageNumber].cProcNum[i],i);
		//and depending on the process change the display
		//including if the line is not needed, blanking that line
	}
	//now lines in the middle can be deleted by ProcessLine
	//so we compress the message into a contiguous block
	//analyse and pack, by shifting up one line at a time
	
	cActualNumberOfLines=Screen[MessageNumber].cNumberOfLines;
	do {
		cDetect=1;
		for (i=0;i<cActualNumberOfLines;i++)
		{
			if (DisplayBuffer.cParamNum[i]==UNUSED_PARAMETER)
			{//shift up everything beneath it
				cDetect=0;
				for (j=i;j<cActualNumberOfLines;j++)
				{
					for (k=0;k<21;k++)
					{
						DisplayBuffer.cLine[j][k]=DisplayBuffer.cLine[j+1][k];
					}
					DisplayBuffer.cParamNum[j]=DisplayBuffer.cParamNum[j+1];
					
				}
				cActualNumberOfLines--;
				break;
			}
			else{
				;
			}
		}

	} while (cDetect==0);
	cLineLimit=cActualNumberOfLines;
	if (cActualNumberOfLines<4)
	{
		cActualNumberOfLines=4;
	}
	//and force update when transfer message next seen
	iStatus |= TRANSFER_DISPLAY_ENABLE;


}
/*
struct ScreenType DisplayBuffer={
//This is the memory for the displays sized
//for the maximum number of rows

unsigned char cArrowsRequired;	//need to display selection arrows
unsigned char cScreenCreatedToGetHere; //in order to recreate later
unsigned char cLine[MAX_SCREEN_SEL][21];
//unsigned char cMsgNo[MAX_SCREEN_SEL]; //message to be displayed NEEDED?
//unsigned char cProcNum[MAX_SCREEN_SEL]; //process to be done on message NEEDED?
unsigned char cParamNum[MAX_SCREEN_SEL];	//parameter associated with line
unsigned char cNextAction[MAX_SCREEN_SEL];	//where to go when enter is presssed
unsigned char cAssociatedDisplay[MAX_SCREEN_SEL];	//display to invoke when enter is preesed

//this can carry a number associated with the line, so that the lines can be packed
//and still correctly identified (it could be eeprom address when the line is actually a parameter

};
Placed here for easy reference

*/

void TransferMsg(unsigned char MessageNumber,void *Destination)
{//takes the message number and places it in the display buffer
//originally this was a 3 dimensional array, but because of the Rabbit constraint
//that the const declaration must be in root memory, this was shifted to
//the xstring format. 


//Destination 1-4 Diplay Line 4
//5-8 ADisplayLine 1-4
#define XACCESS(m, n)	(m + (sizeof(long)*n))
	long x;
	
	unsigned char *p1;
	p1=Destination;

	//calculate the source address
	//each message is 16 bytes long, so multiply the message number
	//by 16
	switch (iParameter[LANGUAGE])
	{
		case 1:
			x=XACCESS(French,MessageNumber);
			break;
		case 2:
			x=XACCESS(German,MessageNumber);
			break;
		default:
			x=XACCESS(English,MessageNumber);
			break;
	}
	
	xmem2root((void*) &x, x, sizeof(long)); 	// dereference x
	xmem2root((void*) p1, x, 21); // load string into data
}
// -------------------------------------------
void ProcessLine (unsigned char LineProcess, unsigned char LineNumber)
{
	
	switch (LineProcess)
	{
		case 0:
			break;
		case 1:
		//add version to line
			strcpy(&DisplayBuffer.cLine[LineNumber][16],VERSION_STRING);
			//version
			break;
		case 2:
		//check if Factory Level to be displayed
			if (iFactoryLevelEnable==0)
			{
				ObliterateLine(LineNumber);
			}
			break;
		case 3:
		//check if RH Alarms to be displayed
			if (iControlInputType==0)
			{
				ObliterateLine(LineNumber);
			}
			break;
		case 4:
		//set time
			//looking at 4th line: blank
			iStatus &=~TIME_DISABLE;
			iPreviousMinutes=0xff;
			//force update
			break;
		case 5:
		//model number
			if (iModel)
			{
				TransferMsg(26,&DisplayBuffer.cLine[LineNumber][0]);
			}
			break;
		case 6:
		//write ENABLED or DISABLED depending on the line
		//and also 3/6 line display
			if ((iModel==0) && (LineNumber>=3))
			{//3 burner unit 4th line and more
				ObliterateLine(LineNumber);	
			}
			else {
				*strpbrk(&DisplayBuffer.cLine[LineNumber][0],"@")=((LineNumber+1) | 0x30);
				if (iParameter[DisplayBuffer.cParamNum[LineNumber]])
				{//ENABLED
					strcpy (&DisplayBuffer.cLine[LineNumber][10],"ENABLED");
				}
				else {
					//DISABLED
					strcpy (&DisplayBuffer.cLine[LineNumber][10],"DISABLED");
				}
			}
			break;
		case 7:
		//place burner number
			*strpbrk(&DisplayBuffer.cLine[LineNumber][0],"@")=(((cFocusParameter-BURNER1)+1) | 0x30);
			break;
		case 8:
			//display enabled or disabled
			//parameter stored to allow parameter to be modified
			//or cancelled by just pressing menu and not enter
			
			if (iModifiableValue)
			{
				TransferMsg(16,&DisplayBuffer.cLine[LineNumber][0]);
			}
			else {
				TransferMsg(17,&DisplayBuffer.cLine[LineNumber][0]);
			}
			break;
		case 9:
			PasteRight((strpbrk(&DisplayBuffer.cLine[LineNumber][0],"@")),iModifiableValue);
			break;
		default:
			break;
	}
}
// -----------------------------------
void TransferDisplay( void)
{//starting at the top line

unsigned char i;
	if (	iStatus & TRANSFER_DISPLAY_ENABLE)
	{
		iStatus &= ~TRANSFER_DISPLAY_ENABLE;
		//clear so doesn't continuously execute
		//every time it is called
		if ((cTopLine+4)>cActualNumberOfLines)
		{
			cTopLine=cActualNumberOfLines-4;
			//ensure we don't scroll past
		}
	
		for (i=0;i<21;i++)
		{
			cBufferLine1[i]=DisplayBuffer.cLine[cTopLine+0][i];
			cBufferLine2[i]=DisplayBuffer.cLine[cTopLine+1][i];
			cBufferLine3[i]=DisplayBuffer.cLine[cTopLine+2][i];
			cBufferLine4[i]=DisplayBuffer.cLine[cTopLine+3][i];
		}
		if (DisplayBuffer.cArrowsRequired)
		{
			//first ensure the arrow is within limits
			if ((cPointerLine>=cTopLine) && (cPointerLine<(cTopLine+4)))
			//hopefully this never gets executed- it is just in case
			{
				;//this is OK
			}
			else {
				if (cPointerLine<cTopLine)
				{
					cPointerLine=cTopLine;
				}
				else {
					cPointerLine=cTopLine+3;
				}
			}
			switch (cPointerLine-cTopLine)
			{
				case 0:
					cBufferLine1[18]=0x7f;
					break;
				case 1:
					cBufferLine2[18]=0x7f;
					break;
				case 2:
					cBufferLine3[18]=0x7f;
					break;
				default:
					cBufferLine4[18]=0x7f;
					break;
			}
			//and now up and down arrows
			if (cActualNumberOfLines>4)
			{
				if (cTopLine!=0)
				{
					cBufferLine1[19]=0x1;	//up arrow
				}
				if ((cTopLine+4)<cActualNumberOfLines)
				{
					cBufferLine4[19]=0x2;	//down arrow
				}
			}
			//and then password indicators
			//PasswordIndicators();
		}
		iStatus |= (WRITE_LINE1 | WRITE_LINE2 | WRITE_LINE3 | WRITE_LINE4);

	}
}
//--------------------------------------------
void PerCent (unsigned int PC, void *LineNum)
{//Takes the percent number and stores it to the last 4 digits of the
//desired line
	unsigned int i;
	unsigned char *p1;

	p1=LineNum;

	if (PC<10)
	{
		utoa(PC,p1+18);
	}
	else {
		if (PC<100)
		{
			utoa(PC,p1+17);
		}
		else {
			utoa(PC,p1+16);
		}
	}
	strcat (p1,"%");
}

// -------------------------------
/*
struct StackType {
unsigned char cStackPointer;
unsigned char cScreenCreatedToGetHere[MAX_SCREEN_SEL];
unsigned char cTopLine[MAX_SCREEN_SEL];
unsigned char cPointerLine[MAX_SCREEN_SEL];
unsigned int iCallingInterPhase[MAX_SCREEN_SEL];
} ProcessStack;

Placed here for easy reference
*/
void PushStack (void)
{//save parameters in order to return to the same place
	ProcessStack.cScreenCreatedToGetHere[ProcessStack.cStackPointer]=
		DisplayBuffer.cScreenCreatedToGetHere;
	ProcessStack.cTopLine[ProcessStack.cStackPointer]=cTopLine;
	ProcessStack.cPointerLine[ProcessStack.cStackPointer]=cPointerLine;
	ProcessStack.iCallingInterPhase[ProcessStack.cStackPointer]=iInterPhase;
	ProcessStack.cStackPointer++;
	//increment the pointer so ready for next push
}
// -------------------------------
void ClearPointers(void)
{
	//must be done separately to allow correct display creation
	cTopLine=0;
	cPointerLine=0;
	iStatus |= TIME_DISABLE;
	//prevent any further time update at deeper levels
}

// -------------------------------
void PopStack (void)
{//restore parameters in order to return to the previous place
	if (ProcessStack.cStackPointer<=1)
	{
		FlattenStack();
	}
	else {
		ProcessStack.cStackPointer--;
		//drop back to valid reading
		CreateDisplay(ProcessStack.cScreenCreatedToGetHere[ProcessStack.cStackPointer]);
		cTopLine=ProcessStack.cTopLine[ProcessStack.cStackPointer];
		cPointerLine=ProcessStack.cPointerLine[ProcessStack.cStackPointer];
		iInterPhase=ProcessStack.iCallingInterPhase[ProcessStack.cStackPointer];
	//if there is a password system- clear the password validation below a certain level here as
	//well as elsewhere, as necessary
		
	}
}
// -------------------------------
void FlattenStack (void)
{//force a restart of the display
	ProcessStack.cStackPointer=0;
	iInterPhase=INITIATE_DISPLAY;
	cTopLine=0;
	cPointerLine=0;
	iStatus &= ~TIME_DISABLE;
	iPreviousMinutes=0xff;
	
	//if there is a password system- clear the password validation here as
	//well as elsewhere, as necessary
}
// -------------------------------
void PointerUp (void)
{
	if (cPointerLine!=0)
		cPointerLine--;
	if (cPointerLine<cTopLine)
		cTopLine=cPointerLine;
		//this should shift the display up
	iStatus|=TRANSFER_DISPLAY_ENABLE;
}
void PointerDown (void)
{
	if ((cPointerLine<(cLineLimit-1)) && (DisplayBuffer.cParamNum[cPointerLine+1]!=UNUSED_PARAMETER))
		cPointerLine++;
	if ((cTopLine+3)<cPointerLine)
	{
		cTopLine=cPointerLine-3;
		//This should force the dispaly to roll down
	}
	iStatus|=TRANSFER_DISPLAY_ENABLE;
}
// -------------------------------------------------
void TimeoutNoKeyEntry (void)
{//timeout and return to main screen
//also need to clear the password
	if (periodTimers[T_KEYBOARD_TIMEOUT]==0)
	{
		FlattenStack();
	}
}
// ----------------------------------------------
void GoToDisplaySelected(void)
{
	PushStack();
	iInterPhase=DisplayBuffer.cNextAction[cPointerLine];
	//in this case all lines point to the same target
	cFocusParameter=DisplayBuffer.cParamNum[cPointerLine];
	//take the parameter to use later.
	//if this is not a parameter entry, then no harm is done.
	iModifiableValue=iParameter[cFocusParameter];
	cParameterScreen=DisplayBuffer.cAssociatedDisplay[cPointerLine];
	CreateDisplay(DisplayBuffer.cAssociatedDisplay[cPointerLine]);
	ClearPointers();
	//must be done separately to allow correct display creation				
}
// ---------------------------------
void ObliterateLine(unsigned char LineNumber)
{//writes all spaces to line, and forces the associtaed
 //parameter to unused (this to allow packing of lines)
	DisplayBuffer.cParamNum[LineNumber]=UNUSED_PARAMETER;
	TransferMsg(0,&DisplayBuffer.cLine[LineNumber][0]);
}
//----------------------------------------------
void IncreaseParameter (void)
{
	if((iModifiableValue+(iStepParameters[cFocusParameter]*iKeyboardIncrement))>=iMaximumParameters[cFocusParameter])
	{
		iModifiableValue=iMaximumParameters[cFocusParameter];
	}
	else {
		iModifiableValue += (iStepParameters[cFocusParameter]*iKeyboardIncrement);
	}
	CreateDisplay(cParameterScreen);
	iStatus|=TRANSFER_DISPLAY_ENABLE;
}
// -----------------------------------------
void DecreaseParameter (void)
{
	if((iModifiableValue-(iStepParameters[cFocusParameter]*iKeyboardIncrement))<=iMinimumParameters[cFocusParameter])
	{
		iModifiableValue=iMinimumParameters[cFocusParameter];
	}
	else {
		iModifiableValue -= (iStepParameters[cFocusParameter]*iKeyboardIncrement);
	}
	CreateDisplay(cParameterScreen);
	iStatus|=TRANSFER_DISPLAY_ENABLE;
}
// ----------------------------------------------
void PasteLeft (void *Dest,unsigned int Conv)
{//convert an integre and then paste it into the
//the display, right justifying aon the location addressed
//by Dest

	unsigned char *p1;
	auto int T1,i;
	auto unsigned char strBuff1[6];

	p1=Dest;

	utoa(Conv,strBuff1);
	T1=strlen(strBuff1);
	for (i=(T1-1);i>=0;i--)
	{
		*p1=strBuff1[i];
		p1--;
	}
}
// -----------------------------------------------
void PasteRight (void *Dest,unsigned int Conv)
{//convert an integre and then paste it into the
//the display, left justifying aon the location addressed
//by Dest

	unsigned char *p1;
	auto int T1,i;
	auto unsigned char strBuff1[6];

	p1=Dest;

	utoa(Conv,strBuff1);
	T1=strlen(strBuff1);
	for (i=0;i<(T1);i++)
	{
		*p1=strBuff1[i];
		p1++;
	}
}
// -----------------------------------------------