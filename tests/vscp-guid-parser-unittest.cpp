#include <stdio.h>
#include <string.h>
#include <gtest/gtest.h>
#include <vscp.h>
#include <vscp-guid-parser.h>

//-----------------------------------------------------------------------------
// vscp_guid_parse - Basic parsing tests
//-----------------------------------------------------------------------------

TEST(_vscp_guid_parser, parse_standard_colon_format)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0xEE, guid[1]);
  ASSERT_EQ(0xDD, guid[2]);
  ASSERT_EQ(0x00, guid[15]);
}

TEST(_vscp_guid_parser, parse_all_zeros_dash)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "-", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
}

TEST(_vscp_guid_parser, parse_all_ff_double_colon)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "::", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(0xFF, guid[i]);
  }
}

TEST(_vscp_guid_parser, parse_double_colon_prefix_ff)
{
  uint8_t guid[16];
  int rv;

  // :: followed by values should fill leading bytes with 0xFF
  rv = vscp_guid_parse(guid, "::01:02:03", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // First 13 bytes should be 0xFF
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0xFF, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_guid_parser, parse_uuid_format_dash)
{
  uint8_t guid[16];
  int rv;

  // UUID format: 8-4-4-4-12
  rv = vscp_guid_parse(guid, "FFFFFFFF-FFFF-FFFF-0102-03AABB440130", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0xFF, guid[1]);
  ASSERT_EQ(0xFF, guid[2]);
  ASSERT_EQ(0xFF, guid[3]);
  ASSERT_EQ(0xFF, guid[4]);
  ASSERT_EQ(0xFF, guid[5]);
  ASSERT_EQ(0xFF, guid[6]);
  ASSERT_EQ(0xFF, guid[7]);
  ASSERT_EQ(0x01, guid[8]);
  ASSERT_EQ(0x02, guid[9]);
  ASSERT_EQ(0x03, guid[10]);
  ASSERT_EQ(0xAA, guid[11]);
  ASSERT_EQ(0xBB, guid[12]);
  ASSERT_EQ(0x44, guid[13]);
  ASSERT_EQ(0x01, guid[14]);
  ASSERT_EQ(0x30, guid[15]);
}

//-----------------------------------------------------------------------------
// vscp_guid_parse - Leading zeros with -: prefix
//-----------------------------------------------------------------------------

TEST(_vscp_guid_parser, parse_hyphen_colon_prefix_zeros)
{
  uint8_t guid[16];
  int rv;

  // -: followed by values should fill leading bytes with 0x00
  rv = vscp_guid_parse(guid, "-:01:02:03", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // First 13 bytes should be 0x00
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_guid_parser, parse_hyphen_colon_with_comma_separator)
{
  uint8_t guid[16];
  int rv;

  // -: with comma separators
  rv = vscp_guid_parse(guid, "-:1,2,3", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // First 13 bytes should be 0x00
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_guid_parser, parse_hyphen_colon_eight_bytes)
{
  uint8_t guid[16];
  int rv;

  // -: with 8 trailing bytes
  rv = vscp_guid_parse(guid, "-:01:02:03:AA:BB:44:01:30", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  // First 8 bytes should be 0x00
  for (int i = 0; i < 8; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
  ASSERT_EQ(0x01, guid[8]);
  ASSERT_EQ(0x02, guid[9]);
  ASSERT_EQ(0x03, guid[10]);
  ASSERT_EQ(0xAA, guid[11]);
  ASSERT_EQ(0xBB, guid[12]);
  ASSERT_EQ(0x44, guid[13]);
  ASSERT_EQ(0x01, guid[14]);
  ASSERT_EQ(0x30, guid[15]);
}

//-----------------------------------------------------------------------------
// vscp_guid_parse - Brace-enclosed formats
//-----------------------------------------------------------------------------

TEST(_vscp_guid_parser, parse_braces_standard_format)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "{FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0xEE, guid[1]);
  ASSERT_EQ(0x00, guid[15]);
}

TEST(_vscp_guid_parser, parse_braces_uuid_format)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "{FFFFFFFF-FFFF-FFFF-0102-03AABB440130}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0xFF, guid[7]);
  ASSERT_EQ(0x01, guid[8]);
  ASSERT_EQ(0x30, guid[15]);
}

TEST(_vscp_guid_parser, parse_braces_double_colon)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "{::01:02:03}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0xFF, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_guid_parser, parse_braces_hyphen_colon)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "{-:01:02:03}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 13; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
  ASSERT_EQ(0x01, guid[13]);
  ASSERT_EQ(0x02, guid[14]);
  ASSERT_EQ(0x03, guid[15]);
}

TEST(_vscp_guid_parser, parse_braces_all_zeros)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "{-}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(0x00, guid[i]);
  }
}

TEST(_vscp_guid_parser, parse_braces_all_ff)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "{::}", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(0xFF, guid[i]);
  }
}

TEST(_vscp_guid_parser, parse_braces_with_whitespace)
{
  uint8_t guid[16];
  int rv;

  rv = vscp_guid_parse(guid, "{ FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00 }", NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(0xFF, guid[0]);
  ASSERT_EQ(0x00, guid[15]);
}

//-----------------------------------------------------------------------------
// vscp_guid_to_string tests
//-----------------------------------------------------------------------------

TEST(_vscp_guid_parser, to_string_standard)
{
  uint8_t guid[16] = { 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,
                       0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00 };
  char strguid[48];
  int rv;

  rv = vscp_guid_to_string(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("FF:EE:DD:CC:BB:AA:99:88:77:66:55:44:33:22:11:00", strguid);
}

TEST(_vscp_guid_parser, to_string_compact_leading_ff)
{
  uint8_t guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
  char strguid[48];
  int rv;

  rv = vscp_guid_to_string_compact(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("::01:02:03:04:05:06:07:08", strguid);
}

TEST(_vscp_guid_parser, to_string_compact_all_ff)
{
  uint8_t guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  char strguid[48];
  int rv;

  rv = vscp_guid_to_string_compact(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("::", strguid);
}

TEST(_vscp_guid_parser, to_string_uuid)
{
  uint8_t guid[16] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                       0x01, 0x02, 0x03, 0xAA, 0xBB, 0x44, 0x01, 0x30 };
  char strguid[48];
  int rv;

  rv = vscp_guid_to_string_uuid(strguid, guid);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_STREQ("FFFFFFFF-FFFF-FFFF-0102-03AABB440130", strguid);
}

//-----------------------------------------------------------------------------
// Roundtrip tests
//-----------------------------------------------------------------------------

TEST(_vscp_guid_parser, roundtrip_standard)
{
  uint8_t guid1[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                        0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
  uint8_t guid2[16];
  char strguid[48];
  int rv;

  rv = vscp_guid_to_string(strguid, guid1);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  rv = vscp_guid_parse(guid2, strguid, NULL);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);

  ASSERT_EQ(0, memcmp(guid1, guid2, 16));
}

//-----------------------------------------------------------------------------
// Error handling tests
//-----------------------------------------------------------------------------

TEST(_vscp_guid_parser, parse_null_guid_pointer)
{
  int rv = vscp_guid_parse(NULL, "FF:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00", NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_guid_parser, parse_null_string_pointer)
{
  uint8_t guid[16];
  int rv = vscp_guid_parse(guid, NULL, NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_guid_parser, to_string_null_guid)
{
  char strguid[48];
  int rv = vscp_guid_to_string(strguid, NULL);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}

TEST(_vscp_guid_parser, to_string_null_string)
{
  uint8_t guid[16] = { 0 };
  int rv = vscp_guid_to_string(NULL, guid);
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, rv);
}
