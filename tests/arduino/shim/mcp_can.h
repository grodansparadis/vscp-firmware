// Host stand-in for the coryjfowler mcp_can library: scripted receive queue,
// logged sends
#pragma once
#include <stdint.h>
#include <deque>
#include <vector>

#define CAN_OK       0
#define CAN_MSGAVAIL 3
#define CAN_NOMSG    4
#define MCP_ANY      0
#define CAN_125KBPS  10
#define MCP_8MHZ     2
#define MCP_16MHZ    1
#define MCP_20MHZ    0
#define MCP_NORMAL   0

struct SimFrame {
  uint32_t id; // 29-bit
  bool ext;
  uint8_t len;
  uint8_t data[8];
};

extern std::deque<SimFrame> sim_rx;
extern std::vector<SimFrame> sim_tx;

class MCP_CAN {
public:
  MCP_CAN(uint8_t) {}
  uint8_t begin(uint8_t, uint8_t, uint8_t) { return CAN_OK; }
  uint8_t setMode(uint8_t) { return CAN_OK; }
  uint8_t sendMsgBuf(unsigned long id, uint8_t ext, uint8_t len, uint8_t *buf)
  {
    SimFrame f{};
    f.id  = id;
    f.ext = ext;
    f.len = len;
    for (int i = 0; i < len; i++) f.data[i] = buf[i];
    sim_tx.push_back(f);
    return CAN_OK;
  }
  uint8_t checkReceive() { return sim_rx.empty() ? CAN_NOMSG : CAN_MSGAVAIL; }
  uint8_t readMsgBuf(unsigned long *id, uint8_t *len, uint8_t *buf)
  {
    SimFrame f = sim_rx.front();
    sim_rx.pop_front();
    *id  = f.id | (f.ext ? 0x80000000UL : 0); // extended flag as mcp_can reports it
    *len = f.len;
    for (int i = 0; i < f.len; i++) buf[i] = f.data[i];
    return CAN_OK;
  }
};
