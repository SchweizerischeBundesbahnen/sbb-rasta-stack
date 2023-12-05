/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redint_red_interface.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer interface.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_redundancy/redint_red_interface.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/rahlp_rasta_helper.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_redundancy/redcty_red_config_types.h"
#include "rasta_redundancy/redtri_transport_interface.h"
#include "redcor_red_core.h"             // NOLINT(build/include_subdir)
#include "reddfq_red_defer_queue.h"      // NOLINT(build/include_subdir)
#include "redrbf_red_received_buffer.h"  // NOLINT(build/include_subdir)
#include "redstm_red_state_machine.h"    // NOLINT(build/include_subdir)
#include "redtyp_red_types.h"            // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of this module.
 */
PRIVATE radef_RaStaReturnCode redint_initialization_state = radef_kNotInitialized;

/**
 * @brief Pointer to redundancy layer configuration.
 */
PRIVATE const redcty_RedundancyLayerConfiguration* redint_redundancy_configuration = NULL;

/**
 * @brief ID of the redundancy core debug logger.
 */
PRIVATE uint16_t redint_logger_id;  //lint !e551 (static variable is used only in the DEBUG build for the debug logger)

/**
 * @brief Transport channel indices for received message polling.
 *
 * tr_channel_polling_read_indices is used to make sure, that the received message polling continues on the next transport channel, after a polling
 * interruption due to low received buffer capacity. It prevents from restarting the received message polling always with the first transport channel, because
 * this could lead to never reading messages form the other transport channels.
 */
PRIVATE uint32_t tr_channel_polling_read_indices[RADEF_MAX_NUMBER_OF_RED_CHANNELS];

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/**
 * @brief Returns true, if a received message is pending on any transport channel of a given redundancy channel.
 *
 * This function reads and evaluates the received message pending flags of all transport channels associated to the given redundancy channel and returns
 * true, if a received message is pending on any of these transport channels.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @return true, if a message is pending on this redundancy channel
 * @return false, if no message is pending on this redundancy channel
 */
static bool IsMessagePending(const uint32_t red_channel_id);

/**
 * @brief Polls received messages from the transport channels, copies messages to the input buffer and triggers the state machine for received message
 * processing.
 *
 * This function polls messages from the transport channels. If the received message pending flag is set and the number of free received buffer entries is
 * greater than the number of used defer queue entries, the message is read from the transport channel and copied to the input buffer. Afterwards the state
 * machine is triggered for received message processing.
 * After a polling interruption due to low received buffer capacity, the received message polling continues on the next transport channel. This prevents from
 * restarting the received message polling always with the first transport channel, because this could lead to never reading messages form the other transport
 * channels.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
static void ReceivedMessagesPolling(const uint32_t red_channel_id);

/**
 * @brief Discards all received messages from the transport channels of a given redundancy channel.
 *
 * This function discards all received messages from the transport channels of a given redundancy channel. While the received message pending flag of a
 * transport channel is set, the messages are read from this transport channel and they are discarded. If all messages of a transport channel are read, the
 * received messages pending flag of this transport channel is cleared and the reading of messages is started on the next configured transport channel of the
 * given redundancy channel, until all messages of all associated transport channels are read and discarded.
 * This function is used to discard all received messages of a redundancy channel, while this redundancy channel is in closed state.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
static void DiscardMessages(const uint32_t red_channel_id);

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

radef_RaStaReturnCode redint_Init(const redcty_RedundancyLayerConfiguration* const redundancy_layer_configuration) {
  radef_RaStaReturnCode return_code = radef_kNoError;

  if (redint_initialization_state == radef_kNoError) {
    return_code = radef_kAlreadyInitialized;
  } else if (redundancy_layer_configuration == NULL) {
    return_code = radef_kInvalidParameter;
  } else if (!redcor_IsConfigurationValid(redundancy_layer_configuration)) {
    return_code = radef_kInvalidConfiguration;
  } else {
    redint_logger_id = ralog_INIT_LOGGER(ralog_kLogLevelNone);
    ralog_LOG_DEBUG(redint_logger_id, "Redundancy interface init ...\n");

    // Initialize redundancy interface module
    redint_redundancy_configuration = redundancy_layer_configuration;

    // Initialize internal data
    for (uint32_t red_channel_index = 0U; red_channel_index < redint_redundancy_configuration->number_of_redundancy_channels; ++red_channel_index) {
      tr_channel_polling_read_indices[red_channel_index] = 0U;
    }

    // Initialize other modules
    redstm_Init(redint_redundancy_configuration->number_of_redundancy_channels);
    reddfq_Init(redint_redundancy_configuration->number_of_redundancy_channels, redint_redundancy_configuration->n_defer_queue_size,
                redint_redundancy_configuration->t_seq);
    redrbf_Init(redint_redundancy_configuration->number_of_redundancy_channels);
    redcor_Init(redint_redundancy_configuration);

    redint_initialization_state = radef_kNoError;
  }

  return return_code;
}

radef_RaStaReturnCode redint_GetInitializationState(void) {
  return redint_initialization_state;
}

radef_RaStaReturnCode redint_OpenRedundancyChannel(const uint32_t redundancy_channel_id) {
  radef_RaStaReturnCode return_code = radef_kNoError;

  if (redint_initialization_state == radef_kNoError) {
    if (rahlp_IsU32InRange(redundancy_channel_id, 0U, redint_redundancy_configuration->number_of_redundancy_channels - 1U)) {
      ralog_LOG_DEBUG(redint_logger_id, "Open red. channel: %u\n", redundancy_channel_id);

      tr_channel_polling_read_indices[redundancy_channel_id] = 0U;
      redstm_ProcessChannelStateMachine(redundancy_channel_id, redstm_kRedundancyChannelEventOpen);
    } else {
      return_code = radef_kInvalidParameter;
    }
  } else {
    return_code = radef_kNotInitialized;
  }

  return return_code;
}

radef_RaStaReturnCode redint_CloseRedundancyChannel(const uint32_t redundancy_channel_id) {
  radef_RaStaReturnCode return_code = radef_kNoError;

  if (redint_initialization_state == radef_kNoError) {
    if (rahlp_IsU32InRange(redundancy_channel_id, 0U, redint_redundancy_configuration->number_of_redundancy_channels - 1U)) {
      ralog_LOG_DEBUG(redint_logger_id, "Close red. channel: %u\n", redundancy_channel_id);

      redstm_ProcessChannelStateMachine(redundancy_channel_id, redstm_kRedundancyChannelEventClose);
    } else {
      return_code = radef_kInvalidParameter;
    }
  } else {
    return_code = radef_kNotInitialized;
  }

  return return_code;
}

radef_RaStaReturnCode redint_SendMessage(const uint32_t redundancy_channel_id, const uint16_t message_size, const uint8_t* const message_data) {
  radef_RaStaReturnCode return_code = radef_kNoError;

  if (redint_initialization_state != radef_kNoError) {
    return_code = radef_kNotInitialized;
  } else if (redstm_GetChannelState(redundancy_channel_id) != redstm_kRedundancyChannelStateUp) {
    return_code = radef_kInvalidOperationInCurrentState;
  } else if (!rahlp_IsU32InRange(redundancy_channel_id, 0U, redint_redundancy_configuration->number_of_redundancy_channels - 1U) || (message_data == NULL)) {
    return_code = radef_kInvalidParameter;
  } else if (!rahlp_IsU32InRange(message_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE)) {
    return_code = radef_kInvalidMessageSize;
  } else {
    ralog_LOG_DEBUG(redint_logger_id, "Send message red. channel: %u\n", redundancy_channel_id);

    redcor_WriteMessagePayloadToSendBuffer(redundancy_channel_id, message_size, message_data);
    redstm_ProcessChannelStateMachine(redundancy_channel_id, redstm_kRedundancyChannelEventSendData);
    // Clear send buffer message pending flag here, in case the state machine decided to not process the message
    redcor_ClearSendBufferMessagePendingFlag(redundancy_channel_id);
  }

  return return_code;
}

radef_RaStaReturnCode redint_ReadMessage(const uint32_t redundancy_channel_id, const uint16_t buffer_size, uint16_t* const message_size,
                                         uint8_t* const message_buffer) {
  radef_RaStaReturnCode return_code = radef_kNoError;

  if (redint_initialization_state != radef_kNoError) {
    return_code = radef_kNotInitialized;
  } else if (!rahlp_IsU16InRange(buffer_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE)) {
    return_code = radef_kInvalidParameter;
  } else if (rahlp_IsU32InRange(redundancy_channel_id, 0U, redint_redundancy_configuration->number_of_redundancy_channels - 1U) && (message_size != NULL) &&
             (message_buffer != NULL)) {
    return_code = redrbf_ReadFromBuffer(redundancy_channel_id, buffer_size, message_size, message_buffer);

    ralog_LOG_DEBUG(redint_logger_id, "Read message red. channel: %u, return_code %u\n", redundancy_channel_id, return_code);
  } else {
    return_code = radef_kInvalidParameter;
  }

  return return_code;
}

radef_RaStaReturnCode redint_CheckTimings(void) {
  radef_RaStaReturnCode return_code = radef_kNoError;

  if (redint_initialization_state == radef_kNoError) {
    // Loop over all configured redundancy channels
    for (uint32_t red_channel_id = 0U; red_channel_id < redint_redundancy_configuration->number_of_redundancy_channels; ++red_channel_id) {
      if (redstm_GetChannelState(red_channel_id) == redstm_kRedundancyChannelStateUp) {
        // Received messages polling
        ReceivedMessagesPolling(red_channel_id);
        // Check defer queue timeout
        if (reddfq_IsTimeout(red_channel_id)) {
          ralog_LOG_DEBUG(redint_logger_id, "Defer queue timeout red. channel: %u\n", red_channel_id);

          redstm_ProcessChannelStateMachine(red_channel_id, redstm_kRedundancyChannelEventDeferTimeout);
        }
      } else {
        // Discard all received messages, while the redundancy channel is not in up state
        DiscardMessages(red_channel_id);
      }
    }
  } else {
    return_code = radef_kNotInitialized;
  }
  return return_code;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static bool IsMessagePending(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, redint_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);

  bool message_pending = false;

  // Loop over all associated transport channels
  for (uint32_t tr_channel_index = 0U;
       tr_channel_index < redint_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels; ++tr_channel_index) {
    const uint32_t kTrChannelId = redint_redundancy_configuration->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index];
    if (redcor_GetMessagePendingFlag(red_channel_id, kTrChannelId)) {
      message_pending = true;
    }
  }
  return message_pending;
}

static void ReceivedMessagesPolling(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, redint_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);

  uint16_t free_received_buffer_entries = redrbf_GetFreeBufferEntries(red_channel_id);
  uint32_t used_defer_queue_entries = reddfq_GetUsedEntries(red_channel_id);
  bool is_message_pending = IsMessagePending(red_channel_id);

  while ((free_received_buffer_entries > used_defer_queue_entries) && is_message_pending) {
    const uint32_t kTrChannelId = redint_redundancy_configuration->redundancy_channel_configurations[red_channel_id]
                                      .transport_channel_ids[tr_channel_polling_read_indices[red_channel_id]];
    if (redcor_GetMessagePendingFlag(red_channel_id, kTrChannelId)) {
      redtyp_RedundancyMessage received_message;
      if (radef_kNoError == redtri_ReadMessage(kTrChannelId, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, &received_message.message_size, received_message.message)) {
        // Message received
        ralog_LOG_DEBUG(redint_logger_id, "Message received red. channel: %u, tr. channel: %u\n", red_channel_id, kTrChannelId);

        redcor_WriteReceivedMessageToInputBuffer(red_channel_id, kTrChannelId, &received_message);
        redstm_ProcessChannelStateMachine(red_channel_id, redstm_kRedundancyChannelEventReceiveData);
        // Clear input buffer message pending flag here, in case the state machine decided to not process the message
        redcor_ClearInputBufferMessagePendingFlag(red_channel_id);
      } else {
        // No more messages pending on this transport channel
        redcor_ClearMessagePendingFlag(red_channel_id, kTrChannelId);
      }
    }

    ++tr_channel_polling_read_indices[red_channel_id];
    // Handle index overflow
    if (tr_channel_polling_read_indices[red_channel_id] >=
        redint_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels) {
      tr_channel_polling_read_indices[red_channel_id] = 0U;
    }

    // Update variables for loop condition
    free_received_buffer_entries = redrbf_GetFreeBufferEntries(red_channel_id);
    used_defer_queue_entries = reddfq_GetUsedEntries(red_channel_id);
    is_message_pending = IsMessagePending(red_channel_id);
  }
}

static void DiscardMessages(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, redint_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);

  uint32_t tr_channel_index = 0U;

  while (IsMessagePending(red_channel_id) &&
         (tr_channel_index < redint_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels)) {
    const uint32_t kTrChannelId = redint_redundancy_configuration->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index];
    redtyp_RedundancyMessage received_message;
    while (radef_kNoError == redtri_ReadMessage(kTrChannelId, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, &received_message.message_size, received_message.message)) {
      // nothing to do, messages are discarded
    }
    redcor_ClearMessagePendingFlag(red_channel_id, kTrChannelId);
    ++tr_channel_index;
  }
}
