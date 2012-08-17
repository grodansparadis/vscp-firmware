#include <p32xxxx.h>    
#include <plib.h>
#include "audio.h"

// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 20 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
//
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_4

// Interrupt Definition Macros
#define ISR(PRIOR,VECT) void __attribute__ ((interrupt(PRIOR))) __attribute__ ((vector(VECT))) interrupt ## VECT (void)
#define INTERRUPT(PRIOR,VECT,NAME) void __attribute__ ((interrupt(PRIOR))) __attribute__ ((vector(VECT))) NAME (void)

#define SYS_FREQ 				(80000000L)
#define PBCLK                   (20000000L)

int adc_mic;

///////////////////////////////////////////////////////////////////////////////
// Timer 1 Interrupt
//      bitclock

static volatile int pos = 0;    // buffer position
INTERRUPT( ipl2, _TIMER_1_VECTOR, Timer1Handler )
{
    // clear the interrupt flag
    mT1ClearIntFlag();
    
    if ( pos < AUDIO_DATA_SIZE ) { 
		pos++;
	}
    else {
        // Start again 
		pos = 0;
	}
	
	SetDCOC4PWM( play_data[ pos ] );
    SetDCOC5PWM( play_data[ pos ] );
}   

///////////////////////////////////////////////////////////////////////////////
// ADC Conversion Interrupt
//

INTERRUPT( ipl1, _ADC_VECTOR, ADCisrHandler )
{
    int offset;
    mAD1ClearIntFlag(); 
    offset = ( ( ( ~ReadActiveBufferADC10() ) & 0x01 ) * 8 );
    adc_mic = ReadADC10( 0 + offset );   // Read microphone level
} 
     
///////////////////////////////////////////////////////////////////////////////
// main
//
        
main()
{ 
	// Config Prefetch Cache 
    SYSTEMConfig( SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE );
    
    // Configure ADC subsystem
    CloseADC10();    
    
    // configure to sample AN4 & AN5
    SetChanADC10( ADC_CH0_NEG_SAMPLEA_NVREF | ADC_CH0_POS_SAMPLEA_AN0  );
         
    OpenADC10( ADC_MODULE_ON |                  // Enable module
                    ADC_FORMAT_INTG |           // Integer format
                    ADC_CLK_AUTO |              // Automatic clock
                    ADC_AUTO_SAMPLING_ON ,      // 
               ADC_VREF_AVDD_AVSS | 
                    ADC_OFFSET_CAL_DISABLE | 
                    ADC_SCAN_OFF | 
                    ADC_SAMPLES_PER_INT_2 | 
                    ADC_ALT_BUF_ON | 
                    ADC_ALT_INPUT_ON,
              SKIP_SCAN_ALL,      
              ADC_CONV_CLK_INTERNAL_RC | 
                    ADC_SAMPLE_TIME_12,
              ENABLE_AN0_ANA               // Microphone
             );
    EnableADC10();
    ConfigIntADC10( ADC_INT_PRI_1 | ADC_INT_ON ); 
			  
	// Setup timer 2
    CloseTimer2();
    OpenTimer2( T2_ON | T2_PS_1_1, 0x100 );
    
    // Setup OC4
    CloseOC4();
    OpenOC4( OC_ON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_CONTINUE_PULSE | OC_LOW_HIGH , 
				0, 
				0 );
				
	// Setup OC5
    CloseOC5();
    OpenOC5( OC_ON | OC_TIMER_MODE16 | OC_TIMER2_SRC | OC_CONTINUE_PULSE | OC_LOW_HIGH , 
				0, 
				0 );			
	
	// Config Timer 1 for bitclock
    OpenTimer1( T1_ON | T1_SOURCE_INT | T1_PS_1_1, 
					( PBCLK / ( 1 * 8000 ) ) );		    
    ConfigIntTimer1( T1_INT_ON | T1_INT_PRIOR_2 );		
	
    INTEnableSystemMultiVectoredInt();
	    
    while ( 1 ) {
		;
    }
}
