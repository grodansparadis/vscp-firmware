// File canusb.h
//


// LED States
// ==========
// Off – No power to adapter.
// Red blinking – Not initialized.
// Green steady – Adapter connected to CAN.
// Green blinking -  USB initialized. Adapter not connected to CAN bus.
// Red + Green blinking - Bus warning
// Red steady - Bus off.

#define LED_OFF                   0     // Both LED's off
#define LED_RED_STEADY            1     // RED steady light
#define LED_GREEN_STEADY          2     // GREEN steady light
#define LED_RED_BLINK_FAST        3     // RED fast blinking
#define LED_RED_BLINK_SLOW        4     // RED slow blinking
#define LED_GREEN_BLINK_FAST      5     // GREEN fast blinking
#define LED_GREEN_BLINK_SLOW      6     // GREEN slow blinking
#define LED_RED_GREEN_BLINK_SLOW  7     // RED/GREEN slow blinking
#define LED_RED_GREEN_BLINK_FAST  8     // RED/GREEN fast blinking


// Commands
// ========
// 

// No operation
#define CANUSB_COMMAND_NOOP               0   

// Set bitrate
//  argument:
//    byte 1:
//      format:
//        0x00 - preset bitrate
//        0x01 - register data
//    byte 2:
//      0x00 - 10 Kbit
//      0x01 - 20 Kbit
//      0x02 - 50 Kbit
//      0x03 - 100 Kbit
//      0x04 - 125 Kbit
//      0x05 - 250 Kbit
//      0x06 - 500 Kbit
//      0x07 - 800 Kbit  
//      0x08 - 1 Mbit
//      or MSB Config
//    byte 3:
//        LSB Config
//    byte 4:
//        BRPR
//  
#define CANUSB_COMMAND_SET_BITRATE        1

// Set filter
//  argument:
//      none
#define CANUSB_COMMAND_SET_FILTER         2

// Set  mask
//  argument:
#define CANUSB_COMMAND_SET_MASK           3

// Open CAN channel
//  argument:
//    byte 1
//      0x00 - normal
//      0x01 - test - silent
//      0x02 - test - loopback
#define CANUSB_COMMAND_OPEN               4

// Close CAN channel
//  argument:
//      none
#define CANUSB_COMMAND_CLOSE              5

// Get status
//  argument:
//    byte 1
//      0x00 - standard.
#define CANUSB_COMMAND_GET_STATUS         6

// Get version
//  argument:
//      0x00 - Hardware version.
//      0x01 - Firmware version.
#define CANUSB_COMMAND_GET_VERSION        7

// Get adapter serial number
//  argument:
//      0x00 - standard.
#define CANUSB_COMMAND_GET_SERIALNO       8

// Autofind bitrate
//  argument:
//      none
#define CANUSB_COMMAND_FIND_BITRATE       9

// Get frame statistics
//  argument:
//      none
#define CANUSB_COMMAND_GET_STAT_FRAME     10

// Get  data statistics
//  argument:
//      none
#define CANUSB_COMMAND_GET_STAT_DATA      11

// Get error statistics
//  argument:
//      none
#define CANUSB_COMMAND_GET_STAT_ERROR     12

// Clearstatistics
//  argument:
//      optional bitfield
#define CANUSB_COMMAND_CLR_STAT           13

//
// Errors
//

// No error
#define CANUSB_ERROR_NONE               0

// Bus warning
//  argument:
//    byte 1: 
//      0 - Going away from bus warning
//      1 - Bus warning occured.
//    Byte 2:
//      Last error code
#define CANUSB_ERROR_BUS_WARNING        1

// Error passive
//  argument:
//    byte 1: 
//      0 - Going away from error passive
//      1 - error passive occured.
//    Byte 2:
//      Last error code
#define CANUSB_ERROR_PASSIVE            2

// Bus off
//  argument:
//    byte 1: 
//      0 - Going away from bus off
//      1 - bus off occured.
//    Byte 2:
//      Last error code
#define CANUSB_ERROR_BUS_OFF            3