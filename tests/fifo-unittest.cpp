#include <cstdint>
#include <gtest/gtest.h>
#include <fifo.h>

TEST(_fifo, init_and_free_space)
{
  uint8_t storage[8] = { 0 };
  fifo_t fifo;

  fifo_init(&fifo, storage, sizeof(storage));

  ASSERT_EQ(sizeof(storage), fifo_getFree(&fifo));
}

TEST(_fifo, write_and_read_order)
{
  uint8_t storage[8] = { 0 };
  fifo_t fifo;
  fifo_init(&fifo, storage, sizeof(storage));

  const uint8_t data[] = { 0x11, 0x22, 0x33, 0x44 };
  uint8_t out[4]       = { 0 };

  ASSERT_EQ(4u, fifo_write(&fifo, data, sizeof(data)));
  ASSERT_EQ(4u, fifo_read(&fifo, out, sizeof(out)));
  ASSERT_EQ(0x11, out[0]);
  ASSERT_EQ(0x22, out[1]);
  ASSERT_EQ(0x33, out[2]);
  ASSERT_EQ(0x44, out[3]);
}

TEST(_fifo, leaves_one_slot_empty_when_full)
{
  uint8_t storage[4] = { 0 };
  fifo_t fifo;
  fifo_init(&fifo, storage, sizeof(storage));

  const uint8_t data[] = { 1, 2, 3, 4 };

  ASSERT_EQ(3u, fifo_write(&fifo, data, sizeof(data)));
  ASSERT_EQ(1u, fifo_getFree(&fifo));
}

TEST(_fifo, wraparound_write_read)
{
  uint8_t storage[4] = { 0 };
  fifo_t fifo;
  fifo_init(&fifo, storage, sizeof(storage));

  const uint8_t first[]  = { 0xAA, 0xBB, 0xCC };
  uint8_t first_out[2]   = { 0 };
  const uint8_t second[] = { 0xDD, 0xEE };
  uint8_t out[3]         = { 0 };

  ASSERT_EQ(3u, fifo_write(&fifo, first, sizeof(first)));
  ASSERT_EQ(2u, fifo_read(&fifo, first_out, sizeof(first_out)));
  ASSERT_EQ(2u, fifo_write(&fifo, second, sizeof(second)));
  ASSERT_EQ(3u, fifo_read(&fifo, out, sizeof(out)));

  ASSERT_EQ(0xCC, out[0]);
  ASSERT_EQ(0xDD, out[1]);
  ASSERT_EQ(0xEE, out[2]);
}

TEST(_fifo, read_from_empty_returns_zero)
{
  uint8_t storage[4] = { 0 };
  fifo_t fifo;
  fifo_init(&fifo, storage, sizeof(storage));

  uint8_t out[2] = { 0 };
  ASSERT_EQ(0u, fifo_read(&fifo, out, sizeof(out)));
}
