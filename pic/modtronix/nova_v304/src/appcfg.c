/*********************************************************************
 * appcfg.c    -    Application Configuration functions
 * Dependencies:    
 * Processor:       PIC18
 * Complier:        MCC18 v1.00.50 or higher
 *                  HITECH PICC-18 V8.35PL3 or higher
 * Company:         Modtronix Engineering - www.modtronix.com
 *
 *********************************************************************
 * Description
 *
 * Application configuration.
 *
 *********************************************************************
 * File History
 *
 * 2004-10-19, David Hosken (DH):
 *    - Initial version
 *
 *********************************************************************
 * Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ********************************************************************/
#define THIS_IS_APPCFG_MODULE

#include "projdefs.h"
#include "appcfg.h"
#include "debug.h"

#include <vscp_2.h>

/////////////////////////////////////////////////
//Debug defines
#define debugPutMsg(msgCode) debugPut2Bytes(0xD0, msgCode)

/////////////////////////////////////////////////
//Uncomment following lines if BAUD rate is statically set. They will calculate SPBRG1 and SPBRGH1
//USART1 is used in 16 bit baud rate generator mode, BRGH high, BRG16 high
//#define BAUD_RATE1       (57600)    // bps
//#define SPBRG1_VAL   ((((CLOCK_FREQ/BAUD_RATE1)/4) - 1) & 0xff)
//#define SPBRGH1_VAL   (((((CLOCK_FREQ/BAUD_RATE1)/4) - 1) >> 8) & 0xff)
//#if SPBRG1_VAL > 255
//    #error "Calculated SPBRG value is out of range for currnet CLOCK_FREQ."
//#endif


/////////////////////////////////////////////////
// Variables

APP_CONFIG AppConfig;   // This is used by other stack elements.

// Array containing converted ADC values
#if defined(APP_USE_ADC8)
BYTE AdcValues[ADC_CHANNELS];
#elif defined(APP_USE_ADC10)
WORD AdcValues[ADC_CHANNELS];
#endif


/**
 * @preCondition  fsysInit() is already called.
 */
void appcfgInit(void)
{
    BYTE *p;
    WORD addr;

    /////////////////////////////////////////////////
    //Load configuration from EEPROM to RAM
    //Read contents of BLCONNFIG structure from EEPROM. It is located at the start of the EEPROM
    addr = 0;
    
    //Configure for EEPROM reading, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x3F;

    p = (BYTE*)&AppConfig;

    for ( ; addr < sizeof(AppConfig); addr++ ) {
	    EEADRH = (addr >> 8) & 0xff;
	    EEADR = addr & 0xff;
        EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.
        *p++ = EEDATA;
    }
}

/**
 * Get a specified byte from the APP_CONFIG configuration structure
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get
 *
 * @return Returns the requested byte
 */
BYTE appcfgGetc(WORD offset) {
	
    // Set address
    EEADR = offset & 0xff;
    EEADRH = (offset >> 8 ) & 0xff;

    // Wait if something is currently being written
    while(EECON1_WR);

    // Configure for EEPROM reading, EEPGD=0, CFGS=0
    EECON1 &= 0x3F;

    EECON1_RD = 1;  //After setting this bit, the data is available on next cycle. Is cleared by hardware.
    return EEDATA;
}

/**
 * Set a specified byte from the APP_CONFIG configuration structure with the data contained
 * in the EEDATA byte.
 *
 * @param offset Contains the offset in the APP_CONFIG structure of the byte to get
 *
 * @return Returns the requested byte
 */
void _appcfgPutc(WORD offset) {
    
    // Set address
    EEADR = offset & 0xff;
    EEADRH = (offset >> 8 ) & 0xff;
    
    // Wait if something is currently being written
    while(EECON1_WR);

    // Configure for EEPROM writing, EEPGD=0, CFGS=0, FREE=0
    EECON1 &= 0x2F;

    EECON1_WREN = 1;        //Write enable
    EECON2      = 0x55;
    EECON2      = 0xAA;
    EECON1_WR   = 1;
    Nop();
    
    // Wait for write to finish
    while(EECON1_WR);
    EECON1_WREN = 0;

    return;
}

/**
 * Save Application Configuartion data to EEPROM
 */
void appcfgSave(void)
{
    /* Currently not required
    BYTE c;
    BYTE *p;

    p = (BYTE*)&AppConfig;

    for ( c = 0; c < sizeof(AppConfig); c++ ) {
        EEDATA=*p++;
        _appcfgPutc(c);
    }
    */
}


/*
 * Configure USART with AppConfig data
 */
void appcfgUSART(void) 
{
    WORD w;

    //Initialize USART1 Control registers
    TXSTA = 0b00100100;     //High BRG speed
    RCSTA = 0b10010000;
    BAUDCON = 0b00001000;

#if (CLOCK_FREQ == 40000000)

    //Get USART BAUD rate setting
    switch ( appcfgGetc( APPCFG_USART1_BAUD ) )
    {
    case BAUD_300:
        w = 33332;
        break;
    case BAUD_1200:
        w = 8332;
        break;
    case BAUD_2400:
        w = 4165;
        break;
    case BAUD_4800:
        w = 2082;
        break;
    case BAUD_9600:
        w = 1040;
        break;
    case BAUD_19200:
        w = 520;
        break;
    case BAUD_38400:
        w = 259;
        break;
    case BAUD_57600:
        w = 172;
        break;
    case BAUD_115200:
        w = 86;
        break;
    default:
        w = 172;
        break;
    }

    SPBRG =  (BYTE)w;
    SPBRGH = (BYTE)(w >> 8);
#else
#error "appcfgUSART() does not support selected clock frequency"
#endif
}

/*
 * Configure the CPU I/O ports direction - Input or output
 */
void appcfgCpuIO(void)
{
    TRISA = appcfgGetc( APPCFG_TRISA );

    // Ensure following TRISB ports are always inputs when NIC INT0 is enabled:
    #if defined( NIC_DISABLE_INT0 )
        TRISB = appcfgGetc( APPCFG_TRISB ); 
    #else
        //B0 = Input from RTL8019AS
        TRISB = appcfgGetc( APPCFG_TRISB) | 0x01;
    #endif

    //Ensure following TRISC ports are always inputs:
    //C3 = Is controlled by I2C unit
    //C4 = Is controlled by I2C unit
    //C6 = Is controlled by USART
    //C7 = Is controlled by USART
    TRISC = appcfgGetc( APPCFG_TRISC ) | 0xd8;

    //Set PORT G and F direction bits
    TRISF = appcfgGetc( APPCFG_TRISF );
    
    //Ensure following TRISG ports are always inputs:
    //G4 = Is connected to IOCHRDY
    TRISG = appcfgGetc( APPCFG_TRISG ) | 0x10;
}

/*
 * Configure the CPU I/O ports default power up values
 */
void appcfgCpuIOValues(void)
{
    // Set PORT A,B,D and F direction bits
    LATA = appcfgGetc( APPCFG_PORTA );
    LATB = appcfgGetc( APPCFG_PORTB );
    LATC = appcfgGetc( APPCFG_PORTC );
    LATF = appcfgGetc( APPCFG_PORTF );
    LATG = appcfgGetc( APPCFG_PORTG );
}

/*
 * Configure the ADC unit
 */
void appcfgADC(void)
{
    ADCON1 = appcfgGetc( APPCFG_ADCON1 );

    // Set ADFM bit
    ADCON2 = appcfgGetc( APPCFG_ADCON2 ) | 0x80;
}

/*
 * Configure PWM
 */
void appcfgPWM(void)
{
	// Timer 2 is used for PWM
	// Output is on pin PC2
	
	// 2.44kHz 14-bit resolution
	PR2 = 0xff;	// PWM Period
	
	// Duty cycle = 0
	CCP1CON = 0x0c; 
	CCPR1L = 0x00;	
	
	// Start the timer
	T2CON = 0x07;
	
	// RC2 is output now
	appcfgPutc( APPCFG_TRISC, appcfgGetc( APPCFG_TRISC ) & 0xfb );
}

//This contains the default values of the APP_CONFIG structure, and has to be placed at off
//0 of the EEPROM = 0xf00000.
//To do this with HiTech PICC18:
//  add "-L-Peeconfig=f00000h" to the linker command line (without the " characters)
//To do this with MPLAB C18 compiler:
//  nothing has to be done!
//
#if defined(HI_TECH_C)  //Hi-Tech compiler
    #pragma psect const=eeconfig
#elif defined(__18CXX)  //MPLAB C18 compiler
    #pragma romdata EESECT=0xf00000
    //#pragma romdata eeconfig=0xf00000
#endif
ROM BYTE eeconfigArray[] = {
    //MyIPAddr
    MY_DEFAULT_IP_ADDR_BYTE1,   //Offset 0
    MY_DEFAULT_IP_ADDR_BYTE2,
    MY_DEFAULT_IP_ADDR_BYTE3,
    MY_DEFAULT_IP_ADDR_BYTE4,
    //MyMACAddr
    MY_DEFAULT_MAC_BYTE1,       //Offset 4
    MY_DEFAULT_MAC_BYTE2,
    MY_DEFAULT_MAC_BYTE3,
    MY_DEFAULT_MAC_BYTE4,
    MY_DEFAULT_MAC_BYTE5,
    MY_DEFAULT_MAC_BYTE6,
    //MyMask
    MY_DEFAULT_MASK_BYTE1,      //Offset 10
    MY_DEFAULT_MASK_BYTE2,
    MY_DEFAULT_MASK_BYTE3,
    MY_DEFAULT_MASK_BYTE4,
    //MyGateway
    MY_DEFAULT_GATE_BYTE1,      //Offset 14
    MY_DEFAULT_GATE_BYTE2,
    MY_DEFAULT_GATE_BYTE3,
    MY_DEFAULT_GATE_BYTE4,
    //wait4bl - Delay how long we wait for the bootloader, value from 0-n. Time = 0.8sec x n
    //Default value = 2.4
    4,
    //flashHasProgram - If 1, the FLASH has been programmed and contain a valid program.
    1,
    //Startup serial delay
    40,                         //Offset 20 = 40 x 50ms = 2 seconds
    //System Flags
    //xxxx xxx1 = PLL Enabled
    //xxxx xx1x = Blink LED on B7
    0x03,                       //Offset 21
    //Network Flags
    //xxxx xxx0 = DHCP Disabled
    0x00,                       //Offset 22
    //SMTPServerAddr
    MY_DEFAULT_IP_ADDR_BYTE1,
    MY_DEFAULT_IP_ADDR_BYTE2,
    MY_DEFAULT_IP_ADDR_BYTE3,   //Offset 25
    MY_DEFAULT_IP_ADDR_BYTE4,
    //TFTPServerAddr
    MY_DEFAULT_IP_ADDR_BYTE1,
    MY_DEFAULT_IP_ADDR_BYTE2,
    MY_DEFAULT_IP_ADDR_BYTE3,
    MY_DEFAULT_IP_ADDR_BYTE4,   //Offset 30
    //USART1_CFG.baud
    BAUD_57600,
    //USART1_CFG.cfg
    0,
    //USART2_CFG.baud
    BAUD_57600,
    //USART2_CFG.cfg
    0,
    //PORT_CFG - 7 bytes for port A, B, C, D, E, F and G. A 0 bit=output, 1 bit=input
    0b11111111, //Port A - All inputs   - Offset 35
	// PortB
	// RB0 - INT0 - Input
	// RB1 - INT1 - Input
	// RB2 CAN TX - output
	// RB3 CAN RX - input
    #if defined(_18F6680) || defined(__18F6680)
    0b00001000, //Port B - All outputs
	#endif
    #if defined(_18F6621) || defined(__18F6621)
    0b00000000, //Port B - All outputs
	#endif
    0b11111011, //Port C - All inputs excep RC2 whis is PWM
    0b11111111, //Port D - All inputs
    0b11111111, //Port E - All inputs
    0b11111111, //Port F - All inputs   - Offset 40
    0b11111111, //Port G - All inputs
    //PORT_VALUE - 7 bytes for port A, B, C, D, E, F and G. Gived default values for output ports
    0b00000000, //Port A - All 0s
    0b00000000, //Port B - All 0s
    0b00000000, //Port C - All 0s
    0b00000000, //Port D - All 0s       - Offset 45
    0b00000000, //Port E - All 0s
    0b00000000, //Port F - All 0s
    0b00000000, //Port G - All 0s       - Offset 48
    //ADC_CFG - adcon1                  - Offset 49
    0x06,   	// A0 - A8  RA0, RA1, RA2, RA3, RA5, RF0, RF1, RF2, RF3
    //ADC_CFG - adcon2                  - Offset 50
    0x2e,   // xxxx x110 - Fosc/64 clock = 1.6us @ 40MHz
            // xx10 1xxx - 12 Tad = 12 x 1.6 = 19.2uS conversion time
            // 0xxx xxxx - Left justified
    //SerialNumber
    0,                                  // Offset 51
    0,
    /** Username - Is a null terminated string. Max 8 char + null termination char*/
    'a',                                // Offset = 53
    'd',                                // Offset = 54
    'm',                                // Offset = 55
    'i',                                // Offset = 56
    'n',                                // Offset = 57
    '\0',                               // Offset = 58
    '\0',                               // Offset = 59
    '\0',                               // Offset = 60
    '\0',                               // Offset = 61
    /** Password - Is a null terminated string. Max 8 char + null termination char*/
    's',                                // Offset = 62
    'e',                                // Offset = 63
    'c',                                // Offset = 64
    'r',                                // Offset = 65
    'e',                                // Offset = 66
    't',                                // Offset = 67
    '\0',                               // Offset = 68
    '\0',                               // Offset = 69
    '\0',                                // Offset = 70
    // VSCP TCP IP Server address - Used when not autodiscovering.
    192,								// Offset = 71
    168,								// Offset = 72
    1,									// Offset = 73
    3,									// Offset = 74
    // VSCP stack flags
    ( VSCP_FUNCTION_USE_TCP | VSCP_FUNCTION_AUTODISCOVER ),	// Offset = 75
    // Auto Discovered TCP Server 0		
    0,									// Offset = 76
    0,									// Offset = 77
    0,									// Offset = 78
    0,									// Offset = 79
    // Auto Discovered TCP Server 1		
    0,									// Offset = 80
    0,									// Offset = 81
    0,									// Offset = 82
    0,									// Offset = 83
    // Auto Discovered TCP Server 2		
    0,									// Offset = 84
    0,									// Offset = 85
    0,									// Offset = 86
    0,									// Offset = 87
    // Port for Auto Discovered TCP Server 0		
    0,									// Offset = 88
    0,									// Offset = 89
    // Port for Auto Discovered TCP Server 1		
    0,									// Offset = 90
    0,									// Offset = 91
    // Port for Auto Discovered TCP Server 2		
    0,									// Offset = 92
    0,									// Offset = 93
    // Number of found Auto Discovered TCP Servers
    0,									// Offset = 94
    // Control
    255,								// Offset = 95
    // User ID
    255,								// Offset = 96
    255,								// Offset = 97
    255,								// Offset = 98
    255,								// Offset = 99
    255,								// Offset = 100
    // Manufacturer ID
    0,									// Offset = 101
    0,									// Offset = 102
    0,									// Offset = 103
    0,									// Offset = 104
    // Manufacturer Sub ID
    0,									// Offset = 105
    0,									// Offset = 106
    0,									// Offset = 107
    0,									// Offset = 108
    // Zone
    22,									// Offset = 109
    // Subzone  (!!!! Bits 0,1,2 should be zero !!! )
    0,									// Offset = 110
    // Nova Node General Control
    0,									// Offset = 111
    // Event Interval
    0,									// Offset = 112
    // A/D Control - Period 
    0,									// Offset = 113
    // A/D Control - Alarm Low
    0,									// Offset = 114
    // A/D Control - Alarm High
    0,									// Offset = 115
    // Output control 0-7
    0x80,								// Offset = 116
    0x80,								// Offset = 117
    0x80,								// Offset = 118
    0x80,								// Offset = 119
    0x80,								// Offset = 120
    0x80,								// Offset = 121
    0x80,								// Offset = 122
    // Input Control
    0x80,								// Offset = 123
    0x80,								// Offset = 124
    0x80,								// Offset = 125
    0x80,								// Offset = 126
    0x80,								// Offset = 127
    0x80,								// Offset = 128
    0x80,								// Offset = 129
    0x80,								// Offset = 130
    // Output Protection timer 0 MSB, LSB
    0,									// Offset = 131
    0,									// Offset = 132
    // Output Protection timer 1 MSB, LSB
    0,									// Offset = 133
    0,									// Offset = 134
    // Output Protection timer 2 MSB, LSB
    0,									// Offset = 135
    0,									// Offset = 136
    // Output Protection timer 3 MSB, LSB
    0,									// Offset = 137
    0,									// Offset = 138
    // Output Protection timer 4 MSB, LSB
    0,									// Offset = 139
    0,									// Offset = 140
    // Output Protection timer 5 MSB, LSB
    0,									// Offset = 141
    0,									// Offset = 142
    // Output Protection timer 6 MSB, LSB
    0,									// Offset = 143
    0,									// Offset = 144
    // Output Protection timer 7 MSB, LSB
    0,									// Offset = 145
    0,									// Offset = 146
    // PWM MSB, LSB
    0,									// Offset = 147
    0,									// Offset = 148
    // Reserved
    0,									// Offset = 149
    // Serial Control
    0,									// Offset = 150
    // A/D Min Channel 0 MSB, LSB
    0,									// Offset = 151
    0,									// Offset = 152
    // A/D Min Channel 1 MSB, LSB
    0,									// Offset = 153
    0,									// Offset = 154
    // A/D Min Channel 2 MSB, LSB
    0,									// Offset = 155
    0,									// Offset = 156
    // A/D Min Channel 3 MSB, LSB
    0,									// Offset = 157
    0,									// Offset = 158
    // A/D Min Channel 4 MSB, LSB
    0,									// Offset = 159
    0,									// Offset = 160
    // A/D Min Channel 5 MSB, LSB
    0,									// Offset = 161
    0,									// Offset = 162
    // A/D Min Channel 6 MSB, LSB
    0,									// Offset = 163
    0,									// Offset = 164
    // A/D Min Channel 7 MSB, LSB
    0,									// Offset = 165
    0,									// Offset = 166
    // A/D Max Channel 0 MSB, LSB
    0,									// Offset = 167
    0,									// Offset = 168
    // A/D Max Channel 1 MSB, LSB
    0,									// Offset = 169
    0,									// Offset = 170
    // A/D Max Channel 2 MSB, LSB
    0,									// Offset = 171
    0,									// Offset = 172
    // A/D Max Channel 3 MSB, LSB
    0,									// Offset = 173
    0,									// Offset = 174
    // A/D Max Channel 4 MSB, LSB
    0,									// Offset = 175
    0,									// Offset = 176
    // A/D Max Channel 5 MSB, LSB
    0,									// Offset = 177
    0,									// Offset = 178
    // A/D Max Channel 6 MSB, LSB
    0,									// Offset = 179
    0,									// Offset = 180
    // A/D Max Channel 7 MSB, LSB
    0,									// Offset = 181
    0,									// Offset = 182
    // Hysteresis
    32,									// Offset = 183
    // CAN Control
    6,									// Offset = 184
    // CAN class mask MSB,LSB
    0xff,								// Offset = 185
    0xff,								// Offset = 186
    // CAN type mask MSB,LSB
    0xff,								// Offset = 187
    0xff,								// Offset = 188
    // CAN class filter MSB,LSB
    0,									// Offset = 189
    0,									// Offset = 190
    // CAN type filter MSB,LSB
    0,									// Offset = 191
    0,									// Offset = 192
    // CAN GUID - MSB - LSB  ( Inhouse GUID series as default)
    0xff,								// Offset = 193 MSB
    0xff,								// Offset = 194
    0xff,								// Offset = 195
    0xff,								// Offset = 196
    0xff,								// Offset = 197
    0xff,								// Offset = 198
    0xff,								// Offset = 199
    0xfc,								// Offset = 200
    0,									// Offset = 201
    0,									// Offset = 202
    0,									// Offset = 203
    0,									// Offset = 204
    0,									// Offset = 205
    0,									// Offset = 206
    0,									// Offset = 207 LSB
};
