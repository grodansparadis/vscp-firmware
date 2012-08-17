/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Andreas Weschenfelder 2007                                   *
 *    File name   : ir_transmitter.c                                       *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Andreas Weschenfelder, Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>

/**********************************************
* IR_PWM = P0.08 Infrarot transmit Diode PWM
* NTC_IR_TX = P0.28 Analog in for NTC or IR LED TX Pulses
***********************************************/
  void init_ir_transmitter(void){
    PINSEL0 |= (U32)(0x02<<16); // Connect P0.08 to PWM4
    PINSEL1 &= (U32) ~((1<<25) | (1<<24)) ; // Connect P0.28 to NTC_IR_TX

	PWMPCR &= ~(1<<4);  // single edge mode for PWM4
        PWMPCR &= ~(1<<12);     // disable outputs for PWM4
        PWMTCR = PWMCR_ENABLE | (1<<3); // Counter Enable & PWM Enable
        PWMMR0 = PWM_MR0_RC5;
        PWMMR4 = PWM_MR4_RC5; //50% Duty
	PWMLER |= (1<<4);  // Enable PWM Match4 Latch  
  }

void disable_ir_transmitter(void){
PINSEL0 &= (U32)~(0x03<<16); // Connect P0.08 to GPIO
F_IO0CLR = IR_PWM;           // set IR LED to Off, workaround to get a defined pin state
PWMPCR &= ~(1<<12);          // disable outputs for PWM4
PWMMR0 = PWM_MR0INIT;        // set PWM0 to main pwm frq (44khz)
}
void enable_ir_transmitter(void){
PINSEL0 |= (U32)(0x02<<16); // Connect P0.08 to PWM4
PWMMR0 = PWM_MR0_RC5;       // set PWM0 to 36khz for IR Transmitter
PWMPCR |= (1<<12);          // enable outputs for PWM4
}

//-> T=1778us ->T_halbbit = 889us
void rc5_halbbit_high(void){
U32 rc5s_startTime;
enable_ir_transmitter();
rc5s_startTime = getSysTICs();
while ((getElapsedSysTICs(rc5s_startTime) / ONE_US) < 889)
  {;}
disable_ir_transmitter();
}

//-> T=1778us ->T_halbbit = 889us
void rc5_halbbit_low(void){
disable_ir_transmitter();
U32 rc5s_startTime;
rc5s_startTime = getSysTICs();
while ((getElapsedSysTICs(rc5s_startTime) / ONE_US) < 889)
  {;}
}

void rc5_bit_h(void)
{
rc5_halbbit_low();
rc5_halbbit_high();
}

void rc5_bit_l(void)
{
rc5_halbbit_high();
rc5_halbbit_low();
}

void send_rc5(char toggle_bit, char adresse, char code){
int i;

//Clear IR-Receiver-Int
ctl_mask_isr(SIR_EINT3);

//Startbits
rc5_bit_h();
rc5_bit_h();

//Toggle
if (toggle_bit)
  {rc5_bit_h();}
  else
  {rc5_bit_l();}

//Adresse
for (i=0;i<5;i++)
  {
  if (adresse & 0x10)
    {rc5_bit_h();}
    else
    {rc5_bit_l();}
  adresse = adresse << 1;
  }

for (i=0;i<6;i++)
  {
  if (code & 0x20)
    {rc5_bit_h();}
    else
    {rc5_bit_l();}
  code = code << 1;
  }
disable_ir_transmitter();
ctl_unmask_isr(SIR_EINT3);
}

