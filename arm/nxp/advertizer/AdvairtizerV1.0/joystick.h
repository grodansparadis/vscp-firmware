/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : joystick.h                                             *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

void get_joystick(void);
void init_joystick(void);
int debounce(U32 port, U32 button);
int get_stick_up(void);
int get_stick_down(void);
int get_stick_left(void);
int get_stick_right(void);
int get_stick_button(void);

#define KEY_DEBOUNCE TWENTY_MS
#endif
