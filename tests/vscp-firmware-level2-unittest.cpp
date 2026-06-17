#include <cstring>
#include <gtest/gtest.h>

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
  return g_stub.now_ts;
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
                          uint8_t /*action*/,
                          const uint8_t* const /*pparam*/)
{
  g_stub.dm_action_calls++;
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
                         uint16_t /*page*/,
                         uint32_t /*reg*/,
                         uint8_t* pval)
{
  g_stub.read_reg_calls++;
  *pval = 0;
  return VSCP_ERROR_SUCCESS;
}

static int stub_write_reg(vscp_frmw2_firmware_config_t* /*pcfg*/,
                          uint16_t /*page*/,
                          uint32_t /*reg*/,
                          uint8_t /*val*/)
{
  g_stub.write_reg_calls++;
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
  vscp_frmw2_setup_event(&cfg, &ex);

  ASSERT_EQ(VSCP_PRIORITY_NORMAL, ex.head);
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

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_send_caps(&cfg));

  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS2_PROTOCOL, g_stub.last_sent.vscp_class);
  ASSERT_EQ(VSCP2_TYPE_PROTOCOL_HIGH_END_SERVER_CAPS, g_stub.last_sent.vscp_type);
  ASSERT_EQ(104, g_stub.last_sent.sizeData);
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

  vscp_frmw2_setup_event(&cfg, &ev); // must not crash
  ASSERT_EQ(VSCP_PRIORITY_NORMAL, ev.head);
}

// ---------------------------------------------------------------------------
// Decision-matrix zone/subzone filtering tests
// ---------------------------------------------------------------------------

// A raw 8-byte Level I DM row laid out as the macros actually read it:
//   byte 0: FLAGS   (LEVEL1_DM_FLAGS uses VSCP_LEVEL2_DM_POS_FLAGS=0)
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
  r.bytes[0] = flags;        // FLAGS at pos 0
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
