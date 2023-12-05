/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmsg_sr_messages.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 4278c3bc2a5a128fd5eeaaa2c5d939b42ea82149
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4935: Messages module must check message size and payload data size correctly,07.12.2022, N. Andres}
 *
 * @brief Implementation of RaSTA SafRetL messages module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srmsg_sr_messages.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "srmd4_sr_md4.h"    // NOLINT(build/include_subdir)
#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

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
PRIVATE bool srmsg_initialized = false;

/**
 * @brief Configured safety code type.
 */
PRIVATE srcty_SafetyCodeType srmsg_safety_code_type;

/**
 * @brief Configured MD4 initial code value.
 */
PRIVATE srcty_Md4InitValue srmsg_md4_initial_value;

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup sr_messages
 * @{
 */

/**
 * @brief Checks if a message type is valid or not
 *
 * This internal function checks the validity of a message type.
 *
 * @implementsReq{RASW-616} Check Message Function
 *
 * @param [in] message_type Message type to check. All entries of ::srtyp_SrMessageType enum are valid. If the value is different from this entries, false is
 * returned.
 * @return true when message type is valid
 * @return false when message type is invalid/not known
 */
static bool IsMessageTypeValid(srtyp_SrMessageType message_type);

/**
 * @brief Set a Uint16 at a specific position in a message
 *
 * This internal function sets a Uint16 byte by byte in little endian format at a given position in a message. If the Uint16 doesn't fit inside the message
 * (position + Uint16 byte size > message size), a ::radef_kInternalError fatal error message is thrown.
 *
 * @implementsReq{RASW-157} Endian Definition
 *
 * @param [in] position Position where the variable must be set in the message [bytes].
 * @param [in] data Variable to be set in the provided message.
 * @param [inout] sr_message Pointer to a message, where the variable must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetUint16InMessage(const uint16_t position, const uint16_t data, srtyp_SrMessage *const sr_message);

/**
 * @brief Set a Uint32 at a specific position in a message
 *
 * This internal function sets a Uint32 byte by byte in little endian format at a given position in a message. If the Uint32 doesn't fit inside the message
 * (position + Uint32 byte size > message size), a ::radef_kInternalError fatal error message is thrown.
 *
 * @implementsReq{RASW-157} Endian Definition
 *
 * @param [in] position Position where the variable must be set in the message [bytes].
 * @param [in] data Variable to be set in the provided message.
 * @param [inout] sr_message Pointer to a message, where the variable must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetUint32InMessage(const uint16_t position, const uint32_t data, srtyp_SrMessage *const sr_message);

/**
 * @brief Set a Uint64 at a specific position in a message
 *
 * This internal function sets a Uint64 byte by byte in little endian format at a given position in a message. If the Uint64 doesn't fit inside the message
 * (position + Uint64 byte size > message size), a ::radef_kInternalError fatal error message is thrown.
 *
 * @implementsReq{RASW-157} Endian Definition
 *
 * @param [in] position Position where the variable must be set in the message [bytes].
 * @param [in] data Variable to be set in the provided message.
 * @param [inout] sr_message Pointer to a message, where the variable must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetUint64InMessage(const uint16_t position, const uint64_t data, srtyp_SrMessage *const sr_message);

/**
 * @brief Set the message header data in a message
 *
 * This internal function sets the header data (message length, message type and ::srtyp_SrMessageHeaderCreate consisting of receiver id, sender id and
 * confirmed time stamp) in the provided message. The sequence number, confirmed sequence number and the timestamp are all set to 0, since they are updated just
 * before sending a message.
 *
 * @implementsReq{RASW-157} Endian Definition
 * @implementsReq{RASW-160} General PDU Message Structure
 * @implementsReq{RASW-161} Message Type
 * @implementsReq{RASW-162} Receiver Identification
 * @implementsReq{RASW-163} Sender Identification
 * @implementsReq{RASW-167} Confirmed Time Stamp
 *
 * @param [in] message_length Message length to set in message [bytes]. The message length is checked if it is in the valid range of
 * (::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + safety code length) <= value <= ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, otherwise a ::radef_kInternalError fatal
 * error is thrown.
 * @param [in] message_type Message type to set in message. Valid range: ::srtyp_kSrMessageMin <= value <= ::srtyp_kSrMessageMax -1.
 * @param [in] message_header Message header data to set in message.
 * @param [out] sr_message Pointer to a message, where the header must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetMessageHeaderInMessage(const uint16_t message_length, const uint16_t message_type, const srtyp_SrMessageHeaderCreate message_header,
                                      srtyp_SrMessage *const sr_message);

/**
 * @brief Set the protocol version in a message
 *
 * This internal function sets the protocol version in the provided message.
 *
 * @implementsReq{RASW-157} Endian Definition
 * @implementsReq{RASW-170} Connection Request Message Structure
 * @implementsReq{RASW-171} Connection Response Message Structure
 * @implementsReq{RASW-173} Protocol Version
 *
 * @param [in] protocol_version Protocol version to set in message. Valid range for every digit of the version: ::srcty_kProtocolVersionMinValue <= value <=
 * ::srcty_kProtocolVersionMaxValue. If the value is outside this range, a ::radef_kInternalError fatal error is thrown.
 * @param [inout] sr_message Pointer to a message, where the protocol version must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 */
static void SetProtocolVersionInMessage(const srtyp_ProtocolVersion protocol_version, srtyp_SrMessage *const sr_message);

/**
 * @brief Set the payload data in a message
 *
 * This internal function write specific payload data at a provided position in the message.
 *
 * @implementsReq{RASW-157} Endian Definition
 * @implementsReq{RASW-160} General PDU Message Structure
 *
 * @param [in] position Start position where the payload must be written in the message [bytes].
 * @param [in] message_payload Pointer to payload data that is written.
 * @param [inout] sr_message Pointer to a message, where the payload data must be set. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 */
static void SetPayloadDataInMessage(const uint16_t position, const srtyp_SrMessagePayload *const message_payload, srtyp_SrMessage *const sr_message);

/**
 * @brief Get a Uint16 from a specific position in a message
 *
 * This internal function extracts a Uint16 byte by byte from a given position in the little endian format message. If the Uint16 extends over the size of
 * the message from the given start position (position + Uint16 byte size > message size), a ::radef_kInternalError fatal error message is thrown.
 *
 * @implementsReq{RASW-157} Endian Definition
 *
 * @param [in] sr_message Pointer to a message, from where a variable must be extracted. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 * @param [in] position  Start position where the variable must be read in the message [bytes].
 * @return uint16_t Read variable from the specified start position.
 */
static uint16_t GetUint16FromMessage(const srtyp_SrMessage *const sr_message, const uint16_t position);

/**
 * @brief Get a Uint32 from a specific position in a message
 *
 * This internal function extracts a Uint32 byte by byte from a given position in the little endian format message. If the Uint32 extends over the size of
 * the message from the given start position (position + Uint32 byte size > message size), a ::radef_kInternalError fatal error message is thrown.
 *
 * @implementsReq{RASW-157} Endian Definition
 *
 * @param [in] sr_message Pointer to a message, from where a variable must be extracted. If the pointer is NULL, a ::radef_kInternalError fatal error is
 * thrown.
 * @param [in] position  Start position where the variable must be read in the message [bytes].
 * @return uint16_t Read variable from the specified start position.
 */
static uint32_t GetUint32FromMessage(const srtyp_SrMessage *const sr_message, const uint16_t position);

/**
 * @brief Get the length of the configured safety code
 *
 * This internal function returns the byte length of the configured safety code.
 *
 * @implementsReq{RASW-168} Safety Code
 *
 * @return uint16_t length of the safety code [bytes].
 */
static uint16_t GetSafetyCodeLength(void);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

static const uint16_t kSafetyCodeNoneLength = 0U;      ///< Length of no safety code [bytes]
static const uint16_t kSafetyCodeLowerMd4Length = 8U;  ///< Length of lower MD4 safety code [bytes]
static const uint16_t kSafetyCodeFullMd4Length = 16U;  ///< Length of full MD4 safety code [bytes]

// Message Lengths
static const uint16_t kMinMsgLengthConnReqResp = 42U;   ///< Minimum message length for connection request & response message [byte]
static const uint16_t kMinMsgLengthRetrReqResp = 28U;   ///< Minimum message length for retransmission request & response message [byte]
static const uint16_t kMinMsgLengthDiscReq = 32U;       ///< Minimum message length for disconnection request message [byte]
static const uint16_t kMinMsgLengthHeartbeat = 28U;     ///< Minimum message length for heartbeat message [byte]
static const uint16_t kMinMsgLengthEmptyDataMsg = 30U;  ///< Minimum message length for an empty data or retransmitted data message [byte]

// General Positions
static const uint16_t kMsgLengthPosition = 0U;                 ///< Start position for message length in PDU message
static const uint16_t kMsgTypePosition = 2U;                   ///< Start position for message type in PDU message
static const uint16_t kMsgReceiverPosition = 4U;               ///< Start position for receiver identification in PDU message
static const uint16_t kMsgSenderPosition = 8U;                 ///< Start position for sender identification in PDU message
static const uint16_t kMsgSequenceNbrPosition = 12U;           ///< Start position for sequence number in PDU message
static const uint16_t kMsgConfirmedSequenceNbrPosition = 16U;  ///< Start position for confirmed sequence number in PDU message
static const uint16_t kMsgTimeStampPosition = 20U;             ///< Start position for time stamp in PDU message
static const uint16_t kMsgConfirmedTimeStampPosition = 24U;    ///< Start position for confirmed time stamp in PDU message

// ConnReq & ConnResp
static const uint16_t kMsgProtocolVersionByteSize = 4U;    ///< Size of the protocol version [byte]
static const uint16_t kMsgProtocolVersionPosition = 28U;   ///< Start position for protocol version in connection PDU message
static const uint16_t kMsgNsendmaxPosition = 32U;          ///< Start position for Nsendmax in connection PDU message
static const uint16_t kMsgReserveParameterPosition = 34U;  ///< Start position for reserve parameter in connection PDU message

// Data & RetrData
static const uint16_t kMsgPayloadDataSizePosition = 28U;  ///< Start position for payload data size in data PDU message
static const uint16_t kMsgPayloadDataPosition = 30U;      ///< Start position for payload data in data PDU message

// DisReq
static const uint16_t kMsgDetailedInfosDisconnectPosition =
    28U;                                                   ///< Start position for detailed informations for disconnection reason in disconnection PDU message
static const uint16_t kMsgReasonDisconnectPosition = 30U;  ///< Start position for reason for disconnection request in disconnection PDU message

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void srmsg_Init(const srcty_SafetyCodeType configured_safety_code_type, const srcty_Md4InitValue configured_md4_initial_value) {
  // Input parameter check
  raas_AssertTrue(!srmsg_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange((uint32_t)configured_safety_code_type, (uint32_t)srcty_kSafetyCodeTypeMin, (uint32_t)(srcty_kSafetyCodeTypeMax)-1U,
                        radef_kInvalidParameter);

  // Initialize local variables
  srmsg_safety_code_type = configured_safety_code_type;
  srmsg_md4_initial_value = configured_md4_initial_value;

  srmsg_initialized = true;
}

void srmsg_CreateConnReqMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max,
                                srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertTrue(message_header.confirmed_time_stamp == 0U, radef_kInvalidParameter);  // Confirmed timestamp must be 0
  raas_AssertU16InRange(n_send_max, srcty_kMinNSendMax, RADEF_MAX_N_SEND_MAX, radef_kInvalidParameter);
  for (uint8_t index = 0U; index < SRTYP_PROTOCOL_VERSION_SIZE; ++index) {
    raas_AssertU8InRange(protocol_version.version[index], srcty_kProtocolVersionMinValue, srcty_kProtocolVersionMaxValue, radef_kInvalidParameter);
  }
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthConnReqResp + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageConnReq, message_header, sr_message);

  // Set specific connection request message data
  SetProtocolVersionInMessage(protocol_version, sr_message);
  SetUint16InMessage(kMsgNsendmaxPosition, n_send_max, sr_message);
  SetUint64InMessage(kMsgReserveParameterPosition, 0UL, sr_message);
}

void srmsg_CreateConnRespMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max,
                                 srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertU16InRange(n_send_max, srcty_kMinNSendMax, RADEF_MAX_N_SEND_MAX, radef_kInvalidParameter);
  for (uint8_t index = 0U; index < SRTYP_PROTOCOL_VERSION_SIZE; ++index) {
    raas_AssertU8InRange(protocol_version.version[index], srcty_kProtocolVersionMinValue, srcty_kProtocolVersionMaxValue, radef_kInvalidParameter);
  }
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthConnReqResp + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageConnResp, message_header, sr_message);

  // Set specific connection request message data
  SetProtocolVersionInMessage(protocol_version, sr_message);
  SetUint16InMessage(kMsgNsendmaxPosition, n_send_max, sr_message);
  SetUint64InMessage(kMsgReserveParameterPosition, 0UL, sr_message);
}

void srmsg_CreateDataMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload *const message_payload,
                             srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);
  raas_AssertU16InRange(message_payload->payload_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, radef_kInvalidParameter);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthEmptyDataMsg + message_payload->payload_size + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageData, message_header,
                            sr_message);

  // Set specific payload data
  SetUint16InMessage(kMsgPayloadDataSizePosition, message_payload->payload_size, sr_message);
  SetPayloadDataInMessage(kMsgPayloadDataPosition, message_payload, sr_message);
}

void srmsg_CreateRetrDataMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload *const message_payload,
                                 srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);
  raas_AssertU16InRange(message_payload->payload_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, radef_kInvalidParameter);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthEmptyDataMsg + message_payload->payload_size + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageRetrData,
                            message_header, sr_message);

  // Set specific retransmission payload data
  SetUint16InMessage(kMsgPayloadDataSizePosition, message_payload->payload_size, sr_message);
  SetPayloadDataInMessage(kMsgPayloadDataPosition, message_payload, sr_message);
}

void srmsg_CreateRetrReqMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthRetrReqResp + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageRetrReq, message_header, sr_message);
}

void srmsg_CreateRetrRespMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthRetrReqResp + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageRetrResp, message_header, sr_message);
}

void srmsg_CreateHeartbeatMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthHeartbeat + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageHb, message_header, sr_message);
}

void srmsg_CreateDiscReqMessage(const srtyp_SrMessageHeaderCreate message_header, const uint16_t detailed_reason, const sraty_DiscReason reason,
                                srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertU16InRange((uint16_t)reason, (uint16_t)sraty_kDiscReasonMin, (uint16_t)sraty_kDiscReasonMax - 1U, radef_kInvalidParameter);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Set message header data
  SetMessageHeaderInMessage(kMinMsgLengthDiscReq + GetSafetyCodeLength(), (uint16_t)srtyp_kSrMessageDiscReq, message_header, sr_message);

  // Set specific disconnect data
  SetUint16InMessage(kMsgDetailedInfosDisconnectPosition, detailed_reason, sr_message);
  SetUint16InMessage(kMsgReasonDisconnectPosition, (uint16_t)reason, sr_message);
}

void srmsg_UpdateMessageHeader(const srtyp_SrMessageHeaderUpdate message_header_update, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);
  raas_AssertU16InRange(sr_message->message_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + GetSafetyCodeLength(), RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE,
                        radef_kInvalidParameter);

  // Set header data in message
  SetUint32InMessage(kMsgConfirmedSequenceNbrPosition, message_header_update.confirmed_sequence_number, sr_message);
  SetUint32InMessage(kMsgTimeStampPosition, message_header_update.time_stamp, sr_message);

  // Calculate md4 with new data if safety code is used
  uint16_t safety_code_length = GetSafetyCodeLength();
  if (safety_code_length > 0U) {
    srmd4_Md4 md4;
    uint16_t message_length_without_safety_code = (uint16_t)(sr_message->message_size - safety_code_length);
    srmd4_CalculateMd4(srmsg_md4_initial_value, message_length_without_safety_code, sr_message->message, &md4);

    for (uint8_t index = 0U; index < safety_code_length; ++index) {
      //lint -save -e661 (Out of bounds pointer access is checked by input parameter check)
      sr_message->message[message_length_without_safety_code + index] = md4.md4[index];
      //lint -restore
    }
  }
}

radef_RaStaReturnCode srmsg_CheckMessage(const srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);
  raas_AssertU16InRange(
      sr_message->message_size, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + GetSafetyCodeLength(),
      RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + RADEF_SR_LAYER_APPLICATION_MESSAGE_LENGTH_SIZE + RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE + GetSafetyCodeLength(),
      radef_kInvalidParameter);

  bool ok = true;
  radef_RaStaReturnCode ret = radef_kNoError;

  // Check safety code
  uint16_t safety_code_length = GetSafetyCodeLength();
  if (safety_code_length > 0U) {
    srmd4_Md4 md4;
    uint16_t message_length_without_safety_code = (uint16_t)(sr_message->message_size - safety_code_length);
    srmd4_CalculateMd4(srmsg_md4_initial_value, message_length_without_safety_code, sr_message->message, &md4);

    for (uint8_t index = 0U; index < safety_code_length; ++index) {
      //lint -save -e661 (Out of bounds pointer access is checked by input parameter check)
      if (sr_message->message[message_length_without_safety_code + index] != md4.md4[index]) {
        //lint -restore
        ret = radef_kInvalidMessageMd4;
        ok = false;
        break;
      }
    }
  }

  // Check message type
  //lint -save -e9030 (numeric value from external interface is casted into enum type that has const defined values)
  srtyp_SrMessageType message_type = (srtyp_SrMessageType)GetUint16FromMessage(sr_message, kMsgTypePosition);
  //lint -restore
  if (ok && !IsMessageTypeValid(message_type)) {
    ret = radef_kInvalidMessageType;
    ok = false;
  }

  // Check message size
  uint16_t message_size = 0U;
  if (ok) {
    message_size = GetUint16FromMessage(sr_message, kMsgLengthPosition);
    if (message_size != sr_message->message_size) {
      ret = radef_kInvalidMessageSize;
      ok = false;
    } else if ((message_type == srtyp_kSrMessageData) || (message_type == srtyp_kSrMessageRetrData)) {
      // Extract message payload size
      const uint16_t kMessagePayloadSize = GetUint16FromMessage(sr_message, kMsgPayloadDataSizePosition);
      // Check that message payload size is exactly message size - header size - message length size - safety code size
      if (kMessagePayloadSize != (message_size - (RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + RADEF_SR_LAYER_APPLICATION_MESSAGE_LENGTH_SIZE + safety_code_length))) {
        ret = radef_kInvalidMessageSize;
        ok = false;
      }
    } else {
      // Nothing to do here
    }
  }

  // Check correct message size for all message types
  if (ok) {
    uint16_t expected_message_size = 0U;
    switch (message_type) {
      case srtyp_kSrMessageConnReq:
      case srtyp_kSrMessageConnResp:
        expected_message_size = kMinMsgLengthConnReqResp + GetSafetyCodeLength();
        break;
      case srtyp_kSrMessageRetrReq:
      case srtyp_kSrMessageRetrResp:
        expected_message_size = kMinMsgLengthRetrReqResp + GetSafetyCodeLength();
        break;
      case srtyp_kSrMessageHb:
        expected_message_size = kMinMsgLengthHeartbeat + GetSafetyCodeLength();
        break;
      case srtyp_kSrMessageDiscReq:
        expected_message_size = kMinMsgLengthDiscReq + GetSafetyCodeLength();
        break;
      case srtyp_kSrMessageData:
      case srtyp_kSrMessageRetrData:
        // Subsequent addition to pass pc lint note 931
        expected_message_size = kMinMsgLengthEmptyDataMsg;
        expected_message_size += GetUint16FromMessage(sr_message, kMsgPayloadDataSizePosition);
        expected_message_size += GetSafetyCodeLength();
        break;
        // no coverage start
      default:
        rasys_FatalError(radef_kInternalError);
        break;
        // no coverage end
    }
    if (expected_message_size != message_size) {
      ret = radef_kInvalidMessageSize;
    }
  }

  return ret;
}

void srmsg_GetMessageHeader(const srtyp_SrMessage *const sr_message, srtyp_SrMessageHeader *const message_header) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);
  raas_AssertNotNull(message_header, radef_kInvalidParameter);

  // Extract message header data
  message_header->message_length = GetUint16FromMessage(sr_message, kMsgLengthPosition);
  //lint -save -e9030 (numeric value from external interface is casted into enum type that has const defined values)
  message_header->message_type = (srtyp_SrMessageType)GetUint16FromMessage(sr_message, kMsgTypePosition);
  //lint -restore
  message_header->receiver_id = GetUint32FromMessage(sr_message, kMsgReceiverPosition);
  message_header->sender_id = GetUint32FromMessage(sr_message, kMsgSenderPosition);
  message_header->sequence_number = GetUint32FromMessage(sr_message, kMsgSequenceNbrPosition);
  message_header->confirmed_sequence_number = GetUint32FromMessage(sr_message, kMsgConfirmedSequenceNbrPosition);
  message_header->time_stamp = GetUint32FromMessage(sr_message, kMsgTimeStampPosition);
  message_header->confirmed_time_stamp = GetUint32FromMessage(sr_message, kMsgConfirmedTimeStampPosition);
}

srtyp_SrMessageType srmsg_GetMessageType(const srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Extract message type
  //lint -save -e9030 (numeric value from external interface is casted into enum type that has const defined values)
  return (srtyp_SrMessageType)GetUint16FromMessage(sr_message, kMsgTypePosition);
  //lint -restore
}

uint32_t srmsg_GetMessageSequenceNumber(const srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);

  // Extract message sequence number
  return GetUint32FromMessage(sr_message, kMsgSequenceNbrPosition);
}

void srmsg_GetDataMessagePayload(const srtyp_SrMessage *const sr_message, srtyp_SrMessagePayload *const message_payload) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);
  //lint -save -e9030 (numeric value from external interface is casted into enum type that has const defined values)
  srtyp_SrMessageType message_type = (srtyp_SrMessageType)GetUint16FromMessage(sr_message, kMsgTypePosition);
  //lint -restore
  raas_AssertTrue((message_type == srtyp_kSrMessageData) || (message_type == srtyp_kSrMessageRetrData), radef_kInvalidParameter);

  // Extract message data
  uint16_t message_payload_size = GetUint16FromMessage(sr_message, kMsgPayloadDataSizePosition);
  raas_AssertU16InRange(message_payload_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, radef_kInvalidParameter);

  message_payload->payload_size = message_payload_size;
  for (uint16_t index = 0U; index < message_payload_size; ++index) {
    message_payload->payload[index] = sr_message->message[kMsgPayloadDataPosition + index];
  }
}

void srmsg_GetConnMessageData(const srtyp_SrMessage *const sr_message, srtyp_ProtocolVersion *const protocol_version, uint16_t *const n_send_max) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);
  raas_AssertNotNull(protocol_version, radef_kInvalidParameter);
  raas_AssertNotNull(n_send_max, radef_kInvalidParameter);
  //lint -save -e9030 (numeric value from external interface is casted into enum type that has const defined values)
  srtyp_SrMessageType message_type = (srtyp_SrMessageType)GetUint16FromMessage(sr_message, kMsgTypePosition);
  //lint -restore
  raas_AssertTrue((message_type == srtyp_kSrMessageConnReq) || (message_type == srtyp_kSrMessageConnResp), radef_kInvalidParameter);

  // Extract protocol version
  for (uint8_t index = 0U; index < SRTYP_PROTOCOL_VERSION_SIZE; ++index) {
    raas_AssertU8InRange(sr_message->message[kMsgProtocolVersionPosition + index], srcty_kProtocolVersionMinValue, srcty_kProtocolVersionMaxValue,
                         radef_kInternalError);
    protocol_version->version[index] = sr_message->message[kMsgProtocolVersionPosition + index];
  }

  // Extract Nsendmax
  *n_send_max = GetUint16FromMessage(sr_message, kMsgNsendmaxPosition);
}

void srmsg_GetDiscMessageData(const srtyp_SrMessage *const sr_message, uint16_t *const detailed_reason, sraty_DiscReason *const reason) {
  // Input parameter check
  raas_AssertTrue(srmsg_initialized, radef_kNotInitialized);
  raas_AssertNotNull(sr_message, radef_kInvalidParameter);
  raas_AssertNotNull(detailed_reason, radef_kInvalidParameter);
  raas_AssertNotNull(reason, radef_kInvalidParameter);
  //lint -save -e9030 (numeric value from external interface is casted into enum type that has const defined values)
  srtyp_SrMessageType message_type = (srtyp_SrMessageType)GetUint16FromMessage(sr_message, kMsgTypePosition);
  //lint -restore
  raas_AssertTrue((message_type == srtyp_kSrMessageDiscReq), radef_kInvalidParameter);

  // Extract detailed reason
  *detailed_reason = GetUint16FromMessage(sr_message, kMsgDetailedInfosDisconnectPosition);

  // Extract reason for disconnection request
  //lint -save -e9030 (numeric value from external interface is casted into enum type that has const defined values)
  *reason = (sraty_DiscReason)GetUint16FromMessage(sr_message, kMsgReasonDisconnectPosition);
  //lint -restore
  raas_AssertU32InRange((uint32_t)*reason, (uint32_t)sraty_kDiscReasonMin, (uint32_t)sraty_kDiscReasonMax - 1U, radef_kInvalidParameter);
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static bool IsMessageTypeValid(srtyp_SrMessageType message_type) {
  bool isValid = false;

  switch (message_type) {
    case srtyp_kSrMessageConnReq:
    case srtyp_kSrMessageConnResp:
    case srtyp_kSrMessageRetrReq:
    case srtyp_kSrMessageRetrResp:
    case srtyp_kSrMessageDiscReq:
    case srtyp_kSrMessageHb:
    case srtyp_kSrMessageData:
    case srtyp_kSrMessageRetrData:
      isValid = true;
      break;
    default:
      isValid = false;
      break;
  }

  return isValid;
}

static void SetUint16InMessage(const uint16_t position, const uint16_t data, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertTrue((position + srcty_kByteCountUint16) <= sr_message->message_size, radef_kInternalError);

  // Write variable in message (little endian format)
  sr_message->message[position + 1U] = (uint8_t)((data >> 8) & 0xFFU);
  sr_message->message[position] = (uint8_t)(data & 0xFFU);
}

static void SetUint32InMessage(const uint16_t position, const uint32_t data, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertTrue((position + srcty_kByteCountUint32) <= sr_message->message_size, radef_kInternalError);

  // Write variable in message (little endian format)
  sr_message->message[position + 3U] = (uint8_t)((data >> 24) & 0xFFU);
  sr_message->message[position + 2U] = (uint8_t)((data >> 16) & 0xFFU);
  sr_message->message[position + 1U] = (uint8_t)((data >> 8) & 0xFFU);
  sr_message->message[position] = (uint8_t)(data & 0xFFU);
}

static void SetUint64InMessage(const uint16_t position, const uint64_t data, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertTrue((position + srcty_kByteCountUint64) <= sr_message->message_size, radef_kInternalError);

  // Write variable in message (little endian format)
  sr_message->message[position + 7U] = (uint8_t)((data >> 56) & 0xFFU);
  sr_message->message[position + 6U] = (uint8_t)((data >> 48) & 0xFFU);
  sr_message->message[position + 5U] = (uint8_t)((data >> 40) & 0xFFU);
  sr_message->message[position + 4U] = (uint8_t)((data >> 32) & 0xFFU);
  sr_message->message[position + 3U] = (uint8_t)((data >> 24) & 0xFFU);
  sr_message->message[position + 2U] = (uint8_t)((data >> 16) & 0xFFU);
  sr_message->message[position + 1U] = (uint8_t)((data >> 8) & 0xFFU);
  sr_message->message[position] = (uint8_t)(data & 0xFFU);
}

static void SetMessageHeaderInMessage(const uint16_t message_length, const uint16_t message_type, const srtyp_SrMessageHeaderCreate message_header,
                                      srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertU16InRange(message_length, RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + GetSafetyCodeLength(), RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);
  raas_AssertU32InRange((uint32_t)message_type, (uint32_t)srtyp_kSrMessageMin, (uint32_t)(srtyp_kSrMessageMax)-1U, radef_kInternalError);

  // Set message size
  sr_message->message_size = message_length;

  // Write message header in message
  SetUint16InMessage(kMsgLengthPosition, message_length, sr_message);
  SetUint16InMessage(kMsgTypePosition, message_type, sr_message);
  SetUint32InMessage(kMsgReceiverPosition, message_header.receiver_id, sr_message);
  SetUint32InMessage(kMsgSenderPosition, message_header.sender_id, sr_message);
  SetUint32InMessage(kMsgSequenceNbrPosition, message_header.sequence_number, sr_message);
  SetUint32InMessage(kMsgConfirmedSequenceNbrPosition, 0U, sr_message);  // Set to 0 for initialization, must be updated by core class before sending
  SetUint32InMessage(kMsgTimeStampPosition, 0U, sr_message);             // Set to 0 for initialization, must be updated by core class before sending
  SetUint32InMessage(kMsgConfirmedTimeStampPosition, message_header.confirmed_time_stamp, sr_message);
}

static void SetProtocolVersionInMessage(const srtyp_ProtocolVersion protocol_version, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertTrue((kMsgProtocolVersionPosition + kMsgProtocolVersionByteSize) <= sr_message->message_size, radef_kInternalError);
  for (uint8_t index = 0U; index < SRTYP_PROTOCOL_VERSION_SIZE; ++index) {
    raas_AssertU8InRange(protocol_version.version[index], srcty_kProtocolVersionMinValue, srcty_kProtocolVersionMaxValue, radef_kInternalError);
  }

  // Write protocol version in message
  for (uint8_t index = 0U; index < SRTYP_PROTOCOL_VERSION_SIZE; ++index) {
    sr_message->message[kMsgProtocolVersionPosition + index] = protocol_version.version[index];
  }
}

static void SetPayloadDataInMessage(const uint16_t position, const srtyp_SrMessagePayload *const message_payload, srtyp_SrMessage *const sr_message) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertNotNull(message_payload, radef_kInternalError);
  raas_AssertTrue((position + message_payload->payload_size) <= sr_message->message_size, radef_kInternalError);

  // Write payload data in message
  for (uint16_t index = 0U; index < message_payload->payload_size; ++index) {
    sr_message->message[position + index] = message_payload->payload[index];
  }
}

static uint16_t GetUint16FromMessage(const srtyp_SrMessage *const sr_message, const uint16_t position) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertTrue((position + srcty_kByteCountUint16) <= sr_message->message_size, radef_kInternalError);

  // Get variable from message (little endian format)
  uint16_t data = 0U;
  for (uint16_t index = 0U; index < srcty_kByteCountUint16; ++index) {
    data += (uint16_t)sr_message->message[position + index] << (index * 8U);
  }

  return data;
}

static uint32_t GetUint32FromMessage(const srtyp_SrMessage *const sr_message, const uint16_t position) {
  // Input parameter check
  raas_AssertNotNull(sr_message, radef_kInternalError);
  raas_AssertTrue((position + srcty_kByteCountUint32) <= sr_message->message_size, radef_kInternalError);

  // Get variable from message (little endian format)
  uint32_t data = 0U;
  for (uint16_t index = 0U; index < srcty_kByteCountUint32; ++index) {
    data += (uint32_t)sr_message->message[position + index] << (index * 8U);
  }

  return data;
}

static uint16_t GetSafetyCodeLength() {
  uint16_t safety_code_length = 0U;

  switch (srmsg_safety_code_type) {
    case srcty_kSafetyCodeTypeNone:
      safety_code_length = kSafetyCodeNoneLength;
      break;
    case srcty_kSafetyCodeTypeLowerMd4:
      safety_code_length = kSafetyCodeLowerMd4Length;
      break;
    case srcty_kSafetyCodeTypeFullMd4:
      safety_code_length = kSafetyCodeFullMd4Length;
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);
      break;
      // no coverage end
  }

  return safety_code_length;
}
