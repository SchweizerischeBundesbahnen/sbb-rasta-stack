/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rednot_red_notifications.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_notifications
 * @{
 *
 * @brief Interface of RaSTA RedL notifications to the SafRetL adapter.
 *
 * This module defines the message received notification and diagnostic notification function interfaces for the RedL. The RedL only defines
 * the interface, the implementation of this notification functions must be done in the SafRetL adapter.
 */
#ifndef REDNOT_RED_NOTIFICATIONS_H_
#define REDNOT_RED_NOTIFICATIONS_H_

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
 * @brief Redundancy layer message received notification function to SafRetL adapter.
 *
 * This function is called by the redundancy layer to notify the upper layer that a received message is ready to be read on a specific redundancy channel.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 */
void rednot_MessageReceivedNotification(const uint32_t red_channel_id);

/**
 * @brief Redundancy layer diagnostic notification function to SafRetL adapter.
 *
 * This function is called by the redundancy layer to notify the upper layer about new diagnostic data from a specific redundancy channel.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 * @param [in] tr_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS.
 * @param [in] TransportChannelDiagnosticData Transport channel diagnostic data structure. Structure and valid ranges can be found in
 * ::radef_TransportChannelDiagnosticData.
 */
void rednot_DiagnosticNotification(const uint32_t red_channel_id, const uint32_t tr_channel_id,
                                   const radef_TransportChannelDiagnosticData TransportChannelDiagnosticData);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDNOT_RED_NOTIFICATIONS_H_
