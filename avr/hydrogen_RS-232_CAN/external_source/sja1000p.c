/**
   sja1000.c
   Linux CAN-bus device driver.
   Written by Arnaud Westenberg email:arnaud@wanadoo.nl
   This software is released under the GPL-License.
   Version 0.6 18 Sept 2000
   Changed for PeliCan mode SJA1000 by Tomasz Motylewski (BFAD GmbH)
   T.Motylewski@bfad.de
*/

#include <linux/delay.h>
#include <asm/irq.h>
#include "sysdep.h"
#include "candrv.h"
#include "sja1000p.h"

//struct chip_t *chip_irq = NULL;
//struct candevice_t *device_irq = NULL;
//struct canfifo_t *fifo_irq = NULL;

void (*put_reg)( u8 data, u32 address);
unsigned (*get_reg)( u32 address );


///////////////////////////////////////////////////////////////////////////////
// sja1000p_enable_configuration

int sja1000p_enable_configuration( struct chip_t *pchip )
{
  int i=0;
  enum sja1000_PeliCAN_MOD flags;
  
  disable_irq( pchip->irq );
  flags = pchip->read_register( pchip->vbase_addr + SJAMOD );
  
  while ((!(flags & MOD_RM)) && (i<=10)) {
    pchip->write_register( MOD_RM, pchip->vbase_addr + SJAMOD );
    // TODO: chinfigurable MOD_AFM (32/16 bit acceptance filter)
    // config MOD_LOM (listen only)
    udelay(100);
    i++;
    flags = pchip->read_register( pchip->vbase_addr + SJAMOD );
  }

  if (i>=10) {
    CANMSG("Reset error\n");
    enable_irq( pchip->irq );
    return -ENODEV;
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_disable_configuration

int sja1000p_disable_configuration( struct chip_t *pchip )
{
  int i=0;
  enum sja1000_PeliCAN_MOD flags;

  flags = pchip->read_register( pchip->vbase_addr + SJAMOD );

  while ( (flags & MOD_RM) && (i<=10) ) {
    pchip->write_register( 0, pchip->vbase_addr + SJAMOD );
    // TODO: chinfigurable MOD_AFM (32/16 bit acceptance filter)
    // config MOD_LOM (listen only)
    udelay(100);
    i++;
    flags = pchip->read_register( pchip->vbase_addr + SJAMOD );
  }

  if (i>=10) {
    CANMSG("Error leaving reset status\n");
    return -ENODEV;
  }

  enable_irq( pchip->irq );
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_chip_config

int sja1000p_chip_config( struct chip_t *pchip )
{
  if ( sja1000p_enable_configuration( pchip ) ) {
    return -ENODEV;
  }

  // Set mode, clock out, comparator 
  pchip->write_register( CDR_PELICAN | pchip->sja_cdr_reg, 
			 pchip->vbase_addr + SJACDR ); 
  // Set driver output configuration 
  pchip->write_register( pchip->sja_ocr_reg, 
			 pchip->vbase_addr + SJAOCR ); 

  if ( sja1000p_extended_mask( pchip, 0x00000000, 0xffffffff ) ) {
    return -ENODEV;
  }
	
  if ( !baudrate ) {
    baudrate=1000;
  }

  if ( sja1000p_baud_rate( pchip, 1000*baudrate, pchip->clock, 0, 75, 0) ) {
    return -ENODEV;
  }

  // Enable hardware interrupts 
  pchip->write_register( ENABLE_INTERRUPTS, pchip->vbase_addr + SJAIER ); 
  sja1000p_disable_configuration( pchip ); 
	
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_extended_mask

int sja1000p_extended_mask( struct chip_t *pchip, 
			    u32 code, 
			    u32 mask )
{
  int i;

  if ( sja1000p_enable_configuration( pchip ) ) {
    return -ENODEV;
  }

  // LSB to +3, MSB to +0	
  for( i=SJA_PeliCAN_AC_LEN; --i>=0; ) {
    pchip->write_register( code & 0xff, pchip->vbase_addr + SJAACR0+i);
    pchip->write_register( mask & 0xff, pchip->vbase_addr + SJAAMR0+i);
    code >>= 8;
    mask >>= 8;
  }

  DEBUGMSG("Setting acceptance code to 0x%lx\n",(unsigned long)code);
  DEBUGMSG("Setting acceptance mask to 0x%lx\n",(unsigned long)mask);

  sja1000p_disable_configuration(pchip);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_baud_rate
//
// Set communication parameters.
// param rate baud rate in Hz
// param clock frequency of sja1000 clock in Hz (ISA osc is 14318000)
// param sjw synchronization jump width (0-3) prescaled clock cycles
// param sampl_pt sample point in % (0-100) sets (TSEG1+1)/(TSEG1+TSEG2+2) 
// ratio
// param flags fields BTR1_SAM, OCMODE, OCPOL, OCTP, OCTN, CLK_OFF, CBP
//

int sja1000p_baud_rate( struct chip_t *pchip, 
			u32 rate, 
			u32 clock, 
			u32 sjw,
			u32 sampl_pt, 
			u32 flags ) 
{
  int best_error = 1000000000, error;
  int best_tseg=0, best_brp=0, best_rate=0, brp=0;
  int tseg=0, tseg1=0, tseg2=0;
	
  if ( sja1000p_enable_configuration( pchip ) ) { 
    return -ENODEV;
  }

  clock /=2;

  // tseg even = round down, odd = round up 
  for ( seg=(0+0+2)*2; tseg<=(MAX_TSEG2+MAX_TSEG1 + 2 ) * 2 + 1; tseg++ ) {
    brp = clock/( ( 1 + tseg/2 ) * rate) + tseg%2;
    if ( brp == 0 || brp > 64 ) {
      continue;
    }
    
    error = rate - clock/(brp*(1+tseg/2));
    
    if ( error < 0 ){
      error = -error;
    }

    if (error <= best_error) {
      best_error = error;
      best_tseg = tseg/2;
      best_brp = brp-1;
      best_rate = clock/(brp*(1+tseg/2));
    }
  }

  if (best_error && (rate/best_error < 10)) {
    CANMSG("baud rate %d is not possible with %d Hz clock\n",
	   rate, 2*clock);
    CANMSG("%d bps. brp=%d, best_tseg=%d, tseg1=%d, tseg2=%d\n",
	   best_rate, best_brp, best_tseg, tseg1, tseg2);
    return -EINVAL;
  }

  tseg2 = best_tseg-(sampl_pt*(best_tseg+1))/100;

  if ( tseg2 < 0 ) {
    tseg2 = 0;
  }

  if ( tseg2 > MAX_TSEG2 ) {
    tseg2 = MAX_TSEG2;
  }
  tseg1 = best_tseg-tseg2-2;
  
  if (tseg1>MAX_TSEG1) {
    tseg1 = MAX_TSEG1;
    tseg2 = best_tseg-tseg1-2;
  }

  DEBUGMSG("Setting %d bps.\n", best_rate);
  DEBUGMSG("brp=%d, best_tseg=%d, tseg1=%d, tseg2=%d, sampl_pt=%d\n",
	   best_brp, best_tseg, tseg1, tseg2,
	   ( 100 * ( best_tseg-tseg2 )/( best_tseg+1 )));


  pchip->write_register( sjw<<6 | best_brp, pchip->vbase_addr + SJABTR0);
  pchip->write_register( ( ( flags & BTR1_SAM) != 0)<<7 | (tseg2<<4) 
			 | tseg1, pchip->vbase_addr + SJABTR1);

  sja1000p_disable_configuration(pchip);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_read

void sja1000p_read( struct chip_t *pchip ) {
  int i, flags, len, datastart;
  do {
    flags = pchip->read_register( pchip->vbase_addr + SJAFRM );
    
    if ( flags&FRM_FF ) {
      ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].id =
	( pchip->read_register( pchip->vbase_addr + SJAID0)<<21) +
	( pchip->read_register( pchip->vbase_addr + SJAID1)<<13) +
	( pchip->read_register( pchip->vbase_addr + SJAID2)<<5) +
	( pchip->read_register( pchip->vbase_addr + SJAID3)>>3);
      datastart = SJADATE;
    } 
    else {
      ( ( struct canmsg_t *)pchip->fifo.prxbuf )[pchip->fifo.head].id =
	( pchip->read_register( pchip->vbase_addr + SJAID0)<<3) +
	( pchip->read_register( pchip->vbase_addr + SJAID1)>>5);
      datastart = SJADATS;
    }
    ( ( struct canmsg_t *)pchip->fifo.prxbuf) [pchip->fifo.head].flags =
      ((flags & FRM_RTR) ? MSG_RTR : 0) |
      ((flags & FRM_FF) ? MSG_EXT : 0);
    len = flags & FRM_DLC_M;
    for(i=0; i< len; i++) {
      ( ( struct canmsg_t *)pchip->fifo.prxbuf)[ pchip->fifo.head ].data[i]=
	pchip->read_register( pchip->vbase_addr + datastart+i);
    }
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].length = len;
    pchip->fifo.head++; pchip->fifo.head %= MAX_BUF_LENGTH;
    // FIXME: what if pchip->fifo.head == pchip->fifo.tail again ?
    pchip->write_register( CMR_RRB, pchip->vbase_addr + SJACMR);
  } while ( pchip->read_register( pchip->vbase_addr + SJASR) & SR_RBS);
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_pre_read_config

int sja1000p_pre_read_config( struct chip_t *pchip )
{
  int i;
  i = pchip->read_register( pchip->vbase_addr + SJASR );
	
  if (!(i&SR_RBS)) {
    return 0;
  }

  //disable interrupts for a moment
  pchip->write_register( DISABLE_INTERRUPTS, 
			 pchip->vbase_addr + SJAIER); 
  sja1000p_read( pchip );

  //enable interrupts
  pchip->write_register( ENABLE_INTERRUPTS,
			 pchip->vbase_addr + SJAIER); 
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_pre_write_config

#define MAX_TRANSMIT_WAIT_LOOPS 200

int sja1000p_pre_write_config( struct chip_t *pchip, struct canmsg_t *pmsg )
{
  int i=0; 
  unsigned int id = 0;

  // Wait until Transmit Buffer Status is released 
  while ( !( pchip->read_register( pchip->vbase_addr +  SJASR ) & SR_TBS ) && 
	  i++ < MAX_TRANSMIT_WAIT_LOOPS ) {
    udelay (i );
  }
	
  if ( !( pchip->read_register( pchip->vbase_addr +  SJASR) & SR_TBS ) ) {
    CANMSG("Transmit timed out, cancelling\n");
    // here we should check if there is no write/select waiting for this
    // transmit. If so, set error ret and wake up.
    // CHECKME: if we do not disable IER_TIE (TX IRQ) here we get interrupt
    // immediately
    pchip->write_register( CMR_AT, pchip->vbase_addr + SJACMR );
    i=0;
    while ( !( pchip->read_register( pchip->vbase_addr + SJASR ) & SR_TBS) &&
	    i++ < MAX_TRANSMIT_WAIT_LOOPS ) {
      udelay( i );
    }
    if ( !( pchip->read_register( pchip->vbase_addr +  SJASR) & SR_TBS ) ) {
      CANMSG("Could not cancel, please reset\n");
      return -EIO;
    }
  }
  pmsg->length &= FRM_DLC_M;
  pchip->write_register( ( ( pmsg->flags & MSG_EXT )? FRM_FF:0 ) |
		 ( ( pmsg->flags & MSG_RTR ) ? FRM_RTR : 0) |
		 pmsg->length, pchip->vbase_addr + SJAFRM );

  if ( pmsg->flags & MSG_EXT ) {
    id = pmsg->id<<3;
    pchip->write_register( id & 0xff, pchip->vbase_addr + SJAID3 );
    id >>= 8;
    pchip->write_register( id & 0xff, pchip->vbase_addr + SJAID2 );
    id >>= 8;
    pchip->write_register( id & 0xff, pchip->vbase_addr + SJAID1 );
    id >>= 8;
    pchip->write_register( id, pchip->vbase_addr + SJAID0);
    for( i=0; i < pmsg->length; i++) {
      pchip->write_register( pmsg->data[i], pchip->vbase_addr + SJADATE + i );
    }
  } 
  else {
    id = pmsg->id >> 5;
    pchip->write_register( id & 0xff, pchip->vbase_addr + SJAID0 );
    id >>= 8;
    pchip->write_register( id & 0xff, pchip->vbase_addr + SJAID1 );
    
    for(i=0; i < pmsg->length; i++) {
      pchip->write_register( pmsg->data[ i ], 
			     pchip->vbase_addr + SJADATS + i );
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_send_msg

int sja1000p_send_msg( struct chip_t *pchip, int bRtr )
{
  pchip->write_register( CMR_TR, pchip->vbase_addr + SJACMR );
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_check_tx_stat

int sja1000p_check_tx_stat( struct chip_t *pchip )
{
  if ( pchip->read_register( pchip->vbase_addr + SJASR ) & SR_TCS ) {
    return 0;
  }
  else {
    return 1;
  }
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_set_btregs

int sja1000p_set_btregs( struct chip_t *pchip, 
			 u16 btr0, 
			 u16 btr1)
{
  if ( sja1000p_enable_configuration( pchip ) ) {
    return -ENODEV;
  }

  pchip->write_register( btr0, pchip->vbase_addr + SJABTR0 );
  pchip->write_register( btr1, pchip->vbase_addr + SJABTR1 );

  sja1000p_disable_configuration( pchip );
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_start_chip

int sja1000p_start_chip( struct chip_t *pchip )
{
  enum sja1000_PeliCAN_MOD flags;

  flags = pchip->read_register( pchip->vbase_addr + SJAMOD) & 
    (MOD_LOM|MOD_STM|MOD_AFM|MOD_SM);
  pchip->write_register( flags, pchip->vbase_addr + SJAMOD );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_stop_chip

int sja1000p_stop_chip( struct chip_t *pchip )
{
  enum sja1000_PeliCAN_MOD flags;

  flags = pchip->read_register( pchip->vbase_addr + SJAMOD) & 
    ( MOD_LOM | MOD_STM | MOD_AFM | MOD_SM );
  pchip->write_register( flags | MOD_RM, pchip->vbase_addr + SJAMOD );

  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// sja1000p_remote_request

int sja1000p_remote_request( struct chip_t *pchip )
{
  CANMSG("sja1000p_remote_request not implemented\n");
  return -ENOSYS;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_standard_mask

int sja1000p_standard_mask( struct chip_t *pchip, 
			    u16 code,
			    u16 mask)
{
  CANMSG("sja1000p_standard_mask not implemented\n");
  return -ENOSYS;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_clear_objects

int sja1000p_clear_objects( struct chip_t *pchip )
{
  CANMSG("sja1000p_clear_objects not implemented\n");
  return -ENOSYS;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_config_irqs

int sja1000p_config_irqs( struct chip_t *pchip, u16 irqs )
{
  CANMSG("sja1000p_config_irqs not implemented\n");
  return -ENOSYS;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000p_irq_handler

void sja1000p_irq_handler( int irq, void *dev_id, struct pt_regs *regs )
{
  int irq_register;
  struct chip_t *pchip = (struct chip_t *)dev_id;
  //struct canfifo_t *pfifo = pchip->pmsgobj[ 0 ]->pfifo;

  //put_reg = pchip->write_register;
  //get_reg = pchip->read_register;

  irq_register = pchip->read_register( pchip->vbase_addr + SJAIR );

  //	DEBUGMSG( "sja1000_irq_handler: SJAIR:%02x\n",irq_register);
  //	DEBUGMSG( "sja1000_irq_handler: SJASR:%02x\n",
  //		  pchip->read_register( pchip->vbase_addr + SJASR ) );

  if ( ( irq_register & ( IR_BEI|IR_EPI|IR_DOI|IR_EI|IR_TI|IR_RI ) ) == 0 ) {
    return;
  }

  if ( ( irq_register & IR_RI ) != 0 ) {

    sja1000p_read( pchip );
    pchip->rv = 0;

    if ( waitqueue_active( &pchip->fifo.readq ) ) {
      wake_up_interruptible( &pchip->fifo.readq );
    }
  }
  if ( ( irq_register & IR_TI) != 0 ) {
    pchip->rv = 0;
    if ( waitqueue_active( &pchip->fifo.writeq ) ) {
      wake_up_interruptible( &pchip->fifo.writeq );
    }
  }
  if ( ( irq_register & (IR_EI|IR_BEI|IR_EPI|IR_DOI)) != 0 ) { 

    // Some error happened
    CANMSG( "Error: status register: 0x%x irq_register: 0x%02x\n",
	    pchip->read_register( pchip->vbase_addr + SJASR ), irq_register );
    // FIXME: chip should be brought to usable state. 
    // Transmission cancelled if in progress.
    // Reset flag set to 0 if chip is already off the bus. Full state report
    pchip->rv = -1;

    if ( waitqueue_active( &pchip->fifo.writeq ) )
      wake_up_interruptible( &pchip->fifo.writeq );

    if (waitqueue_active( &pchip->fifo.readq ) )
      wake_up_interruptible( &pchip->fifo.readq );
  }

  return;
}










