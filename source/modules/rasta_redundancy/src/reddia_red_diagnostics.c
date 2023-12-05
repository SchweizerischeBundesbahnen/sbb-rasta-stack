/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddia_red_diagnostics.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer diagnostics.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "reddia_red_diagnostics.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/rahlp_rasta_helper.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_redundancy/redcty_red_config_types.h"
#include "rasta_redundancy/rednot_red_notifications.h"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of the module. True, if the module is initialized.
 */
PRIVATE bool reddia_initialized = false;

/**
 * @brief Pointer to redundancy layer configuration.
 */
PRIVATE const redcty_RedundancyLayerConfiguration* reddia_redundancy_configuration = NULL;

/**
 * @brief Timestamps of earliest received messages.
 */
PRIVATE reddia_ReceivedMessageTimestamp reddia_received_messages_timestamps[RADEF_MAX_NUMBER_OF_RED_CHANNELS][RADEF_MAX_RED_LAYER_N_DIAGNOSIS];

/**
 * @brief Current number of messages in the current diagnosis window.
 */
PRIVATE uint32_t reddia_current_n_diagnosis[RADEF_MAX_NUMBER_OF_RED_CHANNELS];

/**
 * @brief Diagnostic data of all transport channels.
 */
PRIVATE radef_TransportChannelDiagnosticData reddia_transport_channel_diagnostic_data[RADEF_MAX_NUMBER_OF_RED_CHANNELS][RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS];

/**
 * @brief ID of the redundancy diagnostics debug logger.
 */
PRIVATE uint16_t reddia_logger_id;  //lint !e551 (static variable is used only in the DEBUG build for the debug logger)

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup red_diagnostics
 * @{
 */

/**
 * @brief Get the transport channel index of a specific transport channel of a redundancy channel.
 *
 * The function returns the index of the transport channel in the redundancy channel configuration structure of a specific redundancy channel.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInternalError fatal error is thrown.
 * @return The index of the transport channel in the redundancy channel configuration structure of a specific redundancy channel.
 */
uint32_t GetTransportChannelIndex(const uint32_t red_channel_id, const uint32_t transport_channel_id);

/**
 * @brief Returns true, if a received message timestamp of a message with the given sequence number is already stored. If so, it updates the diagnostic data of
 * the transport channel of the newly received message.
 *
 * First, this function checks if a received message timestamp of a message with the given sequence number is already stored in
 * ::reddia_received_messages_timestamps. If so, ::radef_TransportChannelDiagnosticData.t_drift, ::radef_TransportChannelDiagnosticData.t_drift2 and the
 * ::reddia_ReceivedMessageTimestamp.message_received_flag of the given transport channel are updated.
 *
 * @pre The ::reddia_current_n_diagnosis of the given redundancy channel must be smaller or equal to the configured n_diagnosis, else a ::radef_kInternalError
 * fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInternalError fatal error is thrown.
 * @param [in] message_sequence_number Newly received message sequence number. The full value range is valid and usable.
 * @param current_time_stamp Current timestamp [ms]. The full value range is valid and usable.
 * @return true, if a message with the same sequence number was already received before
 * @return false, if no message with the given sequence number was received before
 */
static bool IsSequenceNumberAlreadyReceivedUpdateDiagnosticData(const uint32_t red_channel_id, const uint32_t transport_channel_id,
                                                                const uint32_t message_sequence_number, const uint32_t current_time_stamp);

/**
 * @brief Add the diagnostic data of a message with a first time received sequence number and trigger the diagnostic notifications, if the diagnostic window is
 * reached.
 *
 * This Function checks, if the configured diagnosis window is reached for the given redundancy channel. If so, the diagnostic data is collected and the
 * diagnostic notifications are triggered for all associated transport channels by calling rednot_DiagnosticNotification() and subsequently the diagnostic data
 * of this redundancy channel is reset by calling ::reddia_InitRedundancyChannelDiagnostics(). Finally the timestamp, sequence number, transport channel id are
 * stored in ::reddia_received_messages_timestamps and the ::reddia_current_n_diagnosis of this transport channel is increased.
 * If ::reddia_current_n_diagnosis of the given redundancy channel is greater than n_diagnosis a ::radef_kInternalError fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInternalError fatal error is thrown.
 * @param [in] message_sequence_number Newly received message sequence number. The full value range is valid and usable.
 * @param current_time_stamp Current timestamp [ms]. The full value range is valid and usable.
 */
static void AddFirstTimeReceivedMessageDiagnosticData(const uint32_t red_channel_id, const uint32_t transport_channel_id,
                                                      const uint32_t message_sequence_number, const uint32_t current_time_stamp);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void reddia_InitRedundancyLayerDiagnostics(const redcty_RedundancyLayerConfiguration* const redundancy_layer_configuration) {
  // Input parameter check
  raas_AssertTrue(!reddia_initialized, radef_kAlreadyInitialized);
  raas_AssertNotNull(redundancy_layer_configuration, radef_kInvalidParameter);
  raas_AssertTrue(reddia_IsConfigurationValid(redundancy_layer_configuration), radef_kInvalidConfiguration);

  reddia_logger_id = ralog_INIT_LOGGER(ralog_kLogLevelNone);
  ralog_LOG_DEBUG(reddia_logger_id, "Redundancy layer diagnostics init ...\n");

  // Initialize local variables
  reddia_redundancy_configuration = redundancy_layer_configuration;
  reddia_initialized = true;

  // Initialize diagnostic data for all configured redundancy channels
  for (uint32_t index = 0U; index < reddia_redundancy_configuration->number_of_redundancy_channels; ++index) {
    reddia_InitRedundancyChannelDiagnostics(index);
  }
}

void reddia_InitRedundancyChannelDiagnostics(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(reddia_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddia_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(reddia_logger_id, "Data init red. channel: %u\n", red_channel_id);

  // Initialize current messages counter
  reddia_current_n_diagnosis[red_channel_id] = 0U;

  // Initialize timestamps data of first received messages
  for (uint32_t diagnosis_index = 0U; diagnosis_index < reddia_redundancy_configuration->n_diagnosis; ++diagnosis_index) {
    reddia_received_messages_timestamps[red_channel_id][diagnosis_index].sequence_number = 0U;
    reddia_received_messages_timestamps[red_channel_id][diagnosis_index].received_time_stamp = 0U;
    for (uint32_t tr_channel_index = 0U;
         tr_channel_index < reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels; ++tr_channel_index) {
      reddia_received_messages_timestamps[red_channel_id][diagnosis_index].message_received_flag[tr_channel_index] = false;
    }
  }

  // Initialize transport channels diagnostic data
  for (uint32_t tr_channel_index = 0U;
       tr_channel_index < reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels; ++tr_channel_index) {
    reddia_transport_channel_diagnostic_data[red_channel_id][tr_channel_index].n_diagnosis = 0U;
    reddia_transport_channel_diagnostic_data[red_channel_id][tr_channel_index].n_missed = 0U;
    reddia_transport_channel_diagnostic_data[red_channel_id][tr_channel_index].t_drift = 0U;
    reddia_transport_channel_diagnostic_data[red_channel_id][tr_channel_index].t_drift2 = 0U;
  }
}

void reddia_UpdateRedundancyChannelDiagnostics(const uint32_t red_channel_id, const uint32_t transport_channel_id, const uint32_t message_sequence_number) {
  // Input parameter check
  raas_AssertTrue(reddia_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddia_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);
  raas_AssertTrue(reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id), radef_kInvalidParameter);

  const uint32_t kCurrentTime = rasys_GetTimerValue();

  if (!IsSequenceNumberAlreadyReceivedUpdateDiagnosticData(red_channel_id, transport_channel_id, message_sequence_number, kCurrentTime)) {
    // Message with not yet received sequence number
    AddFirstTimeReceivedMessageDiagnosticData(red_channel_id, transport_channel_id, message_sequence_number, kCurrentTime);
  }
}

bool reddia_IsConfigurationValid(const redcty_RedundancyLayerConfiguration* const redundancy_layer_configuration) {
  // Input parameter check
  raas_AssertNotNull(redundancy_layer_configuration, radef_kInvalidParameter);

  // Configuration check
  bool configuration_valid = true;
  if (!rahlp_IsU16InRange((uint16_t)redundancy_layer_configuration->check_code_type, (uint16_t)redcty_kCheckCodeMin, (uint16_t)(redcty_kCheckCodeMax)-1U)) {
    configuration_valid = false;
  } else if (!rahlp_IsU32InRange(redundancy_layer_configuration->t_seq, redcty_kMinTSeq, redcty_kMaxTSeq)) {
    configuration_valid = false;
  } else if (!rahlp_IsU32InRange(redundancy_layer_configuration->n_diagnosis, redcty_kMinRedLayerNDiagnosis, RADEF_MAX_RED_LAYER_N_DIAGNOSIS)) {
    configuration_valid = false;
  } else if (!rahlp_IsU32InRange(redundancy_layer_configuration->n_defer_queue_size, redcty_kMinDeferQueueSize, RADEF_MAX_DEFER_QUEUE_SIZE)) {
    configuration_valid = false;
  } else if (!rahlp_IsU32InRange(redundancy_layer_configuration->number_of_redundancy_channels, redcty_kMinNumberOfRedundancyChannels,
                                 RADEF_MAX_NUMBER_OF_RED_CHANNELS)) {
    configuration_valid = false;
  } else {
    for (uint32_t red_channel_index = 0U; red_channel_index < redundancy_layer_configuration->number_of_redundancy_channels; ++red_channel_index) {
      // Check if redundancy cannel id is equal to redundancy channel index
      if (redundancy_layer_configuration->redundancy_channel_configurations[red_channel_index].red_channel_id != red_channel_index) {
        configuration_valid = false;
        // Check number of transport channels per redundancy channel
      } else if (!rahlp_IsU32InRange(redundancy_layer_configuration->redundancy_channel_configurations[red_channel_index].num_transport_channels,
                                     redcty_kMinNumberOfTransportChannels, RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS)) {
        configuration_valid = false;
      } else {
        // Check if transport channel IDs are in the valid range
        for (uint32_t tr_channel_index = 0U;
             tr_channel_index < redundancy_layer_configuration->redundancy_channel_configurations[red_channel_index].num_transport_channels;
             ++tr_channel_index) {
          if (!rahlp_IsU32InRange(redundancy_layer_configuration->redundancy_channel_configurations[red_channel_index].transport_channel_ids[tr_channel_index],
                                  0U, (RADEF_MAX_NUMBER_OF_RED_CHANNELS * RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS) - 1U)) {
            configuration_valid = false;
          }
        }
      }
    }
  }

  return configuration_valid;
}

bool reddia_IsTransportChannelIdValid(const uint32_t red_channel_id, const uint32_t transport_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, reddia_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);

  // Check if the transport channel id is found in the configuration of this redundancy channel
  bool transport_channel_id_valid = false;
  for (uint32_t tr_channel_index = 0U;
       tr_channel_index < reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels; ++tr_channel_index) {
    if (transport_channel_id == reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index]) {
      transport_channel_id_valid = true;
    }
  }

  return transport_channel_id_valid;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

uint32_t GetTransportChannelIndex(const uint32_t red_channel_id, const uint32_t transport_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, reddia_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);

  uint32_t tr_channel_index = 0U;
  bool tr_channel_id_found = false;

  while ((tr_channel_index < reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels) &&
         !tr_channel_id_found) {
    if (transport_channel_id == reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index]) {
      tr_channel_id_found = true;
    } else {
      ++tr_channel_index;
    }
  }

  raas_AssertTrue(tr_channel_id_found, radef_kInternalError);

  return tr_channel_index;
}

static bool IsSequenceNumberAlreadyReceivedUpdateDiagnosticData(const uint32_t red_channel_id, const uint32_t transport_channel_id,
                                                                const uint32_t message_sequence_number, const uint32_t current_time_stamp) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, reddia_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);
  raas_AssertTrue(reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id), radef_kInternalError);
  raas_AssertTrue(reddia_current_n_diagnosis[red_channel_id] <= reddia_redundancy_configuration->n_diagnosis, radef_kInternalError);

  uint32_t timestamp_loop = 0U;
  bool sequence_number_found = false;

  // Search message sequence number in stored timestamps
  while ((timestamp_loop < reddia_current_n_diagnosis[red_channel_id]) && !sequence_number_found) {
    // Start search with the newest message
    uint32_t timestamp_index = (reddia_current_n_diagnosis[red_channel_id] - 1U) - timestamp_loop;
    if (message_sequence_number == reddia_received_messages_timestamps[red_channel_id][timestamp_index].sequence_number) {
      // Message sequence number found
      sequence_number_found = true;
      // Update diagnostic data of the current message received transport channel
      const uint32_t kDeltaTDrift =
          current_time_stamp -
          reddia_received_messages_timestamps[red_channel_id][timestamp_index].received_time_stamp;  // Unsigned integer overflow allowed in this calculation

      ralog_LOG_DEBUG(reddia_logger_id, "Red. channel: %u, Tr. channel: %u, Seq: %u, delta_t_drift: %u\n", red_channel_id, transport_channel_id,
                      message_sequence_number, kDeltaTDrift);

      if (kDeltaTDrift <= reddia_redundancy_configuration->t_seq) {
        const uint32_t kTrChannelIndex = GetTransportChannelIndex(red_channel_id, transport_channel_id);
        // Message delay <= Tseq -> calculate and update Tdrift and Tdrift^2
        reddia_transport_channel_diagnostic_data[red_channel_id][kTrChannelIndex].t_drift += kDeltaTDrift;
        reddia_transport_channel_diagnostic_data[red_channel_id][kTrChannelIndex].t_drift2 += (kDeltaTDrift * kDeltaTDrift);
        reddia_received_messages_timestamps[red_channel_id][timestamp_index].message_received_flag[kTrChannelIndex] = true;
      }
    } else {
      ++timestamp_loop;
    }
  }
  return sequence_number_found;
}

static void AddFirstTimeReceivedMessageDiagnosticData(const uint32_t red_channel_id, const uint32_t transport_channel_id,
                                                      const uint32_t message_sequence_number, const uint32_t current_time_stamp) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, reddia_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);
  raas_AssertTrue(reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id), radef_kInternalError);
  raas_AssertTrue(reddia_current_n_diagnosis[red_channel_id] <= reddia_redundancy_configuration->n_diagnosis, radef_kInternalError);

  if (reddia_current_n_diagnosis[red_channel_id] >= reddia_redundancy_configuration->n_diagnosis) {
    // Redundancy channel diagnostic window reached, loop over for all associated transport channels
    for (uint32_t tr_channel_index = 0U;
         tr_channel_index < reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels; ++tr_channel_index) {
      // Prepare diagnostic notification data

      // Count and update n_missed from timestamps data structure
      for (uint32_t diagnosis_index = 0U; diagnosis_index < reddia_redundancy_configuration->n_diagnosis; ++diagnosis_index) {
        if (!reddia_received_messages_timestamps[red_channel_id][diagnosis_index].message_received_flag[tr_channel_index]) {
          ++reddia_transport_channel_diagnostic_data[red_channel_id][tr_channel_index].n_missed;
        }
      }

      // Update n_diagnosis
      reddia_transport_channel_diagnostic_data[red_channel_id][tr_channel_index].n_diagnosis = reddia_current_n_diagnosis[red_channel_id];

      // Send diagnostic notifications
      const uint32_t kTrChannelId = reddia_redundancy_configuration->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index];
      rednot_DiagnosticNotification(red_channel_id, kTrChannelId, reddia_transport_channel_diagnostic_data[red_channel_id][tr_channel_index]);
      ralog_LOG_DEBUG(reddia_logger_id, "Send diag. notification: Red. channel: %u, Tr. channel: %u\n", red_channel_id, transport_channel_id);
    }
    // Clear diagnostic data of this redundancy channel
    reddia_InitRedundancyChannelDiagnostics(red_channel_id);
  }
  // Add first transport channel received message received timestamp
  ralog_LOG_DEBUG(reddia_logger_id, "Add message: Red. channel: %u, Tr. channel: %u, Seq: %u\n", red_channel_id, transport_channel_id, message_sequence_number);

  reddia_received_messages_timestamps[red_channel_id][reddia_current_n_diagnosis[red_channel_id]].received_time_stamp = current_time_stamp;
  reddia_received_messages_timestamps[red_channel_id][reddia_current_n_diagnosis[red_channel_id]].sequence_number = message_sequence_number;
  reddia_received_messages_timestamps[red_channel_id][reddia_current_n_diagnosis[red_channel_id]]
      .message_received_flag[GetTransportChannelIndex(red_channel_id, transport_channel_id)] = true;
  // Increase number of messages in this diagnosis window
  ++reddia_current_n_diagnosis[red_channel_id];
}
