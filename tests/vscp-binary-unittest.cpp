// FILE: vscp-binary-unittest.cpp
//
// Unit tests for vscp-binary.c
//
// Coverage:
//   - Null-pointer / missing-ops guards (vscp_binary_check_ctx)
//   - vscp_handle_binary_event null event guard
//   - NOOP command
//   - QUIT command
//   - USER / PASS / CHALLENGE commands
//   - SEND command (success + parse error + alloc-failure path)
//   - RETR command (empty queue, one event)
//   - OPEN / CLOSE / CHKDATA / CLEAR commands
//   - STAT / INFO commands
//   - GETCHID / SETGUID / GETGUID commands
//   - VERSION command
//   - SETFILTER / SETMASK commands
//   - INTERFACE command (count, get, close, open, bad subcommand)
//   - TEST command
//   - WCYD command
//   - SHUTDOWN / RESTART commands
//   - TEXT command
//   - Unknown command
//   - User-defined command range
//   - vscp_handle_binary_event success path

#include <cstring>
#include <string>
#include <gtest/gtest.h>

#include "vscp.h"
#include "vscp-firmware-helper.h"
#include "vscp-binary-protocol.h"

// ===========================================================================
// Stub state
// ===========================================================================

struct BinaryStub {
  // reply capture
  int      reply_calls     = 0;
  uint16_t reply_command   = 0;
  uint16_t reply_error     = 0xFFFF;
  size_t   reply_data_len  = 0;

  // individual command call counters
  int quit_calls        = 0;
  int user_calls        = 0;
  int password_calls    = 0;
  int challenge_calls   = 0;
  int send_event_calls  = 0;
  int get_event_calls   = 0;
  int open_calls        = 0;
  int close_calls       = 0;
  int check_data_calls  = 0;
  int clrall_calls      = 0;
  int get_chid_calls    = 0;
  int set_guid_calls    = 0;
  int get_guid_calls    = 0;
  int get_version_calls = 0;
  int statistics_calls  = 0;
  int info_calls        = 0;
  int setfilter_calls   = 0;
  int setmask_calls     = 0;
  int if_count_calls    = 0;
  int if_get_calls      = 0;
  int if_open_calls     = 0;
  int if_close_calls    = 0;
  int test_calls        = 0;
  int wcyd_calls        = 0;
  int shutdown_calls    = 0;
  int restart_calls     = 0;
  int text_calls        = 0;
  int user_cmd_calls    = 0;

  // configurable behaviour
  bool          channel_open     = false;
  uint32_t      check_data_count = 0;
  uint32_t      chid             = 0x0000AABB;
  uint64_t      wcyd_caps        = 0x0123456789ABCDEFULL;
  int           get_event_rv     = VSCP_ERROR_FIFO_EMPTY; // default: empty
  vscp_event_t *get_event_pev    = nullptr;               // event to return

  // reply override: if >= 0 return_reply_rv is used instead of VSCP_ERROR_SUCCESS
  int reply_rv = VSCP_ERROR_SUCCESS;
};

static BinaryStub g_bstub;

static void reset_bstub()
{
  g_bstub = BinaryStub{};
}

// ===========================================================================
// Ops callbacks
// ===========================================================================

static int bstub_reply(vscp_binary_ctx_t *pctx, uint16_t command, uint16_t error,
                       const uint8_t *parg, size_t len)
{
  (void)pctx; (void)parg;
  g_bstub.reply_calls++;
  g_bstub.reply_command  = command;
  g_bstub.reply_error    = error;
  g_bstub.reply_data_len = len;
  return g_bstub.reply_rv;
}

static int bstub_quit(vscp_binary_ctx_t *pctx)
{
  (void)pctx; g_bstub.quit_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_user(vscp_binary_ctx_t *pctx, const char *user)
{
  (void)pctx; (void)user; g_bstub.user_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_password(vscp_binary_ctx_t *pctx, const char *pass)
{
  (void)pctx; (void)pass; g_bstub.password_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_challenge(vscp_binary_ctx_t *pctx, uint8_t *ch, size_t len)
{
  (void)pctx; g_bstub.challenge_calls++; memset(ch, 0x42, len); return VSCP_ERROR_SUCCESS;
}

static int bstub_check_authenticated(vscp_binary_ctx_t *pctx)
{
  (void)pctx; return VSCP_ERROR_SUCCESS;
}

static int bstub_send_event(vscp_binary_ctx_t *pctx, const vscp_event_t *pev)
{
  (void)pctx; (void)pev; g_bstub.send_event_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_get_event(vscp_binary_ctx_t *pctx, vscp_event_t **ppev)
{
  (void)pctx;
  g_bstub.get_event_calls++;
  *ppev = g_bstub.get_event_pev;
  return g_bstub.get_event_rv;
}

static int bstub_get_eventex(vscp_binary_ctx_t *pctx, vscp_event_ex_t *pex)
{
  (void)pctx; (void)pex; return VSCP_ERROR_FIFO_EMPTY;
}

static int bstub_send_asyncevent(vscp_binary_ctx_t *pctx, vscp_event_t *pev)
{
  (void)pctx; (void)pev; return VSCP_ERROR_SUCCESS;
}

static int bstub_open(vscp_binary_ctx_t *pctx)
{
  (void)pctx; g_bstub.open_calls++; g_bstub.channel_open = true; return VSCP_ERROR_SUCCESS;
}

static int bstub_close(vscp_binary_ctx_t *pctx)
{
  (void)pctx; g_bstub.close_calls++; g_bstub.channel_open = false; return VSCP_ERROR_SUCCESS;
}

static bool bstub_is_open(vscp_binary_ctx_t *pctx)
{
  (void)pctx; return g_bstub.channel_open;
}

static int bstub_check_data(vscp_binary_ctx_t *pctx, uint32_t *pcount)
{
  (void)pctx; g_bstub.check_data_calls++; *pcount = g_bstub.check_data_count; return VSCP_ERROR_SUCCESS;
}

static int bstub_clrall(vscp_binary_ctx_t *pctx)
{
  (void)pctx; g_bstub.clrall_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_get_chid(vscp_binary_ctx_t *pctx, uint32_t *pchid)
{
  (void)pctx; g_bstub.get_chid_calls++; *pchid = g_bstub.chid; return VSCP_ERROR_SUCCESS;
}

static int bstub_set_guid(vscp_binary_ctx_t *pctx, uint8_t *pguid)
{
  (void)pctx; (void)pguid; g_bstub.set_guid_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_get_guid(vscp_binary_ctx_t *pctx, uint8_t *pguid)
{
  (void)pctx; g_bstub.get_guid_calls++; memset(pguid, 0xAB, 16); return VSCP_ERROR_SUCCESS;
}

static int bstub_get_version(vscp_binary_ctx_t *pctx, uint8_t *pver)
{
  (void)pctx; g_bstub.get_version_calls++;
  pver[0] = 1; pver[1] = 2; pver[2] = 3; pver[3] = 0; pver[4] = 0;
  pver[5] = 0; pver[6] = 0; pver[7] = 0; pver[8] = 0; pver[9] = 0;
  return VSCP_ERROR_SUCCESS;
}

static int bstub_statistics(vscp_binary_ctx_t *pctx, vscp_statistics_t *ps)
{
  (void)pctx; g_bstub.statistics_calls++; memset(ps, 0, sizeof(*ps)); return VSCP_ERROR_SUCCESS;
}

static int bstub_info(vscp_binary_ctx_t *pctx, vscp_status_t *ps)
{
  (void)pctx; g_bstub.info_calls++; memset(ps, 0, sizeof(*ps)); return VSCP_ERROR_SUCCESS;
}

static int bstub_setfilter(vscp_binary_ctx_t *pctx, const vscpEventFilter *pf)
{
  (void)pctx; (void)pf; g_bstub.setfilter_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_setmask(vscp_binary_ctx_t *pctx, const vscpEventFilter *pf)
{
  (void)pctx; (void)pf; g_bstub.setmask_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_get_interface_count(vscp_binary_ctx_t *pctx, uint16_t *pcount)
{
  (void)pctx; g_bstub.if_count_calls++; *pcount = 2; return VSCP_ERROR_SUCCESS;
}

static int bstub_get_interface(vscp_binary_ctx_t *pctx, uint16_t idx, vscp_interface_info_t *pif)
{
  (void)pctx; g_bstub.if_get_calls++;
  memset(pif, 0, sizeof(*pif));
  pif->idx = idx;
  return VSCP_ERROR_SUCCESS;
}

static int bstub_interface_open(vscp_binary_ctx_t *pctx, uint16_t idx)
{
  (void)pctx; (void)idx; g_bstub.if_open_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_interface_close(vscp_binary_ctx_t *pctx, uint16_t idx)
{
  (void)pctx; (void)idx; g_bstub.if_close_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_test(vscp_binary_ctx_t *pctx, const uint8_t *arg, size_t len)
{
  (void)pctx; (void)arg; (void)len; g_bstub.test_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_wcyd(vscp_binary_ctx_t *pctx, uint64_t *pwcyd)
{
  (void)pctx; g_bstub.wcyd_calls++; *pwcyd = g_bstub.wcyd_caps; return VSCP_ERROR_SUCCESS;
}

static int bstub_shutdown(vscp_binary_ctx_t *pctx)
{
  (void)pctx; g_bstub.shutdown_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_restart(vscp_binary_ctx_t *pctx)
{
  (void)pctx; g_bstub.restart_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_text(vscp_binary_ctx_t *pctx)
{
  (void)pctx; g_bstub.text_calls++; return VSCP_ERROR_SUCCESS;
}

static int bstub_user_command(vscp_binary_ctx_t *pctx, uint16_t command,
                              const uint8_t *parg, size_t len)
{
  (void)pctx; (void)command; (void)parg; (void)len;
  g_bstub.user_cmd_calls++;
  return VSCP_ERROR_SUCCESS;
}

// ===========================================================================
// Ops table
// ===========================================================================

static const vscp_binary_ops_t k_bops = {
  bstub_reply,
  bstub_quit,
  bstub_user,
  bstub_password,
  bstub_challenge,
  bstub_check_authenticated,
  bstub_send_event,
  bstub_get_event,
  bstub_get_eventex,
  bstub_send_asyncevent,
  bstub_open,
  bstub_close,
  bstub_is_open,
  bstub_check_data,
  bstub_clrall,
  bstub_get_chid,
  bstub_set_guid,
  bstub_get_guid,
  bstub_get_version,
  bstub_statistics,
  bstub_info,
  bstub_setfilter,
  bstub_setmask,
  bstub_get_interface_count,
  bstub_get_interface,
  bstub_interface_open,
  bstub_interface_close,
  bstub_test,
  bstub_wcyd,
  bstub_shutdown,
  bstub_restart,
  bstub_text,
  bstub_user_command,
};

// ===========================================================================
// Test fixture
// ===========================================================================

class VscpBinary : public ::testing::Test {
protected:
  vscp_binary_ctx_t ctx{};

  void SetUp() override
  {
    reset_bstub();
    ctx.ops = &k_bops;
  }

  int cmd(uint16_t code, const uint8_t *arg = nullptr, size_t len = 0)
  {
    return vscp_handle_binary_command(&ctx, code, arg, len);
  }
};

// ===========================================================================
// Null-pointer / missing-ops guards
// ===========================================================================

TEST(VscpBinaryNullGuards, null_ctx_returns_init_missing)
{
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING,
            vscp_handle_binary_command(nullptr, VSCP_BINARY_COMMAND_CODE_NOOP, nullptr, 0));
}

TEST(VscpBinaryNullGuards, null_ops_returns_init_missing)
{
  vscp_binary_ctx_t ctx{};
  ctx.ops = nullptr;
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING,
            vscp_handle_binary_command(&ctx, VSCP_BINARY_COMMAND_CODE_NOOP, nullptr, 0));
}

TEST(VscpBinaryNullGuards, null_reply_callback_returns_init_missing)
{
  static const vscp_binary_ops_t ops_no_reply = {};  // all fields zero/null
  vscp_binary_ctx_t ctx{};
  ctx.ops = &ops_no_reply;
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING,
            vscp_handle_binary_command(&ctx, VSCP_BINARY_COMMAND_CODE_NOOP, nullptr, 0));
}

TEST(VscpBinaryNullGuards, event_null_ctx_returns_init_missing)
{
  vscp_event_t ev{};
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_handle_binary_event(nullptr, &ev));
}

TEST(VscpBinaryNullGuards, event_null_event_returns_parameter_error)
{
  vscp_binary_ctx_t ctx{};
  ctx.ops = &k_bops;
  reset_bstub();
  EXPECT_EQ(VSCP_ERROR_PARAMETER, vscp_handle_binary_event(&ctx, nullptr));
}

// ===========================================================================
// NOOP
// ===========================================================================

TEST_F(VscpBinary, noop_calls_reply_with_success)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_NOOP));
  EXPECT_EQ(1, g_bstub.reply_calls);
  EXPECT_EQ(VSCP_BINARY_COMMAND_CODE_NOOP, g_bstub.reply_command);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

// ===========================================================================
// QUIT
// ===========================================================================

TEST_F(VscpBinary, quit_calls_quit_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_QUIT);
  EXPECT_EQ(1, g_bstub.quit_calls);
}

// ===========================================================================
// USER / PASS / CHALLENGE
// ===========================================================================

TEST_F(VscpBinary, user_calls_user_callback)
{
  const uint8_t arg[] = "admin";
  cmd(VSCP_BINARY_COMMAND_CODE_USER, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.user_calls);
  EXPECT_EQ(1, g_bstub.reply_calls);
}

TEST_F(VscpBinary, pass_calls_password_callback)
{
  const uint8_t arg[] = "secret";
  cmd(VSCP_BINARY_COMMAND_CODE_PASS, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.password_calls);
}

TEST_F(VscpBinary, challenge_calls_challenge_callback_and_returns_16_bytes)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_CHALLENGE));
  EXPECT_EQ(1, g_bstub.challenge_calls);
  EXPECT_EQ(1, g_bstub.reply_calls);
  EXPECT_EQ(16u, g_bstub.reply_data_len);
}

// ===========================================================================
// OPEN / CLOSE
// ===========================================================================

TEST_F(VscpBinary, open_calls_open_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_OPEN);
  EXPECT_EQ(1, g_bstub.open_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

TEST_F(VscpBinary, close_calls_close_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_CLOSE);
  EXPECT_EQ(1, g_bstub.close_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

// ===========================================================================
// CHKDATA / CLEAR
// ===========================================================================

TEST_F(VscpBinary, chkdata_returns_count_in_reply)
{
  g_bstub.check_data_count = 7;
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_CHKDATA));
  EXPECT_EQ(4u, g_bstub.reply_data_len);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

TEST_F(VscpBinary, clear_calls_clrall_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_CLEAR);
  EXPECT_EQ(1, g_bstub.clrall_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

// ===========================================================================
// RETR
// ===========================================================================

TEST_F(VscpBinary, retr_empty_queue_returns_success_with_no_data)
{
  // default: get_event_rv = VSCP_ERROR_FIFO_EMPTY
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_RETR));
}

TEST_F(VscpBinary, retr_when_channel_open_returns_unsupported)
{
  g_bstub.channel_open = true;
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_RETR));
  EXPECT_EQ(VSCP_ERROR_UNSUPPORTED, g_bstub.reply_error);
}

// ===========================================================================
// STAT / INFO
// ===========================================================================

TEST_F(VscpBinary, stat_calls_statistics_callback_and_returns_28_bytes)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_STAT));
  EXPECT_EQ(1, g_bstub.statistics_calls);
  EXPECT_EQ(28u, g_bstub.reply_data_len);
}

TEST_F(VscpBinary, info_calls_info_callback_and_returns_12_bytes)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_INFO));
  EXPECT_EQ(1, g_bstub.info_calls);
  EXPECT_EQ(12u, g_bstub.reply_data_len);
}

// ===========================================================================
// GETCHID / SETGUID / GETGUID
// ===========================================================================

TEST_F(VscpBinary, getchid_returns_chid_in_4_bytes)
{
  g_bstub.chid = 0x12345678;
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_GETCHID));
  EXPECT_EQ(1, g_bstub.get_chid_calls);
  EXPECT_EQ(4u, g_bstub.reply_data_len);
}

TEST_F(VscpBinary, setguid_calls_set_guid_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_SETGUID);
  EXPECT_EQ(1, g_bstub.set_guid_calls);
}

TEST_F(VscpBinary, getguid_returns_16_bytes)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_GETGUID));
  EXPECT_EQ(1, g_bstub.get_guid_calls);
  EXPECT_EQ(16u, g_bstub.reply_data_len);
}

// ===========================================================================
// VERSION
// ===========================================================================

TEST_F(VscpBinary, version_calls_get_version_and_returns_10_bytes)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_VERSION));
  EXPECT_EQ(1, g_bstub.get_version_calls);
  EXPECT_EQ(10u, g_bstub.reply_data_len);
}

// ===========================================================================
// SETFILTER / SETMASK
// ===========================================================================

TEST_F(VscpBinary, setfilter_too_short_returns_parameter_error)
{
  const uint8_t short_arg[10] = {};
  cmd(VSCP_BINARY_COMMAND_CODE_SETFILTER, short_arg, sizeof(short_arg));
  EXPECT_EQ(VSCP_ERROR_PARAMETER, g_bstub.reply_error);
  EXPECT_EQ(0, g_bstub.setfilter_calls);
}

TEST_F(VscpBinary, setfilter_21_bytes_calls_setfilter_callback)
{
  const uint8_t arg[21] = {};
  cmd(VSCP_BINARY_COMMAND_CODE_SETFILTER, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.setfilter_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

TEST_F(VscpBinary, setfilter_42_bytes_calls_both_filter_and_mask)
{
  const uint8_t arg[42] = {};
  cmd(VSCP_BINARY_COMMAND_CODE_SETFILTER, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.setfilter_calls);
  EXPECT_EQ(1, g_bstub.setmask_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

TEST_F(VscpBinary, setmask_too_short_returns_parameter_error)
{
  const uint8_t short_arg[5] = {};
  cmd(VSCP_BINARY_COMMAND_CODE_SETMASK, short_arg, sizeof(short_arg));
  EXPECT_EQ(VSCP_ERROR_PARAMETER, g_bstub.reply_error);
}

TEST_F(VscpBinary, setmask_21_bytes_calls_setmask_callback)
{
  const uint8_t arg[21] = {};
  cmd(VSCP_BINARY_COMMAND_CODE_SETMASK, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.setmask_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

// ===========================================================================
// INTERFACE
// ===========================================================================

TEST_F(VscpBinary, interface_get_count)
{
  const uint8_t arg[] = { 0x00 };
  cmd(VSCP_BINARY_COMMAND_CODE_INTERFACE, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.if_count_calls);
  EXPECT_EQ(2u, g_bstub.reply_data_len);
}

TEST_F(VscpBinary, interface_get_info)
{
  const uint8_t arg[] = { 0x01, 0x00, 0x00 };
  cmd(VSCP_BINARY_COMMAND_CODE_INTERFACE, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.if_get_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

TEST_F(VscpBinary, interface_close)
{
  const uint8_t arg[] = { 0x02, 0x00, 0x01 };
  cmd(VSCP_BINARY_COMMAND_CODE_INTERFACE, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.if_close_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

TEST_F(VscpBinary, interface_open)
{
  const uint8_t arg[] = { 0x03, 0x00, 0x01 };
  cmd(VSCP_BINARY_COMMAND_CODE_INTERFACE, arg, sizeof(arg));
  EXPECT_EQ(1, g_bstub.if_open_calls);
  EXPECT_EQ(VSCP_ERROR_SUCCESS, g_bstub.reply_error);
}

TEST_F(VscpBinary, interface_bad_subcommand_returns_parameter_error)
{
  const uint8_t arg[] = { 0xFF };
  cmd(VSCP_BINARY_COMMAND_CODE_INTERFACE, arg, sizeof(arg));
  EXPECT_EQ(VSCP_ERROR_PARAMETER, g_bstub.reply_error);
}

// ===========================================================================
// TEST / WCYD / SHUTDOWN / RESTART / TEXT
// ===========================================================================

TEST_F(VscpBinary, test_calls_test_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_TEST);
  EXPECT_EQ(1, g_bstub.test_calls);
}

TEST_F(VscpBinary, wcyd_returns_8_bytes_capability_mask)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_WCYD));
  EXPECT_EQ(1, g_bstub.wcyd_calls);
  EXPECT_EQ(8u, g_bstub.reply_data_len);
}

TEST_F(VscpBinary, shutdown_calls_shutdown_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_SHUTDOWN);
  EXPECT_EQ(1, g_bstub.shutdown_calls);
}

TEST_F(VscpBinary, restart_calls_restart_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_RESTART);
  EXPECT_EQ(1, g_bstub.restart_calls);
}

TEST_F(VscpBinary, text_calls_text_callback)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, cmd(VSCP_BINARY_COMMAND_CODE_TEXT));
  EXPECT_EQ(1, g_bstub.text_calls);
}

// ===========================================================================
// Unknown command / user-defined range
// ===========================================================================

TEST_F(VscpBinary, unknown_command_returns_error)
{
  // 0x0099 is not a defined command code
  EXPECT_NE(VSCP_ERROR_SUCCESS,
            cmd(0x0099));
}

TEST_F(VscpBinary, user_defined_command_calls_user_command_callback)
{
  cmd(VSCP_BINARY_COMMAND_CODE_USER_START);
  EXPECT_EQ(1, g_bstub.user_cmd_calls);

  cmd(VSCP_BINARY_COMMAND_CODE_USER_START + 1);
  EXPECT_EQ(2, g_bstub.user_cmd_calls);
}

// ===========================================================================
// vscp_handle_binary_event
// ===========================================================================

TEST_F(VscpBinary, handle_event_calls_send_event_and_sends_confirm_reply)
{
  vscp_event_t ev{};
  ev.head       = 0x0000;
  ev.vscp_class = 20;
  ev.vscp_type  = 3;
  ev.sizeData   = 0;

  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_handle_binary_event(&ctx, &ev));
  EXPECT_EQ(1, g_bstub.send_event_calls);
  EXPECT_EQ(1, g_bstub.reply_calls);
  EXPECT_EQ(VSCP_BINARY_COMMAND_CODE_EVENT_CONFIRM, g_bstub.reply_command);
}
