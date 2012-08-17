/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : adc.h                                                  *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

/******************
* vars
******************/
U16 adcval;

/******************
* functions
******************/
  U16 ReadChannel0(U8 mux);
  U16 ReadChannel1(U8 mux);
  void init_adc(void);



