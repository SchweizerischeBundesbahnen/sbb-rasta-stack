/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcor_red_core.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer core module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redcor_red_core.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/rahlp_rasta_helper.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_redundancy/redcty_red_config_types.h"
#include "rasta_redundancy/redtri_transport_interface.h"
#include "reddfq_red_defer_queue.h"      // NOLINT(build/include_subdir)
#include "reddia_red_diagnostics.h"      // NOLINT(build/include_subdir)
#include "redmsg_red_messages.h"         // NOLINT(build/include_subdir)
#include "redrbf_red_received_buffer.h"  // NOLINT(build/include_subdir)
#include "redtyp_red_types.h"            // NOLINT(build/include_subdir)

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
PRIVATE bool redcor_initialized = false;

/**
 * @brief Pointer to redundancy layer configuration.
 */
PRIVATE const redcty_RedundancyLayerConfiguration *redcor_redundancy_configuration = NULL;

/**
 * @brief Process data of the redundancy channels.
 */
PRIVATE redcor_RedundancyChannelData redcor_redundancy_channels[RADEF_MAX_NUMBER_OF_RED_CHANNELS];

/**
 * @brief ID of the redundancy core debug logger.
 */
PRIVATE uint16_t redcor_logger_id;  //lint !e551 (static variable is used only in the DEBUG build for the debug logger)

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup red_core
 * @{
 */

/**
 * @brief This function delivers the messages from the defer queue to the received buffer.
 *
 * This function delivers the messages stored in the defer queue to the received buffer, as long as there is no sequence number missing. The sequence number
 * receive is updated for each delivered message.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
static void DeliverDeferQueue(const uint32_t red_channel_id);

/**
 * @brief Add a received message to the received buffer and call the DeliverDeferQueue() function.
 *
 * This function adds a in sequence received message from the input buffer to the received buffer, updates the diagnostics, increases SeqRx and calls the
 * function DeliverDeferQueue().
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
static void AddMessageToReceivedBufferAndDeliverDeferQueue(const uint32_t red_channel_id);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

static const uint32_t kSequenceNumberRangeCheckFactor = 10U;  ///< Factor for sequence number validity range check

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

bool redcor_IsConfigurationValid(const redcty_RedundancyLayerConfiguration *const redundancy_layer_configuration) {
  // Input parameter check
  raas_AssertNotNull(redundancy_layer_configuration, radef_kInvalidParameter);

  return reddia_IsConfigurationValid(redundancy_layer_configuration);
}

void redcor_Init(const redcty_RedundancyLayerConfiguration *const redundancy_layer_configuration) {
  // Input parameter check
  raas_AssertTrue(!redcor_initialized, radef_kAlreadyInitialized);
  raas_AssertNotNull(redundancy_layer_configuration, radef_kInvalidParameter);
  raas_AssertTrue(reddia_IsConfigurationValid(redundancy_layer_configuration), radef_kInvalidConfiguration);

  redcor_logger_id = ralog_INIT_LOGGER(ralog_kLogLevelNone);
  ralog_LOG_DEBUG(redcor_logger_id, "Redundancy core init ...\n");

  // Initialize local variables
  redcor_redundancy_configuration = redundancy_layer_configuration;
  redcor_initialized = true;

  // Initialize other modules
  redmsg_Init(redcor_redundancy_configuration->check_code_type);
  reddia_InitRedundancyLayerDiagnostics(redcor_redundancy_configuration);

  // Initialize internal data for all configured redundancy channels
  for (uint32_t index = 0U; index < redcor_redundancy_configuration->number_of_redundancy_channels; ++index) {
    redcor_InitRedundancyChannelData(index);
  }
}

void redcor_InitRedundancyChannelData(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Data init red. channel: %u\n", red_channel_id);

  // Initialize redundancy channel data
  redcor_redundancy_channels[red_channel_id].seq_rx = 0U;
  redcor_redundancy_channels[red_channel_id].seq_tx = 0U;
  for (uint32_t tr_channel_index = 0U; tr_channel_index < (RADEF_MAX_NUMBER_OF_RED_CHANNELS * RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS); ++tr_channel_index) {
    redcor_redundancy_channels[red_channel_id].received_data_pending[tr_channel_index] = false;
  }
  // Initialize input buffer
  redcor_redundancy_channels[red_channel_id].input_buffer.message_in_buffer = false;
  redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer.message_size = 0U;
  redcor_redundancy_channels[red_channel_id].input_buffer.transport_channel_id = 0U;
  // Initialize send buffer
  redcor_redundancy_channels[red_channel_id].send_buffer.message_in_buffer = false;
  redcor_redundancy_channels[red_channel_id].send_buffer.message_payload_buffer.payload_size = 0U;

  // Initialize other modules
  reddfq_InitDeferQueue(red_channel_id);
  redrbf_InitBuffer(red_channel_id);
  reddia_InitRedundancyChannelDiagnostics(red_channel_id);
}

void redcor_DeferQueueTimeout(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Defer queue timeout. Red. channel: %u\n", red_channel_id);

  redcor_redundancy_channels[red_channel_id].seq_rx = reddfq_GetOldestSequenceNumber(red_channel_id);
  ralog_LOG_DEBUG(redcor_logger_id, "Oldest sequence number: %u\n", redcor_redundancy_channels[red_channel_id].seq_rx);

  DeliverDeferQueue(red_channel_id);
}

void redcor_WriteReceivedMessageToInputBuffer(const uint32_t red_channel_id,
                                              const uint32_t transport_channel_id,  // needed for diagnostics
                                              const redtyp_RedundancyMessage *const received_message) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);
  raas_AssertTrue(reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id), radef_kInvalidParameter);
  raas_AssertNotNull(received_message, radef_kInvalidParameter);
  raas_AssertU16InRange(received_message->message_size, RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);
  raas_AssertTrue(!redcor_redundancy_channels[red_channel_id].input_buffer.message_in_buffer, radef_kInternalError);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u, Tr. channel: %u\n", red_channel_id, transport_channel_id);

  // Update input buffer
  redcor_redundancy_channels[red_channel_id].input_buffer.message_in_buffer = true;
  redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer.message_size = received_message->message_size;
  redcor_redundancy_channels[red_channel_id].input_buffer.transport_channel_id = transport_channel_id;
  // Copy message
  for (uint16_t copy_index = 0U; copy_index < received_message->message_size; ++copy_index) {
    redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer.message[copy_index] = received_message->message[copy_index];
  }
}

void redcor_ClearInputBufferMessagePendingFlag(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u\n", red_channel_id);

  redcor_redundancy_channels[red_channel_id].input_buffer.message_in_buffer = false;
}

void redcor_ProcessReceivedMessage(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u\n", red_channel_id);

  if (redcor_redundancy_channels[red_channel_id].input_buffer.message_in_buffer) {  // Check if a received message is in the input buffer
    // Check if the message CRC is valid, else ignore the message
    if (radef_kNoError == redmsg_CheckMessageCrc(&redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer)) {
      const uint32_t kReceivedMessageSequenceNumber = redmsg_GetMessageSequenceNumber(&redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer);
      ralog_LOG_DEBUG(redcor_logger_id, "Message sequence number: %u\n", kReceivedMessageSequenceNumber);
      // Ignore message if SeqRx == 0 and SeqTx == 0 and SeqPdu != 0
      if (!((redcor_redundancy_channels[red_channel_id].seq_rx == 0U) && (redcor_redundancy_channels[red_channel_id].seq_tx == 0U) &&
            (kReceivedMessageSequenceNumber != 0U))) {
        const uint32_t tr_channel_id = redcor_redundancy_channels[red_channel_id].input_buffer.transport_channel_id;
        // Check if SeqPdu is older than SeqRx
        if (reddfq_IsSequenceNumberOlder(kReceivedMessageSequenceNumber, redcor_redundancy_channels[red_channel_id].seq_rx)) {
          // Ignore message and update diagnostics
          ralog_LOG_DEBUG(redcor_logger_id, "SeqPdu older than SeqRx.\n");
          reddia_UpdateRedundancyChannelDiagnostics(red_channel_id, tr_channel_id, kReceivedMessageSequenceNumber);
        } else if (kReceivedMessageSequenceNumber == redcor_redundancy_channels[red_channel_id].seq_rx) {
          // Check if SeqPdu == SeqRx (message received in sequence)
          // Deliver message to upper layer
          AddMessageToReceivedBufferAndDeliverDeferQueue(red_channel_id);
        } else if (reddfq_IsSequenceNumberOlder(kReceivedMessageSequenceNumber,
                                                redcor_redundancy_channels[red_channel_id].seq_rx +
                                                    (kSequenceNumberRangeCheckFactor * redcor_redundancy_configuration->n_defer_queue_size) + 1U)) {
          // Check if SeqPdu <= (SeqRx + 10 * N_defer_queue_size), unsigned integer wrap around allowed in this calculation
          ralog_LOG_DEBUG(redcor_logger_id, "SeqPdu <= (SeqRx + 10 * N_defer_queue_size)\n");
          if (reddfq_Contains(red_channel_id, kReceivedMessageSequenceNumber)) {  // Is the message already in the defer queue?
            // Ignore the message and update diagnostics
            ralog_LOG_DEBUG(redcor_logger_id, "Message already in the defer queue.\n");
            reddia_UpdateRedundancyChannelDiagnostics(red_channel_id, tr_channel_id, kReceivedMessageSequenceNumber);
          } else {
            // Add message to the defer queue
            ralog_LOG_DEBUG(redcor_logger_id, "Add message to the defer queue.\n");
            reddfq_AddMessage(red_channel_id, &redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer);
            reddia_UpdateRedundancyChannelDiagnostics(red_channel_id, tr_channel_id, kReceivedMessageSequenceNumber);
          }
        } else {
          // Here is SeqPdu > (SeqRx + 10 * N_defer_queue_size)
          // Ignore the message
          ralog_LOG_DEBUG(redcor_logger_id, "SeqPdu > (SeqRx + 10 * N_defer_queue_size)\n");
        }
      }
    }
  } else {
    ralog_LOG_DEBUG(redcor_logger_id, "No message received.\n");
    rasys_FatalError(radef_kNoMessageReceived);
  }
  // Clear input buffer message pending flag
  redcor_redundancy_channels[red_channel_id].input_buffer.message_in_buffer = false;
}

void redcor_SetMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);
  raas_AssertTrue(reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id), radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u, Tr. channel: %u\n", red_channel_id, transport_channel_id);

  redcor_redundancy_channels[red_channel_id].received_data_pending[transport_channel_id] = true;
}

bool redcor_GetMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);
  raas_AssertTrue(reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id), radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u, Tr. channel: %u\n", red_channel_id, transport_channel_id);

  return redcor_redundancy_channels[red_channel_id].received_data_pending[transport_channel_id];
}

void redcor_ClearMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);
  raas_AssertTrue(reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id), radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u, Tr. channel: %u\n", red_channel_id, transport_channel_id);

  redcor_redundancy_channels[red_channel_id].received_data_pending[transport_channel_id] = false;
}

void redcor_WriteMessagePayloadToSendBuffer(const uint32_t red_channel_id, const uint16_t payload_size, const uint8_t *const payload_data) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);
  raas_AssertNotNull(payload_data, radef_kInvalidParameter);
  raas_AssertU16InRange(payload_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);
  raas_AssertTrue(!redcor_redundancy_channels[red_channel_id].send_buffer.message_in_buffer, radef_kSendBufferFull);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u\n", red_channel_id);

  // Copy message payload to send buffer an set message pending flag
  redcor_redundancy_channels[red_channel_id].send_buffer.message_payload_buffer.payload_size = payload_size;
  for (uint16_t index = 0U; index < payload_size; ++index) {
    redcor_redundancy_channels[red_channel_id].send_buffer.message_payload_buffer.payload[index] = payload_data[index];
  }
  redcor_redundancy_channels[red_channel_id].send_buffer.message_in_buffer = true;
}

void redcor_ClearSendBufferMessagePendingFlag(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u\n", red_channel_id);

  redcor_redundancy_channels[red_channel_id].send_buffer.message_in_buffer = false;
}

void redcor_SendMessage(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInvalidParameter);
  raas_AssertTrue(redcor_redundancy_channels[red_channel_id].send_buffer.message_in_buffer, radef_kNoMessageToSend);

  ralog_LOG_DEBUG(redcor_logger_id, "Red. channel: %u, SeqTx: %u\n", red_channel_id, redcor_redundancy_channels[red_channel_id].seq_tx);

  // Create redundancy layer message
  redtyp_RedundancyMessage redundancy_message_to_send;
  redmsg_CreateMessage(redcor_redundancy_channels[red_channel_id].seq_tx, &redcor_redundancy_channels[red_channel_id].send_buffer.message_payload_buffer,
                       &redundancy_message_to_send);

  // Send message on all transport channels associated to this redundancy channel
  for (uint32_t tr_channel_index = 0U;
       tr_channel_index < redcor_redundancy_configuration->redundancy_channel_configurations[red_channel_id].num_transport_channels; ++tr_channel_index) {
    redtri_SendMessage(redcor_redundancy_configuration->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index],
                       redundancy_message_to_send.message_size, &redundancy_message_to_send.message[0U]);
    ralog_LOG_DEBUG(redcor_logger_id, "Send message tr. channel: %u\n",
                    redcor_redundancy_configuration->redundancy_channel_configurations[red_channel_id].transport_channel_ids[tr_channel_index]);
  }

  ++redcor_redundancy_channels[red_channel_id].seq_tx;  // Unsigned integer wrap around allowed here

  // Clear send buffer message pending flag
  redcor_redundancy_channels[red_channel_id].send_buffer.message_in_buffer = false;
}

void redcor_GetAssociatedRedundancyChannel(const uint32_t transport_channel_id, uint32_t *const red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redcor_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(transport_channel_id, 0U, (RADEF_MAX_NUMBER_OF_RED_CHANNELS * RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS) - 1U, radef_kInvalidParameter);
  raas_AssertNotNull(red_channel_id, radef_kInvalidParameter);

  uint32_t red_channel_search_index = 0U;
  bool red_channel_found = false;

  while ((red_channel_search_index < redcor_redundancy_configuration->number_of_redundancy_channels) && !red_channel_found) {
    uint32_t tr_channel_search_index = 0U;
    while ((tr_channel_search_index < redcor_redundancy_configuration->redundancy_channel_configurations[red_channel_search_index].num_transport_channels) &&
           !red_channel_found) {
      if (transport_channel_id ==
          redcor_redundancy_configuration->redundancy_channel_configurations[red_channel_search_index].transport_channel_ids[tr_channel_search_index]) {
        red_channel_found = true;
        *red_channel_id = red_channel_search_index;
      }
      ++tr_channel_search_index;
    }
    ++red_channel_search_index;
  }
  raas_AssertTrue(red_channel_found, radef_kInvalidParameter);
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static void DeliverDeferQueue(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);

  // Deliver defer queue as long as messages are in sequence
  while (reddfq_Contains(red_channel_id, redcor_redundancy_channels[red_channel_id].seq_rx)) {
    redtyp_RedundancyMessage redundancy_message;
    redtyp_RedundancyMessagePayload redundancy_message_payload;

    // Get message from defer queue
    reddfq_GetMessage(red_channel_id, redcor_redundancy_channels[red_channel_id].seq_rx, &redundancy_message);
    redmsg_GetMessagePayload(&redundancy_message, &redundancy_message_payload);

    // Put message in received buffer
    redrbf_AddToBuffer(red_channel_id, &redundancy_message_payload);
    ralog_LOG_DEBUG(redcor_logger_id, "Add message to received buffer. SeqRx: %u\n", redcor_redundancy_channels[red_channel_id].seq_rx);

    ++redcor_redundancy_channels[red_channel_id].seq_rx;  // Unsigned integer wrap around allowed here
  }
}

static void AddMessageToReceivedBufferAndDeliverDeferQueue(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, redcor_redundancy_configuration->number_of_redundancy_channels - 1U, radef_kInternalError);

  ralog_LOG_DEBUG(redcor_logger_id, "SeqPdu == SeqRx. Add message to received buffer.\n");
  redtyp_RedundancyMessagePayload received_message_payload;
  redmsg_GetMessagePayload(&redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer, &received_message_payload);
  redrbf_AddToBuffer(red_channel_id, &received_message_payload);
  reddia_UpdateRedundancyChannelDiagnostics(red_channel_id, redcor_redundancy_channels[red_channel_id].input_buffer.transport_channel_id,
                                            redmsg_GetMessageSequenceNumber(&redcor_redundancy_channels[red_channel_id].input_buffer.message_buffer));
  ++redcor_redundancy_channels[red_channel_id].seq_rx;  // Unsigned integer wrap around allowed here
  DeliverDeferQueue(red_channel_id);
}
