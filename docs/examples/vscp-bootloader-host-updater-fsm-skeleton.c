/*
  VSCP Bootloader Host Updater (Explicit FSM Skeleton)

  Purpose:
  - Demonstrate host update flow with explicit finite-state transitions.
  - Improve traceability for logs, retries, and error handling.

  State Flow:
  WAIT_BOOT_ACK -> START_BLOCK -> SEND_BLOCK_DATA -> PROGRAM_BLOCK -> NEXT_BLOCK
                                  ^                                         |
                                  |-----------------------------------------|
  NEXT_BLOCK -> ACTIVATE_IMAGE -> DONE
  Any stage can transition to FAIL on protocol errors or exhausted retries.

  This is documentation example code (not compiled by project build files).
*/

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Simplified protocol/event model for skeleton readability
// -----------------------------------------------------------------------------

enum {
  T_ACK_BOOT_LOADER = 1,
  T_START_BLOCK = 2,
  T_START_BLOCK_ACK = 3,
  T_START_BLOCK_NACK = 4,
  T_BLOCK_DATA = 5,
  T_BLOCK_DATA_ACK = 6,
  T_BLOCK_DATA_NACK = 7,
  T_PROGRAM_BLOCK = 8,
  T_PROGRAM_BLOCK_ACK = 9,
  T_PROGRAM_BLOCK_NACK = 10,
  T_ACTIVATE = 11,
  T_ACTIVATE_ACK = 12,
  T_ACTIVATE_NACK = 13,
};

enum {
  U_OK = 0,
  U_TIMEOUT = -1,
  U_PROTOCOL = -2,
  U_RETRY_EXHAUSTED = -3,
  U_CRC_FAIL = -4,
  U_BAD_ARGS = -5,
};

typedef struct host_event_ex {
  uint16_t vscp_type;
  uint16_t sizeData;
  uint8_t data[512];
} host_event_ex_t;

typedef enum updater_state {
  ST_WAIT_BOOT_ACK = 0,
  ST_START_BLOCK,
  ST_SEND_BLOCK_DATA,
  ST_PROGRAM_BLOCK,
  ST_NEXT_BLOCK,
  ST_ACTIVATE_IMAGE,
  ST_DONE,
  ST_FAIL,
} updater_state_t;

typedef struct updater_fsm_ctx {
  const uint8_t *image;
  size_t image_len;

  uint32_t block_size;
  uint32_t block_count;
  uint32_t current_block;
  uint32_t total_blocks;

  uint8_t memory_type;
  uint8_t memory_bank;

  uint32_t timeout_ms;
  uint8_t msg_retries;
  uint8_t block_retries;

  int last_error;
} updater_fsm_ctx_t;

// -----------------------------------------------------------------------------
// Hooks to implement in your host environment
// -----------------------------------------------------------------------------

static int send_event(const host_event_ex_t *ev)
{
  (void) ev;
  return U_OK;
}

static int wait_event(host_event_ex_t *ev, uint32_t timeout_ms)
{
  (void) ev;
  (void) timeout_ms;
  return U_TIMEOUT;
}

static uint16_t calc_crc_ccitt(const uint8_t *image, size_t image_len, uint32_t block_size)
{
  (void) image;
  (void) image_len;
  (void) block_size;
  return 0;
}

// -----------------------------------------------------------------------------
// Utilities
// -----------------------------------------------------------------------------

static void put_u32_be(uint8_t *dst, uint32_t v)
{
  dst[0] = (uint8_t) ((v >> 24) & 0xFF);
  dst[1] = (uint8_t) ((v >> 16) & 0xFF);
  dst[2] = (uint8_t) ((v >> 8) & 0xFF);
  dst[3] = (uint8_t) (v & 0xFF);
}

static uint32_t get_u32_be(const uint8_t *src)
{
  return ((uint32_t) src[0] << 24) |
         ((uint32_t) src[1] << 16) |
         ((uint32_t) src[2] << 8) |
         (uint32_t) src[3];
}

static int wait_type(uint16_t expected_type, updater_fsm_ctx_t *ctx)
{
  host_event_ex_t rx;
  int rv = wait_event(&rx, ctx->timeout_ms);
  if (U_OK != rv) {
    return rv;
  }
  if (rx.vscp_type == expected_type) {
    return U_OK;
  }
  return U_PROTOCOL;
}

static int send_start(updater_fsm_ctx_t *ctx)
{
  host_event_ex_t tx;
  memset(&tx, 0, sizeof(tx));
  tx.vscp_type = T_START_BLOCK;
  tx.sizeData = 6;
  put_u32_be(&tx.data[0], ctx->current_block);
  tx.data[4] = ctx->memory_type;
  tx.data[5] = ctx->memory_bank;
  return send_event(&tx);
}

static int send_program(updater_fsm_ctx_t *ctx)
{
  host_event_ex_t tx;
  memset(&tx, 0, sizeof(tx));
  tx.vscp_type = T_PROGRAM_BLOCK;
  tx.sizeData = 4;
  put_u32_be(&tx.data[0], ctx->current_block);
  return send_event(&tx);
}

static int send_activate(uint16_t crc)
{
  host_event_ex_t tx;
  memset(&tx, 0, sizeof(tx));
  tx.vscp_type = T_ACTIVATE;
  tx.sizeData = 2;
  tx.data[0] = (uint8_t) ((crc >> 8) & 0xFF);
  tx.data[1] = (uint8_t) (crc & 0xFF);
  return send_event(&tx);
}

static int send_data_for_current_block(updater_fsm_ctx_t *ctx)
{
  uint8_t block_buf[1024];
  if (ctx->block_size > sizeof(block_buf)) {
    return U_BAD_ARGS;
  }

  memset(block_buf, 0xFF, ctx->block_size);
  size_t src_off = (size_t) ctx->current_block * ctx->block_size;
  size_t copy_len = (ctx->image_len - src_off) > ctx->block_size ? ctx->block_size : (ctx->image_len - src_off);
  memcpy(block_buf, ctx->image + src_off, copy_len);

  uint32_t offset = 0;
  while (offset < ctx->block_size) {
    host_event_ex_t tx;
    memset(&tx, 0, sizeof(tx));
    tx.vscp_type = T_BLOCK_DATA;

    uint16_t chunk = (uint16_t) ((ctx->block_size - offset) > sizeof(tx.data)
                                   ? sizeof(tx.data)
                                   : (ctx->block_size - offset));
    tx.sizeData = chunk;
    memcpy(tx.data, block_buf + offset, chunk);

    int acked = 0;
    for (uint8_t r = 0; r < ctx->msg_retries; ++r) {
      if (U_OK != send_event(&tx)) {
        continue;
      }
      if (U_OK == wait_type(T_BLOCK_DATA_ACK, ctx)) {
        acked = 1;
        break;
      }
    }

    if (!acked) {
      return U_RETRY_EXHAUSTED;
    }

    offset += chunk;
  }

  return U_OK;
}

// -----------------------------------------------------------------------------
// FSM runner
// -----------------------------------------------------------------------------

int vscp_host_update_fsm(updater_fsm_ctx_t *ctx)
{
  if (!ctx || !ctx->image || ctx->image_len == 0) {
    return U_BAD_ARGS;
  }

  updater_state_t state = ST_WAIT_BOOT_ACK;
  uint8_t per_block_retry = 0;

  while (state != ST_DONE && state != ST_FAIL) {
    switch (state) {

      // Receive bootloader capabilities and validate block geometry.
      case ST_WAIT_BOOT_ACK: {
        host_event_ex_t hello;
        int rv = wait_event(&hello, ctx->timeout_ms);
        if (U_OK != rv || hello.vscp_type != T_ACK_BOOT_LOADER || hello.sizeData < 8) {
          ctx->last_error = U_PROTOCOL;
          state = ST_FAIL;
          break;
        }

        ctx->block_size = get_u32_be(&hello.data[0]);
        ctx->block_count = get_u32_be(&hello.data[4]);
        if (ctx->block_size == 0) {
          ctx->last_error = U_PROTOCOL;
          state = ST_FAIL;
          break;
        }

        ctx->total_blocks = (uint32_t) ((ctx->image_len + ctx->block_size - 1) / ctx->block_size);
        if (ctx->total_blocks > ctx->block_count) {
          ctx->last_error = U_BAD_ARGS;
          state = ST_FAIL;
          break;
        }

        ctx->current_block = 0;
        per_block_retry = 0;
        state = ST_START_BLOCK;
        break;
      }

      // Request start of current block and wait for block-start ACK.
      case ST_START_BLOCK:
        if (U_OK != send_start(ctx) || U_OK != wait_type(T_START_BLOCK_ACK, ctx)) {
          if (++per_block_retry >= ctx->block_retries) {
            ctx->last_error = U_RETRY_EXHAUSTED;
            state = ST_FAIL;
          }
          break;
        }
        state = ST_SEND_BLOCK_DATA;
        break;

      // Send data frames for current block and require ACK for each chunk.
      case ST_SEND_BLOCK_DATA:
        if (U_OK != send_data_for_current_block(ctx)) {
          if (++per_block_retry >= ctx->block_retries) {
            ctx->last_error = U_RETRY_EXHAUSTED;
            state = ST_FAIL;
          }
          else {
            state = ST_START_BLOCK;
          }
          break;
        }
        state = ST_PROGRAM_BLOCK;
        break;

      // Commit current block to target memory.
      case ST_PROGRAM_BLOCK:
        if (U_OK != send_program(ctx) || U_OK != wait_type(T_PROGRAM_BLOCK_ACK, ctx)) {
          if (++per_block_retry >= ctx->block_retries) {
            ctx->last_error = U_RETRY_EXHAUSTED;
            state = ST_FAIL;
          }
          else {
            state = ST_START_BLOCK;
          }
          break;
        }
        state = ST_NEXT_BLOCK;
        break;

      // Advance block index or continue to image activation.
      case ST_NEXT_BLOCK:
        ctx->current_block++;
        per_block_retry = 0;
        state = (ctx->current_block < ctx->total_blocks) ? ST_START_BLOCK : ST_ACTIVATE_IMAGE;
        break;

      // Send expected image CRC and finalize update.
      case ST_ACTIVATE_IMAGE: {
        uint16_t crc = calc_crc_ccitt(ctx->image, ctx->image_len, ctx->block_size);
        if (U_OK != send_activate(crc) || U_OK != wait_type(T_ACTIVATE_ACK, ctx)) {
          ctx->last_error = U_CRC_FAIL;
          state = ST_FAIL;
          break;
        }
        state = ST_DONE;
        break;
      }

      // Fail-safe for unknown state values.
      default:
        ctx->last_error = U_PROTOCOL;
        state = ST_FAIL;
        break;
    }
  }

  return (state == ST_DONE) ? U_OK : ctx->last_error;
}
