#ifndef __CS8900A_H__
#define __CS8900A_H__

#include "uip_arch.h"

void cs8900a_init(void);
void cs8900a_send(void);
u8_t cs8900a_poll(void);

unsigned cs8900a_read( unsigned addr );
void cs8900a_write( unsigned addr, unsigned int data );
unsigned cs8900a_read_addr_high_first( unsigned addr );

#endif /* __CS8900A_H__ */
