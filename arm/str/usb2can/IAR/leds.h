#ifndef __LEDS_H
#define __LEDS_H

#include "75x_lib.h"
#include "usb_lib.h"

// LED States
// ==========
// Off – No power to adapter.
// Red blinking – Not initialized.
// Green steady – Adapter connected to CAN.
// Green blinking -  USB initialized. Adapter not connected to CAN bus.
// Red + Green blinking - Bus warning
// Red steady - Bus off.

// LED Defines
#define GREEN   0
#define RED     1

#define LED_OFF                   0     // Both LED's off
#define LED_RED_STEADY            1     // RED steady light
#define LED_GREEN_STEADY          2     // GREEN steady light
#define LED_RED_BLINK_FAST        3     // RED fast blinking
#define LED_RED_BLINK_SLOW        4     // RED slow blinking
#define LED_GREEN_BLINK_FAST      5     // GREEN fast blinking
#define LED_GREEN_BLINK_SLOW      6     // GREEN slow blinking
#define LED_RED_GREEN_BLINK_SLOW  7     // RED/GREEN slow blinking
#define LED_RED_GREEN_BLINK_FAST  8     // RED/GREEN fast blinking
#define LED_TOGGLE_FAST           9
#define LED_TOGGLE_SLOW           10
#define LED_ON                    11  

#define SLOW_BLINKING_TIME        500  //1000
#define FAST_BLINKING_TIME        100  // 500

extern void ledOn( u8 nled );
extern void ledOff( u8 nled );
extern void setBlinkType( u8 type );
extern void blink( void );
extern volatile bool CANAdapter_stateChange;

// blink
extern u32 blink_state_time;
extern u8 blink_state;
extern u8 blink_type;

#endif
