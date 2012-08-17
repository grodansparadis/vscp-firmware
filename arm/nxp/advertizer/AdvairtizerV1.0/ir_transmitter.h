/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Andreas Weschenfelder 2007                                   *
 *    File name   : ir_transmitter.h                                       *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Andreas Weschenfelder, Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef _IR_TRANSMITTER_H_
  #define _IR_TRANSMITTER_H_
  void init_ir_transmitter(void);

//#define PWM_FRE_RC5_S (36000)
#define PWM_MR0_RC5 (U32)((PWMTICSperSEC/PWM_FRE_RC5_S ) + 0.5)
//#define PWM_MR4INIT (U32)((PWMTICSperSEC/PWM_FREQ3) + 0.5)  //PWM-IR_transmitter
#define PWM_MR4_RC5 (PWM_MR0_RC5/2)

void enable_ir_transmitter(void);
void disable_ir_transmitter(void);
void rc5_halbbit_high(void);
void rc5_halbbit_low(void);
void rc5_bit_h(void);
void rc5_bit_l(void);
void send_rc5(char toggle_bit, char adresse, char code);



#define RC5_SENDER_DIR    F_IO0DIR         
#define RC5_SENDER_PIN    F_IO0PIN
#define RC5_SENDER_IOSET  F_IO0SET
#define RC5_SENDER_IOCLR  F_IO0CLR
#define RC5_SENDER        8         //P0.8

#define NTC_DIR    F_IO0DIR         
#define NTC_PIN    F_IO0PIN
#define NTC_IOSET  F_IO0SET
#define NTC_IOCLR  F_IO0CLR
#define NTC        28         //P0.28


void rc5_send(char toggle_bit, char adresse, char code);
void rc5_halbbit_low(void);
void rc5_halbbit_high(void);
void rc5_bit_h(void);
void rc5_bit_l(void);


U32 rc5s_duration;
U32 rc5s_startTime; 

#endif
