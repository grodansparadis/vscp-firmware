// FILE: vscp-link-protocol-unittest.cpp
//
// Unit tests for vscp-link-protocol.c
//
// Coverage:
//   - Null-pointer guards (vscp_link_check_ctx / vscp_link_check_ctx_auth)
//   - vscp_link_connect / vscp_link_disconnect
//   - vscp_link_parser: incomplete input, empty line, unknown command, CRLF split
//   - Command dispatch via parser: noop, help, quit, user, pass, aliases
//   - Direct handler calls: noop, help, quit, user, pass
//   - Auth gate: unauthenticated commands return NOT_ACCREDITED
//   - Privilege gate: low-privilege returns LOW_PRIVILEGE_ERROR
//   - Authenticated success paths: retr, chkdata, clrall, version, getguid, rcvloop/quitloop

#include <cstring>
#include <string>
#include <gtest/gtest.h>

// Macros consumed by vscp-link-protocol.c; define before any header.
#define THIS_FIRMWARE_TCPIP_LINK_MAX_BUFFER         512
#define THIS_FIRMWARE_TCPIP_LINK_ENABLE_RCVLOOP_CMD 1

// The header uses extern "C" { } correctly; include directly in C++ mode.
#include <vscp.h>
#include <vscp-fifo.h>
#include <vscp-firmware-helper.h>
#include <vscp-link-protocol.h>

// ===========================================================================
// Stub state
// ===========================================================================

struct StubState {
  std::string written;
  bool        authenticated;
  uint8_t     priv_level;
  int         welcome_calls;
  int         disconnect_calls;
  int         quit_calls;
  int         check_user_calls;
  int         check_password_calls;
  int         rcvloop_status;
  int         enable_rcvloop_calls;
  int         enable_rcvloop_last_val;
};

static StubState g_stub;

static void reset_stub()
{
  g_stub               = StubState{};
  g_stub.authenticated = false;
  g_stub.priv_level    = 0;
}

// ===========================================================================
// Ops callbacks
// ===========================================================================

static int stub_write_client(vscp_link_ctx_t *ctx, const char *msg)
{
  (void)ctx;
  if (msg) g_stub.written += msg;
  return VSCP_ERROR_SUCCESS;
}

static int stub_welcome(vscp_link_ctx_t *ctx)
{
  g_stub.welcome_calls++;
  stub_write_client(ctx, VSCP_LINK_MSG_WELCOME);
  return VSCP_ERROR_SUCCESS;
}

static int stub_disconnect(vscp_link_ctx_t *ctx) { (void)ctx; g_stub.disconnect_calls++; return VSCP_ERROR_SUCCESS; }
static int stub_quit(vscp_link_ctx_t *ctx) { g_stub.quit_calls++; stub_write_client(ctx, VSCP_LINK_MSG_GOODBY); return VSCP_ERROR_SUCCESS; }

static int stub_check_user(vscp_link_ctx_t *ctx, const char *user)
{
  (void)user; g_stub.check_user_calls++; stub_write_client(ctx, VSCP_LINK_MSG_USENAME_OK); return VSCP_ERROR_SUCCESS;
}

static int stub_check_password(vscp_link_ctx_t *ctx, const char *pass)
{
  (void)pass; g_stub.check_password_calls++; stub_write_client(ctx, VSCP_LINK_MSG_PASSWORD_OK); return VSCP_ERROR_SUCCESS;
}

static int stub_challenge(vscp_link_ctx_t *ctx, const char *arg)
{
  (void)arg; stub_write_client(ctx, "+OK - challenge\r\n"); return VSCP_ERROR_SUCCESS;
}

static int stub_check_authenticated(vscp_link_ctx_t *ctx)
{
  (void)ctx; return g_stub.authenticated ? VSCP_ERROR_SUCCESS : VSCP_ERROR_NOT_SUPPORTED;
}

static int stub_check_privilege(vscp_link_ctx_t *ctx, uint8_t priv)
{
  (void)ctx; return (g_stub.priv_level >= priv) ? VSCP_ERROR_SUCCESS : VSCP_ERROR_NOT_SUPPORTED;
}

static int stub_get_rcvloop_status(vscp_link_ctx_t *ctx, int *pStatus)
{
  (void)ctx; *pStatus = g_stub.rcvloop_status; return VSCP_ERROR_SUCCESS;
}

static int stub_enable_rcvloop(vscp_link_ctx_t *ctx, int enable)
{
  (void)ctx;
  g_stub.enable_rcvloop_calls++;
  g_stub.enable_rcvloop_last_val = enable;
  g_stub.rcvloop_status          = enable;
  return VSCP_ERROR_SUCCESS;
}

static int stub_send(vscp_link_ctx_t *ctx, vscp_event_t *pev)    { (void)ctx; vscp_fwhlp_deleteEvent(&pev); return VSCP_ERROR_SUCCESS; }
static int stub_retr(vscp_link_ctx_t *ctx, vscp_event_t **ppev)  { (void)ctx; *ppev = nullptr; return VSCP_ERROR_RCV_EMPTY; }
static int stub_chkdata(vscp_link_ctx_t *ctx, uint16_t *pcount)  { (void)ctx; *pcount = 0; return VSCP_ERROR_SUCCESS; }
static int stub_clrall(vscp_link_ctx_t *ctx)                     { (void)ctx; return VSCP_ERROR_SUCCESS; }

static int stub_get_version(vscp_link_ctx_t *ctx, uint8_t *pv)
{
  (void)ctx; pv[0] = 1; pv[1] = 2; pv[2] = 3; pv[3] = 0; return VSCP_ERROR_SUCCESS;
}

static int stub_get_channel_id(vscp_link_ctx_t *ctx, uint16_t *pchid) { (void)ctx; *pchid = 42; return VSCP_ERROR_SUCCESS; }
static int stub_get_guid(vscp_link_ctx_t *ctx, uint8_t *pguid)         { (void)ctx; memset(pguid, 0, 16); return VSCP_ERROR_SUCCESS; }
static int stub_set_guid(vscp_link_ctx_t *ctx, uint8_t *pguid)         { (void)ctx; (void)pguid; return VSCP_ERROR_SUCCESS; }
static int stub_statistics(vscp_link_ctx_t *ctx, vscp_statistics_t *ps){ (void)ctx; memset(ps, 0, sizeof(*ps)); return VSCP_ERROR_SUCCESS; }
static int stub_info(vscp_link_ctx_t *ctx, vscp_status_t *ps)          { (void)ctx; memset(ps, 0, sizeof(*ps)); return VSCP_ERROR_SUCCESS; }
static int stub_wcyd(vscp_link_ctx_t *ctx, uint64_t *pwcyd)            { (void)ctx; *pwcyd = 0xFFFFFFFFFFFFFFFFULL; return VSCP_ERROR_SUCCESS; }
static int stub_set_filter(vscp_link_ctx_t *ctx, vscpEventFilter *pf)  { (void)ctx; (void)pf; return VSCP_ERROR_SUCCESS; }
static int stub_set_mask(vscp_link_ctx_t *ctx, vscpEventFilter *pf)    { (void)ctx; (void)pf; return VSCP_ERROR_SUCCESS; }
static int stub_test(vscp_link_ctx_t *ctx, const char *arg)            { (void)arg; stub_write_client(ctx, VSCP_LINK_MSG_OK); return VSCP_ERROR_SUCCESS; }
static int stub_shutdown(vscp_link_ctx_t *ctx)                         { (void)ctx; return VSCP_ERROR_SUCCESS; }
static int stub_restart(vscp_link_ctx_t *ctx)                          { (void)ctx; return VSCP_ERROR_SUCCESS; }
static int stub_sec(vscp_link_ctx_t *ctx)                              { (void)ctx; return VSCP_ERROR_SUCCESS; }
static int stub_binary(vscp_link_ctx_t *ctx)                           { (void)ctx; return VSCP_ERROR_SUCCESS; }
static uint16_t stub_get_interface_count(vscp_link_ctx_t *ctx)         { (void)ctx; return 0; }

static int stub_get_interface(vscp_link_ctx_t *ctx, uint16_t idx, vscp_interface_info_t *pif)
{
  (void)ctx; (void)idx; (void)pif; return VSCP_ERROR_UNKNOWN_ITEM;
}

static int stub_close_interface(vscp_link_ctx_t *ctx, uint8_t *pguid) { (void)ctx; (void)pguid; return VSCP_ERROR_SUCCESS; }

// ===========================================================================
// Standalone vscp_link_callback_* implementations required by the library.
// Delegate through pctx->ops so tests control behaviour via the ops table.
// ===========================================================================

extern "C" {

int vscp_link_callback_check_authenticated(const vscp_link_ctx_t *pctx)
{
  return pctx->ops->check_authenticated(const_cast<vscp_link_ctx_t *>(pctx));
}

int vscp_link_callback_check_privilege(const vscp_link_ctx_t *pctx, uint8_t priv)
{
  return pctx->ops->check_privilege(const_cast<vscp_link_ctx_t *>(pctx), priv);
}

int vscp_link_callback_get_rcvloop_status(const vscp_link_ctx_t *pctx, int *pStatus)
{
  return pctx->ops->get_rcvloop_status(const_cast<vscp_link_ctx_t *>(pctx), pStatus);
}

int vscp_link_callback_enable_rcvloop(const vscp_link_ctx_t *pctx, int enable)
{
  return pctx->ops->enable_rcvloop(const_cast<vscp_link_ctx_t *>(pctx), enable);
}

// Linker stubs for callbacks declared in the header but not called by the library core.
int vscp_link_callback_welcome(const vscp_link_ctx_t *pctx)            { return pctx->ops->welcome(const_cast<vscp_link_ctx_t *>(pctx)); }
int vscp_link_callback_quit(const vscp_link_ctx_t *pctx)               { return pctx->ops->quit(const_cast<vscp_link_ctx_t *>(pctx)); }
int vscp_link_callback_write_client(const vscp_link_ctx_t *pctx, const char *msg) { return pctx->ops->write_client(const_cast<vscp_link_ctx_t *>(pctx), msg); }
int vscp_link_callback_disconnect(const vscp_link_ctx_t *pctx)         { return pctx->ops->disconnect(const_cast<vscp_link_ctx_t *>(pctx)); }
int vscp_link_callback_help(const vscp_link_ctx_t *pctx, const char *arg) { (void)pctx; (void)arg; return VSCP_ERROR_SUCCESS; }
int vscp_link_callback_event_received(const vscp_link_ctx_t *pctx, const vscpEvent *pev) { (void)pctx; (void)pev; return VSCP_ERROR_SUCCESS; }
uint16_t vscp_link_callback_get_interface_count(const vscp_link_ctx_t *pctx) { (void)pctx; return 0; }
int vscp_link_callback_get_interface(const vscp_link_ctx_t *pctx, uint16_t index, vscp_interface_info_t *pif) { (void)pctx; (void)index; (void)pif; return VSCP_ERROR_UNKNOWN_ITEM; }
int vscp_link_callback_close_interface(const vscp_link_ctx_t *pctx, uint8_t *pguid) { (void)pctx; (void)pguid; return VSCP_ERROR_SUCCESS; }
int vscp_link_callback_check_user(const vscp_link_ctx_t *pctx, const char *user) { (void)pctx; (void)user; return VSCP_ERROR_SUCCESS; }
int vscp_link_callback_check_password(const vscp_link_ctx_t *pctx, const char *pass) { (void)pctx; (void)pass; return VSCP_ERROR_SUCCESS; }
int vscp_link_callback_challenge(const vscp_link_ctx_t *pctx, const char *pw) { (void)pctx; (void)pw; return VSCP_ERROR_SUCCESS; }
int vscp_link_callback_test(const vscp_link_ctx_t *pctx, const char *arg) { (void)pctx; (void)arg; return VSCP_ERROR_SUCCESS; }
int vscp_link_callback_send(const vscp_link_ctx_t *pctx, vscpEvent *pev)  { (void)pctx; vscp_fwhlp_deleteEvent(&pev); return VSCP_ERROR_SUCCESS; }
int vscp_link_callback_retr(const vscp_link_ctx_t *pctx, vscpEvent **ppev){ (void)pctx; *ppev = nullptr; return VSCP_ERROR_RCV_EMPTY; }

} // extern "C"

// ===========================================================================
// Full ops table
// ===========================================================================

static const vscp_link_ops_t k_stub_ops = {
  stub_write_client,
  stub_disconnect,
  stub_quit,
  stub_welcome,
  stub_check_user,
  stub_check_password,
  stub_challenge,
  stub_check_authenticated,
  stub_check_privilege,
  stub_send,
  stub_chkdata,
  stub_clrall,
  stub_retr,
  nullptr,               // rcvloop — not needed for parser tests
  stub_enable_rcvloop,
  stub_get_rcvloop_status,
  stub_get_guid,
  stub_set_guid,
  stub_get_version,
  stub_get_channel_id,
  stub_wcyd,
  stub_statistics,
  stub_info,
  stub_set_filter,
  stub_set_mask,
  stub_get_interface_count,
  stub_get_interface,
  stub_close_interface,
  nullptr,               // help_custom
  stub_test,
  stub_shutdown,
  stub_restart,
  stub_binary,
};

// ===========================================================================
// Test fixture
// ===========================================================================

class VscpLinkProtocol : public ::testing::Test {
protected:
  vscp_link_ctx_t ctx{};

  void SetUp() override
  {
    reset_stub();
    memset(&ctx, 0, sizeof(ctx));
    ctx.ops = &k_stub_ops;
  }

  int parse(const std::string &cmd)
  {
    std::string buf = cmd + "\r\n";
    char *next      = nullptr;
    return vscp_link_parser(&ctx, buf.data(), &next);
  }
};

// ===========================================================================
// Null-pointer guard tests
// ===========================================================================

TEST(VscpLinkNullGuards, connect_null_ctx)
{
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_link_connect(nullptr));
}

TEST(VscpLinkNullGuards, connect_null_ops)
{
  vscp_link_ctx_t ctx{};
  ctx.ops = nullptr;
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_link_connect(&ctx));
}

TEST(VscpLinkNullGuards, disconnect_null_ctx)
{
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_link_disconnect(nullptr));
}

TEST(VscpLinkNullGuards, idle_worker_null_ctx)
{
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_link_idle_worker(nullptr));
}

TEST(VscpLinkNullGuards, parser_null_ctx)
{
  char buf[] = "noop\r\n";
  char *next = nullptr;
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_link_parser(nullptr, buf, &next));
}

TEST(VscpLinkNullGuards, parser_null_pnext)
{
  vscp_link_ctx_t ctx{};
  const vscp_link_ops_t ops{ stub_write_client };
  ctx.ops = &ops;
  char buf[] = "noop\r\n";
  EXPECT_EQ(VSCP_ERROR_INVALID_POINTER, vscp_link_parser(&ctx, buf, nullptr));
}

TEST(VscpLinkNullGuards, noop_null_ctx)
{
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_link_doCmdNoop(nullptr, ""));
}

TEST(VscpLinkNullGuards, send_null_ctx)
{
  EXPECT_EQ(VSCP_ERROR_INIT_MISSING, vscp_link_doCmdSend(nullptr, ""));
}

// ===========================================================================
// vscp_link_connect / vscp_link_disconnect
// ===========================================================================

TEST_F(VscpLinkProtocol, connect_calls_welcome)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_link_connect(&ctx));
  EXPECT_EQ(1, g_stub.welcome_calls);
  EXPECT_NE(std::string::npos, g_stub.written.find("Welcome"));
}

TEST_F(VscpLinkProtocol, disconnect_calls_disconnect_op)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_link_disconnect(&ctx));
  EXPECT_EQ(1, g_stub.disconnect_calls);
}

// ===========================================================================
// vscp_link_parser mechanics
// ===========================================================================

TEST_F(VscpLinkProtocol, parser_incomplete_returns_missing)
{
  char buf[] = "noop";  // no CRLF
  char *next = nullptr;
  EXPECT_EQ(VSCP_ERROR_MISSING, vscp_link_parser(&ctx, buf, &next));
}

TEST_F(VscpLinkProtocol, parser_empty_line_returns_ok)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, parse(""));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, parser_sets_pnext_past_crlf)
{
  std::string buf = "noop\r\nremainder";
  char *next      = nullptr;
  vscp_link_parser(&ctx, buf.data(), &next);
  ASSERT_NE(nullptr, next);
  EXPECT_STREQ("remainder", next);
}

TEST_F(VscpLinkProtocol, parser_unknown_command_writes_unknown_msg)
{
  EXPECT_EQ(VSCP_ERROR_MISSING, parse("invalidxyz"));
  EXPECT_NE(std::string::npos, g_stub.written.find("Unknown command"));
}

TEST_F(VscpLinkProtocol, parser_trims_leading_whitespace)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, parse("   noop"));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, parser_trims_trailing_whitespace)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, parse("noop   "));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, parser_case_insensitive_noop)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, parse("NOOP"));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, parser_case_insensitive_help)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, parse("HELP"));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

// ===========================================================================
// Direct command handler tests — no-auth required
// ===========================================================================

TEST_F(VscpLinkProtocol, noop_returns_ok)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_link_doCmdNoop(&ctx, ""));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, help_returns_ok)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_link_doCmdHelp(&ctx, ""));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, quit_calls_quit_op)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_link_doCmdQuit(&ctx, ""));
  EXPECT_EQ(1, g_stub.quit_calls);
}

TEST_F(VscpLinkProtocol, user_calls_check_user)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_link_doCmdUser(&ctx, "admin"));
  EXPECT_EQ(1, g_stub.check_user_calls);
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, password_calls_check_password)
{
  EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_link_doCmdPassword(&ctx, "secret"));
  EXPECT_EQ(1, g_stub.check_password_calls);
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

// ===========================================================================
// Authentication gate tests
// ===========================================================================

TEST_F(VscpLinkProtocol, send_not_authenticated_returns_not_accredited)
{
  g_stub.authenticated = false;
  parse("send 0,20,3,0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00,0");
  EXPECT_NE(std::string::npos, g_stub.written.find("Need to log in"));
}

TEST_F(VscpLinkProtocol, retr_not_authenticated_returns_not_accredited)
{
  g_stub.authenticated = false;
  parse("retr 1");
  EXPECT_NE(std::string::npos, g_stub.written.find("Need to log in"));
}

TEST_F(VscpLinkProtocol, chkdata_not_authenticated_returns_not_accredited)
{
  g_stub.authenticated = false;
  parse("cdta");
  EXPECT_NE(std::string::npos, g_stub.written.find("Need to log in"));
}

TEST_F(VscpLinkProtocol, clrall_not_authenticated_returns_not_accredited)
{
  g_stub.authenticated = false;
  parse("clra");
  EXPECT_NE(std::string::npos, g_stub.written.find("Need to log in"));
}

// ===========================================================================
// Privilege gate tests
// ===========================================================================

TEST_F(VscpLinkProtocol, send_low_privilege_returns_error)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 2;  // need >= 4
  parse("send 0,20,3,0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00,0");
  EXPECT_NE(std::string::npos, g_stub.written.find("higher privilege"));
}

TEST_F(VscpLinkProtocol, send_sufficient_privilege_accepted)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 4;
  parse("send 0,20,3,0,0,0,00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00,0");
  EXPECT_EQ(std::string::npos, g_stub.written.find("higher privilege"));
}

TEST_F(VscpLinkProtocol, setguid_requires_priv_6)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 4;  // need >= 6
  parse("sgid 00:00:00:00:00:00:00:00:00:00:00:00:00:00:00:00");
  EXPECT_NE(std::string::npos, g_stub.written.find("higher privilege"));
}

TEST_F(VscpLinkProtocol, shutdown_requires_priv_15)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 8;  // need 15
  parse("shutdown");
  EXPECT_NE(std::string::npos, g_stub.written.find("higher privilege"));
}

TEST_F(VscpLinkProtocol, restart_requires_priv_15)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 8;
  parse("restart");
  EXPECT_NE(std::string::npos, g_stub.written.find("higher privilege"));
}

// ===========================================================================
// Authenticated success paths
// ===========================================================================

TEST_F(VscpLinkProtocol, retr_authenticated_empty_queue_returns_no_msg)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;
  parse("retr 1");
  EXPECT_NE(std::string::npos, g_stub.written.find("No event"));
}

TEST_F(VscpLinkProtocol, chkdata_authenticated_returns_ok)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;
  parse("cdta");
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, clrall_authenticated_returns_ok)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;
  parse("clra");
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, version_returns_version_string_and_ok)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;
  parse("vers");
  EXPECT_NE(std::string::npos, g_stub.written.find("1,2,3,0"));
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, getguid_authenticated_returns_ok)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;
  parse("ggid");
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, rcvloop_sets_loop_active)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;
  parse("rcvloop");
  EXPECT_EQ(1, g_stub.enable_rcvloop_last_val);
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

TEST_F(VscpLinkProtocol, quitloop_clears_loop)
{
  g_stub.authenticated  = true;
  g_stub.priv_level     = 15;
  g_stub.rcvloop_status = 1;
  parse("quitloop");
  EXPECT_EQ(0, g_stub.enable_rcvloop_last_val);
  EXPECT_NE(std::string::npos, g_stub.written.find("+OK"));
}

// ===========================================================================
// Command alias tests
// ===========================================================================

TEST_F(VscpLinkProtocol, alias_chkdata_same_as_cdta)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;

  parse("cdta");
  std::string w1 = g_stub.written;
  g_stub.written.clear();
  parse("chkdata");
  std::string w2 = g_stub.written;

  EXPECT_NE(std::string::npos, w1.find("+OK"));
  EXPECT_NE(std::string::npos, w2.find("+OK"));
}

TEST_F(VscpLinkProtocol, alias_version_same_as_vers)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;

  parse("vers");
  std::string w1 = g_stub.written;
  g_stub.written.clear();
  parse("version");
  std::string w2 = g_stub.written;

  EXPECT_EQ(w1, w2);
}

TEST_F(VscpLinkProtocol, alias_clrall_same_as_clra)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;

  parse("clra");
  std::string w1 = g_stub.written;
  g_stub.written.clear();
  parse("clrall");
  std::string w2 = g_stub.written;

  EXPECT_EQ(w1, w2);
}

// ===========================================================================
// Binary mode
// ===========================================================================

TEST_F(VscpLinkProtocol, binary_ack_when_ops_binary_present)
{
  g_stub.authenticated = true;
  g_stub.priv_level    = 15;
  parse("binary");
  EXPECT_NE(std::string::npos, g_stub.written.find("Binary mode enabled"));
}

TEST_F(VscpLinkProtocol, binary_nack_when_ops_binary_null)
{
  static const vscp_link_ops_t ops_no_binary = {
    stub_write_client,
    stub_disconnect,
    stub_quit,
    stub_welcome,
    stub_check_user,
    stub_check_password,
    stub_challenge,
    stub_check_authenticated,
    stub_check_privilege,
    stub_send,
    stub_chkdata,
    stub_clrall,
    stub_retr,
    nullptr,               // rcvloop
    stub_enable_rcvloop,
    stub_get_rcvloop_status,
    stub_get_guid,
    stub_set_guid,
    stub_get_version,
    stub_get_channel_id,
    stub_wcyd,
    stub_statistics,
    stub_info,
    stub_set_filter,
    stub_set_mask,
    stub_get_interface_count,
    stub_get_interface,
    stub_close_interface,
    nullptr,               // help_custom
    stub_test,
    stub_shutdown,
    stub_restart,
    nullptr,               // binary — not supported
  };

  reset_stub();
  memset(&ctx, 0, sizeof(ctx));
  ctx.ops = &ops_no_binary;

  std::string buf = "binary\r\n";
  char *next      = nullptr;
  vscp_link_parser(&ctx, buf.data(), &next);
  EXPECT_NE(std::string::npos, g_stub.written.find("Binary mode not supported"));
}
