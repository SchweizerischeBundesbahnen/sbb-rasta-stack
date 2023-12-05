/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srnot_sr_notifications.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_notifications
 * @{
 *
 * @brief Interface of RaSTA SafRetL notifications to the upper layer.
 *
 * This module defines the notification functions (like message received, connection state and diagnostic) for the application layer. The SafRetL only defines
 * the interface, the implementation of this notification functions must be done in the application layer.
 *
 * @implementsReq{RASW-553} Component sr_notifications Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 */
#ifndef SRNOT_SAFETY_RETRANSMISSION_NOTIFICATIONS_H_
#define SRNOT_SAFETY_RETRANSMISSION_NOTIFICATIONS_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_safety_retransmission/sraty_sr_api_types.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief SafRetL message received notification function.
 *
 * This function is called by the SafRetL to notify the application layer that a received message from a specific connection is ready to be read.
 *
 * @implementsReq{RASW-554} Message Received Notification
 * @implementsReq{RASW-279} Message Received Notification Structure
 * @implementsReq{RASW-302} Connection Identification
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srnot_MessageReceivedNotification(const uint32_t connection_id);

/**
 * @brief SafRetL connection state notification function.
 *
 * This function is called by the SafRetL to notify the application layer about a connection state change of a specific connection. Additionally many
 * informations about the specified connection are send with this notification.
 *
 * @implementsReq{RASW-555} Connection State Notification
 * @implementsReq{RASW-296} Connection State Notification Structure
 * @implementsReq{RASW-299} Connection Identification
 * @implementsReq{RASW-298} Connection State
 * @implementsReq{RASW-293} Buffer Utilisation
 * @implementsReq{RASW-291} Opposite Buffer Size
 * @implementsReq{RASW-295} Disconnect Reason
 * @implementsReq{RASW-294} Detailed Disconnect Reason
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] connection_state Current connection state of the SafRetL. Valid range: ::sraty_kConnectionMin <= value < ::sraty_kConnectionMax.
 * @param [in] buffer_utilisation Struct with buffer utilisation data. Structure and valid ranges can be found in ::sraty_BufferUtilisation.
 * @param [in] opposite_buffer_size Size of the receive buffer of the opposite RaSTA instance. Full value range is valid and usable.
 * @param [in] disconnect_reason Disconnect reason (only valid if connection state changed to ::sraty_kConnectionClosed). Valid range: ::sraty_kDiscReasonMin <=
 * value < ::sraty_kDiscReasonMax.
 * @param [in] detailed_disconnect_reason Detailed disconnect reason application (only valid if connection state changed to ::sraty_kConnectionClosed). Full
 * value range is valid and usable.
 */
void srnot_ConnectionStateNotification(const uint32_t connection_id, const sraty_ConnectionStates connection_state,
                                       const sraty_BufferUtilisation buffer_utilisation, const uint16_t opposite_buffer_size,
                                       const sraty_DiscReason disconnect_reason, const uint16_t detailed_disconnect_reason);

/**
 * @brief SafRetL diagnostic data notification function.
 *
 * This function is called by the SafRetL to notify the application layer about new diagnostic data from the SafRetL. This contains all error counters as well
 * as the distribution of the round trip delay time and the alive time.
 *
 * @implementsReq{RASW-556} Sr Diagnostic Notification
 * @implementsReq{RASW-327} SafRetL Diagnostic Notification Structure
 * @implementsReq{RASW-326} Connection Identification
 * @implementsReq{RASW-321} Connection Diagnostic Data
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] connection_diagnostic_data Diagnostic data of the SafRetL. Structure and valid ranges can be found in ::sraty_ConnectionDiagnosticData.
 */
void srnot_SrDiagnosticNotification(const uint32_t connection_id, const sraty_ConnectionDiagnosticData connection_diagnostic_data);

/**
 * @brief Forwarded RedL diagnostic data notification function.
 *
 * This function is called by the SafRetL to notify the application layer about new diagnostic data from the RedL. This data is forwarded without any
 * modification. This contains the transport channel identification, diagnostic window size, number of missed messages and the average delay indicators Tdrift
 * und Tdrift2.
 *
 * @implementsReq{RASW-557} Red Diagnostic Notification
 * @implementsReq{RASW-325} RedL Diagnostic Notification Structure
 * @implementsReq{RASW-323} Connection Identification
 * @implementsReq{RASW-315} Redundancy Channel Diagnostic Data
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] redundancy_channel_diagnostic_data Diagnostic data from the redundancy channel. Structure and valid ranges can be found in
 * ::sraty_RedundancyChannelDiagnosticData.
 */
void srnot_RedDiagnosticNotification(const uint32_t connection_id, const sraty_RedundancyChannelDiagnosticData redundancy_channel_diagnostic_data);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRNOT_SAFETY_RETRANSMISSION_NOTIFICATIONS_H_
