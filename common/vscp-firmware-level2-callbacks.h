// FILE: vscp-firmware-level2-callbacks.h

// This file holds definitions for callbacks for the VSCP firmware level 2 protocol

/* ******************************************************************************
 * 	VSCP (Very Simple Control Protocol)
 * 	https://www.vscp.org
 *
 * The MIT License (MIT)
 *
 * Copyright (C) 2000-2026 Ake Hedman, Grodans Paradis AB <info@grodansparadis.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *	This file is part of VSCP - Very Simple Control Protocol
 *	https://www.vscp.org
 *
 * ******************************************************************************
 */

/*!
  @file vscp-firmware-level2-callbacks.h
  @defgroup vscp-level2-firmware-callbacks VSCP level II firmware callbacks

  This file defines the callbacks that the VSCP firmware will call to interact
  with the hardware and perform various operations. The firmware expects the
  user to implement these callbacks according to the needs of their specific
  hardware and application.

  @{

*/

/*!
  Get milliseconds

  @param pctx Pointer to context.
  @return Current time in milliseconds.
*/
uint32_t
vscp_frmw2_callback_get_ms(vscp_frmw2_firmware_context_t *pctx);

/*!
  Get GUID

  @param pctx Pointer to context.
  @return Pointer to 16-byte GUID.
*/
const uint8_t *
vscp_frmw2_callback_get_guid(vscp_frmw2_firmware_context_t *pctx);

#ifdef THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS

/*!
  Write Manufacturer ID

  @param pctx Pointer to context.
  @param pos Position to write.
  @param val Value to write.
  @return 0 on success, non-zero on error.
*/
int
vscp_frmw2_callback_write_manufacturer_id(vscp_frmw2_firmware_context_t *pctx, uint8_t pos, uint8_t val);

/*!
  Write GUID

  @param pctx Pointer to context.
  @param pos Position to write.
  @param val Value to write.
  @return 0 on success, non-zero on error.
*/
int
vscp_frmw2_callback_write_guid(vscp_frmw2_firmware_context_t *pctx, uint8_t pos, uint8_t val);

#endif // THIS_FIRMWARE_ENABLE_WRITE_2PROTECTED_LOCATIONS

/*!
  Read user register

  @param pctx Pointer to context.
  @param reg Register to read.
  @param pval Pointer to read value.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_read_user_reg(vscp_frmw2_firmware_context_t *pctx, uint32_t reg, uint8_t *pval);

/*!
  Write user register

  @param pctx Pointer to context.
  @param reg Register to write.
  @param val Value to write.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_write_user_reg(vscp_frmw2_firmware_context_t *pctx, uint32_t reg, uint8_t val);

/*!
  Enter bootloader mode

  @param pctx Pointer to context.
*/
void
vscp_frmw2_callback_enter_bootloader(vscp_frmw2_firmware_context_t *pctx);

/*!
  Report decision matrix

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_report_dmatrix(vscp_frmw2_firmware_context_t *pctx);

/*!
  Report MDF

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_report_mdf(vscp_frmw2_firmware_context_t *pctx);

/*!
  Report events of interest

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_report_events_of_interest(vscp_frmw2_firmware_context_t *pctx);

/*!
  Get timestamp

  @param pctx Pointer to context.
  @return Timestamp value.
*/
uint64_t
vscp_frmw2_callback_get_timestamp(vscp_frmw2_firmware_context_t *pctx);

/*!
  Get time

  @param pctx Pointer to context.
  @param pex Pointer to event.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_get_time(vscp_frmw2_firmware_context_t *pctx, const vscp_event_ex_t *pex);

/*!
  Restore defaults

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_restore_defaults(vscp_frmw2_firmware_context_t *pctx);

/*!
  Write user ID

  @param pctx Pointer to context.
  @param pos Position to write.
  @param val Value to write.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_write_user_id(vscp_frmw2_firmware_context_t *pctx, uint8_t pos, uint8_t val);

/*!
  Get IP address

  @param puserdata Pointer to user data.
  @param ip Pointer to buffer to receive IP address (4 bytes).
  @param size Size of buffer (should be at least 4).
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_get_ip_addr(vscp_frmw2_firmware_context_t *pctx, uint8_t *ip, uint8_t size);

/*!
  Set event time

  @param pctx Pointer to context.
  @param pev Pointer to event.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_set_event_time(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *const pev);

#ifdef THIS_FIRMWARE_VSCP_DISCOVER_SERVER

/*!
  High-end server response callback

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_high_end_server_response(vscp_frmw2_firmware_context_t *pctx);

#endif // THIS_FIRMWARE_VSCP_DISCOVER_SERVER

/*!
  Send segment controller heartbeat
*/
int
vscp_frmw2_callback_segment_ctrl_heartbeat(vscp_frmw2_firmware_context_t *pctx, uint16_t segcrc, uint32_t time);

/*!
  Send event

  @param pctx Pointer to context.
  @param pev Pointer to event.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_send_event(vscp_frmw2_firmware_context_t *pctx, vscp_event_t *pev);

/*!
  Standard register change callback

  @param pctx Pointer to context.
  @param stdreg Standard register that changed.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_stdreg_change(vscp_frmw2_firmware_context_t *pctx, uint32_t stdreg);

/*!
  Decision matrix action callback

  @param pctx Pointer to context.
  @param pev Pointer to event that triggered the action.
  @param code Action code.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_dm_action(vscp_frmw2_firmware_context_t *pctx,
                   const vscp_event_t *pev,
                   uint8_t action,
                   const uint8_t *pparam);

/*!
  Read register

  @param pctx Pointer to context.
  @param reg Register to read.
  @param pval Pointer to variable to receive the value.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_read_reg(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t *pval);

/*!
  Write register

  @param pctx Pointer to context.
  @param page Page number.
  @param reg Register to write.
  @param val Value to write.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
int
vscp_frmw2_callback_write_reg(vscp_frmw2_firmware_context_t *pctx, uint16_t page, uint32_t reg, uint8_t val);

/*!
  Reset callback

  @param pctx Pointer to context.
*/
void
vscp_frmw2_callback_reset(vscp_frmw2_firmware_context_t *pctx);

/*!
  Feed watchdog

  @param pctx Pointer to context.
  @return VSCP_ERROR_SUCCESS on success, else error code.
*/
void
vscp_frmw2_callback_feed_watchdog(vscp_frmw2_firmware_context_t *pctx);

#ifdef __cplusplus
}
#endif

/**
  @}
*/