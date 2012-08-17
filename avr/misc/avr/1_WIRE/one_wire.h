
/*********************************************************************************************************
 Title  :   C header file for the ONE_WIRE FUNCTIONS library (one_wire.h)
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      3/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/



#ifndef ONE_WIRE_H
#define ONE_WIRE    1

/*######################################################################################################*/
/*			         START OF CONFIGURATION BLOCK                                           */
/*######################################################################################################*/

#ifndef  F_CPU
#define  F_CPU		              3686400   /* The cpu clock frequency in Hertz */
#endif

#define ONE_WIRE_PORT	              A	     /* The 1 wire port. Use capital letter (A,B,C,D... etc.) */
#define ONE_WIRE_PIN 	              0	     /* The 1 wire port pin */

#define ONE_WIRE_DYNAMIC_IO_CHANGE    0      /* 0=the above settings apply, 1=can dynamically change */

/* Normally you shouldn't need to change anything in the below definitions */

#define ONE_WIRE_RECOVERY_TIME_US     1      /* The time for the pullup resistor to get the line high */
#define ONE_WIRE_READ_POSITION_US     14     /* Where to sample the DQ line for rx after recovery time*/
#define ONE_WIRE_WRITE_SLOT_TIME_US   90     /* The total time of the WRITE SLOT  */ 
#define ONE_WIRE_READ_SLOT_TIME_US    90     /* The total time of the READ SLOT  */ 
#define ONE_WIRE_RESET_TIME_US        750    /* The total reset pulse time */
#define ONE_WIRE_PRESENSE_TIME_US     100    /* The point of the presence pulse detection after reset */
#define ONE_WIRE_ALARM_TIME_US        2000   /* The required presense pulse duration for a valid alarm */
#define ONE_WIRE_TIMEOUT_MS           4000   /* After that time elapses a fault is detected */

/*######################################################################################################*/
/*			            END OF CONFIGURATION BLOCK                                          */
/*######################################################################################################*/

/********************************************************************************************************/
/*                              KEYWORD DEFINITIONS                                                     */
/********************************************************************************************************/
/* One wire messages */
#define OW_NO_ERROR             0
#define OW_BUS_ERROR            1
#define OW_ALARM_DETECTED       2
#define OW_NO_DEVICE_PRESENT    3
#define OW_CRC_ERROR            4
#define OW_SEARCH_ROM_ACTIVE    5
#define OW_SEARCH_ROM_FINISHED  6

/* ONE WIRE COMMANDS */
#define OW_SKIP_ROM             0xCC
#define OW_MATCH_ROM            0x55
#define OW_READ_ROM             0x33

/* ONE WIRE COMMANDS AND SEARCH ROM FUNCTION COMMANDS. */
#define OW_SEARCH_ROM           0xF0
#define OW_ALARM_SEARCH         0xEC

/* SEARCH ROM commands only */
#define OW_RESET_SEARCH_ROM     1


/********************************************************************************************************/
/*                             PREPROCESSOR DIRECTIVES                                                  */
/********************************************************************************************************/





/********************************************************************************************************/
/*                             PUBLIC FUNCTION PROTOTYPES                                               */
/********************************************************************************************************/

/*           NO SPECIAL INITIALIZATION IS NEEDED. YOU CAN USE THE FUNCTIONS IMMEDIATELY.                */ 

/*
   ow_search_rom() returns the total number of roms found so far and it fills an 8 byte char array
   with the rom code every time it is called. Use the array for reading only.  
   You can call it repeatedly until all roms have been found. If all roms have been found,
   it issues a "OW_SEARCH_ROM_FINISHED" message, else a "OW_SEARCH_ROM_ACTIVE" message.
   Use the ow_get_msg() command to catch those messages.
   ow_search_rom() takes as arguments 1) a pointer to an 8 byte char array and 2) a command.
   It returns the number of rom codes found so far.
  
   The commands are:
   1) "OW_RESET_SEARCH_ROM" resets the search rom variables and exits. No rom searching is performed.
      Rom searching begins from the next function call with the command "OW_CONTINUE_SEARCH_ROM". 
   2) "OW_SEARCH_ROM" fills an 8 byte array with one rom code each time the "ow_search_rom()" is called.
      When all rom codes are found the "ow_search_rom()" function exits returning the total
      rom codes found and a "OW_SEARCH_ROM_FINISHED" message no matter how many times it is called
      after that. To perform a SEARCH_ROM again a call to ow_search_rom() with the "OW_RESET_SEARCH_ROM"
      command is needed.
   3) "OW_ALARM_SEARCH" is the same as the "OW_SEARCH_ROM" but only devices with the alarm flag on are
      detected. 
           
*/ 

extern unsigned char        ow_search_rom(unsigned char *mem_array, unsigned char command);

/* Fills the mem_array with the rom code. Only one device must be connected on the 1 wire bus */
extern void                 ow_read_rom(unsigned char *mem_array);

/*
   Sends a 1 wire reset pulse. Usefull also for determining if there is any error on the 1 wire bus.
   If a function exits abnormally give a ow_reset() and then a ow_get_msg() to determine the error.
*/ 
extern void                 ow_reset(void);

/* Reads a bit from the 1 wire bus. */
extern inline unsigned char ow_get_bit(void);

/* Writes a bit to the 1 wire bus. */
extern inline void          ow_put_bit(unsigned char value);

/* Writes a byte on the 1 wire bus. */
extern void                 ow_put_byte(unsigned char data);

/* Reads a byte from the 1 wire bus. */
extern unsigned char        ow_get_byte(void);

/* Returns any 1 wire related message and error. */
extern unsigned char        ow_get_msg(void);

/* Pulls hard high the DQ line */
extern inline void          ow_pull_hard_dq(unsigned char on_off);

#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
/* Sets the one wire port and pin dynamically if enabled. Use the macro ow_config_io(port,pin) */
extern void                 ow_config_io_(unsigned char port, unsigned char pin);
#endif
/*
ONE WIRE MACROS THAT ARE USED AS FUNCTIONS

void                        ow_command(cmd)

GENERAL USEFULL AND ACCURATE DELAY MACROS

DELAY_US(us)
DELAY_MS(ms)
*/
   


/********************************************************************************************************/
/*                                 FUNCTION RESEMBLING MACROS                                           */
/********************************************************************************************************/

#define ow_command(cmd)            ow_put_byte(cmd)


#if  defined(_IO_REG_MACRO_MODE_) || defined(_SFR_ASM_COMPAT) || defined(__ASSEMBLER__)  \
     || !defined(_SFR_DEFS_H_) 

#define ow_config_io(port,pin) ow_config_io_(port,pin)

#else

#define ow_config_io(port,pin) ow_config_io_(_SFR_MEM_ADDR(port),pin)

#endif

/*
   THE REASON I USE MACROS INSTEAD OF FUNCTIONS FOR TIMING IS THAT TIMING ACCURACY IS NEEDED AND MACROS
   ARE "INLINE" AND THE COMPILER CANNOT MESS WITH THEM VERY MUCH (ALTHOUGH IT CAN SOMETIMES).
*/

/********************************************************************************************************/
/*                                TIMING MACROS AND DEFINITIONS                                         */
/********************************************************************************************************/

/* Accurate Delay macros */
#ifndef DELAY_S_ACCURATE
/* 4 cpu cycles per loop + 1 overhead when a constant is passed. */
#define DELAY_S_ACCURATE(x)  ({ unsigned short number_of_loops=(unsigned short)x; \
                              	__asm__ volatile ( "L_%=:           \n\t"         \
                                                   "sbiw %A0,1      \n\t"         \
                                                   "brne L_%=       \n\t"         \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                               




#define DELAY_US(us)                DELAY_S_ACCURATE( ((((us)*1000L)/(1000000000/F_CPU))-1)/4 )  

#endif /* #ifndef DELAY_S_ACCURATE */

#ifndef DELAY_L_ACCURATE
/* 6 cpu cycles per loop + 3 overhead when a constant is passed. */
#define DELAY_L_ACCURATE(x)  ({ unsigned long number_of_loops=(unsigned long)x;   \
                                __asm__ volatile ( "L_%=: \n\t"                   \
                                                   "subi %A0,lo8(-(-1)) \n\t"     \
                                                   "sbci %B0,hi8(-(-1)) \n\t"     \
                                                   "sbci %C0,hlo8(-(-1)) \n\t"    \
                                                   "sbci %D0,hhi8(-(-1)) \n\t"    \
                                                   "brne L_%= \n\t"               \
                                                   : /* NO OUTPUT */              \
                                                   : "w" (number_of_loops)        \
                                                 );                               \
                             })                                      




#define TIME_L1_MS      ( 1*(F_CPU/6000) )     /* 1 MILLISECOND  */

#define TIME_L100_US    ( 1*(F_CPU/60000) )    /* 100 MICROSECONDS  */

#define DELAY_MS(ms)    DELAY_L_ACCURATE( (TIME_L1_MS*(ms)) )

#define DELAY_L_US(us)  DELAY_L_ACCURATE( (TIME_L100_US*((us)/100))+1 )

#endif /* #ifndef DELAY_L_ACCURATE */




#endif	/*  #ifndef ONE_WIRE_H */
/*######################################################################################################*/
/*					   T H E   E N D						*/
/*######################################################################################################*/

