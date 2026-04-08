#include <cstring>
#include <gtest/gtest.h>

extern "C" {
#include <crc.h>
#include <vscp.h>
#include <vscp-class.h>
#include <vscp-type.h>
#include <vscp-bootloader.h>
}

extern "C" crc crcTable[256];

namespace {

struct BootloaderStubState {
  int init_hw_calls;
  int release_hw_calls;
  int go_app_calls;
  int send_calls;
  int get_event_calls;
  int run_budget;
  uint8_t bootflag;
  vscpboot_config_t cfg;
  uint8_t guid[16];
  vscp_event_ex_t last_sent;
} g_stub;

void reset_stub()
{
  std::memset(&g_stub, 0, sizeof(g_stub));
  g_stub.cfg.vscpLevel  = 0;
  g_stub.cfg.blockSize  = 64;
  g_stub.cfg.blockCount = 10;
}

} // namespace

extern "C" {

void vscpboot_goApplication(void)
{
  g_stub.go_app_calls++;
}

int vscpboot_run(void)
{
  if (g_stub.run_budget > 0) {
    g_stub.run_budget--;
    return 1;
  }

  return 0;
}

int vscpboot_init_hardware(void)
{
  g_stub.init_hw_calls++;
  return VSCP_ERROR_SUCCESS;
}

int vscpboot_release_hardware(void)
{
  g_stub.release_hw_calls++;
  return VSCP_ERROR_SUCCESS;
}

uint8_t vscpboot_getBootFlag(void)
{
  return g_stub.bootflag;
}

int vscpboot_setBootFlag(uint8_t bootflag)
{
  g_stub.bootflag = bootflag;
  return VSCP_ERROR_SUCCESS;
}

void vscpboot_reboot(void)
{
}

vscpboot_config_t* vscpboot_getConfig(void)
{
  return &g_stub.cfg;
}

uint8_t* vscpboot_getGUID(void)
{
  return g_stub.guid;
}

int vscpboot_isMemTypeValid(uint8_t type)
{
  return (0 == type) ? 1 : 0;
}

int vscpboot_isMemBankValid(uint8_t bank)
{
  return (0 == bank) ? 1 : 0;
}

int vscpboot_programBlock(const uint8_t* pblock, uint8_t type, uint8_t bank)
{
  (void)pblock;
  (void)type;
  (void)bank;
  return VSCP_ERROR_SUCCESS;
}

crc vscpboot_calcPrgCrc(void)
{
  return 0;
}

int vscpboot_sendEventEx(vscpEventEx* pex)
{
  g_stub.send_calls++;
  std::memcpy(&g_stub.last_sent, pex, sizeof(vscpEventEx));
  return VSCP_ERROR_SUCCESS;
}

int vscpboot_getEventEx(vscpEventEx* pex)
{
  (void)pex;
  g_stub.get_event_calls++;
  return VSCP_ERROR_FIFO_EMPTY;
}

} // extern "C"

TEST(_vscp_bootloader, init_prepares_crc_for_fast_algorithm)
{
  std::memset(crcTable, 0, sizeof(crcTable));

  const unsigned char msg[] = "123456789";

  vscpboot_init();

  ASSERT_EQ(static_cast<crc>(CHECK_VALUE), crcFast(msg, 9));
}

TEST(_vscp_bootloader, loader_sends_boot_ack_on_entry)
{
  reset_stub();
  g_stub.bootflag  = VSCPBOOT_FLAG_BOOT;
  g_stub.run_budget = 0;

  vscpboot_loader();

  ASSERT_EQ(1, g_stub.init_hw_calls);
  ASSERT_EQ(1, g_stub.release_hw_calls);
  ASSERT_EQ(0, g_stub.go_app_calls);

  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS1_PROTOCOL, g_stub.last_sent.vscp_class);
  ASSERT_EQ(VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER, g_stub.last_sent.vscp_type);
  ASSERT_EQ(8, g_stub.last_sent.sizeData);

  ASSERT_EQ((g_stub.cfg.blockSize >> 24) & 0xFF, g_stub.last_sent.data[0]);
  ASSERT_EQ((g_stub.cfg.blockSize >> 16) & 0xFF, g_stub.last_sent.data[1]);
  ASSERT_EQ((g_stub.cfg.blockSize >> 8) & 0xFF, g_stub.last_sent.data[2]);
  ASSERT_EQ(g_stub.cfg.blockSize & 0xFF, g_stub.last_sent.data[3]);

  ASSERT_EQ((g_stub.cfg.blockCount >> 24) & 0xFF, g_stub.last_sent.data[4]);
  ASSERT_EQ((g_stub.cfg.blockCount >> 16) & 0xFF, g_stub.last_sent.data[5]);
  ASSERT_EQ((g_stub.cfg.blockCount >> 8) & 0xFF, g_stub.last_sent.data[6]);
  ASSERT_EQ(g_stub.cfg.blockCount & 0xFF, g_stub.last_sent.data[7]);
}

TEST(_vscp_bootloader, loader_checks_application_path_when_bootflag_is_clear)
{
  reset_stub();
  g_stub.bootflag   = 0;
  g_stub.run_budget = 0;

  vscpboot_loader();

  ASSERT_EQ(1, g_stub.go_app_calls);
  ASSERT_GE(g_stub.release_hw_calls, 1);
  ASSERT_EQ(1, g_stub.send_calls);
}
