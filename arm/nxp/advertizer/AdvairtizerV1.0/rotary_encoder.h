/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : rotary_encoder.h                                       *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

void rotary_direction(void);
unsigned char get_rotary_encoderdirection(void);
void init_rotary_encoder(void);
int get_rotary_encoderA(void);
int get_rotary_encoderB(void);
int get_rotary_encoderButton(void);

#define ROTARY_ENCODER_DEBOUNCE TWENTY_MS

#endif
