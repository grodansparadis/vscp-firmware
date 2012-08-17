/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : rotary_encoder.c                                       *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#include <global.h>

 volatile char	enc_delta;		// -128 ... 127

/**********************************************************************
 * init_rotary_encoder
 **********************************************************************/
void init_rotary_encoder(void){
  PINSEL2 &= ~(1<<3);//Pins P1.25-16 are used as GPIO pins.
}
/**********************************************************************
 * init_rotary_encoder abfrage
***********************************************************************/
unsigned char get_rotary_encoderdirection(void){
  return enc_delta;
}
int get_rotary_encoderA(void){
  return F_IO1PIN & STICK_LEFT_BIT ? FALSE : TRUE;// debounce(F_IO1PIN, STICK_UP_BIT);
}
int get_rotary_encoderB(void){
  return F_IO1PIN & STICK_RIGHT_BIT ? FALSE : TRUE;//debounce(F_IO1PIN, STICK_DOWN_BIT);
}
int get_rotary_encoderButton(void){
  return F_IO1PIN & STICK_BUTTON_BIT ? FALSE : TRUE;//debounce(F_IO1PIN, STICK_DOWN_BIT);
}

void rotary_direction(void){

 static char enc_last = 0x01;
 char i = 0;

  if( get_rotary_encoderA() )
    i = 1;

  if( get_rotary_encoderB() )
    i ^= 3;				// convert gray to binary

  i -= enc_last ;			// difference new - last

  if( i & 1 ){				// bit 0 = value (1)
    enc_last += i;			// store new as next last
    if (!(get_rotary_encoderA() & get_rotary_encoderA() & 1))    // change only for 00/10 in binary code
    enc_delta += (i & 2) - 1;		// bit 1 = direction (+/-)
  }
}

