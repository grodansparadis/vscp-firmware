/****************************************************************************
 Title  :   S file for the SOFTWARE UART FUNCTIONS library (softuart.s)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      10/Nov/2002
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.


*****************************************************************************/

/*################### DO NOT CHANGE ANYTHING BELOW THIS LINE !!! #####################################*/
/*############## ALL CONFIGURATION IS DONE IN "softuart.cfg" file !!! ################################*/

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif  
/*
   this is for GCC 3.2 . It shouldn't be needed since i didn't define __ASSEMBLER__ anywhere
   but it does since __ASSEMBLER__ is defined somehow and i dont know why... 
*/
#if defined(__ASSEMBLER__)
#define _VECTOR(N) __vector_ ## N  
#endif


#include <io-avr.h>
#include "softuart.cfg"



/*------------------------------------------------------------------------------------------------------*/
/* Warning messages */
#if SUART_TIMEOUT_MODE_CHANGE == 1
#warning " NO TIMEOUT IN INTERRUPT MODE "
#endif

#if    SUART_STOP_BIT_DETECTION > 2
#error " SUART_STOP_BIT_DETECTION CAN BE 0,1 OR 2 "
#endif

/*------------------------------------------------------------------------------------------------------*/

#if   SUART_WILL_USE_A_TIMER == 0 && SUART_DYNAMIC_BAUD_CHANGE == 0

#if    SUART_DELAY1 <=2 || SUART_DELAY2 <= 3 || SUART_DELAY3 <= 3 
#error " BAUDRATE TOO HIGH "
#elif  SUART_DELAY1 > 255 || SUART_DELAY2 > 255 || SUART_DELAY3 > 255
#error " BAUDRATE TOO LOW USE A TIMER "
#endif

#elif SUART_WILL_USE_A_TIMER == 1  && SUART_DYNAMIC_BAUD_CHANGE == 0

#if   TIME_HALF_BIT <= 0 || TIME_1_BIT <= 0 || SUART_BAUDRATE_DELAY < (SUART_CORRECTION_FACTOR*2)
#error " BAUDRATE TOO HIGH "
#endif

#endif /* #elif SUART_WILL_USE_A_TIMER == 1  && SUART_DYNAMIC_BAUD_CHANGE == 0 */

/*------------------------------------------------------------------------------------------------------*/

#if SUART_DYNAMIC_BAUD_CHANGE == 0

#if   (SUART_AVAILABLE_CPU_CYCLES < (RX_PROCESSING_CODE_SIZE*2)) 

#warning " NOT ENOUGH TIME TO PROCESS RX DATA "

#if    SUART_STOP_BIT_DETECTION == 0 || SUART_STOP_BIT_DETECTION == 1
#warning " REDUCE BAUDRATE OR INCREASE # OF STOP BITS "
#elif  SUART_STOP_BIT_DETECTION == 2
#warning " REDUCE BAUDRATE "
#endif

#endif /* #if   (SUART_AVAILABLE_CPU_CYCLES < (RX_PROCESSING_CODE_SIZE*2))  */

#endif /* #if SUART_DYNAMIC_BAUD_CHANGE == 0 */

/*------------------------------------------------------------------------------------------------------*/

#if defined(__AVR_ATmega103__)

#undef  DDRC 
#undef  DDRF 
#undef  PINC 
#undef  PORTF

#define DDRC   0
#define DDRF   0
#define PINC   0
#define PORTF  0

#if SUART_RX_PIN_REG == 0 
#error " THE SUART RX PIN IS OUTPUT ONLY "
#endif

#if SUART_TX_PORT == 0 
#error " THE SUART TX PIN IS INPUT ONLY "
#endif

#if SUART_RX_PIN_REG == PINF
#undef  SUART_RX_PORT_IS_IO
#define SUART_RX_PORT_IS_IO    0
#endif

#if SUART_TX_PORT == PORTC
#undef  SUART_TX_PORT_IS_IO
#define SUART_TX_PORT_IS_IO    0
#endif

#endif

/*------------------------------------------------------------------------------------------------------*/

/* CALCULATE THE I/O REGISTER ADDRESSES */
#if SREG > 0X3F 

#define S_TX_DDR_REG  SUART_TX_DDR_REG - 0X20
#define S_TX_PORT     SUART_TX_PORT    - 0X20
#define S_RX_DDR_REG  SUART_RX_DDR_REG - 0X20
#define S_RX_PIN_REG  SUART_RX_PIN_REG - 0X20
#define S_INT_REG     SUART_INT_REG    - 0X20
#define AVR_TCNT      SUART_TCNT       - 0X20
#define AVR_TCCR      SUART_TCCR       - 0X20
#define AVR_SREG      SREG             - 0X20
#define AVR_EIFR      SUART_INT_FLAG_REG - 0X20

#elif SREG <= 0X3F  

#define S_TX_DDR_REG  SUART_TX_DDR_REG 
#define S_TX_PORT     SUART_TX_PORT    
#define S_RX_DDR_REG  SUART_RX_DDR_REG 
#define S_RX_PIN_REG  SUART_RX_PIN_REG 
#define S_INT_REG     SUART_INT_REG 
#define AVR_TCNT      SUART_TCNT 
#define AVR_TCCR      SUART_TCCR       
#define AVR_SREG      SREG 
#define AVR_EIFR      SUART_INT_FLAG_REG 

#endif  

/*------------------------------------------------------------------------------------------------------*/

/* REGISTER DEFINITIONS */
#define          z_high       r31 
#define          z_low        r30          
#define          s_index      r27
#define          s_temp2      r27          
#define          s_temp1      r26 
#define          s_sreg       r25
#define          s_Rxbyte     r24 
#define          s_Txbyte     r24          
#define          s_bitcnt     r23


/*#######################################################################################################*/

                 .global SUART_PUTCHAR
                 .func SUART_PUTCHAR

SUART_PUTCHAR:   

#if SUART_DYNAMIC_IO_SETTINGS == 1
                 clr      z_high                    /* cbi using variables */
                 lds      z_low,  suart_port
#endif
              
                 ldi      s_bitcnt,9+SUART_STOP_BITS
                 com      s_Txbyte                  /* INVERT THE BYTE TO BE TRANSMITED */
                 sec                                /* SET CARRY (CARRY=1) */

#if    SUART_USE_RS232_LOGIC == 0 
suart_putchar0:  brcc     suart_putchar1            /* IF CARRY IS CLEAR SET THE TX PIN TO 1 */
#elif  SUART_USE_RS232_LOGIC == 1
suart_putchar0:  brcs     suart_putchar1            /* IF CARRY IS SET SET THE TX PIN TO 1 */
#endif 

#if SUART_DYNAMIC_IO_SETTINGS == 1
                 ld       s_temp1, z
                 lds      s_temp2, suart_tx_pin       
                 com      s_temp2 
                 and      s_temp1, s_temp2
                 st       z, s_temp1                     
                 rjmp     suart_putchar2          
                                                    /* sbi using variables */
suart_putchar1:  ld       s_temp1, z
                 lds      s_temp2, suart_tx_pin 
                 or       s_temp1, s_temp2
                 st       z, s_temp1           
                 nop
                 nop

#elif SUART_DYNAMIC_IO_SETTINGS == 0
                 cbi      S_TX_PORT,SUART_TX_PIN    /* ELSE SET THE TX PIN TO 1 */
                 rjmp     suart_putchar2          

suart_putchar1:  sbi      S_TX_PORT,SUART_TX_PIN         
                 nop
#endif
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if SUART_WILL_USE_A_TIMER == 0  &&  SUART_DYNAMIC_BAUD_CHANGE == 1                
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

suart_putchar2:  lds      s_temp1, suart_delay2     /* 1/2 bit delay loop */
                 subi     s_temp1, 1                /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */ 
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
suart_tx_delay1: dec      s_temp1
                 brne     suart_tx_delay1
                 
                 lds      s_temp1, suart_delay3     /* 1/2 bit delay loop. total delay 1 bit*/
                 subi     s_temp1, 1                /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
suart_tx_delay2: dec      s_temp1
                 brne     suart_tx_delay2

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_WILL_USE_A_TIMER == 0  &&  SUART_DYNAMIC_BAUD_CHANGE == 0 
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

suart_putchar2:  ldi      s_temp1, SUART_DELAY2     /* 1/2 bit delay loop */       
suart_tx_delay1: dec      s_temp1                   /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */
                 brne     suart_tx_delay1

                 ldi      s_temp1, SUART_DELAY3     /* 1/2 bit delay loop */ 
suart_tx_delay2: dec      s_temp1                   /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */
                 brne     suart_tx_delay2           

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_WILL_USE_A_TIMER == 1  
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

suart_putchar2:  clr      s_temp1                   /* 1 bit delay loop using a timer */
                 out      AVR_TCNT, s_temp1
                                                  
#if   SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_1_BIT
#elif SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_1_bit
#endif 
                                 
suart_tx_delay1: in       s_temp1, AVR_TCNT
                 cp       s_temp1, s_temp2
                 brlo     suart_tx_delay1

/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif
/*11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

                 lsr      s_Txbyte                  /* SHIFT LEFT TX BYTE THROUGH CARRY */
                 dec      s_bitcnt                  /* DECREMENT BITCOUNT */
                 brne     suart_putchar0            /* TRANSMIT NEXT BIT  */
                                     
                 ret                             


.endfunc
                
/*####################################################################################################*/
/*####################################################################################################*/
#if SUART_DYNAMIC_IO_SETTINGS == 0
/*####################################################################################################*/
/*####################################################################################################*/

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if   SUART_WILL_USE_INTERRUPT  == 1   /* INTERRUPT MODE FOR NON DYNAMIC PORT-PIN RECEIVING */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/


                 .global SUART_RX_FUNCTION
                 .func   SUART_RX_FUNCTION

SUART_RX_FUNCTION: 

/*#############    It took at least 5 cycles to get here ###############*/

                 push    s_sreg                     /* SAVE THE SREG REGISTER FIRST */
                 in      s_sreg, AVR_SREG

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_WILL_USE_A_TIMER == 0  &&  SUART_DYNAMIC_BAUD_CHANGE == 1                    
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                 push    s_temp1                    /* SAVE THE REGISTERS TO BE USED */
                 push    s_temp2
                 push    z_low
                 push    z_high
                 push    s_Rxbyte                    
                 push    s_bitcnt 

                 ldi     s_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */

/********************************* TIMING  CODE BLOCK START ***********************************************/
              
suart_getchar1:  lds     s_temp1, suart_delay1      /* 1/2 bit delay loop to be counted only once */
suart_rx_delay1: dec     s_temp1                     
                 brne    suart_rx_delay1                    
                                                
suart_getchar2:  lds     s_temp1, suart_delay2      /* 1/2 bit delay loop */
                 subi    s_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */   
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */   
suart_rx_delay2: dec     s_temp1                     
                 brne    suart_rx_delay2
                 
                 lds     s_temp1, suart_delay3      /* 1/2 bit delay loop */
                 subi    s_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */   
suart_rx_delay3: dec     s_temp1                     
                 brne    suart_rx_delay3                           

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/ 
#elif SUART_WILL_USE_A_TIMER == 0  &&  SUART_DYNAMIC_BAUD_CHANGE == 0
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                 push    s_temp1 
                 push    s_temp2                       
                 push    z_low                     
                 push    z_high                    
                 push    s_Rxbyte                    
                 push    s_bitcnt 

                 ldi     s_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */
               
suart_getchar1:  ldi     s_temp1, SUART_DELAY1      /* 1/2 bit delay loop to be counted only once */
suart_rx_delay1: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay1                    
                                                
suart_getchar2:  ldi     s_temp1, SUART_DELAY2      /* 1/2 bit delay loop */
suart_rx_delay2: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay2
     
                 ldi     s_temp1, SUART_DELAY3      /* 1/2 bit delay loop */   
suart_rx_delay3: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay3                                 

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/                                                        
#elif SUART_WILL_USE_A_TIMER == 1  
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                 push    s_temp1                               
                 clr     s_temp1                    /* 1/2 bit delay loop to be counted only once */
                 out     AVR_TCNT, s_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */
                 push    s_temp2
                 push    z_low
                 push    z_high
                 push    s_Rxbyte
                 push    s_bitcnt 

                 ldi     s_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */   
#if   SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_HALF_BIT
#elif SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_half_bit
#endif

/***************   Dead time end   ************/
suart_rx_delay1: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay1
            
suart_getchar2:  clr      s_temp1                   /* 1 bit delay loop using a timer */
                 out      AVR_TCNT, s_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if   SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_1_BIT
#elif SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_1_bit
#endif 
/***************   Dead time end   ************/                              
suart_rx_delay2: in       s_temp1, AVR_TCNT
                 cp       s_temp1, s_temp2
                 brlo     suart_rx_delay2

/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/                                                
#endif  /* #if SUART_WILL_USE_A_TIMER == 0  &&  SUART_DYNAMIC_BAUD_CHANGE == 1  */
/*22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/********************************* TIMING  CODE BLOCK END *************************************************/

/**************************  MAIN DATA HADLING CODE BLOCK START *******************************************/
                 clc                               /* CLEAR CARRY */                

#if    SUART_USE_RS232_LOGIC == 0                  
                 sbic    S_RX_PIN_REG, SUART_RX_PIN /* IF RX PIN IS LOW CLEAR CARRY */             
#elif  SUART_USE_RS232_LOGIC == 1
                 sbis    S_RX_PIN_REG, SUART_RX_PIN /* IF RX PIN IS HIGH CLEAR CARRY */             
#endif


                 sec
suart_rotate:    ror     s_Rxbyte                  /* ROTATE RIGHT THE RX BYTE THROUGH CARRY */      
                 dec     s_bitcnt                  /* DECREMENT BITCOUNT */
                 breq    suart_getchar3            /* IF THIS WAS THE LAST BIT WRITE BYTE TO BUFFER */
                                                   /* ELSE */                                               
                 rjmp    suart_getchar2            /* GO GET NEXT BIT */ 

suart_getchar3:

/**************************  MAIN DATA HADLING CODE BLOCK END *******************************************/

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_WILL_USE_A_TIMER == 0 && SUART_STOP_BIT_DETECTION == 2 
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if    SUART_DYNAMIC_BAUD_CHANGE == 0

                 ldi     s_bitcnt, SUART_STOP_BITS    /* load the number of stop bits */
              
suart_getchar4:  ldi     s_temp1, SUART_DELAY2        /* 1 bit delay */
SUART_rx_delay4: dec     s_temp1
                 brne    SUART_rx_delay4

                 ldi     s_temp1, SUART_DELAY3      
SUART_rx_delay5: dec     s_temp1
                 brne    SUART_rx_delay5

                 dec     s_bitcnt                    /* If this was the last stop bit continue */
                 brne    suart_getchar4                  /* else apply 1 more bit delay             */

#elif    SUART_DYNAMIC_BAUD_CHANGE == 1

                 ldi     s_bitcnt, SUART_STOP_BITS    /* load the number of stop bits */
              
suart_getchar4:  lds     s_temp1, suart_delay2        /* 1/2 bit delay */
                 dec     s_temp1
                 nop
SUART_rx_delay4: dec     s_temp1
                 brne    SUART_rx_delay4

                 lds     s_temp1, suart_delay3        /* 1 more 1/2 bit delay total 1 bit */
                 dec     s_temp1
                 nop
SUART_rx_delay5: dec     s_temp1
                 brne    SUART_rx_delay5

                 dec     s_bitcnt                    /* If this was the last stop bit continue */
                 brne    suart_getchar4                  /* else apply 1 more bit delay             */

#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif SUART_WILL_USE_A_TIMER == 1 && SUART_STOP_BIT_DETECTION == 2  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                 ldi     s_bitcnt, SUART_STOP_BITS
            
suart_getchar4:  clr     s_temp1                     /* 1 bit delay loop using a timer */
                 out     AVR_TCNT, s_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_1_BIT 
#elif  SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_1_bit
#endif
/***************   Dead time end   ************/ 
suart_rx_delay4: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay4

                 dec     s_bitcnt
                 brne    suart_getchar4

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/



/* ######### RECEIVING IS COMPLETE SO LETS START WRITTING THE BUFFER ############## */
/* ######### RX DATA PROCESSING FOR NON DYNAMIC PORT-PIN MODE        ############## */
suart_wr_buf_0:  sts     rx_c, r24                           
                 lds     s_index, suart_rx_count

                 lds     s_temp1, suart_receive_string
                 cpi     s_temp1, 1
                 brne    suart_wr_buf_1             
                

                 cpi     s_Rxbyte, RX_TERMINATION_CHAR          
                 breq    suart_term_recv0     

suart_wr_buf_1:  cpi     s_index, SUART_BUF_SIZE     
                 breq    suart_buf_reset                         

suart_wr_buf_2:  ldi     z_low, lo8(suart_rx_buffer)
                 ldi     s_temp1, hi8(suart_rx_buffer)         
                 add     z_low, s_index              
                 adc     z_high,s_temp1                       
suart_wr_buf_3:  st      Z+, s_Rxbyte                          
                 inc     s_index                      
                 clr     s_temp1                              
                 st      Z, s_temp1                            
                              
exit_suart_0:    lds     s_temp1, suart_set_mark
                 tst     s_temp1
                 dec     s_temp1
                 breq    suart_term_recv1
                 sts     suart_set_mark, s_temp1


exit_suart_1:    sts     suart_rx_count, s_index
#if         EXT_INT_EDGE_TRIGERING == 1 
                 ldi     s_temp1, (1<<SUART_INT_FLAG)
                 out     AVR_EIFR, s_temp1
                 ;sts    (AVR_EIFR+0x20), s_temp1
#endif
                 pop     s_bitcnt
                 pop     s_Rxbyte                               
                 pop     z_high      
                 pop     z_low
                 pop     s_temp2
                 pop     s_temp1
                 out     AVR_SREG, s_sreg
                 pop     s_sreg

#if SUART_STOP_BIT_DETECTION == 0 || SUART_STOP_BIT_DETECTION == 1
#if    SUART_USE_RS232_LOGIC == 0 
exit_suart_2:    sbis    S_RX_PIN_REG, SUART_RX_PIN
                 rjmp    exit_suart_2                        
#elif  SUART_USE_RS232_LOGIC == 1 
exit_suart_2:    sbic    S_RX_PIN_REG, SUART_RX_PIN
                 rjmp    exit_suart_2         
#endif
#endif
                 reti                                        

suart_term_recv0:inc     s_index
suart_term_recv1:in      s_temp1, S_INT_REG                       
                 andi    s_temp1, (~(1<<SUART_INT_BIT))                   
                 out     S_INT_REG, s_temp1                     
                 clr     s_temp1
                 sts     suart_set_mark, s_temp1
                 sts     suart_string_received, s_index  
                 rjmp    exit_suart_1

suart_buf_reset: ldi     s_index, 0xFF
                 sts     suart_set_mark, s_index
                 clr     s_index                     
                 ldi     z_low,  lo8(suart_rx_buffer)        
                 ldi     z_high, hi8(suart_rx_buffer)        
                 rjmp    suart_wr_buf_3 



.endfunc


/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_WILL_USE_INTERRUPT  == 0  /* NON INTERRUPT MODE FOR NON DYNAMIC PORT-PIN RECEIVING */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

                 .global SUART_RX_FUNCTION
                 .func   SUART_RX_FUNCTION

SUART_RX_FUNCTION: 

/************************** WATCH FOR A PROPER START BIT CODE BLOCK START ********************************/

#if    SUART_TIMEOUT_ENABLED == 1
                 ldi  r24, lo8(SUART_TIMEOUT_LOOPS)
                 ldi  r25, hi8(SUART_TIMEOUT_LOOPS)
                 ldi  r26, hlo8(SUART_TIMEOUT_LOOPS)
                 ldi  r27, hhi8(SUART_TIMEOUT_LOOPS)
#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if    SUART_USE_RS232_LOGIC == 0  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if SUART_STOP_BIT_DETECTION == 0 
suart_wait_high: sbis    S_RX_PIN_REG, SUART_RX_PIN /* WAIT FOR A HIGH TO LOW TRANSITION OF THE RX PIN */
                 rjmp    suart_wait_high 
#endif
           

suart_wait_low:
#if    SUART_TIMEOUT_ENABLED == 1
                 subi    r24, lo8(-(-1))
                 sbci    r25, hi8(-(-1))  
                 sbci    r26, hlo8(-(-1)) 
                 sbci    r27, hhi8(-(-1)) 
                 in      r30, AVR_SREG
                 sbrc    r30, 1
                 rjmp    suart_timeout1
#endif
                 sbic    S_RX_PIN_REG, SUART_RX_PIN 
                 rjmp    suart_wait_low            

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif  SUART_USE_RS232_LOGIC == 1  /* #if    SUART_USE_RS232_LOGIC == 0  */
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if SUART_STOP_BIT_DETECTION == 0
suart_wait_low:  sbic    S_RX_PIN_REG, SUART_RX_PIN /* WAIT FOR A LOW TO HIGH TRANSITION OF THE RX PIN */
                 rjmp    suart_wait_low            
#endif

suart_wait_high: 
#if    SUART_TIMEOUT_ENABLED == 1                 
                 subi    r24, lo8(-(-1))
                 sbci    r25, hi8(-(-1))  
                 sbci    r26, hlo8(-(-1)) 
                 sbci    r27, hhi8(-(-1)) 
                 in      r30, AVR_SREG
                 sbrc    r30, 1
                 rjmp    suart_timeout1
#endif
                 sbis    S_RX_PIN_REG, SUART_RX_PIN          
                 rjmp    suart_wait_high            

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif  /* #elif  SUART_USE_RS232_LOGIC == 1 */
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/************************** WATCH FOR A PROPER START BIT CODE BLOCK END *********************************/


/*********************************** TIMING  CODE BLOCK START *******************************************/
 
                 ldi     s_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */                                                                                          

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_WILL_USE_A_TIMER == 0                     
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if  SUART_DYNAMIC_BAUD_CHANGE == 1 
              
                 lds     s_temp1, suart_delay1      /* 1/2 bit delay loop to be counted only once */
suart_rx_delay1: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay1                    
                                                
                
suart_getchar2:  lds     s_temp1, suart_delay2      /* 1 bit delay loop */
                 subi    s_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */   
suart_rx_delay2: dec     s_temp1                     
                 brne    suart_rx_delay2                       

                 lds     s_temp1, suart_delay3      /* 1/2 bit delay loop */
                 subi    s_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */   
suart_rx_delay3: dec     s_temp1                     
                 brne    suart_rx_delay3                 

#elif  SUART_DYNAMIC_BAUD_CHANGE == 0

                 ldi     s_temp1, SUART_DELAY1      /* 1/2 bit delay loop to be counted only once */
suart_rx_delay1: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */
                 brne    suart_rx_delay1                    
                                                
suart_getchar2:  ldi     s_temp1, SUART_DELAY2      /* 1/2 bit delay loop */
suart_rx_delay2: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay2
              
                 ldi     s_temp1, SUART_DELAY3      /* 1/2 bit delay loop */   
suart_rx_delay3: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay3                       

#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif SUART_WILL_USE_A_TIMER == 1  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                 clr     s_temp2                    /* 1/2 bit delay loop to be counted only once */
                 out     AVR_TCNT, s_temp2
/* Dead time start(TIMER IS ALREADY COUNTING). */                        
#if    SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_HALF_BIT 
#elif  SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_half_bit
#endif    
/***************   Dead time end   ************/                                                       
suart_rx_delay1: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay1
            
suart_getchar2:  clr     s_temp1                    /* 1 bit delay loop using a timer */
                 out     AVR_TCNT, s_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_1_BIT 
#elif  SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_1_bit
#endif
/***************   Dead time end   ************/                             
suart_rx_delay2: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay2

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/*********************************** TIMING CODE BLOCK END ************************************************/
    
/**************************  MAIN DATA HADLING CODE BLOCK START *******************************************/
                 clc                               /* CLEAR CARRY */                

#if    SUART_USE_RS232_LOGIC == 0                  
                 sbic    S_RX_PIN_REG, SUART_RX_PIN /* IF RX PIN IS LOW CLEAR CARRY */             
#elif  SUART_USE_RS232_LOGIC == 1
                 sbis    S_RX_PIN_REG, SUART_RX_PIN /* IF RX PIN IS HIGH CLEAR CARRY */             
#endif


                 sec
suart_rotate:    ror     s_Rxbyte                  /* ROTATE RIGHT THE RX BYTE THROUGH CARRY */      
                 dec     s_bitcnt                  /* DECREMENT BITCOUNT */
                 breq    suart_getchar3            /* IF THIS WAS THE LAST BIT WRITE BYTE TO BUFFER */
                                                   /* ELSE */                                               
                 rjmp    suart_getchar2            /* GO GET NEXT BIT */ 

suart_getchar3:
/**************************  MAIN DATA HADLING CODE BLOCK END  *******************************************/

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#if SUART_WILL_USE_A_TIMER == 0 && SUART_STOP_BIT_DETECTION == 2 
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

#if    SUART_DYNAMIC_BAUD_CHANGE == 0

                 ldi     s_bitcnt, SUART_STOP_BITS    /* load the number of stop bits */
              
suart_getchar4:  ldi     s_temp1, SUART_DELAY2        /* 1 bit delay */
SUART_rx_delay4: dec     s_temp1
                 brne    SUART_rx_delay4

                 ldi     s_temp1, SUART_DELAY3      
SUART_rx_delay5: dec     s_temp1
                 brne    SUART_rx_delay5

                 dec     s_bitcnt                    /* If this was the last stop bit continue */
                 brne    suart_getchar4                  /* else apply 1 more bit delay             */

#elif    SUART_DYNAMIC_BAUD_CHANGE == 1

                 ldi     s_bitcnt, SUART_STOP_BITS    /* load the number of stop bits */
              
suart_getchar4:  lds     s_temp1, suart_delay2        /* 1/2 bit delay */
                 dec     s_temp1
                 nop
SUART_rx_delay4: dec     s_temp1
                 brne    SUART_rx_delay4

                 lds     s_temp1, suart_delay3        /* 1 more 1/2 bit delay total 1 bit */
                 dec     s_temp1
                 nop
SUART_rx_delay5: dec     s_temp1
                 brne    SUART_rx_delay5

                 dec     s_bitcnt                    /* If this was the last stop bit continue */
                 brne    suart_getchar4                  /* else apply 1 more bit delay             */

#endif

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#elif SUART_WILL_USE_A_TIMER == 1 && SUART_STOP_BIT_DETECTION == 2  
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

                 ldi     s_bitcnt, SUART_STOP_BITS
            
suart_getchar4:  clr     s_temp1                     /* 1 bit delay loop using a timer */
                 out     AVR_TCNT, s_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_1_BIT 
#elif  SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_1_bit
#endif
/***************   Dead time end   ************/ 
suart_rx_delay4: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay4

                 dec     s_bitcnt
                 brne    suart_getchar4

/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/
#endif
/*2222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222*/

/* ######### RECEIVING IS COMPLETE SO LETS START WRITTING THE BUFFER ############## */
/* ######### RX DATA PROCESSING FOR NON DYNAMIC PORT-PIN MODE        ############## */

suart_wr_buf_0:  sts     rx_c, r24                           
                 lds     s_index, suart_rx_count
                             
                 lds     s_temp1, suart_receive_string
                 cpi     s_temp1, 1
                 brne    suart_wr_buf_1             

                 cpi     s_Rxbyte, RX_TERMINATION_CHAR          
                 breq    suart_term_recv0     

suart_wr_buf_1:  cpi     s_index, SUART_BUF_SIZE     
                 breq    suart_buf_reset                         

suart_wr_buf_2:  ldi     z_low, lo8(suart_rx_buffer)
                 ldi     s_temp1, hi8(suart_rx_buffer)         
                 add     z_low, s_index              
                 adc     z_high,s_temp1                       
suart_wr_buf_3:  st      Z+, s_Rxbyte                          
                 inc     s_index                      
                 clr     s_temp1                              
                 st      Z, s_temp1                            
                 
exit_suart_0:    sts     suart_rx_count, s_index 

#if   SUART_STOP_BIT_DETECTION == 1 && SUART_USE_RS232_LOGIC == 0 
exit_suart_1:    sbis    S_RX_PIN_REG, SUART_RX_PIN           /* Wait for high level            */
                 rjmp    exit_suart_1                         /* which should be the stop bit   */
#elif SUART_STOP_BIT_DETECTION == 1 && SUART_USE_RS232_LOGIC == 1
exit_suart_1:    sbic    S_RX_PIN_REG, SUART_RX_PIN           /* Wait for low level             */
                 rjmp    exit_suart_1                         /* which should be the stop bit   */
#endif
                 ret                                         

#if    SUART_TIMEOUT_ENABLED == 1
suart_timeout1:  ldi     s_Rxbyte, RX_TERMINATION_CHAR
                 sts     rx_c, s_Rxbyte 
                 lds     s_index, suart_rx_count 
                 tst     s_index
                 brne    suart_timeout2
                 ldi     s_index, 0xFF
suart_timeout2:  sts     suart_string_received, s_index 
                 sts     suart_rx_count, s_index
                 ldi     s_index, 1
                 sts     suart_timeout_flag, s_index
                 ret
#endif

suart_term_recv0:inc     s_index
                 sts     suart_string_received, s_index           
                 sts     suart_rx_count, s_index
                 ret

suart_buf_reset: clr     s_index                     
                 ldi     z_low,  lo8(suart_rx_buffer)        
                 ldi     z_high, hi8(suart_rx_buffer)        
                 rjmp    suart_wr_buf_3 

.endfunc

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/



/*####################################################################################################*/
/*####################################################################################################*/
#elif SUART_DYNAMIC_IO_SETTINGS == 1   /* NON INTERRUPT DYNAMIC PORT-PIN MODE */
/*####################################################################################################*/
/*####################################################################################################*/

                 .global SUART_RX_FUNCTION
                 .func   SUART_RX_FUNCTION

SUART_RX_FUNCTION: 

#if    SUART_TIMEOUT_ENABLED == 1
                 ldi  r22, lo8(SUART_TIMEOUT_LOOPS)
                 ldi  r23, hi8(SUART_TIMEOUT_LOOPS)
                 ldi  r24, hlo8(SUART_TIMEOUT_LOOPS)
                 ldi  r25, hhi8(SUART_TIMEOUT_LOOPS)
#endif

                 clr      z_high
                 lds      z_low,  suart_port
                 subi     z_low, 2
                 lds      s_temp2, suart_rx_pin 

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if    SUART_USE_RS232_LOGIC == 0  
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if SUART_STOP_BIT_DETECTION == 0 

suart_wait_high: ld       s_temp1, z
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 breq     suart_wait_high            
#endif


suart_wait_low:
#if    SUART_TIMEOUT_ENABLED == 1               
                 subi r22, lo8(-(-1))
                 sbci r23, hi8(-(-1))  
                 sbci r24, hlo8(-(-1)) 
                 sbci r25, hhi8(-(-1)) 
                 in   s_temp1, AVR_SREG
                 sbrc s_temp1, 1
                 rjmp suart_timeout1 
#endif
                 ld       s_temp1, z
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 brne     suart_wait_low 

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif  SUART_USE_RS232_LOGIC == 1
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if SUART_STOP_BIT_DETECTION == 0 

suart_wait_low:  ld       s_temp1, z
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 brne     suart_wait_low 

#endif

suart_wait_high:
#if    SUART_TIMEOUT_ENABLED == 1
                 subi r22, lo8(-(-1))
                 sbci r23, hi8(-(-1))  
                 sbci r24, hlo8(-(-1)) 
                 sbci r25, hhi8(-(-1)) 
                 in   s_temp1, AVR_SREG
                 sbrc s_temp1, 1
                 rjmp suart_timeout1 

#endif               
                 ld       s_temp1, z
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 breq     suart_wait_high                

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #elif  SUART_USE_RS232_LOGIC == 1 */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
                  
                 ldi     s_bitcnt, 8                /* STOP BIT IS EATEN AND USED FOR RX DATA PROCESSING */                                                                                          

#if SUART_WILL_USE_A_TIMER == 0  

#if   SUART_DYNAMIC_BAUD_CHANGE == 1                    
              
                 lds     s_temp1, suart_delay1      /* 1/2 bit delay loop to be counted only once */
suart_rx_delay1: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay1                    
                                                
                
suart_getchar2:  lds     s_temp1, suart_delay2      /* 1 bit delay loop */
                 subi    s_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */   
suart_rx_delay2: dec     s_temp1                     
                 brne    suart_rx_delay2                       

                 lds     s_temp1, suart_delay3      /* 1/2 bit delay loop */
                 subi    s_temp1, 1                 /* SUBTRACT 1 TO COMPENSATE FOR THE Lds's 2 CYCLES */  
                 nop                                /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */   
suart_rx_delay3: dec     s_temp1                     
                 brne    suart_rx_delay3                 

#elif SUART_DYNAMIC_BAUD_CHANGE == 0

                 ldi     s_temp1, SUART_DELAY1      /* 1/2 bit delay loop to be counted only once */
suart_rx_delay1: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */
                 brne    suart_rx_delay1                    
                                                
suart_getchar2:  ldi     s_temp1, SUART_DELAY2      /* 1/2 bit delay loop */
suart_rx_delay2: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay2
              
                 ldi     s_temp1, SUART_DELAY3      /* 1/2 bit delay loop */   
suart_rx_delay3: dec     s_temp1                    /* TOTAL DELAY = (suart_delayX*3)*(1/F_CPU) */ 
                 brne    suart_rx_delay3                       

#endif /* #elif SUART_DYNAMIC_BAUD_CHANGE == 0 */

#elif SUART_WILL_USE_A_TIMER == 1  

                 clr     s_temp2                    /* 1/2 bit delay loop to be counted only once */
                 out     AVR_TCNT, s_temp2
/* Dead time start(TIMER IS ALREADY COUNTING). */                        
#if    SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_HALF_BIT 
#elif  SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_half_bit
#endif    
/***************   Dead time end   ************/                                                       
suart_rx_delay1: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay1
            
suart_getchar2:  clr     s_temp1                    /* 1 bit delay loop using a timer */
                 out     AVR_TCNT, s_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_1_BIT 
#elif  SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_1_bit
#endif
/***************   Dead time end   ************/                             
suart_rx_delay2: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay2

#endif   /* #elif SUART_WILL_USE_A_TIMER == 1  &&  SUART_DYNAMIC_BAUD_CHANGE == 0 */

                 clc                               /* CLEAR CARRY */                

#if    SUART_USE_RS232_LOGIC == 0                  

                 nop
                 ld       s_temp1, z                                      
                 lds      s_temp2, suart_rx_pin 
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 breq     suart_rotate
        
#elif  SUART_USE_RS232_LOGIC == 1
                 nop
                 ld       s_temp1, z
                 lds      s_temp2, suart_rx_pin         
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 brne     suart_rotate            /* IF RX PIN IS HIGH CLEAR CARRY */             
#endif

                 sec                               /* SET CARRY IF NOT SKIPPED BY SBI-CBI ABOVE */
suart_rotate:    ror     s_Rxbyte                  /* ROTATE RIGHT THE RX BYTE THROUGH CARRY */      
                 dec     s_bitcnt                  /* DECREMENT BITCOUNT */
                 breq    suart_getchar3            /* IF THIS WAS THE LAST BIT WRITE BYTE TO BUFFER */
                                                   /* ELSE */                                               
                 rjmp    suart_getchar2            /* GO GET NEXT BIT */ 

suart_getchar3:
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#if SUART_WILL_USE_A_TIMER == 0 && SUART_STOP_BIT_DETECTION == 2 
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

#if    SUART_DYNAMIC_BAUD_CHANGE == 0

                 ldi     s_bitcnt, SUART_STOP_BITS    /* load the number of stop bits */
              
suart_getchar4:  ldi     s_temp1, SUART_DELAY2        /* 1 bit delay */
SUART_rx_delay4: dec     s_temp1
                 brne    SUART_rx_delay4

                 ldi     s_temp1, SUART_DELAY3      
SUART_rx_delay5: dec     s_temp1
                 brne    SUART_rx_delay5

                 dec     s_bitcnt                    /* If this was the last stop bit continue */
                 brne    suart_getchar4                  /* else apply 1 more bit delay             */

#elif    SUART_DYNAMIC_BAUD_CHANGE == 1

                 ldi     s_bitcnt, SUART_STOP_BITS    /* load the number of stop bits */
              
suart_getchar4:  lds     s_temp1, suart_delay2        /* 1/2 bit delay */
                 dec     s_temp1
                 nop
SUART_rx_delay4: dec     s_temp1
                 brne    SUART_rx_delay4

                 lds     s_temp1, suart_delay3        /* 1 more 1/2 bit delay total 1 bit */
                 dec     s_temp1
                 nop
SUART_rx_delay5: dec     s_temp1
                 brne    SUART_rx_delay5

                 dec     s_bitcnt                    /* If this was the last stop bit continue */
                 brne    suart_getchar4                  /* else apply 1 more bit delay             */

#endif

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#elif SUART_WILL_USE_A_TIMER == 1 && SUART_STOP_BIT_DETECTION == 2  
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

                 ldi     s_bitcnt, SUART_STOP_BITS
            
suart_getchar4:  clr     s_temp1                     /* 1 bit delay loop using a timer */
                 out     AVR_TCNT, s_temp1
/* Dead time start(TIMER IS ALREADY COUNTING). */ 
#if    SUART_DYNAMIC_BAUD_CHANGE == 0
                 ldi     s_temp2, TIME_1_BIT 
#elif  SUART_DYNAMIC_BAUD_CHANGE == 1
                 lds     s_temp2, time_1_bit
#endif
/***************   Dead time end   ************/ 
suart_rx_delay4: in      s_temp1, AVR_TCNT
                 cp      s_temp1, s_temp2
                 brlo    suart_rx_delay4

                 dec     s_bitcnt
                 brne    suart_getchar4

/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/
#endif  /* #if SUART_WILL_USE_A_TIMER == 0 && SUART_STOP_BIT_DETECTION == 2 */
/*1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111*/

/* ######### RECEIVING IS COMPLETE SO LETS START WRITTING THE BUFFER ############## */
/* ######### RX DATA PROCESSING FOR DYNAMIC PORT-PIN MODE            ############## */
                 
suart_wr_buf_0:  sts     rx_c, r24                           
                 lds     s_index, suart_rx_count

                 lds     s_temp1, suart_receive_string
                 cpi     s_temp1, 1
                 brne    suart_wr_buf_1        

                 cpi     s_Rxbyte, RX_TERMINATION_CHAR          
                 breq    suart_term_recv0     

suart_wr_buf_1:  cpi     s_index, SUART_BUF_SIZE     
                 breq    suart_buf_reset                         

              

suart_wr_buf_2:  ldi     z_low, lo8(suart_rx_buffer)
                 ldi     s_temp1, hi8(suart_rx_buffer)         
                 add     z_low, s_index              
                 adc     z_high,s_temp1                       
suart_wr_buf_3:  st      Z+, s_Rxbyte                          
                 inc     s_index                      
                 clr     s_temp1                              
                 st      Z, s_temp1                            
                 
exit_suart_0:    sts     suart_rx_count, s_index 

#if    SUART_STOP_BIT_DETECTION == 1 && SUART_USE_RS232_LOGIC == 0 

                 clr      z_high
                 lds      z_low,  suart_port
                 subi     z_low, 2
                 lds      s_temp2, suart_rx_pin  
suart_wait_high: ld       s_temp1, z
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 breq     suart_wait_high            

#elif  SUART_STOP_BIT_DETECTION == 1 && SUART_USE_RS232_LOGIC == 1 

                 clr      z_high
                 lds      z_low,  suart_port
                 subi     z_low, 2
                 lds      s_temp2, suart_rx_pin 
suart_wait_low:  ld       s_temp1, z
                 and      s_temp1, s_temp2
                 tst      s_temp1                
                 brne     suart_wait_low 

#endif
                 ret                                         

suart_timeout1:  ldi     s_Rxbyte, RX_TERMINATION_CHAR
                 sts     rx_c, s_Rxbyte 
                 lds     s_index, suart_rx_count 
                 tst     s_index
                 brne    suart_timeout2
                 ldi     s_index, 0xFF
suart_timeout2:  sts     suart_string_received, s_index 
                 sts     suart_rx_count, s_index
                 ldi     s_index, 1
                 sts     suart_timeout_flag, s_index
                 ret

suart_term_recv0:inc     s_index
                 sts     suart_string_received, s_index           
                 sts     suart_rx_count, s_index
                 ret

suart_buf_reset: clr     s_index                     
                 ldi     z_low,  lo8(suart_rx_buffer)        
                 ldi     z_high, hi8(suart_rx_buffer)        
                 rjmp    suart_wr_buf_3 



.endfunc

/*####################################################################################################*/
/*####################################################################################################*/
#endif   /* #elif SUART_DYNAMIC_IO_SETTINGS == 1  */
/*####################################################################################################*/
/*####################################################################################################*/



                                      /* THANK GOD IS OVER!!! */
/*########################################################################################################*/
/*                                     T   H   E      E   N   D                                           */
/*########################################################################################################*/                

