// FILE: vscp-ws2-unittest.cpp
//
// Unit tests for vscp-ws2.c using the ops-table pattern.
//
// Coverage:
//   - NULL-pointer guards
//   - vscp_ws2_init: success path, missing mandatory ops, ops->init called
//   - vscp_ws2_clearup: optional cleanup callback
//   - vscp_ws2_handle_request: NULL input guards, stub NOT_IMPLEMENTED path

#include <cstring>
#include <string>
#include <gtest/gtest.h>

#include "vscp.h"
#include "vscp-ws2-protocol.h"

// ===========================================================================
// Stub state
// ===========================================================================

struct Ws2Stub {
  std::string last_reply;
  bool        init_called    = false;
  bool        cleanup_called = false;
};

static Ws2Stub g_stub2;

static int stub2_reply(vscp_ws2_ctx_t *pctx, const char *response)
{
  (void) pctx;
  g_stub2.last_reply = response ? response : "";
  return VSCP_ERROR_SUCCESS;
}

static int stub2_generate_sid(vscp_ws2_ctx_t *pctx, uint8_t *sid, size_t size)
{
  (void) pctx;
  for (size_t i = 0; i < size; i++) {
    sid[i] = (uint8_t)(0xAB);
  }
  return VSCP_ERROR_SUCCESS;
}

static int stub2_init_cb(vscp_ws2_ctx_t *pctx)
{
  (void) pctx;
  g_stub2.init_called = true;
  return VSCP_ERROR_SUCCESS;
}

static int stub2_cleanup_cb(vscp_ws2_ctx_t *pctx)
{
  (void) pctx;
  g_stub2.cleanup_called = true;
  return VSCP_ERROR_SUCCESS;
}

static const vscp_ws2_ops_t k_min_ops2 = {
  stub2_reply,
  stub2_generate_sid,
  nullptr, // init
  nullptr, // cleanup
  nullptr, // get_primary_key
  nullptr, // validate_user
  nullptr, // is_allowed_event
  nullptr, // is_allowed_connection
  nullptr, // send_event
  nullptr, // open
  nullptr, // close
  nullptr, // setfilter
  nullptr, // clrqueue
};

static const vscp_ws2_ops_t k_full_ops2 = {
  stub2_reply,
  stub2_generate_sid,
  stub2_init_cb,
  stub2_cleanup_cb,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
  nullptr,
};

// ===========================================================================
// NULL-guard tests
// ===========================================================================

TEST(VscpWs2NullGuards, null_ctx_init_returns_error)
{
  EXPECT_NE(vscp_ws2_init(nullptr, nullptr), VSCP_ERROR_SUCCESS);
}

TEST(VscpWs2NullGuards, null_ops_init_returns_error)
{
  vscp_ws2_ctx_t ctx{};
  ctx.ops = nullptr;
  EXPECT_EQ(vscp_ws2_init(&ctx, nullptr), VSCP_ERROR_INIT_MISSING);
}

TEST(VscpWs2NullGuards, null_reply_op_returns_error)
{
  static const vscp_ws2_ops_t no_reply = {
    nullptr,
    stub2_generate_sid,
  };
  vscp_ws2_ctx_t ctx{};
  ctx.ops = &no_reply;
  EXPECT_EQ(vscp_ws2_init(&ctx, nullptr), VSCP_ERROR_INIT_MISSING);
}

TEST(VscpWs2NullGuards, null_generate_sid_returns_error)
{
  static const vscp_ws2_ops_t no_sid = {
    stub2_reply,
    nullptr,
  };
  vscp_ws2_ctx_t ctx{};
  ctx.ops = &no_sid;
  EXPECT_EQ(vscp_ws2_init(&ctx, nullptr), VSCP_ERROR_INIT_MISSING);
}

TEST(VscpWs2NullGuards, null_ctx_handle_request_returns_error)
{
  EXPECT_NE(vscp_ws2_handle_request(nullptr, "{}", 2), VSCP_ERROR_SUCCESS);
}

TEST(VscpWs2NullGuards, null_ctx_clearup_returns_error)
{
  EXPECT_NE(vscp_ws2_clearup(nullptr, nullptr), VSCP_ERROR_SUCCESS);
}

// ===========================================================================
// Test fixture
// ===========================================================================

class VscpWs2 : public ::testing::Test {
protected:
  vscp_ws2_ctx_t ctx{};

  void SetUp() override
  {
    g_stub2 = Ws2Stub{};
  }

  void init(const vscp_ws2_ops_t *ops, int expect_rv = VSCP_ERROR_SUCCESS)
  {
    memset(&ctx, 0, sizeof(ctx));
    ctx.ops = ops;
    ASSERT_EQ(vscp_ws2_init(&ctx, nullptr), expect_rv);
  }
};

// ===========================================================================
// Init / clearup
// ===========================================================================

TEST_F(VscpWs2, init_sends_auth0_json_challenge)
{
  init(&k_min_ops2);
  // Reply should be a JSON object with type AUTH0
  EXPECT_NE(g_stub2.last_reply.find("AUTH0"), std::string::npos);
  EXPECT_NE(g_stub2.last_reply.find("sid"), std::string::npos);
}

TEST_F(VscpWs2, init_calls_ops_init_when_set)
{
  init(&k_full_ops2);
  EXPECT_TRUE(g_stub2.init_called);
}

TEST_F(VscpWs2, init_does_not_require_ops_init)
{
  init(&k_min_ops2); // ops.init == nullptr — must not crash
}

TEST_F(VscpWs2, init_saves_pdata)
{
  int sentinel = 99;
  memset(&ctx, 0, sizeof(ctx));
  ctx.ops = &k_min_ops2;
  ASSERT_EQ(vscp_ws2_init(&ctx, &sentinel), VSCP_ERROR_SUCCESS);
  EXPECT_EQ(ctx.pdata, &sentinel);
}

TEST_F(VscpWs2, clearup_calls_cleanup_when_set)
{
  init(&k_full_ops2);
  ASSERT_EQ(vscp_ws2_clearup(&ctx, nullptr), VSCP_ERROR_SUCCESS);
  EXPECT_TRUE(g_stub2.cleanup_called);
}

TEST_F(VscpWs2, clearup_succeeds_when_cleanup_null)
{
  init(&k_min_ops2);
  EXPECT_EQ(vscp_ws2_clearup(&ctx, nullptr), VSCP_ERROR_SUCCESS);
}

// ===========================================================================
// handle_request guards and stub response
// ===========================================================================

TEST_F(VscpWs2, handle_request_null_packet_returns_error)
{
  init(&k_min_ops2);
  EXPECT_NE(vscp_ws2_handle_request(&ctx, nullptr, 2), VSCP_ERROR_SUCCESS);
}

TEST_F(VscpWs2, handle_request_zero_len_returns_error)
{
  init(&k_min_ops2);
  EXPECT_NE(vscp_ws2_handle_request(&ctx, "{}", 0), VSCP_ERROR_SUCCESS);
}

TEST_F(VscpWs2, handle_request_stub_returns_not_implemented)
{
  init(&k_min_ops2);
  g_stub2.last_reply.clear();
  // The stub implementation replies with a JSON error object
  vscp_ws2_handle_request(&ctx, "{\"type\":\"COMMAND\",\"command\":\"NOOP\"}", 35);
  EXPECT_FALSE(g_stub2.last_reply.empty());
  EXPECT_NE(g_stub2.last_reply.find("ERROR"), std::string::npos);
}
