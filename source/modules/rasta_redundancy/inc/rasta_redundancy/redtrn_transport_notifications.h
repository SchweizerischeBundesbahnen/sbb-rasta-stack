/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redtrn_transport_notifications.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_transport_notifications
 * @{
 *
 * @brief Interface of RaSTA transport layer notifications.
 *
 * This module defines and implements the transport layer message received notification, as it acts as entry point to the RedL for the transport layer.
 */
#ifndef REDTRN_TRANSPORT_NOTIFICATIONS_H_
#define REDTRN_TRANSPORT_NOTIFICATIONS_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

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
 * @brief Transport layer message received notification function.
 *
 * This function is called by the transport layer to notify the RedL that a new received message is ready to be read. If the associated redundancy channel state
 * machine is in the up state and there are more free received buffer entries than used defer queue entries, the message is read from the transport channel,
 * copied to the input buffer and the state machine is triggered with a ::redstm_kRedundancyChannelEventReceiveData event, to process the received message.
 * If there are not enough free buffer entries in the received buffer, the message is not read and the received message pending flag of this transport channel
 * is set. If the associated redundancy channel state machine is not in the up state, the message is read and discarded.
 * If in any case there was no message to read, the received message pending flag of this transport channel is cleared.
 *
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of a
 * redundancy channel, a ::radef_kInvalidParameter fatal error is thrown.
 */
void redtrn_MessageReceivedNotification(const uint32_t transport_channel_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDTRN_TRANSPORT_NOTIFICATIONS_H_
