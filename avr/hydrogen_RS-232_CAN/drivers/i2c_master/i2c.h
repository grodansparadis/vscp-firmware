
/****************************************************************************
 Title	:   C  include file for the I2C FUNCTIONS library (i2c.h)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:	    22/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/


#ifndef I2C_H
#define I2C_H	1

/*##############################################################################################*/
/*			       START OF CONFIGURATION BLOCK					*/
/*##############################################################################################*/
/*
   The I2C_CLOCK normally is 100 Khz so the delay between level states is 
   (1/I2C_CLOCK)/2 = ( (1/100000)/2 ) = 5 microseconds.
   The use of delay loop for timing is highly accurate due to the use of inline asm code.
   There is no need to use a timer but if you insist you have that option.
   The I2C_TIMEOUT_TIME_IN_MICROSECONDS is set to whatever is the maximum time you think
   your device will take to perform the requested task. Limit chacking is done during compile time.
   After that time the i2c functions will return a "I2C_ERROR_DEVICE_NOT_RESPONDING" 
   or "I2C_ERROR_DEVICE_BUSY" error code. 
*/ 


#ifndef  F_CPU
#define  F_CPU		              3686400   /* The cpu clock frequency in Hertz */
#endif

#define I2C_SDA_PORT	              D	      /* The SDA port. Use capital letter (A,B,C,D... etc.) */
#define SDA_PIN 	              4	      /* The SDA port pin */

#define I2C_SCL_PORT	              D	      /* The SCL port. Use capital letter (A,B,C,D... etc.) */
#define SCL_PIN 	              5	      /* The SCL port pin */

#define I2C_CLOCK_IN_HERTZ            100000  /* in HERTZ */
#define I2C_TIMEOUT_IN_MILLISECONDS   1       /* in milliseconds  */
#define I2C_NUMBER_OF_START_ATTEMPTS  10      /* attempts to issue a succesfull start condition */
#define I2C_ERROR_CHECK_ENABLE        1       /* a way to save code if set to 0 */
#define I2C_WILL_USE_A_TIMER          0       /* 0=use delay loop, 1=use a 8 bit timer */
#define I2C_USE_TIMER                 0       /* 0=TIMER0, 2=TIMER2 (valid if I2C_WILL_USE_A_TIMER==1) */


/*##############################################################################################*/
/*				END OF CONFIGURATION BLOCK					*/
/*##############################################################################################*/

/* Keyword definitions */

#define I2C_BROADCAST                    0X00

#define I2C_READ                   	 1
#define I2C_WRITE                    	 0

#define I2C_QUIT                       	 0
#define I2C_CONTINUE	                 1

#if I2C_ERROR_CHECK_ENABLE == 1

#define I2C_NO_ERROR			 0
#define I2C_ERROR_DEVICE_BUSY		 1
#define I2C_ERROR_DEVICE_NOT_RESPONDING  2
#define I2C_ERROR_BUS_OCCUPIED           3

#endif

/* Function Declaration */

extern void           i2c_start(void);
extern void	      i2c_stop(void);
extern unsigned char  i2c_receive(unsigned char ack);
extern void           i2c_transmit(unsigned char data);
#if I2C_ERROR_CHECK_ENABLE == 1
extern unsigned char  i2c_get_error(void);
#endif

/* Macro definitions */

#define I2C_START(ADDRESS)        { i2c_start(); i2c_transmit(ADDRESS); }
#define I2C_START_TX(ADDRESS)     I2C_START(ADDRESS)
#define I2C_START_RX(ADDRESS)     I2C_START(ADDRESS | I2C_READ)
#define i2c_init()                { }


#endif	/*  #ifndef I2C_H */
/*######################################################################################################*/
/*					   T H E   E N D						*/
/*######################################################################################################*/

