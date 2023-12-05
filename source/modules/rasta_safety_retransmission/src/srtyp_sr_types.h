/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srtyp_sr_types.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_types
 * @{
 *
 * @brief Internal type definitions of RaSTA SafRetL.
 *
 * This module defines the internal data structures used for the RaSTA SafRetL.
 *
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-560} sr_state_machine Events
 */
#ifndef SRTYP_SAFETY_RETRANSMISSION_TYPES_H_
#define SRTYP_SAFETY_RETRANSMISSION_TYPES_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Size of RaSTA protocol version array.
 */
#define SRTYP_PROTOCOL_VERSION_SIZE (4U)

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Enum for the events of a RaSTA connection state machine.
 *
 * @implementsReq{RASW-560} sr_state_machine Events
 */
//lint -esym(849, srtyp_kConnEventMin) (used only for parameter range checking)
//lint -esym(788, srtyp_kConnEventMax) (used only for parameter range checking)
//lint -save -e769 (enumeration is defined globally)
typedef enum {
  srtyp_kConnEventMin = 0,           ///< Min value for connection events enum
  srtyp_kConnEventNone = 0,          ///< No connection event
  srtyp_kConnEventOpen,              ///< Open connection event
  srtyp_kConnEventClose,             ///< Close connection event
  srtyp_kConnEventSendData,          ///< Send data  event
  srtyp_kConnEventConnReqReceived,   ///< Connection request received event
  srtyp_kConnEventConnRespReceived,  ///< Connection response received event
  srtyp_kConnEventRetrReqReceived,   ///< Retransmission request received event
  srtyp_kConnEventRetrRespReceived,  ///< Retransmission response received event
  srtyp_kConnEventDiscReqReceived,   ///< Disconnection request received event
  srtyp_kConnEventHbReceived,        ///< Heartbeat received event
  srtyp_kConnEventDataReceived,      ///< Data received event
  srtyp_kConnEventRetrDataReceived,  ///< Retransmitted data received event
  srtyp_kConnEventSendHb,            ///< Send heartbeat event
  srtyp_kConnEventTimeout,           ///< Connection timeout event
  srtyp_kConnEventMax                ///< Max value for connection events enum
} srtyp_ConnectionEvents;
//lint -restore

/**
 * @brief Enum for SafRetL PDU message types.
 */
//lint -esym(849, srtyp_kSrMessageMin) (used only for parameter range checking)
//lint -esym(788, srtyp_kSrMessageMax) (used only for parameter range checking)
typedef enum {
  srtyp_kSrMessageMin = 6200U,       ///< Min value for sr message type enum
  srtyp_kSrMessageConnReq = 6200U,   ///< Connection request message type
  srtyp_kSrMessageConnResp = 6201U,  ///< Connection response message type
  srtyp_kSrMessageRetrReq = 6212U,   ///< Retransmission request message type
  srtyp_kSrMessageRetrResp = 6213U,  ///< Retransmission response message type
  srtyp_kSrMessageDiscReq = 6216U,   ///< Disconnection request message type
  srtyp_kSrMessageHb = 6220U,        ///< Heartbeat message type
  srtyp_kSrMessageData = 6240U,      ///< Data message type
  srtyp_kSrMessageRetrData = 6241U,  ///< Retransmitted data message type
  srtyp_kSrMessageMax = 6300U        ///< Max value for sr message type enum
} srtyp_SrMessageType;

/**
 * @brief Typedef for a SafRetL PDU message header.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint16_t message_length;  ///< Message length [bytes]. Valid range: ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <= value <= ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE
  srtyp_SrMessageType message_type;    ///< Message type. All enum entries of ::srtyp_SrMessageType are valid and usable.
  uint32_t receiver_id;                ///< Receiver identification. The full value range is valid and usable.
  uint32_t sender_id;                  ///< Sender identification. The full value range is valid and usable.
  uint32_t sequence_number;            ///< SN_PDU: Sequence number. The full value range is valid and usable.
  uint32_t confirmed_sequence_number;  ///< CS_PDU: Confirmed sequence number. The full value range is valid and usable.
  uint32_t time_stamp;                 ///< TS_PDU: Time stamp [ms]. The full value range is valid and usable.
  uint32_t confirmed_time_stamp;       ///< CTS_PDU: Confirmed time stamp [ms]. The full value range is valid and usable.
} srtyp_SrMessageHeader;
//lint -restore

/**
 * @brief Typedef for SafRetL PDU message header data parameters for creating a message.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint32_t receiver_id;           ///< Receiver identification. The full value range is valid and usable.
  uint32_t sender_id;             ///< Sender identification. The full value range is valid and usable.
  uint32_t sequence_number;       ///< SN_PDU: Sequence number. The full value range is valid and usable.
  uint32_t confirmed_time_stamp;  ///< CTS_PDU: Confirmed time stamp [ms]. The full value range is valid and usable.
} srtyp_SrMessageHeaderCreate;
//lint -restore

/**
 * @brief Typedef for SafRetL PDU message header data parameters for updating a message header before sending the message.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint32_t confirmed_sequence_number;  ///< CS_PDU: Confirmed sequence number. The full value range is valid and usable.
  uint32_t time_stamp;                 ///< TS_PDU: Time stamp [ms]. The full value range is valid and usable.
} srtyp_SrMessageHeaderUpdate;
//lint -restore

/**
 * @brief Typedef for a SafRetL PDU message.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint16_t message_size;  ///< Used message size [bytes]. Valid range: ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <= value <= ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE
  uint8_t message[RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE];  ///< Message buffer. For the message data the full value range is valid and usable.
} srtyp_SrMessage;
//lint -restore

/**
 * @brief Typedef for a SafRetL PDU message payload.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint16_t payload_size;  ///< Used payload size [bytes]. Valid range: ::srcty_kMinSrLayerPayloadDataSize <= value <= RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE.
  uint8_t payload[RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE];  ///< Payload buffer. For the message payload the full value range is valid and usable.
} srtyp_SrMessagePayload;
//lint -restore

/**
 * @brief Typedef for RaSTA protocol version array.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint8_t version[SRTYP_PROTOCOL_VERSION_SIZE];  ///< Array containing the version in ASCII encoding. The first two bytes represent the major version, the
                                                 ///< second two bytes represent the minor version. Valid range for every char: ::srcty_kProtocolVersionMinValue
                                                 ///< <= value <= ::srcty_kProtocolVersionMaxValue.
} srtyp_ProtocolVersion;
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

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRTYP_SAFETY_RETRANSMISSION_TYPES_H_
