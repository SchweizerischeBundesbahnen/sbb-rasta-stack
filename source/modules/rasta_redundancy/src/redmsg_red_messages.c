/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redmsg_red_messages.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer messages module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redmsg_red_messages.h"  // NOLINT(build/include_subdir)

#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_redundancy/redcty_red_config_types.h"
#include "redcrc_red_crc.h"    // NOLINT(build/include_subdir)
#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Enum for the check code length.
 */
enum {
  kCheckCodeNoneLength = 0U,   ///< Length of code code type none [bytes]
  kCheckCodeCrc16Length = 2U,  ///< Length of code code type CRC16 [bytes]
  kCheckCodeCrc32Length = 4U   ///< Length of code code type CRC32 [bytes]
};

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of the module. True, if the module is initialized.
 */
PRIVATE bool redmsg_initialized = false;

/**
 * @brief Configured check code type.
 */
PRIVATE redcty_CheckCodeType redmsg_check_code_type;

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup red_messages
 * @{
 */

/**
 * @brief Set a Uint16 at a specific position in a message
 *
 * This internal function sets a Uint16 byte by byte in little endian format at a given position in a message. If the Uint16 doesn't fit inside the message
 * (position + Uint16 byte size > red_message->message_size), a ::radef_kInternalError fatal error message is thrown. red_message->message_size must be set
 * correctly before calling this function.
 *
 * @param [in] position Position where the variable must be set in the message [bytes].
 * @param [in] data Variable to be set in the provided message. The full value range is valid and usable.
 * @param [inout] red_message Pointer to a message, where the variable must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetUint16InMessage(const uint16_t position, const uint16_t data, redtyp_RedundancyMessage *const red_message);

/**
 * @brief Set a Uint32 at a specific position in a message
 *
 * This internal function sets a Uint32 byte by byte in little endian format at a given position in a message. If the Uint32 doesn't fit inside the message
 * (position + Uint32 byte size > red_message->message_size), a ::radef_kInternalError fatal error message is thrown. red_message->message_size must be set
 * correctly before calling this function.
 *
 * @param [in] position Position where the variable must be set in the message [bytes].
 * @param [in] data Variable to be set in the provided message. The full value range is valid and usable.
 * @param [inout] red_message Pointer to a message, where the variable must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetUint32InMessage(const uint16_t position, const uint32_t data, redtyp_RedundancyMessage *const red_message);

/**
 * @brief Set the payload data in a message
 *
 * This internal function writes specific payload data at a provided position in the message. If the payload doesn't fit inside the message
 * (position + payload size > red_message->message_size), a ::radef_kInternalError fatal error message is thrown. red_message->message_size must be set
 * correctly before calling this function.
 *
 * @param [in] message_payload Pointer to payload data that is written. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [inout] red_message Pointer to a message, where the payload data must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 */
static void SetPayloadDataInMessage(const redtyp_RedundancyMessagePayload *const message_payload, redtyp_RedundancyMessage *const red_message);
/**
 * @brief Get a Uint16 from a specific position in a message
 *
 * This internal function extracts a Uint16 byte by byte from a given position in the little endian format message. If the Uint16 extends over the size of
 * the message from the given start position (position + Uint16 byte size > red_message->message_size), a ::radef_kInternalError fatal error message is thrown.
 * red_message->message_size must be set correctly before calling this function.
 *
 * @param [in] red_message Pointer to a message, from where a variable must be extracted. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 * @param [in] position  Start position where the variable must be read in the message [bytes].
 * @return uint16_t Read variable from the specified start position.
 */
static uint16_t GetUint16FromMessage(const redtyp_RedundancyMessage *const red_message, const uint16_t position);

/**
 * @brief Get a Uint32 from a specific position in a message
 *
 * This internal function extracts a Uint32 byte by byte from a given position in the little endian format message. If the Uint32 extends over the size of
 * the message from the given start position (position + Uint32 byte size > red_message->message_size), a ::radef_kInternalError fatal error message is thrown.
 * red_message->message_size must be set correctly before calling this function.
 *
 * @param [in] red_message Pointer to a message, from where a variable must be extracted. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 * @param [in] position  Start position where the variable must be read in the message [bytes].
 * @return uint16_t Read variable from the specified start position.
 */
static uint32_t GetUint32FromMessage(const redtyp_RedundancyMessage *const red_message, const uint16_t position);

/**
 * @brief Get the length of the configured check code
 *
 * This internal function returns the byte length of the configured check code.
 *
 * @param [in] check_code_type check code type. All enum entries of ::redcty_CheckCodeType are valid and usable.
 * @return uint16_t length of the check code [bytes].
 */
static uint16_t GetCheckCodeLength(const redcty_CheckCodeType check_code_type);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

static const uint16_t kByteCountUint16 = 2U;  ///< Byte count of type UInt16_t [bytes]
static const uint16_t kByteCountUint32 = 4U;  ///< Byte count of type UInt32_t [bytes]

// General Positions
static const uint16_t kMsgLengthPosition = 0U;       ///< Start position for message length in PDU message [bytes]
static const uint16_t kMsgReservePosition = 2U;      ///< Start position for reserve bytes in PDU message [bytes]
static const uint16_t kMsgSequenceNbrPosition = 4U;  ///< Start position for sequence number in PDU message [bytes]
static const uint16_t kMsgPayloadDataPosition = 8U;  ///< Start position for payload data in PDU message [bytes]

static const uint16_t kMsgReserveDataInitValue = 0U;  ///< Reserve data initial value

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void redmsg_Init(const redcty_CheckCodeType configured_check_code_type) {
  // Input parameter check
  raas_AssertTrue(!redmsg_initialized, radef_kAlreadyInitialized);
  raas_AssertU16InRange((uint16_t)configured_check_code_type, (uint16_t)redcty_kCheckCodeMin, (uint16_t)(redcty_kCheckCodeMax)-1U, radef_kInvalidParameter);

  // Initialize local variable
  redmsg_check_code_type = configured_check_code_type;
  redmsg_initialized = true;

  // Initialize CRC module
  redcrc_Init(redmsg_check_code_type);
}

void redmsg_CreateMessage(const uint32_t sequence_number, const redtyp_RedundancyMessagePayload *const message_payload,
                          redtyp_RedundancyMessage *const redundancy_message) {
  // Input parameter check
  raas_AssertTrue(redmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);
  raas_AssertU16InRange(message_payload->payload_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);
  raas_AssertNotNull(redundancy_message, radef_kInvalidParameter);

  // Set message header data
  redundancy_message->message_size = kMsgPayloadDataPosition + message_payload->payload_size +
                                     GetCheckCodeLength(redmsg_check_code_type);  // set message size before calling SetUint16InMessage()
  SetUint16InMessage(kMsgLengthPosition, redundancy_message->message_size, redundancy_message);
  SetUint16InMessage(kMsgReservePosition, kMsgReserveDataInitValue, redundancy_message);
  SetUint32InMessage(kMsgSequenceNbrPosition, sequence_number, redundancy_message);

  // Set message payload data
  SetPayloadDataInMessage(message_payload, redundancy_message);

  // Calculate and set check code data
  uint16_t check_code_length = GetCheckCodeLength(redmsg_check_code_type);
  if (check_code_length > 0U) {
    uint32_t calculated_crc = 0U;
    const uint16_t kMessageLengthWithoutCheckCode = kMsgPayloadDataPosition + message_payload->payload_size;
    redcrc_CalculateCrc(kMessageLengthWithoutCheckCode, redundancy_message->message, &calculated_crc);
    switch (check_code_length) {
      case kCheckCodeCrc32Length:
        SetUint32InMessage(kMessageLengthWithoutCheckCode, calculated_crc, redundancy_message);
        break;
      case kCheckCodeCrc16Length:
        SetUint16InMessage(kMessageLengthWithoutCheckCode, (uint16_t)calculated_crc, redundancy_message);
        break;
        // no coverage start
      default:
        rasys_FatalError(radef_kInternalError);
        break;
        // no coverage end
    }
  }
}

radef_RaStaReturnCode redmsg_CheckMessageCrc(const redtyp_RedundancyMessage *const redundancy_message) {
  // Input parameter check
  raas_AssertTrue(redmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(redundancy_message, radef_kInvalidParameter);
  raas_AssertU16InRange(redundancy_message->message_size, RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);
  raas_AssertTrue(redundancy_message->message_size == GetUint16FromMessage(redundancy_message, kMsgLengthPosition), radef_kInvalidParameter);

  // Calculate and verify check code data
  radef_RaStaReturnCode return_code = radef_kNoError;
  uint16_t check_code_length = GetCheckCodeLength(redmsg_check_code_type);
  if (check_code_length > 0U) {
    uint32_t calculated_crc = 0U;
    const uint16_t kMessageLengthWithoutCheckCode = (uint16_t)(redundancy_message->message_size - check_code_length);
    redcrc_CalculateCrc(kMessageLengthWithoutCheckCode, redundancy_message->message, &calculated_crc);
    switch (check_code_length) {
      case kCheckCodeCrc32Length:
        if (GetUint32FromMessage(redundancy_message, kMessageLengthWithoutCheckCode) != calculated_crc) {
          return_code = radef_kInvalidMessageCrc;
        }
        break;
      case kCheckCodeCrc16Length:
        if (GetUint16FromMessage(redundancy_message, kMessageLengthWithoutCheckCode) != (uint16_t)calculated_crc) {
          return_code = radef_kInvalidMessageCrc;
        }
        break;
        // no coverage start
      default:
        rasys_FatalError(radef_kInternalError);
        break;
        // no coverage end
    }
  }

  return return_code;
}

uint32_t redmsg_GetMessageSequenceNumber(const redtyp_RedundancyMessage *const redundancy_message) {
  // Input parameter check
  raas_AssertTrue(redmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(redundancy_message, radef_kInvalidParameter);
  raas_AssertU16InRange(redundancy_message->message_size, RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);
  raas_AssertTrue(redundancy_message->message_size == GetUint16FromMessage(redundancy_message, kMsgLengthPosition), radef_kInvalidParameter);

  // Get and return the sequence number from the message
  return GetUint32FromMessage(redundancy_message, kMsgSequenceNbrPosition);
}

void redmsg_GetMessagePayload(const redtyp_RedundancyMessage *const redundancy_message, redtyp_RedundancyMessagePayload *const message_payload) {
  // Input parameter check
  raas_AssertTrue(redmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(redundancy_message, radef_kInvalidParameter);
  raas_AssertU16InRange(redundancy_message->message_size, RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);
  raas_AssertTrue(redundancy_message->message_size == GetUint16FromMessage(redundancy_message, kMsgLengthPosition), radef_kInvalidParameter);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);

  // Extract message data
  const uint16_t message_payload_size = (uint16_t)(redundancy_message->message_size - (kMsgPayloadDataPosition + GetCheckCodeLength(redmsg_check_code_type)));
  raas_AssertU16InRange(message_payload_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

  message_payload->payload_size = message_payload_size;
  for (uint16_t index = 0U; index < message_payload_size; ++index) {
    message_payload->payload[index] = redundancy_message->message[kMsgPayloadDataPosition + index];
  }
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static void SetUint16InMessage(const uint16_t position, const uint16_t data, redtyp_RedundancyMessage *const red_message) {
  // Input parameter check
  raas_AssertNotNull(red_message, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint16) <= red_message->message_size, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint16) <= RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

  // Write variable in message
  red_message->message[position + 1U] = (uint8_t)((data >> 8) & 0xFFU);
  red_message->message[position] = (uint8_t)(data & 0xFFU);
}

static void SetUint32InMessage(const uint16_t position, const uint32_t data, redtyp_RedundancyMessage *const red_message) {
  // Input parameter check
  raas_AssertNotNull(red_message, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint32) <= red_message->message_size, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint32) <= RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

  // Write variable in message
  red_message->message[position + 3U] = (uint8_t)((data >> 24) & 0xFFU);
  red_message->message[position + 2U] = (uint8_t)((data >> 16) & 0xFFU);
  red_message->message[position + 1U] = (uint8_t)((data >> 8) & 0xFFU);
  red_message->message[position] = (uint8_t)(data & 0xFFU);
}

static void SetPayloadDataInMessage(const redtyp_RedundancyMessagePayload *const message_payload, redtyp_RedundancyMessage *const red_message) {
  // Input parameter check
  raas_AssertNotNull(message_payload, radef_kInternalError);
  raas_AssertNotNull(red_message, radef_kInternalError);
  raas_AssertTrue(message_payload->payload_size <= red_message->message_size, radef_kInternalError);
  raas_AssertTrue(message_payload->payload_size <= RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

  // Write payload data in message
  for (uint16_t index = 0U; index < message_payload->payload_size; ++index) {
    red_message->message[kMsgPayloadDataPosition + index] = message_payload->payload[index];
  }
}

static uint16_t GetUint16FromMessage(const redtyp_RedundancyMessage *const red_message, const uint16_t position) {
  // Input parameter check
  raas_AssertNotNull(red_message, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint16) <= red_message->message_size, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint16) <= RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

  // Get variable from message
  uint16_t data = 0U;
  for (uint16_t index = 0U; index < kByteCountUint16; ++index) {
    data += (uint16_t)red_message->message[position + index] << (index * 8U);
  }

  return data;
}

static uint32_t GetUint32FromMessage(const redtyp_RedundancyMessage *const red_message, const uint16_t position) {
  // Input parameter check
  raas_AssertNotNull(red_message, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint32) <= red_message->message_size, radef_kInternalError);
  raas_AssertTrue((position + kByteCountUint32) <= RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);

  // Get variable from message
  uint32_t data = 0U;
  for (uint16_t index = 0U; index < kByteCountUint32; ++index) {
    data += (uint32_t)red_message->message[position + index] << (index * 8U);
  }

  return data;
}

static uint16_t GetCheckCodeLength(const redcty_CheckCodeType check_code_type) {
  uint16_t check_code_length = 0U;

  switch (check_code_type) {
    case redcty_kCheckCodeA:
      check_code_length = kCheckCodeNoneLength;
      break;
    case redcty_kCheckCodeB:
    case redcty_kCheckCodeC:
      check_code_length = kCheckCodeCrc32Length;
      break;
    case redcty_kCheckCodeD:
    case redcty_kCheckCodeE:
      check_code_length = kCheckCodeCrc16Length;
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);
      break;
      // no coverage end
  }

  return check_code_length;
}
