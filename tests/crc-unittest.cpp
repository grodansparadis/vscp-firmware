#include <cstring>
#include <gtest/gtest.h>
#include <crc.h>

TEST(_crc, crc_ccitt_check_value_slow)
{
  const unsigned char message[] = "123456789";
  const crc value               = crcSlow(message, 9);
  ASSERT_EQ(static_cast<crc>(CHECK_VALUE), value);
}

TEST(_crc, crc_ccitt_check_value_fast)
{
  const unsigned char message[] = "123456789";
  crcInit();
  const crc value = crcFast(message, 9);
  ASSERT_EQ(static_cast<crc>(CHECK_VALUE), value);
}

TEST(_crc, crc_fast_matches_slow)
{
  const unsigned char message[] = "VSCP CRC regression sample";
  crcInit();

  const crc slow = crcSlow(message, static_cast<int>(std::strlen(reinterpret_cast<const char*>(message))));
  const crc fast = crcFast(message, static_cast<int>(std::strlen(reinterpret_cast<const char*>(message))));

  ASSERT_EQ(slow, fast);
}

TEST(_crc, crc_empty_message)
{
  const unsigned char message[] = "";
  crcInit();

  const crc slow = crcSlow(message, 0);
  const crc fast = crcFast(message, 0);

  ASSERT_EQ(static_cast<crc>(INITIAL_REMAINDER ^ FINAL_XOR_VALUE), slow);
  ASSERT_EQ(slow, fast);
}
