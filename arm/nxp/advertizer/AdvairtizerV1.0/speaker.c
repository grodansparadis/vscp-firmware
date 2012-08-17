/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : speaker.c                                              *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>
// SineTable
const unsigned char SineTable[256] = {133,136,139,142,
                             145,149,152,155,
                             158,161,164,167,
                             169,172,175,178,
                             181,184,186,189,
                             192,194,197,200,
                             202,205,207,209,
                             212,214,216,218,
                             220,222,224,226,
                             228,230,232,233,
                             235,237,238,240,
                             241,242,243,245,
                             246,247,248,248,
                             249,250,251,251,
                             252,252,252,253,
                             253,253,253,253,
                             253,253,252,252,
                             252,251,251,250,
                             249,248,248,247,
                             246,245,243,242,
                             241,240,238,237,
                             235,233,232,230,
                             228,226,224,222,
                             220,218,216,214,
                             212,209,207,205,
                             202,200,197,194,
                             192,189,186,184,
                             181,178,175,172,
                             169,167,164,161,
                             158,155,152,149,
                             145,142,139,136,
                             133,130,127,124,
                             121,118,115,112,
                             109,105,102,99,
                             96,93,90,87,
                             85,82,79,76,
                             73,70,68,65,
                             62,60,57,54,
                             52,49,47,45,
                             42,40,38,36,
                             34,32,30,28,
                             26,24,22,21,
                             19,17,16,14,
                             13,12,11,9,
                             8,7,6,6,
                             5,4,3,3,
                             2,2,2,1,
                             1,1,1,1,
                             1,1,2,2,
                             2,3,3,4,
                             5,6,6,7,
                             8,9,11,12,
                             13,14,16,17,
                             19,21,22,24,
                             26,28,30,32,
                             34,36,38,40,
                             42,45,47,49,
                             52,54,57,60,
                             62,65,68,70,
                             73,76,79,82,
                             85,87,90,93,
                             96,99,102,105,
                             109,112,115,118,
                             121,124,127,127};
#define SAMPLE_RATE 3686                
#define FREQSCALE   383//1048               
        
static U16 sample;                 // value that eventually gets output to DAC
static U32 phaseAccum = 0;         // phase accumulator
static U32 phaseInc = 0;           // phase increment
static U8  amplitude = 255;        // oscillator amplitude

static int mainVolume = 255;
void synth_freq(U16 freq){
    phaseInc = freq * FREQSCALE; 
}

void synth_amp(int amp){
    amplitude = amp; 
}

char TEXT_SONG1[]      =   "Sirene1";
const U16 Sirene1[] = {
            0,
            32,400, 32,397, 32,394, 32,391, 32,388, 32,385, 32,382, 32,379,
            32,376, 32,373, 32,370, 32,367, 32,364, 32,361, 32,358, 32,355,
            32,352, 32,349, 32,346, 32,343, 32,340, 32,337, 32,334, 32,331, 
            32,328, 32,325, 32,322, 32,319, 32,316, 32,313, 32,310, 32,307, 
            32,304, 32,301, 32,298, 32,298, 32,301, 32,304, 32,307, 32,310, 
            32,313, 32,316, 32,319, 32,322, 32,325, 32,328, 32,331, 32,334, 
            32,337, 32,340, 32,343, 32,346, 32,349, 32,352, 32,355, 32,358, 
            32,361, 32,364, 32,367, 32,370, 32,373, 32,376, 32,379, 32,382, 
            32,385, 32,388, 32,391, 32,394, 32,397, 32,400,
            0, 1
        };


  U16 *Songs[]    = {Sirene1,0};//Sirene2, 0};
  U16 *pSong;     // pointer to the different songs in flash
  char *TEXT_SONG_TBL[]    = { TEXT_SONG1};//,TEXT_SONG2}
  static char Tone = 0;

/**********************************************************************
 * PWM0 Interrupt
 * The Interrupt Service Routine (ISR) is running @
 * PWM_MR0INIT (44khz).
***********************************************************************/
U16 pwmzaehler = 0;
U32 sinuswert = 0;

void PWM0ISR(void){
    switch(PWMIR) {
      default:
        break;
    case PWMIR_MR6I:
    phaseAccum += phaseInc;                             //
    sample = SineTable[(phaseAccum >> 16) & 0x000000ff];//
    /* adjust amplitude */
    sample *= amplitude;
    sample >>= 8; 
if(sample == 0)
  sample = 1;
            PWMMR6 = (sample * 18) /10;                 //
            PWMLER |= (1<<6);                           // Enable PWM Match6 Latch
      break;
    }
    PWMIR |= PWMIR_MR6I;                                // 0xffff clears the interrupt.
} 

/*****************************************************************************
*
*   Function name : Play_Tune
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Plays the song
*
*****************************************************************************/
void play_tune(void){
 PWMPCR |= (1<<PWMENA6);                    // enable outputs for PWM6
 synth_amp(255);                            // full volume
    static char song = 0;
        pSong = Songs[song];                   // point to this song             
        Tone = 0;                           //Start the song from the beginning

while(Tone<142){
 synth_freq(*(pSong + Tone));               // play 440 Hz for 2 seconds
  Tone++; 
 pause(ONE_MS * (*(pSong + Tone)));
  Tone++; 
  }
  Tone=0;
 PWMPCR &= ~(1<<PWMENA6);                   // disable outputs for PWM6
 synth_amp(0);                              // volume
}
 
/***************************************************************************
 * init speaker
 ***************************************************************************/
void init_speaker(void){
    sound_init();
	}

/**********************************************************************
 * Piep in ms
***********************************************************************/
void piep(U16 zeit, uint16_t frq){

 PINSEL0 |= (U32)(0x02<<18);                // select PWM6
 PWMPCR |= (1<<PWMENA6);                    // enable outputs for PWM6
 synth_amp(255);                            // full volume
 synth_freq(frq);
 pause(ONE_MS * zeit);
 synth_amp(0);                              // volume off
 PWMPCR &= ~(1<<PWMENA6);                   // disable outputs for PWM6
 PINSEL0 &= ~((1<<18) | (1<<19));           // select GPIO
 F_IO0SET = BUZZER;                         // workaround to get a defined pin state
}

/**********************************************************************
 * Piep ok
***********************************************************************/
void piep_ok(void){
    piep(60, 700);
    piep(60, 1000);
    piep(90, 1500);
        }
/**********************************************************************
 * Piep error
***********************************************************************/
void piep_error(void){
    piep(50, 700);
    pause(ONE_MS*20);
    piep(50, 700);
    pause(ONE_MS*20);
    piep(50, 700);
    pause(ONE_MS*20);
    piep(50, 700);
    pause(ONE_MS*20);
    piep(50, 700);
	}
/*
not used
void enable_timer1(void){
    T1TCR = 1;  // Enable 
}
void disable_timer1(void){
    T1TCR = 0;  // disable 
    T1TC = 0; //reset Timer Counter
}
*/

/**********************************************************************
 * sound init
***********************************************************************/
void sound_init(void)
{
	// set function of Pin P0.9 PWM6
        PINSEL0 |= (U32)(0x02<<18); //PWM6 function select register
        PWMMR6 = 150;               //
        PWMPCR &= ~(1<<PWMSEL6);    // single edge mode for PWM6
        PWMLER |= (1<<6);           // Enable PWM Match6 Latch
        PWMPCR |= (1<<PWMENA6);     // enable outputs for PWM6
        PWMMCR |= (1<<18);          // Interrupt on PWMMR6: An interrupt is generated when PWMMR6 matches the value
        PWMIR |= PWMIR_MR6I;        // clears the interrupt.
        ctl_set_isr(SIR_PWM0, PRIO_PWM0, CTL_ISR_TRIGGER_FIXED, PWM0ISR, 0);
        ctl_unmask_isr(SIR_PWM0);
}

