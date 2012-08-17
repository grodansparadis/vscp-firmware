// all hardware related routines should be placed here

#include "hardware.c"

void ini_hardware();
unsigned char ReadSwitch (unsigned char i);


#define LED_STATUS_ON       ((PORTE &= ~_BV(4)))
#define LED_STATUS_OFF      ((PORTE |= _BV(4)))
#define LED_STATUS_TOGGLE   ((PORTE ^= _BV(4)))
#define BTN_INIT_PRESSED    (!(PINE & _BV(5)))

#define BTN_SW1_PRESSED     (!(PINA & _BV(0)))
#define BTN_SW2_PRESSED     (!(PINA & _BV(1)))
#define BTN_SW3_PRESSED     (!(PINA & _BV(2)))
#define BTN_SW4_PRESSED     (!(PINA & _BV(3)))
#define BTN_SW5_PRESSED     (!(PINA & _BV(4)))
#define BTN_SW6_PRESSED     (!(PINA & _BV(5)))
#define BTN_SW7_PRESSED     (!(PINA & _BV(6)))
#define BTN_SW8_PRESSED     (!(PINA & _BV(7)))

#define outputport			PORTB
#define outputpin			PINB

#define read_output			(PINB) //should equal the output-port




#define time_debounce	10
#define time_start		100


