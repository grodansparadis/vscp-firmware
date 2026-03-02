#include <cstring>
#include <gtest/gtest.h>

extern "C" {
#include <vscp-class.h>
#include <vscp-type.h>

typedef struct {
  uint8_t flags;
  uint8_t priority;
  uint16_t vscp_class;
  uint8_t vscp_type;
  uint8_t oaddr;
  uint8_t data[8];
} vscpevent_t;

#define VSCP_ADDRESS_FREE 0xff

#define VSCP_STATE_STARTUP 0x00
#define VSCP_SUBSTATE_NONE 0x00

#define VSCP_LED_OFF    0x00
#define VSCP_LED_ON     0x01
#define VSCP_LED_BLINK1 0x02

#define VSCP_VALID_MSG 0x80

#define VSCP_PRIORITY_HIGH   0x00
#define VSCP_PRIORITY_NORMAL 0x03
#define VSCP_PRIORITY_LOW    0x07

#define VSCP_INITIALIZED_BYTE0_INDEX 0
#define VSCP_INITIALIZED_BYTE1_INDEX 1
#define VSCP_INITIALIZED_BYTE0_VALUE 0x55
#define VSCP_INITIALIZED_BYTE1_VALUE 0xAA

#define VSCP_BOOTLOADER_NONE 0xff

extern uint8_t vscp_nickname;
extern uint8_t vscp_errorcnt;
extern uint8_t vscp_node_state;
extern uint8_t vscp_node_substate;
extern uint8_t vscp_initledfunc;
extern uint16_t vscp_page_select;
extern vscpevent_t vscp_imsg;
extern vscpevent_t vscp_omsg;

void vscp_init(void);
int8_t vscp_check_pstorage(void);
void vscp_sendHeartBeat(uint8_t zone, uint8_t subzone);
void vscp_handleProtocolEvent(void);
int8_t vscp_sendEvent(void);
}

namespace {

struct FirmwareStubState {
  uint8_t nickname_permanent;
  uint8_t control[2];
  uint8_t guid[16];
  uint8_t user_id[5];
  uint8_t manufacturer[8];
  uint8_t app_regs[128];
  uint8_t mdf_url[32];

  int init_pstorage_calls;
  int restore_defaults_calls;
  int send_calls;

  uint16_t last_class;
  uint8_t last_type;
  uint8_t last_nodeid;
  uint8_t last_priority;
  uint8_t last_size;
  uint8_t last_data[8];

  int send_return_value;
  int get_return_value;
} g_stub;

void reset_stub()
{
  std::memset(&g_stub, 0, sizeof(g_stub));
  g_stub.nickname_permanent = VSCP_ADDRESS_FREE;
  g_stub.send_return_value  = 1;

  for (int i = 0; i < 16; ++i) {
    g_stub.guid[i] = static_cast<uint8_t>(0x10 + i);
  }
}

} // namespace

extern "C" {

int8_t getVSCPFrame(uint16_t* pvscpclass,
                    uint8_t* pvscptype,
                    uint8_t* pNodeId,
                    uint8_t* pPriority,
                    uint8_t* pSize,
                    uint8_t* pData)
{
  (void)pvscpclass;
  (void)pvscptype;
  (void)pNodeId;
  (void)pPriority;
  (void)pSize;
  (void)pData;
  return static_cast<int8_t>(g_stub.get_return_value);
}

int8_t sendVSCPFrame(uint16_t vscpclass,
                     uint8_t vscptype,
                     uint8_t nodeid,
                     uint8_t priority,
                     uint8_t size,
                     uint8_t* pData)
{
  g_stub.send_calls++;
  g_stub.last_class    = vscpclass;
  g_stub.last_type     = vscptype;
  g_stub.last_nodeid   = nodeid;
  g_stub.last_priority = priority;
  g_stub.last_size     = size;
  std::memcpy(g_stub.last_data, pData, sizeof(g_stub.last_data));
  return static_cast<int8_t>(g_stub.send_return_value);
}

uint8_t vscp_getMajorVersion(void) { return 1; }
uint8_t vscp_getMinorVersion(void) { return 0; }
uint8_t vscp_getSubMinorVersion(void) { return 0; }

uint8_t vscp_getGUID(uint8_t idx) { return g_stub.guid[idx & 0x0f]; }
void vscp_setGUID(uint8_t idx, uint8_t data) { g_stub.guid[idx & 0x0f] = data; }

uint8_t vscp_getUserID(uint8_t idx) { return g_stub.user_id[idx % 5]; }
void vscp_setUserID(uint8_t idx, uint8_t data) { g_stub.user_id[idx % 5] = data; }

uint8_t vscp_getManufacturerId(uint8_t idx) { return g_stub.manufacturer[idx & 0x07]; }
void vscp_setManufacturerId(uint8_t idx, uint8_t data) { g_stub.manufacturer[idx & 0x07] = data; }

uint8_t vscp_getBootLoaderAlgorithm(void) { return VSCP_BOOTLOADER_NONE; }
uint8_t vscp_getBufferSize(void) { return 8; }
uint8_t vscp_getRegisterPagesUsed(void) { return 1; }
uint8_t vscp_getMDF_URL(uint8_t idx) { return g_stub.mdf_url[idx & 0x1f]; }

uint8_t vscp_readNicknamePermanent(void) { return g_stub.nickname_permanent; }
void vscp_writeNicknamePermanent(uint8_t nickname) { g_stub.nickname_permanent = nickname; }

uint8_t vscp_getControlByte(uint8_t idx) { return g_stub.control[idx & 0x01]; }
void vscp_setControlByte(uint8_t idx, uint8_t ctrl) { g_stub.control[idx & 0x01] = ctrl; }

void vscp_init_pstorage(void) { g_stub.init_pstorage_calls++; }

uint8_t vscp_getPageSelect(uint8_t idx)
{
  return (idx == 0) ? static_cast<uint8_t>((vscp_page_select >> 8) & 0xff)
                    : static_cast<uint8_t>(vscp_page_select & 0xff);
}

void vscp_setPageSelect(uint8_t idx, uint8_t data)
{
  if (0 == idx) {
    vscp_page_select = static_cast<uint16_t>((vscp_page_select & 0x00ff) | (static_cast<uint16_t>(data) << 8));
  }
  else {
    vscp_page_select = static_cast<uint16_t>((vscp_page_select & 0xff00) | data);
  }
}

uint8_t vscp_readAppReg(uint8_t reg) { return g_stub.app_regs[reg & 0x7f]; }
uint8_t vscp_writeAppReg(uint8_t reg, uint8_t value)
{
  g_stub.app_regs[reg & 0x7f] = value;
  return value;
}

void vscp_getMatrixInfo(char* pData)
{
  std::memset(pData, 0, 7);
}

void vscp_getEmbeddedMdfInfo(void)
{
}

void vscp_goBootloaderMode(uint8_t algorithm)
{
  (void)algorithm;
}

uint8_t vscp_getZone(void) { return 3; }
uint8_t vscp_getSubzone(void) { return 7; }
uint32_t vscp_getFamilyCode(void) { return 0x11223344; }
uint32_t vscp_getFamilyType(void) { return 0x55667788; }
uint16_t vscp_getFirmwareCode(void) { return 0x1234; }

void vscp_restoreDefaults(void)
{
  g_stub.restore_defaults_calls++;
}

} // extern "C"

TEST(_vscp_firmware, init_uses_free_nickname_when_persistent_zero)
{
  reset_stub();
  g_stub.nickname_permanent = 0;

  vscp_init();

  ASSERT_EQ(VSCP_ADDRESS_FREE, vscp_nickname);
  ASSERT_EQ(VSCP_LED_BLINK1, vscp_initledfunc);
  ASSERT_EQ(VSCP_STATE_STARTUP, vscp_node_state);
  ASSERT_EQ(VSCP_SUBSTATE_NONE, vscp_node_substate);
}

TEST(_vscp_firmware, check_pstorage_returns_true_when_initialized)
{
  reset_stub();
  g_stub.control[0] = VSCP_INITIALIZED_BYTE0_VALUE;
  g_stub.control[1] = VSCP_INITIALIZED_BYTE1_VALUE;

  ASSERT_EQ(1, vscp_check_pstorage());
  ASSERT_EQ(0, g_stub.init_pstorage_calls);
}

TEST(_vscp_firmware, check_pstorage_initializes_when_invalid)
{
  reset_stub();
  g_stub.control[0] = 0x00;
  g_stub.control[1] = 0x00;

  ASSERT_EQ(0, vscp_check_pstorage());
  ASSERT_EQ(1, g_stub.init_pstorage_calls);
  ASSERT_EQ(VSCP_ADDRESS_FREE, g_stub.nickname_permanent);
  ASSERT_EQ(VSCP_INITIALIZED_BYTE0_VALUE, g_stub.control[0]);
  ASSERT_EQ(VSCP_INITIALIZED_BYTE1_VALUE, g_stub.control[1]);
}

TEST(_vscp_firmware, send_heartbeat_populates_information_event)
{
  reset_stub();
  vscp_nickname = 0x22;

  vscp_sendHeartBeat(5, 9);

  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS1_INFORMATION, g_stub.last_class);
  ASSERT_EQ(VSCP_TYPE_INFORMATION_NODE_HEARTBEAT, g_stub.last_type);
  ASSERT_EQ(0x22, g_stub.last_nodeid);
  ASSERT_EQ(3, g_stub.last_size);
  ASSERT_EQ(0, g_stub.last_data[0]);
  ASSERT_EQ(5, g_stub.last_data[1]);
  ASSERT_EQ(9, g_stub.last_data[2]);
}

TEST(_vscp_firmware, handle_set_nickname_updates_and_acks)
{
  reset_stub();
  vscp_nickname = 0x11;

  vscp_imsg.flags      = VSCP_VALID_MSG | 2;
  vscp_imsg.vscp_class = VSCP_CLASS1_PROTOCOL;
  vscp_imsg.vscp_type  = VSCP_TYPE_PROTOCOL_SET_NICKNAME;
  vscp_imsg.data[0]    = 0x11;
  vscp_imsg.data[1]    = 0x44;

  vscp_handleProtocolEvent();

  ASSERT_EQ(0x44, vscp_nickname);
  ASSERT_EQ(0x44, g_stub.nickname_permanent);
  ASSERT_EQ(1, g_stub.send_calls);
  ASSERT_EQ(VSCP_CLASS1_PROTOCOL, g_stub.last_class);
  ASSERT_EQ(VSCP_TYPE_PROTOCOL_NICKNAME_ACCEPTED, g_stub.last_type);
  ASSERT_EQ(1, g_stub.last_size);
  ASSERT_EQ(0x44, g_stub.last_data[0]);
}

TEST(_vscp_firmware, send_event_failure_increments_error_counter)
{
  reset_stub();
  vscp_nickname = 0x33;
  vscp_errorcnt = 0;

  vscp_omsg.flags      = VSCP_VALID_MSG | 1;
  vscp_omsg.priority   = VSCP_PRIORITY_NORMAL;
  vscp_omsg.vscp_class = VSCP_CLASS1_PROTOCOL;
  vscp_omsg.vscp_type  = VSCP_TYPE_PROTOCOL_NEW_NODE_ONLINE;
  vscp_omsg.data[0]    = 0x33;

  g_stub.send_return_value = 0;

  ASSERT_EQ(0, vscp_sendEvent());
  ASSERT_EQ(1, vscp_errorcnt);
}
