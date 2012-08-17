/*
#include <htc.h>
__CONFIG(0, GCP_OFF & BWP_OFF & PWP_BLKS(128) &
ICESEL_PGx2 & BKBUG_ON);
__CONFIG(1,FWDTEN_OFF & POSCMD_HS & FNOSC_PRIPLL & FPBDIV_8);
__CONFIG(2,FPLLMULT_18 & FPLLIDIV_2 & __FPLLODIV(0));
void init(void)
{
TRISD = 0xFFF8; // port direction
}
int counter;
void main(void)
{
init();
while (1){
PORTD = counter;
_delay(3000000);
counter++;
}
}
*/


#include <htc.h>
#include <plib.h>

__CONFIG( 0, GCP_OFF & 
                BWP_OFF & 
                PWP_BLKS(128) &
                ICESEL_PGx2 & 
                BKBUG_ON );
__CONFIG( 1, FWDTEN_OFF & 
                POSCMD_HS & 
                FNOSC_PRIPLL & 
                FPBDIV_8 );
__CONFIG( 2, FPLLMULT_20 & 
                FPLLIDIV_2 &
                __FPLLODIV( 1 ) );


#define FOSC 80000000
#define T1_TICK (FOSC/256/8)


extern volatile int counter;
void interrupt timer_1_isr( void ) @ TIMER_1_VCTR 
{
    mT1ClearIntFlag();
    counter++;
}


void init(void)
{
    SYSTEMConfig( FOSC, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE );
    OpenTimer1( T1_ON | T1_SOURCE_INT | T1_PS_1_256, T1_TICK );
    ConfigIntTimer1( T1_INT_ON | T1_INT_PRIOR_2 );
    INTEnableSystemMultiVectoredInt();
    mPORTDSetPinsDigitalOut(BIT_0 | BIT_1 | BIT_2);
}

volatile int counter;
void main( void ) {
    init();
    ei(); 
    while ( 1 )
    {
        mPORTDWrite( counter ); 
    }
}