// FILE: vscp-ws1-unittest.cpp
//
// Unit tests for vscp-ws1.c using the ops-table pattern.
//
// Coverage:
//   - NULL-pointer guards (vscp_ws1_check_ctx via init/clearup/handle_text)
//   - vscp_ws1_init: success path, missing mandatory ops, ops->init called
//   - vscp_ws1_clearup: NULL ops->cleanup ignored
//   - vscp_ws1_handle_text_protocol_request: NULL frame, zero len, oversize
//   - Text command dispatch: NOOP, VERSION, CHALLENGE, OPEN, CLOSE
//   - SETFILTER: missing arg, bad format, success (stores filter in ctx)
//   - CLRQUEUE: optional callback NULL path and callback path
//   - COPYRIGHT: optional callback NULL path and callback path
//   - Unknown command: negative reply

#include <cstring>
#include <string>
#include <gtest/gtest.h>

// Must come before the module header — ws1.c uses these macros.
#define VSCP_FWHLP_BINARY_FRAME_SUPPORT
#define VSCP_FWHLP_CRYPTO_SUPPORT

#include "vscp.h"
#include "vscp-firmware-helper.h"
#include "vscp-ws1-protocol.h"

// ===========================================================================
// Stub state
// ===========================================================================

struct Ws1Stub {
  std::string last_reply;
  bool        init_called    = false;
  bool        cleanup_called = false;
  bool        open_called    = false;
  bool        close_called   = false;
  bool        clrq_called    = false;
  bool        copy_called    = false;
  bool        sf_called      = false;
  int         open_rv        = VSCP_ERROR_SUCCESS;
  int         close_rv       = VSCP_ERROR_SUCCESS;
};

static Ws1Stub g_stub;

static int stub_reply(vscp_ws1_ctx_t *pctx, const char *response)
{
  (void) pctx;
  g_stub.last_reply = response ? response : "";
  return VSCP_ERROR_SUCCESS;
}

static int stub_generate_sid(vscp_ws1_ctx_t *pctx, uint8_t *sid, size_t size)
{
  (void) pctx;
  for (size_t i = 0; i < size; i++) {
    sid[i] = (uint8_t)(i + 1);
  }
  return VSCP_ERROR_SUCCESS;
}

static int stub_init_cb(vscp_ws1_ctx_t *pctx)
{
  (void) pctx;
  g_stub.init_called = true;
  return VSCP_ERROR_SUCCESS;
}

static int stub_cleanup_cb(vscp_ws1_ctx_t *pctx)
{
  (void) pctx;
  g_stub.cleanup_called = true;
  return VSCP_ERROR_SUCCESS;
}

static int stub_open(vscp_ws1_ctx_t *pctx)
{
  (void) pctx;
  g_stub.open_called = true;
  return g_stub.open_rv;
}

static int stub_close(vscp_ws1_ctx_t *pctx)
{
  (void) pctx;
  g_stub.close_called = true;
  return g_stub.close_rv;
}

static int stub_clrq(vscp_ws1_ctx_t *pctx)
{
  (void) pctx;
  g_stub.clrq_called = true;
  return VSCP_ERROR_SUCCESS;
}

static int stub_copyright(vscp_ws1_ctx_t *pctx)
{
  (void) pctx;
  g_stub.copy_called = true;
  pctx->ops->reply(pctx, "+;COPYRIGHT;Test (c) 2026");
  return VSCP_ERROR_SUCCESS;
}

static int stub_setfilter(vscp_ws1_ctx_t *pctx, const vscpEventFilter *pfilter)
{
  (void) pctx;
  (void) pfilter;
  g_stub.sf_called = true;
  return VSCP_ERROR_SUCCESS;
}

// Minimal ops table (just mandatory fields)
static const vscp_ws1_ops_t k_min_ops = {
  stub_reply,
  stub_generate_sid,
  nullptr, // init
  nullptr, // cleanup
  nullptr, // get_primary_key
  nullptr, // validate_user
  nullptr, // is_allowed_event
  nullptr, // is_allowed_connection
  nullptr, // send_event
  nullptr, // copyright
  nullptr, // open
  nullptr, // close
  nullptr, // setfilter
  nullptr, // clrqueue
};

// Full ops table
static const vscp_ws1_ops_t k_full_ops = {
  stub_reply,
  stub_generate_sid,
  stub_init_cb,
  stub_cleanup_cb,
  nullptr, // get_primary_key
  nullptr, // validate_user
  nullptr, // is_allowed_event
  nullptr, // is_allowed_connection
  nullptr, // send_event
  stub_copyright,
  stub_open,
  stub_close,
  stub_setfilter,
  stub_clrq,
};

// ===========================================================================
// Helper: build a text frame "C;<command>[;<arg>]" from a C-string
// ===========================================================================

static std::string makeFrame(const char *command, const char *arg = nullptr)
{
  std::string f = "C;";
  f += command;
  if (arg) {
    f += ";";
    f += arg;
  }
  return f;
}

// ===========================================================================
// Test fixture
// ===========================================================================

class VscpWs1 : public ::testing::Test {
protected:
  vscp_ws1_ctx_t ctx{};

  void SetUp() override
  {
    g_stub = Ws1Stub{};
  }

  // Initialize ctx with the given ops table
  void init(const vscp_ws1_ops_t *ops, int expect_rv = VSCP_ERROR_SUCCESS)
  {
    memset(&ctx, 0, sizeof(ctx));
    ctx.ops = ops;
    ASSERT_EQ(vscp_ws1_init(&ctx, nullptr), expect_rv);
  }

  // Mark the connection as logged in (AUTH is tested separately)
  void login() { ctx.bAuthenticated = true; }

  // Send a text frame and return the last reply
  std::string cmd(const char *command, const char *arg = nullptr)
  {
    g_stub.last_reply.clear();
    std::string f = makeFrame(command, arg);
    vscp_ws1_handle_text_protocol_request(&ctx, f.c_str(), (uint16_t) f.size());
    return g_stub.last_reply;
  }
};

// ===========================================================================
// NULL-guard tests
// ===========================================================================

TEST(VscpWs1NullGuards, null_ctx_init_returns_error)
{
  EXPECT_NE(vscp_ws1_init(nullptr, nullptr), VSCP_ERROR_SUCCESS);
}

TEST(VscpWs1NullGuards, null_ops_init_returns_error)
{
  vscp_ws1_ctx_t ctx{};
  ctx.ops = nullptr;
  EXPECT_EQ(vscp_ws1_init(&ctx, nullptr), VSCP_ERROR_INIT_MISSING);
}

TEST(VscpWs1NullGuards, null_reply_op_returns_error)
{
  static const vscp_ws1_ops_t no_reply_ops = {
    nullptr,           // reply — missing mandatory op
    stub_generate_sid,
  };
  vscp_ws1_ctx_t ctx{};
  ctx.ops = &no_reply_ops;
  EXPECT_EQ(vscp_ws1_init(&ctx, nullptr), VSCP_ERROR_INIT_MISSING);
}

TEST(VscpWs1NullGuards, null_generate_sid_returns_error)
{
  static const vscp_ws1_ops_t no_sid_ops = {
    stub_reply,
    nullptr, // generate_sid — missing mandatory op
  };
  vscp_ws1_ctx_t ctx{};
  ctx.ops = &no_sid_ops;
  EXPECT_EQ(vscp_ws1_init(&ctx, nullptr), VSCP_ERROR_INIT_MISSING);
}

TEST(VscpWs1NullGuards, null_ctx_handle_text_returns_error)
{
  const char *frame = "C;NOOP";
  EXPECT_NE(vscp_ws1_handle_text_protocol_request(nullptr, frame, (uint16_t) strlen(frame)),
            VSCP_ERROR_SUCCESS);
}

TEST(VscpWs1NullGuards, null_ctx_clearup_returns_error)
{
  EXPECT_NE(vscp_ws1_clearup(nullptr, nullptr), VSCP_ERROR_SUCCESS);
}

// ===========================================================================
// Init / clearup
// ===========================================================================

TEST_F(VscpWs1, init_sends_auth0_challenge)
{
  init(&k_min_ops);
  // Reply must start with "+;AUTH0;"
  EXPECT_EQ(g_stub.last_reply.substr(0, 8), "+;AUTH0;");
  // Must contain 32 hex chars (16 bytes)
  ASSERT_GE(g_stub.last_reply.size(), 8u + 32u);
}

TEST_F(VscpWs1, init_calls_ops_init_when_set)
{
  init(&k_full_ops);
  EXPECT_TRUE(g_stub.init_called);
}

TEST_F(VscpWs1, init_does_not_require_ops_init)
{
  init(&k_min_ops); // ops.init == nullptr — must not crash
}

TEST_F(VscpWs1, init_saves_pdata)
{
  int sentinel = 42;
  memset(&ctx, 0, sizeof(ctx));
  ctx.ops = &k_min_ops;
  ASSERT_EQ(vscp_ws1_init(&ctx, &sentinel), VSCP_ERROR_SUCCESS);
  EXPECT_EQ(ctx.pdata, &sentinel);
}

TEST_F(VscpWs1, clearup_calls_cleanup_when_set)
{
  init(&k_full_ops);
  ASSERT_EQ(vscp_ws1_clearup(&ctx, nullptr), VSCP_ERROR_SUCCESS);
  EXPECT_TRUE(g_stub.cleanup_called);
}

TEST_F(VscpWs1, clearup_succeeds_when_cleanup_null)
{
  init(&k_min_ops);
  EXPECT_EQ(vscp_ws1_clearup(&ctx, nullptr), VSCP_ERROR_SUCCESS);
}

// ===========================================================================
// handle_text_protocol_request — bad frames
// ===========================================================================

TEST_F(VscpWs1, handle_text_null_frame_returns_error)
{
  init(&k_min_ops);
  EXPECT_NE(vscp_ws1_handle_text_protocol_request(&ctx, nullptr, 5), VSCP_ERROR_SUCCESS);
}

TEST_F(VscpWs1, handle_text_zero_len_returns_error)
{
  init(&k_min_ops);
  EXPECT_NE(vscp_ws1_handle_text_protocol_request(&ctx, "C;NOOP", 0), VSCP_ERROR_SUCCESS);
}

TEST_F(VscpWs1, handle_text_oversize_frame_returns_error)
{
  init(&k_min_ops);
  std::string big(VSCP_WS1_MAX_PACKET_SIZE + 1, 'X');
  EXPECT_NE(vscp_ws1_handle_text_protocol_request(&ctx, big.c_str(), (uint16_t) big.size()),
            VSCP_ERROR_SUCCESS);
}

TEST_F(VscpWs1, handle_text_unknown_frame_type_returns_error)
{
  init(&k_min_ops);
  const char *frame = "X;NOOP"; // 'X' is not C, E, +, or -
  EXPECT_NE(vscp_ws1_handle_text_protocol_request(&ctx, frame, (uint16_t) strlen(frame)),
            VSCP_ERROR_SUCCESS);
}

// ===========================================================================
// Command dispatch
// ===========================================================================

TEST_F(VscpWs1, noop_returns_positive_reply)
{
  init(&k_min_ops);
  EXPECT_EQ(cmd("NOOP"), "+;NOOP");
}

TEST_F(VscpWs1, noop_case_insensitive)
{
  init(&k_min_ops);
  EXPECT_EQ(cmd("noop"), "+;NOOP");
}

TEST_F(VscpWs1, version_returns_version_string)
{
  init(&k_min_ops);
  std::string r = cmd("VERSION");
  EXPECT_EQ(r.substr(0, 9), "+;VERSION");
}

TEST_F(VscpWs1, challenge_resends_auth0_sid)
{
  init(&k_min_ops);
  g_stub.last_reply.clear();
  std::string r = cmd("CHALLENGE");
  EXPECT_EQ(r.substr(0, 8), "+;AUTH0;");
}

TEST_F(VscpWs1, open_sets_bopen_true)
{
  init(&k_min_ops); // ops->open is NULL → defaults to success
  login();
  EXPECT_FALSE(ctx.bOpen);
  std::string r = cmd("OPEN");
  EXPECT_EQ(r, "+;OPEN");
  EXPECT_TRUE(ctx.bOpen);
}

TEST_F(VscpWs1, open_calls_optional_callback)
{
  init(&k_full_ops);
  login();
  cmd("OPEN");
  EXPECT_TRUE(g_stub.open_called);
  EXPECT_TRUE(ctx.bOpen);
}

TEST_F(VscpWs1, open_failure_sends_negative_reply)
{
  init(&k_full_ops);
  login();
  g_stub.open_rv = VSCP_ERROR_ERROR;
  std::string r  = cmd("OPEN");
  EXPECT_EQ(r.substr(0, 7), "-;OPEN;");
  EXPECT_FALSE(ctx.bOpen);
}

TEST_F(VscpWs1, close_sets_bopen_false)
{
  init(&k_min_ops);
  ctx.bOpen = true;
  std::string r = cmd("CLOSE");
  EXPECT_EQ(r, "+;CLOSE");
  EXPECT_FALSE(ctx.bOpen);
}

TEST_F(VscpWs1, close_calls_optional_callback)
{
  init(&k_full_ops);
  ctx.bOpen = true;
  cmd("CLOSE");
  EXPECT_TRUE(g_stub.close_called);
}

TEST_F(VscpWs1, copyright_null_ops_sends_not_supported)
{
  init(&k_min_ops); // ops->copyright == nullptr
  std::string r = cmd("COPYRIGHT");
  EXPECT_NE(r.find("-"), std::string::npos);
}

TEST_F(VscpWs1, copyright_calls_optional_callback)
{
  init(&k_full_ops);
  cmd("COPYRIGHT");
  EXPECT_TRUE(g_stub.copy_called);
  EXPECT_NE(g_stub.last_reply.find("COPYRIGHT"), std::string::npos);
}

TEST_F(VscpWs1, setfilter_missing_arg_returns_negative_reply)
{
  init(&k_min_ops);
  login();
  std::string r = cmd("SETFILTER", nullptr);
  EXPECT_EQ(r.substr(0, 10), "-;SETFILTE");
}

TEST_F(VscpWs1, setfilter_alias_sf_works)
{
  init(&k_min_ops);
  login();
  std::string r = cmd("SF", nullptr);
  EXPECT_EQ(r.substr(0, 2), "-;"); // missing arg → error
}

TEST_F(VscpWs1, setfilter_valid_stores_filter_in_ctx)
{
  init(&k_min_ops); // ops->setfilter == nullptr → store in ctx only
  login();
  // filter string: "priority_filter,priority_mask,class_filter,class_mask,type_filter,type_mask,guid_filter"
  std::string r = cmd("SETFILTER", "0,0,0,0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
  EXPECT_EQ(r, "+;SF");
}

TEST_F(VscpWs1, setfilter_calls_optional_callback)
{
  init(&k_full_ops);
  login();
  cmd("SETFILTER", "0,0,0,0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
  EXPECT_TRUE(g_stub.sf_called);
}

TEST_F(VscpWs1, clrqueue_null_ops_returns_positive)
{
  init(&k_min_ops);
  login();
  std::string r = cmd("CLRQUEUE");
  EXPECT_EQ(r, "+;CLRQ");
}

TEST_F(VscpWs1, clrqueue_alias_clrq_works)
{
  init(&k_min_ops);
  login();
  std::string r = cmd("CLRQ");
  EXPECT_EQ(r, "+;CLRQ");
}

TEST_F(VscpWs1, clrqueue_calls_optional_callback)
{
  init(&k_full_ops);
  login();
  cmd("CLRQUEUE");
  EXPECT_TRUE(g_stub.clrq_called);
}

TEST_F(VscpWs1, unknown_command_returns_negative_reply)
{
  init(&k_min_ops);
  std::string r = cmd("BOGUSCMD");
  EXPECT_EQ(r.substr(0, 2), "-;");
}

// ===========================================================================
// Authentication requirements
// ===========================================================================

TEST_F(VscpWs1, open_refused_before_login)
{
  init(&k_full_ops);
  std::string r = cmd("OPEN");
  EXPECT_EQ(r, "-;OPEN;5,Not authorised.");
  EXPECT_FALSE(ctx.bOpen);
  EXPECT_FALSE(g_stub.open_called);
}

TEST_F(VscpWs1, setfilter_refused_before_login)
{
  init(&k_full_ops);
  std::string r = cmd("SETFILTER", "0,0,0,0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
  EXPECT_EQ(r.substr(0, 14), "-;SETFILTER;5,");
  EXPECT_FALSE(g_stub.sf_called);
  r = cmd("SF", "0,0,0,0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
  EXPECT_EQ(r.substr(0, 7), "-;SF;5,");
}

TEST_F(VscpWs1, clrqueue_refused_before_login)
{
  init(&k_full_ops);
  EXPECT_EQ(cmd("CLRQUEUE").substr(0, 13), "-;CLRQUEUE;5,");
  EXPECT_EQ(cmd("CLRQ").substr(0, 9), "-;CLRQ;5,");
  EXPECT_FALSE(g_stub.clrq_called);
}

TEST_F(VscpWs1, close_allowed_before_login)
{
  init(&k_min_ops);
  EXPECT_EQ(cmd("CLOSE"), "+;CLOSE");
}

// validate_user stub that accepts anything, so only the framework checks run
static int stub_validate_ok(vscp_ws1_ctx_t *pctx, const uint8_t *pcrypto, uint8_t crypto_len, const uint8_t *psid)
{
  (void) pcrypto;
  (void) crypto_len;
  (void) psid;
  strcpy(pctx->user.username, "user");
  return VSCP_ERROR_SUCCESS;
}

static int stub_validate_fail(vscp_ws1_ctx_t *pctx, const uint8_t *pcrypto, uint8_t crypto_len, const uint8_t *psid)
{
  (void) pcrypto;
  (void) crypto_len;
  (void) psid;
  strcpy(pctx->user.username, "partial"); // Must be cleared by the framework
  return VSCP_ERROR_INVALID_PERMISSION;
}

static vscp_ws1_ops_t make_auth_ops(int (*validate)(vscp_ws1_ctx_t *, const uint8_t *, uint8_t, const uint8_t *))
{
  vscp_ws1_ops_t ops = k_min_ops;
  ops.validate_user  = validate;
  return ops;
}

// stub_generate_sid fills 01 02 03 ... 10
static const char *k_sid_hex   = "0102030405060708090A0B0C0D0E0F10";
static const char *k_crypto_16 = "00112233445566778899AABBCCDDEEFF";

TEST_F(VscpWs1, auth_with_session_sid_succeeds)
{
  static vscp_ws1_ops_t ops = make_auth_ops(stub_validate_ok);
  init(&ops);
  std::string arg = std::string(k_sid_hex) + ";" + k_crypto_16;
  std::string r   = cmd("AUTH", arg.c_str());
  EXPECT_EQ(r.substr(0, 12), "+;AUTH;user;");
  EXPECT_TRUE(ctx.bAuthenticated);
}

TEST_F(VscpWs1, auth_with_other_sid_refused)
{
  // A login captured on another connection (different sid) must not work
  static vscp_ws1_ops_t ops = make_auth_ops(stub_validate_ok);
  init(&ops);
  std::string arg = std::string("FFEEDDCCBBAA99887766554433221100;") + k_crypto_16;
  std::string r   = cmd("AUTH", arg.c_str());
  EXPECT_EQ(r, "-;AUTH;8,Not authorized");
  EXPECT_FALSE(ctx.bAuthenticated);
}

TEST_F(VscpWs1, failed_reauth_logs_out)
{
  static vscp_ws1_ops_t ops = make_auth_ops(stub_validate_fail);
  init(&ops);
  ctx.bAuthenticated = true;
  ctx.bOpen          = true;
  std::string arg    = std::string(k_sid_hex) + ";" + k_crypto_16;
  std::string r      = cmd("AUTH", arg.c_str());
  EXPECT_EQ(r, "-;AUTH;8,Not authorized");
  EXPECT_FALSE(ctx.bAuthenticated);
  EXPECT_FALSE(ctx.bOpen);
  EXPECT_EQ(ctx.user.username[0], '\0');
}

TEST_F(VscpWs1, auth_long_crypto_does_not_overflow)
{
  // 128 bytes of crypto (the maximum hex length) used to write one byte past
  // the crypto buffer. Now it is just refused as not 16 bytes.
  static vscp_ws1_ops_t ops = make_auth_ops(stub_validate_ok);
  init(&ops);
  std::string arg = std::string(k_sid_hex) + ";" + std::string(256, 'A');
  std::string r   = cmd("AUTH", arg.c_str());
  EXPECT_EQ(r, "-;AUTH;8,Not authorized");
  EXPECT_FALSE(ctx.bAuthenticated);
}

TEST_F(VscpWs1, auth_reply_with_full_user_record_is_bounded)
{
  // Fill every user field to its maximum. The reply used to be built with
  // strcat into a 512 byte buffer and overflowed it.
  static vscp_ws1_ops_t ops = make_auth_ops(
    [](vscp_ws1_ctx_t *pctx, const uint8_t *, uint8_t, const uint8_t *) -> int {
      memset(pctx->user.username, 'u', sizeof(pctx->user.username) - 1);
      memset(&pctx->user.allowed_remotes, 0xAB, sizeof(pctx->user.allowed_remotes));
      for (auto &ev : pctx->user.events) {
        ev[0] = 0x12;
        ev[1] = 0x34;
      }
#ifndef VSCP_WS1_DISABLE_USER_FULL_NAME
      memset(pctx->user.fullname, 'f', sizeof(pctx->user.fullname) - 1);
#endif
#ifndef VSCP_WS1_DISABLE_USER_NOTES
      memset(pctx->user.note, 'n', sizeof(pctx->user.note) - 1);
#endif
      return VSCP_ERROR_SUCCESS;
    });
  init(&ops);
  std::string arg = std::string(k_sid_hex) + ";" + k_crypto_16;
  std::string r   = cmd("AUTH", arg.c_str());
  EXPECT_EQ(r.substr(0, 7), "+;AUTH;");
  EXPECT_LT(r.size(), (size_t) 512);
}

TEST_F(VscpWs1, unknown_long_command_reply_is_bounded)
{
  init(&k_min_ops);
  std::string longcmd(500, 'X');
  std::string r = cmd(longcmd.c_str());
  EXPECT_EQ(r.substr(0, 2), "-;");
  EXPECT_LT(r.size(), (size_t) 512);
}

TEST_F(VscpWs1, short_encrypted_binary_frame_refused)
{
  // Encrypted frames shorter than type byte + one block + IV used to make
  // len - 16 underflow and decrypt far past the frame buffer
  init(&k_min_ops);
  uint8_t frame[16] = { 0x01 }; // Frame type 0, AES-128
  for (size_t len = 1; len <= sizeof(frame); len++) {
    EXPECT_EQ(VSCP_ERROR_INVALID_FRAME, vscp_ws1_handle_binary_protocol_request(&ctx, frame, len)) << len;
  }
  uint8_t odd[1 + 20 + 16] = { 0x01 }; // Not whole AES blocks
  EXPECT_EQ(VSCP_ERROR_INVALID_FRAME, vscp_ws1_handle_binary_protocol_request(&ctx, odd, sizeof(odd)));
}

TEST_F(VscpWs1, events_only_for_logged_in_open_clients)
{
  init(&k_min_ops);
  vscp_event_t ev{};
  ev.vscp_class = 10;
  ev.vscp_type  = 6;

  EXPECT_FALSE(vscp_ws1_is_event_for_client(&ctx, &ev));
  ctx.bOpen = true;
  EXPECT_FALSE(vscp_ws1_is_event_for_client(&ctx, &ev)); // Open but not logged in
  login();
  EXPECT_TRUE(vscp_ws1_is_event_for_client(&ctx, &ev));

  // Filter on class 20 only
  ctx.filter.mask_class   = 0xffff;
  ctx.filter.filter_class = 20;
  EXPECT_FALSE(vscp_ws1_is_event_for_client(&ctx, &ev));
  ev.vscp_class = 20;
  EXPECT_TRUE(vscp_ws1_is_event_for_client(&ctx, &ev));

  ctx.bOpen = false;
  EXPECT_FALSE(vscp_ws1_is_event_for_client(&ctx, &ev));
}
