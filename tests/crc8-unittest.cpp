#include <gtest/gtest.h>
#include <crc8.h>

TEST(_crc8, crc8_standard_vector_123456789)
{
  unsigned char crc = 0x00;
  const unsigned char message[] = "123456789";

  init_crc8();
  for (int i = 0; i < 9; ++i) {
    crc8(&crc, message[i]);
  }

  ASSERT_EQ(0xF4, crc);
}

TEST(_crc8, crc8_incremental_and_reinit)
{
  unsigned char crc = 0x00;

  init_crc8();
  crc8(&crc, static_cast<unsigned char>('A'));
  crc8(&crc, static_cast<unsigned char>('B'));

  unsigned char expected = crc;

  init_crc8();
  crc = 0x00;
  crc8(&crc, static_cast<unsigned char>('A'));
  crc8(&crc, static_cast<unsigned char>('B'));

  ASSERT_EQ(expected, crc);
}
