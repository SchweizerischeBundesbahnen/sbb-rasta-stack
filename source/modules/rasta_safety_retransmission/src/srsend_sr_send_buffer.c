/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srsend_sr_send_buffer.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA SafRetL send buffer module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srsend_sr_send_buffer.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "srmsg_sr_messages.h"  // NOLINT(build/include_subdir)
#include "srtyp_sr_types.h"     // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------
/**
 * @brief Struct for SafRetL send message.
 */
typedef struct {
  bool already_sent;        ///< was this message already sent (used for retransmission)
  srtyp_SrMessage message;  ///< message
} SrSendMessage;

/**
 * @brief Struct for SafRetL send messages buffer.
 */
typedef struct {
  uint16_t remove_idx;                           ///< buffer remove index (next confirmed message to remove form the buffer)
  uint16_t read_idx;                             ///< buffer read index (next message to transmit or re-transmit)
  uint16_t write_idx;                            ///< buffer write index (next new message to write to the buffer)
  uint16_t used_elements;                        ///< current amount of elements in the buffer
  uint16_t not_sent_elements;                    ///< current amount of not sent elements in the buffer
  SrSendMessage buffer[RADEF_SEND_BUFFER_SIZE];  ///< buffer with the messages, waiting for to be send
} SrSendBuffer;

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of the module.
 * true, if the module is initialized.
 */
PRIVATE bool srsend_initialized = false;

/**
 * @brief Number of configured RaSTA connections.
 */
PRIVATE uint32_t srsend_number_of_connections = 0U;

/**
 * @brief Send buffers for all RaSTA connections.
 *
 * This array contains a buffer for every RaSTA connection and additionally a temporary buffer for the preparation of the send buffer for a retransmission.
 */
PRIVATE SrSendBuffer srsend_send_buffers[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS + 1U];

/**
 * @brief Memory to transfer and create messages. Valid range as described in ::srtyp_SrMessage.
 */
PRIVATE srtyp_SrMessage scratch_message;

/**
 * @brief Memory to transfer and create messages payload. Valid range as described in ::srtyp_SrMessagePayload.
 */
PRIVATE srtyp_SrMessagePayload scratch_message_payload;

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup sr_sendBuf
 * @{
 */

/**
 * @brief Initialize the send buffer of a dedicated RaSTA connection.
 *
 * This function initializes the buffer of a given RaSTA connection. It resets all properties of the buffer (read, write, remove index and used entries) and
 * also sets the message length of all elements in the buffer to 0 and clears the already sent flag.  This also lets you initialize the temporary buffer
 * that sits at the end of the array after the normal connection buffer elements.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-601} Init Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value <= configured number of connections. (Also includes temporary buffer
 * element).
 */
static void InitBuffer(const uint32_t connection_id);

/**
 * @brief Add a SafRetL message to the send buffer of a dedicated RaSTA connection.
 *
 * When there is free space in the buffer, a SafRetL message is added to the buffer. If the buffer is full, a ::radef_kSendBufferFull fatal error message
 * is thrown. After adding the message to the buffer, the position index and used entires are updated. This also lets you add a element to the temporary
 * buffer that sits at the end of the array after the normal connection buffer elements.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-596} Add to Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value <= configured number of connections (Also includes temporary buffer
 * element).
 * @param [in] message Pointer to SafRetL message structure that must be added to the buffer. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown. For the message payload the full value range is valid and usable, the message size has a valid range of ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <=
 * value <= ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE.
 */
static void AddToBuffer(const uint32_t connection_id, const srtyp_SrMessage* const message);

/**
 * @brief Copy the temporary buffer into a connection buffer
 *
 * This internal function initializes the buffer of a connection and then copies the temporary buffer, which was prepared in advance for a retransmission, into
 * this empty connection buffer.
 *
 * @implementsReq{RASW-603} Prepare Buffer for Retransmission Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
static void CopyTempBufferIntoConnectionBuffer(const uint32_t connection_id);

/**
 * @brief Create a new Data message and add it to the temporary buffer
 *
 * This internal function creates a new Data message based on the passed Data message. It is then added to the temporary buffer and the sequence number in the
 * header is incremented.
 *
 * @implementsReq{RASW-603} Prepare Buffer for Retransmission Function
 *
 * @param [in] sr_message Pointer to the input data or retrData message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [inout] new_msg_header Pointer to the header data to create a new message. The header contains also the current sequence number from which the new
 * message must continue and which at the end of the function will be incremented by one. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 */
static void CreateNewDataMsgAndAddToTempBuffer(const srtyp_SrMessage* const sr_message, srtyp_SrMessageHeaderCreate* const new_msg_header);

/**
 * @brief Create a new RetrData message and add it to the temporary buffer
 *
 * This internal function creates a new RetrData message based on the passed Data or RetrData message. It is then added to the temporary buffer and the sequence
 * number in the header is incremented.
 *
 * @implementsReq{RASW-603} Prepare Buffer for Retransmission Function
 *
 * @param [in] sr_message Pointer to the input data or retrData message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [inout] new_msg_header Pointer to the header data to create a new message. The header contains also the current sequence number from which the new
 * message must continue and which at the end of the function will be incremented by one. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 */
static void CreateNewRetrDataMsgAndAddToTempBuffer(const srtyp_SrMessage* const sr_message, srtyp_SrMessageHeaderCreate* const new_msg_header);

/**
 * @brief Create a new heartbeat message and add it to the temporary buffer
 *
 * This internal function creates a new heartbeat message. It is then added to the temporary buffer and the sequence number in the header is incremented.
 *
 * @implementsReq{RASW-603} Prepare Buffer for Retransmission Function
 *
 * @param [inout] new_msg_header Pointer to the header data to create a new message. The header contains also the current sequence number from which the new
 * message must continue and which at the end of the function will be incremented by one. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 */
static void CreateNewHbMsgAndAddToTempBuffer(srtyp_SrMessageHeaderCreate* const new_msg_header);

/**
 * @brief Create a new RetrReq message and add it to the temporary buffer
 *
 * This internal function creates a new RetrReq message. It is then added to the temporary buffer and the sequence number in the header is incremented.
 *
 * @implementsReq{RASW-603} Prepare Buffer for Retransmission Function
 *
 * @param [inout] new_msg_header Pointer to the header data to create a new message. The header contains also the current sequence number from which the new
 * message must continue and which at the end of the function will be incremented by one. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 */
static void CreateNewRetrReqMsgAndAddToTempBuffer(srtyp_SrMessageHeaderCreate* const new_msg_header);

/**
 * @brief Increment a send buffer index by a increment value and handle overflow.
 *
 * This internal function increments a passed buffer index by a specified increment and handles a possible overflow by exceeding the maximum buffer size
 * ::RADEF_SEND_BUFFER_SIZE. In case of an overflow, the maximum value is subtracted from the index.
 *
 * @implementsReq{RASW-604} Read Message to Send Function
 * @implementsReq{RASW-603} Prepare Buffer for Retransmission Function
 * @implementsReq{RASW-605} Remove from Buffer Function
 *
 * @param [in] bufferIndex Pointer to the buffer index. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [in] increment Index increment value. The full value range is valid and usable.
 */
static void IncrementSendBufferIndexAndHandleOverflow(uint16_t* const bufferIndex, const uint16_t increment);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

static const uint16_t kIndexIncrement = 1U;  ///< Increment value for send buffer indexes

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void srsend_Init(const uint32_t configured_connections) {
  // Input parameter check
  raas_AssertTrue(!srsend_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange(configured_connections, srcty_kMinNumberOfRaStaConnections, RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS, radef_kInvalidParameter);

  srsend_number_of_connections = configured_connections;

  srsend_initialized = true;

  // Init buffers for all configured connections
  for (uint32_t index = 0U; index < srsend_number_of_connections; ++index) {
    srsend_InitBuffer(index);
  }
}

void srsend_InitBuffer(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);

  InitBuffer(connection_id);
}

void srsend_AddToBuffer(const uint32_t connection_id, const srtyp_SrMessage* const message) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);
  raas_AssertNotNull(message, radef_kInvalidParameter);
  raas_AssertU16InRange(message->message_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);

  AddToBuffer(connection_id, message);
}

radef_RaStaReturnCode srsend_ReadMessageToSend(const uint32_t connection_id, srtyp_SrMessage* const message) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);
  raas_AssertNotNull(message, radef_kInvalidParameter);

  radef_RaStaReturnCode ret = radef_kNoError;

  // Read message from buffer
  if (srsend_send_buffers[connection_id].not_sent_elements > 0U) {
    const SrSendMessage* const kBufferElement = &srsend_send_buffers[connection_id].buffer[srsend_send_buffers[connection_id].read_idx];

    raas_AssertU16InRange(kBufferElement->message.message_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE,
                          radef_kInternalError);  // Check if message size is in valid range
    message->message_size = kBufferElement->message.message_size;

    for (uint32_t index = 0U; index < message->message_size; ++index) {
      message->message[index] = kBufferElement->message.message[index];
    }
    srsend_send_buffers[connection_id].buffer[srsend_send_buffers[connection_id].read_idx].already_sent = true;

    IncrementSendBufferIndexAndHandleOverflow(&srsend_send_buffers[connection_id].read_idx, kIndexIncrement);
    --srsend_send_buffers[connection_id].not_sent_elements;

  } else {
    // Buffer is empty
    ret = radef_kNoMessageToSend;
  }

  return ret;
}

void srsend_PrepareBufferForRetr(const uint32_t connection_id, const uint32_t sequence_number_for_retransmission,
                                 const srtyp_SrMessageHeaderCreate message_header, uint32_t* const new_current_sequence_number) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);
  raas_AssertNotNull(new_current_sequence_number, radef_kInvalidParameter);

  srtyp_SrMessageHeaderCreate new_msg_header = message_header;
  uint32_t retr_sequence_number = sequence_number_for_retransmission;

  raas_AssertTrue(srsend_send_buffers[connection_id].used_elements != 0U, radef_kInvalidSequenceNumber);

  bool retr_end_reached = false;

  // Prepare temporary retransmission buffer
  const uint32_t kRetrBufferId = srsend_number_of_connections;
  InitBuffer(kRetrBufferId);

  // Create retransmission response
  srmsg_CreateRetrRespMessage(new_msg_header, &scratch_message);
  AddToBuffer(kRetrBufferId, &scratch_message);
  ++new_msg_header.sequence_number;

  // Copy messages and transform data & retrData messages
  for (uint16_t index = 0U; index < srsend_send_buffers[connection_id].used_elements; ++index) {
    uint16_t element_index = srsend_send_buffers[connection_id].remove_idx;
    IncrementSendBufferIndexAndHandleOverflow(&element_index, index);

    ++retr_sequence_number;  // Unsigned integer wrap around is allowed here.
    raas_AssertTrue(srmsg_GetMessageSequenceNumber(&srsend_send_buffers[connection_id].buffer[element_index].message) == retr_sequence_number,
                    radef_kInvalidSequenceNumber);

    srtyp_SrMessageType current_message_type = srmsg_GetMessageType(&srsend_send_buffers[connection_id].buffer[element_index].message);

    if (srsend_send_buffers[connection_id].buffer[element_index].already_sent) {
      if ((current_message_type == srtyp_kSrMessageData) || (current_message_type == srtyp_kSrMessageRetrData)) {
        CreateNewRetrDataMsgAndAddToTempBuffer(&srsend_send_buffers[connection_id].buffer[element_index].message, &new_msg_header);
      } else {
        // Messages with other message types are dropped
      }
    } else {
      // While still RetrData is available, add it for retransmission. As soon as an other message type (only RetrReq & Data messages under normal
      // conditions) is added, retrData is then forbidden. Filter HB messages from beeing added
      if (current_message_type == srtyp_kSrMessageRetrData) {
        raas_AssertTrue(!retr_end_reached, radef_kInternalError);  // RetrData no longer allowed when retr_end reached

        CreateNewRetrDataMsgAndAddToTempBuffer(&srsend_send_buffers[connection_id].buffer[element_index].message, &new_msg_header);
      } else if (current_message_type == srtyp_kSrMessageData) {
        if (!retr_end_reached) {
          retr_end_reached = true;
        }

        CreateNewDataMsgAndAddToTempBuffer(&srsend_send_buffers[connection_id].buffer[element_index].message, &new_msg_header);
      } else if (current_message_type == srtyp_kSrMessageRetrReq) {
        if (!retr_end_reached) {
          CreateNewHbMsgAndAddToTempBuffer(&new_msg_header);
          retr_end_reached = true;
        }
        CreateNewRetrReqMsgAndAddToTempBuffer(&new_msg_header);
      } else if (current_message_type == srtyp_kSrMessageHb) {
        // HB messages are dropped
      } else {
        // Messages with other message types are not allowed
        rasys_FatalError(radef_kInternalError);
      }
    }
  }

  // Add HB msg to finish retransmission if still running
  if (!retr_end_reached) {
    CreateNewHbMsgAndAddToTempBuffer(&new_msg_header);
  }

  CopyTempBufferIntoConnectionBuffer(connection_id);

  // Set new current sequence number
  *new_current_sequence_number = new_msg_header.sequence_number;
}

radef_RaStaReturnCode srsend_IsSequenceNumberInBuffer(const uint32_t connection_id, const uint32_t sequence_number) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);

  radef_RaStaReturnCode ret = radef_kInvalidSequenceNumber;

  if (srsend_send_buffers[connection_id].used_elements != 0U) {
    for (uint16_t index = 0U; index < srsend_send_buffers[connection_id].used_elements; ++index) {
      uint16_t element_index = (uint16_t)(srsend_send_buffers[connection_id].remove_idx + index) % RADEF_SEND_BUFFER_SIZE;

      if (srmsg_GetMessageSequenceNumber(&srsend_send_buffers[connection_id].buffer[element_index].message) == sequence_number) {
        ret = radef_kNoError;
        break;  // End for-loop when message with matching sequence number is found
      }
    }
  }

  return ret;
}

void srsend_RemoveFromBuffer(const uint32_t connection_id, const uint32_t confirmed_sequence_number) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);

  if (srsend_send_buffers[connection_id].used_elements != 0U) {  // When no messages in buffer, nothing can be confirmed
    uint16_t already_send_elements = (uint16_t)(srsend_send_buffers[connection_id].used_elements - srsend_send_buffers[connection_id].not_sent_elements);
    raas_AssertTrue(already_send_elements <= RADEF_SEND_BUFFER_SIZE, radef_kInternalError);

    while (srsend_send_buffers[connection_id].used_elements > srsend_send_buffers[connection_id].not_sent_elements) {
      bool finish_loop = false;
      uint16_t element_index = srsend_send_buffers[connection_id].remove_idx;  // If elements are removed, the remove index is updated every loop iteration

      // Get sequence number difference. Care about sequence number overflow
      uint32_t current_sequence_nbr = srmsg_GetMessageSequenceNumber(&srsend_send_buffers[connection_id].buffer[element_index].message);
      uint32_t sequence_number_difference = confirmed_sequence_number - current_sequence_nbr;  // Unsigned integer wrap around is allowed here.

      if (sequence_number_difference < RADEF_SEND_BUFFER_SIZE) {
        srsend_send_buffers[connection_id].buffer[element_index].already_sent = false;
        srsend_send_buffers[connection_id].buffer[element_index].message.message_size = 0U;

        --srsend_send_buffers[connection_id].used_elements;
        IncrementSendBufferIndexAndHandleOverflow(&srsend_send_buffers[connection_id].remove_idx, kIndexIncrement);

        // Stop loop if confirmed sequence number is found
        if (current_sequence_nbr == confirmed_sequence_number) {
          finish_loop = true;
        }
      } else {
        finish_loop = true;  // Stop loop when newer sequence number is found
      }

      if (finish_loop) {
        break;
      }
    }
  }
}

uint16_t srsend_GetFreeBufferEntries(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);
  raas_AssertTrue(RADEF_SEND_BUFFER_SIZE >= srsend_send_buffers[connection_id].used_elements, radef_kInternalError);

  return (RADEF_SEND_BUFFER_SIZE - srsend_send_buffers[connection_id].used_elements);
}

uint16_t srsend_GetUsedBufferEntries(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);

  return srsend_send_buffers[connection_id].used_elements;
}

uint16_t srsend_GetNumberOfMessagesToSend(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srsend_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srsend_number_of_connections, radef_kInvalidParameter);

  return srsend_send_buffers[connection_id].not_sent_elements;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static void InitBuffer(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(connection_id <= srsend_number_of_connections, radef_kInternalError);

  // Init buffer for specified connection id
  srsend_send_buffers[connection_id].read_idx = 0U;
  srsend_send_buffers[connection_id].write_idx = 0U;
  srsend_send_buffers[connection_id].remove_idx = 0U;
  srsend_send_buffers[connection_id].used_elements = 0U;
  srsend_send_buffers[connection_id].not_sent_elements = 0U;

  // Reset message size and send flag of every element in the buffer of this connection
  for (uint16_t index = 0U; index < RADEF_MAX_N_SEND_MAX; ++index) {
    srsend_send_buffers[connection_id].buffer[index].already_sent = false;
    srsend_send_buffers[connection_id].buffer[index].message.message_size = 0U;
  }
}

static void AddToBuffer(const uint32_t connection_id, const srtyp_SrMessage* const message) {
  // Input parameter check
  raas_AssertTrue(connection_id <= srsend_number_of_connections, radef_kInternalError);
  raas_AssertNotNull(message, radef_kInternalError);
  raas_AssertU16InRange(message->message_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

  raas_AssertTrue(srsend_send_buffers[connection_id].used_elements < RADEF_SEND_BUFFER_SIZE, radef_kSendBufferFull);

  // Add message to buffer
  SrSendMessage* const buffer_element = &srsend_send_buffers[connection_id].buffer[srsend_send_buffers[connection_id].write_idx];

  buffer_element->message.message_size = message->message_size;
  for (uint32_t i = 0U; i < message->message_size; ++i) {
    buffer_element->message.message[i] = message->message[i];
  }
  srsend_send_buffers[connection_id].buffer[srsend_send_buffers[connection_id].write_idx].already_sent = false;

  ++srsend_send_buffers[connection_id].used_elements;
  ++srsend_send_buffers[connection_id].not_sent_elements;
  IncrementSendBufferIndexAndHandleOverflow(&srsend_send_buffers[connection_id].write_idx, kIndexIncrement);
}

static void CopyTempBufferIntoConnectionBuffer(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(connection_id <= srsend_number_of_connections, radef_kInternalError);
  raas_AssertTrue(srsend_send_buffers[connection_id].used_elements < RADEF_SEND_BUFFER_SIZE, radef_kInternalError);

  // Reset send buffer of the connection
  InitBuffer(connection_id);

  // Copy temp buffer back to send buffer
  const uint32_t kRetrBufferId = srsend_number_of_connections;
  srsend_send_buffers[connection_id].read_idx = srsend_send_buffers[kRetrBufferId].read_idx;
  srsend_send_buffers[connection_id].remove_idx = srsend_send_buffers[kRetrBufferId].remove_idx;
  srsend_send_buffers[connection_id].write_idx = srsend_send_buffers[kRetrBufferId].write_idx;
  srsend_send_buffers[connection_id].used_elements = srsend_send_buffers[kRetrBufferId].used_elements;
  srsend_send_buffers[connection_id].not_sent_elements = srsend_send_buffers[kRetrBufferId].not_sent_elements;

  // Copy messages and transform data & retrData messages
  for (uint16_t index = 0U; index < srsend_send_buffers[connection_id].used_elements; ++index) {
    uint16_t element_index = srsend_send_buffers[connection_id].remove_idx;
    IncrementSendBufferIndexAndHandleOverflow(&element_index, index);

    raas_AssertU16InRange(srsend_send_buffers[kRetrBufferId].buffer[element_index].message.message_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE,
                          RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

    srsend_send_buffers[connection_id].buffer[element_index].already_sent = srsend_send_buffers[kRetrBufferId].buffer[element_index].already_sent;
    srsend_send_buffers[connection_id].buffer[element_index].message.message_size =
        srsend_send_buffers[kRetrBufferId].buffer[element_index].message.message_size;

    for (uint16_t msg_index = 0U; msg_index < srsend_send_buffers[connection_id].buffer[element_index].message.message_size; ++msg_index) {
      srsend_send_buffers[connection_id].buffer[element_index].message.message[msg_index] =
          srsend_send_buffers[kRetrBufferId].buffer[element_index].message.message[msg_index];
    }
  }
}

static void CreateNewDataMsgAndAddToTempBuffer(const srtyp_SrMessage* const sr_message, srtyp_SrMessageHeaderCreate* const new_msg_header) {
  // Input parameter check
  raas_AssertNotNull(new_msg_header, radef_kInternalError);
  raas_AssertNotNull(sr_message, radef_kInternalError);

  // Extract msg data and create new retrData msg
  srmsg_GetDataMessagePayload(sr_message, &scratch_message_payload);
  srmsg_CreateDataMessage(*new_msg_header, &scratch_message_payload, &scratch_message);

  // Add retrData msg to temp buffer
  const uint32_t kRetrBufferId = srsend_number_of_connections;
  AddToBuffer(kRetrBufferId, &scratch_message);
  ++new_msg_header->sequence_number;
}

static void CreateNewRetrDataMsgAndAddToTempBuffer(const srtyp_SrMessage* const sr_message, srtyp_SrMessageHeaderCreate* const new_msg_header) {
  // Input parameter check
  raas_AssertNotNull(new_msg_header, radef_kInternalError);
  raas_AssertNotNull(sr_message, radef_kInternalError);

  // Extract msg data and create new retrData msg
  srmsg_GetDataMessagePayload(sr_message, &scratch_message_payload);
  srmsg_CreateRetrDataMessage(*new_msg_header, &scratch_message_payload, &scratch_message);

  // Add retrData msg to temp buffer
  const uint32_t kRetrBufferId = srsend_number_of_connections;
  AddToBuffer(kRetrBufferId, &scratch_message);
  ++new_msg_header->sequence_number;
}

static void CreateNewHbMsgAndAddToTempBuffer(srtyp_SrMessageHeaderCreate* const new_msg_header) {
  // Input parameter check
  raas_AssertNotNull(new_msg_header, radef_kInternalError);

  // Create new HB msg
  srmsg_CreateHeartbeatMessage(*new_msg_header, &scratch_message);

  // Add HB msg to temp buffer
  const uint32_t kRetrBufferId = srsend_number_of_connections;
  AddToBuffer(kRetrBufferId, &scratch_message);
  ++new_msg_header->sequence_number;
}

static void CreateNewRetrReqMsgAndAddToTempBuffer(srtyp_SrMessageHeaderCreate* const new_msg_header) {
  // Input parameter check
  raas_AssertNotNull(new_msg_header, radef_kInternalError);

  // Create new retrReq msg
  srmsg_CreateRetrReqMessage(*new_msg_header, &scratch_message);

  // Add retrReq msg to temp buffer
  const uint32_t kRetrBufferId = srsend_number_of_connections;
  AddToBuffer(kRetrBufferId, &scratch_message);
  ++new_msg_header->sequence_number;
}

static void IncrementSendBufferIndexAndHandleOverflow(uint16_t* const bufferIndex, const uint16_t increment) {
  // Input parameter check
  raas_AssertNotNull(bufferIndex, radef_kInternalError);

  *bufferIndex += increment;

  // Manage index overflow
  if (*bufferIndex >= RADEF_SEND_BUFFER_SIZE) {
    *bufferIndex -= RADEF_SEND_BUFFER_SIZE;
  }
}
