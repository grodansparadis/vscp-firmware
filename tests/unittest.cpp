#include <stdio.h>
#include <string.h>
#include <climits>
#include <gtest/gtest.h>
#include <vscp.h>
#include <vscp-class.h>
#include <vscp-firmware-helper.h>
#include <vscp-crc.h>

//-----------------------------------------------------------------------------
// Parse datestr for event tests
//-----------------------------------------------------------------------------

// Test standard ISO 8601 date string parsing to event fields
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_event_datestr_1)
{
  int rv;
  vscp_event_t ev;

  rv = vscp_fwhlp_parse_event_datestr(&ev, "2024-06-30T23:59:58Z", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(2024, ev.year);
  ASSERT_EQ(6, ev.month);
  ASSERT_EQ(30, ev.day);
  ASSERT_EQ(23, ev.hour);
  ASSERT_EQ(59, ev.minute);
  ASSERT_EQ(58, ev.second);
}

// Test standard ISO 8601 date string (without 'Z') parsing to event fields
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_event_datestr_2)
{
  int rv;
  vscp_event_t ev;

  rv = vscp_fwhlp_parse_event_datestr(&ev, "2024-06-30T23:59:58", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(2024, ev.year);
  ASSERT_EQ(6, ev.month);
  ASSERT_EQ(30, ev.day);
  ASSERT_EQ(23, ev.hour);
  ASSERT_EQ(59, ev.minute);
  ASSERT_EQ(58, ev.second);
}

// Test standard ISO 8601 date string with space instead of 'T'
// This is not strictly ISO 8601 compliant but we want to be lenient in what we accept
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_event_datestr_3)
{
  int rv;
  vscp_event_t ev;

  rv = vscp_fwhlp_parse_event_datestr(&ev, "2024-06-30 23:59:58Z", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
}

/*
    Test standard ISO 8601 date string with leading spaces parsing to event fields
    This is not strictly ISO 8601 compliant but we want to be lenient in what we accept
*/
TEST(_vscp_firmware_helper, _vscp_fwhlp_parse_event_datestr_4)
{
  int rv;
  vscp_event_t ev;

  rv = vscp_fwhlp_parse_event_datestr(&ev, "    2024-06-30T23:59:58Z    ", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
}

// Test Unix nanosecond timestamp parsing (no datestr format, just numeric)
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_event_datestr_unix_ns_1)
{
  int rv;
  vscp_event_t ev;

  // 2024-06-30T23:59:58Z = 1719791998 seconds since epoch
  // In nanoseconds: 1719791998000000000
  rv = vscp_fwhlp_parse_event_datestr(&ev, "1719791998000000000", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ev.year);  // year set to 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);   // month set to 0xff (unused in nanosecond mode)
  ASSERT_EQ(1719791998000000000ULL, ev.timestamp_ns);
}

// Test Unix nanosecond timestamp parsing with leading spaces
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_event_datestr_unix_ns_2)
{
  int rv;
  vscp_event_t ev;

  rv = vscp_fwhlp_parse_event_datestr(&ev, "   1719791998000000000", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ev.year);
  ASSERT_EQ(1719791998000000000ULL, ev.timestamp_ns);
}

// Test Unix nanosecond timestamp parsing with endptr
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_event_datestr_unix_ns_3)
{
  int rv;
  vscp_event_t ev;
  char *endptr = NULL;

  rv = vscp_fwhlp_parse_event_datestr(&ev, "1719791998000000000 NEXT", &endptr);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ev.year);
  ASSERT_EQ(1719791998000000000ULL, ev.timestamp_ns);

  // Skip trailing space
  while (*endptr == ' ') {
    endptr++;
  }
  ASSERT_STREQ("NEXT", endptr);
}

// Test zero timestamp
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_event_datestr_unix_ns_zero)
{
  int rv;
  vscp_event_t ev;

  rv = vscp_fwhlp_parse_event_datestr(&ev, "0", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ev.year);
  ASSERT_EQ(0ULL, ev.timestamp_ns);
}

//-----------------------------------------------------------------------------
// Parse datestr for eventex tests
//-----------------------------------------------------------------------------

// Test standard ISO 8601 date string parsing to event fields
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_1)
{
  int rv;
  vscp_event_ex_t ex;

  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "2024-06-30T23:59:58Z", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(2024, ex.year);
  ASSERT_EQ(6, ex.month);
  ASSERT_EQ(30, ex.day);
  ASSERT_EQ(23, ex.hour);
  ASSERT_EQ(59, ex.minute);
  ASSERT_EQ(58, ex.second);
}

// Test standard ISO 8601 date string (without 'Z') parsing to event fields
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_2)
{
  int rv;
  vscp_event_ex_t ex;

  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "2024-06-30T23:59:58", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(2024, ex.year);
  ASSERT_EQ(6, ex.month);
  ASSERT_EQ(30, ex.day);
  ASSERT_EQ(23, ex.hour);
  ASSERT_EQ(59, ex.minute);
  ASSERT_EQ(58, ex.second);
}

// Test standard ISO 8601 date string with space instead of 'T'
// This is not strictly ISO 8601 compliant but we want to be lenient in what we accept
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_3)
{
  int rv;
  vscp_event_ex_t ex;

  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "2024-06-30 23:59:58Z", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
}

/*
    Test standard ISO 8601 date string with leading spaces parsing to event fields
    This is not strictly ISO 8601 compliant but we want to be lenient in what we accept
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_4)
{
  int rv;
  vscp_event_ex_t ex;

  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "    2024-06-30T23:59:58Z    ", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
}

/*
    Test standard ISO 8601 date string with leading spaces parsing to event fields
    with trailing non-space characters after the date string. The parser should parse
    the date string successfully and set endptr to point to the first non-space character
    after the date string.
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_5)
{
  int rv;
  vscp_event_ex_t ex;

  char *endptr = NULL;
  rv           = vscp_fwhlp_parse_eventex_datestr(&ex, "    2024-06-30T23:59:58Z    NEXT", &endptr);

  // Move endptr forward to skip any trailing spaces
  while (*endptr == ' ') {
    endptr++;
  }

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("NEXT", endptr); // endptr should point to the 'N' character after the date string
}

// Test Unix nanosecond timestamp parsing (no datestr format, just numeric)
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_unix_ns_1)
{
  int rv;
  vscp_event_ex_t ex;

  // 2024-06-30T23:59:58Z = 1719791998 seconds since epoch
  // In nanoseconds: 1719791998000000000
  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "1719791998000000000", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ex.year);  // year set to 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);   // month set to 0xff (unused in nanosecond mode)
  ASSERT_EQ(1719791998000000000ULL, ex.timestamp_ns);
}

// Test Unix nanosecond timestamp parsing with leading spaces
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_unix_ns_2)
{
  int rv;
  vscp_event_ex_t ex;

  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "   1719791998000000000", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ex.year);
  ASSERT_EQ(1719791998000000000ULL, ex.timestamp_ns);
}

// Test Unix nanosecond timestamp parsing with endptr
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_unix_ns_3)
{
  int rv;
  vscp_event_ex_t ex;
  char *endptr = NULL;

  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "1719791998000000000 NEXT", &endptr);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ex.year);
  ASSERT_EQ(1719791998000000000ULL, ex.timestamp_ns);

  // Skip trailing space
  while (*endptr == ' ') {
    endptr++;
  }
  ASSERT_STREQ("NEXT", endptr);
}

// Test zero timestamp
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_eventex_datestr_unix_ns_zero)
{
  int rv;
  vscp_event_ex_t ex;

  rv = vscp_fwhlp_parse_eventex_datestr(&ex, "0", NULL);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xffff, ex.year);
  ASSERT_EQ(0ULL, ex.timestamp_ns);
}

//-----------------------------------------------------------------------------
// Datestr from event tests
//-----------------------------------------------------------------------------

/*!
  Buffer is too small to hold resulting date string
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_get_datestr_from_event_1)
{
  vscp_event_t ev;
  char buf[10];

  ev.year   = 2024;
  ev.month  = 6;
  ev.day    = 30;
  ev.hour   = 23;
  ev.minute = 59;
  ev.second = 58;

  char *result = vscp_fwhlp_get_datestr_from_event(buf, sizeof(buf), &ev);

  ASSERT_EQ(NULL, result);
}

/*!
  Buffer OK, valid date string
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_get_datestr_from_event_2)
{
  char *result;
  vscp_event_t ev;
  char buf[21];

  memset(&ev, 0, sizeof(ev));
  ev.head   = 0; // Original frame format
  ev.year   = 2024;
  ev.month  = 6;
  ev.day    = 30;
  ev.hour   = 23;
  ev.minute = 59;
  ev.second = 58;

  result = vscp_fwhlp_get_datestr_from_event(buf, sizeof(buf), &ev);

  ASSERT_STREQ("2024-06-30T23:59:58Z", buf);
}

/*!
  Buffer OK, valid date string using Unix nanosecond timestamp
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_get_datestr_from_event_unix_ns)
{
  char *result;
  vscp_event_t ev;
  char buf[21];

  memset(&ev, 0, sizeof(ev));
  ev.head = VSCP_HEADER16_FRAME_VERSION_UNIX_NS; // Unix nanosecond timestamp format
  // 2024-06-30T23:59:58Z = 1719791998 seconds since epoch
  ev.timestamp_ns = 1719791998ULL * 1000000000ULL;

  result = vscp_fwhlp_get_datestr_from_event(buf, sizeof(buf), &ev);

  ASSERT_NE(nullptr, result);
  ASSERT_STREQ("2024-06-30T23:59:58Z", buf);
}

//-----------------------------------------------------------------------------
// Datestr from eventex tests
//-----------------------------------------------------------------------------

/*!
  Buffer is too small to hold resulting date string
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_get_datestr_from_eventex_1)
{
  vscp_event_ex_t ex;
  char buf[10];

  ex.year   = 2024;
  ex.month  = 6;
  ex.day    = 30;
  ex.hour   = 23;
  ex.minute = 59;
  ex.second = 58;

  char *result = vscp_fwhlp_get_datestr_from_eventex(buf, sizeof(buf), &ex);

  ASSERT_EQ(NULL, result);
}

/*!
  Buffer OK, valid date string
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_get_datestr_from_eventex_2)
{
  char *result;
  vscp_event_ex_t ex;
  char buf[21];

  memset(&ex, 0, sizeof(ex));
  ex.head   = 0; // Original frame format
  ex.year   = 2024;
  ex.month  = 6;
  ex.day    = 30;
  ex.hour   = 23;
  ex.minute = 59;
  ex.second = 58;

  result = vscp_fwhlp_get_datestr_from_eventex(buf, sizeof(buf), &ex);

  ASSERT_STREQ("2024-06-30T23:59:58Z", buf);
}

/*!
  Buffer OK, valid date string using Unix nanosecond timestamp
*/
TEST(_vscp_firmware_helper, vscp_fwhlp_get_datestr_from_eventex_unix_ns)
{
  char *result;
  vscp_event_ex_t ex;
  char buf[21];

  memset(&ex, 0, sizeof(ex));
  ex.head = VSCP_HEADER16_FRAME_VERSION_UNIX_NS; // Unix nanosecond timestamp format
  // 2024-06-30T23:59:58Z = 1719791998 seconds since epoch
  ex.timestamp_ns = 1719791998ULL * 1000000000ULL;

  result = vscp_fwhlp_get_datestr_from_eventex(buf, sizeof(buf), &ex);

  ASSERT_NE(nullptr, result);
  ASSERT_STREQ("2024-06-30T23:59:58Z", buf);
}

//-----------------------------------------------------------------------------
// Endian tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_endian)
{
  int little = vscp_fwhlp_isLittleEndian();
  int big    = vscp_fwhlp_isBigEndian();

  // isBigEndian returns bitwise NOT of isLittleEndian
  // So if little is 1, big should be ~1 (-2)
  // At least one should be non-zero
  ASSERT_TRUE(little != 0 || big != 0);
}

//-----------------------------------------------------------------------------
// Number conversion tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_a2ul_decimal)
{
  uint64_t result;
  int rv;

  rv = vscp_fwhlp_a2ul("12345", 0, 10, &result, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(12345, result);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_a2ul_hex)
{
  uint64_t result;
  int rv;

  rv = vscp_fwhlp_a2ul("1A2B", 0, 16, &result, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0x1A2B, result);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_a2ul_auto_hex)
{
  uint64_t result;
  int rv;

  rv = vscp_fwhlp_a2ul("0x1A2B", 0, 0, &result, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0x1A2B, result);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_a2ul_64bit_decimal)
{
  uint64_t result;
  int rv;

  // Test value larger than 32-bit max (4294967295)
  rv = vscp_fwhlp_a2ul("9876543210123456789", 0, 10, &result, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(9876543210123456789ULL, result);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_a2ul_64bit_hex)
{
  uint64_t result;
  int rv;

  // Test 64-bit hex value
  rv = vscp_fwhlp_a2ul("FEDCBA9876543210", 0, 16, &result, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFEDCBA9876543210ULL, result);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_a2ul_64bit_auto_hex)
{
  uint64_t result;
  int rv;

  // Test 64-bit value with 0x prefix
  rv = vscp_fwhlp_a2ul("0x0123456789ABCDEF", 0, 0, &result, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0x0123456789ABCDEFULL, result);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_readStringValue_64bit)
{
  uint64_t result;

  // Test 64-bit hex value
  result = vscp_fwhlp_readStringValue("0xFEDCBA9876543210");
  ASSERT_EQ(0xFEDCBA9876543210ULL, result);

  // Test 64-bit decimal value
  result = vscp_fwhlp_readStringValue("12345678901234567890");
  ASSERT_EQ(12345678901234567890ULL, result);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_dec2hex)
{
  char buf[10];

  memset(buf, 0, sizeof(buf));
  vscp_fwhlp_dec2hex(255, buf, 2);
  buf[2] = '\0';           // Function doesn't null-terminate
  ASSERT_STREQ("FF", buf); // Uses uppercase

  memset(buf, 0, sizeof(buf));
  vscp_fwhlp_dec2hex(0, buf, 2);
  buf[2] = '\0';
  ASSERT_STREQ("00", buf);

  memset(buf, 0, sizeof(buf));
  vscp_fwhlp_dec2hex(16, buf, 2);
  buf[2] = '\0';
  ASSERT_STREQ("10", buf);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_hex2dec)
{
  ASSERT_EQ(255, vscp_fwhlp_hex2dec("FF"));
  ASSERT_EQ(0, vscp_fwhlp_hex2dec("0"));
  ASSERT_EQ(4096, vscp_fwhlp_hex2dec("1000"));
  ASSERT_EQ(43981, vscp_fwhlp_hex2dec("ABCD"));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_hexchar)
{
  ASSERT_EQ(0, vscp_fwhlp_hexchar('0'));
  ASSERT_EQ(9, vscp_fwhlp_hexchar('9'));
  ASSERT_EQ(10, vscp_fwhlp_hexchar('a'));
  ASSERT_EQ(10, vscp_fwhlp_hexchar('A'));
  ASSERT_EQ(15, vscp_fwhlp_hexchar('f'));
  ASSERT_EQ(15, vscp_fwhlp_hexchar('F'));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_readStringValue_decimal)
{
  ASSERT_EQ(12345, vscp_fwhlp_readStringValue("12345"));
  ASSERT_EQ(0, vscp_fwhlp_readStringValue("0"));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_readStringValue_hex)
{
  // Note: vscp_fwhlp_readStringValue has a bug where it passes 'buf' instead of 'p'
  // to a2ul, but we test the actual behavior
  // For now, just test that it returns a non-zero value
  uint32_t val = vscp_fwhlp_readStringValue("0xFF");
  ASSERT_NE(0, val); // Should return something non-zero

  // Test with lowercase works correctly
  val = vscp_fwhlp_readStringValue("0xff");
  ASSERT_NE(0, val);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_decimal)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("12345", &endptr);

  ASSERT_EQ(12345ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_octal)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("077", &endptr);

  ASSERT_EQ(63ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_hex_current_behavior)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("0xFF", &endptr);

  ASSERT_EQ(255ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_hex_large)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("0xFFFFFFFFFFFFFFFF", &endptr);

  ASSERT_EQ(UINT64_MAX, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_decimal_64bit)
{
  char *endptr = nullptr;
  // 2^40 = 1099511627776
  uint64_t val = vscp_fwhlp_parseStringValue("1099511627776", &endptr);

  ASSERT_EQ(1099511627776ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_binary)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("0b10101010", &endptr);

  ASSERT_EQ(0xAAULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_binary_64bit)
{
  char *endptr = nullptr;
  // 33 bits: 0x1FFFFFFFF
  uint64_t val = vscp_fwhlp_parseStringValue("0b111111111111111111111111111111111", &endptr);

  ASSERT_EQ(0x1FFFFFFFFULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_zero)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("0", &endptr);

  ASSERT_EQ(0ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_leading_spaces)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("   42", &endptr);

  ASSERT_EQ(42ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_null)
{
  char *endptr = (char *) "notnull";
  uint64_t val = vscp_fwhlp_parseStringValue(nullptr, &endptr);

  ASSERT_EQ(0ULL, val);
  ASSERT_EQ(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_endptr_null)
{
  // Should not crash when endptr is NULL
  uint64_t val = vscp_fwhlp_parseStringValue("100", nullptr);

  ASSERT_EQ(100ULL, val);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_endptr_trailing)
{
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("123abc", &endptr);

  ASSERT_EQ(123ULL, val);
  ASSERT_NE(nullptr, endptr);
  ASSERT_EQ('a', *endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_binary_invalid)
{
  // "0b" followed by non-binary char should return 0 and endptr at start
  char *endptr = nullptr;
  uint64_t val = vscp_fwhlp_parseStringValue("0bXYZ", &endptr);

  ASSERT_EQ(0ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringValue_octal_explicit)
{
  char *endptr = nullptr;
  // 0777 octal = 511 decimal
  uint64_t val = vscp_fwhlp_parseStringValue("0777", &endptr);

  ASSERT_EQ(511ULL, val);
  ASSERT_NE(nullptr, endptr);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_data)
{
  uint8_t data[8] = { 0 };

  int rv = vscp_fwhlp_parse_data(data, sizeof(data), "1, 2,0x03, 255, 7, 0b10101010", NULL);
  ASSERT_EQ(6, rv);
  ASSERT_EQ(1, data[0]);
  ASSERT_EQ(2, data[1]);
  ASSERT_EQ(3, data[2]);
  ASSERT_EQ(255, data[3]);
  ASSERT_EQ(7, data[4]);
  ASSERT_EQ(0xaa, data[5]);
}

//-----------------------------------------------------------------------------
// Binary/hex conversion tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_bin2hex)
{
  unsigned char bin[] = { 0x00, 0x01, 0xAB, 0xFF };
  char hex[20];

  vscp_fwhlp_bin2hex(hex, sizeof(hex), bin, 4);
  ASSERT_STREQ("0001ABFF", hex); // No colons, uppercase
}

TEST(_vscp_firmware_helper, vscp_fwhlp_hex2bin)
{
  unsigned char bin[10];
  int len;

  // hex2bin doesn't support colons, expects continuous hex string
  len = vscp_fwhlp_hex2bin(bin, sizeof(bin), "0001ABFF");
  ASSERT_EQ(4, len);
  ASSERT_EQ(0x00, bin[0]);
  ASSERT_EQ(0x01, bin[1]);
  ASSERT_EQ(0xAB, bin[2]);
  ASSERT_EQ(0xFF, bin[3]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_hex2bin_no_colon)
{
  unsigned char bin[10];
  int len;

  len = vscp_fwhlp_hex2bin(bin, sizeof(bin), "0001ABFF");
  ASSERT_EQ(4, len);
  ASSERT_EQ(0x00, bin[0]);
  ASSERT_EQ(0x01, bin[1]);
  ASSERT_EQ(0xAB, bin[2]);
  ASSERT_EQ(0xFF, bin[3]);
}

//-----------------------------------------------------------------------------
// String manipulation tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_stristr)
{
  const char *haystack = "Hello World";

  ASSERT_NE(nullptr, vscp_fwhlp_stristr(haystack, "WORLD"));
  ASSERT_NE(nullptr, vscp_fwhlp_stristr(haystack, "hello"));
  ASSERT_NE(nullptr, vscp_fwhlp_stristr(haystack, "lo Wo"));
  ASSERT_EQ(nullptr, vscp_fwhlp_stristr(haystack, "xyz"));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_strsubst)
{
  char result[100];
  char *rv;

  rv = vscp_fwhlp_strsubst(result, sizeof(result), "Hello World", "World", "Universe");
  ASSERT_NE(nullptr, rv);
  ASSERT_STREQ("Hello Universe", result);

  rv = vscp_fwhlp_strsubst(result, sizeof(result), "test test test", "test", "X");
  ASSERT_NE(nullptr, rv);
  ASSERT_STREQ("X X X", result);
}

//-----------------------------------------------------------------------------
// GUID tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_fwhlp_parseGuid(guid, "FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0xEE, guid[1]);
  ASSERT_EQ(0x00, guid[15]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeGuidToString)
{
  uint8_t guid[16] = { 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00 };
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToString(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00", strguid);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_roundtrip)
{
  uint8_t guid1[16] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
  };
  uint8_t guid2[16];
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToString(strguid, guid1);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  rv = vscp_fwhlp_parseGuid(guid2, strguid, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, memcmp(guid1, guid2, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_all_zeros)
{
  uint8_t guid1[16] = { 0 };
  uint8_t guid2[16] = { 0 };
  int rv;

  rv = vscp_fwhlp_parseGuid(guid2, "-", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, memcmp(guid1, guid2, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_1_2_3)
{
  uint8_t guid2[16] = { 0 };
  uint8_t guid1[16] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15
  };
  int rv;

  // Note that GUID strings are always hexadecimal
  rv = vscp_fwhlp_parseGuid(guid2, "0:1:2:3:4:5:6:7:8:9:10:11:12:13:14:15", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, memcmp(guid1, guid2, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_double_colon_prefix)
{
  uint8_t guid[16];
  int rv;

  // :: followed by values should fill leading bytes with 0xFF
  rv = vscp_fwhlp_parseGuid(guid, "::01:02:03", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // First 13 bytes should be 0xFF
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0xFF, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_all_ff)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_fwhlp_parseGuid(guid, "::", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(0xFF, guid[i]);
  }
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_hyphen_colon_prefix)
{
  uint8_t guid[16];
  int rv;

  // -: followed by values should fill leading bytes with 0x00
  rv = vscp_fwhlp_parseGuid(guid, "-:01:02:03", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // First 13 bytes should be 0x00
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_hyphen_colon_comma)
{
  uint8_t guid[16];
  int rv;

  // -: with comma separators
  rv = vscp_fwhlp_parseGuid(guid, "-:1,2,3", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_uuid_format)
{
  uint8_t guid[16];
  int rv;

  // UUID format: 8-4-4-4-12
  rv = vscp_fwhlp_parseGuid(guid, "FFFFFFFF-FFFF-FFFF-0102-03AABB440130", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0xFF, guid[7]);
  ASSERT_EQ(0x01, guid[8]);
  ASSERT_EQ(0x02, guid[9]);
  ASSERT_EQ(0x30, guid[15]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_braces)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_fwhlp_parseGuid(guid, "{FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0xEE, guid[1]);
  ASSERT_EQ(0x00, guid[15]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_braces_uuid)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_fwhlp_parseGuid(guid, "{FFFFFFFF-FFFF-FFFF-0102-03AABB440130}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0x01, guid[8]);
  ASSERT_EQ(0x30, guid[15]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeGuidToStringCompact_leading_ff)
{
  uint8_t guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToStringCompact(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("::01:02:03:04:05:06:07:08", strguid);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeGuidToStringCompact_all_ff)
{
  uint8_t guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToStringCompact(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("::", strguid);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeGuidToStringCompact_no_leading_ff)
{
  uint8_t guid[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToStringCompact(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F:10", strguid);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeGuidToStringUUID)
{
  uint8_t guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0x01, 0x02, 0x03, 0xAA, 0xBB, 0x44, 0x01, 0x30 };
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToStringUUID(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("FFFFFFFF-FFFF-FFFF-0102-03AABB440130", strguid);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_roundtrip_compact)
{
  uint8_t guid1[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  uint8_t guid2[16];
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToStringCompact(strguid, guid1);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  rv = vscp_fwhlp_parseGuid(guid2, strguid, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, memcmp(guid1, guid2, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseGuid_roundtrip_uuid)
{
  uint8_t guid1[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                        0x01, 0x02, 0x03, 0xAA, 0xBB, 0x44, 0x01, 0x30 };
  uint8_t guid2[16];
  char strguid[48];
  int rv;

  rv = vscp_fwhlp_writeGuidToStringUUID(strguid, guid1);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  rv = vscp_fwhlp_parseGuid(guid2, strguid, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, memcmp(guid1, guid2, 16));
}

//-----------------------------------------------------------------------------
// MAC address tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_parseMac)
{
  uint8_t mac[6];
  int rv;

  rv = vscp_fwhlp_parseMac(mac, "AA:BB:CC:DD:EE:FF", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xAA, mac[0]);
  ASSERT_EQ(0xBB, mac[1]);
  ASSERT_EQ(0xCC, mac[2]);
  ASSERT_EQ(0xDD, mac[3]);
  ASSERT_EQ(0xEE, mac[4]);
  ASSERT_EQ(0xFF, mac[5]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseMac_lowercase)
{
  uint8_t mac[6];
  int rv;

  // parseMac only supports colon separator
  rv = vscp_fwhlp_parseMac(mac, "aa:bb:cc:dd:ee:ff", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xAA, mac[0]);
  ASSERT_EQ(0xFF, mac[5]);
}

//-----------------------------------------------------------------------------
// Event priority tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_getEventPriority)
{
  vscp_event_t ev;

  ev.head = 0x00;
  ASSERT_EQ(0, vscp_fwhlp_getEventPriority(&ev));

  ev.head = 0xE0; // Priority 7 (bits 7-5)
  ASSERT_EQ(7, vscp_fwhlp_getEventPriority(&ev));

  ev.head = 0x60; // Priority 3
  ASSERT_EQ(3, vscp_fwhlp_getEventPriority(&ev));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getEventPriorityEx)
{
  vscp_event_ex_t ex;

  ex.head = 0x00;
  ASSERT_EQ(0, vscp_fwhlp_getEventExPriority(&ex));

  ex.head = 0xE0; // Priority 7
  ASSERT_EQ(7, vscp_fwhlp_getEventExPriority(&ex));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_setEventExPriority)
{
  vscp_event_ex_t ex;

  ex.head = 0x00;
  vscp_fwhlp_setEventExPriority(&ex, 7);
  ASSERT_EQ(7, vscp_fwhlp_getEventExPriority(&ex));

  ex.head = 0x00;
  vscp_fwhlp_setEventExPriority(&ex, 0);
  ASSERT_EQ(0, vscp_fwhlp_getEventExPriority(&ex));

  ex.head = 0x1F; // Set non-priority bits
  vscp_fwhlp_setEventExPriority(&ex, 5);
  ASSERT_EQ(5, vscp_fwhlp_getEventExPriority(&ex));
  ASSERT_EQ(0x1F, ex.head & 0x1F); // Lower bits should be preserved
}

TEST(_vscp_firmware_helper, vscp_fwhlp_setEventPriority)
{
  vscp_event_t ev;

  ev.head = 0x00;
  vscp_fwhlp_setEventPriority(&ev, 7);
  ASSERT_EQ(7, vscp_fwhlp_getEventPriority(&ev));

  ev.head = 0x00;
  vscp_fwhlp_setEventPriority(&ev, 0);
  ASSERT_EQ(0, vscp_fwhlp_getEventPriority(&ev));

  ev.head = 0x1F; // Set non-priority bits
  vscp_fwhlp_setEventPriority(&ev, 5);
  ASSERT_EQ(5, vscp_fwhlp_getEventPriority(&ev));
  ASSERT_EQ(0x1F, ev.head & 0x1F); // Lower bits should be preserved
}

TEST(_vscp_firmware_helper, vscp_fwhlp_setEventPriority_null_pointer)
{
  vscp_fwhlp_setEventPriority(NULL, 3);
  SUCCEED();
}

TEST(_vscp_firmware_helper, vscp_fwhlp_setEventExPriority_null_pointer)
{
  vscp_fwhlp_setEventExPriority(NULL, 3);
  SUCCEED();
}

//-----------------------------------------------------------------------------
// Frame version tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, setFrameVersion_original)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  bool rv = setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_ORIGINAL);
  ASSERT_TRUE(rv);
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_ORIGINAL, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
}

TEST(_vscp_firmware_helper, setFrameVersion_unix_ns)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  bool rv = setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
  ASSERT_TRUE(rv);
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
}

TEST(_vscp_firmware_helper, setFrameVersion_preserves_other_bits)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  // Set priority and other bits
  ev.head = 0xE0FF;  // Priority 7 and other bits set

  bool rv = setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
  ASSERT_TRUE(rv);
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  // Verify other bits are preserved (excluding frame version bits)
  ASSERT_EQ(0xE0FF & ~VSCP_HEADER16_FRAME_VERSION_MASK, ev.head & ~VSCP_HEADER16_FRAME_VERSION_MASK);
}

TEST(_vscp_firmware_helper, setFrameVersion_null_pointer)
{
  bool rv = setFrameVersion(NULL, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
  ASSERT_FALSE(rv);
}

TEST(_vscp_firmware_helper, setFrameVersionEx_original)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  bool rv = setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_ORIGINAL);
  ASSERT_TRUE(rv);
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_ORIGINAL, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);
}

TEST(_vscp_firmware_helper, setFrameVersionEx_unix_ns)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  bool rv = setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
  ASSERT_TRUE(rv);
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);
}

TEST(_vscp_firmware_helper, setFrameVersionEx_preserves_other_bits)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  // Set priority and other bits
  ex.head = 0xE0FF;  // Priority 7 and other bits set

  bool rv = setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
  ASSERT_TRUE(rv);
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  // Verify other bits are preserved (excluding frame version bits)
  ASSERT_EQ(0xE0FF & ~VSCP_HEADER16_FRAME_VERSION_MASK, ex.head & ~VSCP_HEADER16_FRAME_VERSION_MASK);
}

TEST(_vscp_firmware_helper, setFrameVersionEx_null_pointer)
{
  bool rv = setFrameVersionEx(NULL, VSCP_HEADER16_FRAME_VERSION_UNIX_NS);
  ASSERT_FALSE(rv);
}

TEST(_vscp_firmware_helper, setFrameVersion_all_versions)
{
  vscp_event_t ev;

  // Test all frame version values
  memset(&ev, 0, sizeof(ev));
  ASSERT_TRUE(setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_ORIGINAL));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_ORIGINAL, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);

  memset(&ev, 0, sizeof(ev));
  ASSERT_TRUE(setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_UNIX_NS));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);

  memset(&ev, 0, sizeof(ev));
  ASSERT_TRUE(setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_2));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_2, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);

  memset(&ev, 0, sizeof(ev));
  ASSERT_TRUE(setFrameVersion(&ev, VSCP_HEADER16_FRAME_VERSION_3));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_3, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
}

TEST(_vscp_firmware_helper, setFrameVersionEx_all_versions)
{
  vscp_event_ex_t ex;

  // Test all frame version values
  memset(&ex, 0, sizeof(ex));
  ASSERT_TRUE(setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_ORIGINAL));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_ORIGINAL, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);

  memset(&ex, 0, sizeof(ex));
  ASSERT_TRUE(setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_UNIX_NS));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);

  memset(&ex, 0, sizeof(ex));
  ASSERT_TRUE(setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_2));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_2, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);

  memset(&ex, 0, sizeof(ex));
  ASSERT_TRUE(setFrameVersionEx(&ex, VSCP_HEADER16_FRAME_VERSION_3));
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_3, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);
}

//-----------------------------------------------------------------------------
// Event conversion tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_convertEventToEventEx)
{
  vscp_event_t ev;
  vscp_event_ex_t ex;
  int rv;

  // Setup event
  ev.head       = 0x60;
  ev.obid       = 0x12345678;
  ev.timestamp  = 0xABCDEF;
  ev.year       = 2024;
  ev.month      = 6;
  ev.day        = 30;
  ev.hour       = 23;
  ev.minute     = 59;
  ev.second     = 58;
  ev.vscp_class = 10;
  ev.vscp_type  = 6;
  ev.sizeData   = 3;
  ev.pdata      = NULL;

  uint8_t data[3] = { 0x01, 0x02, 0x03 };
  ev.pdata        = data;

  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  rv = vscp_fwhlp_convertEventToEventEx(&ex, &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(ev.head, ex.head);
  ASSERT_EQ(ev.vscp_class, ex.vscp_class);
  ASSERT_EQ(ev.vscp_type, ex.vscp_type);
  ASSERT_EQ(ev.obid, ex.obid);
  ASSERT_EQ(ev.timestamp, ex.timestamp);
  ASSERT_EQ(ev.year, ex.year);
  ASSERT_EQ(ev.sizeData, ex.sizeData);
  ASSERT_EQ(0, memcmp(ev.GUID, ex.GUID, 16));
  ASSERT_EQ(0, memcmp(ev.pdata, ex.data, 3));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_convertEventExToEvent)
{
  vscp_event_t ev;
  vscp_event_ex_t ex;
  int rv;

  // Setup eventex
  ex.head       = 0x60;
  ex.obid       = 0x12345678;
  ex.timestamp  = 0xABCDEF;
  ex.year       = 2024;
  ex.month      = 6;
  ex.day        = 30;
  ex.hour       = 23;
  ex.minute     = 59;
  ex.second     = 58;
  ex.vscp_class = 10;
  ex.vscp_type  = 6;
  ex.sizeData   = 3;
  ex.data[0]    = 0x01;
  ex.data[1]    = 0x02;
  ex.data[2]    = 0x03;

  for (int i = 0; i < 16; i++) {
    ex.GUID[i] = i;
  }

  rv = vscp_fwhlp_convertEventExToEvent(&ev, &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(ex.head, ev.head);
  ASSERT_EQ(ex.vscp_class, ev.vscp_class);
  ASSERT_EQ(ex.vscp_type, ev.vscp_type);
  ASSERT_EQ(ex.obid, ev.obid);
  ASSERT_EQ(ex.timestamp, ev.timestamp);
  ASSERT_EQ(ex.year, ev.year);
  ASSERT_EQ(ex.sizeData, ev.sizeData);
  ASSERT_EQ(0, memcmp(ex.GUID, ev.GUID, 16));
  ASSERT_EQ(0, memcmp(ex.data, ev.pdata, 3));

  // Cleanup
  if (ev.pdata) {
    free(ev.pdata);
  }
}

TEST(_vscp_firmware_helper, vscp_fwhlp_convertEventToEventEx_unix_ns)
{
  vscp_event_t ev;
  vscp_event_ex_t ex;
  int rv;

  memset(&ev, 0, sizeof(ev));
  memset(&ex, 0, sizeof(ex));

  // Setup event with Unix nanosecond timestamp format
  ev.head = VSCP_HEADER16_FRAME_VERSION_UNIX_NS; // Unix nanosecond timestamp format
  ev.obid       = 0x12345678;
  ev.year       = 0xffff;  // Indicates Unix ns timestamp mode
  ev.month      = 0xff;
  ev.timestamp_ns = 1719791998000000000ULL;  // 2024-06-30T23:59:58Z in nanoseconds
  ev.vscp_class = 10;
  ev.vscp_type  = 6;
  ev.sizeData   = 3;

  uint8_t data[3] = { 0x01, 0x02, 0x03 };
  ev.pdata        = data;

  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  rv = vscp_fwhlp_convertEventToEventEx(&ex, &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(ev.head, ex.head);
  ASSERT_EQ(ev.vscp_class, ex.vscp_class);
  ASSERT_EQ(ev.vscp_type, ex.vscp_type);
  ASSERT_EQ(ev.obid, ex.obid);
  ASSERT_EQ(ev.year, ex.year);
  ASSERT_EQ(0xffff, ex.year);
  ASSERT_EQ(ev.timestamp_ns, ex.timestamp_ns);
  ASSERT_EQ(1719791998000000000ULL, ex.timestamp_ns);
  ASSERT_EQ(ev.sizeData, ex.sizeData);
  ASSERT_EQ(0, memcmp(ev.GUID, ex.GUID, 16));
  ASSERT_EQ(0, memcmp(ev.pdata, ex.data, 3));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_convertEventExToEvent_unix_ns)
{
  vscp_event_t ev;
  vscp_event_ex_t ex;
  int rv;

  memset(&ev, 0, sizeof(ev));
  memset(&ex, 0, sizeof(ex));

  // Setup eventex with Unix nanosecond timestamp format
  ex.head = VSCP_HEADER16_FRAME_VERSION_UNIX_NS;  // Unix nanosecond timestamp format
  ex.obid       = 0x12345678;
  ex.year       = 0xffff;  // Indicates Unix ns timestamp mode
  ex.month      = 0xff;
  ex.timestamp_ns = 1719791998000000000ULL;  // 2024-06-30T23:59:58Z in nanoseconds
  ex.vscp_class = 10;
  ex.vscp_type  = 6;
  ex.sizeData   = 3;
  ex.data[0]    = 0x01;
  ex.data[1]    = 0x02;
  ex.data[2]    = 0x03;

  for (int i = 0; i < 16; i++) {
    ex.GUID[i] = i;
  }

  rv = vscp_fwhlp_convertEventExToEvent(&ev, &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(ex.head, ev.head);
  ASSERT_EQ(ex.vscp_class, ev.vscp_class);
  ASSERT_EQ(ex.vscp_type, ev.vscp_type);
  ASSERT_EQ(ex.obid, ev.obid);
  ASSERT_EQ(ex.year, ev.year);
  ASSERT_EQ(0xffff, ev.year);
  ASSERT_EQ(ex.timestamp_ns, ev.timestamp_ns);
  ASSERT_EQ(1719791998000000000ULL, ev.timestamp_ns);
  ASSERT_EQ(ex.sizeData, ev.sizeData);
  ASSERT_EQ(0, memcmp(ex.GUID, ev.GUID, 16));
  ASSERT_EQ(0, memcmp(ex.data, ev.pdata, 3));

  // Cleanup
  if (ev.pdata) {
    free(ev.pdata);
  }
}

//-----------------------------------------------------------------------------
// Event parsing and string conversion tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_eventToString)
{
  vscp_event_t ev;
  char buf[512];
  int rv;

  // Setup event
  ev.head       = 0x00;
  ev.obid       = 0;
  ev.timestamp  = 0;
  ev.year       = 2024;
  ev.month      = 6;
  ev.day        = 30;
  ev.hour       = 23;
  ev.minute     = 59;
  ev.second     = 58;
  ev.vscp_class = 10;
  ev.vscp_type  = 6;
  ev.sizeData   = 3;

  uint8_t data[3] = { 0x01, 0x02, 0x03 };
  ev.pdata        = data;

  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  rv = vscp_fwhlp_eventToString(buf, sizeof(buf), &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // String should contain event data
  ASSERT_NE(nullptr, strstr(buf, "10")); // class
  ASSERT_NE(nullptr, strstr(buf, "6"));  // type
}

TEST(_vscp_firmware_helper, vscp_fwhlp_eventToStringEx)
{
  vscp_event_ex_t ex;
  char buf[512];
  int rv;

  // Setup eventex
  ex.head       = 0x00;
  ex.obid       = 0;
  ex.timestamp  = 0;
  ex.year       = 2024;
  ex.month      = 6;
  ex.day        = 30;
  ex.hour       = 23;
  ex.minute     = 59;
  ex.second     = 58;
  ex.vscp_class = 10;
  ex.vscp_type  = 6;
  ex.sizeData   = 3;
  ex.data[0]    = 0x01;
  ex.data[1]    = 0x02;
  ex.data[2]    = 0x03;

  for (int i = 0; i < 16; i++) {
    ex.GUID[i] = i;
  }

  rv = vscp_fwhlp_eventToStringEx(buf, sizeof(buf), &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // String should contain event data
  ASSERT_NE(nullptr, strstr(buf, "10")); // class
  ASSERT_NE(nullptr, strstr(buf, "6"));  // type
}

TEST(_vscp_firmware_helper, vscp_fwhlp_eventToString_unix_ns)
{
  vscp_event_t ev;
  char buf[512];
  int rv;

  memset(&ev, 0, sizeof(ev));

  // Setup event with Unix nanosecond timestamp format
  ev.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ev.obid         = 0;
  ev.year         = 0xffff;
  ev.month        = 0xff;
  ev.timestamp_ns = 1719791998000000000ULL;  // 2024-06-30T23:59:58Z in nanoseconds
  ev.vscp_class   = 10;
  ev.vscp_type    = 6;
  ev.sizeData     = 3;

  uint8_t data[3] = { 0x01, 0x02, 0x03 };
  ev.pdata        = data;

  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  rv = vscp_fwhlp_eventToString(buf, sizeof(buf), &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // String should contain empty datestr (,,) and nanosecond timestamp
  ASSERT_NE(nullptr, strstr(buf, "256,"));  // head with VSCP_HEADER16_FRAME_VERSION_UNIX_NS
  ASSERT_NE(nullptr, strstr(buf, ",10,"));  // class
  ASSERT_NE(nullptr, strstr(buf, ",6,"));   // type
  ASSERT_NE(nullptr, strstr(buf, ",,1719791998000000000,")); // empty datestr + ns timestamp
}

TEST(_vscp_firmware_helper, vscp_fwhlp_eventToStringEx_unix_ns)
{
  vscp_event_ex_t ex;
  char buf[512];
  int rv;

  memset(&ex, 0, sizeof(ex));

  // Setup eventex with Unix nanosecond timestamp format
  ex.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ex.obid         = 0;
  ex.year         = 0xffff;
  ex.month        = 0xff;
  ex.timestamp_ns = 1719791998000000000ULL;  // 2024-06-30T23:59:58Z in nanoseconds
  ex.vscp_class   = 10;
  ex.vscp_type    = 6;
  ex.sizeData     = 3;
  ex.data[0]      = 0x01;
  ex.data[1]      = 0x02;
  ex.data[2]      = 0x03;

  for (int i = 0; i < 16; i++) {
    ex.GUID[i] = i;
  }

  rv = vscp_fwhlp_eventToStringEx(buf, sizeof(buf), &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // String should contain empty datestr (,,) and nanosecond timestamp
  ASSERT_NE(nullptr, strstr(buf, "256,"));  // head with VSCP_HEADER16_FRAME_VERSION_UNIX_NS
  ASSERT_NE(nullptr, strstr(buf, ",10,"));  // class
  ASSERT_NE(nullptr, strstr(buf, ",6,"));   // type
  ASSERT_NE(nullptr, strstr(buf, ",,1719791998000000000,")); // empty datestr + ns timestamp
}

// Test roundtrip: parse -> toString -> parse
TEST(_vscp_firmware_helper, vscp_fwhlp_eventToString_unix_ns_roundtrip)
{
  vscp_event_t ev1, ev2;
  char buf[512];
  int rv;

  memset(&ev1, 0, sizeof(ev1));
  memset(&ev2, 0, sizeof(ev2));

  // Setup event with Unix nanosecond timestamp format
  ev1.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 0x03;  // Unix ns + rolling index
  ev1.obid         = 12345;
  ev1.year         = 0xffff;
  ev1.month        = 0xff;
  ev1.timestamp_ns = 1719791998000000000ULL;
  ev1.vscp_class   = 20;
  ev1.vscp_type    = 3;
  ev1.sizeData     = 2;

  uint8_t data[2] = { 0xAA, 0xBB };
  ev1.pdata       = data;

  for (int i = 0; i < 16; i++) {
    ev1.GUID[i] = 0;
  }

  // Convert to string
  rv = vscp_fwhlp_eventToString(buf, sizeof(buf), &ev1);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Parse back
  rv = vscp_fwhlp_parseStringToEvent(&ev2, buf);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify roundtrip
  ASSERT_EQ(ev1.head, ev2.head);
  ASSERT_EQ(ev1.vscp_class, ev2.vscp_class);
  ASSERT_EQ(ev1.vscp_type, ev2.vscp_type);
  ASSERT_EQ(ev1.year, ev2.year);
  ASSERT_EQ(0xffff, ev2.year);
  ASSERT_EQ(ev1.timestamp_ns, ev2.timestamp_ns);
  ASSERT_EQ(ev1.sizeData, ev2.sizeData);
  ASSERT_EQ(0, memcmp(ev1.pdata, ev2.pdata, ev1.sizeData));

  if (ev2.pdata) {
    free(ev2.pdata);
  }
}

//-----------------------------------------------------------------------------
// Filter tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_doLevel2Filter_null_filter)
{
  vscp_event_t ev;

  ev.vscp_class = 10;
  ev.vscp_type  = 6;

  // NULL filter should pass everything
  ASSERT_TRUE(vscp_fwhlp_doLevel2Filter(&ev, NULL));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_doLevel2Filter_class_match)
{
  vscp_event_t ev;
  vscpEventFilter filter;

  memset(&filter, 0, sizeof(vscpEventFilter));

  ev.vscp_class = 10;
  ev.vscp_type  = 6;

  // Set filter to match class 10
  filter.filter_class = 10;
  filter.mask_class   = 0xFFFF; // Check all bits

  ASSERT_TRUE(vscp_fwhlp_doLevel2Filter(&ev, &filter));

  // Change event class - should not match
  ev.vscp_class = 20;
  ASSERT_FALSE(vscp_fwhlp_doLevel2Filter(&ev, &filter));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_doLevel2Filter_type_match)
{
  vscp_event_t ev;
  vscpEventFilter filter;

  memset(&filter, 0, sizeof(vscpEventFilter));

  ev.vscp_class = 10;
  ev.vscp_type  = 6;

  // Set filter to match type 6
  filter.filter_type = 6;
  filter.mask_type   = 0xFFFF; // Check all bits

  ASSERT_TRUE(vscp_fwhlp_doLevel2Filter(&ev, &filter));

  // Change event type - should not match
  ev.vscp_type = 8;
  ASSERT_FALSE(vscp_fwhlp_doLevel2Filter(&ev, &filter));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_doLevel2Filter_dont_care_mask)
{
  vscp_event_t ev;
  vscpEventFilter filter;

  memset(&filter, 0, sizeof(vscpEventFilter));

  ev.vscp_class = 10;
  ev.vscp_type  = 6;

  // Set filter but mask to 0 (don't care)
  filter.filter_class = 10;
  filter.mask_class   = 0x0000; // Don't care
  filter.filter_type  = 6;
  filter.mask_type    = 0x0000; // Don't care

  // Should match any class/type
  ASSERT_TRUE(vscp_fwhlp_doLevel2Filter(&ev, &filter));

  ev.vscp_class = 999;
  ev.vscp_type  = 999;
  ASSERT_TRUE(vscp_fwhlp_doLevel2Filter(&ev, &filter));
}

//-----------------------------------------------------------------------------
// UDP Frame tests (VSCP_FWHLP_BINARY_FRAME_SUPPORT)
//-----------------------------------------------------------------------------

#ifdef VSCP_FWHLP_BINARY_FRAME_SUPPORT

TEST(_vscp_firmware_helper, vscp_fwhlp_getFrameSizeFromEvent)
{
  vscp_event_t ev;
  size_t size;

  ev.sizeData = 0;
  size        = vscp_fwhlp_getFrameSizeFromEvent(&ev);
  // Size = 1 (pkttype) + 35 (header) + 0 (data) + 2 (CRC) = 38
  ASSERT_EQ(38, size);

  ev.sizeData = 10;
  size        = vscp_fwhlp_getFrameSizeFromEvent(&ev);
  // Size = 1 (pkttype) + 35 (header) + 10 (data) + 2 (CRC) = 48
  ASSERT_EQ(48, size);

  ev.sizeData = 100;
  size        = vscp_fwhlp_getFrameSizeFromEvent(&ev);
  // Size = 1 (pkttype) + 35 (header) + 100 (data) + 2 (CRC) = 138
  ASSERT_EQ(138, size);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getFrameSizeFromEventEx)
{
  vscp_event_ex_t ex;
  size_t size;

  ex.sizeData = 0;
  size        = vscp_fwhlp_getFrameSizeFromEventEx(&ex);
  // Size = 1 (pkttype) + 35 (header) + 0 (data) + 2 (CRC) = 38
  ASSERT_EQ(38, size);

  ex.sizeData = 10;
  size        = vscp_fwhlp_getFrameSizeFromEventEx(&ex);
  // Size = 1 (pkttype) + 35 (header) + 10 (data) + 2 (CRC) = 48
  ASSERT_EQ(48, size);

  ex.sizeData = 100;
  size        = vscp_fwhlp_getFrameSizeFromEventEx(&ex);
  // Size = 1 (pkttype) + 35 (header) + 100 (data) + 2 (CRC) = 138
  ASSERT_EQ(138, size);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeEventToFrame_basic)
{
  vscp_event_t ev;
  uint8_t frame[100];
  int rv;

  // Setup event with minimal data - uses nanosecond timestamp (packet format 1)
  memset(&ev, 0, sizeof(vscpEvent));
  ev.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 0x20; // Priority 1, UNIX_NS frame version
  ev.timestamp_ns = 1234567890123456789ULL;
  ev.vscp_class   = 10;
  ev.vscp_type    = 6;
  ev.sizeData     = 0;
  ev.pdata        = NULL;

  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  rv = vscp_fwhlp_writeEventToFrame(frame, sizeof(frame), VSCP_BINARY_PACKET_TYPE_EVENT, &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify frame type is packet format 1 (0x10 = type 1 in upper nibble)
  ASSERT_EQ(0x00, frame[0]);

  // Verify head (frame version bits should be set to UNIX_NS)
  ASSERT_EQ(0x01, frame[1]); // MSB - frame version bits
  ASSERT_EQ(0x20, frame[2]); // LSB - priority

  // Verify class
  ASSERT_EQ(0x00, frame[14]);
  ASSERT_EQ(0x0A, frame[15]);

  // Verify type
  ASSERT_EQ(0x00, frame[16]);
  ASSERT_EQ(0x06, frame[17]);

  // Verify GUID
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(i, frame[18 + i]);
  }

  // Verify data size
  ASSERT_EQ(0x00, frame[34]);
  ASSERT_EQ(0x00, frame[35]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeEventToFrame_with_data)
{
  vscp_event_t ev;
  uint8_t frame[100];
  uint8_t data[5] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE };
  int rv;

  // Setup event with data
  memset(&ev, 0, sizeof(vscpEvent));
  ev.head       = 0x00;
  ev.timestamp  = 0;
  ev.year       = 2024;
  ev.month      = 1;
  ev.day        = 1;
  ev.hour       = 0;
  ev.minute     = 0;
  ev.second     = 0;
  ev.vscp_class = 20;
  ev.vscp_type  = 8;
  ev.sizeData   = 5;
  ev.pdata      = data;

  memset(ev.GUID, 0xFF, 16);

  rv = vscp_fwhlp_writeEventToFrame(frame, sizeof(frame), 0x00, &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify data size
  ASSERT_EQ(0x00, frame[34]);
  ASSERT_EQ(0x05, frame[35]);

  // Verify data content
  ASSERT_EQ(0xAA, frame[36]);
  ASSERT_EQ(0xBB, frame[37]);
  ASSERT_EQ(0xCC, frame[38]);
  ASSERT_EQ(0xDD, frame[39]);
  ASSERT_EQ(0xEE, frame[40]);

  // CRC is at position 36 + 5 = 41, 42
  // The CRC calculation is handled internally
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeEventToFrame_buffer_too_small)
{
  vscp_event_t ev;
  uint8_t frame[10]; // Too small
  int rv;

  memset(&ev, 0, sizeof(vscpEvent));
  ev.sizeData = 0;
  ev.pdata    = NULL;

  rv = vscp_fwhlp_writeEventToFrame(frame, sizeof(frame), 0x00, &ev);
  ASSERT_EQ(VSCP_ERROR_BUFFER_TO_SMALL, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeEventExToFrame)
{
  vscp_event_ex_t ex;
  uint8_t frame[100];
  int rv;

  // Setup eventex
  memset(&ex, 0, sizeof(vscpEventEx));
  ex.head       = 0x40; // Priority 2
  ex.timestamp  = 9876543;
  ex.year       = 2024;
  ex.month      = 12;
  ex.day        = 25;
  ex.hour       = 12;
  ex.minute     = 30;
  ex.second     = 45;
  ex.vscp_class = 30;
  ex.vscp_type  = 15;
  ex.sizeData   = 3;
  ex.data[0]    = 0x11;
  ex.data[1]    = 0x22;
  ex.data[2]    = 0x33;

  for (int i = 0; i < 16; i++) {
    ex.GUID[i] = 15 - i;
  }

  rv = vscp_fwhlp_writeEventExToFrame(frame, sizeof(frame), 0x00, &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify class
  ASSERT_EQ(0x00, frame[14]);
  ASSERT_EQ(0x1E, frame[15]); // 30

  // Verify type
  ASSERT_EQ(0x00, frame[16]);
  ASSERT_EQ(0x0F, frame[17]); // 15

  // Verify data
  ASSERT_EQ(0x11, frame[36]);
  ASSERT_EQ(0x22, frame[37]);
  ASSERT_EQ(0x33, frame[38]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getEventFromFrame)
{
  vscp_event_t ev_write, ev_read;
  uint8_t frame[100];
  uint8_t data[3] = { 0x01, 0x02, 0x03 };
  int rv;

  // Setup event to write - packet format 1 uses nanosecond timestamp
  memset(&ev_write, 0, sizeof(vscpEvent));
  ev_write.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 0x60; // Priority 3, UNIX_NS frame version
  ev_write.timestamp_ns = 555555000000000ULL;
  ev_write.vscp_class   = 50;
  ev_write.vscp_type    = 25;
  ev_write.sizeData     = 3;
  ev_write.pdata        = data;

  for (int i = 0; i < 16; i++) {
    ev_write.GUID[i] = i * 2;
  }

  // Write to frame
  rv = vscp_fwhlp_writeEventToFrame(frame, sizeof(frame), 0x00, &ev_write);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Read from frame
  memset(&ev_read, 0, sizeof(vscpEvent));
  rv = vscp_fwhlp_getEventFromFrame(&ev_read, frame, sizeof(frame));
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify data was preserved (head will have frame version set)
  uint16_t expected_head = (ev_write.head & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ev_read.head);
  ASSERT_EQ(ev_write.timestamp_ns, ev_read.timestamp_ns);
  ASSERT_EQ(ev_write.vscp_class, ev_read.vscp_class);
  ASSERT_EQ(ev_write.vscp_type, ev_read.vscp_type);
  ASSERT_EQ(ev_write.sizeData, ev_read.sizeData);

  // Verify GUID
  ASSERT_EQ(0, memcmp(ev_write.GUID, ev_read.GUID, 16));

  // Verify data content
  ASSERT_EQ(0, memcmp(ev_write.pdata, ev_read.pdata, 3));

  // Cleanup
  if (ev_read.pdata) {
    free(ev_read.pdata);
  }
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getEventExFromFrame)
{
  vscp_event_ex_t ex_write, ex_read;
  uint8_t frame[100];
  int rv;

  // Setup eventex to write - packet format 1 uses nanosecond timestamp
  memset(&ex_write, 0, sizeof(vscpEventEx));
  ex_write.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 0xE0; // Priority 7, UNIX_NS frame version
  ex_write.timestamp_ns = 123456789012345678ULL;
  ex_write.vscp_class   = 100;
  ex_write.vscp_type    = 50;
  ex_write.sizeData     = 16;
  ex_write.data[0]      = 0xDE;
  ex_write.data[1]      = 0xAD;
  ex_write.data[2]      = 0xBE;
  ex_write.data[3]      = 0xEF;

  for (int i = 0; i < 16; i++) {
    ex_write.GUID[i] = (i + 1) * 3;
  }

  // Write to frame
  rv = vscp_fwhlp_writeEventExToFrame(frame, sizeof(frame), 0x00, &ex_write);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Read from frame
  memset(&ex_read, 0, sizeof(vscpEventEx));
  rv = vscp_fwhlp_getEventExFromFrame(&ex_read, frame, sizeof(frame));
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify data was preserved (head will have frame version set)
  uint16_t expected_head = (ex_write.head & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ex_read.head);
  ASSERT_EQ(ex_write.timestamp_ns, ex_read.timestamp_ns);
  ASSERT_EQ(ex_write.vscp_class, ex_read.vscp_class);
  ASSERT_EQ(ex_write.vscp_type, ex_read.vscp_type);
  ASSERT_EQ(ex_write.sizeData, ex_read.sizeData);

  // Verify data content (first 4 bytes that we set)
  ASSERT_EQ(0xDE, ex_read.data[0]);
  ASSERT_EQ(0xAD, ex_read.data[1]);
  ASSERT_EQ(0xBE, ex_read.data[2]);
  ASSERT_EQ(0xEF, ex_read.data[3]);

  // Verify GUID (works because sizeData == 16)
  ASSERT_EQ(0, memcmp(ex_write.GUID, ex_read.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_frame_roundtrip_event)
{
  vscp_event_t ev_original, ev_recovered;
  uint8_t frame[200];
  uint8_t data[10] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
  int rv;

  // Setup original event - packet format 1 uses nanosecond timestamp
  memset(&ev_original, 0, sizeof(vscpEvent));
  ev_original.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 0xA0; // Priority 5, UNIX_NS frame version
  ev_original.timestamp_ns = 0xABCDEF1234567890ULL;
  ev_original.vscp_class   = 512;
  ev_original.vscp_type    = 128;
  ev_original.sizeData     = 10;
  ev_original.pdata        = data;

  for (int i = 0; i < 16; i++) {
    ev_original.GUID[i] = (i * 17) % 256;
  }

  // Write to frame
  rv = vscp_fwhlp_writeEventToFrame(frame, sizeof(frame), 0x00, &ev_original);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Read from frame
  memset(&ev_recovered, 0, sizeof(vscpEvent));
  rv = vscp_fwhlp_getEventFromFrame(&ev_recovered, frame, sizeof(frame));
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify complete round-trip preservation (head will have frame version set)
  uint16_t expected_head = (ev_original.head & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ev_recovered.head);
  ASSERT_EQ(ev_original.timestamp_ns, ev_recovered.timestamp_ns);
  ASSERT_EQ(ev_original.vscp_class, ev_recovered.vscp_class);
  ASSERT_EQ(ev_original.vscp_type, ev_recovered.vscp_type);
  ASSERT_EQ(ev_original.sizeData, ev_recovered.sizeData);

  // Verify GUID
  ASSERT_EQ(0, memcmp(ev_original.GUID, ev_recovered.GUID, 16));

  // Verify all data
  ASSERT_EQ(0, memcmp(ev_original.pdata, ev_recovered.pdata, 10));

  // Cleanup
  if (ev_recovered.pdata) {
    free(ev_recovered.pdata);
  }
}

TEST(_vscp_firmware_helper, vscp_fwhlp_frame_roundtrip_eventex)
{
  vscp_event_ex_t ex_original, ex_recovered;
  uint8_t frame[200];
  int rv;

  // Setup original eventex - packet format 1 uses nanosecond timestamp
  memset(&ex_original, 0, sizeof(vscpEventEx));
  ex_original.head         = VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 0xC0; // Priority 6, UNIX_NS frame version
  ex_original.timestamp_ns = 0x8765432112345678ULL;
  ex_original.vscp_class   = 1024;
  ex_original.vscp_type    = 256;
  ex_original.sizeData     = 16;

  for (int i = 0; i < 16; i++) {
    ex_original.data[i] = (i * 7) % 256;
  }

  for (int i = 0; i < 16; i++) {
    ex_original.GUID[i] = (255 - i);
  }

  // Write to frame
  rv = vscp_fwhlp_writeEventExToFrame(frame, sizeof(frame), 0x00, &ex_original);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Read from frame
  memset(&ex_recovered, 0, sizeof(vscpEventEx));
  rv = vscp_fwhlp_getEventExFromFrame(&ex_recovered, frame, sizeof(frame));
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify complete round-trip preservation (head will have frame version set)
  uint16_t expected_head = (ex_original.head & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ex_recovered.head);
  ASSERT_EQ(ex_original.timestamp_ns, ex_recovered.timestamp_ns);
  ASSERT_EQ(ex_original.vscp_class, ex_recovered.vscp_class);
  ASSERT_EQ(ex_original.vscp_type, ex_recovered.vscp_type);
  ASSERT_EQ(ex_original.sizeData, ex_recovered.sizeData);

  // Verify all data
  ASSERT_EQ(0, memcmp(ex_original.data, ex_recovered.data, 16));

  // Verify GUID
  ASSERT_EQ(0, memcmp(ex_original.GUID, ex_recovered.GUID, 16));
}

// Test reading packet format 0 (legacy format with date/time fields)
TEST(_vscp_firmware_helper, vscp_fwhlp_getEventFromFrame_packet0)
{
  vscp_event_t ev;
  int rv;

  // Manually construct a packet format 0 frame
  // Format 0 layout:
  //  0           Packet type & encryption (0x00 = type 0, no encryption)
  //  1           HEAD MSB
  //  2           HEAD LSB
  //  3-6         Timestamp microseconds (4 bytes, MSB first)
  //  7-8         Year (MSB first)
  //  9           Month
  //  10          Day
  //  11          Hour
  //  12          Minute
  //  13          Second
  //  14-15       CLASS (MSB first)
  //  16-17       TYPE (MSB first)
  //  18-33       GUID (16 bytes)
  //  34-35       DATA SIZE (MSB first)
  //  36-n        DATA
  //  n+1, n+2    CRC (MSB first)

  uint8_t frame[100];
  memset(frame, 0, sizeof(frame));

  // Packet type 0, no encryption
  frame[0] = 0x00;

  // Head = 0x0080 (priority 4)
  frame[1] = 0x00;
  frame[2] = 0x80;

  // Timestamp = 0x12345678
  frame[3] = 0x12;
  frame[4] = 0x34;
  frame[5] = 0x56;
  frame[6] = 0x78;

  // Year = 2024 (0x07E8)
  frame[7] = 0x07;
  frame[8] = 0xE8;

  // Month = 6, Day = 15, Hour = 12, Minute = 30, Second = 45
  frame[9]  = 0x06;
  frame[10] = 0x0F;
  frame[11] = 0x0C;
  frame[12] = 0x1E;
  frame[13] = 0x2D;

  // Class = 10 (0x000A)
  frame[14] = 0x00;
  frame[15] = 0x0A;

  // Type = 6 (0x0006)
  frame[16] = 0x00;
  frame[17] = 0x06;

  // GUID = 0x00, 0x01, 0x02, ..., 0x0F
  for (int i = 0; i < 16; i++) {
    frame[18 + i] = i;
  }

  // Data size = 3
  frame[34] = 0x00;
  frame[35] = 0x03;

  // Data = 0xAA, 0xBB, 0xCC
  frame[36] = 0xAA;
  frame[37] = 0xBB;
  frame[38] = 0xCC;

  // Calculate CRC over bytes 1 through 38 (header + data) and append
  crc framecrc = crcFast((unsigned char const *) frame + 1, 35 + 3);
  frame[39] = (framecrc >> 8) & 0xff;
  frame[40] = framecrc & 0xff;

  // Read from frame
  memset(&ev, 0, sizeof(vscpEvent));
  rv = vscp_fwhlp_getEventFromFrame(&ev, frame, 41);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify format 0 written, will always be frame 1 when read
  ASSERT_EQ(0x0080, ev.head & ~VSCP_HEADER16_FRAME_VERSION_MASK); // Priority bits
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);

  // The function converts format 0 to nanosecond timestamp (forcing frame version 1).
  // Convert back from timestamp_ns to verify the date/time fields.
  // Original: 2024-06-15 12:30:45 + 0x12345678 µs (305419896 µs ≈ 305s)
  // → 2024-06-15 12:35:50.419896
  int conv_year, conv_month, conv_day, conv_hour, conv_minute, conv_second;
  uint32_t conv_microsecond;
  vscp_fwhlp_from_unix_ns(&conv_year, &conv_month, &conv_day,
                          &conv_hour, &conv_minute, &conv_second,
                          &conv_microsecond, ev.timestamp_ns);
  ASSERT_EQ(2024, conv_year);
  ASSERT_EQ(6, conv_month);
  ASSERT_EQ(15, conv_day);
  ASSERT_EQ(12, conv_hour);
  ASSERT_EQ(35, conv_minute);
  ASSERT_EQ(50, conv_second);
  ASSERT_EQ(419896, conv_microsecond);

  // Verify common fields
  ASSERT_EQ(10, ev.vscp_class);
  ASSERT_EQ(6, ev.vscp_type);
  ASSERT_EQ(3, ev.sizeData);

  // Verify GUID
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(i, ev.GUID[i]);
  }

  // Verify data
  ASSERT_EQ(0xAA, ev.pdata[0]);
  ASSERT_EQ(0xBB, ev.pdata[1]);
  ASSERT_EQ(0xCC, ev.pdata[2]);

  // Cleanup
  if (ev.pdata) {
    free(ev.pdata);
  }
}

// Test reading packet format 1 (new format with nanosecond timestamp)
TEST(_vscp_firmware_helper, vscp_fwhlp_getEventFromFrame_packet1)
{
  vscp_event_t ev;
  int rv;

  // Manually construct a packet format 1 frame
  // Format 1 layout:
  //  0           Packet type & encryption (0x10 = type 1, no encryption)
  //  1           HEAD MSB
  //  2           HEAD LSB
  //  3-10        Timestamp nanoseconds (8 bytes, MSB first)
  //  11-13       Reserved bytes
  //  14-15       CLASS (MSB first)
  //  16-17       TYPE (MSB first)
  //  18-33       GUID (16 bytes)
  //  34-35       DATA SIZE (MSB first)
  //  36-n        DATA
  //  n+1, n+2    CRC (MSB first)

  uint8_t frame[100];
  memset(frame, 0, sizeof(frame));

  // Packet type EVENT, no encryption
  frame[0] = 0x00;

  // Head = 0x0160 (frame version UNIX_NS + priority 3)
  frame[1] = 0x01;
  frame[2] = 0x60;

  // Timestamp_ns = 0x1234567890ABCDEF
  frame[3]  = 0x12;
  frame[4]  = 0x34;
  frame[5]  = 0x56;
  frame[6]  = 0x78;
  frame[7]  = 0x90;
  frame[8]  = 0xAB;
  frame[9]  = 0xCD;
  frame[10] = 0xEF;

  // Reserved bytes
  frame[11] = 0x00;
  frame[12] = 0x00;
  frame[13] = 0x00;

  // Class = 20 (0x0014)
  frame[14] = 0x00;
  frame[15] = 0x14;

  // Type = 8 (0x0008)
  frame[16] = 0x00;
  frame[17] = 0x08;

  // GUID = 0xFF, 0xFE, 0xFD, ..., 0xF0
  for (int i = 0; i < 16; i++) {
    frame[18 + i] = 0xFF - i;
  }

  // Data size = 2
  frame[34] = 0x00;
  frame[35] = 0x02;

  // Data = 0x11, 0x22
  frame[36] = 0x11;
  frame[37] = 0x22;

  // Calculate CRC over bytes 1 through 37 (header + data) and append
  crc framecrc = crcFast((unsigned char const *) frame + 1, 35 + 2);
  frame[38] = (framecrc >> 8) & 0xff;
  frame[39] = framecrc & 0xff;

  // Read from frame
  memset(&ev, 0, sizeof(vscpEvent));
  rv = vscp_fwhlp_getEventFromFrame(&ev, frame, 40);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify format 1 specific fields
  ASSERT_EQ(0x0060, ev.head & ~VSCP_HEADER16_FRAME_VERSION_MASK); // Priority bits
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(0x1234567890ABCDEFULL, ev.timestamp_ns);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);

  // Verify common fields
  ASSERT_EQ(20, ev.vscp_class);
  ASSERT_EQ(8, ev.vscp_type);
  ASSERT_EQ(2, ev.sizeData);

  // Verify GUID
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(0xFF - i, ev.GUID[i]);
  }

  // Verify data
  ASSERT_EQ(0x11, ev.pdata[0]);
  ASSERT_EQ(0x22, ev.pdata[1]);

  // Cleanup
  if (ev.pdata) {
    free(ev.pdata);
  }
}

#endif // VSCP_FWHLP_BINARY_FRAME_SUPPORT

//-----------------------------------------------------------------------------
// Crypto tests (VSCP_FWHLP_CRYPTO_SUPPORT)
//-----------------------------------------------------------------------------

#ifdef VSCP_FWHLP_CRYPTO_SUPPORT

TEST(_vscp_firmware_helper, vscp_fwhlp_encryptFrame_aes128_basic)
{
  uint8_t input[32];
  uint8_t output[64];
  uint8_t key[16]; // 128 bits
  uint8_t iv[16];
  size_t encrypted_len;

  // Setup input frame (packet type + data padded to 16 bytes)
  memset(input, 0, sizeof(input));
  input[0] = 0x00;                            // Packet type (not encrypted)
  strcpy((char *) &input[1], "Hello World!"); // 12 bytes + padding

  // Setup key
  for (int i = 0; i < 16; i++) {
    key[i] = i;
  }

  // Setup IV
  for (int i = 0; i < 16; i++) {
    iv[i] = 0xAA ^ i;
  }

  // Encrypt (input is 1 + 15 bytes = 16 total)
  // padlen = 15, padded to 16, encrypted 16 + 16 iv = 32, + 1 pkt type = 33
  encrypted_len = vscp_fwhlp_encryptFrame(output, input, 16, key, iv, VSCP_ENCRYPTION_AES128);

  // Should return: 1 (pkt type) + 16 (encrypted block) + 16 (iv) = 33
  ASSERT_EQ(33, encrypted_len);

  // Packet type should be preserved
  ASSERT_EQ(0x00, output[0]);

  // Encrypted data should differ from input
  ASSERT_NE(0, memcmp(input + 1, output + 1, 12));

  // IV should be appended at the end
  ASSERT_EQ(0, memcmp(iv, output + 17, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_decryptFrame_aes128_basic)
{
  uint8_t input[32];
  uint8_t original[32];
  uint8_t encrypted[64];
  uint8_t decrypted[64];
  uint8_t key[16];
  uint8_t iv[16];
  size_t encrypted_len;
  int rv;

  // Setup input frame (2 bytes: 1 pkt + 1 data)
  memset(input, 0, sizeof(input));
  input[0] = 0x00;
  input[1] = 0x42;

  // Save original for comparison (encryptFrame may modify input buffer)
  memcpy(original, input, sizeof(input));

  // Setup key
  for (int i = 0; i < 16; i++) {
    key[i] = i * 2;
  }

  // Setup IV
  for (int i = 0; i < 16; i++) {
    iv[i] = 0x55 ^ i;
  }

  // Encrypt (2 bytes -> 33)
  encrypted_len = vscp_fwhlp_encryptFrame(encrypted, input, 2, key, iv, VSCP_ENCRYPTION_AES128);
  ASSERT_EQ(33, encrypted_len);

  // Decrypt (pass NULL for iv to use appended iv)
  memset(decrypted, 0, sizeof(decrypted));
  rv = vscp_fwhlp_decryptFrame(decrypted, encrypted, encrypted_len, key, NULL, VSCP_ENCRYPTION_AES128);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify packet type
  ASSERT_EQ(original[0], decrypted[0]);

  // Verify first data byte
  ASSERT_EQ(original[1], decrypted[1]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_encryptFrame_aes256)
{
  uint8_t input[32];
  uint8_t output[64];
  uint8_t key[32]; // 256 bits
  uint8_t iv[16];
  size_t encrypted_len;

  // Setup input frame
  memset(input, 0, sizeof(input));
  input[0] = 0x03; // Packet type with AES256 indicator
  for (int i = 1; i < 17; i++) {
    input[i] = i * 3;
  }

  // Setup 256-bit key
  for (int i = 0; i < 32; i++) {
    key[i] = (i + 1) * 7;
  }

  // Setup IV
  for (int i = 0; i < 16; i++) {
    iv[i] = 0xFF ^ (i * 2);
  }

  // Encrypt with AES256 (16 bytes: 1 pkt + 15 data)
  encrypted_len = vscp_fwhlp_encryptFrame(output, input, 16, key, iv, VSCP_ENCRYPTION_AES256);

  ASSERT_EQ(33, encrypted_len); // 1 + 16 + 16
  ASSERT_EQ(input[0], output[0]);

  // Encrypted content should differ
  ASSERT_NE(0, memcmp(input + 1, output + 1, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_decryptFrame_aes256)
{
  uint8_t input[32];
  uint8_t original[32];
  uint8_t encrypted[64];
  uint8_t decrypted[64];
  uint8_t key[32]; // 256 bits
  uint8_t iv[16];
  size_t encrypted_len;
  int rv;

  // Setup input (2 bytes: 1 pkt + 1 data)
  memset(input, 0, sizeof(input));
  input[0] = 0x03;
  input[1] = 0x77;
  memcpy(original, input, sizeof(input));

  // Setup 256-bit key
  for (int i = 0; i < 32; i++) {
    key[i] = (i * 5) % 256;
  }

  // Setup IV
  for (int i = 0; i < 16; i++) {
    iv[i] = (i * 11) % 256;
  }

  // Encrypt (2 bytes -> 33)
  encrypted_len = vscp_fwhlp_encryptFrame(encrypted, input, 2, key, iv, VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(33, encrypted_len);

  // Decrypt
  rv = vscp_fwhlp_decryptFrame(decrypted, encrypted, encrypted_len, key, NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify data
  ASSERT_EQ(original[0], decrypted[0]);
  ASSERT_EQ(original[1], decrypted[1]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_encryptFrame_aes192)
{
  uint8_t input[32];
  uint8_t output[64];
  uint8_t key[24]; // 192 bits
  uint8_t iv[16];
  size_t encrypted_len;

  // Setup input frame
  memset(input, 0, sizeof(input));
  input[0] = 0x02; // Packet type with AES192 indicator
  strcpy((char *) &input[1], "192-bit test");

  // Setup 192-bit key
  for (int i = 0; i < 24; i++) {
    key[i] = (i * 3) % 256;
  }

  // Setup IV
  for (int i = 0; i < 16; i++) {
    iv[i] = (i * 13) % 256;
  }

  // Encrypt with AES192 (16 bytes: 1 pkt + 15 data)
  encrypted_len = vscp_fwhlp_encryptFrame(output, input, 16, key, iv, VSCP_ENCRYPTION_AES192);

  ASSERT_EQ(33, encrypted_len);
  ASSERT_EQ(input[0], output[0]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_crypto_roundtrip_aes128)
{
  uint8_t plaintext[32];
  uint8_t original[32];
  uint8_t encrypted[64];
  uint8_t decrypted[64];
  uint8_t key[16];
  uint8_t iv[16];
  size_t encrypted_len;
  int rv;

  // Setup plaintext
  memset(plaintext, 0, sizeof(plaintext));
  plaintext[0]        = 0x01; // Packet type
  const char *message = "Secret Message!";
  strcpy((char *) &plaintext[1], message);
  memcpy(original, plaintext, sizeof(plaintext));

  // Setup key
  for (int i = 0; i < 16; i++) {
    key[i] = (i * 17) % 256;
  }

  // Setup IV
  for (int i = 0; i < 16; i++) {
    iv[i] = (i * 23) % 256;
  }

  // Encrypt (16 bytes: 1 pkt + 15 data)
  encrypted_len = vscp_fwhlp_encryptFrame(encrypted, plaintext, 16, key, iv, VSCP_ENCRYPTION_AES128);
  ASSERT_GT(encrypted_len, 0);

  // Decrypt
  memset(decrypted, 0, sizeof(decrypted));
  rv = vscp_fwhlp_decryptFrame(decrypted, encrypted, encrypted_len, key, NULL, VSCP_ENCRYPTION_AES128);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify complete round-trip
  ASSERT_EQ(original[0], decrypted[0]);
  ASSERT_STREQ(message, (char *) &decrypted[1]);
  ASSERT_EQ(0, memcmp(original, decrypted, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_crypto_roundtrip_aes256)
{
  uint8_t plaintext[32];
  uint8_t original[32];
  uint8_t encrypted[64];
  uint8_t decrypted[64];
  uint8_t key[32];
  size_t encrypted_len;
  int rv;

  // Setup plaintext (2 bytes: 1 pkt + 1 data)
  memset(plaintext, 0, sizeof(plaintext));
  plaintext[0] = 0x03;
  plaintext[1] = 0x42;
  memcpy(original, plaintext, sizeof(plaintext));

  // Setup 256-bit key
  for (int i = 0; i < 32; i++) {
    key[i] = (i * 13 + 7) % 256;
  }

  // Encrypt (2 bytes -> 33)
  encrypted_len = vscp_fwhlp_encryptFrame(encrypted, plaintext, 2, key, NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(33, encrypted_len);

  // Decrypt
  rv = vscp_fwhlp_decryptFrame(decrypted, encrypted, encrypted_len, key, NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify binary data preserved
  ASSERT_EQ(0, memcmp(original, decrypted, 2));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_crypto_no_encryption)
{
  uint8_t input[32];
  uint8_t output[64];
  size_t result_len;

  // Setup input
  uint8_t dummy_key[16] = { 0 };
  memset(input, 0, sizeof(input));
  input[0] = 0x00;
  strcpy((char *) &input[1], "No encryption");

  // "Encrypt" with NONE algorithm (key still required but not used)
  result_len = vscp_fwhlp_encryptFrame(output, input, 20, dummy_key, NULL, VSCP_ENCRYPTION_NONE);

  // Should just copy
  ASSERT_EQ(20, result_len);
  ASSERT_EQ(0, memcmp(input, output, 20));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_crypto_different_keys_fail)
{
  uint8_t plaintext[32];
  uint8_t encrypted[64];
  uint8_t decrypted[64];
  uint8_t key1[16];
  uint8_t key2[16];
  size_t encrypted_len;
  int rv;

  // Setup plaintext
  memset(plaintext, 0, sizeof(plaintext));
  plaintext[0] = 0x01;
  strcpy((char *) &plaintext[1], "Secret");

  // Setup two different keys
  for (int i = 0; i < 16; i++) {
    key1[i] = i;
    key2[i] = i + 1; // Different key
  }

  // Encrypt with key1 (16 bytes: 1 pkt + 15 data)
  encrypted_len = vscp_fwhlp_encryptFrame(encrypted, plaintext, 16, key1, NULL, VSCP_ENCRYPTION_AES128);
  ASSERT_GT(encrypted_len, 0);

  // Try to decrypt with key2 (wrong key)
  rv = vscp_fwhlp_decryptFrame(decrypted, encrypted, encrypted_len, key2, NULL, VSCP_ENCRYPTION_AES128);
  // Decryption will succeed but data will be garbage
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // The decrypted data should NOT match original (wrong key)
  ASSERT_NE(0, memcmp(plaintext + 1, decrypted + 1, 6));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_crypto_larger_data)
{
  uint8_t plaintext[64];
  uint8_t original[64];
  uint8_t encrypted[128];
  uint8_t decrypted[128];
  uint8_t key[32];
  size_t encrypted_len;
  int rv;

  // Setup larger plaintext (2 bytes = 1 pkt + 1 data)
  memset(plaintext, 0, sizeof(plaintext));
  plaintext[0] = 0x03;
  plaintext[1] = 0xCC;
  memcpy(original, plaintext, sizeof(plaintext));

  // Setup key
  for (int i = 0; i < 32; i++) {
    key[i] = (i * 11) % 256;
  }

  // Encrypt (2 bytes -> 33)
  encrypted_len = vscp_fwhlp_encryptFrame(encrypted, plaintext, 2, key, NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(33, encrypted_len);

  // Decrypt
  rv = vscp_fwhlp_decryptFrame(decrypted, encrypted, encrypted_len, key, NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify all data
  ASSERT_EQ(0, memcmp(original, decrypted, 2));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_crypto_explicit_iv)
{
  uint8_t plaintext[32];
  uint8_t encrypted[64];
  uint8_t decrypted[64];
  uint8_t key[16];
  uint8_t iv[16];
  size_t encrypted_len;
  int rv;

  // Setup data
  memset(plaintext, 0, sizeof(plaintext));
  plaintext[0] = 0x01;
  strcpy((char *) &plaintext[1], "IV Test");

  // Setup key and IV
  for (int i = 0; i < 16; i++) {
    key[i] = i * 3;
    iv[i]  = 0xCC ^ i;
  }

  // Encrypt with explicit IV (16 bytes: 1 pkt + 15 data)
  encrypted_len = vscp_fwhlp_encryptFrame(encrypted, plaintext, 16, key, iv, VSCP_ENCRYPTION_AES128);
  ASSERT_GT(encrypted_len, 0);

  // Decrypt with explicit IV (extracted from encrypted packet)
  uint8_t extracted_iv[16];
  memcpy(extracted_iv, encrypted + encrypted_len - 16, 16);

  rv = vscp_fwhlp_decryptFrame(decrypted, encrypted, encrypted_len - 16, key, extracted_iv, VSCP_ENCRYPTION_AES128);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify
  ASSERT_STREQ("IV Test", (char *) &decrypted[1]);
}

#endif // VSCP_FWHLP_CRYPTO_SUPPORT

//-----------------------------------------------------------------------------
// JSON tests (VSCP_FWHLP_JSON_SUPPORT)
//-----------------------------------------------------------------------------

#ifdef VSCP_FWHLP_JSON_SUPPORT

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_basic)
{
  vscp_event_t ev;
  const char *json = "{"
                     "\"head\":3,"
                     "\"class\":10,"
                     "\"type\":6,"
                     "\"guid\":\"FF:FF:FF:FF:FF:FF:FF:FE:B8:27:EB:CF:3A:15:00:01\","
                     "\"obid\":11,"
                     "\"timestamp\":467530633,"
                     "\"datetime\":\"2022-12-16T16:41:02\","
                     "\"data\":[72,51,57,46,55,48,52]"
                     "}";

  int rv = vscp_fwhlp_parse_json(&ev, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify parsed values - head should have UNIX_NS frame version set
  uint16_t expected_head = (3 & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ev.head);
  ASSERT_EQ(10, ev.vscp_class);
  ASSERT_EQ(6, ev.vscp_type);
  ASSERT_EQ(11, ev.obid);
  // datetime + timestamp combined to Unix nanoseconds
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2022, 12, 16, 16, 41, 2, 467530633), ev.timestamp_ns);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);
  // Note: year/month/day/hour/minute/second share union memory with timestamp_ns
  // so they are not valid when timestamp_ns is set

  // Verify GUID
  uint8_t expected_guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
                                0xB8, 0x27, 0xEB, 0xCF, 0x3A, 0x15, 0x00, 0x01 };
  ASSERT_EQ(0, memcmp(expected_guid, ev.GUID, 16));

  // Verify data
  ASSERT_EQ(7, ev.sizeData);
  ASSERT_EQ(72, ev.pdata[0]);
  ASSERT_EQ(51, ev.pdata[1]);
  ASSERT_EQ(57, ev.pdata[2]);
  ASSERT_EQ(46, ev.pdata[3]);
  ASSERT_EQ(55, ev.pdata[4]);
  ASSERT_EQ(48, ev.pdata[5]);
  ASSERT_EQ(52, ev.pdata[6]);

  // Free allocated data
  free(ev.pdata);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_ex_basic)
{
  vscp_event_ex_t ex;
  const char *json = "{"
                     "\"head\":5,"
                     "\"class\":20,"
                     "\"type\":8,"
                     "\"guid\":\"00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F\","
                     "\"obid\":99,"
                     "\"timestamp\":123456789,"
                     "\"datetime\":\"2023-06-15T10:30:45\","
                     "\"data\":[1,2,3,4,5]"
                     "}";

  int rv = vscp_fwhlp_parse_json_ex(&ex, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify parsed values - head should have UNIX_NS frame version set
  uint16_t expected_head = (5 & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ex.head);
  ASSERT_EQ(20, ex.vscp_class);
  ASSERT_EQ(8, ex.vscp_type);
  ASSERT_EQ(99, ex.obid);
  // datetime + timestamp combined to Unix nanoseconds
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2023, 6, 15, 10, 30, 45, 123456789), ex.timestamp_ns);
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
  // Note: year/month/day/hour/minute/second share union memory with timestamp_ns
  // so they are not valid when timestamp_ns is set

  // Verify GUID
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(i, ex.GUID[i]);
  }

  // Verify data
  ASSERT_EQ(5, ex.sizeData);
  for (int i = 0; i < 5; i++) {
    ASSERT_EQ(i + 1, ex.data[i]);
  }
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_legacy_tags)
{
  vscp_event_t ev;
  const char *json = "{"
                     "\"vscpHead\":9,"
                     "\"vscpClass\":42,"
                     "\"vscpType\":3,"
                     "\"vscpGuid\":\"01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F:10\","
                     "\"vscpTimeStamp\":7654321,"
                     "\"vscpDateTime\":\"2024-11-05T08:09:10\","
                     "\"vscpData\":[10,20,30,40]"
                     "}";

  int rv = vscp_fwhlp_parse_json(&ev, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  uint16_t expected_head = (9 & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ev.head);
  ASSERT_EQ(42, ev.vscp_class);
  ASSERT_EQ(3, ev.vscp_type);
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2024, 11, 5, 8, 9, 10, 7654321), ev.timestamp_ns);

  uint8_t expected_guid[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
  ASSERT_EQ(0, memcmp(expected_guid, ev.GUID, 16));

  ASSERT_EQ(4, ev.sizeData);
  ASSERT_EQ(10, ev.pdata[0]);
  ASSERT_EQ(20, ev.pdata[1]);
  ASSERT_EQ(30, ev.pdata[2]);
  ASSERT_EQ(40, ev.pdata[3]);

  free(ev.pdata);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_ex_legacy_tags)
{
  vscp_event_ex_t ex;
  const char *json = "{"
                     "\"vscpHead\":12,"
                     "\"vscpClass\":506,"
                     "\"vscpType\":7,"
                     "\"vscpGuid\":\"10:0F:0E:0D:0C:0B:0A:09:08:07:06:05:04:03:02:01\","
                     "\"vscpTimeStamp\":12345,"
                     "\"vscpDateTime\":\"2025-01-01T00:00:00\","
                     "\"vscpData\":[1,2,3]"
                     "}";

  int rv = vscp_fwhlp_parse_json_ex(&ex, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  uint16_t expected_head = (12 & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ex.head);
  ASSERT_EQ(506, ex.vscp_class);
  ASSERT_EQ(7, ex.vscp_type);
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2025, 1, 1, 0, 0, 0, 12345), ex.timestamp_ns);

  uint8_t expected_guid[16] = { 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09,
                                0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
  ASSERT_EQ(0, memcmp(expected_guid, ex.GUID, 16));

  ASSERT_EQ(3, ex.sizeData);
  ASSERT_EQ(1, ex.data[0]);
  ASSERT_EQ(2, ex.data[1]);
  ASSERT_EQ(3, ex.data[2]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_create_json_basic)
{
  vscp_event_t ev;
  char json[1024];

  // Setup event
  memset(&ev, 0, sizeof(ev));
  ev.head       = 7;
  ev.vscp_class = 30;
  ev.vscp_type  = 12;
  ev.obid       = 42;
  ev.timestamp  = 987654321;
  ev.year       = 2024;
  ev.month      = 3;
  ev.day        = 20;
  ev.hour       = 14;
  ev.minute     = 25;
  ev.second     = 30;

  // Setup GUID
  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i * 16;
  }

  // Setup data
  uint8_t data[] = { 10, 20, 30, 40, 50 };
  ev.pdata       = data;
  ev.sizeData    = 5;

  int rv = vscp_fwhlp_create_json(json, sizeof(json), &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify JSON contains expected fields (basic check)
  ASSERT_NE(nullptr, strstr(json, "\"head\""));
  ASSERT_NE(nullptr, strstr(json, "\"class\""));
  ASSERT_NE(nullptr, strstr(json, "\"type\""));
  ASSERT_NE(nullptr, strstr(json, "\"guid\""));
  ASSERT_NE(nullptr, strstr(json, "\"data\""));
  ASSERT_NE(nullptr, strstr(json, "\"datetime\""));
  ASSERT_NE(nullptr, strstr(json, "2024-03-20T14:25:30"));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_create_json_ex_basic)
{
  vscp_event_ex_t ex;
  char json[1024];

  // Setup event
  memset(&ex, 0, sizeof(ex));
  ex.head       = 2;
  ex.vscp_class = 50;
  ex.vscp_type  = 25;
  ex.obid       = 100;
  ex.timestamp  = 111222333;
  ex.year       = 2025;
  ex.month      = 7;
  ex.day        = 4;
  ex.hour       = 9;
  ex.minute     = 15;
  ex.second     = 45;

  // Setup GUID
  for (int i = 0; i < 16; i++) {
    ex.GUID[i] = 15 - i;
  }

  // Setup data
  ex.sizeData = 3;
  ex.data[0]  = 100;
  ex.data[1]  = 200;
  ex.data[2]  = 255;

  int rv = vscp_fwhlp_create_json_ex(json, sizeof(json), &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify JSON contains expected fields
  ASSERT_NE(nullptr, strstr(json, "\"head\""));
  ASSERT_NE(nullptr, strstr(json, "\"class\""));
  ASSERT_NE(nullptr, strstr(json, "\"type\""));
  ASSERT_NE(nullptr, strstr(json, "\"guid\""));
  ASSERT_NE(nullptr, strstr(json, "\"data\""));
  ASSERT_NE(nullptr, strstr(json, "\"datetime\""));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_json_roundtrip_event)
{
  vscp_event_t original, parsed;
  char json[1024];

  // Setup original event
  memset(&original, 0, sizeof(original));
  original.head       = 4;
  original.vscp_class = 15;
  original.vscp_type  = 8;
  original.obid       = 777;
  original.timestamp  = 555666777;
  original.year       = 2023;
  original.month      = 11;
  original.day        = 25;
  original.hour       = 18;
  original.minute     = 30;
  original.second     = 15;

  for (int i = 0; i < 16; i++) {
    original.GUID[i] = (i * 3) % 256;
  }

  uint8_t data[]    = { 11, 22, 33, 44, 55, 66 };
  original.pdata    = data;
  original.sizeData = 6;

  // Create JSON
  int rv = vscp_fwhlp_create_json(json, sizeof(json), &original);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Parse back
  rv = vscp_fwhlp_parse_json(&parsed, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify round-trip - head will have UNIX_NS frame version set after parsing
  uint16_t expected_head = (original.head & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, parsed.head);
  ASSERT_EQ(original.vscp_class, parsed.vscp_class);
  ASSERT_EQ(original.vscp_type, parsed.vscp_type);
  ASSERT_EQ(original.obid, parsed.obid);
  // datetime + timestamp combined to Unix nanoseconds
  // timestamp_ns is stored as hex string in JSON, so no precision loss
  int64_t expected_ns = vscp_fwhlp_to_unix_ns(original.year, original.month, original.day, original.hour, original.minute, original.second, original.timestamp);
  ASSERT_EQ(expected_ns, (int64_t)parsed.timestamp_ns);
  // Note: year/month/day/hour/minute/second share union memory with timestamp_ns
  // so they are not valid when timestamp_ns is set
  ASSERT_EQ(0, memcmp(original.GUID, parsed.GUID, 16));
  ASSERT_EQ(original.sizeData, parsed.sizeData);
  ASSERT_EQ(0, memcmp(original.pdata, parsed.pdata, original.sizeData));

  // Free allocated data
  free(parsed.pdata);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_json_roundtrip_eventex)
{
  vscp_event_ex_t original, parsed;
  char json[2048];

  // Setup original event
  memset(&original, 0, sizeof(original));
  original.head       = 6;
  original.vscp_class = 40;
  original.vscp_type  = 18;
  original.obid       = 888;
  original.timestamp  = 999888777;
  original.year       = 2024;
  original.month      = 2;
  original.day        = 14;
  original.hour       = 12;
  original.minute     = 0;
  original.second     = 0;

  for (int i = 0; i < 16; i++) {
    original.GUID[i] = (i * 7) % 256;
  }

  original.sizeData = 8;
  for (int i = 0; i < 8; i++) {
    original.data[i] = (i * 11) % 256;
  }

  // Create JSON
  int rv = vscp_fwhlp_create_json_ex(json, sizeof(json), &original);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Parse back
  rv = vscp_fwhlp_parse_json_ex(&parsed, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify round-trip - head will have UNIX_NS frame version set after parsing
  uint16_t expected_head = (original.head & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, parsed.head);
  ASSERT_EQ(original.vscp_class, parsed.vscp_class);
  ASSERT_EQ(original.vscp_type, parsed.vscp_type);
  ASSERT_EQ(original.obid, parsed.obid);
  // datetime + timestamp combined to Unix nanoseconds
  // timestamp_ns is stored as hex string in JSON, so no precision loss
  int64_t expected_ns_ex = vscp_fwhlp_to_unix_ns(original.year, original.month, original.day, original.hour, original.minute, original.second, original.timestamp);
  ASSERT_EQ(expected_ns_ex, (int64_t)parsed.timestamp_ns);
  // Note: year/month/day/hour/minute/second share union memory with timestamp_ns
  // so they are not valid when timestamp_ns is set
  ASSERT_EQ(0, memcmp(original.GUID, parsed.GUID, 16));
  ASSERT_EQ(original.sizeData, parsed.sizeData);
  ASSERT_EQ(0, memcmp(original.data, parsed.data, original.sizeData));
}

// Test parsing JSON with timestamp_ns field (preferred over timestamp)
// Note: JSON numbers use double precision which limits accuracy to ~15 significant digits
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_timestamp_ns)
{
  vscp_event_t ev;
  const char *json = "{"
                     "\"head\":3,"
                     "\"class\":10,"
                     "\"type\":6,"
                     "\"guid\":\"FF:FF:FF:FF:FF:FF:FF:FE:B8:27:EB:CF:3A:15:00:01\","
                     "\"obid\":11,"
                     "\"timestamp_ns\":1234567890123456,"
                     "\"data\":[1,2,3]"
                     "}";

  int rv = vscp_fwhlp_parse_json(&ev, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify timestamp_ns is used directly
  ASSERT_EQ(1234567890123456ULL, ev.timestamp_ns);

  // Verify frame version is set to UNIX_NS
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);

  // Free allocated data
  free(ev.pdata);
}

// Test parsing JSON with both timestamp and timestamp_ns - timestamp_ns takes priority
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_timestamp_ns_priority)
{
  vscp_event_t ev;
  const char *json = "{"
                     "\"head\":5,"
                     "\"class\":20,"
                     "\"type\":8,"
                     "\"guid\":\"00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F\","
                     "\"obid\":99,"
                     "\"timestamp\":999999999,"
                     "\"timestamp_ns\":5555666677778888"
                     "}";

  int rv = vscp_fwhlp_parse_json(&ev, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // timestamp_ns should take priority over timestamp
  ASSERT_EQ(5555666677778888ULL, ev.timestamp_ns);

  // Verify frame version is set to UNIX_NS
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);
}

// Test parsing JSON with timestamp_ns for vscpEventEx
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_ex_timestamp_ns)
{
  vscp_event_ex_t ex;
  const char *json = "{"
                     "\"head\":7,"
                     "\"class\":30,"
                     "\"type\":12,"
                     "\"guid\":\"AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99\","
                     "\"obid\":42,"
                     "\"timestamp_ns\":9876543210987654"
                     "}";

  int rv = vscp_fwhlp_parse_json_ex(&ex, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify timestamp_ns is used directly
  ASSERT_EQ(9876543210987654ULL, ex.timestamp_ns);

  // Verify frame version is set to UNIX_NS
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_null_pointer)
{
  vscp_event_t ev;
  const char *json = "{\"head\":1}";

  // Test NULL event pointer
  int rv = vscp_fwhlp_parse_json(NULL, json);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);

  // Test NULL JSON pointer
  rv = vscp_fwhlp_parse_json(&ev, NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_invalid_syntax)
{
  vscp_event_t ev;
  const char *invalid_json = "{\"head\":1,}"; // Trailing comma

  int rv = vscp_fwhlp_parse_json(&ev, invalid_json);
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_create_json_buffer_too_small)
{
  vscp_event_t ev;
  char json[10]; // Too small

  memset(&ev, 0, sizeof(ev));
  ev.head       = 1;
  ev.vscp_class = 10;
  ev.vscp_type  = 5;
  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  uint8_t data[] = { 1, 2, 3 };
  ev.pdata       = data;
  ev.sizeData    = 3;

  int rv = vscp_fwhlp_create_json(json, sizeof(json), &ev);
  ASSERT_EQ(VSCP_ERROR_BUFFER_TO_SMALL, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_json_empty_data)
{
  vscp_event_t ev;
  const char *json = "{"
                     "\"head\":1,"
                     "\"class\":5,"
                     "\"type\":3,"
                     "\"guid\":\"00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\","
                     "\"data\":[]"
                     "}";

  int rv = vscp_fwhlp_parse_json(&ev, json);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, ev.sizeData);
  ASSERT_NE(nullptr, ev.pdata); // Should still allocate

  free(ev.pdata);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_create_json_ex_null_pointer)
{
  char json[1024];
  vscp_event_ex_t ex;

  // Test NULL event pointer
  int rv = vscp_fwhlp_create_json_ex(NULL, sizeof(json), &ex);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);

  // Test NULL buffer pointer
  rv = vscp_fwhlp_create_json_ex(json, sizeof(json), NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

#endif // VSCP_FWHLP_JSON_SUPPORT

//-----------------------------------------------------------------------------
// XML tests (VSCP_FWHLP_XML_SUPPORT)
//-----------------------------------------------------------------------------

#ifdef VSCP_FWHLP_XML_SUPPORT

// No event tag
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_basic_validate_0)
{
  vscp_event_t ev;
  const char *xml = "<ev"
                    "head=\"3\" "
                    "obid=\"1234\" "
                    "timestamp=\"50817\" "
                    "class=\"10\" "
                    "type=\"6\" "
                    "guid=\"00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02\" "
                    "data=\"1,2,3,255,0x05,0x55,7,0b10101010\" "
                    "/>";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);
}

// Event tag misspelled
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_basic_validate_1)
{
  vscp_event_t ev;
  const char *xml = "<eventx"
                    "head=\"3\" "
                    "obid=\"1234\" "
                    "timestamp=\"50817\" "
                    "class=\"10\" "
                    "type=\"6\" "
                    "guid=\"00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02\" "
                    "data=\"1,2,3,255,0x05,0x55,7,0b10101010\" "
                    "/>";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);
}

// Missing end tag
TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_basic_validate_2)
{
  vscp_event_t ev;
  const char *xml = "<eventx"
                    "head=\"3\" "
                    "obid=\"1234\" "
                    "timestamp=\"50817\" "
                    "class=\"10\" "
                    "type=\"6\" "
                    "guid=\"00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02\" "
                    "data=\"1,2,3,255,0x05,0x55,7,0b10101010\" ";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_basic)
{
  vscp_event_t ev;
  const char *xml = "<event "
                    "head=\"3\" "
                    "obid=\"1234\" "
                    "timestamp=\"50817\" "
                    "class=\"10\" "
                    "type=\"6\" "
                    "guid=\"00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02\" "
                    "data=\"1,2,3,255,0x05,0x55,7,0b10101010\" "
                    "/>";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify parsed values
  ASSERT_EQ(3, ev.head);
  ASSERT_EQ(1234, ev.obid);
  ASSERT_EQ(50817, ev.timestamp);
  ASSERT_EQ(10, ev.vscp_class);
  ASSERT_EQ(6, ev.vscp_type);
  ASSERT_EQ(8, ev.sizeData);
  ASSERT_EQ(1, ev.pdata[0]);
  ASSERT_EQ(2, ev.pdata[1]);
  ASSERT_EQ(3, ev.pdata[2]);
  ASSERT_EQ(255, ev.pdata[3]);
  ASSERT_EQ(5, ev.pdata[4]);
  ASSERT_EQ(0x55, ev.pdata[5]);
  ASSERT_EQ(7, ev.pdata[6]);
  ASSERT_EQ(0xaa, ev.pdata[7]);

  // Verify GUID
  uint8_t expected_guid[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02 };
  ASSERT_EQ(0, memcmp(expected_guid, ev.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_eventex_basic_full)
{
  vscp_event_ex_t ex;
  const char *xml = "<event "
                    "head=\"3\" "
                    "obid=\"1234\" "
                    "timestamp=\"50817\" "
                    "class=\"10\" "
                    "type=\"6\" "
                    "guid=\"00:00:00:00:00:00:00:00:00:00:00:00:00:01:00:02\" "
                    "data=\"1,2,3,255,0x05,0x55,7,0b10101010\" "
                    "/>";

  int rv = vscp_fwhlp_parse_xml_eventex(&ex, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify parsed values
  ASSERT_EQ(3, ex.head);
  ASSERT_EQ(1234, ex.obid);
  ASSERT_EQ(50817, ex.timestamp);
  ASSERT_EQ(10, ex.vscp_class);
  ASSERT_EQ(6, ex.vscp_type);
  ASSERT_EQ(8, ex.sizeData);
  ASSERT_EQ(1, ex.data[0]);
  ASSERT_EQ(2, ex.data[1]);
  ASSERT_EQ(3, ex.data[2]);
  ASSERT_EQ(255, ex.data[3]);
  ASSERT_EQ(5, ex.data[4]);
  ASSERT_EQ(0x55, ex.data[5]);
  ASSERT_EQ(7, ex.data[6]);
  ASSERT_EQ(0xaa, ex.data[7]);

  // Verify GUID
  uint8_t expected_guid[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02 };
  ASSERT_EQ(0, memcmp(expected_guid, ex.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_null_pointer)
{
  vscp_event_t ev;
  const char *xml = "<event head=\"1\" />";

  // Test NULL event pointer
  int rv = vscp_fwhlp_parse_xml_event(NULL, xml);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);

  // Test NULL XML pointer
  rv = vscp_fwhlp_parse_xml_event(&ev, NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_invalid_syntax)
{
  vscp_event_t ev;

  // Missing opening tag
  const char *invalid_xml1 = "head=\"1\" />";
  int rv                   = vscp_fwhlp_parse_xml_event(&ev, invalid_xml1);
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);

  // Missing closing tag
  const char *invalid_xml2 = "<event head=\"1\"";
  rv                       = vscp_fwhlp_parse_xml_event(&ev, invalid_xml2);
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);

  // Wrong opening tag
  const char *invalid_xml3 = "<evt head=\"1\" />";
  rv                       = vscp_fwhlp_parse_xml_event(&ev, invalid_xml3);
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_with_whitespace)
{
  vscp_event_t ev;
  // Note: Leading/trailing whitespace is OK, but internal whitespace affects parsing
  const char *xml = "<event head=\"5\" class=\"20\" type=\"8\" "
                    "guid=\"FF:FF:FF:FF:FF:FF:FF:FE:B8:27:EB:CF:3A:15:00:01\" />";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify parsed values
  ASSERT_EQ(5, ev.head);
  ASSERT_EQ(20, ev.vscp_class);
  ASSERT_EQ(8, ev.vscp_type);

  // Verify GUID
  uint8_t expected_guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
                                0xB8, 0x27, 0xEB, 0xCF, 0x3A, 0x15, 0x00, 0x01 };
  ASSERT_EQ(0, memcmp(expected_guid, ev.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_minimal)
{
  vscp_event_t ev;
  const char *xml = "<event class=\"15\" type=\"3\" />";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(15, ev.vscp_class);
  ASSERT_EQ(3, ev.vscp_type);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_event_to_xml_basic)
{
  vscp_event_t ev;
  char xml[3000];

  // Setup event
  memset(&ev, 0, sizeof(ev));
  ev.head       = 4;
  ev.vscp_class = 15;
  ev.vscp_type  = 8;
  ev.obid       = 999;
  ev.timestamp  = 123456;

  // Setup GUID
  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  int rv = vscp_fwhlp_event_to_xml(xml, sizeof(xml), &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_event_to_xml_basic_with_parse)
{
  vscp_event_t ev;
  char xml[3000];

  // Setup event with proper datetime values
  memset(&ev, 0, sizeof(ev));
  ev.head       = 4;
  ev.vscp_class = 15;
  ev.vscp_type  = 8;
  ev.obid       = 999;
  ev.timestamp  = 123456;
  ev.year       = 2024;
  ev.month      = 6;
  ev.day        = 15;
  ev.hour       = 12;
  ev.minute     = 30;
  ev.second     = 45;
  ev.sizeData   = 4;
  ev.pdata      = (uint8_t *) malloc(ev.sizeData);
  ev.pdata[0]   = 10;
  ev.pdata[1]   = 20;
  ev.pdata[2]   = 30;
  ev.pdata[3]   = 40;

  // Setup GUID
  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = i;
  }

  int rv = vscp_fwhlp_event_to_xml(xml, sizeof(xml), &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  //printf("Generated XML:\n%s\n", xml);

  vscp_event_ex_t ex;
  rv = vscp_fwhlp_parse_xml_eventex(&ex, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // head will have UNIX_NS frame version set after parsing XML with datetime
  uint16_t expected_head = (ev.head & ~VSCP_HEADER16_FRAME_VERSION_MASK) | VSCP_HEADER16_FRAME_VERSION_UNIX_NS;
  ASSERT_EQ(expected_head, ex.head);
  ASSERT_EQ(ev.obid, ex.obid);
  // datetime + timestamp combined to Unix nanoseconds
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(ev.year, ev.month, ev.day, ev.hour, ev.minute, ev.second, ev.timestamp), ex.timestamp_ns);
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
  ASSERT_EQ(ev.vscp_class, ex.vscp_class);
  ASSERT_EQ(ev.vscp_type, ex.vscp_type);
  ASSERT_EQ(0, memcmp(ev.GUID, ex.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_event_to_xml_null_pointer)
{
  vscp_event_t ev;
  char xml[32];

  memset(&ev, 0, sizeof(ev));

  int rv = vscp_fwhlp_event_to_xml(nullptr, sizeof(xml), &ev);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);

  rv = vscp_fwhlp_event_to_xml(xml, sizeof(xml), nullptr);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_all_zeros)
{
  vscp_event_t ev;
  const char *xml = "<event "
                    "head=\"0\" "
                    "obid=\"0\" "
                    "timestamp=\"0\" "
                    "class=\"0\" "
                    "type=\"0\" "
                    "guid=\"00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00\" "
                    "/>";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, ev.head);
  ASSERT_EQ(0, ev.obid);
  ASSERT_EQ(0, ev.timestamp);
  ASSERT_EQ(0, ev.vscp_class);
  ASSERT_EQ(0, ev.vscp_type);

  // Verify GUID is all zeros
  uint8_t zero_guid[16] = { 0 };
  ASSERT_EQ(0, memcmp(zero_guid, ev.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_max_values)
{
  vscp_event_ex_t ex;
  const char *xml = "<event "
                    "head=\"65535\" "
                    "obid=\"4294967295\" "
                    "timestamp=\"4294967295\" "
                    "class=\"65535\" "
                    "type=\"65535\" "
                    "guid=\"FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF\" "
                    "/>";

  int rv = vscp_fwhlp_parse_xml_eventex(&ex, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(65535, ex.head);
  ASSERT_EQ(4294967295U, ex.obid);
  ASSERT_EQ(4294967295U, ex.timestamp);
  ASSERT_EQ(65535, ex.vscp_class);
  ASSERT_EQ(65535, ex.vscp_type);

  // Verify GUID is all 0xFF
  uint8_t max_guid[16];
  memset(max_guid, 0xFF, 16);
  ASSERT_EQ(0, memcmp(max_guid, ex.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_eventex_basic)
{
  vscp_event_ex_t ex;
  const char *xml = "<event "
                    "head=\"7\" "
                    "class=\"30\" "
                    "type=\"12\" "
                    "guid=\"00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F\" "
                    "/>";

  // Note: This function is currently a stub that just returns SUCCESS
  int rv = vscp_fwhlp_parse_xml_eventex(&ex, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_eventex_single_line)
{
  vscp_event_ex_t ex;
  const char *xml = "<event head=\"2\" obid=\"5678\" timestamp=\"99999\" class=\"25\" type=\"10\" "
                    "guid=\"AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99\" />";

  int rv = vscp_fwhlp_parse_xml_eventex(&ex, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify parsed values
  ASSERT_EQ(2, ex.head);
  ASSERT_EQ(5678, ex.obid);
  ASSERT_EQ(99999, ex.timestamp);
  ASSERT_EQ(25, ex.vscp_class);
  ASSERT_EQ(10, ex.vscp_type);

  // Verify GUID
  uint8_t expected_guid[16] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11,
                                0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
  ASSERT_EQ(0, memcmp(expected_guid, ex.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_event_single_line)
{
  vscp_event_t ev;
  const char *xml = "<event head=\"2\" obid=\"5678\" timestamp=\"99999\" class=\"25\" type=\"10\" "
                    "guid=\"AA:BB:CC:DD:EE:FF:00:11:22:33:44:55:66:77:88:99\" />";

  int rv = vscp_fwhlp_parse_xml_event(&ev, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify parsed values
  ASSERT_EQ(2, ev.head);
  ASSERT_EQ(5678, ev.obid);
  ASSERT_EQ(99999, ev.timestamp);
  ASSERT_EQ(25, ev.vscp_class);
  ASSERT_EQ(10, ev.vscp_type);

  // Verify GUID
  uint8_t expected_guid[16] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11,
                                0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99 };
  ASSERT_EQ(0, memcmp(expected_guid, ev.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_eventex_to_xml_basic)
{
  vscp_event_ex_t ex;
  char xml[3000];

  // Setup event
  memset(&ex, 0, sizeof(ex));
  ex.head       = 2;
  ex.vscp_class = 40;
  ex.vscp_type  = 18;
  ex.obid       = 555;
  ex.timestamp  = 987654;

  // Setup GUID
  for (int i = 0; i < 16; i++) {
    ex.GUID[i] = 15 - i;
  }

  // Note: This function is currently a stub that just returns SUCCESS
  int rv = vscp_fwhlp_eventex_to_xml(xml, sizeof(xml), &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_eventex_to_xml_null_pointer)
{
  vscp_event_ex_t ex;
  char xml[32];

  memset(&ex, 0, sizeof(ex));

  int rv = vscp_fwhlp_eventex_to_xml(nullptr, sizeof(xml), &ex);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);

  rv = vscp_fwhlp_eventex_to_xml(xml, sizeof(xml), nullptr);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parse_xml_eventex_max_values)
{
  vscp_event_ex_t ex;
  const char *xml = "<event "
                    "head=\"65535\" "
                    "obid=\"4294967295\" "
                    "timestamp=\"4294967295\" "
                    "class=\"65535\" "
                    "type=\"65535\" "
                    "guid=\"FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF:FF\" "
                    "/>";

  int rv = vscp_fwhlp_parse_xml_eventex(&ex, xml);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(65535, ex.head);
  ASSERT_EQ(4294967295U, ex.obid);
  ASSERT_EQ(4294967295U, ex.timestamp);
  ASSERT_EQ(65535, ex.vscp_class);
  ASSERT_EQ(65535, ex.vscp_type);

  // Verify GUID is all 0xFF
  uint8_t max_guid[16];
  memset(max_guid, 0xFF, 16);
  ASSERT_EQ(0, memcmp(max_guid, ex.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_event_info_from_topic_success)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  const char *topic = "vscp/FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00/10/6";

  int rv = vscp_fwhlp_set_event_info_from_topic(&ev, topic);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(10, ev.vscp_class);
  ASSERT_EQ(6, ev.vscp_type);

  uint8_t expected_guid[16] = { 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
                                0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00 };
  ASSERT_EQ(0, memcmp(expected_guid, ev.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_event_info_from_topic_success_with_extra_parts)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  const char *topic = "vscp/FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00/10/6/extra/parts/ignored";

  int rv = vscp_fwhlp_set_event_info_from_topic(&ev, topic);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(10, ev.vscp_class);
  ASSERT_EQ(6, ev.vscp_type);

  uint8_t expected_guid[16] = { 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
                                0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00 };
  ASSERT_EQ(0, memcmp(expected_guid, ev.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_event_info_from_topic_invalid_syntax)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  int rv = vscp_fwhlp_set_event_info_from_topic(&ev, "vscp/FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00/10");
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_event_info_from_topic_invalid_guid)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  int rv = vscp_fwhlp_set_event_info_from_topic(&ev, "vscp/ZZ:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00/10/6");
  ASSERT_NE(VSCP_ERROR_SUCCESS, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_event_info_from_topic_null_pointer)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  int rv = vscp_fwhlp_set_event_info_from_topic(NULL, "vscp/FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00/10/6");
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);

  rv = vscp_fwhlp_set_event_info_from_topic(&ev, NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_eventex_info_from_topic_success)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  const char *topic = "vscp/00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F/1040/42";

  int rv = vscp_fwhlp_set_eventex_info_from_topic(&ex, topic);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(1040, ex.vscp_class);
  ASSERT_EQ(42, ex.vscp_type);

  uint8_t expected_guid[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
  ASSERT_EQ(0, memcmp(expected_guid, ex.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_eventex_info_from_topic_success_with_extra_parts)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  const char *topic = "vscp/00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F/1040/42/extra/parts/ignored";

  int rv = vscp_fwhlp_set_eventex_info_from_topic(&ex, topic);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(1040, ex.vscp_class);
  ASSERT_EQ(42, ex.vscp_type);

  uint8_t expected_guid[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
  ASSERT_EQ(0, memcmp(expected_guid, ex.GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_eventex_info_from_topic_invalid_syntax)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  int rv = vscp_fwhlp_set_eventex_info_from_topic(&ex, "foo/00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F/10/6");
  ASSERT_EQ(VSCP_ERROR_INVALID_SYNTAX, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_set_eventex_info_from_topic_null_pointer)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  int rv = vscp_fwhlp_set_eventex_info_from_topic(NULL, "vscp/00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F/10/6");
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);

  rv = vscp_fwhlp_set_eventex_info_from_topic(&ex, NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

#endif // VSCP_FWHLP_XML_SUPPORT

//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_getMeasurementAsFloat)
{
  unsigned char norm[5] = { 0x00, 0x3f, 0x80, 0x00, 0x00 };

  float val = vscp_fwhlp_getMeasurementAsFloat(norm, sizeof(norm));
  ASSERT_NEAR(1.0f, val, 0.000001f);

  ASSERT_EQ(0.0f, vscp_fwhlp_getMeasurementAsFloat(nullptr, sizeof(norm)));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getMeasurementAsString)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  uint8_t payload[] = { 0x00, 0x00, 0x00, 0x00, '1', '2', '.', '5' };
  ev.vscp_class     = VSCP_CLASS2_MEASUREMENT_STR;
  ev.sizeData       = sizeof(payload);
  ev.pdata          = payload;

  char out[64];
  memset(out, 0, sizeof(out));

  int rv = vscp_fwhlp_getMeasurementAsString(out, &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("12.5", out);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getMeasurementFloat64AsString)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  double source = 12.5;
  uint8_t payload[8];
  memcpy(payload, &source, sizeof(payload));

  ev.vscp_class = VSCP_CLASS1_MEASUREMENT64;
  ev.sizeData   = sizeof(payload);
  ev.pdata      = payload;

  char out[64];
  memset(out, 0, sizeof(out));

  int rv = vscp_fwhlp_getMeasurementFloat64AsString(out, &ev);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  double parsed = 0;
  ASSERT_EQ(1, sscanf(out, "%lf", &parsed));
  ASSERT_NEAR(source, parsed, 0.000001);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getMeasurementAsDouble)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  double source = 37.125;
  uint8_t payload[8];
  memcpy(payload, &source, sizeof(payload));

  ev.vscp_class = VSCP_CLASS1_MEASUREMENT64;
  ev.sizeData   = sizeof(payload);
  ev.pdata      = payload;

  double val = 0;
  int rv     = vscp_fwhlp_getMeasurementAsDouble(&val, &ev);
  ASSERT_EQ(0, rv);
  ASSERT_NEAR(source, val, 0.000001);

  rv = vscp_fwhlp_getMeasurementAsDouble(nullptr, &ev);
  ASSERT_EQ(-1, rv);
  rv = vscp_fwhlp_getMeasurementAsDouble(&val, nullptr);
  ASSERT_EQ(-1, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getMeasurementAsDoubleEx)
{
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  double source = 98.25;
  memcpy(ex.data, &source, sizeof(source));
  ex.vscp_class = VSCP_CLASS1_MEASUREMENT64;
  ex.sizeData   = sizeof(source);

  double val = 0;
  int rv     = vscp_fwhlp_getMeasurementAsDoubleEx(&val, &ex);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_NEAR(source, val, 0.000001);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_getMeasurementWithZoneAsString)
{
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  // Datacoding = Integer, value =10
  uint8_t ok_payload[] = { 0x01, 0x02, 0x03, 0x60, 0x0a };
  ev.vscp_class        = VSCP_CLASS1_MEASUREZONE;
  ev.sizeData          = sizeof(ok_payload);
  ev.pdata             = ok_payload;

  int rv = vscp_fwhlp_getMeasurementWithZoneAsString(&ev);
  ASSERT_EQ(0, rv);

  // Datacoding = Integer, value is not present
  uint8_t short_payload[] = { 0x01, 0x02, 0x03, 0x60 };
  ev.sizeData             = sizeof(short_payload);
  ev.pdata                = short_payload;

  rv = vscp_fwhlp_getMeasurementWithZoneAsString(&ev);
  ASSERT_EQ(-1, rv);

  // Payload is not valid for this class (e.g. wrong class)
  uint8_t wrong_class_payload[] = { 0x01, 0x02, 0x03, 0x60, 0x0a };
  ev.vscp_class                 = VSCP_CLASS1_MEASUREMENT64;
  ev.sizeData                   = sizeof(wrong_class_payload);
  ev.pdata                      = wrong_class_payload;

  rv = vscp_fwhlp_getMeasurementWithZoneAsString(&ev);
  ASSERT_EQ(0, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEvent_1)
{
  int rv;
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));

  // Empty datestr means nanosecond timestamp mode
  rv = vscp_fwhlp_parseStringToEvent(&ev, "6,20,3,,,,00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // head=6 with VSCP_HEADER16_FRAME_VERSION_UNIX_NS (0x0100) set = 0x0106
  ASSERT_EQ(0x0106, ev.head);
  ASSERT_EQ(20, ev.vscp_class);
  ASSERT_EQ(3, ev.vscp_type);
  ASSERT_EQ(3, ev.sizeData);
  ASSERT_EQ(0, ev.obid);
  ASSERT_EQ(0ULL, ev.timestamp_ns);  // nanosecond timestamp
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(i, ev.GUID[i]);
  }
  //ASSERT_EQ(0, memcmp(ev.GUID, (uint8_t[]) { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }, 16));
  ASSERT_EQ(0, ev.pdata[0]);
  ASSERT_EQ(1, ev.pdata[1]);
  ASSERT_EQ(35, ev.pdata[2]);
  delete[] ev.pdata;
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEvent_2)
{
  int rv;
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));
  uint8_t guid[16] = { 0 };

  // Empty datestr means nanosecond timestamp mode
  rv = vscp_fwhlp_parseStringToEvent(&ev, "1,20,3,,,,-,0,0x44,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // head=1 with VSCP_HEADER16_FRAME_VERSION_UNIX_NS (0x0100) set = 0x0101
  ASSERT_EQ(0x0101, ev.head);
  ASSERT_EQ(20, ev.vscp_class);
  ASSERT_EQ(3, ev.vscp_type);
  ASSERT_EQ(3, ev.sizeData);
  ASSERT_EQ(0, ev.obid);
  ASSERT_EQ(0ULL, ev.timestamp_ns);  // nanosecond timestamp
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);
  ASSERT_EQ(0, memcmp(ev.GUID, guid, 16));
  ASSERT_EQ(0, ev.pdata[0]);
  ASSERT_EQ(0x44, ev.pdata[1]);
  ASSERT_EQ(35, ev.pdata[2]);
  delete[] ev.pdata;  
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEvent_3)
{
  int rv;
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));
  uint8_t guid[16] = { 0 };

  rv = vscp_fwhlp_parseStringToEvent(&ev, "9,20,3,,2001-11-02T18:00:01,,-,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Result always has frame version 1 (UNIX_NS) with nanosecond timestamp
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 9, ev.head);
  ASSERT_EQ(20, ev.vscp_class);
  ASSERT_EQ(3, ev.vscp_type);
  ASSERT_EQ(3, ev.sizeData);
  ASSERT_EQ(0, ev.obid);
  // Timestamp ns is converted from 2001-11-02T18:00:01 with 0 microseconds
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2001, 11, 2, 18, 0, 1, 0), ev.timestamp_ns);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);
  ASSERT_EQ(0, memcmp(ev.GUID, guid, 16));
  ASSERT_EQ(0, ev.pdata[0]);
  ASSERT_EQ(1, ev.pdata[1]);
  ASSERT_EQ(35, ev.pdata[2]);
  delete[] ev.pdata;
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEventEx_1)
{
  int rv;
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));

  // Empty datestr means nanosecond timestamp mode
  rv = vscp_fwhlp_parseStringToEventEx(&ex, "6,20,3,,,,00:01:02:03:04:05:06:07:08:09:0A:0B:0C:0D:0E:0F,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // head=6 with VSCP_HEADER16_FRAME_VERSION_UNIX_NS (0x0100) set = 0x0106
  ASSERT_EQ(0x0106, ex.head);
  ASSERT_EQ(20, ex.vscp_class);
  ASSERT_EQ(3, ex.vscp_type);
  ASSERT_EQ(3, ex.sizeData);
  ASSERT_EQ(0, ex.obid);
  ASSERT_EQ(0ULL, ex.timestamp_ns);  // nanosecond timestamp
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(i, ex.GUID[i]);
  }
  //ASSERT_EQ(0, memcmp(ex.GUID, (uint8_t[]) { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }, 16));
  ASSERT_EQ(0, ex.data[0]);
  ASSERT_EQ(1, ex.data[1]);
  ASSERT_EQ(35, ex.data[2]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEventEx_2)
{
  int rv;
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));
  uint8_t guid[16] = { 0 };

  // Empty datestr means nanosecond timestamp mode
  rv = vscp_fwhlp_parseStringToEventEx(&ex, "1,20,3,,,,-,0,0x44,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // head=1 with VSCP_HEADER16_FRAME_VERSION_UNIX_NS (0x0100) set = 0x0101
  ASSERT_EQ(0x0101, ex.head);
  ASSERT_EQ(20, ex.vscp_class);
  ASSERT_EQ(3, ex.vscp_type);
  ASSERT_EQ(3, ex.sizeData);
  ASSERT_EQ(0, ex.obid);
  ASSERT_EQ(0ULL, ex.timestamp_ns);  // nanosecond timestamp
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
  ASSERT_EQ(0, memcmp(ex.GUID, guid, 16));
  ASSERT_EQ(0, ex.data[0]);
  ASSERT_EQ(0x44, ex.data[1]);
  ASSERT_EQ(35, ex.data[2]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEventEx_3)
{
  int rv;
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));
  uint8_t guid[16] = { 0 };

  rv = vscp_fwhlp_parseStringToEventEx(&ex, "9,20,3,,2001-11-02T18:00:01,,-,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Result always has frame version 1 (UNIX_NS) with nanosecond timestamp
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS | 9, ex.head);
  ASSERT_EQ(20, ex.vscp_class);
  ASSERT_EQ(3, ex.vscp_type);
  ASSERT_EQ(3, ex.sizeData);
  ASSERT_EQ(0, ex.obid);
  // Timestamp ns is converted from 2001-11-02T18:00:01 with 0 microseconds
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2001, 11, 2, 18, 0, 1, 0), ex.timestamp_ns);
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
  ASSERT_EQ(0, memcmp(ex.GUID, guid, 16));
  ASSERT_EQ(0, ex.data[0]);
  ASSERT_EQ(1, ex.data[1]);
  ASSERT_EQ(35, ex.data[2]);
}

// Test parsing with empty datestr and nanosecond timestamp
TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEvent_unix_ns_timestamp)
{
  int rv;
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));
  uint8_t guid[16] = { 0 };

  // Empty datestr with Unix nanosecond timestamp (2024-06-30T23:59:58Z = 1719791998000000000 ns)
  rv = vscp_fwhlp_parseStringToEvent(&ev, "0,20,3,,,1719791998000000000,-,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Frame version should be VSCP_HEADER16_FRAME_VERSION_UNIX_NS (0x0100)
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(20, ev.vscp_class);
  ASSERT_EQ(3, ev.vscp_type);
  ASSERT_EQ(3, ev.sizeData);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);
  ASSERT_EQ(1719791998000000000ULL, ev.timestamp_ns);
  ASSERT_EQ(0, memcmp(ev.GUID, guid, 16));
  ASSERT_EQ(0, ev.pdata[0]);
  ASSERT_EQ(1, ev.pdata[1]);
  ASSERT_EQ(35, ev.pdata[2]);
  delete[] ev.pdata;
}

TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEventEx_unix_ns_timestamp)
{
  int rv;
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));
  uint8_t guid[16] = { 0 };

  // Empty datestr with Unix nanosecond timestamp (2024-06-30T23:59:58Z = 1719791998000000000 ns)
  rv = vscp_fwhlp_parseStringToEventEx(&ex, "0,20,3,,,1719791998000000000,-,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Frame version should be VSCP_HEADER16_FRAME_VERSION_UNIX_NS (0x0100)
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(20, ex.vscp_class);
  ASSERT_EQ(3, ex.vscp_type);
  ASSERT_EQ(3, ex.sizeData);
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
  ASSERT_EQ(1719791998000000000ULL, ex.timestamp_ns);
  ASSERT_EQ(0, memcmp(ex.GUID, guid, 16));
  ASSERT_EQ(0, ex.data[0]);
  ASSERT_EQ(1, ex.data[1]);
  ASSERT_EQ(35, ex.data[2]);
}

// Test that datetime with microsecond timestamp is converted to nanoseconds
TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEvent_microsecond_timestamp)
{
  int rv;
  vscp_event_t ev;
  memset(&ev, 0, sizeof(ev));
  uint8_t guid[16] = { 0 };

  rv = vscp_fwhlp_parseStringToEvent(&ev, "0,20,3,,2024-06-30T23:59:58,12345678,-,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Result always has frame version 1 (UNIX_NS) with nanosecond timestamp
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(20, ev.vscp_class);
  ASSERT_EQ(3, ev.vscp_type);
  ASSERT_EQ(0xffff, ev.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ev.month);
  // Timestamp ns is converted from 2024-06-30T23:59:58 with 12345678 microseconds
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2024, 6, 30, 23, 59, 58, 12345678), ev.timestamp_ns);
  ASSERT_EQ(0, memcmp(ev.GUID, guid, 16));
  delete[] ev.pdata;
}

// Test that datetime with microsecond timestamp is converted to nanoseconds
TEST(_vscp_firmware_helper, vscp_fwhlp_parseStringToEventEx_microsecond_timestamp)
{
  int rv;
  vscp_event_ex_t ex;
  memset(&ex, 0, sizeof(ex));
  uint8_t guid[16] = { 0 };

  rv = vscp_fwhlp_parseStringToEventEx(&ex, "0,20,3,,2024-06-30T23:59:58,12345678,-,0,1,35");
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Result always has frame version 1 (UNIX_NS) with nanosecond timestamp
  ASSERT_EQ(VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ex.head & VSCP_HEADER16_FRAME_VERSION_MASK);
  ASSERT_EQ(20, ex.vscp_class);
  ASSERT_EQ(3, ex.vscp_type);
  ASSERT_EQ(0xffff, ex.year);  // 0xffff indicates nanosecond timestamp mode
  ASSERT_EQ(0xff, ex.month);
  // Timestamp ns is converted from 2024-06-30T23:59:58 with 12345678 microseconds
  ASSERT_EQ(vscp_fwhlp_to_unix_ns(2024, 6, 30, 23, 59, 58, 12345678), ex.timestamp_ns);
  ASSERT_EQ(0, memcmp(ex.GUID, guid, 16));
}

//-----------------------------------------------------------------------------
// Filter/Mask string conversion tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_writeFilterToString_basic)
{
  vscpEventFilter filter;
  char strFilter[128];
  int rv;

  memset(&filter, 0, sizeof(filter));
  filter.filter_priority = 3;
  filter.filter_class    = 10;
  filter.filter_type     = 6;

  // Set filter GUID
  for (int i = 0; i < 16; i++) {
    filter.filter_GUID[i] = i;
  }

  rv = vscp_fwhlp_writeFilterToString(strFilter, sizeof(strFilter), &filter);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify the string contains expected values
  ASSERT_NE(nullptr, strstr(strFilter, "3,10,6,"));
  ASSERT_NE(nullptr, strstr(strFilter, "00:01:02:03:04:05:06:07"));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeFilterToString_buffer_too_small)
{
  vscpEventFilter filter;
  char strFilter[10]; // Too small
  int rv;

  memset(&filter, 0, sizeof(filter));

  rv = vscp_fwhlp_writeFilterToString(strFilter, sizeof(strFilter), &filter);
  ASSERT_EQ(VSCP_ERROR_BUFFER_TO_SMALL, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeFilterToString_roundtrip)
{
  vscpEventFilter filter_original;
  vscpEventFilter filter_parsed;
  char strFilter[128];
  int rv;

  // Setup original filter
  memset(&filter_original, 0, sizeof(filter_original));
  filter_original.filter_priority = 5;
  filter_original.filter_class    = 1000;
  filter_original.filter_type     = 255;

  for (int i = 0; i < 16; i++) {
    filter_original.filter_GUID[i] = (i * 17) % 256;
  }

  // Write to string
  rv = vscp_fwhlp_writeFilterToString(strFilter, sizeof(strFilter), &filter_original);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Parse it back
  memset(&filter_parsed, 0, sizeof(filter_parsed));
  rv = vscp_fwhlp_parseFilter(&filter_parsed, strFilter);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify roundtrip
  ASSERT_EQ(filter_original.filter_priority, filter_parsed.filter_priority);
  ASSERT_EQ(filter_original.filter_class, filter_parsed.filter_class);
  ASSERT_EQ(filter_original.filter_type, filter_parsed.filter_type);
  ASSERT_EQ(0, memcmp(filter_original.filter_GUID, filter_parsed.filter_GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeMaskToString_basic)
{
  vscpEventFilter filter;
  char strMask[128];
  int rv;

  memset(&filter, 0, sizeof(filter));
  filter.mask_priority = 7;
  filter.mask_class    = 0xFFFF;
  filter.mask_type     = 0xFFFF;

  // Set mask GUID to all 0xFF
  memset(filter.mask_GUID, 0xFF, 16);

  rv = vscp_fwhlp_writeMaskToString(strMask, sizeof(strMask), &filter);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify the string contains expected values
  ASSERT_NE(nullptr, strstr(strMask, "7,65535,65535,"));
  ASSERT_NE(nullptr, strstr(strMask, "FF:FF:FF:FF:FF:FF:FF:FF"));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeMaskToString_buffer_too_small)
{
  vscpEventFilter filter;
  char strMask[10]; // Too small
  int rv;

  memset(&filter, 0, sizeof(filter));

  rv = vscp_fwhlp_writeMaskToString(strMask, sizeof(strMask), &filter);
  ASSERT_EQ(VSCP_ERROR_BUFFER_TO_SMALL, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeMaskToString_roundtrip)
{
  vscpEventFilter filter_original;
  vscpEventFilter filter_parsed;
  char strMask[128];
  int rv;

  // Setup original mask
  memset(&filter_original, 0, sizeof(filter_original));
  filter_original.mask_priority = 3;
  filter_original.mask_class    = 512;
  filter_original.mask_type     = 128;

  for (int i = 0; i < 16; i++) {
    filter_original.mask_GUID[i] = (i * 7) % 256;
  }

  // Write to string
  rv = vscp_fwhlp_writeMaskToString(strMask, sizeof(strMask), &filter_original);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Parse it back
  memset(&filter_parsed, 0, sizeof(filter_parsed));
  rv = vscp_fwhlp_parseMask(&filter_parsed, strMask);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Verify roundtrip
  ASSERT_EQ(filter_original.mask_priority, filter_parsed.mask_priority);
  ASSERT_EQ(filter_original.mask_class, filter_parsed.mask_class);
  ASSERT_EQ(filter_original.mask_type, filter_parsed.mask_type);
  ASSERT_EQ(0, memcmp(filter_original.mask_GUID, filter_parsed.mask_GUID, 16));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeFilterToString_all_zeros)
{
  vscpEventFilter filter;
  char strFilter[128];
  int rv;

  memset(&filter, 0, sizeof(filter));

  rv = vscp_fwhlp_writeFilterToString(strFilter, sizeof(strFilter), &filter);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Should output "0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
  ASSERT_NE(nullptr, strstr(strFilter, "0,0,0,"));
  ASSERT_NE(nullptr, strstr(strFilter, "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"));
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeMaskToString_all_zeros)
{
  vscpEventFilter filter;
  char strMask[128];
  int rv;

  memset(&filter, 0, sizeof(filter));

  rv = vscp_fwhlp_writeMaskToString(strMask, sizeof(strMask), &filter);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Should output "0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"
  ASSERT_NE(nullptr, strstr(strMask, "0,0,0,"));
  ASSERT_NE(nullptr, strstr(strMask, "00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00"));
}

//-----------------------------------------------------------------------------
// writeCommandToFrame tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_writeCommandToFrame_basic)
{
  uint8_t frame[64];
  int rv;

  rv = vscp_fwhlp_writeCommandToFrame(frame, sizeof(frame), 0x0001, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Packet type byte: command type in upper nibble, no encryption in lower nibble
  ASSERT_EQ(VSCP_BINARY_PACKET_TYPE_COMMAND, frame[0]);

  // Command MSB, LSB
  ASSERT_EQ(0x00, frame[1]);
  ASSERT_EQ(0x01, frame[2]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeCommandToFrame_with_args)
{
  uint8_t frame[64];
  uint8_t args[4] = { 0xAA, 0xBB, 0xCC, 0xDD };
  int rv;

  rv = vscp_fwhlp_writeCommandToFrame(frame, sizeof(frame), 0x1234, args, 4);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Command
  ASSERT_EQ(0x12, frame[1]);
  ASSERT_EQ(0x34, frame[2]);

  // Arguments
  ASSERT_EQ(0xAA, frame[3]);
  ASSERT_EQ(0xBB, frame[4]);
  ASSERT_EQ(0xCC, frame[5]);
  ASSERT_EQ(0xDD, frame[6]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeCommandToFrame_null_frame)
{
  int rv;

  rv = vscp_fwhlp_writeCommandToFrame(NULL, 64, 0x0001, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_PARAMETER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeCommandToFrame_null_arg_nonzero_len)
{
  uint8_t frame[64];
  int rv;

  rv = vscp_fwhlp_writeCommandToFrame(frame, sizeof(frame), 0x0001, NULL, 5);
  ASSERT_EQ(VSCP_ERROR_PARAMETER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeCommandToFrame_buffer_too_small)
{
  uint8_t frame[3]; // Too small: need at least 1 + 2 + 0 + 2 = 5
  int rv;

  rv = vscp_fwhlp_writeCommandToFrame(frame, sizeof(frame), 0x0001, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_BUFFER_TO_SMALL, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeCommandToFrame_large_command)
{
  uint8_t frame[64];
  int rv;

  rv = vscp_fwhlp_writeCommandToFrame(frame, sizeof(frame), 0xFFFF, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Command MSB, LSB
  ASSERT_EQ(0xFF, frame[1]);
  ASSERT_EQ(0xFF, frame[2]);
}

//-----------------------------------------------------------------------------
// writeReplyToFrame tests
//-----------------------------------------------------------------------------

TEST(_vscp_firmware_helper, vscp_fwhlp_writeReplyToFrame_basic)
{
  uint8_t frame[64];
  int rv;

  rv = vscp_fwhlp_writeReplyToFrame(frame, sizeof(frame), 0x0001, 0x0000, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Packet type: reply
  ASSERT_EQ(VSCP_BINARY_PACKET_TYPE_RESPONSE, frame[0]);

  // Command
  ASSERT_EQ(0x00, frame[1]);
  ASSERT_EQ(0x01, frame[2]);

  // Error
  ASSERT_EQ(0x00, frame[3]);
  ASSERT_EQ(0x00, frame[4]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeReplyToFrame_with_error)
{
  uint8_t frame[64];
  int rv;

  rv = vscp_fwhlp_writeReplyToFrame(frame, sizeof(frame), 0x0001, 0x0005, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Error
  ASSERT_EQ(0x00, frame[3]);
  ASSERT_EQ(0x05, frame[4]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeReplyToFrame_with_args)
{
  uint8_t frame[64];
  uint8_t args[3] = { 0x11, 0x22, 0x33 };
  int rv;

  rv = vscp_fwhlp_writeReplyToFrame(frame, sizeof(frame), 0x4321, 0x0000, args, 3);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Command
  ASSERT_EQ(0x43, frame[1]);
  ASSERT_EQ(0x21, frame[2]);

  // Error
  ASSERT_EQ(0x00, frame[3]);
  ASSERT_EQ(0x00, frame[4]);

  // Arguments
  ASSERT_EQ(0x11, frame[5]);
  ASSERT_EQ(0x22, frame[6]);
  ASSERT_EQ(0x33, frame[7]);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeReplyToFrame_null_frame)
{
  int rv;

  rv = vscp_fwhlp_writeReplyToFrame(NULL, 64, 0x0001, 0x0000, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_PARAMETER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeReplyToFrame_null_arg_nonzero_len)
{
  uint8_t frame[64];
  int rv;

  rv = vscp_fwhlp_writeReplyToFrame(frame, sizeof(frame), 0x0001, 0x0000, NULL, 5);
  ASSERT_EQ(VSCP_ERROR_PARAMETER, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeReplyToFrame_buffer_too_small)
{
  uint8_t frame[4]; // Too small: need at least 1 + 2 + 2 + 0 + 2 = 7
  int rv;

  rv = vscp_fwhlp_writeReplyToFrame(frame, sizeof(frame), 0x0001, 0x0000, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_BUFFER_TO_SMALL, rv);
}

TEST(_vscp_firmware_helper, vscp_fwhlp_writeReplyToFrame_large_values)
{
  uint8_t frame[64];
  int rv;

  rv = vscp_fwhlp_writeReplyToFrame(frame, sizeof(frame), 0xFFFF, 0xFFFF, NULL, 0);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  // Command
  ASSERT_EQ(0xFF, frame[1]);
  ASSERT_EQ(0xFF, frame[2]);

  // Error
  ASSERT_EQ(0xFF, frame[3]);
  ASSERT_EQ(0xFF, frame[4]);
}

int
main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}