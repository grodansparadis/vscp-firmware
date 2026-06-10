/*!
  @brief Header for fifo implementation for VSCP events
  @file vscp_fifo.h

  Thread safety (tread/write threads) is achieved by having an extra pos and use lookahead method
  https://embeddedartistry.com/blog/2017/05/17/creating-a-circular-buffer-in-c-and-c/
*/

/*
  use
  =======================================================================
  vcp_fifo_t is a simple circular buffer for pointers to vscp_event_t 
  structures.  It is not thread-safe on its own, but can be used in a 
  single-producer/single-consumer scenario without additional locking 
  if the producer and consumer are in separate threads.
  
  vscp_fifo_t g_vscp_rx_fifo;

  Init  
  ----
  vscp_fifo_init(&g_vscp_rx_fifo, VSCP_FIFO_SIZE);  
  
  Add event
  --------
  vscp_event_t *pev = vscp_fwhlp_newEvent();
  // fill pev fields ...
  vscp_fifo_write(&g_vscp_rx_fifo, pev);  // returns 0 if full  

  consume
  -------
  vscp_event_t *pev = NULL;
  while (vscp_fifo_read(&g_vscp_rx_fifo, &pev)) {
    // process pev ...
    vscp_fwhlp_deleteEvent(&pev);
  }

  deinit (without clearing)
  ------
  vscp_fifo_deinit(&g_vscp_rx_fifo);  // drains remaining events, frees buffer
*/


/*! 
  Define to prevent recursive inclusion -------------------------------------
*/
#ifndef __VSCP_FIFO_H
#define __VSCP_FIFO_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*! 
  Includes ------------------------------------------------------------------
*/
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <vscp.h>

/*!
  Fifo structure 
*/
typedef struct {
	vscp_event_t **buf;
	size_t head;
	size_t tail;
	size_t size;
} vscp_fifo_t;


/*! 
  Prototypes 
*/

/*!
  @brief Initialize the fifo
  @param f Pointer to fifo structure
  @param size Size of fifo    
*/
void vscp_fifo_init(vscp_fifo_t *f, uint16_t size);

/*!
  @brief Clear the fifo
  @param f Pointer to fifo structure
*/
void vscp_fifo_clear(vscp_fifo_t *f);

/*!
  @brief Read a VSCP event from the fifo
  @param f Pointer to fifo structure
  @param pev Pointer to pointer to event structure
  @return Non zero if successful
*/
size_t vscp_fifo_read(vscp_fifo_t *f, vscp_event_t **pev);

/*!
  @brief Put a VSCP event in the fifo
  @param f Pointer to fifo structure
  @param pev Pointer to VSCP event
  @return Non zero if successful
*/
size_t vscp_fifo_write(vscp_fifo_t *f, vscp_event_t *pev);

/*!
  @brief Get the number of events in the fifo
  @param f Pointer to fifo structure
  @return Number of events in the fifo
*/
size_t vscp_fifo_getFree(vscp_fifo_t *f);

/*!
 * @brief  Drain and destroy a VSCP event FIFO, freeing every event still
 *         queued inside it, then release the FIFO's internal buffer.
 *
 * Safe to call even when the FIFO is already empty.  After this call the
 * vscp_fifo_t object must not be used without a new vscp_fifo_init().
 *
 * @param  f  Pointer to the FIFO to destroy.
 */
void
vscp_fifo_deinit(vscp_fifo_t *f);

#ifdef __cplusplus
}
#endif

#endif /* __VSCP_FIFO_H */
