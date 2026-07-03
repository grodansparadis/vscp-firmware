// FILE: vscp-rs485-protocol.h
//
// The MIT License (MIT)
//
// Copyright (C) 2000-2026 Ake Hedman, the VSCP project
// <info@vscp.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This file defines VSCP RS-485/RS-422 transport framing constants for a
// master-polling multi-drop bus where each client has a preset 8-bit nickname.
//

#ifndef _VSCP_RS485_PROTOCOL_H_
#define _VSCP_RS485_PROTOCOL_H_

#include <vscp.h>

#ifdef __cplusplus
extern "C" {
#endif

// Framing control bytes (same escaping model as VSCP serial framing)
#define VSCP_RS485_DLE 0x10
#define VSCP_RS485_STX 0x02
#define VSCP_RS485_ETX 0x03

// Bus limits and addressing
#define VSCP_RS485_MAX_CLIENTS   255
#define VSCP_RS485_NICKNAME_SIZE 1
#define VSCP_RS485_MASTER_ID     0x00
#define VSCP_RS485_BROADCAST_ID  0xFF

// Receiver/parser state machine
#define STATE_VSCP_RS485_WAIT_FOR_FRAME_START 0
#define STATE_VSCP_RS485_WAIT_FOR_FRAME_END   1
#define STATE_VSCP_RS485_FRAME_RECEIVED       2

// Frame layout positions (decoded payload between STX/ETX delimiters)
#define VSCP_RS485_POS_FRAME_TYPE             0
#define VSCP_RS485_POS_DESTINATION_NICKNAME   1
#define VSCP_RS485_POS_SOURCE_NICKNAME        2
#define VSCP_RS485_POS_SEQUENCE               3
#define VSCP_RS485_POS_FLAGS                  4
#define VSCP_RS485_POS_FRAME_SIZE_PAYLOAD_MSB 5
#define VSCP_RS485_POS_FRAME_SIZE_PAYLOAD_LSB 6
#define VSCP_RS485_POS_FRAME_PAYLOAD          7

// Flags (VSCP_RS485_POS_FLAGS)
#define VSCP_RS485_FLAG_ENCRYPTED   0x01
#define VSCP_RS485_FLAG_ACK_REQUEST 0x02
#define VSCP_RS485_FLAG_RESERVED    0xFC

// Frame types for bus operation and event transfer
#define VSCP_RS485_FRAME_TYPE_NOOP          0
#define VSCP_RS485_FRAME_TYPE_POLL_REQUEST  1
#define VSCP_RS485_FRAME_TYPE_POLL_RESPONSE 2
#define VSCP_RS485_FRAME_TYPE_VSCP_EVENT    3
#define VSCP_RS485_FRAME_TYPE_NO_EVENT      4
#define VSCP_RS485_FRAME_TYPE_COMMAND       5
#define VSCP_RS485_FRAME_TYPE_COMMAND_REPLY 6
#define VSCP_RS485_FRAME_TYPE_ACK           251
#define VSCP_RS485_FRAME_TYPE_NACK          252
#define VSCP_RS485_FRAME_TYPE_ERROR         253

// Transport level command set
#define VSCP_RS485_COMMAND_NOOP                  0
#define VSCP_RS485_COMMAND_POLL                  1
#define VSCP_RS485_COMMAND_SET_ENCRYPTION_NONE   2
#define VSCP_RS485_COMMAND_SET_ENCRYPTION_AES128 3

// Driver/protocol errors
#define VSCP_RS485_ERROR_CHECKSUM           1
#define VSCP_RS485_ERROR_UNKNOWN_OPERATION  2
#define VSCP_RS485_ERROR_INVALID_NICKNAME   3
#define VSCP_RS485_ERROR_ENCRYPTION_FAILURE 4

// Encryption modes for this transport
#define VSCP_RS485_ENCRYPTION_NONE   0
#define VSCP_RS485_ENCRYPTION_AES128 1

// Packed size for capabilities structure
#define VSCP_RS485_CAPS_SIZE 4

// Capability set for a node/client on RS-485/RS-422 transport
typedef struct
{
  uint8_t maxClients;         // Maximum supported polled clients
  uint8_t encryptionSupported; // 0: no, 1: yes
  uint8_t defaultEncryption;   // VSCP_RS485_ENCRYPTION_*
  uint8_t reserved;
} vscp_rs485_caps;

#ifdef __cplusplus
}
#endif

#endif
