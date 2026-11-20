#include <cstring>
#include <gtest/gtest.h>
#include <vector>

extern "C" {
#include "vscp.h"
#include "vscp-class.h"
#include "vscp-type.h"
#include "vscp-firmware-level2.h"
}

namespace {

struct Frmw2StubState {
  uint32_t now_ms;
  uint64_t now_ts;
  int send_calls;
  int set_time_calls;
  int stdreg_change_calls;
  int restore_defaults_calls;
  int enter_bootloader_calls;
  int dm_action_calls;
  int segment_heartbeat_calls;
  int report_interest_calls;
  int read_reg_calls;
  int write_reg_calls;
  uint16_t last_read_page;
  uint16_t last_write_page;
  uint32_t last_write_reg;
  uint8_t last_write_val;
  uint8_t last_dm_action;
  // Timestamp simulation: when ts_step != 0 every get_timestamp() call
  // advances now_ts; ts_wrap32 makes it behave like a 32-bit micros().
  uint64_t ts_step;
  bool ts_wrap32;
  int ts_calls;
  bool ts_runaway; // set if a busy-wait never ended
  vscp_event_t last_sent;
  uint8_t last_sent_data[VSCP_MAX_DATA];
} g_stub;

void reset_stub()
{
  std::memset(&g_stub, 0, sizeof(g_stub));
  g_stub.last_sent.pdata = g_stub.last_sent_data;
  g_stub.now_ms = 1000;
  g_stub.now_ts = 12345678;
}

// ---------------------------------------------------------------------------
// Stub callbacks — referenced by the ops table below.
// ---------------------------------------------------------------------------

static uint32_t stub_get_milliseconds(vscp_frmw2_firmware_config_t* /*pcfg*/)
{
  return g_stub.now_ms;
}

static uint64_t stub_get_timestamp(vscp_frmw2_firmware_config_t* /*pcfg*/)
{
  if (g_stub.ts_step) {
    if (++g_stub.ts_calls > 100000) {
      g_stub.ts_runaway = true;
      return UINT64_MAX; // break out of a busy-wait that would never end
    }
    g_stub.now_ts += g_stub.ts_step;
  }
  return g_stub.ts_wrap32 ? (uint32_t) g_stub.now_ts : g_stub.now_ts;
}

static void stub_enter_bootloader(vscp_frmw2_firmware_config_t* /*pcfg*/)
{
  g_stub.enter_bootloader_calls++;
}

static int stub_send_event(vscp_frmw2_firmware_config_t* /*pcfg*/, vscpEvent* pev)
{
  g_stub.send_calls++;
  g_stub.last_sent.head       = pev->head;
  g_stub.last_sent.obid       = pev->obid;
  g_stub.last_sent.year       = pev->year;
  g_stub.last_sent.month      = pev->month;
  g_stub.last_sent.day        = pev->day;
  g_stub.last_sent.hour       = pev->hour;
  g_stub.last_sent.minute     = pev->minute;
  g_stub.last_sent.second     = pev->second;
  g_stub.last_sent.timestamp  = pev->timestamp;
  g_stub.last_sent.vscp_class = pev->vscp_class;
  g_stub.last_sent.vscp_type  = pev->vscp_type;
  std::memcpy(g_stub.last_sent.GUID, pev->GUID, 16);
  g_stub.last_sent.sizeData   = pev->sizeData;
  if (pev->pdata && pev->sizeData > 0) {
    std::memcpy(g_stub.last_sent_data, pev->pdata, pev->sizeData);
  }
  return VSCP_ERROR_SUCCESS;
}

static int stub_dm_action(vscp_frmw2_firmware_config_t* /*pcfg*/,
                          const vscpEvent* const /*pev*/,
                          uint8_t action,
                          const uint8_t* const /*pparam*/)
{
  g_stub.dm_action_calls++;
  g_stub.last_dm_action = action;
  return VSCP_ERROR_SUCCESS;
}

static int stub_segment_ctrl_heartbeat(vscp_frmw2_firmware_config_t* /*pcfg*/,
                                       uint16_t /*segcrc*/,
                                       uint32_t /*time*/)
{
  g_stub.segment_heartbeat_calls++;
  return VSCP_ERROR_SUCCESS;
}

static int stub_report_events_of_interest(vscp_frmw2_firmware_config_t* /*pcfg*/)
{
  g_stub.report_interest_calls++;
  return VSCP_ERROR_SUCCESS;
}

static int stub_set_event_time(vscp_frmw2_firmware_config_t* /*pcfg*/, vscpEvent* pev)
{
  g_stub.set_time_calls++;
  pev->year      = 2026;
  pev->month     = 3;
  pev->day       = 2;
  pev->hour      = 12;
  pev->minute    = 0;
  pev->second    = 0;
  pev->timestamp = g_stub.now_ms;
  return VSCP_ERROR_SUCCESS;
}

static int stub_restore_defaults(vscp_frmw2_firmware_config_t* /*pcfg*/)
{
  g_stub.restore_defaults_calls++;
  return VSCP_ERROR_SUCCESS;
}

static void stub_reset(vscp_frmw2_firmware_config_t* /*pcfg*/) {}

static int stub_get_ip_addr(vscp_frmw2_firmware_config_t* /*pcfg*/,
                            uint8_t* pipaddr,
                            uint8_t size)
{
  std::memset(pipaddr, 0, size);
  return VSCP_ERROR_SUCCESS;
}

static int stub_read_reg(vscp_frmw2_firmware_config_t* /*pcfg*/,
                         uint16_t page,
                         uint32_t /*reg*/,
                         uint8_t* pval)
{
  g_stub.read_reg_calls++;
  g_stub.last_read_page = page;
  *pval = 0;
  return VSCP_ERROR_SUCCESS;
}

static int stub_write_reg(vscp_frmw2_firmware_config_t* /*pcfg*/,
                          uint16_t page,
                          uint32_t reg,
                          uint8_t val)
{
  g_stub.write_reg_calls++;
  g_stub.last_write_page = page;
  g_stub.last_write_reg  = reg;
  g_stub.last_write_val  = val;
  return VSCP_ERROR_SUCCESS;
}

static int stub_stdreg_change(vscp_frmw2_firmware_config_t* /*pcfg*/, uint32_t /*stdreg*/)
{
  g_stub.stdreg_change_calls++;
  return VSCP_ERROR_SUCCESS;
}

static void stub_feed_watchdog(vscp_frmw2_firmware_config_t* /*pcfg*/) {}

static const vscp_frmw2_ops_t g_ops = {
  stub_get_milliseconds,
  stub_get_timestamp,
  stub_send_event,
  stub_dm_action,
  stub_segment_ctrl_heartbeat,
  stub_report_events_of_interest,
  stub_set_event_time,
  stub_get_ip_addr,
  stub_read_reg,
  stub_write_reg,
  stub_stdreg_change,
  stub_restore_defaults,
  stub_enter_bootloader,
  stub_reset,
  stub_feed_watchdog,
};

void init_config(vscp_frmw2_firmware_config_t& cfg)
{
  std::memset(&cfg, 0, sizeof(cfg));
  cfg.level                          = VSCP_LEVEL2;
  cfg.probe_timeout                  = VSCP_FRMW2_UNASSIGNED;
  cfg.probe_timeout_count            = VSCP_FRMW2_UNASSIGNED;
  cfg.nickname                       = 0x0022;
  cfg.bInterestedInAllEvents         = 1;
  cfg.bUse16BitNickname              = 0;
  cfg.bEnableWriteProtectedLocations = 1;
  cfg.ops                              = &g_ops;

  for (int i = 0; i < 16; ++i) {
    cfg.guid[i]   = static_cast<uint8_t>(0xA0 + i);
    cfg.ipaddr[i] = static_cast<uint8_t>(i);
  }

  const char* name = "UnitTestNode";
  std::memset(cfg.deviceName, 0, sizeof(cfg.deviceName));
  std::memcpy(cfg.deviceName, name, std::strlen(name));
}

} // namespace

TEST(_vscp_frmw2, init_level2_sets_active_state)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level = VSCP_LEVEL2;

  const int rv = vscp_frmw2_init(&cfg);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(FRMW2_STATE_ACTIVE, cfg.state);
  ASSERT_EQ(0u, cfg.alarm_status);
  ASSERT_EQ(0u, cfg.errorCounter);
  ASSERT_GE(g_stub.set_time_calls, 1);
}

TEST(_vscp_frmw2, setup_event_level1_uses_nickname_in_guid)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0x1234;

  uint8_t ex_data[VSCP_MAX_DATA];
  vscp_event_t ex;
  std::memset(&ex, 0, sizeof(ex));
  std::memset(ex_data, 0, sizeof(ex_data));
  ex.pdata = ex_data;
  vscp_frmw2_setup_event(&cfg, &ex, 0);

  ASSERT_EQ(VSCP_PRIORITY_NORMAL | VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ex.head);
  ASSERT_EQ(VSCP_CLASS2_LEVEL1_PROTOCOL, ex.vscp_class);
  ASSERT_EQ(VSCP2_TYPE_PROTOCOL_READ_WRITE_RESPONSE, ex.vscp_type);
  ASSERT_EQ(0u, ex.sizeData);
  ASSERT_EQ(0x12, ex.GUID[14]);
  ASSERT_EQ(0x34, ex.GUID[15]);
}

TEST(_vscp_frmw2, send_heartbeat_level2_includes_device_name)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level = VSCP_LEVEL2;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  g_stub.send_calls = 0; // init sends heartbeat+caps; isolate the explicit call below

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_send_heartbeat(&cfg));
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS2_INFORMATION, g_stub.last_sent.vscp_class);
  ASSERT_EQ(VSCP2_TYPE_INFORMATION_HEART_BEAT, g_stub.last_sent.vscp_type);
  ASSERT_EQ(std::strlen(reinterpret_cast<const char*>(cfg.deviceName)), g_stub.last_sent.sizeData);
  ASSERT_EQ(0, std::memcmp(cfg.deviceName, g_stub.last_sent.pdata, g_stub.last_sent.sizeData));
}

TEST(_vscp_frmw2, send_caps_contains_guid_ip_and_name)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level = VSCP_LEVEL2;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  g_stub.send_calls = 0; // init sends heartbeat+caps; isolate the explicit call below

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_send_caps(&cfg));

  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS2_PROTOCOL, g_stub.last_sent.vscp_class);
  ASSERT_EQ(VSCP2_TYPE_PROTOCOL_HIGH_END_SERVER_CAPS, g_stub.last_sent.vscp_type);
  ASSERT_EQ(104, g_stub.last_sent.sizeData);
  ASSERT_EQ(0x40, g_stub.last_sent.pdata[0]); // Decision matrix
  ASSERT_EQ(0x80, g_stub.last_sent.pdata[6]); // VSCP link protocol (TCP/IP)
  ASSERT_EQ(0x28, g_stub.last_sent.pdata[7]); // IPv4, two connections, no SSL
  ASSERT_EQ(0, std::memcmp(cfg.guid, g_stub.last_sent.pdata + 8, 16));
  ASSERT_EQ(0, std::memcmp(cfg.ipaddr, g_stub.last_sent.pdata + 24, 16));
  ASSERT_EQ(0, std::memcmp(cfg.deviceName, g_stub.last_sent.pdata + 40, 64));
}

// ---------------------------------------------------------------------------
// Null-pointer guard tests
// ---------------------------------------------------------------------------

TEST(_vscp_frmw2, work_null_ctx_returns_error)
{
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, vscp_frmw2_work(nullptr, nullptr));
}

TEST(_vscp_frmw2, handle_protocol_event_null_pex_returns_error)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, vscp_frmw2_handle_protocol_event(&cfg, nullptr));
}

TEST(_vscp_frmw2, handle_protocol_event_null_pdata_nonzero_size_returns_error)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = VSCP_CLASS1_PROTOCOL;
  ev.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
  ev.sizeData   = 1;
  ev.pdata      = nullptr; // non-zero sizeData but null pdata

  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, vscp_frmw2_handle_protocol_event(&cfg, &ev));
}

TEST(_vscp_frmw2, feed_level1_dm_null_pdata_nonzero_size_returns_error)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.sizeData = 3;
  ev.pdata    = nullptr;

  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, vscp_frmw2_feed_level1_dm(&cfg, &ev));
}

TEST(_vscp_frmw2, feed_level2_dm_null_pdata_nonzero_size_returns_error)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.sizeData = 3;
  ev.pdata    = nullptr;

  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, vscp_frmw2_feed_level2_dm(&cfg, &ev));
}

TEST(_vscp_frmw2, setup_event_null_pdata_does_not_crash)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.pdata = nullptr; // no buffer — setup_event should skip the memset

  vscp_frmw2_setup_event(&cfg, &ev, 0); // must not crash
  ASSERT_EQ(VSCP_PRIORITY_NORMAL | VSCP_HEADER16_FRAME_VERSION_UNIX_NS, ev.head);
}

// ---------------------------------------------------------------------------
// Decision-matrix zone/subzone filtering tests
// ---------------------------------------------------------------------------

// A raw 8-byte Level I DM row (VSCP spec layout):
//   byte 0: originating address (VSCP_LEVEL1_DM_POS_OADDR=0)
//   byte 1: flags        (VSCP_LEVEL1_DM_POS_FLAGS=1)
//   byte 2: class-mask   (VSCP_LEVEL1_DM_POS_CLASSMASK=2)
//   byte 3: class-filter  (VSCP_LEVEL1_DM_POS_CLASSFILTER=3)
//   byte 4: type-mask    (VSCP_LEVEL1_DM_POS_TYPEMASK=4)
//   byte 5: type-filter   (VSCP_LEVEL1_DM_POS_TYPEFILTER=5)
//   byte 6: action       (VSCP_LEVEL1_DM_POS_ACTION=6)
//   byte 7: action-param  (VSCP_LEVEL1_DM_POS_ACTIONPARAM=7)
struct Dm1Row {
  uint8_t bytes[8];
};

static Dm1Row make_dm1_row(uint8_t flags, uint8_t class_mask = 0xff,
                            uint8_t class_filter = 0, uint8_t type_mask = 0xff,
                            uint8_t type_filter = 0, uint8_t action = 0)
{
  Dm1Row r = {};
  r.bytes[1] = flags;        // FLAGS at pos 1
  r.bytes[2] = class_mask;   // CLASS_MASK at pos 2
  r.bytes[3] = class_filter; // CLASS_FILTER at pos 3
  r.bytes[4] = type_mask;    // TYPE_MASK at pos 4
  r.bytes[5] = type_filter;  // TYPE_FILTER at pos 5
  r.bytes[6] = action;       // ACTION at pos 6
  return r;
}

static void setup_dm1(vscp_frmw2_firmware_config_t& cfg, Dm1Row* rows, uint8_t n)
{
  cfg.pDm       = rows[0].bytes;
  cfg.nDmRows   = n;
  cfg.sizeDmRow = sizeof(Dm1Row);
}

TEST(_vscp_frmw2, feed_level1_dm_zone_wildcard_matches)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level = VSCP_LEVEL1;
  cfg.zone  = 0x05;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  Dm1Row row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED | VSCP_LEVEL1_DM_FLAG_CHECK_ZONE);
  setup_dm1(cfg, &row, 1);

  uint8_t buf[3] = { 0, 0xff, 0 }; // zone byte = 255 (wildcard)
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.sizeData = 3;
  ev.pdata    = buf;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls); // must match due to wildcard
}

TEST(_vscp_frmw2, feed_level1_dm_zone_mismatch_skips_row)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level = VSCP_LEVEL1;
  cfg.zone  = 0x05;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  Dm1Row row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED | VSCP_LEVEL1_DM_FLAG_CHECK_ZONE);
  setup_dm1(cfg, &row, 1);

  uint8_t buf[3] = { 0, 0x07, 0 }; // zone byte = 7, ctx zone = 5 → mismatch
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.sizeData = 3;
  ev.pdata    = buf;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(0, g_stub.dm_action_calls); // must skip
}

TEST(_vscp_frmw2, feed_level1_dm_subzone_mismatch_skips_row)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level   = VSCP_LEVEL1;
  cfg.subzone = 0x03;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  Dm1Row row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED | VSCP_LEVEL1_DM_FLAG_CHECK_SUBZONE);
  setup_dm1(cfg, &row, 1);

  uint8_t buf[3] = { 0, 0, 0x09 }; // subzone byte = 9, ctx subzone = 3 → mismatch
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.sizeData = 3;
  ev.pdata    = buf;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(0, g_stub.dm_action_calls);
}

TEST(_vscp_frmw2, feed_level1_dm_short_event_skips_zone_check)
{
  // Event with sizeData < 2 must not read pdata[1]; zone check is skipped
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level = VSCP_LEVEL1;
  cfg.zone  = 0x05;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  Dm1Row row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED | VSCP_LEVEL1_DM_FLAG_CHECK_ZONE);
  setup_dm1(cfg, &row, 1);

  uint8_t buf[1] = { 0 };
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.sizeData = 1; // too short to have zone byte
  ev.pdata    = buf;

  // Zone check is skipped when sizeData <= 1 — action should fire
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
}

// ---------------------------------------------------------------------------
// Protocol event routing tests
// ---------------------------------------------------------------------------

TEST(_vscp_frmw2, handle_protocol_event_probe_ack_on_nickname_match)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0x0042;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub(); // clear sends made by init
  cfg.state = FRMW2_STATE_ACTIVE;

  // NEW_NODE_ONLINE with 2-byte address [MSB, LSB] matching our nickname
  uint8_t buf[2] = { 0x00, 0x42 };
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = VSCP_CLASS1_PROTOCOL;
  ev.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
  ev.sizeData   = 2;
  ev.pdata      = buf;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_handle_protocol_event(&cfg, &ev));
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_TYPE_PROTOCOL_PROBE_ACK, g_stub.last_sent.vscp_type);
}

TEST(_vscp_frmw2, handle_protocol_event_probe_ack_on_nickname_mismatch_no_send)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0x0042;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub(); // clear sends made by init
  cfg.state = FRMW2_STATE_ACTIVE;

  uint8_t buf[1] = { 0x11 }; // different nickname
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = VSCP_CLASS1_PROTOCOL;
  ev.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
  ev.sizeData   = 1;
  ev.pdata      = buf;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_handle_protocol_event(&cfg, &ev));
  ASSERT_EQ(0, g_stub.send_calls); // must NOT send — not addressed to us
}

TEST(_vscp_frmw2, handle_protocol_event_read_register_sends_rw_response)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0x0007;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub(); // clear sends made by init
  cfg.state = FRMW2_STATE_ACTIVE;

  // READ_REGISTER: [nickname_lsb, reg] — use page 0 (user reg 0)
  // ADJSTDREG macro requires a pctx; use the raw offset (0) for Level I
  constexpr uint8_t kReg = VSCP_STD_REGISTER_MAJOR_VERSION; // offset 0x80
  uint8_t buf[2] = { 0x07, kReg };
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = VSCP_CLASS1_PROTOCOL;
  ev.vscp_type  = VSCP_TYPE_PROTOCOL_READ_REGISTER;
  ev.sizeData   = 2;
  ev.pdata      = buf;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_handle_protocol_event(&cfg, &ev));
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_TYPE_PROTOCOL_RW_RESPONSE, g_stub.last_sent.vscp_type);
  ASSERT_EQ(2u, g_stub.last_sent.sizeData);
  ASSERT_EQ(kReg, g_stub.last_sent.pdata[0]); // register echoed
  ASSERT_EQ(FRMW2_MAJOR_VERSION, g_stub.last_sent.pdata[1]); // register value
}

// ---------------------------------------------------------------------------
// Bounds checks on received protocol events
// ---------------------------------------------------------------------------

TEST(_vscp_frmw2, handle_protocol_event_probe_ack_on_8bit_nickname_match)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0x0042;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();
  cfg.state = FRMW2_STATE_ACTIVE;

  // One data byte (8-bit node id). Used to read a second byte past the data.
  uint8_t buf[1] = { 0x42 };
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = VSCP_CLASS1_PROTOCOL;
  ev.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
  ev.sizeData   = 1;
  ev.pdata      = buf;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_handle_protocol_event(&cfg, &ev));
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_TYPE_PROTOCOL_PROBE_ACK, g_stub.last_sent.vscp_type);
}

TEST(_vscp_frmw2, handle_protocol_event_extended_page_write_bad_size_refused)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0x0042;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();
  cfg.state = FRMW2_STATE_ACTIVE;

  // Too short made the byte count wrap, too long overran a 4 byte stack buffer
  for (uint16_t size : { 0, 1, 2, 3, 4, 9, 64, 512 }) {
    std::vector<uint8_t> buf(size ? size : 1, 0x42);
    vscp_event_t ev;
    std::memset(&ev, 0, sizeof(ev));
    ev.vscp_class = VSCP_CLASS1_PROTOCOL;
    ev.vscp_type  = VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE;
    ev.sizeData   = size;
    ev.pdata      = size ? buf.data() : nullptr;

    ASSERT_EQ(VSCP_ERROR_INVALID_FRAME, vscp_frmw2_handle_protocol_event(&cfg, &ev)) << size;
  }
}

// ---------------------------------------------------------------------------
// Level I compatibility fixes
// ---------------------------------------------------------------------------

namespace {

// Level I node that already has a nickname (goes ACTIVE in init)
void init_level1_active(vscp_frmw2_firmware_config_t& cfg, uint16_t nickname)
{
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = nickname;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  ASSERT_EQ(FRMW2_STATE_ACTIVE, cfg.state);
  reset_stub();
}

vscp_event_t make_event(uint16_t vscp_class, uint16_t vscp_type, uint8_t* pdata, uint16_t size, uint8_t from = 0)
{
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = vscp_class;
  ev.vscp_type  = vscp_type;
  ev.sizeData   = size;
  ev.pdata      = pdata;
  ev.GUID[15]   = from;
  return ev;
}

} // namespace

TEST(_vscp_frmw2, send_probe_carries_probe_nickname_8bit)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0xffff; // unassigned
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  cfg.probe_nickname = 0x17;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_send_probe(&cfg, OPT_PROBE));
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE, g_stub.last_sent.vscp_type);
  ASSERT_EQ(1u, g_stub.last_sent.sizeData);
  ASSERT_EQ(0x17, g_stub.last_sent.pdata[0]); // was 0xff (own nickname)
  ASSERT_EQ(0xff, g_stub.last_sent.GUID[15]); // sender is still "no nickname"
}

TEST(_vscp_frmw2, send_probe_carries_probe_nickname_16bit)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level             = VSCP_LEVEL1;
  cfg.bUse16BitNickname = 1;
  cfg.nickname          = 0xffff;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  cfg.probe_nickname = 0x1234;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_send_probe(&cfg, OPT_PROBE));
  ASSERT_EQ(2u, g_stub.last_sent.sizeData);
  ASSERT_EQ(0x12, g_stub.last_sent.pdata[0]);
  ASSERT_EQ(0x34, g_stub.last_sent.pdata[1]);
}

TEST(_vscp_frmw2, nickname_discovery_probes_then_assigns_free_nickname)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0xffff;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  ASSERT_EQ(FRMW2_STATE_PROBE, cfg.state);
  ASSERT_EQ(FRMW2_SUBSTATE_SEGCTRL_PROBE_WAITING, cfg.substate);
  reset_stub();

  // Nobody answers. Run long enough for the segment controller wait plus
  // VSCP_PROBE_TIMEOUT_COUNT + 1 probes for nickname 1.
  int probes_for_1 = 0;
  for (int i = 0; i < 2000 && FRMW2_STATE_ACTIVE != cfg.state; i++) {
    g_stub.now_ms += 10;
    int before = g_stub.send_calls;
    vscp_frmw2_work(&cfg, nullptr);
    if (g_stub.send_calls != before && FRMW2_STATE_PROBE == cfg.state &&
        VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE == g_stub.last_sent.vscp_type && 1 == g_stub.last_sent.sizeData) {
      ASSERT_EQ(1, g_stub.last_sent.pdata[0]);
      probes_for_1++;
    }
  }
  ASSERT_EQ(FRMW2_STATE_ACTIVE, cfg.state);
  ASSERT_EQ(1, cfg.nickname);
  ASSERT_EQ(VSCP_PROBE_TIMEOUT_COUNT + 1, probes_for_1);
  ASSERT_EQ(2, g_stub.stdreg_change_calls); // app told to persist the nickname (MSB + LSB)
}

TEST(_vscp_frmw2, nickname_discovery_error_when_all_nicknames_taken)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.level    = VSCP_LEVEL1;
  cfg.nickname = 0xffff;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  cfg.state          = FRMW2_STATE_PROBE;
  cfg.substate       = FRMW2_SUBSTATE_PROBE_WAITING;
  cfg.probe_nickname = 0xfe; // last usable 8-bit nickname

  vscp_event_t ack = make_event(VSCP_CLASS1_PROTOCOL, VSCP_TYPE_PROTOCOL_PROBE_ACK, nullptr, 0, 0xfe);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_nickname_discovery(&cfg, &ack));
  ASSERT_EQ(FRMW2_STATE_ERROR, cfg.state);
}

TEST(_vscp_frmw2, feed_level1_dm_reads_flags_from_byte_1)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x22);

  // Originating address 0x80 must not be mistaken for the "enabled" flag
  Dm1Row row = make_dm1_row(0 /* disabled */);
  row.bytes[0] = 0x80;
  setup_dm1(cfg, &row, 1);

  uint8_t buf[3] = { 0 };
  vscp_event_t ev = make_event(VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_TURNON, buf, 3, 0x10);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(0, g_stub.dm_action_calls);

  // Enabled in byte 1, originating address check against byte 0
  row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED | VSCP_LEVEL1_DM_FLAG_CHECK_OADDR, 0xff, VSCP_CLASS1_CONTROL, 0xff,
                     VSCP_TYPE_CONTROL_TURNON, 7);
  row.bytes[0] = 0x10;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
  ASSERT_EQ(7, g_stub.last_dm_action);

  ev.GUID[15] = 0x11; // other sender
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
}

TEST(_vscp_frmw2, feed_level1_dm_class_bit8_from_flags)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x22);

  // Match class 0x102 exactly: mask 0x1ff, filter 0x102
  Dm1Row row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED | VSCP_LEVEL1_DM_FLAG_CLASS_MASK |
                              VSCP_LEVEL1_DM_FLAG_CLASS_FILTER,
                            0xff, 0x02, 0x00, 0x00, 1);
  setup_dm1(cfg, &row, 1);

  vscp_event_t ev = make_event(0x102, 0, nullptr, 0);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);

  ev.vscp_class = 0x002; // differs only in bit 8
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level1_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
}

TEST(_vscp_frmw2, work_active_with_event_feeds_dm_and_runs_heartbeat)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x22);
  cfg.interval_heartbeat = 1000;
  cfg.last_heartbeat     = g_stub.now_ms;

  Dm1Row row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED, 0xff, VSCP_CLASS1_CONTROL, 0xff, VSCP_TYPE_CONTROL_TURNON, 3);
  setup_dm1(cfg, &row, 1);

  g_stub.now_ms += 1500; // heartbeat due
  uint8_t buf[3]  = { 0 };
  vscp_event_t ev = make_event(VSCP_CLASS1_CONTROL, VSCP_TYPE_CONTROL_TURNON, buf, 3, 0x10);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_work(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS1_INFORMATION, g_stub.last_sent.vscp_class);
  ASSERT_EQ(VSCP_TYPE_INFORMATION_NODE_HEARTBEAT, g_stub.last_sent.vscp_type);
}

TEST(_vscp_frmw2, write_reg_passes_selected_page)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x22);

  cfg.page_select = 0x00ff;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_write_reg(&cfg, 0x05, 0xAB));
  ASSERT_EQ(1, g_stub.write_reg_calls);
  ASSERT_EQ(0x00ff, g_stub.last_write_page); // was always 0
  ASSERT_EQ(0x05u, g_stub.last_write_reg);
  ASSERT_EQ(0xAB, g_stub.last_write_val);

  uint8_t val;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_read_reg(&cfg, 0x05, &val));
  ASSERT_EQ(0x00ff, g_stub.last_read_page);
}

TEST(_vscp_frmw2, page_select_registers_are_writable)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x22);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_write_reg(&cfg, VSCP_STD_REGISTER_PAGE_SELECT_MSB, 0x12));
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_write_reg(&cfg, VSCP_STD_REGISTER_PAGE_SELECT_LSB, 0x34));
  ASSERT_EQ(0x1234, cfg.page_select);

  uint8_t msb, lsb;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_read_reg(&cfg, VSCP_STD_REGISTER_PAGE_SELECT_MSB, &msb));
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_read_reg(&cfg, VSCP_STD_REGISTER_PAGE_SELECT_LSB, &lsb));
  ASSERT_EQ(0x12, msb);
  ASSERT_EQ(0x34, lsb);
}

TEST(_vscp_frmw2, read_standard_multibyte_registers_level1)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x22);
  const uint8_t uid[5] = { 1, 2, 3, 4, 5 };
  std::memcpy(cfg.userId, uid, 5);
  cfg.manufacturerId         = 0x11223344;
  cfg.manufacturerSubId      = 0x55667788;
  cfg.standard_device_family = 0x99AABBCC;
  cfg.standard_device_type   = 0xDDEEFF01;

  auto rd = [&](uint32_t reg) {
    uint8_t v = 0xEE;
    EXPECT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_read_reg(&cfg, reg, &v));
    return v;
  };
  for (int i = 0; i < 5; i++) {
    EXPECT_EQ(uid[i], rd(VSCP_STD_REGISTER_USER_ID + i)) << i;
  }
  const uint8_t man[4] = { 0x11, 0x22, 0x33, 0x44 }, sub[4] = { 0x55, 0x66, 0x77, 0x88 };
  const uint8_t fam[4] = { 0x99, 0xAA, 0xBB, 0xCC }, typ[4] = { 0xDD, 0xEE, 0xFF, 0x01 };
  for (int i = 0; i < 4; i++) {
    EXPECT_EQ(man[i], rd(VSCP_STD_REGISTER_USER_MANDEV_ID + i)) << i;
    EXPECT_EQ(sub[i], rd(VSCP_STD_REGISTER_USER_MANSUBDEV_ID + i)) << i;
    EXPECT_EQ(fam[i], rd(VSCP_STD_REGISTER_DEVICE_FAMILY + i)) << i;
    EXPECT_EQ(typ[i], rd(VSCP_STD_REGISTER_DEVICE_TYPE + i)) << i;
  }
  EXPECT_EQ(0, rd(0xB0)); // reserved range reads as zero
}

TEST(_vscp_frmw2, standard_registers_level2_addressing)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg); // Level II
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  cfg.manufacturerId = 0x11223344;
  reset_stub();

  uint8_t v = 0;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_read_reg(&cfg, 0xffffff00 + VSCP_STD_REGISTER_GUID + 3, &v));
  ASSERT_EQ(cfg.guid[3], v);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_read_reg(&cfg, 0xffffff00 + VSCP_STD_REGISTER_USER_MANDEV_ID + 1, &v));
  ASSERT_EQ(0x22, v);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_write_reg(&cfg, 0xffffff00 + VSCP_STD_REGISTER_USER_ID + 4, 0x5A));
  ASSERT_EQ(0x5A, cfg.userId[4]);
}

TEST(_vscp_frmw2, write_user_id_all_bytes)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x22);
  for (int i = 0; i < 5; i++) {
    ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_write_reg(&cfg, VSCP_STD_REGISTER_USER_ID + i, (uint8_t) (0x30 + i)));
    ASSERT_EQ(0x30 + i, cfg.userId[i]);
  }
  ASSERT_EQ(5, g_stub.stdreg_change_calls);
}

TEST(_vscp_frmw2, extended_page_write_for_other_node_ignored)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x42);

  uint8_t buf[6]  = { 0x43, 0x00, 0xff, 0x00, 0xAA, 0xBB }; // node 0x43, page 255
  vscp_event_t ev = make_event(VSCP_CLASS1_PROTOCOL, VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE, buf, 6);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_handle_protocol_event(&cfg, &ev));
  ASSERT_EQ(0, g_stub.write_reg_calls);
  ASSERT_EQ(0, g_stub.send_calls);
}

TEST(_vscp_frmw2, extended_page_write_to_us_uses_page_and_responds)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x42);
  g_stub.ts_step = 10; // the response path busy-waits 100 us on get_timestamp()

  uint8_t buf[6]  = { 0x42, 0x00, 0xff, 0x03, 0xAA, 0xBB };
  vscp_event_t ev = make_event(VSCP_CLASS1_PROTOCOL, VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_WRITE, buf, 6);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_handle_protocol_event(&cfg, &ev));
  ASSERT_EQ(2, g_stub.write_reg_calls);
  ASSERT_EQ(0x00ff, g_stub.last_write_page);
  ASSERT_EQ(0x04u, g_stub.last_write_reg);
  ASSERT_EQ(0xBB, g_stub.last_write_val);
  ASSERT_EQ(0, cfg.page_select); // restored
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_TYPE_PROTOCOL_EXTENDED_PAGE_RESPONSE, g_stub.last_sent.vscp_type);
}

TEST(_vscp_frmw2, extended_page_read_wait_survives_timestamp_wrap)
{
  vscp_frmw2_firmware_config_t cfg;
  init_level1_active(cfg, 0x42);

  // A 32-bit microsecond counter 40 us before it wraps
  g_stub.ts_wrap32 = true;
  g_stub.ts_step   = 7;
  g_stub.now_ts    = 0xFFFFFFFFull - 40;

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_extended_page_read(&cfg, 0x42, 0, 0, 8));
  ASSERT_FALSE(g_stub.ts_runaway);
  ASSERT_EQ(2, g_stub.send_calls);
}

// ---------------------------------------------------------------------------
// Level II decision matrix rows
//   bytes 0-3   control (MSB first)
//   bytes 4-5   class
//   bytes 6-7   type
//   bytes 8-23  originating GUID
//   bytes 24-25 action
//   bytes 26-31 action parameter
// ---------------------------------------------------------------------------

struct Dm2Row {
  uint8_t bytes[32];
};

static Dm2Row make_dm2_row(uint32_t flags, uint16_t vscp_class, uint16_t vscp_type, uint8_t action)
{
  Dm2Row r   = {};
  r.bytes[0] = static_cast<uint8_t>(flags >> 24);
  r.bytes[1] = static_cast<uint8_t>(flags >> 16);
  r.bytes[2] = static_cast<uint8_t>(flags >> 8);
  r.bytes[3] = static_cast<uint8_t>(flags);
  r.bytes[4] = static_cast<uint8_t>(vscp_class >> 8);
  r.bytes[5] = static_cast<uint8_t>(vscp_class);
  r.bytes[6] = static_cast<uint8_t>(vscp_type >> 8);
  r.bytes[7] = static_cast<uint8_t>(vscp_type);
  r.bytes[25] = action;
  return r;
}

static void setup_dm2(vscp_frmw2_firmware_config_t& cfg, Dm2Row* rows, uint8_t n)
{
  cfg.pDm       = rows[0].bytes;
  cfg.nDmRows   = n;
  cfg.sizeDmRow = sizeof(Dm2Row);
}

TEST(_vscp_frmw2, feed_level2_dm_matches_class_and_type_exactly)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  Dm2Row row = make_dm2_row(VSCP_LEVEL2_DM_FLAG_ENABLED, 30, 5, 7);
  setup_dm2(cfg, &row, 1);

  uint8_t buf[3] = { 0, 0, 0 };
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.sizeData   = 3;
  ev.pdata      = buf;
  ev.vscp_class = 30;
  ev.vscp_type  = 6; // Other type
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(0, g_stub.dm_action_calls);

  ev.vscp_class = 20; // Other class
  ev.vscp_type  = 5;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(0, g_stub.dm_action_calls);

  ev.vscp_class = 30;
  ev.vscp_type  = 5;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
  ASSERT_EQ(7, g_stub.last_dm_action);
}

TEST(_vscp_frmw2, feed_level2_dm_flags_are_read_from_byte_0)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  // Enabled bit only in byte 0: the row must trigger
  Dm2Row row = make_dm2_row(VSCP_LEVEL2_DM_FLAG_ENABLED, 30, 5, 2);
  setup_dm2(cfg, &row, 1);
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = 30;
  ev.vscp_type  = 5;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);

  // Disabled row (0x80 only in byte 1, as the old macro read it) must not trigger
  reset_stub();
  Dm2Row off = make_dm2_row(0x00800000, 30, 5, 2);
  setup_dm2(cfg, &off, 1);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(0, g_stub.dm_action_calls);
}

TEST(_vscp_frmw2, feed_level2_dm_origin_check_uses_level2_flag)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  Dm2Row row = make_dm2_row(VSCP_LEVEL2_DM_FLAG_ENABLED | VSCP_LEVEL2_DM_FLAG_CHECK_OADDR, 30, 5, 3);
  for (int i = 0; i < 16; i++) {
    row.bytes[8 + i] = static_cast<uint8_t>(0x10 + i);
  }
  setup_dm2(cfg, &row, 1);

  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = 30;
  ev.vscp_type  = 5;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev)); // GUID all zero
  ASSERT_EQ(0, g_stub.dm_action_calls);

  for (int i = 0; i < 16; i++) {
    ev.GUID[i] = static_cast<uint8_t>(0x10 + i);
  }
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
}

TEST(_vscp_frmw2, feed_level2_dm_index_is_data_byte_0)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.index = 3;
  cfg.zone  = 9;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  Dm2Row row = make_dm2_row(VSCP_LEVEL2_DM_FLAG_ENABLED | VSCP_LEVEL2_DM_FLAG_CHECK_INDEX, 20, 3, 4);
  setup_dm2(cfg, &row, 1);

  uint8_t buf[3] = { 3, 0, 0 }; // index 3 in byte 0
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = 20;
  ev.vscp_type  = 3;
  ev.sizeData   = 3;
  ev.pdata      = buf;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);

  buf[0] = 4; // Other index
  buf[1] = 3; // Byte 1 holding the index value must not matter
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_level2_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
}

TEST(_vscp_frmw2, feed_level2_dm_null_event_returns_error)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  ASSERT_EQ(VSCP_ERROR_INVALID_POINTER, vscp_frmw2_feed_level2_dm(&cfg, nullptr));
}

TEST(_vscp_frmw2, feed_dm_level2_device_with_level1_rows)
{
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg); // Level II device
  cfg.bDmLevel1Rows = 1;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));
  reset_stub();

  // Level I row: CLASS1.CONTROL (30), TURN-ON (5) -> action 2
  Dm1Row row = make_dm1_row(VSCP_LEVEL1_DM_FLAG_ENABLED, 0xff, 30, 0xff, 5, 2);
  setup_dm1(cfg, &row, 1);

  uint8_t buf[3] = { 0, 0, 0 };
  vscp_event_t ev;
  std::memset(&ev, 0, sizeof(ev));
  ev.vscp_class = 30;
  ev.vscp_type  = 5;
  ev.sizeData   = 3;
  ev.pdata      = buf;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
  ASSERT_EQ(2, g_stub.last_dm_action);

  ev.vscp_type = 6;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_feed_dm(&cfg, &ev));
  ASSERT_EQ(1, g_stub.dm_action_calls);
}
