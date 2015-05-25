/* 
 * File:   driver.h
 * Author: Musumeci Salvatore
 *
 * Aim of this module is to provided an abstrated layer for your VSCP application.
 * This part of code is demanded to inteface the HW with your code, ignoring the HW variant
 *
 * For example, if you have to hardware variant where same pin is connected in two different pin, your code will not change
 * Will be this interface that will be recompiled in order to support the change.
 *
 * Also, this interface is able to handling the CAN message for input
 * Obviously all parameters could be customized using normal VSCP register access
 * Created on 10 agosto 2014, 2.47
 */

#ifndef DRIVER_H
#define	DRIVER_H

#include <xc.h>
#include <inttypes.h>

#ifdef	__cplusplus
extern "C" {
#endif

/*  EEPROM LAYOUT
 *
 *  Config input: 1byte x PIN_IN_SIZE
 *  Zone for input: 1 byte x PIN_IN_SIZE
 *  Subzone for input: 1byte x PIN_IN_SIZE
 *  Config output: 1byte x PIN_OUT_SIZE
 *  Subzone for output: 1byte x PIN_OUT_SIZE
 *
 */

#define OFFSET_PORT_TO_TRIS 0x12
#define OFFSET_LAT_TO_TRIS 0x09
#define PIN_OUT_SIZE 8
#define PIN_IN_SIZE 8

//VSCP button and led definition
#define vscp_ledPin  PORTAbits.RA2
#define vscp_ledTris TRISAbits.TRISA2
#define vscp_btnPin  PORTBbits.RB5
#define vscp_btnTris TRISBbits.TRISB5
#define greenLed_pin PORTCbits.RC7
#define greenLed_tris TRISCbits.TRISC7
#define redLed_pin   PORTCbits.RC6
#define redLed_tris  TRISCbits.TRISC6

#define PICKIT_CH1 PORTBbits.RB7
#define PICKIT_CH2 PORTBbits.RB6

typedef struct {
    unsigned   _1mS :1;
    unsigned  _10mS :1;
    unsigned _100mS :1;
    unsigned    _1s :1;
}timeBasedEventStruct;

extern uint8_t vscp_zone;
extern struct _omsg vscp_omsg;

void hardware_setup();
void setOutput (unsigned char pin, unsigned char state);
uint8_t getInput (unsigned char pin);
void TMR0_interrupt();
void hardware_freeRunning();


#define VSCP_BOARD_EEPROM_LENGTH 3*PIN_IN_SIZE + 2*PIN_OUT_SIZE
#define TMR0H_INIT 0x63
#define TMR0L_INIT 0xC0


#ifdef	__cplusplus
}
#endif

#endif	/* DRIVER_H */

