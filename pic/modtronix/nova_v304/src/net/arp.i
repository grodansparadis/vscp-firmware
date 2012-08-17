#line 1 "arp.c"
#line 1 "arp.c"

#line 46 "arp.c"
 
#line 1 "d:/MCC18/h/string.h"

#line 3 "d:/MCC18/h/string.h"


#line 1 "d:/MCC18/h/stddef.h"
 

#line 4 "d:/MCC18/h/stddef.h"

typedef unsigned char wchar_t;


#line 10 "d:/MCC18/h/stddef.h"
 
typedef signed short int ptrdiff_t;
typedef signed short int ptrdiffram_t;
typedef signed short long int ptrdiffrom_t;


#line 20 "d:/MCC18/h/stddef.h"
 
typedef unsigned short int size_t;
typedef unsigned short int sizeram_t;
typedef unsigned short long int sizerom_t;


#line 34 "d:/MCC18/h/stddef.h"
 
#line 36 "d:/MCC18/h/stddef.h"


#line 41 "d:/MCC18/h/stddef.h"
 
#line 43 "d:/MCC18/h/stddef.h"

#line 45 "d:/MCC18/h/stddef.h"
#line 5 "d:/MCC18/h/string.h"

#line 7 "d:/MCC18/h/string.h"


#line 20 "d:/MCC18/h/string.h"
 
#line 22 "d:/MCC18/h/string.h"


#line 25 "d:/MCC18/h/string.h"
 
#line 27 "d:/MCC18/h/string.h"

 

#line 39 "d:/MCC18/h/string.h"
 
void *memcpy (auto void *s1, auto const void *s2, auto size_t n);


#line 55 "d:/MCC18/h/string.h"
 
void *memmove (auto void *s1, auto const void *s2, auto size_t n);


#line 67 "d:/MCC18/h/string.h"
 
char *strcpy (auto char *s1, auto const char *s2);


#line 83 "d:/MCC18/h/string.h"
 
char *strncpy (auto char *s1, auto const char *s2, auto size_t n);


#line 97 "d:/MCC18/h/string.h"
 
char *strcat (auto char *s1, auto const char *s2);


#line 113 "d:/MCC18/h/string.h"
 
char *strncat (auto char *s1, auto const char *s2, auto size_t n);


#line 128 "d:/MCC18/h/string.h"
 
signed char memcmp (auto const void *s1, auto const void *s2, auto size_t n);


#line 141 "d:/MCC18/h/string.h"
 
signed char strcmp (auto const char *s1, auto const char *s2);


#line 147 "d:/MCC18/h/string.h"
 


#line 161 "d:/MCC18/h/string.h"
 
signed char strncmp (auto const char *s1, auto const char *s2, auto size_t n);


#line 167 "d:/MCC18/h/string.h"
 


#line 183 "d:/MCC18/h/string.h"
 
void *memchr (auto const void *s, auto unsigned char c, auto size_t n);


#line 199 "d:/MCC18/h/string.h"
 
char *strchr (auto const char *s, auto unsigned char c);


#line 210 "d:/MCC18/h/string.h"
 
size_t strcspn (auto const char *s1, auto const char *s2);


#line 222 "d:/MCC18/h/string.h"
 
char *strpbrk (auto const char *s1, auto const char *s2);


#line 238 "d:/MCC18/h/string.h"
 
char *strrchr (auto const char *s, auto unsigned char c);


#line 249 "d:/MCC18/h/string.h"
 
size_t strspn (auto const char *s1, auto const char *s2);


#line 262 "d:/MCC18/h/string.h"
 
char *strstr (auto const char *s1, auto const char *s2);


#line 305 "d:/MCC18/h/string.h"
 
char *strtok (auto char *s1, auto const char *s2);


#line 321 "d:/MCC18/h/string.h"
 
void *memset (auto void *s, auto unsigned char c, auto size_t n);


#line 339 "d:/MCC18/h/string.h"
 
#line 341 "d:/MCC18/h/string.h"


#line 349 "d:/MCC18/h/string.h"
 
size_t strlen (auto const char *s);


#line 358 "d:/MCC18/h/string.h"
 
char *strupr (auto char *s);


#line 367 "d:/MCC18/h/string.h"
 
char *strlwr (auto char *s);



 

#line 379 "d:/MCC18/h/string.h"
 
far  rom void *memcpypgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 389 "d:/MCC18/h/string.h"
 
void *memcpypgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 398 "d:/MCC18/h/string.h"
 
far  rom void *memcpyram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 407 "d:/MCC18/h/string.h"
 
far  rom void *memmovepgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 417 "d:/MCC18/h/string.h"
 
void *memmovepgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 426 "d:/MCC18/h/string.h"
 
far  rom void *memmoveram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 434 "d:/MCC18/h/string.h"
 
far  rom char *strcpypgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 443 "d:/MCC18/h/string.h"
 
char *strcpypgm2ram (auto char *s1, auto const far  rom char *s2);


#line 451 "d:/MCC18/h/string.h"
 
far  rom char *strcpyram2pgm (auto far  rom char *s1, auto const char *s2);


#line 460 "d:/MCC18/h/string.h"
 
far  rom char *strncpypgm (auto far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 470 "d:/MCC18/h/string.h"
 
char *strncpypgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 479 "d:/MCC18/h/string.h"
 
far  rom char *strncpyram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 487 "d:/MCC18/h/string.h"
 
far  rom char *strcatpgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 496 "d:/MCC18/h/string.h"
 
char *strcatpgm2ram (auto char *s1, auto const far  rom char *s2);


#line 504 "d:/MCC18/h/string.h"
 
far  rom char *strcatram2pgm (auto far  rom char *s1, auto const char *s2);


#line 513 "d:/MCC18/h/string.h"
 
far  rom char *strncatpgm (auto far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 523 "d:/MCC18/h/string.h"
 
char *strncatpgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 532 "d:/MCC18/h/string.h"
 
far  rom char *strncatram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 541 "d:/MCC18/h/string.h"
 
signed char memcmppgm (auto far  rom void *s1, auto const far  rom void *s2, auto sizerom_t n);


#line 551 "d:/MCC18/h/string.h"
 
signed char memcmppgm2ram (auto void *s1, auto const far  rom void *s2, auto sizeram_t n);


#line 560 "d:/MCC18/h/string.h"
 
signed char memcmpram2pgm (auto far  rom void *s1, auto const void *s2, auto sizeram_t n);


#line 568 "d:/MCC18/h/string.h"
 
signed char strcmppgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 577 "d:/MCC18/h/string.h"
 
signed char strcmppgm2ram (auto const char *s1, auto const far  rom char *s2);


#line 585 "d:/MCC18/h/string.h"
 
signed char strcmpram2pgm (auto const far  rom char *s1, auto const char *s2);


#line 594 "d:/MCC18/h/string.h"
 
signed char strncmppgm (auto const far  rom char *s1, auto const far  rom char *s2, auto sizerom_t n);


#line 604 "d:/MCC18/h/string.h"
 
signed char strncmppgm2ram (auto char *s1, auto const far  rom char *s2, auto sizeram_t n);


#line 613 "d:/MCC18/h/string.h"
 
signed char strncmpram2pgm (auto far  rom char *s1, auto const char *s2, auto sizeram_t n);


#line 622 "d:/MCC18/h/string.h"
 
far  rom char *memchrpgm (auto const far  rom char *s, auto const unsigned char c, auto sizerom_t n);


#line 631 "d:/MCC18/h/string.h"
 
far  rom char *strchrpgm (auto const far  rom char *s, auto unsigned char c);


#line 639 "d:/MCC18/h/string.h"
 
sizerom_t strcspnpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 647 "d:/MCC18/h/string.h"
 
sizerom_t strcspnpgmram (auto const far  rom char *s1, auto const char *s2);


#line 655 "d:/MCC18/h/string.h"
 
sizeram_t strcspnrampgm (auto const char *s1, auto const far  rom char *s2);


#line 663 "d:/MCC18/h/string.h"
 
far  rom char *strpbrkpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 671 "d:/MCC18/h/string.h"
 
far  rom char *strpbrkpgmram (auto const far  rom char *s1, auto const char *s2);


#line 679 "d:/MCC18/h/string.h"
 
char *strpbrkrampgm (auto const char *s1, auto const far  rom char *s2);


#line 688 "d:/MCC18/h/string.h"
 
 


#line 696 "d:/MCC18/h/string.h"
 
sizerom_t strspnpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 704 "d:/MCC18/h/string.h"
 
sizerom_t strspnpgmram (auto const far  rom char *s1, auto const char *s2);


#line 712 "d:/MCC18/h/string.h"
 
sizeram_t strspnrampgm (auto const char *s1, auto const far  rom char *s2);


#line 720 "d:/MCC18/h/string.h"
 
far  rom char *strstrpgm (auto const far  rom char *s1, auto const far  rom char *s2);


#line 729 "d:/MCC18/h/string.h"
 
far  rom char *strstrpgmram (auto const far  rom char *s1, auto const char *s2);


#line 737 "d:/MCC18/h/string.h"
 
char *strstrrampgm (auto const char *s1, auto const far  rom char *s2);


#line 745 "d:/MCC18/h/string.h"
 
far  rom char *strtokpgm (auto far  rom char *s1, auto const far  rom char *s2);


#line 754 "d:/MCC18/h/string.h"
 
char *strtokpgmram (auto char *s1, auto const far  rom char *s2);


#line 762 "d:/MCC18/h/string.h"
 
far  rom char *strtokrampgm (auto far  rom char *s1, auto const char *s2);


#line 771 "d:/MCC18/h/string.h"
 
far  rom void *memsetpgm (auto far  rom void *s, auto unsigned char c, auto sizerom_t n);


#line 778 "d:/MCC18/h/string.h"
 
far  rom char *struprpgm (auto far  rom char *s);


#line 785 "d:/MCC18/h/string.h"
 
far  rom char *strlwrpgm (auto far  rom char *s);


#line 792 "d:/MCC18/h/string.h"
 
sizerom_t strlenpgm (auto const far  rom char *s);

#line 796 "d:/MCC18/h/string.h"

#line 798 "d:/MCC18/h/string.h"

#line 805 "d:/MCC18/h/string.h"
#line 814 "d:/MCC18/h/string.h"

#line 816 "d:/MCC18/h/string.h"
#line 47 "arp.c"







#line 55 "arp.c"
 
#line 57 "arp.c"
#line 58 "arp.c"


#line 61 "arp.c"
 
#line 63 "arp.c"
#line 64 "arp.c"



#line 68 "arp.c"
 
typedef struct _ARP_PACKET
{
    WORD        HardwareType;
    WORD        Protocol;
    BYTE        MACAddrLen;
    BYTE        ProtocolLen;
    WORD        Operation;
    MAC_ADDR    SenderMACAddr;
    IP_ADDR     SenderIPAddr;
    MAC_ADDR    TargetMACAddr;
    IP_ADDR     TargetIPAddr;
} ARP_PACKET;

static void SwapARPPacket(ARP_PACKET *p);



#line 94 "arp.c"
 
BOOL ARPGet(NODE_INFO *remote, BYTE *opCode)
{
    ARP_PACKET packet;

    
    MACRxbufGetArray((BYTE*)&packet, sizeof(packet));

    MACRxbufDiscard();

    SwapARPPacket(&packet);

    if ( packet.HardwareType != (0x0001)      ||
         packet.MACAddrLen != sizeof(MAC_ADDR)  ||
         packet.ProtocolLen != sizeof(IP_ADDR) )
         return FALSE;

    if ( packet.Operation == 0x02  )
        *opCode = ARP_REPLY;
    else if ( packet.Operation == 0x01  )
        *opCode = ARP_REQUEST;
    else
    {
        *opCode = ARP_UNKNOWN;
        return FALSE;
    }

    
    if ( (packet.TargetIPAddr.v[0] == MY_IP_BYTE1) &&
         (packet.TargetIPAddr.v[1] == MY_IP_BYTE2) &&
         (packet.TargetIPAddr.v[2] == MY_IP_BYTE3) &&
         (packet.TargetIPAddr.v[3] == MY_IP_BYTE4) )
    {
        remote->MACAddr     = packet.SenderMACAddr;
        remote->IPAddr      = packet.SenderIPAddr;
        return TRUE;
    }
    else
        return FALSE;
}



#line 143 "arp.c"
 
void ARPPut(NODE_INFO *remote,
            BYTE opCode)
{
    ARP_PACKET packet;

    packet.HardwareType             = (0x0001) ;
    packet.Protocol                 = (0x0800) ;
    packet.MACAddrLen               = sizeof(MAC_ADDR);
    packet.ProtocolLen              = sizeof(IP_ADDR);

    if ( opCode == ARP_REQUEST )
    {
        packet.Operation            = 0x01 ;
        packet.TargetMACAddr.v[0]   = 0xff;
        packet.TargetMACAddr.v[1]   = 0xff;
        packet.TargetMACAddr.v[2]   = 0xff;
        packet.TargetMACAddr.v[3]   = 0xff;
        packet.TargetMACAddr.v[4]   = 0xff;
        packet.TargetMACAddr.v[5]   = 0xff;
    }
    else
    {
        packet.Operation            = 0x02 ;
        packet.TargetMACAddr        = remote->MACAddr;
    }

    packet.SenderMACAddr.v[0]       = MY_MAC_BYTE1;
    packet.SenderMACAddr.v[1]       = MY_MAC_BYTE2;
    packet.SenderMACAddr.v[2]       = MY_MAC_BYTE3;
    packet.SenderMACAddr.v[3]       = MY_MAC_BYTE4;
    packet.SenderMACAddr.v[4]       = MY_MAC_BYTE5;
    packet.SenderMACAddr.v[5]       = MY_MAC_BYTE6;

    packet.SenderIPAddr.v[0]        = MY_IP_BYTE1;
    packet.SenderIPAddr.v[1]        = MY_IP_BYTE2;
    packet.SenderIPAddr.v[2]        = MY_IP_BYTE3;
    packet.SenderIPAddr.v[3]        = MY_IP_BYTE4;


    
#line 188 "arp.c"
 
    if (((packet.SenderIPAddr.v[0] ^ remote->IPAddr.v[0]) &
                                                MY_MASK_BYTE1) ||
        ((packet.SenderIPAddr.v[1] ^ remote->IPAddr.v[1]) &
                                                MY_MASK_BYTE2) ||
        ((packet.SenderIPAddr.v[2] ^ remote->IPAddr.v[2]) &
                                                MY_MASK_BYTE3) ||
        ((packet.SenderIPAddr.v[3] ^ remote->IPAddr.v[3]) &
                                                MY_MASK_BYTE4) )
    {
        packet.TargetIPAddr.v[0] = MY_GATE_BYTE1;
        packet.TargetIPAddr.v[1] = MY_GATE_BYTE2;
        packet.TargetIPAddr.v[2] = MY_GATE_BYTE3;
        packet.TargetIPAddr.v[3] = MY_GATE_BYTE4;
    }
    else
        packet.TargetIPAddr             = remote->IPAddr;

    SwapARPPacket(&packet);

    
    
    MACPutHeader(&packet.TargetMACAddr, MAC_ARP, sizeof(packet));

    MACPutArray((BYTE*)&packet, sizeof(packet));

    MACFlush();
}



#line 220 "arp.c"
 
static void SwapARPPacket(ARP_PACKET *p)
{
    p->HardwareType     = swaps(p->HardwareType);
    p->Protocol         = swaps(p->Protocol);
    p->Operation        = swaps(p->Operation);
}
