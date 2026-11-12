// vscp-authenticated-frame-unittest.cpp
//
// Tests for authenticated binary frames (packet version 1, AES-CBC +
// HMAC-SHA-256). The known answer record is the interoperability vector from
// the VSCP helper library documentation (docs/vscpremotetcpif-binary.md).
//

#include <gtest/gtest.h>

#include <stdint.h>
#include <string.h>

#include <string>
#include <vector>

extern "C" {
#include <vscp.h>
#include <vscp-crc.h>
#include "vscp-firmware-helper.h"
}

namespace {

std::vector<uint8_t>
fromHex(const std::string &hex)
{
  std::vector<uint8_t> out;
  for (size_t i = 0; i + 1 < hex.size(); i += 2) {
    out.push_back((uint8_t) std::stoul(hex.substr(i, 2), nullptr, 16));
  }
  return out;
}

// Public test keys - only for testing
const std::vector<uint8_t> aesKey  = fromHex("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f");
const std::vector<uint8_t> hmacKey = fromHex("202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f");
const std::vector<uint8_t> fixedIv = fromHex("000102030405060708090a0b0c0d0e0f");

const std::vector<uint8_t> knownPlain = fromHex("e000076de8");
const std::vector<uint8_t> knownRecord =
  fromHex("552031e3b865bcf520d83d2708502ae712638aeb000102030405060708090a0b0c0d0e0f270fec51e9794541f47aab3dd9f947ce");

class AuthenticatedFrame : public ::testing::Test {
protected:
  void SetUp() override { crcInit(); }
};

} // namespace

// Command frames carry the CRC over command + arguments (the plain frame of
// the vector is command 0x0007 without arguments)
TEST_F(AuthenticatedFrame, CommandFrameCrcMatchesVector)
{
  uint8_t frame[16];
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_fwhlp_writeCommandToFrame(frame, sizeof(frame), 0x0007, NULL, 0));
  ASSERT_EQ(0, memcmp(frame, knownPlain.data(), knownPlain.size()));
}

TEST_F(AuthenticatedFrame, KnownAnswerEncode)
{
  uint8_t wire[128];
  size_t len = vscp_fwhlp_encodeAuthenticatedFrame(wire,
                                                   sizeof(wire),
                                                   knownPlain.data(),
                                                   knownPlain.size(),
                                                   aesKey.data(),
                                                   hmacKey.data(),
                                                   fixedIv.data(),
                                                   VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(knownRecord.size(), len);
  ASSERT_EQ(0, memcmp(wire, knownRecord.data(), len));
}

TEST_F(AuthenticatedFrame, KnownAnswerDecode)
{
  uint8_t plain[64];
  size_t len = vscp_fwhlp_decodeAuthenticatedFrame(plain,
                                                   sizeof(plain),
                                                   knownRecord.data(),
                                                   knownRecord.size(),
                                                   aesKey.data(),
                                                   hmacKey.data(),
                                                   VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(knownPlain.size(), len);
  // The type byte keeps the authenticated encryption nibble
  ASSERT_EQ(0xe3, plain[0]);
  ASSERT_EQ(0, memcmp(plain + 1, knownPlain.data() + 1, knownPlain.size() - 1));
}

TEST_F(AuthenticatedFrame, ParseHeader)
{
  uint8_t version = 0xff;
  size_t inner    = 0;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_fwhlp_parseFrameHeader(knownRecord.data(), knownRecord.size(), &version, &inner));
  ASSERT_EQ(VSCP_FWHLP_FRAME_VERSION_AUTHENTICATED, version);
  ASSERT_EQ(49u, inner);

  uint8_t bad[3] = { 0x54, 0x20, 0x31 };
  ASSERT_EQ(VSCP_ERROR_INVALID_FRAME, vscp_fwhlp_parseFrameHeader(bad, sizeof(bad), &version, &inner));
  ASSERT_EQ(VSCP_ERROR_PARAMETER, vscp_fwhlp_parseFrameHeader(bad, 2, &version, &inner));
}

// Every single changed byte (prefix, type, ciphertext, IV, tag) is rejected
// and the output buffer is left unchanged
TEST_F(AuthenticatedFrame, TamperingIsRejected)
{
  for (size_t i = 0; i < knownRecord.size(); i++) {
    std::vector<uint8_t> rec = knownRecord;
    rec[i] ^= 0x01;
    uint8_t plain[64];
    memset(plain, 0xa5, sizeof(plain));
    size_t len = vscp_fwhlp_decodeAuthenticatedFrame(plain,
                                                     sizeof(plain),
                                                     rec.data(),
                                                     rec.size(),
                                                     aesKey.data(),
                                                     hmacKey.data(),
                                                     VSCP_ENCRYPTION_FROM_TYPE_BYTE);
    ASSERT_EQ(0u, len) << "byte " << i;
    for (size_t k = 0; k < sizeof(plain); k++) {
      ASSERT_EQ(0xa5, plain[k]);
    }
  }
}

TEST_F(AuthenticatedFrame, WrongKeysAndPolicy)
{
  uint8_t plain[64];
  std::vector<uint8_t> wrongHmac = hmacKey;
  wrongHmac[31] ^= 1;
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(plain, sizeof(plain), knownRecord.data(), knownRecord.size(),
                                                    aesKey.data(), wrongHmac.data(), VSCP_ENCRYPTION_AES256));
  std::vector<uint8_t> wrongAes = aesKey;
  wrongAes[0] ^= 1;
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(plain, sizeof(plain), knownRecord.data(), knownRecord.size(),
                                                    wrongAes.data(), hmacKey.data(), VSCP_ENCRYPTION_AES256));
  // Agreed algorithm differs from the record
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(plain, sizeof(plain), knownRecord.data(), knownRecord.size(),
                                                    aesKey.data(), hmacKey.data(), VSCP_ENCRYPTION_AES128));
  // Truncated and extended records
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(plain, sizeof(plain), knownRecord.data(), knownRecord.size() - 1,
                                                    aesKey.data(), hmacKey.data(), VSCP_ENCRYPTION_AES256));
  std::vector<uint8_t> longer = knownRecord;
  longer.push_back(0);
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(plain, sizeof(plain), longer.data(), longer.size(),
                                                    aesKey.data(), hmacKey.data(), VSCP_ENCRYPTION_AES256));
  // A version 0 prefix is never accepted (no downgrade)
  std::vector<uint8_t> v0 = knownRecord;
  v0[1] &= 0x1f;
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(plain, sizeof(plain), v0.data(), v0.size(), aesKey.data(),
                                                    hmacKey.data(), VSCP_ENCRYPTION_AES256));
  // Output too small
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(plain, 4, knownRecord.data(), knownRecord.size(), aesKey.data(),
                                                    hmacKey.data(), VSCP_ENCRYPTION_AES256));
}

TEST_F(AuthenticatedFrame, UnencryptedIsRefused)
{
  uint8_t wire[128];
  ASSERT_EQ(0u, vscp_fwhlp_encodeAuthenticatedFrame(wire, sizeof(wire), knownPlain.data(), knownPlain.size(),
                                                    aesKey.data(), hmacKey.data(), NULL, VSCP_ENCRYPTION_NONE));
  // Too short inner frame
  ASSERT_EQ(0u, vscp_fwhlp_encodeAuthenticatedFrame(wire, sizeof(wire), knownPlain.data(), 4, aesKey.data(),
                                                    hmacKey.data(), NULL, VSCP_ENCRYPTION_AES256));
  // Output too small
  ASSERT_EQ(0u, vscp_fwhlp_encodeAuthenticatedFrame(wire, 51, knownPlain.data(), knownPlain.size(), aesKey.data(),
                                                    hmacKey.data(), fixedIv.data(), VSCP_ENCRYPTION_AES256));
}

// Round trips for all AES sizes, many lengths, random IV, in place
TEST_F(AuthenticatedFrame, RoundTrip)
{
  for (uint8_t alg = VSCP_ENCRYPTION_AES128; alg <= VSCP_ENCRYPTION_AES256; alg++) {
    for (size_t bodyLen = 2; bodyLen < 120; bodyLen += 7) {
      // Event-like frame: type, body, CRC over the body
      std::vector<uint8_t> plain(1 + bodyLen + 2);
      plain[0] = 0x00;
      for (size_t i = 0; i < bodyLen; i++) {
        plain[1 + i] = (uint8_t) (i * 31 + alg);
      }
      crc c                = crcFast(plain.data() + 1, (int) bodyLen);
      plain[1 + bodyLen]   = (uint8_t) (c >> 8);
      plain[1 + bodyLen + 1] = (uint8_t) c;

      std::vector<uint8_t> wire(plain.size() + VSCP_FWHLP_FRAME_HEADER_SIZE + VSCP_FWHLP_FRAME_V1_OVERHEAD);
      size_t wireLen = vscp_fwhlp_encodeAuthenticatedFrame(wire.data(), wire.size(), plain.data(), plain.size(),
                                                           aesKey.data(), hmacKey.data(), NULL, alg);
      ASSERT_GT(wireLen, 0u);
      ASSERT_EQ(alg, wire[VSCP_FWHLP_FRAME_HEADER_SIZE] & 0x0f);

      std::vector<uint8_t> out(plain.size());
      size_t outLen = vscp_fwhlp_decodeAuthenticatedFrame(out.data(), out.size(), wire.data(), wireLen,
                                                          aesKey.data(), hmacKey.data(), alg);
      ASSERT_EQ(plain.size(), outLen);
      ASSERT_EQ(alg, out[0] & 0x0f);
      ASSERT_EQ(0, memcmp(out.data() + 1, plain.data() + 1, plain.size() - 1));

      // In place inner encrypt/decrypt
      std::vector<uint8_t> buf(plain.size() + VSCP_FWHLP_FRAME_V1_OVERHEAD);
      memcpy(buf.data(), plain.data(), plain.size());
      size_t innerLen = vscp_fwhlp_encryptFrameV1(buf.data(), buf.size(), buf.data(), plain.size(), aesKey.data(),
                                                  hmacKey.data(), NULL, alg);
      ASSERT_GT(innerLen, 0u);
      size_t back = vscp_fwhlp_decryptFrameV1(buf.data(), buf.size(), buf.data(), innerLen, aesKey.data(),
                                              hmacKey.data());
      ASSERT_EQ(plain.size(), back);
      ASSERT_EQ(0, memcmp(buf.data() + 1, plain.data() + 1, plain.size() - 1));
    }
  }
}

// Two encryptions of the same frame use different random IVs
TEST_F(AuthenticatedFrame, RandomIv)
{
  uint8_t a[128], b[128];
  size_t la = vscp_fwhlp_encodeAuthenticatedFrame(a, sizeof(a), knownPlain.data(), knownPlain.size(), aesKey.data(),
                                                  hmacKey.data(), NULL, VSCP_ENCRYPTION_AES256);
  size_t lb = vscp_fwhlp_encodeAuthenticatedFrame(b, sizeof(b), knownPlain.data(), knownPlain.size(), aesKey.data(),
                                                  hmacKey.data(), NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_EQ(la, lb);
  ASSERT_NE(0, memcmp(a + la - 32, b + lb - 32, 16)); // IVs
}

// A frame with a bad inner CRC is rejected even with a valid tag
TEST_F(AuthenticatedFrame, BadInnerCrcIsRejected)
{
  std::vector<uint8_t> plain = knownPlain;
  plain[4] ^= 0xff;
  uint8_t wire[128];
  size_t wireLen = vscp_fwhlp_encodeAuthenticatedFrame(wire, sizeof(wire), plain.data(), plain.size(), aesKey.data(),
                                                       hmacKey.data(), NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_GT(wireLen, 0u);
  uint8_t out[64];
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(out, sizeof(out), wire, wireLen, aesKey.data(), hmacKey.data(),
                                                    VSCP_ENCRYPTION_AES256));
}

// Invalid packet types are rejected like in the VSCP helper library
TEST_F(AuthenticatedFrame, InvalidPacketTypeIsRejected)
{
  std::vector<uint8_t> plain = knownPlain;
  plain[0]                   = 0x23; // Packet type 2 is not defined
  uint8_t wire[128];
  size_t wireLen = vscp_fwhlp_encodeAuthenticatedFrame(wire, sizeof(wire), plain.data(), plain.size(), aesKey.data(),
                                                       hmacKey.data(), NULL, VSCP_ENCRYPTION_AES256);
  ASSERT_GT(wireLen, 0u);
  uint8_t out[64];
  ASSERT_EQ(0u, vscp_fwhlp_decodeAuthenticatedFrame(out, sizeof(out), wire, wireLen, aesKey.data(), hmacKey.data(),
                                                    VSCP_ENCRYPTION_AES256));
}
