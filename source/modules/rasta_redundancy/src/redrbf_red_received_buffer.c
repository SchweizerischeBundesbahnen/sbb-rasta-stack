/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redrbf_red_received_buffer.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer received messages buffer module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redrbf_red_received_buffer.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_redundancy/redcty_red_config_types.h"
#include "rasta_redundancy/rednot_red_notifications.h"
#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Struct for redundancy layer received messages payload buffer.
 */
typedef struct {
  uint16_t read_idx;                                             ///< buffer read index (next message to read)
  uint16_t write_idx;                                            ///< buffer write index (next message to write)
  uint16_t used_elements;                                        ///< current amount of used elements in the buffer [messages]
  redtyp_RedundancyMessagePayload buffer[RADEF_MAX_N_SEND_MAX];  ///< buffer with the payload of correctly received
                                                                 ///< messages, waiting for the read from the safety and retransmission layer adapter
} ReceivedBuffer;

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of the module. True, if the module is initialized.
 */
PRIVATE bool redrbf_initialized = false;

/**
 * @brief Number of configured redundancy channels.
 */
PRIVATE uint32_t redrbf_number_of_red_channels = 0U;

/**
 * @brief Received buffers for all redundancy channels.
 */
PRIVATE ReceivedBuffer redrbf_received_buffers[RADEF_MAX_NUMBER_OF_RED_CHANNELS];

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void redrbf_Init(const uint32_t configured_red_channels) {
  // Input parameter check
  raas_AssertTrue(!redrbf_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange(configured_red_channels, redcty_kMinNumberOfRedundancyChannels, RADEF_MAX_NUMBER_OF_RED_CHANNELS, radef_kInvalidParameter);

  redrbf_number_of_red_channels = configured_red_channels;

  redrbf_initialized = true;

  // Init buffers for all configured channels
  for (uint32_t index = 0U; index < redrbf_number_of_red_channels; ++index) {
    redrbf_InitBuffer(index);
  }
}

void redrbf_InitBuffer(const uint32_t red_channel_id) {
  raas_AssertTrue(redrbf_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redrbf_number_of_red_channels - 1U, radef_kInvalidParameter);

  // Init buffer for specified connection id
  redrbf_received_buffers[red_channel_id].read_idx = 0U;
  redrbf_received_buffers[red_channel_id].write_idx = 0U;
  redrbf_received_buffers[red_channel_id].used_elements = 0U;

  // Reset payload size of every element in the buffer of this connection
  for (uint16_t index = 0U; index < RADEF_MAX_N_SEND_MAX; ++index) {
    redrbf_received_buffers[red_channel_id].buffer[index].payload_size = 0U;
  }
}

void redrbf_AddToBuffer(const uint32_t red_channel_id, const redtyp_RedundancyMessagePayload* const message_payload) {
  // Input parameter check
  raas_AssertTrue(redrbf_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redrbf_number_of_red_channels - 1U, radef_kInvalidParameter);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);
  raas_AssertU16InRange(message_payload->payload_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);

  // Add message to buffer
  if (redrbf_received_buffers[red_channel_id].used_elements < RADEF_MAX_N_SEND_MAX) {
    redtyp_RedundancyMessagePayload* const buffer_element = &redrbf_received_buffers[red_channel_id].buffer[redrbf_received_buffers[red_channel_id].write_idx];

    buffer_element->payload_size = message_payload->payload_size;
    for (uint16_t index = 0U; index < message_payload->payload_size; ++index) {
      buffer_element->payload[index] = message_payload->payload[index];
    }

    redrbf_received_buffers[red_channel_id].used_elements++;
    redrbf_received_buffers[red_channel_id].write_idx++;

    // Manage write index overflow
    if (redrbf_received_buffers[red_channel_id].write_idx == RADEF_MAX_N_SEND_MAX) {
      redrbf_received_buffers[red_channel_id].write_idx = 0U;
    }

    // Send notification to SafRetL adapter
    rednot_MessageReceivedNotification(red_channel_id);
  } else {
    rasys_FatalError(radef_kReceiveBufferFull);
  }
}

radef_RaStaReturnCode redrbf_ReadFromBuffer(const uint32_t red_channel_id, const uint16_t buffer_size, uint16_t* const message_size,
                                            uint8_t* const message_buffer) {
  // Input parameter check
  raas_AssertTrue(redrbf_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redrbf_number_of_red_channels - 1U, radef_kInvalidParameter);
  raas_AssertNotNull(message_size, radef_kInvalidParameter);
  raas_AssertNotNull(message_buffer, radef_kInvalidParameter);

  radef_RaStaReturnCode ret = radef_kNoError;

  // Read message from buffer
  if (redrbf_received_buffers[red_channel_id].used_elements > 0U) {
    const redtyp_RedundancyMessagePayload* const buffer_element =
        &redrbf_received_buffers[red_channel_id].buffer[redrbf_received_buffers[red_channel_id].read_idx];
    raas_AssertU16InRange(buffer_element->payload_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

    if (buffer_element->payload_size <= buffer_size) {  // Check external memory buffer size
      *message_size = buffer_element->payload_size;
      for (uint16_t index = 0U; index < buffer_element->payload_size; ++index) {
        message_buffer[index] = buffer_element->payload[index];
      }

      redrbf_received_buffers[red_channel_id].used_elements--;
      redrbf_received_buffers[red_channel_id].read_idx++;

      // Manage read index overflow
      if (redrbf_received_buffers[red_channel_id].read_idx == RADEF_MAX_N_SEND_MAX) {
        redrbf_received_buffers[red_channel_id].read_idx = 0U;
      }
    } else {
      // External memory buffer is too small
      ret = radef_kInvalidBufferSize;
    }
  } else {
    // Received buffer is empty
    ret = radef_kNoMessageReceived;
  }

  return ret;
}

uint16_t redrbf_GetFreeBufferEntries(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redrbf_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redrbf_number_of_red_channels - 1U, radef_kInvalidParameter);

  raas_AssertTrue(RADEF_MAX_N_SEND_MAX >= redrbf_received_buffers[red_channel_id].used_elements, radef_kInternalError);

  return (RADEF_MAX_N_SEND_MAX - redrbf_received_buffers[red_channel_id].used_elements);
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
