/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddia_red_diagnostics.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_diagnostics
 * @{
 *
 * @brief Interface of RaSTA redundancy layer diagnostics.
 *
 * This module provides the RaSTA RedL diagnostics functionality:
 * - Initialize the diagnostic data of a redundancy channel
 * - Update diagnostic data of a redundancy channel with the data of a newly received message
 * - Trigger diagnostic notifications with diagnostic data of the transport channels, if the diagnostic window for a redundancy channel is reached
 * .
 * Additional the following helper functions are provided:
 * - Check the validity of the RedL configuration
 * - Check the association of a transport channel to a redundancy channel
 * .
 */
#ifndef REDDIA_RED_DIAGNOSTICS_H_
#define REDDIA_RED_DIAGNOSTICS_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "rasta_redundancy/redcty_red_config_types.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Struct for the timestamps of first received messages.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint32_t sequence_number;                                            ///< Sequence number of the message.
  uint32_t received_time_stamp;                                        ///< Message received timestamp, when this message was received first.
  bool message_received_flag[RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS];  ///< Message received flag for each transport channel. This is used to correctly count
                                                                       ///< n_missed, for messages which didn't arrive at all on a transport channel.
} reddia_ReceivedMessageTimestamp;
//lint -restore

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
 * @brief Initialize the RedL diagnostics module.
 *
 * This function is used to initialize the RedL diagnostics module. The validity of the configuration is checked by calling the ::reddia_IsConfigurationValid()
 * function. It saves the passed redundancy layer configuration pointer. For all configured channels, the ::reddia_InitRedundancyChannelDiagnostics() function
 * is called to properly initialize the diagnostic data for all configured channels.
 *
 * @pre The RedL diagnostics module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @param [in] redundancy_layer_configuration Pointer to redundancy layer configuration data structure. If the pointer is NULL, a ::radef_kInvalidParameter
 * fatal error is thrown. If the configuration is not valid a::radef_kInvalidConfiguration fatal error is thrown.
 */
void reddia_InitRedundancyLayerDiagnostics(const redcty_RedundancyLayerConfiguration *const redundancy_layer_configuration);

/**
 * @brief Initialize diagnostic data of a dedicated redundancy channel.
 *
 * This function initializes the diagnostic data of a given redundancy channel. It resets the following properties:
 * - timestamps include complete ::reddia_ReceivedMessageTimestamp structure
 * - Ndiagnosis
 * - Nmissed
 * - Tdrift
 * - Tdrift^2
 *.
 * @pre The RedL diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void reddia_InitRedundancyChannelDiagnostics(const uint32_t red_channel_id);

/**
 * @brief Update redundancy channel diagnostic data with the data of a newly received message.
 *
 * This function updates the redundancy channel diagnostic data with the data of a newly received message.
 * If a message with a specific sequence number is received for the first time, the sequence number and a message received timestamp of this message is saved
 * and the internal message received flag for this transport channel is set and the current Ndiagnosis (message counter) of the given redundancy channel is
 * increased. If the sequence number of a newly received message is found in the already received message sequence numbers, the difference to the first message
 * received timestamp to the current time is calculated. If this difference is smaller than the configured Tseq, the difference is added to Tdrift and the
 * square of the difference is added to Tdrift^2 and the internal message received flag of the transport channel is set. If the configured RedL diagnosis window
 * is reached for this redundancy channel, the Nmissed of each transport channel is calculated out of the internal message received flags and a diagnostic
 * notification is sent to all associated transport channels and the diagnostic data of this redundancy channel is cleared by calling
 * ::reddia_InitRedundancyChannelDiagnostics(). If the current Ndiagnosis is greater than the configured RedL diagnosis window size, a ::radef_kInternalError
 * fatal error is thrown.
 *
 * @pre The RedL diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [in] message_sequence_number Newly received message sequence number. The full value range is valid and usable.
 */
void reddia_UpdateRedundancyChannelDiagnostics(const uint32_t red_channel_id, const uint32_t transport_channel_id, const uint32_t message_sequence_number);

/**
 * @brief Checks if the redundancy layer configuration is valid.
 *
 * This function checks if all elements of the redundancy layer configuration are in their valid ranges and if all configuration elements are consistent to
 * the others. A configuration is valid if all of the following conditions are met:
 * - ::redcty_kCheckCodeMin <= check_code_type < ::redcty_kCheckCodeMax
 * - ::redcty_kMinTSeq <= t_seq <= ::redcty_kMaxTSeq
 * - ::redcty_kMinRedLayerNDiagnosis <= n_diagnosis <= ::RADEF_MAX_RED_LAYER_N_DIAGNOSIS
 * - ::redcty_kMinDeferQueueSize <= n_defer_queue_size <= ::RADEF_MAX_DEFER_QUEUE_SIZE
 * - ::redcty_kMinNumberOfRedundancyChannels <= number_of_redundancy_channels <= ::RADEF_MAX_NUMBER_OF_RED_CHANNELS
 * - red_channel_ids are identical to their index in the configuration data structure
 * - ::redcty_kMinNumberOfTransportChannels <= num_transport_channels <= ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS
 * - 0 <= transport_channel_ids < (::RADEF_MAX_NUMBER_OF_RED_CHANNELS * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS)
 * .
 * @param [in] redundancy_layer_configuration Pointer to redundancy layer configuration data structure. If the pointer is NULL, a ::radef_kInvalidParameter
 * fatal error is thrown.
 * @return true, if the configuration is valid.
 * @return false, if the configuration is invalid.
 */
bool reddia_IsConfigurationValid(const redcty_RedundancyLayerConfiguration *const redundancy_layer_configuration);

/**
 * @brief Checks, if a transport channel identification is valid for a given redundancy channel.
 *
 * This function checks, if a transport channel identification is found in the configuration of a given redundancy channel.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param transport_channel_id Transport channel identification to check. The full value range is valid, but if the transport channel is not associated to
 * the redundancy channel, the function returns false.
 * @return true, if the transport channel identification is found in the configuration of the given redundancy channel
 * @return false, if the transport channel identification is not found in the configuration of the given redundancy channel
 */
bool reddia_IsTransportChannelIdValid(const uint32_t red_channel_id, const uint32_t transport_channel_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDDIA_RED_DIAGNOSTICS_H_
