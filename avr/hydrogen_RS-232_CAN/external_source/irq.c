/* irq.c
 * Linux CAN-bus device driver.
 * Written by Arnaud Westenberg email:arnaud@wanadoo.nl
 * This software is released under the GPL-License.
 * Version 0.7  6 Aug 2001
 */

#include "sysdep.h"
#include "candrv.h"
#include "irq.h"
#include "i82527.h"
#include "sja1000.h"

// prototypes
inline void i82527_irq_read_handler( struct chip_t *pchip, 
				     u32 msgid  );

inline void i82527_irq_write_handler( struct chip_t *pchip );

void i82527_irq_rtr_handler( struct chip_t *pchip,  
			     u32 msgid );

void sja1000_irq_read_handler( struct chip_t *pchip );
void sja1000_irq_write_handler( struct chip_t *pchip );


///////////////////////////////////////////////////////////////////////////////
// i82527_irq_write_handler
//
// Send a message from the output fifo ( if any ).
//

inline void i82527_irq_write_handler( struct chip_t *pchip )
{  
  pchip->write_register( ( MVAL_RES | TXIE_RES | RXIE_RES | INTPD_RES ), 
			 pchip->obj_base_addr_tx + iMSGCTL0 );
  
  // Get ready to send next message
  spin_lock( &pchip->spwlock );
  
  if ( pchip->fifo.txrp == pchip->fifo.txwp ) { 
    // Nothing to write
    spin_unlock( &pchip->spwlock );
    pchip->fifo.tx_in_progress = 0;
    pchip->rv = 0;
    wake_up_interruptible( &pchip->fifo.writeq );
    return;
  }
  
  // We have stuff to write... Config. chip for write
  if ( pchip->pre_write_config( pchip,
				( struct canmsg_t *)pchip->fifo.txrp ) ) {
    // Failed...
    pchip->rv = -1;
    spin_unlock( &pchip->spwlock );
    wake_up_interruptible( &pchip->fifo.writeq );
    return;
  }

  // Send the message
  if ( pchip->send_msg( pchip, ( ( struct canmsg_t *)pchip->fifo.txrp )->flags & MSG_RTR ) ) {
    // Failed...
    spin_unlock( &pchip->spwlock );
    DEBUGMSG("82527_irq_handler: Unable to send message\n" );
    pchip->rv = -1;   
  }
  else {
    // Another message sent
    pchip->stat.cntTxPkt++;
    pchip->stat.cntTxData += (( struct canmsg_t *)pchip->fifo.txrp )->length;
    pchip->fifo.txrp += sizeof( struct canmsg_t );
    if ( pchip->fifo.txrp >= ( pchip->fifo.ptxbuf + pchip->fifo.txsize ) ) {
      pchip->fifo.txrp = pchip->fifo.ptxbuf; // wrapped 
    }
  }
  
  // Wake up any waiting writer
  spin_unlock( &pchip->spwlock );
  wake_up_interruptible( &pchip->fifo.writeq );
  return;
}

///////////////////////////////////////////////////////////////////////////////
// i82527_irq_read_handler
//
// Message received form the line. Write it in the input fifo.
//

inline void i82527_irq_read_handler( struct chip_t *pchip, 
				     u32 msgid  )
{
  int i=0, bDataAvail=1 ;
  u16 spacefree = 0;

  while ( bDataAvail ) {    
    
    spin_lock( &pchip->sprlock );
    
    if ( pchip->fifo.rxrp == pchip->fifo.rxwp ) {
      spacefree = pchip->fifo.rxsize;
    }
    else {
      spacefree = ( ( pchip->fifo.rxrp + pchip->fifo.rxsize - pchip->fifo.rxwp ) % 
		    pchip->fifo.rxsize );
    }

    if ( spacefree <= sizeof( struct canmsg_t ) ) {
      wake_up_interruptible( &pchip->fifo.readq );
      CANMSG("Fifo full - Message lost!\n");
      pchip->stat.cntRxFifoOvr++;
      pchip->rv = -1;   
    }
    else {

      // Message length
      ( ( struct canmsg_t *)pchip->fifo.rxwp )->length = 
	( pchip->read_register( pchip->obj_base_addr_rx + 
				iMSGCFG) & 0xf0 ) >> 4;
      
      // Message id
      ( ( struct canmsg_t *)pchip->fifo.rxwp )->id = msgid;
      
      // Fetch message bytes
      for ( i=0; i < ( ( struct canmsg_t *)pchip->fifo.rxwp )->length; i++ ) {
	( ( struct canmsg_t *)pchip->fifo.rxwp )->data[ i ] = 
	  pchip->read_register( pchip->obj_base_addr_rx + 
				iMSGDAT0 + i );
      }
      
      // One message has been read
      pchip->fifo.rxwp += sizeof( struct canmsg_t );
      if ( pchip->fifo.rxwp >= ( pchip->fifo.prxbuf + pchip->fifo.rxsize ) ) {
	pchip->fifo.rxwp = pchip->fifo.prxbuf; // wrapped 
      }

      // Another received packet
      pchip->stat.cntRxPkt++;

      // Add databytes read to statistics block
      pchip->stat.cntRxData += ( pchip->read_register( pchip->obj_base_addr_rx + 
						       iMSGCFG) & 0xf0 ) >> 4;
    }
    
    spin_unlock( &pchip->sprlock );
    
    // Make the chip ready to receive the next message
    pchip->write_register( ( MVAL_SET | TXIE_RES | RXIE_SET | INTPD_RES ), 
			   pchip->obj_base_addr_rx + iMSGCTL0 );
    
    pchip->write_register( ( RMPD_RES | TXRQ_RES | MLST_RES |  NEWD_RES ), 
			   pchip->obj_base_addr_rx + iMSGCTL1 );
    
    // Check if new data arrived
    if ( !( ( bDataAvail = pchip->read_register( pchip->obj_base_addr_rx + 
						 iMSGCTL1 ) ) & NEWD_SET ) ) {
      break;
    }
    
    if ( bDataAvail & MLST_SET ) {
      CANMSG("82527 fifo full: Message lost!\n");
    }

  } // while

  // Wake up any waiting reader
  wake_up_interruptible( &pchip->fifo.readq );
}

///////////////////////////////////////////////////////////////////////////////
// i82527_irq_handler
//

void i82527_irq_handler( int irq, void *dev_id, struct pt_regs *regs )
{
  //struct rtr_id *rtr_search = pghw->prtr_queue;
  struct chip_t *pchip = (struct chip_t *)dev_id;
  int id0=0, id1=0, id2=0, id3=0;
  u8 idxobj = 0, irqreg = 0;
  u32 msgid = 0;
  unsigned short flags = 0;
  
  irqreg = pchip->read_register( pchip->vbase_addr + iIRQ );  
  pchip->chip_status = pchip->read_register( pchip->vbase_addr + iSTAT );
  
  // Reset status register
  pchip->write_register( 0, pchip->vbase_addr + iSTAT );

  DEBUGMSG( "IRQ handler: addr=%lx irqreg=0x%x status=0x%x\n", 
  	    (long)( pchip->vbase_addr + iSTAT), 
  	    irqreg, 
  	    pchip->chip_status );

  while ( irqreg ) {
    
    // Handle change in status register
    if ( irqreg == 0x01 ) {
      DEBUGMSG( "Status register: 0x%x\n", pchip->chip_status );

      if ( pchip->chip_status & iSTAT_WAKE ) {
	pchip->stat.cntWakeup++;
      }
      
      if ( pchip->chip_status & iSTAT_WARN ) {
	// There is an abnormal # of errors
	pchip->stat.cntWarnings++;
	CANMSG("82527_irq_handler: Bus Warning\n" );
      }
      
      if ( pchip->chip_status & iSTAT_BOFF ) {
	// We have a bus off condition
	pchip->stat.cntBusOff++;
	pchip->fifo.tx_in_progress = 0;	
	flags = ( pchip->read_register( pchip->vbase_addr + iCTL ) | 
		  ( iCTL_IE | iCTL_SIE | iCTL_EIE ) );
	flags &= ~iCTL_INI; // Reset init flag
	
	pchip->write_register( flags, pchip->vbase_addr + iCTL );    
	CANMSG("82527_irq_handler: Bus Off\n" );
      }
      
      // Errors to statistics
      switch( pchip->chip_status & 0x07 ) {
	
      case 0: // No error
	break;
	
      case 1: // Stuff error
	pchip->stat.cntStuffErr++;
	break;
	
      case 2: // Form error 
	pchip->stat.cntFormErr++;
	break;
	
      case 3: // Ack error
	pchip->stat.cntAckErr++;
	break;
	
      case 4: // Bit 1 error
	pchip->stat.cntBit1Err++;
	break;
	
      case 5: // Bit 0 error
	pchip->stat.cntBit0Err++;
	break;
	
      case 6: // CRC error
	pchip->stat.cntCrcErr++;
	break;
	
      case 7: // unused
	break;
	
      }
      //return; // continue?
    }
    else {
      
      if ( irqreg == 0x02 ) {
	idxobj = 14;
      }
      else {
	idxobj = irqreg - 3;
      }
      
      //DEBUGMSG( "Interrupt handler: addr=%lx devid=%lx irqreq=%x status=0x%x\n", 
      //	      (unsigned long)pchip->vbase_addr + iIRQ,
      //      (unsigned long)dev_id, 
      //      irqreg,
      //      statreg );  
      
      if ( 0 == idxobj ) {
	//   * * * Transmitt * * *
	i82527_irq_write_handler( pchip ); 
      }
      else if ( 14 == idxobj ) { 
	//   * * * Receive * * *
	if ( extended ) {
	  id0 = pchip->read_register( pchip->obj_base_addr_rx + 
				      iMSGID3 );
	  id1 = pchip->read_register( pchip->obj_base_addr_rx + 
				      iMSGID2 ) << 8;
	  id2 = pchip->read_register( pchip->obj_base_addr_rx + 
				      iMSGID1 ) << 16;
	  id3 = pchip->read_register( pchip->obj_base_addr_rx + 
				      iMSGID0 ) << 24;
	  msgid = ( id0 | id1 | id2 | id3 ) >> 3;
	}
	else {
	  id0 = pchip->read_register( pchip->obj_base_addr_rx + 
				      iMSGID1 );
	  id1 = pchip->read_register( pchip->obj_base_addr_rx + 
				      iMSGID0 ) << 8;
	  msgid = ( id0 | id1 ) >> 5;
	}
	
	/*
	  spin_lock( &pghw->rtr_lock );
	  
	  while ( rtr_search != NULL ) {
	  if ( rtr_search->id == msgid ) {
	  break;
	  }
	  rtr_search = rtr_search->next;
	  }
	  
	  spin_unlock( &pghw->rtr_lock );
	*/
	
	//if ( ( rtr_search != NULL ) && (rtr_search->id == msgid ) ) {
	//i82527_irq_rtr_handler( pchip, msgid );
	//}
	//else {
	
	i82527_irq_read_handler( pchip, msgid ); 
	//}
	
      }
      else {
	//   * * * Some other strange interrupt * * *
      }
    }

    // Get irq status again
    irqreg = pchip->read_register( pchip->vbase_addr + iIRQ );
  
  } // while  

  

}

///////////////////////////////////////////////////////////////////////////////
// i82527_irq_rtr_handler
// 

void i82527_irq_rtr_handler( struct chip_t *pchip, u32 msgid   )
{
  short int i=0;
  struct rtr_id *prtr_search = pghw->prtr_queue;
  
  pchip->write_register( ( MVAL_RES | TXIE_RES | RXIE_RES | INTPD_RES ), 
	   pchip->obj_base_addr_rx + iMSGCTL0 );
  pchip->write_register( ( RMPD_RES | TXRQ_RES | MLST_RES | NEWD_RES ), 
	   pchip->obj_base_addr_rx + iMSGCTL1 );
	
  spin_lock( &pghw->rtr_lock );

  prtr_search->rtr_message->id = msgid;
  prtr_search->rtr_message->length = 
    ( pchip->read_register( pchip->obj_base_addr_rx + 
			    iMSGCFG ) & 0xf0 ) >> 4;

  for ( i=0; i<prtr_search->rtr_message->length; i++ ) {
    prtr_search->rtr_message->data[ i ] = 
      pchip->read_register( pchip->obj_base_addr_rx + 
			    iMSGDAT0 + i );
  }
	
  spin_unlock( &pgwh->rtr_lock );
  wake_up_interruptible( &prtr_search->rtr_wq );

}


///////////////////////////////////////////////////////////////////////////////
// sja1000_irq_handler
// 

void sja1000_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
  u8 irqreg = 0;
  struct chip_t *pchip = (struct chip_t *)dev_id;

  irqreg = pchip->read_register( pchip->vbase_addr + SJAIR );
  //	DEBUGMSG("sja1000_irq_handler: SJAIR:%02x\n",irq_register);
  //	DEBUGMSG("sja1000_irq_handler: SJASR:%02x\n",
  //		  pchip->read_register(pchip->chip_vbase_addr + SJASR ) );

  if ( ( irqreg & ( IR_WUI | IR_DOI | IR_EI | IR_TI | IR_RI ) ) == 0 ) {
    return;
  }

  //pchip->obj_base_addr = pchip->pmsgobj[ 0 ]->obj_base_addr;

  if ( ( irqreg & IR_RI ) != 0 ) { 
    sja1000_irq_read_handler( pchip );
  }

  if ( ( irqreg & IR_TI) != 0 ) { 
    sja1000_irq_write_handler( pchip );
  }

  if ( ( irqreg & ( IR_EI|IR_DOI ) ) != 0 ) { 
    // Some error happened
    // FIXME: chip should be brought to usable state. 
    // Transmission cancelled if in progress.
    // Reset flag set to 0 if chip is already off the bus. Full state report
    CANMSG("Error: status register: 0x%x irq_register: 0x%02x\n",
	   pchip->read_register( pchip->vbase_addr + SJASR ), 
	   irqreg );
    pchip->rv = -1;

    wake_up_interruptible( &pchip->fifo.writeq );
    wake_up_interruptible( &pchip->fifo.readq );
    
  }

  return;
}


///////////////////////////////////////////////////////////////////////////////
// sja1000_irq_read_handler
//

void sja1000_irq_read_handler( struct chip_t *pchip )
{
  int i=0, id=0, tmp=1;

  while ( tmp ) {
    id = pchip->read_register( pchip->obj_base_addr_rx + 
			       SJARXID0 ) | 
      ( pchip->read_register( pchip->obj_base_addr_rx + 
			      SJARXID1 ) << 8 );
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].length = id & 0x0f;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].flags = 
      id&ID0_RTR ? MSG_RTR : 0;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].timestamp = 0;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].cob = 0;
    ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].id = id>>5;

    for ( i=0; i < ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].length; i++) {
      ( ( struct canmsg_t *)pchip->fifo.prxbuf )[ pchip->fifo.head ].data[ i ] = 
	pchip->read_register( pchip->obj_base_addr_rx + 
			      SJARXDAT0 + i );
    }

    pchip->write_register( CMR_RRB, pchip->obj_base_addr_rx + 
			   SJACMR );

    pchip->fifo.head++;
    if ( pchip->fifo.head >= MAX_BUF_LENGTH-1 ) {
      pchip->fifo.head=0;
    }

    tmp = ( pchip->read_register( pchip->obj_base_addr_rx + 
				  SJASR ) & SR_RBS );
  }

  wake_up_interruptible( &pchip->fifo.readq );

}

///////////////////////////////////////////////////////////////////////////////
// sja1000_irq_write_handler
//

void sja1000_irq_write_handler( struct chip_t *pchip )
{
  pchip->fifo.txrp++;
  if ( pchip->fifo.txrp >= pchip->fifo.ptxbuf + MAX_BUF_LENGTH ) {
    pchip->fifo.txrp = pchip->fifo.ptxbuf;
  }

  // Check if output buffer is empty
  if ( pchip->fifo.txrp == pchip->fifo.rxwp ) { 
    pchip->fifo.tx_in_progress = 0;
    pchip->rv = 0;
    wake_up_interruptible( &pchip->fifo.writeq );
    return;
  }

  if ( pchip->pre_write_config( pchip, ( struct canmsg_t *)pchip->fifo.txrp ) ) {
    pchip->rv = -1;
    wake_up_interruptible( &pchip->fifo.writeq );
    return;
  }

  if ( pchip->send_msg( pchip, ( ( struct canmsg_t *)pchip->fifo.txrp)->flags ) ) {
      pchip->rv = -1;
      wake_up_interruptible( &pchip->fifo.writeq );
      return;
  }
}








