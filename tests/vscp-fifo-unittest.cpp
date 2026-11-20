#include <cstdlib>
#include <gtest/gtest.h>
#include "vscp-fifo.h"
#include "vscp-firmware-helper.h"

TEST(_vscp_fifo, init_and_capacity)
{
  vscp_fifo_t fifo;
  vscp_fifo_init(&fifo, 3);

  ASSERT_EQ(3u, vscp_fifo_getFree(&fifo));

  std::free(fifo.buf);
}

TEST(_vscp_fifo, write_read_order)
{
  vscp_fifo_t fifo;
  vscp_fifo_init(&fifo, 3);

  vscp_event_t e1 = {};
  vscp_event_t e2 = {};

  ASSERT_EQ(1u, vscp_fifo_write(&fifo, &e1));
  ASSERT_EQ(1u, vscp_fifo_write(&fifo, &e2));

  vscpEvent* out = nullptr;
  ASSERT_EQ(1u, vscp_fifo_read(&fifo, &out));
  ASSERT_EQ(&e1, out);

  ASSERT_EQ(1u, vscp_fifo_read(&fifo, &out));
  ASSERT_EQ(&e2, out);

  ASSERT_EQ(0u, vscp_fifo_read(&fifo, &out));

  std::free(fifo.buf);
}

TEST(_vscp_fifo, full_condition_and_free_count)
{
  vscp_fifo_t fifo;
  vscp_fifo_init(&fifo, 2);

  vscp_event_t e1 = {};
  vscp_event_t e2 = {};
  vscp_event_t e3 = {};

  ASSERT_EQ(2u, vscp_fifo_getFree(&fifo));
  ASSERT_EQ(1u, vscp_fifo_write(&fifo, &e1));
  ASSERT_EQ(1u, vscp_fifo_getFree(&fifo));
  ASSERT_EQ(1u, vscp_fifo_write(&fifo, &e2));
  ASSERT_EQ(0u, vscp_fifo_getFree(&fifo));

  ASSERT_EQ(0u, vscp_fifo_write(&fifo, &e3));

  vscpEvent* out = nullptr;
  ASSERT_EQ(1u, vscp_fifo_read(&fifo, &out));
  ASSERT_EQ(&e1, out);
  ASSERT_EQ(1u, vscp_fifo_getFree(&fifo));

  std::free(fifo.buf);
}

TEST(_vscp_fifo, wraparound_preserves_fifo_order)
{
  vscp_fifo_t fifo;
  vscp_fifo_init(&fifo, 2);

  vscp_event_t e1 = {};
  vscp_event_t e2 = {};
  vscp_event_t e3 = {};
  vscpEvent* out = nullptr;

  ASSERT_EQ(1u, vscp_fifo_write(&fifo, &e1));
  ASSERT_EQ(1u, vscp_fifo_write(&fifo, &e2));
  ASSERT_EQ(1u, vscp_fifo_read(&fifo, &out));
  ASSERT_EQ(&e1, out);

  ASSERT_EQ(1u, vscp_fifo_write(&fifo, &e3));

  ASSERT_EQ(1u, vscp_fifo_read(&fifo, &out));
  ASSERT_EQ(&e2, out);
  ASSERT_EQ(1u, vscp_fifo_read(&fifo, &out));
  ASSERT_EQ(&e3, out);

  std::free(fifo.buf);
}

TEST(_vscp_fifo, clear_resets_fifo_state)
{
  vscp_fifo_t fifo;
  vscp_fifo_init(&fifo, 3);

  // Clear deletes the queued events (and their data)
  vscp_event_t *e1 = vscp_fwhlp_newEvent();
  vscp_event_t *e2 = vscp_fwhlp_newEvent();
  ASSERT_NE(nullptr, e1);
  ASSERT_NE(nullptr, e2);
  e2->sizeData = 4;
  e2->pdata    = (uint8_t *) std::malloc(4);

  ASSERT_EQ(1u, vscp_fifo_write(&fifo, e1));
  ASSERT_EQ(1u, vscp_fifo_write(&fifo, e2));

  vscp_fifo_clear(&fifo);

  ASSERT_EQ(3u, vscp_fifo_getFree(&fifo));

  vscpEvent* out = nullptr;
  ASSERT_EQ(0u, vscp_fifo_read(&fifo, &out));

  std::free(fifo.buf);
}
