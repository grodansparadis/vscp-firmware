;//
;// Select Device (Select one, comment out the others) 
;//
;//#define _ATMEGA32
#define _ATMEGA161 
;//#define _ATMEGA163 
;//#define _ATMEGA128 

;// Select Boot Size (select one, comment out the others).
;//   NB!!! Remember to manually change the linker command file accordingly !!!
;//   i.e Set "Code = xxxx-xxxx" accordingly. See issue 1) under the Special 
;//   considerations section in the Appnote.*/
;//#define _B128 
;//#define _B256 
#define _B512 
;//#define _B1024 
;//#define _B2048

;// Define pin for enter selfprogramming mode
#define PROGPORT PORTD
#define PROGDDR = DDRD
#define PROGPIN PIND
#define PROGCTRL PD4
#define PROGMODE !(PROGPIN & (1<<PROGCTRL))  // TRUE if PROGCTRL on PROGPIN is low

#ifdef _ATMEGA32 
  #include "iom32.h"
  #define PARTCODE 0x73
  
  #define sig_byte1 0x1E
  #define sig_byte2 0x95
  #define sig_byte3 0x01
  #define PAGESIZE 128      //Size in Bytes
  
  #ifdef _B128
    #error This part does not support 128 word boot block
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*(unsigned int)16384 / PAGESIZE)- (2*256 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES (((2*(unsigned int)16384) / PAGESIZE)- (2*512 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*(unsigned int)16384 / PAGESIZE)- (2*1024 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B2048
    #define APP_PAGES ((2*(unsigned int)16384) / PAGESIZE)- (2*2048 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
#endif

#ifdef _ATMEGA161
  #include "iom161.h"
  #define PARTCODE 0x60

  #define sig_byte1 0x1E
  #define sig_byte2 0x94
  #define sig_byte3 0x01
  #define PAGESIZE 128       //Size in Bytes
  #ifdef _B128
    #error The ATmega161 only supports 512 words boot block
  #endif
  #ifdef _B256 
    #error The ATmega161 only supports 512 words boot block
  #endif
  #ifdef _B512 
    #define APP_PAGES ((2*(unsigned int)8192 / PAGESIZE )- (2*512 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B1024 
    #error The ATmega161 only supports 512 words boot block
  #endif
  #ifdef _B2048 
    #error The ATmega161 only supports 512 words boot block
  #endif
#endif

#ifdef _ATMEGA163
  #include "iom163.h"
  #define PARTCODE 0x66

  #define sig_byte1 0x1E
  #define sig_byte2 0x94
  #define sig_byte3 0x02
  #define PAGESIZE 128      //Size in Bytes
  
  #ifdef _B128
    #error This program does not fit in 128 words, minimum 256 words
  #endif
  #ifdef _B256
    #define APP_PAGES ((2*(unsigned int)8192 / PAGESIZE)- (2*256 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*(unsigned int)8192 / PAGESIZE)- (2*512 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
 #define APP_END 1024
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*(unsigned int)8192 / PAGESIZE)- (2*1024 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif  
  #ifdef _B2048
    #error This part does not support 2048 word boot block
  #endif
#endif

#ifdef _ATMEGA128
  #include "iom128.h"
  #define PARTCODE 0x44

  #define sig_byte1 0x1E
  #define sig_byte2 0x97
  #define sig_byte3 0x02
  #define PAGESIZE 256      //Size in Bytes
  #define LARGE_MEMORY      //Enables the use of RAMPZ for FLASH memory larger than 64K
  
  #ifdef _B128
    #error This program does not fit in 128 words, minimum 512 words
  #endif
  #ifdef _B256
    #error This program does not fit in 256 words, minimum 512 words  
  #endif
  #ifdef _B512
    #define APP_PAGES ((2*(unsigned int)65536 / PAGESIZE)- (2*512 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif
   #ifdef _B1024
    #define APP_PAGES ((2*(unsigned int)65536 / PAGESIZE)- (2*1024 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif  
   #ifdef _B2048
    #define APP_PAGES ((2*(unsigned int)65536 / PAGESIZE)- (2*2048 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif  
   #ifdef _B4096
    #define APP_PAGES ((2*(unsigned int)65536 / PAGESIZE)- (2*4096 / PAGESIZE )) 
    #define APP_END APP_PAGES * PAGESIZE 
  #endif  
#endif

