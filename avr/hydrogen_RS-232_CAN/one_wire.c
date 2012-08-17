
/*********************************************************************************************************
 Title  :   C include file for the ONE_WIRE FUNCTIONS library (one_wire.c)
 Author:    Chris efstathiou  
 E-mail:    hendrix@otenet.gr
 Homepage:  ........................
 Date:      3/Dec/2002
 Compiler:  AVR-GCC with AVR-AS
 MCU type:  any AVR MCU device
 Comments:  This software is FREE but without any warranty of any kind.
*********************************************************************************************************/

/* 
    Although i could had written it in the new style, i prefer the old one for compatibility sake.
*/

/********************************************************************************************************/
/*                                   PREPROCESSOR DIRECTIVES                                            */
/********************************************************************************************************/

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1	   /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1	   /* This is for GCC 3.2 */
#endif

#include <io.h>
#include "one_wire.h"

/* Register name forming */
#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif

#define ONE_WIRE_OUT_REG   CONCAT1(PORT, ONE_WIRE_PORT)
#define ONE_WIRE_DDR_REG   CONCAT1(DDR,  ONE_WIRE_PORT)
#define ONE_WIRE_PIN_REG   CONCAT1(PIN,  ONE_WIRE_PORT)

/*******************************************************************************************************/
/* SOME ERROR CHECKING WHEN USING MEGA 103 */

#if defined(__AVR_ATmega103__)

#undef  DDRC
#undef  DDRF

#define DDRC   0
#define DDRF   0

#if ONE_WIRE_DDR_REG == 0 
#error "SORRY THE ONE_WIRE PIN PORT MUST BE BIDIRECTIONAL"
#endif

#endif /* #if defined(__AVR_ATmega103__) */

#if ONE_WIRE_RECOVERY_TIME_US > 15 
#warning " 1 WIRE RECOVERY TIME OUT OF SPECS "
#endif

#if ONE_WIRE_RESET_TIME_US < 480
#warning " 1 WIRE RESET TIME OUT OF SPECS "
#endif

#if ONE_WIRE_RESET_TIME_US > 960
#warning " 1 WIRE RESET TIME MIGHT CAUSE 1 WIRE RESETS "
#endif

#if ONE_WIRE_PRESENSE_TIME_US > 240
#warning " 1 WIRE PRESENSE TIME OUT OF SPECS "
#endif

#if ONE_WIRE_WRITE_SLOT_TIME_US > 120 || ONE_WIRE_WRITE_SLOT_TIME_US < 60
#warning " 1 WIRE WRITE SLOT TIME OUT OF SPECS "
#endif

#if ONE_WIRE_READ_SLOT_TIME_US > 120 || ONE_WIRE_READ_SLOT_TIME_US < 60
#warning " 1 WIRE READ SLOT TIME OUT OF SPECS "
#endif

#if ONE_WIRE_READ_POSITION_US > 15
#warning " 1 WIRE READ POSITION TIME OUT OF SPECS "
#endif

#if  ONE_WIRE_READ_POSITION_US-ONE_WIRE_RECOVERY_TIME_US <=0 

#if  ONE_WIRE_RECOVERY_TIME_US >= ONE_WIRE_READ_POSITION_US
#error " 1 WIRE RECOVERY TIME TOO HIGH "
#elif ONE_WIRE_READ_POSITION_US <= 5
#error " 1 WIRE READ POSITION TIME TOO SMALL "
#endif

#endif

/* Decide what the i/o space to memory space will be */
#ifndef IO_TO_MEM_OFFSET
#if SREG > 0X3F
#define  IO_TO_MEM_OFFSET  0x00
#else
#define  IO_TO_MEM_OFFSET  0x20
#endif
#endif


#ifndef DELAY_C_ACCURATE
/* 3 cpu cycles per loop when a constant is passed.*/
#define DELAY_C_ACCURATE(x)   ({ unsigned char number_of_loops=(unsigned char)x;  \
                               	 __asm__ volatile ( "L_%=:             \n\t"      \
                                                    "dec %A0           \n\t"      \
                                                    "brne L_%=         \n\t"      \
                                                    : /* NO OUTPUT */             \
                                                    : "r" (number_of_loops)       \
		                                  );                                      \
                              })                  

#endif

/********************************************************************************************************/
/*           OVERHEAD CALCULATION FOR THE TRANSITION TIME NEEDED FOR THE DQ LINE LOGIC                  */
/********************************************************************************************************/

#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
#define ONE_WIRE_DQ_OVERHEAD_CYCLES     12
#define ONE_WIRE_READ_OVERHEAD_CYCLES   (ONE_WIRE_DQ_OVERHEAD_CYCLES/2)       
#elif ONE_WIRE_DYNAMIC_IO_CHANGE == 0
#define ONE_WIRE_DQ_OVERHEAD_CYCLES     1
#define ONE_WIRE_READ_OVERHEAD_CYCLES   1
#endif

/********************************************************************************************************/
/*               DELAY MACRO DEFINITION FOR THE ONE WIRE RECOVERY TIME                                  */
/********************************************************************************************************/

#define OW_RECOVERY_TIME_ACCURATE    ( (ONE_WIRE_RECOVERY_TIME_US*1000)/(1000000000/(F_CPU*10)) )
#define OW_RECOVERY_TIME_REAL        (OW_RECOVERY_TIME_ACCURATE/10)

#if   OW_RECOVERY_TIME_ACCURATE - (OW_RECOVERY_TIME_REAL*10) >=5
#define OW_RECOVERY_TIME_TEMP        (OW_RECOVERY_TIME_REAL+1)
#else 
#define OW_RECOVERY_TIME_TEMP        OW_RECOVERY_TIME_REAL
#endif

#define OW_RECOVERY_TIME             (OW_RECOVERY_TIME_TEMP-ONE_WIRE_DQ_OVERHEAD_CYCLES)

#if  OW_RECOVERY_TIME<= 0

#define DELAY_OW_RECOVERY_TIME()     { }

#elif  OW_RECOVERY_TIME== 1
#define DELAY_OW_RECOVERY_TIME()     __asm__ volatile("nop")

#elif OW_RECOVERY_TIME == 2
#define DELAY_OW_RECOVERY_TIME()     { __asm__ volatile("nop"); __asm__ volatile("nop"); }

#elif OW_RECOVERY_TIME >= 3 
#define  OW_RECOVERY_DELAY_LOOPS     (OW_RECOVERY_TIME/3)

#if  OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 0

#define DELAY_OW_RECOVERY_TIME()     DELAY_C_ACCURATE(OW_RECOVERY_DELAY_LOOPS)

#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 1

#define DELAY_OW_RECOVERY_TIME()     { DELAY_C_ACCURATE(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop");                          \
                                     } 

#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 2

#define DELAY_OW_RECOVERY_TIME()     { DELAY_C_ACCURATE(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                     }
#elif OW_RECOVERY_TIME - (OW_RECOVERY_DELAY_LOOPS*3) == 3

#define DELAY_OW_RECOVERY_TIME()     { DELAY_C_ACCURATE(OW_RECOVERY_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                       __asm__ volatile("nop");                          \
                                     } 

#endif /* #if  OW_RECOVERY_TIME - OW_RECOVERY_DELAY_LOOPS == 0 */

#endif  /* #if  OW_RECOVERY_TIME<= 0 -> #elif  OW_RECOVERY_TIME >= 21 */

/********************************************************************************************************/
/*               DELAY MACRO DEFINITION FOR THE ONE WIRE READ TIME POSITION                             */
/********************************************************************************************************/

#define OW_READ_TIME_POS_ACCURATE    ( (ONE_WIRE_READ_POSITION_US*1000)/(1000000000/(F_CPU*10)) )
#define OW_READ_TIME_POS_REAL        (OW_READ_TIME_POS_ACCURATE/10)

#if   OW_READ_TIME_POS_ACCURATE - (OW_READ_TIME_POS_REAL*10) >=5
#define OW_READ_TIME_POS_TEMP        (OW_READ_TIME_POS_REAL+1)
#else 
#define OW_READ_TIME_POS_TEMP        OW_READ_TIME_POS_REAL
#endif

#if OW_RECOVERY_TIME<= 0
#define OW_READ_POS_TEMP     (OW_READ_TIME_POS_TEMP-(ONE_WIRE_DQ_OVERHEAD_CYCLES - OW_RECOVERY_TIME_TEMP))
#define OW_READ_POS          (OW_READ_POS_TEMP-ONE_WIRE_READ_OVERHEAD_CYCLES)
#elif OW_RECOVERY_TIME > 0
#define OW_READ_POS          (OW_READ_TIME_POS_TEMP-ONE_WIRE_READ_OVERHEAD_CYCLES)
#endif


#if  OW_READ_POS<= 0
#warning " READ TIME POSITION DELAY IS 0"
#define DELAY_OW_READ_POS()     { }

#elif  OW_READ_POS== 1
#define DELAY_OW_READ_POS()     __asm__ volatile("nop")

#elif OW_READ_POS == 2
#define DELAY_OW_READ_POS()     { __asm__ volatile("nop"); __asm__ volatile("nop"); }

#elif OW_READ_POS >= 3 
#define  OW_READ_POS_DELAY_LOOPS     (OW_READ_POS/3)

#if  OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*3) == 0

#define DELAY_OW_READ_POS()     DELAY_C_ACCURATE(OW_READ_POS_DELAY_LOOPS)

#elif OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*3) == 1

#define DELAY_OW_READ_POS()     { DELAY_C_ACCURATE(OW_READ_POS_DELAY_LOOPS);        \
                                       __asm__ volatile("nop");                          \
                                     } 

#elif OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*3) == 2

#define DELAY_OW_READ_POS()     { DELAY_C_ACCURATE(OW_READ_POS_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                     }
#elif OW_READ_POS - (OW_READ_POS_DELAY_LOOPS*3) == 3

#define DELAY_OW_READ_POS()     { DELAY_C_ACCURATE(OW_READ_POS_DELAY_LOOPS);        \
                                       __asm__ volatile("nop"); __asm__ volatile("nop"); \
                                       __asm__ volatile("nop");                          \
                                     } 

#endif /* #if  OW_READ_POS - OW_READ_POS_DELAY_LOOPS == 0 */

#endif  /* #if  OW_READ_POS<= 0 -> #elif OW_READ_POS >= 3 */


/* Pin states */
#if ONE_WIRE_DYNAMIC_IO_CHANGE == 0

#define ONE_WIRE_DQ_1()        { cbi(ONE_WIRE_DDR_REG, ONE_WIRE_PIN); }
#define ONE_WIRE_DQ_0()        { sbi(ONE_WIRE_DDR_REG, ONE_WIRE_PIN); }
#define OW_PORT_1()            { sbi(ONE_WIRE_OUT_REG, ONE_WIRE_PIN); sbi(ONE_WIRE_DDR_REG, ONE_WIRE_PIN); }
#define OW_PORT_INPUT()        { cbi(ONE_WIRE_DDR_REG, ONE_WIRE_PIN); cbi(ONE_WIRE_OUT_REG, ONE_WIRE_PIN); }
#define BIT_IS_SET(port,pin)   ( bit_is_set(ONE_WIRE_PIN_REG, ONE_WIRE_PIN) )
#define BIT_IS_CLEAR(port,pin) ( bit_is_clear(ONE_WIRE_PIN_REG, ONE_WIRE_PIN) )

#elif ONE_WIRE_DYNAMIC_IO_CHANGE == 1

#define ONE_WIRE_DQ_1()        { *((unsigned char*)(ow_ddr_reg+IO_TO_MEM_OFFSET)) &= (~ow_pin); }

#define ONE_WIRE_DQ_0()        { *((unsigned char*)(ow_ddr_reg+IO_TO_MEM_OFFSET)) |= ow_pin; }

#define OW_PORT_1()            { *((unsigned char*)(ow_out_reg+IO_TO_MEM_OFFSET)) |= ow_pin;       \
                                 *((unsigned char*)(ow_ddr_reg+IO_TO_MEM_OFFSET)) |= ow_pin;       \
                               }

#define OW_PORT_INPUT()        { *((unsigned char*)(ow_ddr_reg+IO_TO_MEM_OFFSET)) &= (~ow_pin);    \
                                 *((unsigned char*)(ow_out_reg+IO_TO_MEM_OFFSET)) &= (~ow_pin);    \
                               }

#define BIT_IS_SET(port,pin)   ( *((unsigned char*)(port+IO_TO_MEM_OFFSET)) & pin )

#define BIT_IS_CLEAR(port,pin) ( !(*((unsigned char*)(port+IO_TO_MEM_OFFSET)) & pin) )

#endif /* #if ONE_WIRE_DYNAMIC_IO_CHANGE == 0 -> #elif ONE_WIRE_DYNAMIC_IO_CHANGE == 1 */


/********************************************************************************************************/
/*                                   TYPE DEFINITIONS                                                   */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/





/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/
volatile unsigned char ow_system_message=0;
#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
unsigned char ow_out_reg = ONE_WIRE_OUT_REG;
unsigned char ow_ddr_reg = ONE_WIRE_DDR_REG;
unsigned char ow_pin_reg = ONE_WIRE_PIN_REG;
unsigned char ow_pin = ( 1<<ONE_WIRE_PIN );
#endif
/********************************************************************************************************/
/*                                   LOCAL FUNCTIONS                                                    */
/********************************************************************************************************/


/********************************************************************************************************/
/*                                   PUBLIC FUNCTIONS                                                   */
/********************************************************************************************************/
#if ONE_WIRE_DYNAMIC_IO_CHANGE == 1
void ow_config_io_(unsigned char port, unsigned char pin)
{
    ow_out_reg=port;
    ow_ddr_reg=(port-1);
    ow_pin_reg=(port-2);
    ow_pin=(1<<pin);
    ONE_WIRE_DQ_1();
    DELAY_OW_RECOVERY_TIME();

    return;
}
#endif

inline unsigned char ow_get_bit(void)
{
    unsigned char x=0; 

    ONE_WIRE_DQ_0();
    DELAY_OW_RECOVERY_TIME();
    ONE_WIRE_DQ_1();
    DELAY_OW_READ_POS();
    if( BIT_IS_SET( ow_pin_reg, ow_pin ) ) { 
        x=1; 
    }
    DELAY_US(ONE_WIRE_READ_SLOT_TIME_US);                   

    return(x);
}
/*######################################################################################################*/

inline void ow_put_bit(unsigned char value)
{
    if( value ) {
        ONE_WIRE_DQ_0();
        DELAY_OW_RECOVERY_TIME();
        ONE_WIRE_DQ_1(); 
        DELAY_US(ONE_WIRE_WRITE_SLOT_TIME_US);
    }
    else {
        ONE_WIRE_DQ_0();
        DELAY_US(ONE_WIRE_WRITE_SLOT_TIME_US);
        ONE_WIRE_DQ_1();
        DELAY_OW_RECOVERY_TIME();
    }
 
    return;
}
/*######################################################################################################*/

inline void ow_pull_hard_dq(unsigned char on_off) 
{ 
    if( on_off ) { 
        OW_PORT_1(); 
    } 
    else { 
        OW_PORT_INPUT(); 
    }

    return;
}
/*######################################################################################################*/

void ow_reset(void)
{
unsigned char x=0, y=0;

/* Perform the 1 wire reset */
ONE_WIRE_DQ_0();
DELAY_US(ONE_WIRE_RESET_TIME_US);
ONE_WIRE_DQ_1();
DELAY_US(ONE_WIRE_PRESENSE_TIME_US);

/* Noise reduction */
for(x=0,y=0; x<10; x++)
  {
     if( BIT_IS_SET(ow_pin_reg, ow_pin ) ) { y++; }
     DELAY_US(ONE_WIRE_PRESENSE_TIME_US/10);
     if(y>=2) { ow_system_message=OW_NO_ERROR; break; }
     else{ ow_system_message=OW_NO_DEVICE_PRESENT; }
  }  

DELAY_US(ONE_WIRE_ALARM_TIME_US-ONE_WIRE_PRESENSE_TIME_US);

/* Check to see if the DQ line returned to high, else issue an error */
if( BIT_IS_CLEAR(ow_pin_reg, ow_pin ) )
 { 
    ow_system_message=OW_ALARM_DETECTED;
 }

DELAY_US(ONE_WIRE_TIMEOUT_MS-ONE_WIRE_ALARM_TIME_US-ONE_WIRE_PRESENSE_TIME_US);

/* If the line is still low then something is damaged or stuck. */
if( BIT_IS_CLEAR(ow_pin_reg, ow_pin ) )
 { 
    ow_system_message=OW_BUS_ERROR;
 }

return;
}
/*######################################################################################################*/

void ow_put_byte(unsigned char data)
{
unsigned char bit=0;

        /* write the byte, lsb first */
	for(bit=0; bit<=7; bit++)
	  {
              ow_put_bit(data & 0x01);
	      data = (data>>1);
	  }



return;
}
/*######################################################################################################*/

unsigned char ow_get_byte(void)
{
register unsigned char bit=0;
unsigned char byte=0;


        /* read the byte, lsb first */
	for(bit=0; bit<=7; bit++)
	  {
             if(ow_get_bit()) { byte|=(1<<bit); }
      	  }


return(byte);
}
/*######################################################################################################*/

void ow_read_rom(unsigned char *mem_array)
{
unsigned char x=0;

   ow_reset();
   ow_command(OW_READ_ROM);
   for(x=0; x<8; x++) { *(mem_array+x)=ow_get_byte(); }
   ow_reset();

return;
}
/*######################################################################################################*/

unsigned char ow_search_rom(unsigned char *mem_array, unsigned char command)
{
static unsigned char last_descrepancy, done_flag, rom_codes_found;
unsigned char        a=0, b=0, bit=0, descrepancy_marker=0, rom_bit_index=0;


/* If the command is "OW_RESET_SEARCH_ROM", set all static variables to 0 */
if(command == OW_RESET_SEARCH_ROM )
 {
    last_descrepancy=0;
    done_flag=0;
    rom_codes_found=0;
    /* Clear the code containing array */
    for(a=0; a<8; a++) { *(mem_array+a)=0; }
    return(0);
 } 

if(done_flag) { ow_system_message=OW_SEARCH_ROM_FINISHED; return(rom_codes_found); }

ow_reset();
if(ow_get_msg()==OW_NO_DEVICE_PRESENT || ow_get_msg()==OW_BUS_ERROR || ow_get_msg()==OW_ALARM_DETECTED)
 {
    last_descrepancy=0;
    ow_system_message=OW_SEARCH_ROM_FINISHED;
    return(0);
 } 

/* Initialization of non static variables */
rom_bit_index=1;
descrepancy_marker=0;

ow_command(command);


while(1)
    {
      a=ow_get_bit();
      b=ow_get_bit();

      if(a==b && a==1) 
       {
          last_descrepancy=0;
       	  rom_codes_found=0;
          ow_system_message=OW_SEARCH_ROM_FINISHED;
          return(0);
       }
      else if(a==b && a==0)
            {
               if(rom_bit_index==last_descrepancy)
                { 
                  *(mem_array+((rom_bit_index-1)/8)) |= (1<<((rom_bit_index-1)%8)); 
                  ow_put_bit(1);
                  rom_bit_index++;
                }
               else{
                     if(rom_bit_index > last_descrepancy)
                      {
                        *(mem_array+((rom_bit_index-1)/8)) &= (~(1<<((rom_bit_index-1)%8))); 
                        descrepancy_marker=rom_bit_index;
                        ow_put_bit(0);
                        rom_bit_index++;
                      }    
                     else{
                            bit=( *(mem_array+((rom_bit_index-1)/8)) & (1<<((rom_bit_index-1)%8)) ); 
                            if(bit == 0)          
                             {
                                descrepancy_marker=rom_bit_index;
                             }
                            ow_put_bit(bit);
                            rom_bit_index++; 
                         }
                    }    

            }
           else{
                 if(a) { *(mem_array+((rom_bit_index-1)/8)) |= (1<<((rom_bit_index-1)%8));    }
                 else  { *(mem_array+((rom_bit_index-1)/8)) &= (~(1<<((rom_bit_index-1)%8))); } 	
                 ow_put_bit(a);
                 rom_bit_index++;
               }    

      if(rom_bit_index>64) { break; }  /* the loop continues to the 64th bit of the ROM code */

    }

/* out of the while(1) loop code that sets the variables for the next call of this function */
last_descrepancy=descrepancy_marker;

/* If last_descrepancy == 0 means that all rom codes have been found, else the rom search is not finished */
if(last_descrepancy == 0)
 { 
   done_flag=1;
   rom_codes_found++;
   ow_system_message=OW_SEARCH_ROM_FINISHED;
 }
else{ rom_codes_found++; ow_system_message=OW_SEARCH_ROM_ACTIVE; }

ow_reset();

return(rom_codes_found); 
}
/*######################################################################################################*/

unsigned char ow_get_msg(void)
{

return(ow_system_message);
}







