/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file sradno_sr_adapter_notifications.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 3d5012751f7f8e2a1c005ab71aeb88c68eaf7107
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4932: sr_adapter_notification module functions are not error tolerant,08.12.2022, N. Andres}
 *
 * @addtogroup sr_adapter_notifications
 * @{
 *
 * @brief Interface of RaSTA SafRetL adapter notifications.
 *
 * This module defines and implements the interface notifications (like message received notification and diagnostic notification) for the SafRetL, as they act
 * as entry point to for the SafRetL adapter.
 *
 * @implementsReq{RASW-654} Component sr_adapter_notifications Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRADNO_SAFETY_RETRANSMISSION_ADAPTER_NOTIFICATIONS_H_
#define SRADNO_SAFETY_RETRANSMISSION_ADAPTER_NOTIFICATIONS_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"

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
 * @brief Message received notification from the RedL.
 *
 * This function is called by the SafRetL adapter to notify the SafRetL that a received message from a specific redundancy channel is ready to be read. The
 * redundancy channel id is checked if it is in a valid range, otherwise a ::radef_kInvalidParameter is returned. New messages are read as long as received
 * messages pending and enough space in send and received buffers (checked with ::srcor_IsReceivedMsgPendingAndBuffersNotFull).
 *
 * @remark There is a 1:1 mapping between the connection id and redundancy channel id.
 *
 * @implementsReq{RASW-655} Message Received Notification
 * @implementsReq{RASW-335} Message Received Notification Structure
 * @implementsReq{RASW-338} Redundancy Channel Id
 * @implementsReq{RASW-900} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-747} Message Received Notification Sequence
 * @implementsReq{RASW-769} Received Message Polling Sequence
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode sradno_MessageReceivedNotification(const uint32_t red_channel_id);

/**
 * @brief Diagnostic notification from the RedL.
 *
 * This function is called by the SafRetL adapter to notify the SafRetL about new diagnostic data from the RedL. The redundancy channel id is checked if it is
 * in a valid range, otherwise a ::radef_kInvalidParameter is returned. The passed diagnostic data contains the transport channel identification, diagnostic
 * window size, number of missed messages and the average delay indicators Tdrift und Tdrift2.
 *
 * @implementsReq{RASW-656} Diagnostic Notification
 * @implementsReq{RASW-332} Diagnostic Notification Structure
 * @implementsReq{RASW-331} Redundancy Channel Id
 * @implementsReq{RASW-334} Transport Channel Id
 * @implementsReq{RASW-333} Transport Channel Diagnostic Data
 * @implementsReq{RASW-901} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-751} Redundancy Diagnostic Notification Sequence
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
 * @param [in] tr_channel_id Transport channel identification. The full value range is valid and usable.
 * @param [in] tr_channel_diagnostic_data Transport channel diagnostic data. Structure and valid ranges can be found in ::radef_TransportChannelDiagnosticData.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode sradno_DiagnosticNotification(const uint32_t red_channel_id, const uint32_t tr_channel_id,
                                                    const radef_TransportChannelDiagnosticData tr_channel_diagnostic_data);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRADNO_SAFETY_RETRANSMISSION_ADAPTER_NOTIFICATIONS_H_
