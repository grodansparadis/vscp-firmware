/*!
  @brief fifo implementation
  @file vscp_fifo.c
*/

#include "vscp-fifo.h"

///////////////////////////////////////////////////////////////////////////////
// vscp_fifo_init
//

void
vscp_fifo_init(vscp_fifo_t* f, uint16_t size)
{
  f->head = 0;
  f->tail = 0;
  f->size = size + 1; // One pos used for full signaling
  f->buf  = malloc(f->size * sizeof(struct VscpEvent*));
  memset(f->buf, 0, f->size * sizeof(struct VscpEvent*));
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fifo_clear
//

void
vscp_fifo_clear(vscp_fifo_t* f)
{
  f->head = 0;
  f->tail = 0;
  for (int i = 0; i < f->size; i++) {
    if (NULL != f->buf[i]) {
      vscpEvent* pev = f->buf[i];
      if (NULL == pev->pdata) {
        free(pev->pdata);
        pev->pdata = NULL;
      }
      f->buf[i] = NULL;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fifo_read
//

size_t
vscp_fifo_read(vscp_fifo_t* f, vscpEvent** pev)
{
  if (f->tail != f->head) {            // see if any data is available
    *pev            = f->buf[f->tail]; // grab the event from the fifo
    f->buf[f->tail] = NULL;            // clear the pointer
    f->tail++;                         // increment the tail
    if (f->tail == f->size) {          // check for wrap-around
      f->tail = 0;
    }
  }
  else {
    return 0; // number of events read
  }

  return 1; // number of events read
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fifo_write
//

size_t
vscp_fifo_write(vscp_fifo_t* f, vscpEvent* pev)
{
  uint32_t i;

  // first check to see if there is space in the fifo
  if (((f->head + 1) % f->size) == f->tail) {
    return 0; // no space levt in the fifo
  }
  else {
    f->buf[f->head] = pev;    // write the event to the fifo
    f->head++;                // increment the head
    if (f->head == f->size) { // check for wrap-around
      f->head = 0;
    }
  }

  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// vscp_fifo_getFree
//

size_t
vscp_fifo_getFree(vscp_fifo_t* f)
{
  if (f->head < f->tail) {
    return (f->tail - f->head - 1);
  }
  else {
    return (f->size + (f->tail - f->head) - 1);
  }
}


