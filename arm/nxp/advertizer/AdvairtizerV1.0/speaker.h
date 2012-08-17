/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : speaker.h                                              *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

/***************************************************************************
 * port define
 ***************************************************************************/

void PWM0ISR(void);
void enable_timer1(void);
void disable_timer1(void);
void init_speaker(void);
void piep_ok(void);
void piep_error(void);
void sound_init(void);
void play_tune(void);



