/* 
 * sja1000.c
 * Linux CAN-bus device driver.
 * Written by Arnaud Westenberg email:arnaud@wanadoo.nl
 * This software is released under the GPL-License.
 */

#include <linux/delay.h>
#include <asm/irq.h>
#include "sysdep.h"
#include "candrv.h"
#include "sja1000.h"

///////////////////////////////////////////////////////////////////////////////
// sja1000_enable_configuration

int sja1000_enable_configuration( struct chip_t *pchip )
{
  int i=0;
  unsigned flags;

  disable_irq( pchip->irq );
  flags = pchip->read_register( pchip->vbase_addr + SJACR );

  while ( ( !(flags & CR_RR) ) && (i<=10) ) {
    pchip->write_register( flags | CR_RR, pchip->vbase_addr + SJACR );
    udelay(100 );
    i++;
    flags = pchip->read_register( pchip->vbase_addr + SJACR );    
  }

  if ( i>=10 ) {
    CANMSG("Reset error\n");
    enable_irq( pchip->irq );
    return -ENODEV;
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_disable_configuration

int sja1000_disable_configuration( struct chip_t *pchip )
{
  int i=0;
  unsigned flags;

  flags = pchip->read_register( pchip->vbase_addr + SJACR );

  while ( (flags & CR_RR) && (i<=10) ) {
    pchip->write_register( flags & (CR_RIE|CR_TIE|CR_EIE|CR_OIE ), 
			   pchip->vbase_addr + SJACR );
    udelay( 100 );
    i++;
    flags = pchip->read_register( pchip->vbase_addr + SJACR );
  }

  if ( i>=10 ) {
    CANMSG("Error leaving reset status\n");
    return -ENODEV;
  }

  enable_irq( pchip->irq );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_disable_configuration

int sja1000_chip_config( struct chip_t *pchip )
{
  if ( sja1000_enable_configuration( pchip ) )
    return -ENODEV;

  // Set mode, clock out, comparator 
  pchip->write_register( pchip->sja_cdr_reg, pchip->vbase_addr + SJACDR ); 

  // Set driver output configuration 
  pchip->write_register( pchip->sja_ocr_reg, pchip->vbase_addr + SJAOCR ); 

  if ( sja1000_standard_mask( pchip, 0x0000, 0xffff ) )
    return -ENODEV;
	
  if (!baudrate)
    baudrate=1000;

  if ( sja1000_baud_rate( pchip, 1000*baudrate, pchip->clock, 0, 75, 0 ) )
    return -ENODEV;

  // Enable hardware interrupts 
  pchip->write_register( ( CR_RIE | CR_TIE | CR_EIE | CR_OIE ), 
			 pchip->vbase_addr + SJACR ); 

  sja1000_disable_configuration( pchip );
	
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_disable_configuration

int sja1000_standard_mask( struct chip_t *pchip, 
			   u16 code, 
			   u16 mask)
{
  u8 write_code, write_mask;

  if ( sja1000_enable_configuration( pchip ) )
    return -ENODEV;

  /* The acceptance code bits (SJAACR bits 0-7) and the eight most 
   * significant bits of the message identifier (id.10 to id.3) must be
   * equal to those bit positions which are marked relevant by the 
   * acceptance mask bits (SJAAMR bits 0-7).
   * (id.10 to id.3) = (SJAACR.7 to SJAACR.0) v (SJAAMR.7 to SJAAMR.0)
   * (Taken from Philips sja1000 Data Sheet)
   */
  write_code = (u8) code >> 3;
  write_mask = (u8) mask >> 3;
	
  pchip->write_register( write_code, pchip->vbase_addr + SJAACR );
  pchip->write_register( write_mask, pchip->vbase_addr + SJAAMR );

  DEBUGMSG( "Setting acceptance code to 0x%lx\n",
	    (unsigned long)code);
  DEBUGMSG( "Setting acceptance mask to 0x%lx\n",
	    (unsigned long)mask);

  sja1000_disable_configuration( pchip );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_baud_rate

/* Set communication parameters.
 * param rate baud rate in Hz
 * param clock frequency of sja1000 clock in Hz (ISA osc is 14318000)
 * param sjw synchronization jump width (0-3) prescaled clock cycles
 * param sampl_pt sample point in % (0-100) sets (TSEG1+2)/(TSEG1+TSEG2+3) 
 * ratio param flags fields BTR1_SAM, OCMODE, OCPOL, OCTP, OCTN, CLK_OFF, CBP
 */

int sja1000_baud_rate( struct chip_t *pchip, 
		       u32 rate, 
		       u32 clock, 
		       u32 sjw,
		       u32 sampl_pt, 
		       u32 flags)
{
  int best_error = 1000000000, error;
  int best_tseg=0, best_brp=0, best_rate=0, brp=0;
  int tseg=0, tseg1=0, tseg2=0;
	
  if ( sja1000_enable_configuration( pchip ) )
    return -ENODEV;

  clock /=2;

  /* tseg even = round down, odd = round up */
  for (tseg=(0+0+2)*2; tseg<=(MAX_TSEG2+MAX_TSEG1+2)*2+1; tseg++) {
    brp = clock/((1+tseg/2)*rate)+tseg%2;
    if (brp == 0 || brp > 64)
      continue;
    error = rate - clock/(brp*(1+tseg/2));
    if (error < 0)
      error = -error;
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
  if (tseg2 < 0)
    tseg2 = 0;
  if (tseg2 > MAX_TSEG2)
    tseg2 = MAX_TSEG2;
  tseg1 = best_tseg-tseg2-2;
  if (tseg1 > MAX_TSEG1) {
    tseg1 = MAX_TSEG1;
    tseg2 = best_tseg-tseg1-2;
  }

  DEBUGMSG("Setting %d bps.\n", best_rate);
  DEBUGMSG("brp=%d, best_tseg=%d, tseg1=%d, tseg2=%d, sampl_pt=%d\n",
	   best_brp, best_tseg, tseg1, tseg2,
	   (100*(best_tseg-tseg2)/(best_tseg+1)));


  pchip->write_register( sjw<<6 | best_brp, pchip->vbase_addr + SJABTR0 );
  pchip->write_register( ((flags & BTR1_SAM) != 0)<<7 | tseg2<<4 | tseg1,
		 pchip->vbase_addr + SJABTR1);
  //	can_write_reg( pchip, OCR_MODE_NORMAL | OCR_TX0_LH | OCR_TX1_ZZ, 
  //                   SJAOCR);
  /* BASIC mode, bypass input comparator */
  //	pchip->write_register( CDR_CBP| /* CDR_CLK_OFF | */ 7, 
  // pchip->vbase_addr + SJACDR);

  sja1000_disable_configuration( pchip );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_pre_read_config
//

int sja1000_pre_read_config( struct chip_t *pchip )
{
  int i;
  int id;
  i = pchip->read_register( pchip->vbase_addr + SJASR );
	
  if ( !( i&SR_RBS ) ) {
    //Temp
    for ( i=0; i<0x20; i++ ) {
      CANMSG("0x%x is 0x%x\n",
	     i,
	     pchip->read_register( pchip->vbase_addr + i ) );
    }
    return 0;
  }
  sja1000_start_chip( pchip );

  // disable interrupts for a moment
  pchip->write_register( 0, pchip->vbase_addr + SJACR); 
  
  // TODO: this would be best sja1000_irq_read_handler( pchip );
  // now just duplicate the code.
  
  do {
    id = ( pchip->read_register( pchip->vbase_addr + 
				 SJARXID1)<<8) + 
      pchip->read_register( pchip->vbase_addr + SJARXID0 );
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].length = 
      (id>>8) & 0x0f;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].id = id>>5;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].flags = 
      id&ID0_RTR ? MSG_RTR : 0;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].timestamp = 
      0;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].cob = 0;
    for (i=0; i < ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].length; i++) {
      ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].data[ i ] = 
	pchip->read_register( pchip->vbase_addr + SJARXDAT0 + i);
    }
    pchip->fifo.head++;
    if ( pchip->fifo.head == MAX_BUF_LENGTH -1)
      pchip->fifo.head = 0;
    pchip->write_register( CMR_RRB, pchip->vbase_addr + SJACMR );
  } while ( pchip->read_register( pchip->vbase_addr + SJASR ) & SR_RBS );

  // enable interrupts
  pchip->write_register( CR_OIE | CR_EIE | CR_TIE | CR_RIE, 
			 pchip->vbase_addr + SJACR );

  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_pre_write_config

#define MAX_TRANSMIT_WAIT_LOOPS 200

int sja1000_pre_write_config( struct chip_t *pchip, 
			      struct canmsg_t *pmsg )
{
  int i=0, id=0;

  //sja1000 goes automatically into reset mode on errors
  sja1000_start_chip( pchip ); 

  /* Wait until Transmit Buffer Status is released */
  while ( !( pchip->read_register( pchip->vbase_addr + SJASR) & SR_TBS) && 
	  i++<MAX_TRANSMIT_WAIT_LOOPS) {
    udelay(i);
  }
	
  if ( ! ( pchip->read_register( pchip->vbase_addr + SJASR ) & SR_TBS ) ) {
    CANMSG("Transmit timed out, cancelling\n");
    pchip->write_register( CMR_AT, pchip->vbase_addr + SJACMR);
    i=0;
    while ( !( pchip->read_register( pchip->vbase_addr + SJASR) & SR_TBS) &&
	    i++<MAX_TRANSMIT_WAIT_LOOPS) {
      udelay(i);
    }
    if ( !( pchip->read_register( pchip->vbase_addr +  SJASR) & SR_TBS)) {
      CANMSG("Could not cancel, please reset\n");
      return -EIO;
    }
  }

  id = (pmsg->id<<5) | ((pmsg->flags&MSG_RTR)?ID0_RTR:0) | pmsg->length;

  pchip->write_register( id>>8, pchip->vbase_addr + SJATXID1);
  pchip->write_register( id & 0xff, pchip->vbase_addr + SJATXID0);

  for (i=0; i<pmsg->length; i++)
    pchip->write_register( pmsg->data[i], pchip->vbase_addr + SJATXDAT0+i);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_send_msg

int sja1000_send_msg( struct chip_t *pchip, int bRtr )
{
  pchip->write_register( CMR_TR, pchip->vbase_addr + SJACMR);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_check_tx_stat

int sja1000_check_tx_stat( struct chip_t *pchip )
{
  if ( pchip->read_register( pchip->vbase_addr + SJASR ) & SR_TCS)
    return 0;
  else
    return 1;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_send_msg

int sja1000_set_btregs( struct chip_t *pchip, 
			unsigned short btr0, 
			unsigned short btr1 )
{
  if (sja1000_enable_configuration( pchip ) ) {
    return -ENODEV;
  }

  pchip->write_register( btr0, pchip->vbase_addr + SJABTR0);
  pchip->write_register( btr1, pchip->vbase_addr + SJABTR1 );

  sja1000_disable_configuration( pchip );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_start_chip

int sja1000_start_chip( struct chip_t *pchip )
{
  unsigned short flags = 0;

  flags = pchip->read_register( pchip->vbase_addr + SJACR) & 
    ( CR_RIE|CR_TIE|CR_EIE|CR_OIE);
  pchip->write_register( flags, pchip->vbase_addr + SJACR);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_stop_chip

int sja1000_stop_chip( struct chip_t *pchip )
{
  unsigned short flags = 0;

  flags = pchip->read_register( pchip->vbase_addr + SJACR) & 
    (CR_RIE|CR_TIE|CR_EIE|CR_OIE);
  pchip->write_register( flags | CR_RR, pchip->vbase_addr + SJACR );

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_remote_request

int sja1000_remote_request( struct chip_t *pchip )
{
  CANMSG("sja1000_remote_request not implemented\n");
  return -ENOSYS;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_extended_mask

int sja1000_extended_mask( struct chip_t *pchip, 
			   u32 code,
			   u32 mask)
{
  CANMSG("sja1000_extended_mask not implemented\n");
  return -ENOSYS;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_clear_objects

int sja1000_clear_objects( struct chip_t *pchip )
{
  CANMSG("sja1000_clear_objects not implemented\n");
  return -ENOSYS;
}

///////////////////////////////////////////////////////////////////////////////
// sja1000_config_irqs

int sja1000_config_irqs( struct chip_t *pchip, u16 irqs )
{
  CANMSG("sja1000_config_irqs not implemented\n");
  return -ENOSYS;
}





