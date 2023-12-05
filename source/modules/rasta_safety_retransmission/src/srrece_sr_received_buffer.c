/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srrece_sr_received_buffer.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA SafRetL received buffer module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srrece_sr_received_buffer.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "rasta_safety_retransmission/srnot_sr_notifications.h"
#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Struct for SafRetL received messages payload buffer.
 */
typedef struct {
  uint16_t read_idx;       ///< buffer read index (next message to read)
  uint16_t write_idx;      ///< buffer write index (next message to write)
  uint16_t used_elements;  ///< current amount of used elements in the buffer
  srtyp_SrMessagePayload
      buffer[RADEF_MAX_N_SEND_MAX];  ///< buffer with the payload of correctly received messages, waiting for the read from the application layer
} SrReceivedBuffer;

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of the module.
 * true, if the module is initialized.
 */
PRIVATE bool srrece_initialized = false;

/**
 * @brief Number of configured RaSTA connections.
 */
PRIVATE uint32_t srrece_number_of_connections = 0U;

/**
 * @brief Configured receive buffer size [messages].
 */
PRIVATE uint16_t srrece_n_send_max = 0U;

/**
 * @brief Received buffers for all RaSTA connections.
 */
PRIVATE SrReceivedBuffer srrece_received_buffers[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup sr_receiveBuf
 * @{
 */

/**
 * @brief Increment a received buffer index by a increment value and handle overflow.
 *
 * This internal function increments a passed buffer index by a specified increment and handles a possible overflow by exceeding the maximum buffer size
 * ::srrece_n_send_max. In case of an overflow, the index is set to 0.
 *
 * @implementsReq{RASW-608} Add to Buffer Function
 * @implementsReq{RASW-613} Read from Buffer Function
 *
 * @param [in] bufferIndex Pointer to the buffer index. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [in] increment Index increment value. The full value range is valid and usable.
 */
static void IncrementReceivedBufferIndexAndHandleOverflow(uint16_t* const bufferIndex, const uint16_t increment);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

static const uint16_t kIndexIncrement = 1U;  ///< Increment value for received buffer indexes

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void srrece_Init(const uint32_t configured_connections, const uint16_t configured_n_send_max) {
  // Input parameter check
  raas_AssertTrue(!srrece_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange(configured_connections, srcty_kMinNumberOfRaStaConnections, RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS, radef_kInvalidParameter);
  raas_AssertU16InRange(configured_n_send_max, srcty_kMinNSendMax, RADEF_MAX_N_SEND_MAX, radef_kInvalidParameter);

  srrece_number_of_connections = configured_connections;
  srrece_n_send_max = configured_n_send_max;

  srrece_initialized = true;

  // Init buffers for all configured connections
  for (uint32_t index = 0U; index < srrece_number_of_connections; ++index) {
    srrece_InitBuffer(index);
  }
}

void srrece_InitBuffer(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srrece_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srrece_number_of_connections, radef_kInvalidParameter);

  // Init buffer for specified connection id
  srrece_received_buffers[connection_id].read_idx = 0U;
  srrece_received_buffers[connection_id].write_idx = 0U;
  srrece_received_buffers[connection_id].used_elements = 0U;

  // Reset payload size of every element in the buffer of this connection
  for (uint16_t index = 0U; index < RADEF_MAX_N_SEND_MAX; ++index) {
    srrece_received_buffers[connection_id].buffer[index].payload_size = 0U;
  }
}

void srrece_AddToBuffer(const uint32_t connection_id, const srtyp_SrMessagePayload* const message_payload) {
  // Input parameter check
  raas_AssertTrue(srrece_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srrece_number_of_connections, radef_kInvalidParameter);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);
  raas_AssertU16InRange(message_payload->payload_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, radef_kInvalidParameter);

  raas_AssertTrue(srrece_received_buffers[connection_id].used_elements < srrece_n_send_max, radef_kReceiveBufferFull);

  // Add message to buffer
  srtyp_SrMessagePayload* const buffer_element = &srrece_received_buffers[connection_id].buffer[srrece_received_buffers[connection_id].write_idx];

  buffer_element->payload_size = message_payload->payload_size;
  for (uint16_t index = 0U; index < message_payload->payload_size; ++index) {
    buffer_element->payload[index] = message_payload->payload[index];
  }

  ++srrece_received_buffers[connection_id].used_elements;
  IncrementReceivedBufferIndexAndHandleOverflow(&srrece_received_buffers[connection_id].write_idx, kIndexIncrement);

  // Send notification to application layer
  srnot_MessageReceivedNotification(connection_id);
}

radef_RaStaReturnCode srrece_ReadFromBuffer(const uint32_t connection_id, srtyp_SrMessagePayload* const message_payload) {
  // Input parameter check
  raas_AssertTrue(srrece_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srrece_number_of_connections, radef_kInvalidParameter);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);

  radef_RaStaReturnCode ret = radef_kNoError;

  // Read message from buffer
  if (srrece_received_buffers[connection_id].used_elements > 0U) {
    const srtyp_SrMessagePayload* const buffer_element = &srrece_received_buffers[connection_id].buffer[srrece_received_buffers[connection_id].read_idx];
    raas_AssertU16InRange(buffer_element->payload_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, radef_kInternalError);

    message_payload->payload_size = buffer_element->payload_size;
    for (uint16_t index = 0U; index < message_payload->payload_size; ++index) {
      message_payload->payload[index] = buffer_element->payload[index];
    }

    --srrece_received_buffers[connection_id].used_elements;
    IncrementReceivedBufferIndexAndHandleOverflow(&srrece_received_buffers[connection_id].read_idx, kIndexIncrement);
  } else {
    // Buffer is empty
    ret = radef_kNoMessageReceived;
  }

  return ret;
}

uint32_t srrece_GetPayloadSizeOfNextMessageToRead(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srrece_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srrece_number_of_connections, radef_kInvalidParameter);

  uint32_t next_payload_size = 0U;

  if (srrece_received_buffers[connection_id].used_elements > 0U) {
    next_payload_size = srrece_received_buffers[connection_id].buffer[srrece_received_buffers[connection_id].read_idx].payload_size;
  }

  return next_payload_size;
}

uint16_t srrece_GetFreeBufferEntries(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srrece_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srrece_number_of_connections, radef_kInvalidParameter);

  raas_AssertTrue(srrece_n_send_max >= srrece_received_buffers[connection_id].used_elements, radef_kInternalError);

  return (uint16_t)(srrece_n_send_max - srrece_received_buffers[connection_id].used_elements);
}

uint16_t srrece_GetUsedBufferEntries(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srrece_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srrece_number_of_connections, radef_kInvalidParameter);

  return srrece_received_buffers[connection_id].used_elements;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static void IncrementReceivedBufferIndexAndHandleOverflow(uint16_t* const bufferIndex, const uint16_t increment) {
  // Input parameter check
  raas_AssertNotNull(bufferIndex, radef_kInternalError);

  *bufferIndex += increment;

  // Manage index overflow
  if (*bufferIndex >= srrece_n_send_max) {
    *bufferIndex = 0U;
  }
}
