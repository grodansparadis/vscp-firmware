#line 1 "snmp.c"

#line 44 "snmp.c"
 
#line 46 "snmp.c"

#line 1 "D:/mcc18/h/string.h"

#line 3 "D:/mcc18/h/string.h"


#line 1 "D:/mcc18/h/stddef.h"
 

#line 4 "D:/mcc18/h/stddef.h"

#line 6 "D:/mcc18/h/stddef.h"

typedef unsigned char wchar_t;
typedef unsigned char size_t;    


#line 13 "D:/mcc18/h/stddef.h"
 
typedef signed short int ptrdiff_t;
typedef signed short int ptrdiffram_t;
typedef signed short long int ptrdiffrom_t;


#line 23 "D:/mcc18/h/stddef.h"
 
typedef unsigned short int size_t;
typedef unsigned short int sizeram_t;
typedef unsigned short long int sizerom_t;


#line 37 "D:/mcc18/h/stddef.h"
 
#line 39 "D:/mcc18/h/stddef.h"


#line 44 "D:/mcc18/h/stddef.h"
 
#line 46 "D:/mcc18/h/stddef.h"

#line 48 "D:/mcc18/h/stddef.h"
#line 71 "D:/mcc18/h/stddef.h"

#line 73 "D:/mcc18/h/stddef.h"
#line 5 "D:/mcc18/h/string.h"

#line 7 "D:/mcc18/h/string.h"


#line 20 "D:/mcc18/h/string.h"
 
#line 22 "D:/mcc18/h/string.h"


#line 25 "D:/mcc18/h/string.h"
 
#line 27 "D:/mcc18/h/string.h"

 

#line 39 "D:/mcc18/h/string.h"
 
void *memcpy (auto void *s1, auto const void *s2, auto size_t n);


#line 55 "D:/mcc18/h/string.h"
 
void *memmove (auto void *s1, auto const void *s2, auto size_t n);


#line 67 "D:/mcc18/h/string.h"
 
char *strcpy (auto char *s1, auto const char *s2);


#line 83 "D:/mcc18/h/string.h"
 
char *strncpy (auto char *s1, auto const char *s2, auto size_t n);


#line 97 "D:/mcc18/h/string.h"
 
char *strcat (auto char *s1, auto const char *s2);


#line 113 "D:/mcc18/h/string.h"
 
char *strncat (auto char *s1, auto const char *s2, auto size_t n);


#line 128 "D:/mcc18/h/string.h"
 
signed char memcmp (auto const void *s1, auto const void *s2, auto size_t n);


#line 141 "D:/mcc18/h/string.h"
 
signed char strcmp (auto const char *s1, auto const char *s2);


#line 147 "D:/mcc18/h/string.h"
 


#line 161 "D:/mcc18/h/string.h"
 
signed char strncmp (auto const char *s1, auto const char *s2, auto size_t n);


#line 167 "D:/mcc18/h/string.h"
 


#line 183 "D:/mcc18/h/string.h"
 
void *memchr (auto const void *s, auto unsigned char c, auto size_t n);


#line 199 "D:/mcc18/h/string.h"
 
char *strchr (auto const char *s, auto unsigned char c);


#line 210 "D:/mcc18/h/string.h"
 
size_t strcspn (auto const char *s1, auto const char *s2);


#line 222 "D:/mcc18/h/string.h"
 
char *strpbrk (auto const char *s1, auto const char *s2);


#line 238 "D:/mcc18/h/string.h"
 
char *strrchr (auto const char *s, auto unsigned char c);


#line 249 "D:/mcc18/h/string.h"
 
size_t strspn (auto const char *s1, auto const char *s2);


#line 262 "D:/mcc18/h/string.h"
 
char *strstr (auto const char *s1, auto const char *s2);


#line 305 "D:/mcc18/h/string.h"
 
char *strtok (auto char *s1, auto const char *s2);


#line 321 "D:/mcc18/h/string.h"
 
void *memset (auto void *s, auto unsigned char c, auto size_t n);


#line 339 "D:/mcc18/h/string.h"
 
#line 341 "D:/mcc18/h/string.h"


#line 349 "D:/mcc18/h/string.h"
 
size_t strlen (auto const char *s);


#line 358 "D:/mcc18/h/string.h"
 
char *strupr (auto char *s);


#line 367 "D:/mcc18/h/string.h"
 
char *strlwr (auto char *s);



 

#line 379 "D:/mcc18/h/string.h"
 
far  rom void *memcpypgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 389 "D:/mcc18/h/string.h"
 
void *memcpypgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 398 "D:/mcc18/h/string.h"
 
far  rom void *memcpyram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 407 "D:/mcc18/h/string.h"
 
far  rom void *memmovepgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 417 "D:/mcc18/h/string.h"
 
void *memmovepgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 426 "D:/mcc18/h/string.h"
 
far  rom void *memmoveram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 434 "D:/mcc18/h/string.h"
 
far  rom char *strcpypgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 443 "D:/mcc18/h/string.h"
 
char *strcpypgm2ram (auto char *s1, auto const far  rom char *s2);


#line 451 "D:/mcc18/h/string.h"
 
far  rom char *strcpyram2pgm (auto far  rom char *s1, auto const char *s2);


#line 460 "D:/mcc18/h/string.h"
 
far  rom char *strncpypgm (auto far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 470 "D:/mcc18/h/string.h"
 
char *strncpypgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 479 "D:/mcc18/h/string.h"
 
far  rom char *strncpyram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 487 "D:/mcc18/h/string.h"
 
far  rom char *strcatpgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 496 "D:/mcc18/h/string.h"
 
char *strcatpgm2ram (auto char *s1, auto const far  rom char *s2);


#line 504 "D:/mcc18/h/string.h"
 
far  rom char *strcatram2pgm (auto far  rom char *s1, auto const char *s2);


#line 513 "D:/mcc18/h/string.h"
 
far  rom char *strncatpgm (auto far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 523 "D:/mcc18/h/string.h"
 
char *strncatpgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 532 "D:/mcc18/h/string.h"
 
far  rom char *strncatram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 541 "D:/mcc18/h/string.h"
 
signed char memcmppgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 551 "D:/mcc18/h/string.h"
 
signed char memcmppgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 560 "D:/mcc18/h/string.h"
 
signed char memcmpram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 568 "D:/mcc18/h/string.h"
 
signed char strcmppgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 577 "D:/mcc18/h/string.h"
 
signed char strcmppgm2ram (auto const char *s1, auto const far  rom char *s2);


#line 585 "D:/mcc18/h/string.h"
 
signed char strcmpram2pgm (auto const far  rom char *s1, auto const char *s2);


#line 594 "D:/mcc18/h/string.h"
 
signed char strncmppgm (auto const far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 604 "D:/mcc18/h/string.h"
 
signed char strncmppgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 613 "D:/mcc18/h/string.h"
 
signed char strncmpram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 622 "D:/mcc18/h/string.h"
 
far  rom char *memchrpgm (auto const far  rom char *s, auto const unsigned char c, auto sizerom_t n);


#line 631 "D:/mcc18/h/string.h"
 
far  rom char *strchrpgm (auto const far  rom char *s, auto unsigned char c);


#line 639 "D:/mcc18/h/string.h"
 
sizerom_t strcspnpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 647 "D:/mcc18/h/string.h"
 
sizerom_t strcspnpgmram (auto const far  rom char *s1, auto const char *s2);


#line 655 "D:/mcc18/h/string.h"
 
sizeram_t strcspnrampgm (auto const char *s1, auto const far  rom char *s2);


#line 663 "D:/mcc18/h/string.h"
 
far  rom char *strpbrkpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 671 "D:/mcc18/h/string.h"
 
far  rom char *strpbrkpgmram (auto const far  rom char *s1, auto const char *s2);


#line 679 "D:/mcc18/h/string.h"
 
char *strpbrkrampgm (auto const char *s1, auto const far  rom char *s2);


#line 688 "D:/mcc18/h/string.h"
 
 


#line 696 "D:/mcc18/h/string.h"
 
sizerom_t strspnpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 704 "D:/mcc18/h/string.h"
 
sizerom_t strspnpgmram (auto const far  rom char *s1, auto const char *s2);


#line 712 "D:/mcc18/h/string.h"
 
sizeram_t strspnrampgm (auto const char *s1, auto const far  rom char *s2);


#line 720 "D:/mcc18/h/string.h"
 
far  rom char *strstrpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 729 "D:/mcc18/h/string.h"
 
far  rom char *strstrpgmram (auto const far  rom char *s1, auto const char *s2);


#line 737 "D:/mcc18/h/string.h"
 
char *strstrrampgm (auto const char *s1, auto const far  rom char *s2);


#line 745 "D:/mcc18/h/string.h"
 
far  rom char *strtokpgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 754 "D:/mcc18/h/string.h"
 
char *strtokpgmram (auto char *s1, auto const far  rom char *s2);


#line 762 "D:/mcc18/h/string.h"
 
far  rom char *strtokrampgm (auto far  rom char *s1, auto const char *s2);


#line 771 "D:/mcc18/h/string.h"
 
far  rom void *memsetpgm (auto far  rom void *s, auto unsigned char c, auto sizerom_t n);


#line 778 "D:/mcc18/h/string.h"
 
far  rom char *struprpgm (auto far  rom char *s);


#line 785 "D:/mcc18/h/string.h"
 
far  rom char *strlwrpgm (auto far  rom char *s);


#line 792 "D:/mcc18/h/string.h"
 
sizerom_t strlenpgm (auto const far  rom char *s);

#line 796 "D:/mcc18/h/string.h"

#line 798 "D:/mcc18/h/string.h"

#line 805 "D:/mcc18/h/string.h"
#line 814 "D:/mcc18/h/string.h"

#line 816 "D:/mcc18/h/string.h"
#line 47 "snmp.c"


#line 1 "./snmp.h"

#line 38 "./snmp.h"
 

#line 41 "./snmp.h"

#line 1 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 3 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 7 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 1 "./compiler.h"

#line 48 "./compiler.h"
 


#line 52 "./compiler.h"

#line 54 "./compiler.h"
#line 55 "./compiler.h"
#line 57 "./compiler.h"
#line 59 "./compiler.h"
#line 60 "./compiler.h"
#line 61 "./compiler.h"
#line 62 "./compiler.h"
#line 63 "./compiler.h"
#line 64 "./compiler.h"

#line 66 "./compiler.h"
#line 68 "./compiler.h"

#line 70 "./compiler.h"
#line 72 "./compiler.h"

#line 74 "./compiler.h"
#line 1 "D:/mcc18/h/p18cxxx.h"

#line 3 "D:/mcc18/h/p18cxxx.h"

#line 5 "D:/mcc18/h/p18cxxx.h"
#line 7 "D:/mcc18/h/p18cxxx.h"
#line 9 "D:/mcc18/h/p18cxxx.h"
#line 11 "D:/mcc18/h/p18cxxx.h"
#line 13 "D:/mcc18/h/p18cxxx.h"
#line 15 "D:/mcc18/h/p18cxxx.h"
#line 17 "D:/mcc18/h/p18cxxx.h"
#line 19 "D:/mcc18/h/p18cxxx.h"
#line 21 "D:/mcc18/h/p18cxxx.h"
#line 23 "D:/mcc18/h/p18cxxx.h"
#line 25 "D:/mcc18/h/p18cxxx.h"
#line 27 "D:/mcc18/h/p18cxxx.h"
#line 29 "D:/mcc18/h/p18cxxx.h"
#line 31 "D:/mcc18/h/p18cxxx.h"
#line 33 "D:/mcc18/h/p18cxxx.h"
#line 35 "D:/mcc18/h/p18cxxx.h"
#line 37 "D:/mcc18/h/p18cxxx.h"
#line 39 "D:/mcc18/h/p18cxxx.h"
#line 41 "D:/mcc18/h/p18cxxx.h"
#line 43 "D:/mcc18/h/p18cxxx.h"
#line 45 "D:/mcc18/h/p18cxxx.h"
#line 47 "D:/mcc18/h/p18cxxx.h"
#line 49 "D:/mcc18/h/p18cxxx.h"
#line 51 "D:/mcc18/h/p18cxxx.h"
#line 53 "D:/mcc18/h/p18cxxx.h"
#line 55 "D:/mcc18/h/p18cxxx.h"
#line 57 "D:/mcc18/h/p18cxxx.h"
#line 59 "D:/mcc18/h/p18cxxx.h"
#line 61 "D:/mcc18/h/p18cxxx.h"
#line 63 "D:/mcc18/h/p18cxxx.h"
#line 65 "D:/mcc18/h/p18cxxx.h"
#line 67 "D:/mcc18/h/p18cxxx.h"
#line 69 "D:/mcc18/h/p18cxxx.h"
#line 71 "D:/mcc18/h/p18cxxx.h"
#line 73 "D:/mcc18/h/p18cxxx.h"
#line 75 "D:/mcc18/h/p18cxxx.h"
#line 77 "D:/mcc18/h/p18cxxx.h"
#line 79 "D:/mcc18/h/p18cxxx.h"
#line 81 "D:/mcc18/h/p18cxxx.h"
#line 83 "D:/mcc18/h/p18cxxx.h"
#line 85 "D:/mcc18/h/p18cxxx.h"
#line 87 "D:/mcc18/h/p18cxxx.h"
#line 89 "D:/mcc18/h/p18cxxx.h"
#line 91 "D:/mcc18/h/p18cxxx.h"
#line 93 "D:/mcc18/h/p18cxxx.h"
#line 95 "D:/mcc18/h/p18cxxx.h"
#line 1 "D:/mcc18/h/p18f6621.h"

#line 6 "D:/mcc18/h/p18f6621.h"
 


#line 10 "D:/mcc18/h/p18f6621.h"

extern volatile near unsigned char       ECCP2DEL;
extern volatile near struct {
  unsigned PDC0:1;
  unsigned PDC1:1;
  unsigned PDC2:1;
  unsigned PDC3:1;
  unsigned PDC4:1;
  unsigned PDC5:1;
  unsigned PDC6:1;
  unsigned PRSEN:1;
} ECCP2DELbits;
extern volatile near unsigned char       PWM2CON;
extern volatile near struct {
  unsigned PDC0:1;
  unsigned PDC1:1;
  unsigned PDC2:1;
  unsigned PDC3:1;
  unsigned PDC4:1;
  unsigned PDC5:1;
  unsigned PDC6:1;
  unsigned PRSEN:1;
} PWM2CONbits;
extern volatile near unsigned char       ECCP2AS;
extern volatile near struct {
  unsigned PSSBD0:1;
  unsigned PSSBD1:1;
  unsigned PSSAC0:1;
  unsigned PSSAC1:1;
  unsigned ECCP2AS0:1;
  unsigned ECCP2AS1:1;
  unsigned ECCP2AS2:1;
  unsigned ECCP2ASE:1;
} ECCP2ASbits;
extern volatile near unsigned char       ECCP3DEL;
extern volatile near struct {
  unsigned PDC0:1;
  unsigned PDC1:1;
  unsigned PDC2:1;
  unsigned PDC3:1;
  unsigned PDC4:1;
  unsigned PDC5:1;
  unsigned PDC6:1;
  unsigned PRSEN:1;
} ECCP3DELbits;
extern volatile near unsigned char       PWM3CON;
extern volatile near struct {
  unsigned PDC0:1;
  unsigned PDC1:1;
  unsigned PDC2:1;
  unsigned PDC3:1;
  unsigned PDC4:1;
  unsigned PDC5:1;
  unsigned PDC6:1;
  unsigned PRSEN:1;
} PWM3CONbits;
extern volatile near unsigned char       ECCP3AS;
extern volatile near struct {
  unsigned PSSBD0:1;
  unsigned PSSBD1:1;
  unsigned PSSAC0:1;
  unsigned PSSAC1:1;
  unsigned ECCP3AS0:1;
  unsigned ECCP3AS1:1;
  unsigned ECCP3AS2:1;
  unsigned ECCP3ASE:1;
} ECCP3ASbits;
extern volatile near unsigned char       RCSTA2;
extern volatile near struct {
  unsigned RX9D:1;
  unsigned OERR:1;
  unsigned FERR:1;
  unsigned ADEN:1;
  unsigned CREN:1;
  unsigned SREN:1;
  unsigned RX9:1;
  unsigned SPEN:1;
} RCSTA2bits;
extern volatile near unsigned char       TXSTA2;
extern volatile near struct {
  unsigned TX9D:1;
  unsigned TRMT:1;
  unsigned BRGH:1;
  unsigned :1;
  unsigned SYNC:1;
  unsigned TXEN:1;
  unsigned TX9:1;
  unsigned CSRC:1;
} TXSTA2bits;
extern volatile near unsigned char       TXREG2;
extern volatile near unsigned char       RCREG2;
extern volatile near unsigned char       SPBRG2;
extern volatile near unsigned char       CCP5CON;
extern volatile near struct {
  unsigned CCP5M0:1;
  unsigned CCP5M1:1;
  unsigned CCP5M2:1;
  unsigned CCP5M3:1;
  unsigned CCP5Y:1;
  unsigned CCP5X:1;
} CCP5CONbits;
extern volatile near unsigned            CCPR5;
extern volatile near unsigned char       CCPR5L;
extern volatile near unsigned char       CCPR5H;
extern volatile near unsigned char       CCP4CON;
extern volatile near struct {
  unsigned CCP4M0:1;
  unsigned CCP4M1:1;
  unsigned CCP4M2:1;
  unsigned CCP4M3:1;
  unsigned CCP4Y:1;
  unsigned CCP4X:1;
} CCP4CONbits;
extern volatile near unsigned            CCPR4;
extern volatile near unsigned char       CCPR4L;
extern volatile near unsigned char       CCPR4H;
extern volatile near unsigned char       T4CON;
extern volatile near struct {
  unsigned T4CKPS0:1;
  unsigned T4CKPS1:1;
  unsigned TMR4ON:1;
  unsigned TOUTPS0:1;
  unsigned TOUTPS1:1;
  unsigned TOUTPS2:1;
  unsigned TOUTPS3:1;
} T4CONbits;
extern volatile near unsigned char       PR4;
extern volatile near unsigned char       TMR4;
extern volatile near unsigned char       ECCP1DEL;
extern volatile near struct {
  unsigned PDC0:1;
  unsigned PDC1:1;
  unsigned PDC2:1;
  unsigned PDC3:1;
  unsigned PDC4:1;
  unsigned PDC5:1;
  unsigned PDC6:1;
  unsigned PRSEN:1;
} ECCP1DELbits;
extern volatile near unsigned char       PWM1CON;
extern volatile near struct {
  unsigned PDC0:1;
  unsigned PDC1:1;
  unsigned PDC2:1;
  unsigned PDC3:1;
  unsigned PDC4:1;
  unsigned PDC5:1;
  unsigned PDC6:1;
  unsigned PRSEN:1;
} PWM1CONbits;
extern volatile near unsigned char       BAUDCON2;
extern volatile near struct {
  unsigned ABDEN:1;
  unsigned WUE:1;
  unsigned :1;
  unsigned BRG16:1;
  unsigned SCKP:1;
  unsigned :1;
  unsigned RCMT:1;
} BAUDCON2bits;
extern volatile near unsigned char       BAUDCTL2;
extern volatile near struct {
  unsigned ABDEN:1;
  unsigned WUE:1;
  unsigned :1;
  unsigned BRG16:1;
  unsigned SCKP:1;
  unsigned :1;
  unsigned RCMT:1;
} BAUDCTL2bits;
extern volatile near unsigned char       SPBRGH2;
extern volatile near unsigned char       BAUDCON1;
extern volatile near struct {
  unsigned ABDEN:1;
  unsigned WUE:1;
  unsigned :1;
  unsigned BRG16:1;
  unsigned SCKP:1;
  unsigned :1;
  unsigned RCMT:1;
} BAUDCON1bits;
extern volatile near unsigned char       BAUDCTL1;
extern volatile near struct {
  unsigned ABDEN:1;
  unsigned WUE:1;
  unsigned :1;
  unsigned BRG16:1;
  unsigned SCKP:1;
  unsigned :1;
  unsigned RCMT:1;
} BAUDCTL1bits;
extern volatile near unsigned char       SPBRGH1;
extern volatile near unsigned char       PORTA;
extern volatile near union {
  struct {
    unsigned RA0:1;
    unsigned RA1:1;
    unsigned RA2:1;
    unsigned RA3:1;
    unsigned RA4:1;
    unsigned RA5:1;
    unsigned RA6:1;
  };
  struct {
    unsigned AN0:1;
    unsigned AN1:1;
    unsigned AN2:1;
    unsigned AN3:1;
    unsigned T0CKI:1;
    unsigned AN4:1;
    unsigned OSC2:1;
  };
  struct {
    unsigned :2;
    unsigned VREFM:1;
    unsigned VREFP:1;
    unsigned :1;
    unsigned LVDIN:1;
    unsigned CLKO:1;
  };
} PORTAbits;
extern volatile near unsigned char       PORTB;
extern volatile near union {
  struct {
    unsigned RB0:1;
    unsigned RB1:1;
    unsigned RB2:1;
    unsigned RB3:1;
    unsigned RB4:1;
    unsigned RB5:1;
    unsigned RB6:1;
    unsigned RB7:1;
  };
  struct {
    unsigned INT0:1;
    unsigned INT1:1;
    unsigned INT2:1;
    unsigned INT3:1;
    unsigned KBI0:1;
    unsigned KBI1:1;
    unsigned KBI2:1;
    unsigned KBI3:1;
  };
  struct {
    unsigned :5;
    unsigned PGM:1;
    unsigned PGC:1;
    unsigned PGD:1;
  };
} PORTBbits;
extern volatile near unsigned char       PORTC;
extern volatile near union {
  struct {
    unsigned RC0:1;
    unsigned RC1:1;
    unsigned RC2:1;
    unsigned RC3:1;
    unsigned RC4:1;
    unsigned RC5:1;
    unsigned RC6:1;
    unsigned RC7:1;
  };
  struct {
    unsigned T1OSO:1;
    unsigned T1OSI:1;
    unsigned CCP1:1;
    unsigned SCK:1;
    unsigned SDI:1;
    unsigned SDO:1;
    unsigned TX1:1;
    unsigned RX1:1;
  };
  struct {
    unsigned T13CKI:1;
    unsigned :1;
    unsigned P1A:1;
    unsigned SCL:1;
    unsigned SDA:1;
    unsigned :1;
    unsigned CK1:1;
    unsigned DT1:1;
  };
} PORTCbits;
extern volatile near unsigned char       PORTD;
extern volatile near union {
  struct {
    unsigned RD0:1;
    unsigned RD1:1;
    unsigned RD2:1;
    unsigned RD3:1;
    unsigned RD4:1;
    unsigned RD5:1;
    unsigned RD6:1;
    unsigned RD7:1;
  };
  struct {
    unsigned PSP0:1;
    unsigned PSP1:1;
    unsigned PSP2:1;
    unsigned PSP3:1;
    unsigned PSP4:1;
    unsigned PSP5:1;
    unsigned PSP6:1;
    unsigned PSP7:1;
  };
  struct {
    unsigned AD0:1;
    unsigned AD1:1;
    unsigned AD2:1;
    unsigned AD3:1;
    unsigned AD4:1;
    unsigned AD5:1;
    unsigned AD6:1;
    unsigned AD7:1;
  };
} PORTDbits;
extern volatile near unsigned char       PORTE;
extern volatile near union {
  struct {
    unsigned RE0:1;
    unsigned RE1:1;
    unsigned RE2:1;
    unsigned RE3:1;
    unsigned RE4:1;
    unsigned RE5:1;
    unsigned RE6:1;
    unsigned RE7:1;
  };
  struct {
    unsigned RD:1;
    unsigned WR:1;
    unsigned CS:1;
  };
  struct {
    unsigned P2D:1;
    unsigned P2C:1;
    unsigned P2B:1;
    unsigned P3C:1;
    unsigned P3B:1;
    unsigned P1C:1;
    unsigned P1B:1;
  };
} PORTEbits;
extern volatile near unsigned char       PORTF;
extern volatile near union {
  struct {
    unsigned RF0:1;
    unsigned RF1:1;
    unsigned RF2:1;
    unsigned RF3:1;
    unsigned RF4:1;
    unsigned RF5:1;
    unsigned RF6:1;
    unsigned RF7:1;
  };
  struct {
    unsigned AN5:1;
    unsigned AN6:1;
    unsigned AN7:1;
    unsigned AN8:1;
    unsigned AN9:1;
    unsigned AN10:1;
    unsigned AN11:1;
    unsigned SS:1;
  };
  struct {
    unsigned :1;
    unsigned C2OUTF:1;
    unsigned C1OUTF:1;
    unsigned :2;
    unsigned CVREFF:1;
  };
} PORTFbits;
extern volatile near unsigned char       PORTG;
extern volatile near union {
  struct {
    unsigned RG0:1;
    unsigned RG1:1;
    unsigned RG2:1;
    unsigned RG3:1;
    unsigned RG4:1;
    unsigned RG5:1;
  };
  struct {
    unsigned CCP3:1;
    unsigned TX2:1;
    unsigned RX2:1;
    unsigned CCP4:1;
    unsigned CCP5:1;
    unsigned MCLR:1;
  };
  struct {
    unsigned P3A:1;
    unsigned CK2:1;
    unsigned DT2:1;
    unsigned P3D:1;
    unsigned P1D:1;
    unsigned VPP:1;
  };
} PORTGbits;
extern volatile near unsigned char       LATA;
extern volatile near struct {
  unsigned LATA0:1;
  unsigned LATA1:1;
  unsigned LATA2:1;
  unsigned LATA3:1;
  unsigned LATA4:1;
  unsigned LATA5:1;
  unsigned LATA6:1;
} LATAbits;
extern volatile near unsigned char       LATB;
extern volatile near struct {
  unsigned LATB0:1;
  unsigned LATB1:1;
  unsigned LATB2:1;
  unsigned LATB3:1;
  unsigned LATB4:1;
  unsigned LATB5:1;
  unsigned LATB6:1;
  unsigned LATB7:1;
} LATBbits;
extern volatile near unsigned char       LATC;
extern volatile near struct {
  unsigned LATC0:1;
  unsigned LATC1:1;
  unsigned LATC2:1;
  unsigned LATC3:1;
  unsigned LATC4:1;
  unsigned LATC5:1;
  unsigned LATC6:1;
  unsigned LATC7:1;
} LATCbits;
extern volatile near unsigned char       LATD;
extern volatile near struct {
  unsigned LATD0:1;
  unsigned LATD1:1;
  unsigned LATD2:1;
  unsigned LATD3:1;
  unsigned LATD4:1;
  unsigned LATD5:1;
  unsigned LATD6:1;
  unsigned LATD7:1;
} LATDbits;
extern volatile near unsigned char       LATE;
extern volatile near struct {
  unsigned LATE0:1;
  unsigned LATE1:1;
  unsigned LATE2:1;
  unsigned LATE3:1;
  unsigned LATE4:1;
  unsigned LATE5:1;
  unsigned LATE6:1;
  unsigned LATE7:1;
} LATEbits;
extern volatile near unsigned char       LATF;
extern volatile near struct {
  unsigned LATF0:1;
  unsigned LATF1:1;
  unsigned LATF2:1;
  unsigned LATF3:1;
  unsigned LATF4:1;
  unsigned LATF5:1;
  unsigned LATF6:1;
  unsigned LATF7:1;
} LATFbits;
extern volatile near unsigned char       LATG;
extern volatile near struct {
  unsigned LATG0:1;
  unsigned LATG1:1;
  unsigned LATG2:1;
  unsigned LATG3:1;
  unsigned LATG4:1;
  unsigned LATG5:1;
} LATGbits;
extern volatile near unsigned char       DDRA;
extern volatile near struct {
  unsigned RA0:1;
  unsigned RA1:1;
  unsigned RA2:1;
  unsigned RA3:1;
  unsigned RA4:1;
  unsigned RA5:1;
  unsigned RA6:1;
} DDRAbits;
extern volatile near unsigned char       TRISA;
extern volatile near struct {
  unsigned TRISA0:1;
  unsigned TRISA1:1;
  unsigned TRISA2:1;
  unsigned TRISA3:1;
  unsigned TRISA4:1;
  unsigned TRISA5:1;
  unsigned TRISA6:1;
} TRISAbits;
extern volatile near unsigned char       DDRB;
extern volatile near struct {
  unsigned RB0:1;
  unsigned RB1:1;
  unsigned RB2:1;
  unsigned RB3:1;
  unsigned RB4:1;
  unsigned RB5:1;
  unsigned RB6:1;
  unsigned RB7:1;
} DDRBbits;
extern volatile near unsigned char       TRISB;
extern volatile near struct {
  unsigned TRISB0:1;
  unsigned TRISB1:1;
  unsigned TRISB2:1;
  unsigned TRISB3:1;
  unsigned TRISB4:1;
  unsigned TRISB5:1;
  unsigned TRISB6:1;
  unsigned TRISB7:1;
} TRISBbits;
extern volatile near unsigned char       DDRC;
extern volatile near struct {
  unsigned RC0:1;
  unsigned RC1:1;
  unsigned RC2:1;
  unsigned RC3:1;
  unsigned RC4:1;
  unsigned RC5:1;
  unsigned RC6:1;
  unsigned RC7:1;
} DDRCbits;
extern volatile near unsigned char       TRISC;
extern volatile near struct {
  unsigned TRISC0:1;
  unsigned TRISC1:1;
  unsigned TRISC2:1;
  unsigned TRISC3:1;
  unsigned TRISC4:1;
  unsigned TRISC5:1;
  unsigned TRISC6:1;
  unsigned TRISC7:1;
} TRISCbits;
extern volatile near unsigned char       DDRD;
extern volatile near struct {
  unsigned RD0:1;
  unsigned RD1:1;
  unsigned RD2:1;
  unsigned RD3:1;
  unsigned RD4:1;
  unsigned RD5:1;
  unsigned RD6:1;
  unsigned RD7:1;
} DDRDbits;
extern volatile near unsigned char       TRISD;
extern volatile near struct {
  unsigned TRISD0:1;
  unsigned TRISD1:1;
  unsigned TRISD2:1;
  unsigned TRISD3:1;
  unsigned TRISD4:1;
  unsigned TRISD5:1;
  unsigned TRISD6:1;
  unsigned TRISD7:1;
} TRISDbits;
extern volatile near unsigned char       DDRE;
extern volatile near struct {
  unsigned RE0:1;
  unsigned RE1:1;
  unsigned RE2:1;
  unsigned RE3:1;
  unsigned RE4:1;
  unsigned RE5:1;
  unsigned RE6:1;
  unsigned RE7:1;
} DDREbits;
extern volatile near unsigned char       TRISE;
extern volatile near struct {
  unsigned TRISE0:1;
  unsigned TRISE1:1;
  unsigned TRISE2:1;
  unsigned TRISE3:1;
  unsigned TRISE4:1;
  unsigned TRISE5:1;
  unsigned TRISE6:1;
  unsigned TRISE7:1;
} TRISEbits;
extern volatile near unsigned char       DDRF;
extern volatile near struct {
  unsigned RF0:1;
  unsigned RF1:1;
  unsigned RF2:1;
  unsigned RF3:1;
  unsigned RF4:1;
  unsigned RF5:1;
  unsigned RF6:1;
  unsigned RF7:1;
} DDRFbits;
extern volatile near unsigned char       TRISF;
extern volatile near struct {
  unsigned TRISF0:1;
  unsigned TRISF1:1;
  unsigned TRISF2:1;
  unsigned TRISF3:1;
  unsigned TRISF4:1;
  unsigned TRISF5:1;
  unsigned TRISF6:1;
  unsigned TRISF7:1;
} TRISFbits;
extern volatile near unsigned char       DDRG;
extern volatile near struct {
  unsigned RG0:1;
  unsigned RG1:1;
  unsigned RG2:1;
  unsigned RG3:1;
  unsigned RG4:1;
  unsigned RG5:1;
} DDRGbits;
extern volatile near unsigned char       TRISG;
extern volatile near struct {
  unsigned TRISG0:1;
  unsigned TRISG1:1;
  unsigned TRISG2:1;
  unsigned TRISG3:1;
  unsigned TRISG4:1;
  unsigned TRISG5:1;
} TRISGbits;
extern volatile near unsigned char       PIE1;
extern volatile near struct {
  unsigned TMR1IE:1;
  unsigned TMR2IE:1;
  unsigned CCP1IE:1;
  unsigned SSPIE:1;
  unsigned TXIE:1;
  unsigned RCIE:1;
  unsigned ADIE:1;
  unsigned PSPIE:1;
} PIE1bits;
extern volatile near unsigned char       PIR1;
extern volatile near struct {
  unsigned TMR1IF:1;
  unsigned TMR2IF:1;
  unsigned CCP1IF:1;
  unsigned SSPIF:1;
  unsigned TXIF:1;
  unsigned RCIF:1;
  unsigned ADIF:1;
  unsigned PSPIF:1;
} PIR1bits;
extern volatile near unsigned char       IPR1;
extern volatile near struct {
  unsigned TMR1IP:1;
  unsigned TMR2IP:1;
  unsigned CCP1IP:1;
  unsigned SSPIP:1;
  unsigned TXIP:1;
  unsigned RCIP:1;
  unsigned ADIP:1;
  unsigned PSPIP:1;
} IPR1bits;
extern volatile near unsigned char       PIE2;
extern volatile near struct {
  unsigned CCP2IE:1;
  unsigned TMR3IE:1;
  unsigned LVDIE:1;
  unsigned BCLIE:1;
  unsigned EEIE:1;
  unsigned :1;
  unsigned CMIE:1;
} PIE2bits;
extern volatile near unsigned char       PIR2;
extern volatile near struct {
  unsigned CCP2IF:1;
  unsigned TMR3IF:1;
  unsigned LVDIF:1;
  unsigned BCLIF:1;
  unsigned EEIF:1;
  unsigned :1;
  unsigned CMIF:1;
} PIR2bits;
extern volatile near unsigned char       IPR2;
extern volatile near struct {
  unsigned CCP2IP:1;
  unsigned TMR3IP:1;
  unsigned LVDIP:1;
  unsigned BCLIP:1;
  unsigned EEIP:1;
  unsigned :1;
  unsigned CMIP:1;
} IPR2bits;
extern volatile near unsigned char       PIE3;
extern volatile near struct {
  unsigned CCP3IE:1;
  unsigned CCP4IE:1;
  unsigned CCP5IE:1;
  unsigned TMR4IE:1;
  unsigned TX2IE:1;
  unsigned RC2IE:1;
} PIE3bits;
extern volatile near unsigned char       PIR3;
extern volatile near struct {
  unsigned CCP3IF:1;
  unsigned CCP4IF:1;
  unsigned CCP5IF:1;
  unsigned TMR4IF:1;
  unsigned TX2IF:1;
  unsigned RC2IF:1;
} PIR3bits;
extern volatile near unsigned char       IPR3;
extern volatile near struct {
  unsigned CCP3IP:1;
  unsigned CCP4IP:1;
  unsigned CCP5IP:1;
  unsigned TMR4IP:1;
  unsigned TX2IP:1;
  unsigned RC2IP:1;
} IPR3bits;
extern volatile near unsigned char       EECON1;
extern volatile near struct {
  unsigned RD:1;
  unsigned WR:1;
  unsigned WREN:1;
  unsigned WRERR:1;
  unsigned FREE:1;
  unsigned :1;
  unsigned CFGS:1;
  unsigned EEPGD:1;
} EECON1bits;
extern volatile near unsigned char       EECON2;
extern volatile near unsigned char       EEDATA;
extern volatile near unsigned char       EEADR;
extern volatile near unsigned char       EEADRH;
extern volatile near unsigned char       RCSTA;
extern volatile near struct {
  unsigned RX9D:1;
  unsigned OERR:1;
  unsigned FERR:1;
  unsigned ADEN:1;
  unsigned CREN:1;
  unsigned SREN:1;
  unsigned RX9:1;
  unsigned SPEN:1;
} RCSTAbits;
extern volatile near unsigned char       RCSTA1;
extern volatile near struct {
  unsigned RX9D:1;
  unsigned OERR:1;
  unsigned FERR:1;
  unsigned ADEN:1;
  unsigned CREN:1;
  unsigned SREN:1;
  unsigned RX9:1;
  unsigned SPEN:1;
} RCSTA1bits;
extern volatile near unsigned char       TXSTA;
extern volatile near struct {
  unsigned TX9D:1;
  unsigned TRMT:1;
  unsigned BRGH:1;
  unsigned :1;
  unsigned SYNC:1;
  unsigned TXEN:1;
  unsigned TX9:1;
  unsigned CSRC:1;
} TXSTAbits;
extern volatile near unsigned char       TXSTA1;
extern volatile near struct {
  unsigned TX9D:1;
  unsigned TRMT:1;
  unsigned BRGH:1;
  unsigned :1;
  unsigned SYNC:1;
  unsigned TXEN:1;
  unsigned TX9:1;
  unsigned CSRC:1;
} TXSTA1bits;
extern volatile near unsigned char       TXREG;
extern volatile near unsigned char       TXREG1;
extern volatile near unsigned char       RCREG;
extern volatile near unsigned char       RCREG1;
extern volatile near unsigned char       SPBRG;
extern volatile near unsigned char       SPBRG1;
extern volatile near unsigned char       PSPCON;
extern volatile near struct {
  unsigned :4;
  unsigned PSPMODE:1;
  unsigned IBOV:1;
  unsigned OBF:1;
  unsigned IBF:1;
} PSPCONbits;
extern volatile near unsigned char       T3CON;
extern volatile near union {
  struct {
    unsigned TMR3ON:1;
    unsigned TMR3CS:1;
    unsigned T3SYNC:1;
    unsigned T3CCP1:1;
    unsigned T3CKPS0:1;
    unsigned T3CKPS1:1;
    unsigned T3CCP2:1;
    unsigned RD16:1;
  };
  struct {
    unsigned :2;
    unsigned T3NSYNC:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_T3SYNC:1;
  };
} T3CONbits;
extern volatile near unsigned char       TMR3L;
extern volatile near unsigned char       TMR3H;
extern volatile near unsigned char       CMCON;
extern volatile near struct {
  unsigned CM0:1;
  unsigned CM1:1;
  unsigned CM2:1;
  unsigned CIS:1;
  unsigned C1INV:1;
  unsigned C2INV:1;
  unsigned C1OUT:1;
  unsigned C2OUT:1;
} CMCONbits;
extern volatile near unsigned char       CVRCON;
extern volatile near struct {
  unsigned CVR0:1;
  unsigned CVR1:1;
  unsigned CVR2:1;
  unsigned CVR3:1;
  unsigned CVREF:1;
  unsigned CVRR:1;
  unsigned CVROE:1;
  unsigned CVREN:1;
} CVRCONbits;
extern volatile near unsigned char       ECCP1AS;
extern volatile near struct {
  unsigned PSSBD0:1;
  unsigned PSSBD1:1;
  unsigned PSSAC0:1;
  unsigned PSSAC1:1;
  unsigned ECCP1AS0:1;
  unsigned ECCP1AS1:1;
  unsigned ECCP1AS2:1;
  unsigned ECCP1ASE:1;
} ECCP1ASbits;
extern volatile near unsigned char       CCP3CON;
extern volatile near struct {
  unsigned CCP3M0:1;
  unsigned CCP3M1:1;
  unsigned CCP3M2:1;
  unsigned CCP3M3:1;
  unsigned CCP3Y:1;
  unsigned CCP3X:1;
  unsigned P3M0:1;
  unsigned P3M1:1;
} CCP3CONbits;
extern volatile near unsigned            CCPR3;
extern volatile near unsigned char       CCPR3L;
extern volatile near unsigned char       CCPR3H;
extern volatile near unsigned char       CCP2CON;
extern volatile near struct {
  unsigned CCP2M0:1;
  unsigned CCP2M1:1;
  unsigned CCP2M2:1;
  unsigned CCP2M3:1;
  unsigned CCP2Y:1;
  unsigned CCP2X:1;
  unsigned P2M0:1;
  unsigned P2M1:1;
} CCP2CONbits;
extern volatile near unsigned            CCPR2;
extern volatile near unsigned char       CCPR2L;
extern volatile near unsigned char       CCPR2H;
extern volatile near unsigned char       CCP1CON;
extern volatile near struct {
  unsigned CCP1M0:1;
  unsigned CCP1M1:1;
  unsigned CCP1M2:1;
  unsigned CCP1M3:1;
  unsigned CCP1Y:1;
  unsigned CCP1X:1;
  unsigned P1M0:1;
  unsigned P1M1:1;
} CCP1CONbits;
extern volatile near unsigned            CCPR1;
extern volatile near unsigned char       CCPR1L;
extern volatile near unsigned char       CCPR1H;
extern volatile near unsigned char       ADCON2;
extern volatile near struct {
  unsigned ADCS0:1;
  unsigned ADCS1:1;
  unsigned ADCS2:1;
  unsigned ACQT0:1;
  unsigned ACQT1:1;
  unsigned ACQT2:1;
  unsigned :1;
  unsigned ADFM:1;
} ADCON2bits;
extern volatile near unsigned char       ADCON1;
extern volatile near struct {
  unsigned PCFG0:1;
  unsigned PCFG1:1;
  unsigned PCFG2:1;
  unsigned PCFG3:1;
  unsigned VCFG0:1;
  unsigned VCFG1:1;
} ADCON1bits;
extern volatile near unsigned char       ADCON0;
extern volatile near union {
  struct {
    unsigned ADON:1;
    unsigned GO_DONE:1;
    unsigned CHS0:1;
    unsigned CHS1:1;
    unsigned CHS2:1;
    unsigned CHS3:1;
  };
  struct {
    unsigned :1;
    unsigned DONE:1;
  };
  struct {
    unsigned :1;
    unsigned GO:1;
  };
  struct {
    unsigned :1;
    unsigned NOT_DONE:1;
  };
} ADCON0bits;
extern volatile near unsigned            ADRES;
extern volatile near unsigned char       ADRESL;
extern volatile near unsigned char       ADRESH;
extern volatile near unsigned char       SSPCON2;
extern volatile near struct {
  unsigned SEN:1;
  unsigned RSEN:1;
  unsigned PEN:1;
  unsigned RCEN:1;
  unsigned ACKEN:1;
  unsigned ACKDT:1;
  unsigned ACKSTAT:1;
  unsigned GCEN:1;
} SSPCON2bits;
extern volatile near unsigned char       SSPCON1;
extern volatile near struct {
  unsigned SSPM0:1;
  unsigned SSPM1:1;
  unsigned SSPM2:1;
  unsigned SSPM3:1;
  unsigned CKP:1;
  unsigned SSPEN:1;
  unsigned SSPOV:1;
  unsigned WCOL:1;
} SSPCON1bits;
extern volatile near unsigned char       SSPSTAT;
extern volatile near union {
  struct {
    unsigned BF:1;
    unsigned UA:1;
    unsigned R_W:1;
    unsigned S:1;
    unsigned P:1;
    unsigned D_A:1;
    unsigned CKE:1;
    unsigned SMP:1;
  };
  struct {
    unsigned :2;
    unsigned I2C_READ:1;
    unsigned I2C_START:1;
    unsigned I2C_STOP:1;
    unsigned I2C_DAT:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_W:1;
    unsigned :2;
    unsigned NOT_A:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_WRITE:1;
    unsigned :2;
    unsigned NOT_ADDRESS:1;
  };
  struct {
    unsigned :2;
    unsigned READ_WRITE:1;
    unsigned :2;
    unsigned DATA_ADDRESS:1;
  };
  struct {
    unsigned :2;
    unsigned R:1;
    unsigned :2;
    unsigned D:1;
  };
} SSPSTATbits;
extern volatile near unsigned char       SSPADD;
extern volatile near unsigned char       SSPBUF;
extern volatile near unsigned char       T2CON;
extern volatile near struct {
  unsigned T2CKPS0:1;
  unsigned T2CKPS1:1;
  unsigned TMR2ON:1;
  unsigned T2OUTPS0:1;
  unsigned T2OUTPS1:1;
  unsigned T2OUTPS2:1;
  unsigned T2OUTPS3:1;
} T2CONbits;
extern volatile near unsigned char       PR2;
extern volatile near unsigned char       TMR2;
extern volatile near unsigned char       T1CON;
extern volatile near union {
  struct {
    unsigned TMR1ON:1;
    unsigned TMR1CS:1;
    unsigned T1SYNC:1;
    unsigned T1OSCEN:1;
    unsigned T1CKPS0:1;
    unsigned T1CKPS1:1;
    unsigned :1;
    unsigned RD16:1;
  };
  struct {
    unsigned :2;
    unsigned T1INSYNC:1;
  };
  struct {
    unsigned :2;
    unsigned NOT_T1SYNC:1;
  };
} T1CONbits;
extern volatile near unsigned char       TMR1L;
extern volatile near unsigned char       TMR1H;
extern volatile near unsigned char       RCON;
extern volatile near union {
  struct {
    unsigned NOT_BOR:1;
    unsigned NOT_POR:1;
    unsigned NOT_PD:1;
    unsigned NOT_TO:1;
    unsigned NOT_RI:1;
    unsigned :1;
    unsigned NOT_LWRT:1;
    unsigned NOT_IPEN:1;
  };
  struct {
    unsigned BOR:1;
    unsigned POR:1;
    unsigned PD:1;
    unsigned TO:1;
    unsigned RI:1;
    unsigned :1;
    unsigned LWRT:1;
    unsigned IPEN:1;
  };
} RCONbits;
extern volatile near unsigned char       WDTCON;
extern volatile near union {
  struct {
    unsigned SWDTEN:1;
  };
  struct {
    unsigned SWDTE:1;
  };
} WDTCONbits;
extern volatile near unsigned char       LVDCON;
extern volatile near union {
  struct {
    unsigned LVDL0:1;
    unsigned LVDL1:1;
    unsigned LVDL2:1;
    unsigned LVDL3:1;
    unsigned LVDEN:1;
    unsigned IRVST:1;
  };
  struct {
    unsigned LVV0:1;
    unsigned LVV1:1;
    unsigned LVV2:1;
    unsigned LVV3:1;
    unsigned :1;
    unsigned BGST:1;
  };
} LVDCONbits;
extern volatile near unsigned char       OSCCON;
extern volatile near struct {
  unsigned SCS0:1;
  unsigned SCS1:1;
  unsigned PLLEN:1;
  unsigned LOCK:1;
} OSCCONbits;
extern volatile near unsigned char       T0CON;
extern volatile near struct {
  unsigned T0PS0:1;
  unsigned T0PS1:1;
  unsigned T0PS2:1;
  unsigned PSA:1;
  unsigned T0SE:1;
  unsigned T0CS:1;
  unsigned T08BIT:1;
  unsigned TMR0ON:1;
} T0CONbits;
extern volatile near unsigned char       TMR0L;
extern volatile near unsigned char       TMR0H;
extern          near unsigned char       STATUS;
extern          near struct {
  unsigned C:1;
  unsigned DC:1;
  unsigned Z:1;
  unsigned OV:1;
  unsigned N:1;
} STATUSbits;
extern          near unsigned            FSR2;
extern          near unsigned char       FSR2L;
extern          near unsigned char       FSR2H;
extern volatile near unsigned char       PLUSW2;
extern volatile near unsigned char       PREINC2;
extern volatile near unsigned char       POSTDEC2;
extern volatile near unsigned char       POSTINC2;
extern          near unsigned char       INDF2;
extern          near unsigned char       BSR;
extern          near unsigned            FSR1;
extern          near unsigned char       FSR1L;
extern          near unsigned char       FSR1H;
extern volatile near unsigned char       PLUSW1;
extern volatile near unsigned char       PREINC1;
extern volatile near unsigned char       POSTDEC1;
extern volatile near unsigned char       POSTINC1;
extern          near unsigned char       INDF1;
extern          near unsigned char       WREG;
extern          near unsigned            FSR0;
extern          near unsigned char       FSR0L;
extern          near unsigned char       FSR0H;
extern volatile near unsigned char       PLUSW0;
extern volatile near unsigned char       PREINC0;
extern volatile near unsigned char       POSTDEC0;
extern volatile near unsigned char       POSTINC0;
extern          near unsigned char       INDF0;
extern volatile near unsigned char       INTCON3;
extern volatile near union {
  struct {
    unsigned INT1IF:1;
    unsigned INT2IF:1;
    unsigned INT3IF:1;
    unsigned INT1IE:1;
    unsigned INT2IE:1;
    unsigned INT3IE:1;
    unsigned INT1IP:1;
    unsigned INT2IP:1;
  };
  struct {
    unsigned INT1F:1;
    unsigned INT2F:1;
    unsigned INT3F:1;
    unsigned INT1E:1;
    unsigned INT2E:1;
    unsigned INT3E:1;
    unsigned INT1P:1;
    unsigned INT2P:1;
  };
} INTCON3bits;
extern volatile near unsigned char       INTCON2;
extern volatile near union {
  struct {
    unsigned RBIP:1;
    unsigned INT3IP:1;
    unsigned TMR0IP:1;
    unsigned INTEDG3:1;
    unsigned INTEDG2:1;
    unsigned INTEDG1:1;
    unsigned INTEDG0:1;
    unsigned NOT_RBPU:1;
  };
  struct {
    unsigned :1;
    unsigned INT3P:1;
    unsigned T0IP:1;
    unsigned :4;
    unsigned RBPU:1;
  };
} INTCON2bits;
extern volatile near unsigned char       INTCON;
extern volatile near union {
  struct {
    unsigned RBIF:1;
    unsigned INT0IF:1;
    unsigned TMR0IF:1;
    unsigned RBIE:1;
    unsigned INT0IE:1;
    unsigned TMR0IE:1;
    unsigned PEIE:1;
    unsigned GIE:1;
  };
  struct {
    unsigned :1;
    unsigned INT0F:1;
    unsigned T0IF:1;
    unsigned :1;
    unsigned INT0E:1;
    unsigned T0IE:1;
    unsigned GIEL:1;
    unsigned GIEH:1;
  };
} INTCONbits;
extern          near unsigned            PROD;
extern          near unsigned char       PRODL;
extern          near unsigned char       PRODH;
extern volatile near unsigned char       TABLAT;
extern volatile near unsigned short long TBLPTR;
extern volatile near unsigned char       TBLPTRL;
extern volatile near unsigned char       TBLPTRH;
extern volatile near unsigned char       TBLPTRU;
extern volatile near unsigned short long PC;
extern volatile near unsigned char       PCL;
extern volatile near unsigned char       PCLATH;
extern volatile near unsigned char       PCLATU;
extern volatile near unsigned char       STKPTR;
extern volatile near struct {
  unsigned STKPTR0:1;
  unsigned STKPTR1:1;
  unsigned STKPTR2:1;
  unsigned STKPTR3:1;
  unsigned STKPTR4:1;
  unsigned :1;
  unsigned STKUNF:1;
  unsigned STKOVF:1;
} STKPTRbits;
extern          near unsigned short long TOS;
extern          near unsigned char       TOSL;
extern          near unsigned char       TOSH;
extern          near unsigned char       TOSU;



#line 1244 "D:/mcc18/h/p18f6621.h"
 
#line 1246 "D:/mcc18/h/p18f6621.h"
#line 1247 "D:/mcc18/h/p18f6621.h"


#line 1250 "D:/mcc18/h/p18f6621.h"
 
#line 1252 "D:/mcc18/h/p18f6621.h"
#line 1253 "D:/mcc18/h/p18f6621.h"
#line 1254 "D:/mcc18/h/p18f6621.h"
#line 1255 "D:/mcc18/h/p18f6621.h"

#line 1257 "D:/mcc18/h/p18f6621.h"
#line 1258 "D:/mcc18/h/p18f6621.h"
#line 1259 "D:/mcc18/h/p18f6621.h"
#line 1260 "D:/mcc18/h/p18f6621.h"
#line 1261 "D:/mcc18/h/p18f6621.h"


#line 1265 "D:/mcc18/h/p18f6621.h"
 
#line 1267 "D:/mcc18/h/p18f6621.h"


#line 1277 "D:/mcc18/h/p18f6621.h"
 

#line 1304 "D:/mcc18/h/p18f6621.h"
#line 1305 "D:/mcc18/h/p18f6621.h"


#line 1308 "D:/mcc18/h/p18f6621.h"
 
#line 1310 "D:/mcc18/h/p18f6621.h"
#line 1311 "D:/mcc18/h/p18f6621.h"
#line 1312 "D:/mcc18/h/p18f6621.h"
#line 1313 "D:/mcc18/h/p18f6621.h"
#line 1314 "D:/mcc18/h/p18f6621.h"
#line 1315 "D:/mcc18/h/p18f6621.h"
#line 1316 "D:/mcc18/h/p18f6621.h"
#line 1317 "D:/mcc18/h/p18f6621.h"
#line 1318 "D:/mcc18/h/p18f6621.h"
#line 1319 "D:/mcc18/h/p18f6621.h"
#line 1320 "D:/mcc18/h/p18f6621.h"
#line 1321 "D:/mcc18/h/p18f6621.h"

#line 1323 "D:/mcc18/h/p18f6621.h"
#line 1324 "D:/mcc18/h/p18f6621.h"


#line 1327 "D:/mcc18/h/p18f6621.h"
 
#line 1329 "D:/mcc18/h/p18f6621.h"
#line 1330 "D:/mcc18/h/p18f6621.h"
#line 1331 "D:/mcc18/h/p18f6621.h"

#line 1333 "D:/mcc18/h/p18f6621.h"
#line 1334 "D:/mcc18/h/p18f6621.h"

#line 1336 "D:/mcc18/h/p18f6621.h"
#line 1337 "D:/mcc18/h/p18f6621.h"
#line 1338 "D:/mcc18/h/p18f6621.h"
#line 1339 "D:/mcc18/h/p18f6621.h"


#line 1342 "D:/mcc18/h/p18f6621.h"
 
#line 1344 "D:/mcc18/h/p18f6621.h"
#line 1345 "D:/mcc18/h/p18f6621.h"
#line 1346 "D:/mcc18/h/p18f6621.h"

#line 1348 "D:/mcc18/h/p18f6621.h"
#line 1349 "D:/mcc18/h/p18f6621.h"
#line 1350 "D:/mcc18/h/p18f6621.h"
#line 1351 "D:/mcc18/h/p18f6621.h"
#line 1352 "D:/mcc18/h/p18f6621.h"
#line 1353 "D:/mcc18/h/p18f6621.h"
#line 1354 "D:/mcc18/h/p18f6621.h"
#line 1355 "D:/mcc18/h/p18f6621.h"
#line 1356 "D:/mcc18/h/p18f6621.h"
#line 1357 "D:/mcc18/h/p18f6621.h"
#line 1358 "D:/mcc18/h/p18f6621.h"
#line 1359 "D:/mcc18/h/p18f6621.h"
#line 1360 "D:/mcc18/h/p18f6621.h"
#line 1361 "D:/mcc18/h/p18f6621.h"
#line 1362 "D:/mcc18/h/p18f6621.h"
#line 1363 "D:/mcc18/h/p18f6621.h"


#line 1366 "D:/mcc18/h/p18f6621.h"
 
#line 1368 "D:/mcc18/h/p18f6621.h"
#line 1369 "D:/mcc18/h/p18f6621.h"
#line 1370 "D:/mcc18/h/p18f6621.h"

#line 1372 "D:/mcc18/h/p18f6621.h"
#line 1373 "D:/mcc18/h/p18f6621.h"

#line 1375 "D:/mcc18/h/p18f6621.h"
#line 1376 "D:/mcc18/h/p18f6621.h"


#line 1379 "D:/mcc18/h/p18f6621.h"
 
#line 1381 "D:/mcc18/h/p18f6621.h"
#line 1382 "D:/mcc18/h/p18f6621.h"
#line 1383 "D:/mcc18/h/p18f6621.h"

#line 1385 "D:/mcc18/h/p18f6621.h"
#line 1386 "D:/mcc18/h/p18f6621.h"

#line 1388 "D:/mcc18/h/p18f6621.h"
#line 1389 "D:/mcc18/h/p18f6621.h"


#line 1392 "D:/mcc18/h/p18f6621.h"
 
#line 1394 "D:/mcc18/h/p18f6621.h"
#line 1395 "D:/mcc18/h/p18f6621.h"
#line 1396 "D:/mcc18/h/p18f6621.h"

#line 1398 "D:/mcc18/h/p18f6621.h"
#line 1399 "D:/mcc18/h/p18f6621.h"

#line 1401 "D:/mcc18/h/p18f6621.h"
#line 1402 "D:/mcc18/h/p18f6621.h"

#line 1404 "D:/mcc18/h/p18f6621.h"
#line 1405 "D:/mcc18/h/p18f6621.h"


#line 1408 "D:/mcc18/h/p18f6621.h"
 
#line 1410 "D:/mcc18/h/p18f6621.h"
#line 1411 "D:/mcc18/h/p18f6621.h"
#line 1412 "D:/mcc18/h/p18f6621.h"

#line 1414 "D:/mcc18/h/p18f6621.h"
#line 1415 "D:/mcc18/h/p18f6621.h"


#line 1418 "D:/mcc18/h/p18f6621.h"
 
#line 1420 "D:/mcc18/h/p18f6621.h"
#line 1421 "D:/mcc18/h/p18f6621.h"
#line 1422 "D:/mcc18/h/p18f6621.h"

#line 1424 "D:/mcc18/h/p18f6621.h"
#line 1425 "D:/mcc18/h/p18f6621.h"

#line 1427 "D:/mcc18/h/p18f6621.h"
#line 1428 "D:/mcc18/h/p18f6621.h"

#line 1430 "D:/mcc18/h/p18f6621.h"
#line 1431 "D:/mcc18/h/p18f6621.h"


#line 1434 "D:/mcc18/h/p18f6621.h"
 
#line 1436 "D:/mcc18/h/p18f6621.h"
#line 1437 "D:/mcc18/h/p18f6621.h"
#line 1438 "D:/mcc18/h/p18f6621.h"

#line 1440 "D:/mcc18/h/p18f6621.h"
#line 1441 "D:/mcc18/h/p18f6621.h"

#line 1443 "D:/mcc18/h/p18f6621.h"
#line 1444 "D:/mcc18/h/p18f6621.h"


#line 1447 "D:/mcc18/h/p18f6621.h"
 
#line 1449 "D:/mcc18/h/p18f6621.h"
#line 1450 "D:/mcc18/h/p18f6621.h"
#line 1451 "D:/mcc18/h/p18f6621.h"

#line 1453 "D:/mcc18/h/p18f6621.h"
#line 1454 "D:/mcc18/h/p18f6621.h"

#line 1456 "D:/mcc18/h/p18f6621.h"
#line 1457 "D:/mcc18/h/p18f6621.h"

#line 1459 "D:/mcc18/h/p18f6621.h"
#line 1460 "D:/mcc18/h/p18f6621.h"


#line 1463 "D:/mcc18/h/p18f6621.h"
 
#line 1465 "D:/mcc18/h/p18f6621.h"
#line 1466 "D:/mcc18/h/p18f6621.h"
#line 1467 "D:/mcc18/h/p18f6621.h"


#line 1470 "D:/mcc18/h/p18f6621.h"
#line 95 "D:/mcc18/h/p18cxxx.h"

#line 97 "D:/mcc18/h/p18cxxx.h"
#line 99 "D:/mcc18/h/p18cxxx.h"
#line 101 "D:/mcc18/h/p18cxxx.h"
#line 103 "D:/mcc18/h/p18cxxx.h"
#line 105 "D:/mcc18/h/p18cxxx.h"
#line 107 "D:/mcc18/h/p18cxxx.h"
#line 109 "D:/mcc18/h/p18cxxx.h"
#line 111 "D:/mcc18/h/p18cxxx.h"
#line 113 "D:/mcc18/h/p18cxxx.h"
#line 115 "D:/mcc18/h/p18cxxx.h"
#line 117 "D:/mcc18/h/p18cxxx.h"
#line 119 "D:/mcc18/h/p18cxxx.h"
#line 121 "D:/mcc18/h/p18cxxx.h"
#line 123 "D:/mcc18/h/p18cxxx.h"
#line 125 "D:/mcc18/h/p18cxxx.h"
#line 127 "D:/mcc18/h/p18cxxx.h"
#line 129 "D:/mcc18/h/p18cxxx.h"
#line 131 "D:/mcc18/h/p18cxxx.h"
#line 133 "D:/mcc18/h/p18cxxx.h"
#line 135 "D:/mcc18/h/p18cxxx.h"
#line 137 "D:/mcc18/h/p18cxxx.h"
#line 139 "D:/mcc18/h/p18cxxx.h"
#line 141 "D:/mcc18/h/p18cxxx.h"
#line 143 "D:/mcc18/h/p18cxxx.h"
#line 145 "D:/mcc18/h/p18cxxx.h"
#line 147 "D:/mcc18/h/p18cxxx.h"

#line 149 "D:/mcc18/h/p18cxxx.h"
#line 74 "./compiler.h"
    
                            
#line 77 "./compiler.h"

#line 79 "./compiler.h"
#line 82 "./compiler.h"

#line 1 "D:/mcc18/h/stdlib.h"
 

#line 4 "D:/mcc18/h/stdlib.h"

#line 6 "D:/mcc18/h/stdlib.h"

#line 9 "D:/mcc18/h/stdlib.h"
 
 

#line 16 "D:/mcc18/h/stdlib.h"
 
double atof (const auto char *s);

#line 28 "D:/mcc18/h/stdlib.h"
 
signed char atob (const auto char *s);


#line 39 "D:/mcc18/h/stdlib.h"
 
int atoi (const auto char *s);

#line 47 "D:/mcc18/h/stdlib.h"
 
long atol (const auto char *s);

#line 58 "D:/mcc18/h/stdlib.h"
 
unsigned long atoul (const auto char *s);


#line 71 "D:/mcc18/h/stdlib.h"
 
char *btoa (auto signed char value, auto char *s);

#line 83 "D:/mcc18/h/stdlib.h"
 
char *itoa (auto int value, auto char *s);

#line 95 "D:/mcc18/h/stdlib.h"
 
char *ltoa (auto long value, auto char *s);

#line 107 "D:/mcc18/h/stdlib.h"
 
char *ultoa (auto unsigned long value, auto char *s);
 


#line 112 "D:/mcc18/h/stdlib.h"
 
 

#line 116 "D:/mcc18/h/stdlib.h"
 
#line 118 "D:/mcc18/h/stdlib.h"


#line 124 "D:/mcc18/h/stdlib.h"
 
int rand (void);

#line 136 "D:/mcc18/h/stdlib.h"
 
void srand (auto unsigned int seed);
 
#line 140 "D:/mcc18/h/stdlib.h"
#line 149 "D:/mcc18/h/stdlib.h"

#line 151 "D:/mcc18/h/stdlib.h"
#line 83 "./compiler.h"



#line 87 "./compiler.h"
 
#line 89 "./compiler.h"
#line 90 "./compiler.h"

#line 92 "./compiler.h"
#line 93 "./compiler.h"
#line 94 "./compiler.h"
#line 95 "./compiler.h"
#line 96 "./compiler.h"
#line 97 "./compiler.h"

typedef enum _BOOL { FALSE = 0, TRUE } BOOL;
typedef unsigned char BYTE;                 
typedef unsigned short int WORD;            

typedef unsigned short long SWORD;          
#line 104 "./compiler.h"
#line 106 "./compiler.h"
typedef unsigned long DWORD;                

typedef union _BYTE_VAL
{
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
    } bits;
    BYTE Val;
} BYTE_VAL;

typedef union _SWORD_VAL
{
    SWORD Val;
    struct
    {
        BYTE LSB;
        BYTE MSB;
        BYTE USB;
    } byte;
} SWORD_VAL;

typedef struct _WORD_ARRAY
{
    BYTE offset0;
    BYTE offset1;
} WORD_ARRAY;

#line 142 "./compiler.h"
#line 143 "./compiler.h"

typedef union _WORD_VAL
{
    WORD Val;
    struct
    {
        BYTE LSB;
        BYTE MSB;
    } byte;
    BYTE v[2];
} WORD_VAL;

#line 156 "./compiler.h"
#line 157 "./compiler.h"

typedef union _DWORD_VAL
{
    DWORD Val;
    struct
    {
        BYTE LOLSB;
        BYTE LOMSB;
        BYTE HILSB;
        BYTE HIMSB;
    } byte;
    struct
    {
        WORD LSW;
        WORD MSW;
    } word;
    BYTE v[4];
} DWORD_VAL;
#line 176 "./compiler.h"
#line 177 "./compiler.h"
#line 178 "./compiler.h"
#line 179 "./compiler.h"

typedef BYTE BUFFER;

#line 183 "./compiler.h"
#line 184 "./compiler.h"
#line 185 "./compiler.h"
#line 186 "./compiler.h"
#line 187 "./compiler.h"

#line 189 "./compiler.h"

#line 191 "./compiler.h"
#line 192 "./compiler.h"
#line 193 "./compiler.h"

#line 195 "./compiler.h"
#line 196 "./compiler.h"
#line 197 "./compiler.h"
#line 198 "./compiler.h"
#line 199 "./compiler.h"

#line 201 "./compiler.h"
#line 202 "./compiler.h"
#line 203 "./compiler.h"
#line 204 "./compiler.h"
#line 205 "./compiler.h"
#line 206 "./compiler.h"


#line 209 "./compiler.h"
#line 210 "./compiler.h"
#line 211 "./compiler.h"
#line 212 "./compiler.h"
#line 213 "./compiler.h"
#line 214 "./compiler.h"
#line 215 "./compiler.h"
#line 216 "./compiler.h"
#line 217 "./compiler.h"
#line 218 "./compiler.h"
#line 219 "./compiler.h"

#line 221 "./compiler.h"
#line 222 "./compiler.h"
#line 223 "./compiler.h"
#line 224 "./compiler.h"
#line 225 "./compiler.h"
#line 226 "./compiler.h"
#line 227 "./compiler.h"
#line 228 "./compiler.h"

#line 230 "./compiler.h"
#line 231 "./compiler.h"
#line 232 "./compiler.h"
#line 233 "./compiler.h"
#line 234 "./compiler.h"
#line 235 "./compiler.h"
#line 236 "./compiler.h"
#line 237 "./compiler.h"

#line 239 "./compiler.h"
#line 240 "./compiler.h"
#line 241 "./compiler.h"
#line 242 "./compiler.h"
#line 243 "./compiler.h"
#line 244 "./compiler.h"
#line 245 "./compiler.h"
#line 246 "./compiler.h"

#line 248 "./compiler.h"
#line 249 "./compiler.h"
#line 250 "./compiler.h"
#line 251 "./compiler.h"
#line 252 "./compiler.h"
#line 253 "./compiler.h"
#line 254 "./compiler.h"
#line 255 "./compiler.h"
#line 256 "./compiler.h"
#line 257 "./compiler.h"
#line 258 "./compiler.h"
#line 259 "./compiler.h"
#line 260 "./compiler.h"
#line 261 "./compiler.h"
#line 262 "./compiler.h"
#line 263 "./compiler.h"

#line 265 "./compiler.h"
#line 266 "./compiler.h"
#line 267 "./compiler.h"
#line 268 "./compiler.h"
#line 269 "./compiler.h"

#line 271 "./compiler.h"
#line 272 "./compiler.h"
#line 273 "./compiler.h"
#line 274 "./compiler.h"
#line 275 "./compiler.h"
#line 276 "./compiler.h"

#line 278 "./compiler.h"
#line 279 "./compiler.h"
#line 280 "./compiler.h"

#line 282 "./compiler.h"

#line 284 "./compiler.h"

#line 286 "./compiler.h"
#line 287 "./compiler.h"
#line 288 "./compiler.h"
#line 289 "./compiler.h"
#line 290 "./compiler.h"
#line 291 "./compiler.h"
#line 292 "./compiler.h"

#line 294 "./compiler.h"
#line 295 "./compiler.h"

#line 297 "./compiler.h"
#line 298 "./compiler.h"

#line 300 "./compiler.h"

#line 302 "./compiler.h"
#line 303 "./compiler.h"
#line 304 "./compiler.h"
#line 305 "./compiler.h"

#line 307 "./compiler.h"
#line 308 "./compiler.h"
#line 309 "./compiler.h"
#line 310 "./compiler.h"
#line 311 "./compiler.h"

#line 313 "./compiler.h"
#line 314 "./compiler.h"
#line 315 "./compiler.h"
#line 316 "./compiler.h"
#line 317 "./compiler.h"
#line 318 "./compiler.h"

#line 320 "./compiler.h"
#line 321 "./compiler.h"
#line 322 "./compiler.h"

#line 324 "./compiler.h"
#line 325 "./compiler.h"

#line 327 "./compiler.h"


#line 330 "./compiler.h"

#line 332 "./compiler.h"
#line 456 "./compiler.h"
#line 460 "./compiler.h"
#line 481 "./compiler.h"


#line 484 "./compiler.h"
#line 8 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 12 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 14 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"





#line 20 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 22 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 23 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 25 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 29 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 



#line 34 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 

#line 37 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 40 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 43 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 46 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 




#line 52 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 56 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

 


#line 62 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 









#line 76 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 78 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 80 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 82 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 86 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 88 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 96 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 97 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 99 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 101 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 104 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 108 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 110 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 114 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 116 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 119 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 121 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 131 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 133 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 137 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 139 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 140 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 143 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 

#line 146 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 148 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"




#line 155 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 

#line 158 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 161 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 162 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 163 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 166 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 168 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 170 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 173 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 175 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 177 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 183 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 185 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 191 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 




#line 200 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 

#line 203 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 205 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 207 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 209 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 211 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 214 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 216 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 217 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 218 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 219 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 220 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 223 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 225 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 226 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 228 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 229 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 231 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 232 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 233 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 235 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 237 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 240 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 242 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 243 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 245 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 246 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 248 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 250 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 253 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 255 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 256 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 258 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 259 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 264 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 266 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 268 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 270 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 272 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 284 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 286 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 287 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 289 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 290 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 294 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 296 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 297 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 298 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 299 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 301 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 302 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 303 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 304 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 306 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 307 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 308 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 309 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 311 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 312 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 313 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 314 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 315 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 316 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 320 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 321 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 322 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 323 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 324 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 325 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 330 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 332 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 333 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 334 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 335 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 341 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 343 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 344 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 345 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 346 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 350 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 352 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 353 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 354 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 355 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 368 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 373 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 375 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 376 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 377 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 379 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 382 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 384 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 387 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 389 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 391 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 393 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 396 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 397 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 398 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 399 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 405 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 409 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 412 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 414 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 416 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 418 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 420 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 424 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 426 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 427 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 428 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 430 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 432 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 434 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 436 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 437 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 438 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 439 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 441 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 444 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 
#line 446 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 447 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 448 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 450 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 452 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 455 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 458 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 459 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 461 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 462 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 463 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 465 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 467 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 468 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 469 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 471 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 473 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 474 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 475 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"


#line 478 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 481 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"



#line 485 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 42 "./snmp.h"



#line 48 "./snmp.h"
 
#line 50 "./snmp.h"



#line 56 "./snmp.h"
 
#line 58 "./snmp.h"
#line 59 "./snmp.h"


#line 62 "./snmp.h"
 
#line 64 "./snmp.h"



#line 68 "./snmp.h"
#line 69 "./snmp.h"
#line 70 "./snmp.h"


#line 87 "./snmp.h"
 
void SNMPInit(void);


#line 107 "./snmp.h"
 
BOOL SNMPTask(void);



#line 116 "./snmp.h"
 
typedef BYTE SNMP_ID;
typedef BYTE SNMP_INDEX;

typedef union _SNMP_VAL
{
    DWORD dword;
    WORD  word;
    BYTE  byte;
    BYTE  v[sizeof(DWORD)];
} SNMP_VAL;


#line 170 "./snmp.h"
 

    extern BOOL SNMPGetVar(SNMP_ID var, SNMP_INDEX index,
                           BYTE *ref, SNMP_VAL* val);
#line 175 "./snmp.h"



#line 200 "./snmp.h"
 

    extern BOOL SNMPGetNextIndex(SNMP_ID var, SNMP_INDEX *index);
#line 204 "./snmp.h"



#line 225 "./snmp.h"
 

    extern BOOL SNMPIsValidSetLen(SNMP_ID var, BYTE len);
#line 229 "./snmp.h"



#line 268 "./snmp.h"
 

    extern BOOL SNMPSetVar(SNMP_ID var, SNMP_INDEX index,
                            BYTE ref, SNMP_VAL val);
#line 273 "./snmp.h"




#line 282 "./snmp.h"
 
typedef enum _SNMP_ACTION
{
    SNMP_GET            = 0xa0,
    SNMP_GET_NEXT       = 0xa1,
    SNMP_GET_RESPONSE   = 0xa2,
    SNMP_SET            = 0xa3,
    SNMP_TRAP           = 0xa4,
    SNMP_ACTION_UNKNOWN = 0
} SNMP_ACTION;


#line 314 "./snmp.h"
 

    extern BOOL SNMPValidate(SNMP_ACTION SNMPAction, char* community);
#line 318 "./snmp.h"




#line 348 "./snmp.h"
 
void SNMPNotifyPrepare(IP_ADDR *remoteHost,
                        char *community,
                        BYTE communityLen,
                        SNMP_ID agentIDVar,
                        BYTE notificationCode,
                        DWORD timestamp);



#line 381 "./snmp.h"
 
BOOL SNMPIsNotifyReady(IP_ADDR *remoteHost);



#line 419 "./snmp.h"
 
BOOL SNMPNotify(SNMP_ID var, SNMP_VAL val, SNMP_INDEX index);





#line 427 "./snmp.h"
#line 49 "snmp.c"


#line 1 "./udp.h"

#line 38 "./udp.h"
 


#line 42 "./udp.h"

#line 1 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 3 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 12 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 20 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 34 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 46 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 62 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 76 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 80 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 82 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 86 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 96 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 97 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 99 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 101 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 104 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 108 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 114 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 119 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 131 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 137 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 143 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 155 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 161 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 163 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 166 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 168 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 170 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 173 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 175 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 177 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 183 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 191 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 200 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 205 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 207 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 209 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 211 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 214 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 216 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 217 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 219 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 220 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 223 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 225 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 226 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 228 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 229 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 231 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 233 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 235 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 237 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 240 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 242 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 243 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 245 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 246 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 248 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 250 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 253 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 255 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 256 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 258 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 259 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 264 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 266 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 268 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 270 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 272 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 284 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 286 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 287 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 289 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 290 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 294 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 330 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 341 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 350 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 368 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 373 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 375 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 377 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 379 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 382 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 387 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 389 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 391 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 393 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 396 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 399 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 405 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 409 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 414 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 416 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 418 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 420 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 424 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 426 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 428 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 430 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 432 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 434 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 437 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 439 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 441 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 444 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 446 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 448 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 450 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 452 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 455 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 459 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 461 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 463 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 465 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 467 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 469 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 471 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 473 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 475 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 478 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 481 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 485 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 43 "./udp.h"


#line 1 "./ip.h"

#line 40 "./ip.h"
 


#line 44 "./ip.h"

#line 1 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 3 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 12 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 20 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 34 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 46 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 62 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 76 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 80 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 82 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 86 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 96 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 97 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 99 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 101 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 104 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 108 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 114 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 119 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 131 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 137 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 143 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 155 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 161 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 163 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 166 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 168 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 170 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 173 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 175 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 177 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 183 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 191 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 200 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 205 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 207 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 209 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 211 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 214 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 216 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 217 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 219 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 220 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 223 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 225 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 226 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 228 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 229 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 231 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 233 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 235 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 237 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 240 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 242 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 243 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 245 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 246 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 248 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 250 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 253 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 255 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 256 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 258 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 259 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 264 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 266 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 268 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 270 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 272 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 284 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 286 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 287 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 289 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 290 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 294 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 330 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 341 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 350 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 368 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 373 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 375 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 377 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 379 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 382 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 387 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 389 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 391 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 393 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 396 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 399 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 405 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 409 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 414 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 416 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 418 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 420 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 424 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 426 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 428 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 430 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 432 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 434 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 437 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 439 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 441 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 444 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 446 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 448 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 450 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 452 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 455 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 459 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 461 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 463 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 465 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 467 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 469 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 471 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 473 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 475 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 478 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 481 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 485 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 45 "./ip.h"

#line 1 "./mac.h"

#line 40 "./mac.h"
 


#line 55 "./mac.h"
 


#line 59 "./mac.h"

#line 1 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 3 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 12 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 20 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 34 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 46 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 62 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 76 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 80 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 82 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 86 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 96 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 97 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 99 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 101 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 104 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 108 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 114 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 119 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 131 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 137 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 143 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 155 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 161 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 163 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 166 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 168 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 170 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 173 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 175 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 177 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 183 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 191 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 200 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 205 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 207 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 209 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 211 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 214 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 216 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 217 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 219 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 220 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 223 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 225 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 226 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 228 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 229 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 231 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 233 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 235 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 237 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 240 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 242 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 243 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 245 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 246 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 248 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 250 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 253 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 255 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 256 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 258 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 259 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 264 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 266 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 268 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 270 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 272 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 284 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 286 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 287 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 289 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 290 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 294 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 330 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 341 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 350 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 368 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 373 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 375 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 377 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 379 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 382 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 387 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 389 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 391 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 393 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 396 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 399 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 405 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 409 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 414 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 416 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 418 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 420 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 424 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 426 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 428 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 430 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 432 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 434 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 437 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 439 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 441 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 444 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 446 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 448 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 450 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 452 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 455 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 459 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 461 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 463 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 465 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 467 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 469 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 471 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 473 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 475 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 478 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 481 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 485 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 60 "./mac.h"


#line 63 "./mac.h"
#line 64 "./mac.h"
#line 65 "./mac.h"

#line 67 "./mac.h"

typedef struct _MAC_ADDR
{
    BYTE v[6];
} MAC_ADDR;

void    MACTask(void);
void    MACInit(void);
#line 76 "./mac.h"
#line 78 "./mac.h"
BOOL    MACIsTxReady(void);

BOOL    MACGetHeader(MAC_ADDR *remote, BYTE* type);
BYTE    MACGet(void);
WORD    MACGetArray(BYTE *val, WORD len);
void    MACDiscardRx(void);

void    MACPutHeader(MAC_ADDR *remote,
                     BYTE type,
                     WORD dataLen);
void    MACPut(BYTE val);
void    MACPutArray(BYTE *val, WORD len);
void    MACFlush(void);
void    MACDiscardTx(BUFFER buffer);

void    MACSetRxBuffer(WORD offset);
void    MACSetTxBuffer(BUFFER buffer, WORD offset);
void    MACReserveTxBuffer(BUFFER buffer);

WORD    MACGetOffset(void);

WORD    MACGetFreeRxSize(void);
void    MACShowDebug(void);

#line 103 "./mac.h"
#line 104 "./mac.h"

#line 106 "./mac.h"
#line 107 "./mac.h"
extern  BYTE NICCurrentTxBuffer;
extern  BYTE NICCurrentRdPtr;
#line 110 "./mac.h"
#line 113 "./mac.h"
#line 114 "./mac.h"

BOOL MACIsLinked(void);


#line 119 "./mac.h"
#line 46 "./ip.h"


#line 49 "./ip.h"
#line 50 "./ip.h"
#line 51 "./ip.h"

typedef union _IP_ADDR
{
    BYTE        v[4];
    DWORD       Val;
} IP_ADDR;

typedef struct _NODE_INFO
{
    MAC_ADDR    MACAddr;	
    IP_ADDR     IPAddr;		
} NODE_INFO;	


#line 66 "./ip.h"
 
typedef struct _IP_HEADER
{
    BYTE    VersionIHL;
    BYTE    TypeOfService;
    WORD    TotalLength;
    WORD    Identification;
    WORD    FragmentInfo;
    BYTE    TimeToLive;
    BYTE    Protocol;
    WORD    HeaderChecksum;
    IP_ADDR SourceAddress;
    IP_ADDR DestAddress;

} IP_HEADER;



#line 97 "./ip.h"
 
#line 99 "./ip.h"



#line 116 "./ip.h"
 
#line 118 "./ip.h"




#line 144 "./ip.h"
 
WORD    IPPutHeader(NODE_INFO *remote,
                    BYTE protocol,
                    WORD len);



#line 164 "./ip.h"
 
#line 166 "./ip.h"





#line 195 "./ip.h"
 
BOOL IPGetHeader(IP_ADDR *localIP,
                 NODE_INFO *remote,
                 BYTE *protocol,
                 WORD *len);



#line 216 "./ip.h"
 
#line 218 "./ip.h"




#line 235 "./ip.h"
 
#line 237 "./ip.h"





#line 255 "./ip.h"
 
#line 257 "./ip.h"





#line 263 "./ip.h"



#line 45 "./udp.h"

#line 1 "./mac.h"

#line 40 "./mac.h"
 


#line 55 "./mac.h"
 

#line 76 "./mac.h"
#line 78 "./mac.h"
#line 106 "./mac.h"
#line 107 "./mac.h"
#line 110 "./mac.h"
#line 113 "./mac.h"
#line 114 "./mac.h"
#line 119 "./mac.h"
#line 46 "./udp.h"



typedef WORD UDP_PORT;
typedef BYTE UDP_SOCKET;

typedef struct _UDP_SOCKET_INFO
{
    NODE_INFO   remoteNode;	
    UDP_PORT    remotePort;	
    UDP_PORT    localPort;	
    WORD        TxCount;	
    WORD        RxCount;	
    BUFFER      TxBuffer;	
    WORD        TxOffset;

    struct
    {
        unsigned int bFirstRead     : 1;
    } Flags;

} UDP_SOCKET_INFO;


#line 71 "./udp.h"
#line 72 "./udp.h"


#line 76 "./udp.h"
 
#line 78 "./udp.h"
    extern UDP_SOCKET activeUDPSocket;
    extern UDP_SOCKET_INFO  UDPSocketInfo[(2) ];
#line 81 "./udp.h"


typedef struct _UDP_HEADER
{
    UDP_PORT    SourcePort;
    UDP_PORT    DestinationPort;
    WORD        Length;
    WORD        Checksum;
} UDP_HEADER;



#line 106 "./udp.h"
 
void        UDPInit(void);



#line 139 "./udp.h"
 
UDP_SOCKET UDPOpen(UDP_PORT localPort,
                   NODE_INFO *remoteNode,
                   UDP_PORT remotePort);



#line 161 "./udp.h"
 
void UDPClose(UDP_SOCKET s);



#line 181 "./udp.h"
 
#line 183 "./udp.h"



#line 209 "./udp.h"
 
BOOL UDPPut(BYTE v);



#line 231 "./udp.h"
 
void UDPFlush(void);




#line 256 "./udp.h"
 
BOOL UDPIsGetReady(UDP_SOCKET s);




#line 278 "./udp.h"
 
BOOL UDPGet(BYTE *v);



#line 297 "./udp.h"
 
void UDPDiscard(void);




#line 320 "./udp.h"
 
BOOL        UDPProcess(NODE_INFO *remoteNode, WORD len);



#line 339 "./udp.h"
 
#line 341 "./udp.h"



#line 357 "./udp.h"
 
#line 359 "./udp.h"


#line 362 "./udp.h"
#line 51 "snmp.c"

#line 1 "./tick.h"

#line 39 "./tick.h"
 


#line 43 "./tick.h"

#line 1 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 3 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 12 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 20 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 34 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 46 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 62 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 76 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 80 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 82 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 86 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 96 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 97 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 99 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 101 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 104 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 108 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 114 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 119 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 131 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 137 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 143 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 155 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 161 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 163 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 166 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 168 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 170 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 173 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 175 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 177 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 183 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 191 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 200 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 205 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 207 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 209 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 211 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 214 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 216 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 217 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 219 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 220 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 223 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 225 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 226 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 228 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 229 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 231 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 233 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 235 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 237 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 240 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 242 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 243 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 245 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 246 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 248 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 250 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 253 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 255 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 256 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 258 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 259 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 264 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 266 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 268 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 270 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 272 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 284 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 286 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 287 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 289 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 290 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 294 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 330 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 341 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 350 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 368 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 373 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 375 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 377 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 379 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 382 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 387 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 389 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 391 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 393 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 396 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 399 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 405 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 409 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 414 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 416 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 418 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 420 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 424 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 426 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 428 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 430 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 432 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 434 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 437 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 439 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 441 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 444 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 446 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 448 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 450 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 452 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 455 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 459 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 461 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 463 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 465 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 467 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 469 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 471 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 473 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 475 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 478 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 481 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 485 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 44 "./tick.h"


typedef unsigned long TICK;

#line 49 "./tick.h"
#line 51 "./tick.h"

#line 53 "./tick.h"
#line 55 "./tick.h"

#line 57 "./tick.h"

#line 59 "./tick.h"



#line 64 "./tick.h"
 

extern TICK TickCount;
#line 68 "./tick.h"


#line 83 "./tick.h"
 
void TickInit(void);




#line 102 "./tick.h"
 
TICK TickGet(void);



#line 120 "./tick.h"
 

void TickUpdate(void);

#line 125 "./tick.h"
#line 52 "snmp.c"

#line 1 "./arptsk.h"

#line 40 "./arptsk.h"
 


#line 44 "./arptsk.h"

#line 1 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 3 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 12 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 20 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 34 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 46 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 62 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 76 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 80 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 82 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 86 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 96 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 97 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 99 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 101 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 104 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 108 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 114 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 119 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 131 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 137 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 143 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 155 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 161 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 163 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 166 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 168 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 170 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 173 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 175 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 177 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 183 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 191 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 200 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 205 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 207 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 209 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 211 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 214 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 216 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 217 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 219 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 220 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 223 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 225 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 226 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 228 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 229 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 231 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 233 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 235 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 237 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 240 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 242 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 243 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 245 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 246 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 248 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 250 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 253 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 255 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 256 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 258 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 259 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 264 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 266 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 268 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 270 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 272 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 284 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 286 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 287 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 289 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 290 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 294 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 330 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 341 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 350 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 368 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 373 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 375 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 377 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 379 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 382 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 387 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 389 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 391 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 393 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 396 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 399 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 405 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 409 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 414 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 416 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 418 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 420 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 424 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 426 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 428 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 430 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 432 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 434 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 437 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 439 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 441 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 444 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 446 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 448 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 450 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 452 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 455 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 459 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 461 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 463 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 465 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 467 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 469 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 471 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 473 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 475 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 478 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 481 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 485 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 45 "./arptsk.h"

#line 1 "./ip.h"

#line 40 "./ip.h"
 


#line 66 "./ip.h"

#line 97 "./ip.h"

#line 116 "./ip.h"

#line 144 "./ip.h"

#line 164 "./ip.h"

#line 195 "./ip.h"

#line 216 "./ip.h"

#line 235 "./ip.h"

#line 255 "./ip.h"
#line 263 "./ip.h"



#line 46 "./arptsk.h"




#line 63 "./arptsk.h"
 
void ARPInit(void);



#line 81 "./arptsk.h"
 
BOOL ARPProcess(void);



#line 99 "./arptsk.h"
 
void ARPResolve(IP_ADDR *IPAddr);



#line 122 "./arptsk.h"
 
BOOL ARPIsResolved(IP_ADDR *IPAddr, MAC_ADDR *MACAddr);



#line 128 "./arptsk.h"
#line 53 "snmp.c"

#line 55 "snmp.c"
#line 1 "./mpfs.h"

#line 43 "./mpfs.h"
 


#line 47 "./mpfs.h"

#line 1 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 3 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
 


#line 12 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 20 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 34 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 46 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 62 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 76 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 80 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 82 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 86 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 96 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 97 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 99 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 101 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 104 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 108 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 114 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 119 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 131 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 137 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 143 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 155 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 161 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 163 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 166 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 168 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 170 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 173 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 175 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 177 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 183 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 191 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 200 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 205 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 207 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 209 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 211 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 214 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 216 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 217 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 219 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 220 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 223 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 225 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 226 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 228 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 229 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 231 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 233 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 235 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 237 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 240 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 242 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 243 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 245 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 246 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 248 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 250 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 253 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 255 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 256 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 258 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 259 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 264 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 266 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 268 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 270 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 272 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 284 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 286 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 287 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 289 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 290 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 294 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 330 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 341 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 350 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 368 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 373 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 375 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 377 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 379 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 382 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 387 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 389 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 391 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 393 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 396 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 399 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 405 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 409 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 414 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 416 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 418 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 420 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 424 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 426 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 428 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 430 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 432 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 434 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 437 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 439 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 441 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"

#line 444 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 446 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 448 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 450 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 452 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 455 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 459 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 461 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 463 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 465 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 467 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 469 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 471 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 473 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 475 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 478 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 481 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 485 "D:/dev/firmware/pic/sbc65/vscp/projdefs.h"
#line 48 "./mpfs.h"


#line 51 "./mpfs.h"
#line 56 "./mpfs.h"
    typedef WORD MPFS;
#line 58 "./mpfs.h"
    typedef WORD MPFS_OFFSET;

#line 61 "./mpfs.h"

#line 63 "./mpfs.h"

#line 65 "./mpfs.h"
#line 66 "./mpfs.h"
#line 67 "./mpfs.h"
#line 69 "./mpfs.h"




#line 88 "./mpfs.h"
 
BOOL MPFSInit(void);



#line 107 "./mpfs.h"
 
MPFS   MPFSOpen(BYTE* name);




#line 126 "./mpfs.h"
 
void MPFSClose(void);



#line 145 "./mpfs.h"
 
#line 147 "./mpfs.h"
    BOOL MPFSGetBegin(MPFS handle);
#line 149 "./mpfs.h"
#line 151 "./mpfs.h"



#line 169 "./mpfs.h"
 
BYTE MPFSGet(void);



#line 190 "./mpfs.h"
 
#line 192 "./mpfs.h"
    MPFS MPFSGetEnd(void);
#line 194 "./mpfs.h"
#line 196 "./mpfs.h"



#line 213 "./mpfs.h"
 
#line 215 "./mpfs.h"



#line 233 "./mpfs.h"
 
MPFS MPFSFormat(void);




#line 254 "./mpfs.h"
 
#line 256 "./mpfs.h"
    BOOL MPFSPutBegin(MPFS handle);
#line 258 "./mpfs.h"
#line 260 "./mpfs.h"



#line 279 "./mpfs.h"
 
BOOL MPFSPut(BYTE b);



#line 302 "./mpfs.h"
 
MPFS MPFSPutEnd(void);



#line 321 "./mpfs.h"
 
#line 323 "./mpfs.h"
extern BYTE mpfsOpenCount;
#line 325 "./mpfs.h"


#line 340 "./mpfs.h"
 
MPFS MPFSSeek(MPFS_OFFSET offset);



#line 358 "./mpfs.h"
 
#line 360 "./mpfs.h"


#line 363 "./mpfs.h"

#line 365 "./mpfs.h"
    extern MPFS _currentHandle;
    extern BYTE _currentCount;
#line 368 "./mpfs.h"


#line 371 "./mpfs.h"
#line 55 "snmp.c"


#line 58 "snmp.c"
#line 60 "snmp.c"

#line 62 "snmp.c"
