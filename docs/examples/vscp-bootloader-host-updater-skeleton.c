/*
  VSCP Bootloader Host Updater Skeleton

  Purpose:
  - Show a reference host-side state machine for firmware upload.
  - Keep transport/API hooks abstract so this can be adapted to CAN, TCP, etc.

  This file is documentation-oriented example code and is not wired into build targets.
*/

#include <stdint.h>
#include <stddef.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Protocol constants (replace/include from your VSCP headers in real host code)
// -----------------------------------------------------------------------------

enum {
  VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER          = 1,
  VSCP_TYPE_PROTOCOL_START_BLOCK              = 2,
  VSCP_TYPE_PROTOCOL_START_BLOCK_ACK          = 3,
  VSCP_TYPE_PROTOCOL_START_BLOCK_NACK         = 4,
  VSCP_TYPE_PROTOCOL_BLOCK_DATA               = 5,
  VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK           = 6,
  VSCP_TYPE_PROTOCOL_BLOCK_DATA_NACK          = 7,
  VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA       = 8,
  VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK   = 9,
  VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_NACK  = 10,
  VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE       = 11,
  VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_ACK   = 12,
  VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_NACK  = 13,
};

enum {
  HOST_OK = 0,
  HOST_ERR_TIMEOUT = -1,
  HOST_ERR_PROTOCOL = -2,
  HOST_ERR_RETRY_EXHAUSTED = -3,
  HOST_ERR_CRC = -4,
  HOST_ERR_ARGS = -5,
};

typedef struct host_vscp_event_ex {
  uint16_t vscp_class;
  uint16_t vscp_type;
  uint16_t sizeData;
  uint8_t data[512];
} host_vscp_event_ex_t;

typedef struct updater_ctx {
  uint32_t block_size;
  uint32_t block_count;
  uint8_t memory_type;
  uint8_t memory_bank;
  uint32_t response_timeout_ms;
  uint8_t max_msg_retries;
  uint8_t max_block_retries;
} updater_ctx_t;

// -----------------------------------------------------------------------------
// Transport hooks (implement for your environment)
// -----------------------------------------------------------------------------

static int transport_send_event(const host_vscp_event_ex_t *ev)
{
  (void) ev;
  return HOST_OK;
}

static int transport_wait_event(host_vscp_event_ex_t *ev, uint32_t timeout_ms)
{
  (void) ev;
  (void) timeout_ms;
  return HOST_ERR_TIMEOUT;
}

static uint16_t calc_crc_ccitt_image(const uint8_t *image, size_t image_len,
                                     uint32_t block_size)
{
  (void) image;
  (void) image_len;
  (void) block_size;
  return 0;
}

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------

static void write_u32_be(uint8_t *dst, uint32_t val)
{
  dst[0] = (uint8_t) ((val >> 24) & 0xFF);
  dst[1] = (uint8_t) ((val >> 16) & 0xFF);
  dst[2] = (uint8_t) ((val >> 8) & 0xFF);
  dst[3] = (uint8_t) (val & 0xFF);
}

static uint32_t read_u32_be(const uint8_t *src)
{
  return ((uint32_t) src[0] << 24) |
         ((uint32_t) src[1] << 16) |
         ((uint32_t) src[2] << 8) |
         (uint32_t) src[3];
}

static int wait_for_type(uint16_t expected_type, updater_ctx_t *ctx)
{
  // Wait for one event and enforce expected protocol type.
  host_vscp_event_ex_t rx;
  int rv = transport_wait_event(&rx, ctx->response_timeout_ms);
  if (HOST_OK != rv) {
    return rv;
  }
  return (rx.vscp_type == expected_type) ? HOST_OK : HOST_ERR_PROTOCOL;
}

static int send_start_block(updater_ctx_t *ctx, uint32_t block_index)
{
  // Request start of a specific firmware block on target.
  host_vscp_event_ex_t tx;
  memset(&tx, 0, sizeof(tx));
  tx.vscp_type = VSCP_TYPE_PROTOCOL_START_BLOCK;
  tx.sizeData = 6;
  write_u32_be(&tx.data[0], block_index);
  tx.data[4] = ctx->memory_type;
  tx.data[5] = ctx->memory_bank;
  return transport_send_event(&tx);
}

static int send_program_block(uint32_t block_index)
{
  // Request commit/program operation for the current block.
  host_vscp_event_ex_t tx;
  memset(&tx, 0, sizeof(tx));
  tx.vscp_type = VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA;
  tx.sizeData = 4;
  write_u32_be(&tx.data[0], block_index);
  return transport_send_event(&tx);
}

static int send_activate_image(uint16_t crc)
{
  // Finalize upload by sending expected image CRC.
  host_vscp_event_ex_t tx;
  memset(&tx, 0, sizeof(tx));
  tx.vscp_type = VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE;
  tx.sizeData = 2;
  tx.data[0] = (uint8_t) ((crc >> 8) & 0xFF);
  tx.data[1] = (uint8_t) (crc & 0xFF);
  return transport_send_event(&tx);
}

static int send_block_data_frames(const uint8_t *block_buf, uint32_t block_size,
                                  updater_ctx_t *ctx)
{
  // Stream one block as chunked data frames, requiring ACK per chunk.
  uint32_t offset = 0;
  while (offset < block_size) {
    host_vscp_event_ex_t tx;
    memset(&tx, 0, sizeof(tx));
    tx.vscp_type = VSCP_TYPE_PROTOCOL_BLOCK_DATA;

    uint16_t chunk = (uint16_t) ((block_size - offset) > sizeof(tx.data) ? sizeof(tx.data)
                                                                       : (block_size - offset));
    tx.sizeData = chunk;
    memcpy(tx.data, block_buf + offset, chunk);

    int acked = 0;
    for (uint8_t retry = 0; retry < ctx->max_msg_retries; ++retry) {
      if (HOST_OK != transport_send_event(&tx)) {
        continue;
      }
      int rv = wait_for_type(VSCP_TYPE_PROTOCOL_BLOCK_DATA_ACK, ctx);
      if (HOST_OK == rv) {
        acked = 1;
        break;
      }
    }

    if (!acked) {
      return HOST_ERR_RETRY_EXHAUSTED;
    }

    offset += chunk;
  }

  return HOST_OK;
}

// -----------------------------------------------------------------------------
// Main update routine
// -----------------------------------------------------------------------------

int vscp_host_update_firmware(const uint8_t *image, size_t image_len, updater_ctx_t *ctx)
{
  if (!image || !ctx || image_len == 0) {
    return HOST_ERR_ARGS;
  }

  // Step 1: Receive bootloader handshake with block geometry.
  host_vscp_event_ex_t hello;
  int rv = transport_wait_event(&hello, ctx->response_timeout_ms);
  if (HOST_OK != rv || hello.vscp_type != VSCP_TYPE_PROTOCOL_ACK_BOOT_LOADER || hello.sizeData < 8) {
    return HOST_ERR_PROTOCOL;
  }

  ctx->block_size = read_u32_be(&hello.data[0]);
  ctx->block_count = read_u32_be(&hello.data[4]);
  if (ctx->block_size == 0) {
    return HOST_ERR_PROTOCOL;
  }

  uint32_t total_blocks = (uint32_t) ((image_len + ctx->block_size - 1) / ctx->block_size);
  if (total_blocks > ctx->block_count) {
    return HOST_ERR_ARGS;
  }

  // Step 2: Send each block (start, data, program) with bounded retries.
  for (uint32_t block = 0; block < total_blocks; ++block) {
    uint8_t block_buf[1024]; // Replace with dynamic allocation if block size can exceed this
    if (ctx->block_size > sizeof(block_buf)) {
      return HOST_ERR_ARGS;
    }

    memset(block_buf, 0xFF, ctx->block_size);
    size_t src_off = (size_t) block * ctx->block_size;
    size_t copy_len = (image_len - src_off) > ctx->block_size ? ctx->block_size : (image_len - src_off);
    memcpy(block_buf, image + src_off, copy_len);

    int block_done = 0;
    for (uint8_t block_retry = 0; block_retry < ctx->max_block_retries; ++block_retry) {
      if (HOST_OK != send_start_block(ctx, block)) {
        continue;
      }

      rv = wait_for_type(VSCP_TYPE_PROTOCOL_START_BLOCK_ACK, ctx);
      if (HOST_OK != rv) {
        continue;
      }

      rv = send_block_data_frames(block_buf, ctx->block_size, ctx);
      if (HOST_OK != rv) {
        continue;
      }

      if (HOST_OK != send_program_block(block)) {
        continue;
      }

      rv = wait_for_type(VSCP_TYPE_PROTOCOL_PROGRAM_BLOCK_DATA_ACK, ctx);
      if (HOST_OK == rv) {
        block_done = 1;
        break;
      }
    }

    if (!block_done) {
      return HOST_ERR_RETRY_EXHAUSTED;
    }
  }

  // Step 3: Activate image and verify activation ACK.
  uint16_t expected_crc = calc_crc_ccitt_image(image, image_len, ctx->block_size);
  if (HOST_OK != send_activate_image(expected_crc)) {
    return HOST_ERR_PROTOCOL;
  }

  rv = wait_for_type(VSCP_TYPE_PROTOCOL_ACTIVATE_NEW_IMAGE_ACK, ctx);
  if (HOST_OK != rv) {
    return HOST_ERR_CRC;
  }

  return HOST_OK;
}
