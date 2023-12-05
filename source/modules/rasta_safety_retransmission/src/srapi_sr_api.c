/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srapi_sr_api.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA SafRetL API.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_safety_retransmission/srapi_sr_api.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/rahlp_rasta_helper.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "srcor_sr_core.h"              // NOLINT(build/include_subdir)
#include "srrece_sr_received_buffer.h"  // NOLINT(build/include_subdir)
#include "srsend_sr_send_buffer.h"      // NOLINT(build/include_subdir)
#include "srstm_sr_state_machine.h"     // NOLINT(build/include_subdir)
#include "srtyp_sr_types.h"             // NOLINT(build/include_subdir)

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
 * @brief Initialization state of the module.
 * true, if the module is initialized.
 */
PRIVATE bool srapi_initialized = false;

/**
 * @brief Pointer to SafRetL configuration.
 */
PRIVATE const srcty_SafetyRetransmissionConfiguration *srapi_sr_configuration = NULL;

/**
 * @brief Memory to transfer messages
 */
PRIVATE srtyp_SrMessagePayload srapi_scratch_message_payload;

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

radef_RaStaReturnCode srapi_Init(const srcty_SafetyRetransmissionConfiguration *const safety_retransmission_configuration) {
  radef_RaStaReturnCode ret = radef_kNoError;

  // Input parameter check
  if (safety_retransmission_configuration == NULL) {
    ret = radef_kInvalidParameter;
  } else if (srapi_initialized) {
    ret = radef_kAlreadyInitialized;
  } else if (srcor_IsConfigurationValid(safety_retransmission_configuration)) {
    srapi_sr_configuration = safety_retransmission_configuration;

    srstm_Init(srapi_sr_configuration->number_of_connections);
    srcor_Init(srapi_sr_configuration);
    srapi_initialized = true;

    ret = srapi_GetInitializationState();
  } else {
    ret = radef_kInvalidConfiguration;
  }

  return ret;
}

radef_RaStaReturnCode srapi_GetInitializationState(void) {
  radef_RaStaReturnCode ret = radef_kNotInitialized;

  if (srapi_initialized) {
    ret = radef_kNoError;
  }
  return ret;
}

radef_RaStaReturnCode srapi_OpenConnection(const uint32_t sender_id, const uint32_t receiver_id, const uint32_t network_id, uint32_t *const connection_id) {
  radef_RaStaReturnCode ret = radef_kNoError;

  // Input parameter check
  if (connection_id == NULL) {
    ret = radef_kInvalidParameter;
  } else if (!srapi_initialized) {
    ret = radef_kNotInitialized;
  } else if (network_id == srapi_sr_configuration->rasta_network_id) {
    ret = srcor_GetConnectionId(sender_id, receiver_id, connection_id);
    if (ret == radef_kNoError) {
      srstm_ProcessConnectionStateMachine(*connection_id, srtyp_kConnEventOpen, false, false);
    }
  } else {
    ret = radef_kInvalidParameter;
  }

  return ret;
}

radef_RaStaReturnCode srapi_CloseConnection(const uint32_t connection_id, const uint16_t detailed_reason) {
  radef_RaStaReturnCode ret = radef_kNoError;

  if (!srapi_initialized) {
    ret = radef_kNotInitialized;
  } else if (connection_id < srapi_sr_configuration->number_of_connections) {
    srcor_SetDiscDetailedReason(connection_id, detailed_reason);
    srstm_ProcessConnectionStateMachine(connection_id, srtyp_kConnEventClose, false, false);
  } else {
    ret = radef_kInvalidParameter;
  }

  return ret;
}

radef_RaStaReturnCode srapi_SendData(const uint32_t connection_id, const uint16_t message_size, const uint8_t *const message_data) {
  radef_RaStaReturnCode ret = radef_kNoError;

  // Input parameter check
  if (message_data == NULL) {
    ret = radef_kInvalidParameter;
  } else if (!rahlp_IsU16InRange(message_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE)) {
    ret = radef_kInvalidMessageSize;
  } else if (!srapi_initialized) {
    ret = radef_kNotInitialized;
  } else if (connection_id < srapi_sr_configuration->number_of_connections) {
    if (srstm_GetConnectionState(connection_id) == sraty_kConnectionClosed) {
      ret = radef_kInvalidOperationInCurrentState;
    } else {
      if (srsend_GetFreeBufferEntries(connection_id) > 0U) {
        srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, message_size, message_data);
        srstm_ProcessConnectionStateMachine(connection_id, srtyp_kConnEventSendData, false, false);
      } else {
        ret = radef_kSendBufferFull;
      }
    }
  } else {
    ret = radef_kInvalidParameter;
  }

  return ret;
}

radef_RaStaReturnCode srapi_ReadData(const uint32_t connection_id, const uint16_t buffer_size, uint16_t *const message_size, uint8_t *const message_buffer) {
  radef_RaStaReturnCode ret = radef_kNoError;

  // Input parameter check
  if (message_size == NULL) {
    ret = radef_kInvalidParameter;
  } else if (message_buffer == NULL) {
    ret = radef_kInvalidParameter;
  } else if (!rahlp_IsU16InRange(buffer_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE)) {
    ret = radef_kInvalidBufferSize;
  } else if (!srapi_initialized) {
    ret = radef_kNotInitialized;
  } else if (srrece_GetPayloadSizeOfNextMessageToRead(connection_id) > buffer_size) {  // Not enough space to save the next message
    ret = radef_kInvalidBufferSize;
  } else if (connection_id < srapi_sr_configuration->number_of_connections) {
    ret = srrece_ReadFromBuffer(connection_id, &srapi_scratch_message_payload);

    if (ret == radef_kNoError) {
      // Copy message size & data to provided memory
      *message_size = srapi_scratch_message_payload.payload_size;

      for (uint32_t index = 0U; index < *message_size; ++index) {
        message_buffer[index] = srapi_scratch_message_payload.payload[index];
      }
    }
  } else {
    ret = radef_kInvalidParameter;
  }

  return ret;
}

radef_RaStaReturnCode srapi_GetConnectionState(const uint32_t connection_id, sraty_ConnectionStates *const connection_state,
                                               sraty_BufferUtilisation *const buffer_utilisation, uint16_t *const opposite_buffer_size) {
  radef_RaStaReturnCode ret = radef_kNoError;

  // Input parameter check
  if (connection_state == NULL) {
    ret = radef_kInvalidParameter;
  } else if (buffer_utilisation == NULL) {
    ret = radef_kInvalidParameter;
  } else if (opposite_buffer_size == NULL) {
    ret = radef_kInvalidParameter;
  } else if (!srapi_initialized) {
    ret = radef_kNotInitialized;
  } else if (connection_id < srapi_sr_configuration->number_of_connections) {
    *connection_state = srstm_GetConnectionState(connection_id);
    srcor_GetBufferSizeAndUtilisation(connection_id, buffer_utilisation, opposite_buffer_size);
  } else {
    ret = radef_kInvalidParameter;
  }

  return ret;
}

radef_RaStaReturnCode srapi_CheckTimings(void) {
  radef_RaStaReturnCode ret = radef_kNoError;

  if (srapi_initialized) {
    for (uint32_t index = 0U; index < srapi_sr_configuration->number_of_connections; ++index) {
      uint32_t connection_id = index;  // Connection_id always start from 0 and counts upwards
      srtyp_ConnectionEvents connection_event = srtyp_kConnEventNone;
      bool sequence_number_in_seq = false;
      bool confirmed_time_stamp_in_seq = false;

      // Received messages polling
      while (srcor_IsReceivedMsgPendingAndBuffersNotFull(connection_id)) {
        srcor_ReceiveMessage(connection_id, &connection_event, &sequence_number_in_seq, &confirmed_time_stamp_in_seq);
        if (connection_event != srtyp_kConnEventNone) {
          srstm_ProcessConnectionStateMachine(connection_id, connection_event, sequence_number_in_seq, confirmed_time_stamp_in_seq);
        }
      }

      // Send pending messages
      srcor_SendPendingMessages(connection_id);

      // Check timings
      if (srcor_IsMessageTimeout(connection_id)) {
        // Message timeout
        srstm_ProcessConnectionStateMachine(connection_id, srtyp_kConnEventTimeout, false, false);
      } else {
        // No message timeout
        bool no_messages_to_send = (srsend_GetNumberOfMessagesToSend(connection_id) == 0U);
        if (srcor_IsHeartbeatInterval(connection_id) && no_messages_to_send) {
          srstm_ProcessConnectionStateMachine(connection_id, srtyp_kConnEventSendHb, false, false);
        }
      }
    }
  } else {
    ret = radef_kNotInitialized;
  }

  return ret;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
