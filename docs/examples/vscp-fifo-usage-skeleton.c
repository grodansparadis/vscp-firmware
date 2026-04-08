#include <stdlib.h>
#include <string.h>

#include <vscp-fifo.h>
#include <vscp-firmware-helper.h>

static vscp_event_t *new_test_event(uint16_t vscp_class, uint16_t vscp_type)
{
  vscp_event_t *pev = vscp_fwhlp_newEvent();
  if (NULL == pev) {
    return NULL;
  }

  memset(pev, 0, sizeof(vscpEvent));
  pev->vscp_class = vscp_class;
  pev->vscp_type = vscp_type;
  pev->sizeData = 0;
  pev->pdata = NULL;

  return pev;
}

int main(void)
{
  vscp_fifo_t queue;
  vscp_event_t *rx = NULL;

  vscp_fifo_init(&queue, 8);

  vscp_event_t *ev1 = new_test_event(10, 6);
  vscp_event_t *ev2 = new_test_event(20, 8);

  if ((NULL != ev1) && (NULL != ev2)) {
    (void)vscp_fifo_write(&queue, ev1);
    (void)vscp_fifo_write(&queue, ev2);
  }

  while (vscp_fifo_read(&queue, &rx)) {
    if (NULL != rx) {
      vscp_fwhlp_deleteEvent(&rx);
    }
  }

  vscp_fifo_clear(&queue);
  free(queue.buf);
  queue.buf = NULL;

  return 0;
}
