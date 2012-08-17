/***************************************************************************
 *    www.braintechnology.de                                               *
 *    Advairtizer V1.0                                                     *
 *                                                                         *
 *    (c) Copyright Braintechnology 2007                                   *
 *    File name   : config.h                                               *
 *    Description : Demo Software for Advairtizer Development Modules      *
 *    History     :                                                        *
 *    Date        : Oktober, 2007                                          *
 *    Author      : Kai Klein                                              *
 *    $Revision: 1.0 $                                                     *
 ***************************************************************************/

/********************************* 
* Timer0 = Systime (sysTime.c)
* Timer1 = not used
* PWM5 = LED Backlight (glcdctrl.c)
**********************************/
  #ifndef _CONFIG_H_
  #define _CONFIG_H_

// some handy DEFINES
#ifndef FALSE
#define FALSE               0
#ifndef TRUE
#define TRUE                !FALSE
#endif
#endif

//realtime clock
#define GERMAN

#ifndef BIT
#define BIT(n)              (1L << (n))
#endif

/********************************* 
* Version
**********************************/
#define version_txt "1.00"
#define model_txt "ADV-B"

/********************************* 
* 0 no debug output
* 1 Normal Mode init output to LCD
* 2 Debug error to Speaker
* 3 Debug error to LCD 
**********************************/
#ifndef DEBUG
#define DEBUG 0
#endif

/************************************
* LCD Rotation
* 1 rotation ok 180° readable from top ,lcd right Joystick left
* 2 rotation ok 0° Normal, readable from down , lcd left Joystick right
* 3 rotation ok 270° readable from right, lcd top Joystick down
* 4 rotation ok 90° readable from left, lcd down Joystick top
************************************/
#define lcd_rotation 2 // Rotation
// declare functions and values from crt0.S & the linker control file
extern void reset(void);
extern void exit( int exit_code );// changed RCWorks V1.6 -> V1.7
extern void abort(void);

/* 	PLL
   - Main clock F_OSC=14,7MHz @ Olimex LPC-P2106) [limits: 10 MHz to 25 MHz]
   - System should run at max. Frequency (60MHz) [limit: max 60 MHz]
   - Choose multiplier M=4 
     so cclk = M * F_OSC= 4 * 14745000Hz = 58980000 Hz
   - MSEL-Bit in PLLCFG (bits 0-4) MSEL = M-1
   - F_CCO must be inbetween the limits 156 MHz to 320 MHz
     datasheet: F_CCO = F_OSC * M * 2 * P
   - choose devider P=2 => F_CCO = 14745000Hz * 4 * 2 * 2
     = 235920000 ~=236 MHz
   - PSEL0 (Bit5 in PLLCFG) = 1, PSEL1 (Bit6) = 0 (0b01)
*/

// PLL setup values
#define FOSC                (12000000)  // Master Oscillator Freq.
#ifndef OSCILLATOR_CLOCK_FREQUENCY
#define OSCILLATOR_CLOCK_FREQUENCY FOSC
#endif
#define PLL_MUL             (5)         // PLL Multiplier
#define CCLK                (FOSC * PLL_MUL) // CPU Clock Freq.
                                            // Pheripheral Bus Speed Divider
#define PBSD                1           // MUST BE 1(60mhz), 2(30mhz), or 4(15mhz)
#define PCLK                (CCLK / PBSD) //= 60Mhz(PBSD=1) Pheripheal Bus Clock Freq.
#define MSEL		(PLL_MUL-1)
#define PSEL0 		5
#define PSEL1 		6
#define PLLE		0
#define PLLC		1
#define PLOCK		10
#define PLL_FEED1	0xAA
#define PLL_FEED2	0x55
//SSP
#define CPSDVSR         2//This even value between 2 and 254, by which PCLK is divided 0
#define SCR             1//SSP Serial Clock Rate. The number of prescaler-output clocks per bit on the bus, minus one
                         //0=30MHz,1=15MHz,2=10MHz,4=6MHz,14=2MHz,29=1MHz 
// SPI-IOSET-Maske
#define SPI0_IOSET_MASK 0x00001500 // SCK,MISO and MOSI for SPI0 usage

//PWM
#define PWM_PCLK_DIV 2  //main PWM
#define PWM_FREQ1 (44000) //frq for main PWM0 the Master PWM frq (also used for sound)
#define PWM_FREQ2 (36000) //frq for LCD Backlight must smaler then PWM_FREQ1
#define PWM_FRE_RC5_S (36000) //frq for IR Transmitter must smaler then PWM_FREQ1
#define PWMTICSperSEC (PCLK / (PWM_PCLK_DIV + 1))

#define PWM_MR0INIT (U32)((PWMTICSperSEC/PWM_FREQ1) + 0.5)  //Main PWM TIMER //682,3182045

// Do some value range testing
#if ((FOSC < 10000000) || (FOSC > 25000000))//= 12Mhz
#error Fosc out of range (10MHz-25MHz)
#error correct and recompile
#endif

#if ((CCLK < 10000000) || (CCLK > 60000000))//= 60Mhz
#error cclk out of range (10MHz-60MHz)
#error correct PLL_MUL and recompile
#endif

#if ((FCCO < 150000000) || (FCCO > 320000000))//= 240Mhz
#error Fcco out of range (156MHz-320MHz)
#error internal algorithm error
#endif

#define MAM_MODE    2
#define MAM_FETCH   3

/* Cotrast: PWM 5 */
// for PINSEL0 setup:
#define GLCDCTRL_PINMASK ((U32)(0x03<<16)|(0x03<<18))
#define GLCDCTRL_PINSEL ((U32)(0x02<<16)|(0x02<<18))

//Interrupt-Priorities
#define PRIO_EINT2            5   +1      //Ethernet Controller (enc28j60.c)
#define PRIO_INT_ENC28J60     PRIO_EINT2
#define PRIO_EINT3            7   +1      //IR_SENSOR (ir_sensor.c)
#define PRIO_IR_SENSOR        PRIO_EINT3
#define PRIO_RTC              2   +1      //Realtime Clock (rtc.c)
#define PRIO_TIMER1           3   +1      //free not used
#define PRIO_PWM0             1   +1      //Sound PWM (speaker.c)
#define PRIO_UART0            6   +1      //RX RS485 RS232 (uart.c)
#define PRIO_USB              1           //USB ISR
#define PRIO_TIMER0           4   +1      //Systime (sysTime.c)

/*****************************************
* Port Bit Definitions
* PIO 0
*****************************************/
#define TXDATA              BIT(0)      // P0.00
#define RXDATA              BIT(1)      // P0.01
#define SCL0                BIT(2)      // P0.02 unused for I²C0
#define SDA0                BIT(3)      // P0.03 unused for I²C0
#define SCK0                BIT(4)      // P0.04 unused for SPI0
#define MISO0               BIT(5)      // P0.05 unused for SPI0
#define MOSI0               BIT(6)      // P0.06 unused for SPI0
#define CS_ETHERNET         BIT(7)      // P0.07 CS_ETHERNET
#define IR_PWM              BIT(8)      // P0.08 Infrarot tx Diode PWM
#define BUZZER              BIT(9)      // P0.09 Piezo
#define ANALOG1             BIT(10)     // P0.10 ANALOG1
#define CS_FLASH            BIT(11)     // P0.11 CS_FLASH
#define RELAIS1             BIT(12)     // P0.12 RELAIS1
#define CS_SDCARD           BIT(13)     // P0.13 CS_SDCARD
#define WOL_ETHERNET        BIT(14)     // P0.14 WOL_ETHERNET
#define INT_ETHERNET        BIT(15)     // P0.15 INT_ETHERNET
#define LCD_RESET           BIT(16)     // P0.16 LCD_RESET
#define LCD_CLK             BIT(17)     // P0.17 LCD_CLK
#define LCD_MISO            BIT(18)     // P0.18 
#define LCD_DAT             BIT(19)     // P0.19 LCD_DAT
#define LCD_CS              BIT(20)     // P0.20 LCD_CS
#define LCD_BLIGHT          BIT(21)     // P0.21 LCD Backlight PWM
#define LCD_RS              BIT(22)     // P0.22 LCD_RS
#define VBUS                BIT(23)     // P0.23 is high if USB is Connected
#define P24_UNUSED_BIT      BIT(24)     // P0.24 not bondet
#define ANALOG_OUT          BIT(25)     // P0.25 Analog Output
#define P26_UNUSED_BIT      BIT(26)     // P0.26 not bondet
#define P27_UNUSED_BIT      BIT(27)     // P0.27 not bondet
#define NTC_IR_TX           BIT(28)     // P0.28 Analog in for NTC or IR LED TX Pulses
#define LIGHT_SENSOR        BIT(29)     // P0.29 LIGHT_SENSOR
#define IR_SENSOR           BIT(30)     // P0.30 IR_SENSOR
#define USB_CONNECT         BIT(31)     // P0.31 Connect 1,5K restistor to USB +D

// PIO 1
#define P1_00_UNUSED_BIT      BIT(0)      // P1.00 unused - low output
#define P1_01_UNUSED_BIT      BIT(1)      // P1.00 unused - low output
#define P1_02_UNUSED_BIT      BIT(2)      // P1.02 unused - low output
#define P1_03_UNUSED_BIT      BIT(3)      // P1.03 unused - low output
#define P1_04_UNUSED_BIT      BIT(4)      // P1.04 unused - low output
#define P1_05_UNUSED_BIT      BIT(5)      // P1.05 unused - low output
#define P1_06_UNUSED_BIT      BIT(6)      // P1.06 unused - low output
#define P1_07_UNUSED_BIT      BIT(7)      // P1.07 unused - low output
#define P1_08_UNUSED_BIT      BIT(8)      // P1.08 unused - low output
#define P1_09_UNUSED_BIT      BIT(9)      // P1.09 unused - low output
#define P1_10_UNUSED_BIT      BIT(10)     // P1.10 unused - low output
#define P1_11_UNUSED_BIT      BIT(11)     // P1.11 unused - low output
#define P1_12_UNUSED_BIT      BIT(12)     // P1.12 unused - low output
#define P1_13_UNUSED_BIT      BIT(13)     // P1.13 unused - low output
#define P1_14_UNUSED_BIT      BIT(14)     // P1.14 unused - low output
#define P1_15_UNUSED_BIT      BIT(15)     // P1.15 unused - low output
#define STICK_LEFT_BIT        BIT(16)     // P1.16 STICK_LEFT_BIT & Encoder right
#define STICK_DOWN_BIT        BIT(17)     // P1.17 STICK_DOWN_BIT
#define STICK_UP_BIT          BIT(18)     // P1.18 STICK_UP_BIT
#define STICK_RIGHT_BIT       BIT(19)     // P1.19 STICK_RIGHT_BIT & Encoder left
#define RX232_ENABLE_BIT      BIT(20)     // P1.20 RX232_ENABLE_BIT
#define STICK_BUTTON_BIT      BIT(21)     // P1.21 STICK_BUTTON_BIT & Encoder Button
#define RX485_ENABLE_BIT      BIT(22)     // P1.22 RX485_ENABLE
#define HB_LED                BIT(23)     // P1.23 HeartBeat LED
#define P1_24_UNUSED_BIT      BIT(24)     // P1.24 unused - low output
#define IO1                   BIT(25)     // P1.25 I/O 1 General I/O
#define P1_26_UNUSED_BIT      BIT(26)     // P1.26 unused - low output
#define P1_27_UNUSED_BIT      BIT(27)     // P1.27 unused - low output
#define P1_28_UNUSED_BIT      BIT(28)     // P1.28 unused - low output
#define P1_29_UNUSED_BIT      BIT(29)     // P1.29 unused - low output
#define P1_30_UNUSED_BIT      BIT(30)     // P1.30 unused - low output
#define P1_31_UNUSED_BIT      BIT(31)     // P1.31 unused - low output

  #define PIO0_INPUT_BITS      (U32) ( \
                                           ANALOG1 | \
                                           INT_ETHERNET | \
                                           WOL_ETHERNET | \
                                           IR_SENSOR | \
                                           VBUS | \
                                           0 )
  
  #define PIO0_ZERO_BITS       (U32) ( \
                                           SCK0 | \
                                           MISO0 | \
                                           MOSI0 | \
                                           LCD_BLIGHT | \
                                           IR_PWM | \
                                           RELAIS1 | \
                                           ANALOG_OUT | \
                                           0 )
  
  #define PIO0_ONE_BITS        (U32) ( \
                                           SCL0 | \
                                           SDA0 | \
                                           BUZZER | \
                                           LCD_RESET | \
                                           LCD_RS | \
                                           LCD_CLK | \
                                           LCD_DAT | \
                                           LCD_CS | \
                                           CS_FLASH | \
                                           CS_ETHERNET | \
                                           CS_SDCARD | \
                                           USB_CONNECT | \
                                           0 )
  
  #define PIO0_OUTPUT_BITS     (U32) ( \
                                           PIO0_ZERO_BITS | \
                                           PIO0_ONE_BITS )
    
  #define PIO1_INPUT_BITS      (U32) ( \
                                           STICK_LEFT_BIT | \
                                           STICK_RIGHT_BIT | \
                                           STICK_DOWN_BIT | \
                                           STICK_UP_BIT | \
                                           STICK_BUTTON_BIT | \
                                           NTC_IR_TX | \
                                           0 )
  
  #define PIO1_ZERO_BITS       (U32) ( \
                                           P1_00_UNUSED_BIT | \
                                           P1_01_UNUSED_BIT | \
                                           P1_02_UNUSED_BIT | \
                                           P1_03_UNUSED_BIT | \
                                           P1_04_UNUSED_BIT | \
                                           P1_05_UNUSED_BIT | \
                                           P1_06_UNUSED_BIT | \
                                           P1_07_UNUSED_BIT | \
                                           P1_08_UNUSED_BIT | \
                                           P1_09_UNUSED_BIT | \
                                           P1_10_UNUSED_BIT | \
                                           P1_11_UNUSED_BIT | \
                                           P1_12_UNUSED_BIT | \
                                           P1_13_UNUSED_BIT | \
                                           P1_14_UNUSED_BIT | \
                                           P1_15_UNUSED_BIT | \
                                           P1_24_UNUSED_BIT | \
                                           IO1 | \
                                           P1_26_UNUSED_BIT | \
                                           P1_27_UNUSED_BIT | \
                                           P1_28_UNUSED_BIT | \
                                           P1_29_UNUSED_BIT | \
                                           P1_30_UNUSED_BIT | \
                                           P1_31_UNUSED_BIT | \
                                           0 )
  
  #define PIO1_ONE_BITS        (U32) ( \
                                          HB_LED | \
                                          RX485_ENABLE_BIT | \
                                          RX232_ENABLE_BIT | \
                                           0 )
  
  #define PIO1_OUTPUT_BITS     (U32) ( \
                                           PIO1_ZERO_BITS | \
                                           PIO1_ONE_BITS )
  #endif
