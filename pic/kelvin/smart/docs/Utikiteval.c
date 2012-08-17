/////////////////////////////////////////////////////////////////////////
/* UTI EVALUATION BOARD PROGRAM
   written by Alexander Kerezov
   Tecnical University,
   Sofia, Bulgaria
   e-mail: akerezov@tu-sofia.acad.bg

   This program is configuring the UTI, doing the measurements
   and sending the result according to the user request using 
   a standart RS232 interface - 8 data bit, 1 stop bit,
   No parity error check. The Baud rate is set authomaticaly
   after sending the character '@'.
   In order to read the on line help, type 'H' or 'h'.
   
   Please feel free to copy and distribute this program free of charge.
   The autor does not accept any lyability for any damages done as a
   result from the application of that program or a part of it.

   For more details please contact the autor.
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	FAST UTI PROGRAM !
		last revision: 01.07.2001
   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

*/

#include <16F73.H>

/* Pin definitions */


#define SFmode		PIN_B0
#define PowerDown	PIN_B2
#define PD		02	/* PIN_B2 */
#define CML		PIN_A3
#define OUTPUT		PIN_C2
#define Sync	PIN_C3
#define SEL1	PIN_B5
#define SEL2	PIN_B4
#define SEL3	PIN_A5
#define SEL4	PIN_B1

#define CLR	PIN_A1
#define SCK	PIN_A2
#define DATA	PIN_A0

#byte 	PORTA =	0x05
#byte 	PORTB =	0x06
#byte 	PORTC =	0x07
#byte   PIR1  =	0x0C
#byte   CCPR1H=	0x16
#byte   CCPR1L=	0x15
#byte   RCSTA =	0x18
#byte   RCREG =	0x1A
#byte	TMR1L =	0x0E
#byte	TMR1H =	0x0F
#byte   SPBRG = 0x99
#byte   TXSTA = 0x98
#byte   FSR   = 0x04
#byte   INDF  = 0x00

#bit    RCIF  =	PIR1.5
#bit    CCP1IF= PIR1.2
#bit    carry = 0x03.0
#bit    Zero  = 0x03.2
#bit    CREN  =	RCSTA.4
#bit    BRGH  = TXSTA.2
#bit    TMR1IF= PIR1.0
#bit    RxD_pin  = PORTC.7

long TMR_1;
#byte   TMR_1  = 0x0E

/* PIC16c73 configuration */

#use 	fast_io(B)
#use 	fast_io(C)
#use delay(clock=20000000)
#fuses   HS,NOPROTECT,NOWDT,PUT,NOLVP,NOBROWNOUT
//#fuses   HS,NOPROTECT,NOWDT,PUT
#use rs232(baud=38400, xmit=PIN_C6, rcv=PIN_C7)
#priority timer1,ccp1

/* Variables definition */
int	TMR_compare,answer,Phases,buffer_pointer,softT1;
int	pnt_s,bp,temp_softT1,k,Save_Status,Save_PCLATH,Save_FSR;
long	pnt_h;
short	RESET_FLG,EnableTransmission,Corect_Phase,PreCorect_Phase,Syncronize,PreSyncronize,Mode3_FLG;

#bit    BP_0 = buffer_pointer.0


struct capture {
	int	softwareT;
	long	hardwareT;
		} old_CCP,new_CCP,compare,buffer[10],TxD_buffer[10];
int	*pTxD_buffer,*pBuffer;

#byte   new_CCP_H  = new_CCP+2
#byte   old_CCP_H  = old_CCP+2


int  const Line1[128] = {"\r\n\n              U T I   E V A L U A T I O N   B O A R D   H E L P
\r
\r       s -> Set Slow Mode            f -> Set Fast Mode"};

int  const Line2[232] = {"\r\n       t -> CML = 1 (CMUX12)         n -> CML = 0 (CMUX02)
\r       m -> Single measurement       M -> Infinite measurement cycle
\r                                     p -> Power down
\r       0 -> Mode 0  - 5 Capacitors, 0÷2 pF"};

int  const Line3[225] = {"\r\n       1 -> Mode 1  - 3 Capacitors, 0÷2 pF
\r       2 -> Mode 2  - 5 Capacitors, 0÷12 pF
\r       3 -> Mode 3  - Capacitors, 0÷2 pF if CML=0 / 0÷12 pF if CML=1
\r       4 -> Mode 4  - 3 Capacitors, variable range to 300 pF"};

int  const Line4[230] = {"\r\n       5 -> Mode 5  - Platinium resistor Pt100 - Pt1000, 4 wire
\r       6 -> Mode 6  - Termistor 1k÷25k, 4 wire
\r       7 -> Mode 7  - 2 or 3 platinium resistors Pt100 - Pt1000
\r       8 -> Mode 8  - 2 or 3 termistors 1k÷25k"};

int  const Line5[208] = {"\r\n       9 -> Mode 9  - Resistive bridge, ref. is Vbridge,+/- 200 mV
\r       A -> Mode 10 - Resistive bridge, ref. is Vbridge,+/- 12.5 mV
\r       B -> Mode 11 - Resistive bridge, ref. is Ibridge,+/- 200 mV"};

int  const Line6[207] = {"\r\n       C -> Mode 12 - Resistive bridge, ref. is Ibridge,+/- 12.5 mV
\r       D -> Mode 13 - Resistive bridge and 2 resistors,+/- 200 mV
\r       E -> Mode 14 - Resistive bridge and 2 resistors,+/- 12.5 mV"};

int  const Line7[77] = {"\r\n       F -> Mode 15 - 3 Potentiometers 1k÷50k
\r    Please Make a Choise :"};

/* Timer 1 overflow interrupt service routine */
#int_timer1
void timer1_isr() 
{
   softT1++;
}

/* Input Capture 1 interrupt service routine */
#int_ccp1
/* Input Capture 1 interrupt service routine */
void ccp1_isr() 
{


 if (Mode3_FLG)	// Mode 3
 {
   buffer_pointer++;

   if (BP_0)		//First pass
   {
	output_low (Data);	// Preload 0 as Data
	output_low (SCK);	// Serial Clock generation

  	// Save the captures
	old_CCP.softwareT = softT1;
        old_CCP.hardwareT = CCP_1;
	TMR_compare = TMR1H;
	if (!TMR1IF)
        	if (CCPR1H == 0xFF && TMR_compare == 00) 
			old_CCP.softwareT = softT1-1;
   	if (buffer_pointer == 1)
  	{
		TxD_buffer[1] = buffer[1];
		TxD_buffer[2] = buffer[2];
		TxD_buffer[3] = buffer[3];
		TxD_buffer[4] = buffer[4];
		TxD_buffer[5] = buffer[5];
		TxD_buffer[6] = buffer[6];
		TxD_buffer[7] = buffer[7];
		TxD_buffer[8] = buffer[8];
		TxD_buffer[9] = buffer[9];

		EnableTransmission = 1;	// Transmit - once per sycle
  	}

   }
   else		//Second pass
   {
	//Pulse measurement - Read the captures
	new_CCP.softwareT = softT1;
        new_CCP.hardwareT = CCP_1;
	TMR_compare = TMR1H;
	if (!TMR1IF)
        	if (CCPR1H == 0xFF && TMR_compare == 00) 
			new_CCP.softwareT = softT1-1;

   	// Calculate phase duration
	//old_CCP = new_CCP - old_CCP;
	#asm
		MOVF   	old_CCP.hardwareT,W
		SUBWF  	new_CCP.hardwareT,W
		MOVWF  	old_CCP.hardwareT
		BTFSC  	Carry
		goto	NoOvf_1
		incf	old_CCP_H,f
		BTFSC  	Zero
		incf	old_CCP.softwareT,f
	NoOvf_1:
		MOVF   	old_CCP_H,W
		SUBWF  	new_CCP_H,W
		MOVWF  	old_CCP_H
		BTFSS  	Carry
		incf	old_CCP.softwareT,f
		MOVF   	old_CCP.softwareT,W
		SUBWF  	new_CCP.softwareT,W
		MOVWF  	old_CCP.softwareT
	#endasm
		// Error protection, if old_CCP.softwareT > 0x80 the time is more then 1.6 seconds
		if (old_CCP.softwareT > 0x80) old_CCP.softwareT = 0; // Error protection, 
		
	//	pBuffer = buffer + 3 * (buffer_pointer / 2);
	#asm
		bcf	Carry
		rrf	buffer_pointer,W
		addlw	buffer
		addwf	buffer_pointer,w
		movwf	pBuffer
	#endasm
	// Write the result in the buffer
	*pBuffer = old_CCP.softwareT;
	#asm
		incf	FSR,f
		movf	old_CCP.hardwareT,W
		movwf	INDF
		incf	FSR,f
		movf	old_CCP_H,W
		movwf	INDF
	#endasm

	//Shift MUX & Sync generation

	// Syncronization pulse generation routine
   	if (!(buffer_pointer < (Phases << 1)))	
		{
		output_low (CLR);	// Clear MUX Begin
		output_high (Data);	// Data = 1 on the first pulse
		buffer_pointer = 0;
		output_low(Sync);
		output_high (CLR);	// Clear MUX End
		}
   	else	output_high(Sync);

	output_high (SCK);	// Serial Clock generation - Latch the DATA

	CCP1IF = 0;		// Clear possible switching interrupts
   }

 }
 else	// All the other Modes
 {
 	buffer_pointer++;
	// Syncronization pulse generation routine

   	if (buffer_pointer > Phases)		
		{
		buffer_pointer = 0;
		output_low(Sync);
		if (Corect_Phase)  EnableTransmission = 1;	// Transmit - once per sycle
		if (PreSyncronize) Syncronize = 1; 		// Syncronize - once per sycle
		}
   	else	output_high(Sync);

	//Pulse measurement

   	// Save the captures
	new_CCP.softwareT = softT1;
        new_CCP.hardwareT = CCP_1;
	TMR_compare = TMR1H;
	if (!TMR1IF)
        	if (CCPR1H == 0xFF && TMR_compare == 00) 
			new_CCP.softwareT = softT1-1;

   	// Calculate phase duration
	/*
		if(new_CCP.hardwareT < old_CCP.hardwareT) old_CCP.softwareT++;
      	 	old_CCP.softwareT = new_CCP.softwareT - old_CCP.softwareT;
		old_CCP.hardwareT = new_CCP.hardwareT - old_CCP.hardwareT;
	*/ 	
	#asm
		MOVF   	old_CCP.hardwareT,W
		SUBWF  	new_CCP.hardwareT,W
		MOVWF  	old_CCP.hardwareT
		BTFSC  	Carry
		goto	NoOvf_11
		incf	old_CCP_H,f
		BTFSC  	Zero
		incf	old_CCP.softwareT,f
	NoOvf_11:
		MOVF   	old_CCP_H,W
		SUBWF  	new_CCP_H,W
		MOVWF  	old_CCP_H
		BTFSS  	Carry
		incf	old_CCP.softwareT,f
		MOVF   	old_CCP.softwareT,W
		SUBWF  	new_CCP.softwareT,W
		MOVWF  	old_CCP.softwareT
	#endasm

	//	pBuffer = buffer + 3 * buffer_pointer;
	//	pBuffer = buffer + buffer_pointer + (buffer_pointer<<1);
	#asm
		bcf	Carry
		rlf	buffer_pointer,W
		addlw	buffer
		addwf	buffer_pointer,w
		movwf	pBuffer
	#endasm
	// Write the result in the buffer
	*pBuffer = old_CCP.softwareT;
	#asm
		incf	FSR,f
		movf	old_CCP.hardwareT,W
		movwf	INDF
		incf	FSR,f
		movf	old_CCP_H,W
		movwf	INDF
	#endasm
        old_CCP = new_CCP;	   // Save the old values
 }
}

void syncr()
{
  if (!Mode3_FLG)	// Mode = All but Mode 3
  {
    // Syncronize once per sycle
    PreSyncronize = 1;
    if(Syncronize)
    {
	Syncronize = 0;

	// Find the first phase in the sequense - find the lowerst number 
	bp=1;
	compare.softwareT=buffer[1].softwareT; 
	compare.hardwareT=buffer[1].hardwareT;

	// Extend the buffer for compare
 	buffer[Phases+1].softwareT=buffer[0].softwareT; 
	buffer[Phases+1].hardwareT=buffer[0].hardwareT;

	for (k=2; k<=Phases; k++)
		{
			if (compare.softwareT > buffer[k].softwareT) 
			{
	   			bp=k;
				compare.softwareT=buffer[k].softwareT; 
				compare.hardwareT=buffer[k].hardwareT;
 			}
			if (compare.softwareT == buffer[k].softwareT) 
	  		{
	    			if (compare.hardwareT > buffer[k].hardwareT) 
				{
					bp=k;
					compare.softwareT=buffer[k].softwareT; 
					compare.hardwareT=buffer[k].hardwareT;
 				}
	  		}
		}
	if (buffer[bp-1].softwareT < buffer[bp+1].softwareT) bp--;
	if (buffer[bp-1].softwareT == buffer[bp+1].softwareT) 
		if (buffer[bp-1].hardwareT < buffer[bp+1].hardwareT) bp--;

	if (bp == 0) 	
	{
		if (PreCorect_Phase == 1)	Corect_Phase = 1;
		PreCorect_Phase = 1;

		TxD_buffer[0] = buffer[0];
		TxD_buffer[1] = buffer[1];
		TxD_buffer[2] = buffer[2];
		TxD_buffer[3] = buffer[3];
		TxD_buffer[4] = buffer[4];
		TxD_buffer[5] = buffer[5];
		TxD_buffer[6] = buffer[6];
	}
	else
	{ 
		PreSyncronize = 0;
		PreCorect_Phase = 0;
		Corect_Phase = 0;
		while (buffer_pointer != Phases);
		buffer_pointer = (buffer_pointer - bp);
	}
    }
  }

}
/* Set Slow mode routine */
void SetSlowMode(void){
	output_low(SFmode);
	output_high(PowerDown);
	answer=0;
	}

/* Set Fast mode routine */
void SetFastMode(void){
	output_high(SFmode);
	output_high(PowerDown);
	answer=0;
	}

/* Set CML routine */
void SetCML(void){
if (Mode3_FLG)	{
	output_high(CML);
	output_high(PowerDown);
	answer=0;
		}
	}

/* Clear CML routine */
void ClrCML(void){
	output_low(CML);
	output_high(PowerDown);
	answer=0;
	}

/* Set Power Down mode routine */
void SetPDMode(void){
	output_low(PowerDown);
	answer=0;
	}

/* Set Input Mode routine */
void SetMode(S1,S2,S3,S4,Ph){
int	i;

	output_bit(SEL1,S1);
	output_bit(SEL2,S2);
	output_bit(SEL3,S3);
	output_bit(SEL4,S4);
	output_low(CML);
	Phases=Ph;
	output_high(PowerDown);
	answer=0;
	RESET_FLG=1;

	}

/* Measurent and result send routine */
void NewShowMeasurement(void)
{
int	i;

  if (bit_test(PORTB,PD))
  {

	if (Mode3_FLG)
	{
		while (!EnableTransmission);
		EnableTransmission = 0;

  		for (i=1; !(i > Phases); i++)
		{
		//	pnt_s = TxD_buffer[i].softwareT;
		//	pnt_h = TxD_buffer[i].hardwareT;
	
			if (buffer[i].softwareT != 0) printf("%X",buffer[i].softwareT);
			printf("%lX ",buffer[i].hardwareT);
		}
		printf("\r\n");
		if (answer == 'm')	answer=0;
	}
	else	
	{
		// Wait new sycle
		if (EnableTransmission) 
		{
			EnableTransmission = 0;

			/* Calculate the first phase duration */
			pnt_s=TxD_buffer[0].softwareT+TxD_buffer[1].softwareT;
			pnt_h=TxD_buffer[0].hardwareT+TxD_buffer[1].hardwareT;
			if (carry) pnt_s++;

			if (pnt_s != 0) printf("%X",pnt_s);
			printf("%lX ",pnt_h);
		
  			for (i=2; i<=Phases; i++)
			{
				pnt_s=TxD_buffer[i].softwareT;
				pnt_h=TxD_buffer[i].hardwareT;
	
				if (pnt_s != 0) printf("%X",pnt_s);
				printf("%lX ",pnt_h);
			}
  			printf("\r\n");
			if (answer == 'm')	answer=0;
		}
	}
  }
}


/* Print Help message routine */
void ShowHelp(void)
{
	for (k = 0; k < 128; k++)	putc ( Line1[k] );
	for (k = 0; k < 232; k++)	putc ( Line2[k] );
	for (k = 0; k < 225; k++)	putc ( Line3[k] );
	for (k = 0; k < 230; k++)	putc ( Line4[k] );
	for (k = 0; k < 208; k++)	putc ( Line5[k] );
	for (k = 0; k < 207; k++)	putc ( Line6[k] );
	for (k = 0; k < 77 ; k++)	putc ( Line7[k] );

   	answer=0;
	}


void main(void) 
	{
 	set_TRIS_A(0x00);
   	set_TRIS_B(0x00);
   	set_TRIS_C(0x86);

   	setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);
   	setup_CCP1(ccp_capture_re);
   	setup_CCP2(ccp_capture_fe);
  
   	output_low(PowerDown); 	/* Put the device in Power Down Mode */
   	output_low(SFmode); 	/* Put the device in Slow Mode */
   	output_low(CML); 	/* Put the device in Normal Mode */

// Configure USART : 0x40 should be send for correct 
// Baud rate calculation - 7 logical 0 (start bit included)


	do
	{
//	   printf ("\r\nWaiting...");
	   while (RxD_pin);	// Wait Start bit
	   TMR_1 = 0;
	   while (!RxD_pin);	// Wait logical 1 after 7 bits in logical 0
	   pnt_h = get_Timer1();	
	   while (RxD_pin);	// Wait logical 0 of the MSB
	   old_CCP.hardwareT = get_Timer1();	
	   while (!RxD_pin);	// Wait Stop bit
	   new_CCP.hardwareT = get_Timer1();

//	   printf ("\r\nCaracter received");

	   // Check if the transmitted value was 0x40	

	   new_CCP.hardwareT -= old_CCP.hardwareT; 	// logical 0 duty
	   old_CCP.hardwareT -= pnt_h; 	// logical 1 duty
	
//	   printf ("\r\n%lX %lX %lX",new_CCP.hardwareT,old_CCP.hardwareT,pnt_h);

	   new_CCP.hardwareT = new_CCP.hardwareT - old_CCP.hardwareT;
	   if ( new_CCP.hardwareT > 0x8000)  new_CCP.hardwareT = 0xFFFF - new_CCP.hardwareT;
	   if (new_CCP.hardwareT < 10)
	   {
		new_CCP.hardwareT = pnt_h / 7;	// 7 logical 0 duty
		new_CCP.hardwareT = new_CCP.hardwareT - old_CCP.hardwareT;
		if ( new_CCP.hardwareT > 0x8000)  new_CCP.hardwareT = 0xFFFF - new_CCP.hardwareT;

		if (new_CCP.hardwareT < 10) break; // it was 0x40, go out	
	   }
	delay_ms(2);	
	} while (1);

//	   printf ("\r\nOUT");

	BRGH  = 1;	// High Baud rates
	// SPBRG calculation: (7 bits duration * 16) / 450
	pnt_h = pnt_h << 4;
	pnt_h = pnt_h / 450;
	SPBRG = pnt_h;
	
	RESET_FLG=0;
 	CREN=0;	     /* Clear Possible Errors  */
	CREN=1;	     /* Enable Continous Receive  */
 	answer = RCREG; 
 	answer = RCREG;		// Empty RxD stack
 	answer = 0;
	Mode3_FLG = 0;

   	enable_interrupts(int_timer1);
   	enable_interrupts(int_ccp1);
   	enable_interrupts(global);


   	do 	{
  
		answer = getc();
   		CREN=0;	     /* Clear Possible Errors  */
  		CREN=1;	     /* Enable Continous Receive  */
		switch(answer)
			{
			case'H':ShowHelp();
				break;
			case'h':ShowHelp();
				break;
			case'?':ShowHelp();
				break;
			case'0':SetMode(0,0,0,0,5);
				break;
			case'1':SetMode(0,0,0,1,3);
				break;
			case'2':SetMode(0,0,1,0,5);
				break;
			case'3':SetMode(0,0,1,1,9);
				Mode3_FLG = 1;
				answer = getc();
   				CREN=0;	     /* Clear Possible Errors  */
  				CREN=1;	     /* Enable Continous Receive  */
				switch(answer)
				{
					case'1': Phases = 1;
						break;
					case'2': Phases = 2;
						break;
					case'3': Phases = 3;
						break;
					case'4': Phases = 4;
						break;
					case'5': Phases = 5;
						break;
					case'6': Phases = 6;
						break;
					case'7': Phases = 7;
						break;
					case'8': Phases = 8;
						break;
					case'9': Phases = 9;
						break;
					default: break;
				}
				break;
			case'4':SetMode(0,1,0,0,3);
				break;
			case'5':SetMode(0,1,0,1,4);
				break;
			case'6':SetMode(0,1,1,0,4);
				break;
			case'7':SetMode(0,1,1,1,5);
				break;
			case'8':SetMode(1,0,0,0,5);
				break;
			case'9':SetMode(1,0,0,1,3);
				break;
			case'A':SetMode(1,0,1,0,3);
				break;
			case'B':SetMode(1,0,1,1,3);
				break;
			case'C':SetMode(1,1,0,0,3);
				break;
			case'D':SetMode(1,1,0,1,5);
				break;
			case'E':SetMode(1,1,1,0,5);
				break;
			case'F':SetMode(1,1,1,1,5);
				break;
			default:break;
			}
		} while (!RESET_FLG);

   	do 	{
		syncr();
		if(RCIF){ 
			answer=getc();
   			CREN=0;	     /* Clear Possible Errors  */
  			CREN=1;	     /* Enable Continous Receive  */
			}
		switch(answer)
			{
			case'H':ShowHelp();
				break;
			case'h':ShowHelp();
				break;
			case'?':ShowHelp();
				break;
			case's':SetSlowMode();
				break;
			case'f':SetFastMode();
				break;
			case'p':SetPDMode();
				break;
			case't':SetCML();
				break;
			case'n':ClrCML();
				break;
			case'm':NewShowMeasurement();
				break;
			case'M':NewShowMeasurement();
				break;
			default:break;
			}
		} while (TRUE);
	}
