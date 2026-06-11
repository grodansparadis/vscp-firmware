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
  vscp_event_ex_t last_sent;
} g_stub;

void reset_stub()
{
  std::memset(&g_stub, 0, sizeof(g_stub));
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

static int stub_send_event_ex(vscp_frmw2_firmware_config_t* /*pcfg*/, vscpEventEx* pex)
{
  g_stub.send_calls++;
  std::memcpy(&g_stub.last_sent, pex, sizeof(vscpEventEx));
  return VSCP_ERROR_SUCCESS;
}

static int stub_dm_action(vscp_frmw2_firmware_config_t* /*pcfg*/,
                          const vscpEventEx* const /*pex*/,
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

static int stub_set_event_time(vscp_frmw2_firmware_config_t* /*pcfg*/, vscpEventEx* pex)
{
  g_stub.set_time_calls++;
  pex->year      = 2026;
  pex->month     = 3;
  pex->day       = 2;
  pex->hour      = 12;
  pex->minute    = 0;
  pex->second    = 0;
  pex->timestamp = g_stub.now_ms;
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
  stub_send_event_ex,
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
  cfg.m_level                          = VSCP_LEVEL2;
  cfg.m_probe_timeout                  = VSCP_FRMW2_UNASSIGNED;
  cfg.m_probe_timeout_count            = VSCP_FRMW2_UNASSIGNED;
  cfg.m_nickname                       = 0x0022;
  cfg.m_bInterestedInAllEvents         = 1;
  cfg.m_bUse16BitNickname              = 0;
  cfg.m_bEnableWriteProtectedLocations = 1;
  cfg.ops                              = &g_ops;

  for (int i = 0; i < 16; ++i) {
    cfg.m_guid[i]   = static_cast<uint8_t>(0xA0 + i);
    cfg.m_ipaddr[i] = static_cast<uint8_t>(i);
  }

  const char* name = "UnitTestNode";
  std::memset(cfg.m_deviceName, 0, sizeof(cfg.m_deviceName));
  std::memcpy(cfg.m_deviceName, name, std::strlen(name));
}

} // namespace

TEST(_vscp_frmw2, init_level2_sets_active_state)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.m_level = VSCP_LEVEL2;

  const int rv = vscp_frmw2_init(&cfg);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, rv);
  ASSERT_EQ(FRMW2_STATE_ACTIVE, cfg.m_state);
  ASSERT_EQ(0u, cfg.m_alarm_status);
  ASSERT_EQ(0u, cfg.m_errorCounter);
  ASSERT_GE(g_stub.set_time_calls, 1);
}

TEST(_vscp_frmw2, setup_event_ex_level1_uses_nickname_in_guid)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  cfg.m_level    = VSCP_LEVEL1;
  cfg.m_nickname = 0x1234;

  vscp_event_ex_t ex;
  std::memset(&ex, 0, sizeof(ex));
  vscp_frmw2_setup_event_ex(&cfg, &ex);

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
  cfg.m_level = VSCP_LEVEL2;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_send_heartbeat(&cfg));
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS2_INFORMATION, g_stub.last_sent.vscp_class);
  ASSERT_EQ(VSCP2_TYPE_INFORMATION_HEART_BEAT, g_stub.last_sent.vscp_type);
  ASSERT_EQ(std::strlen(reinterpret_cast<const char*>(cfg.m_deviceName)), g_stub.last_sent.sizeData);
  ASSERT_EQ(0, std::memcmp(cfg.m_deviceName, g_stub.last_sent.data, g_stub.last_sent.sizeData));
}

TEST(_vscp_frmw2, send_caps_contains_guid_ip_and_name)
{
  reset_stub();
  vscp_frmw2_firmware_config_t cfg;
  init_config(cfg);
  cfg.m_level = VSCP_LEVEL2;
  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_init(&cfg));

  ASSERT_EQ(VSCP_ERROR_SUCCESS, vscp_frmw2_send_caps(&cfg));

  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS2_PROTOCOL, g_stub.last_sent.vscp_class);
  ASSERT_EQ(VSCP2_TYPE_PROTOCOL_HIGH_END_SERVER_CAPS, g_stub.last_sent.vscp_type);
  ASSERT_EQ(104, g_stub.last_sent.sizeData);
  ASSERT_EQ(0, std::memcmp(cfg.m_guid, g_stub.last_sent.data + 8, 16));
  ASSERT_EQ(0, std::memcmp(cfg.m_ipaddr, g_stub.last_sent.data + 24, 16));
  ASSERT_EQ(0, std::memcmp(cfg.m_deviceName, g_stub.last_sent.data + 40, 64));
}
