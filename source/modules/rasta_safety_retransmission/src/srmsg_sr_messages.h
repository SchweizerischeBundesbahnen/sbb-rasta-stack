/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmsg_sr_messages.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 87bc446bbb57fb82711f48fb5ccb62bccae929e8
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4935: Messages module must check message size and payload data size correctly,07.12.2022, N. Andres}
 *
 * @addtogroup sr_messages
 * @{
 *
 * @brief Interface of RaSTA SafRetL messages module.
 *
 * This module provides all needed functionality for SafRetL messages. This contains the following:
 * - validate a message
 * - create all different types of SafRetL messages
 * - update header information of a SafRetL message
 * - extract header or specific message type informations/data
 * .
 *
 * @implementsReq{RASW-615} Component sr_messages Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRMSG_SAFETY_RETRANSMISSION_MESSAGES_H_
#define SRMSG_SAFETY_RETRANSMISSION_MESSAGES_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Initialize SafRetL messages module.
 *
 * This function is used to initialize the messages module. It saves the passed safety code type and the MD4 initial value. A fatal error is raised, if this
 * function is called multiple times.
 *
 * @pre The messages module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-629} Init sr_messages Function
 *
 * @param [in] configured_safety_code_type Configured RaSTA safety code type. All enum entries of ::srcty_SafetyCodeType are valid and usable.
 * @param [in] configured_md4_initial_value Configured MD4 initial value. The full value range is valid and usable.
 */
void srmsg_Init(const srcty_SafetyCodeType configured_safety_code_type, const srcty_Md4InitValue configured_md4_initial_value);

/**
 * @brief Create a new SafRetL connection request message.
 *
 * This function creates a SafRetL connection request message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID, receiver ID
 * and confirmed_time_stamp), the protocol version ::srtyp_ProtocolVersion and its own receive buffer size. The caller must provide a pointer to a memory block,
 * where the function can store the newly created message. The function sets all ConnReq information, including the message length, message type, sender
 * & receiver ID, confirmed time stamp, protocol version and the Nsendmax.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-617} Create Connection Request Message Function
 * @implementsReq{RASW-170} Connection Request Message Structure
 * @implementsReq{RASW-172} Message Length
 * @implementsReq{RASW-173} Protocol Version
 * @implementsReq{RASW-174} Nsendmax
 * @implementsReq{RASW-175} Reserve
 *
 * @param [in] message_header Message header data to create a new message. The confirmed time stamp must be 0, otherwise a ::radef_kInvalidParameter fatal error
 * is thrown. For the other sub-parameters of the header, the full value range is valid and usable.
 * @param [in] protocol_version Array containing the protocol version. Valid range for every digit of the version: ::srcty_kProtocolVersionMinValue <= value <=
 * ::srcty_kProtocolVersionMaxValue. If the value is outside this range, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [in] n_send_max Nsendmax (receive buffer size). Valid range: ::srcty_kMinNSendMax <= value <= ::RADEF_MAX_N_SEND_MAX. If the value is outside this
 * range, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] sr_message Pointer to memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateConnReqMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max,
                                srtyp_SrMessage *const sr_message);

/**
 * @brief Create a new SafRetL connection response message.
 *
 * This function creates a SafRetL connection response message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID, receiver ID
 * and confirmed_time_stamp), the protocol version ::srtyp_ProtocolVersion and its own receive buffer size. The caller must provide a pointer to a memory block,
 * where the function can store the newly created message. The function sets all ConnResp information, including the message length, message type, sender
 * & receiver ID, confirmed time stamp, protocol version and the Nsendmax.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-618} Create Connection Response Message Function
 * @implementsReq{RASW-171} Connection Response Message Structure
 * @implementsReq{RASW-172} Message Length
 * @implementsReq{RASW-173} Protocol Version
 * @implementsReq{RASW-174} Nsendmax
 * @implementsReq{RASW-175} Reserve
 *
 * @param [in] message_header Message header data to create a new message. For all sub-parameters of the header, the full value range is valid and usable.
 * @param [in] protocol_version Array containing the protocol version. Valid range for every digit of the version: ::srcty_kProtocolVersionMinValue <= value <=
 * ::srcty_kProtocolVersionMaxValue. If the value is outside this range, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [in] n_send_max Nsendmax (receive buffer size). Valid range: ::srcty_kMinNSendMax <= value <= ::RADEF_MAX_N_SEND_MAX. If the value is outside this
 * range, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] sr_message Pointer to memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateConnRespMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max,
                                 srtyp_SrMessage *const sr_message);

/**
 * @brief Create a new SafRetL data message.
 *
 * This function creates a SafRetL data message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID, receiver ID and
 * confirmed_time_stamp) and message payload (::srtyp_SrMessagePayload containing payload size and the payload). The caller must provide a pointer to a memory
 * block, where the function can store the newly created message. The function sets all Data information, including the message length, message type,
 * sender & receiver ID, confirmed time stamp, payload length and payload data.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-623} Create Data Message Function
 * @implementsReq{RASW-191} Data Message Structure
 * @implementsReq{RASW-192} Message Length
 * @implementsReq{RASW-193} Size of Message Data
 * @implementsReq{RASW-194} Data Message
 *
 * @param [in] message_header Message header data to create a new message. For all sub-parameters of the header, the full value range is valid and usable.
 * @param [in] message_payload Pointer to message payload structure. The payload size has a valid range from ::srcty_kMinSrLayerPayloadDataSize <= value <=
 * ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, a ::radef_kInvalidParameter fatal error is thrown otherwise. For the message payload the full value range is valid
 * and usable.
 * @param [out] sr_message Pointer to memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateDataMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload *const message_payload,
                             srtyp_SrMessage *const sr_message);

/**
 * @brief Create a new SafRetL retransmitted data message.
 *
 * This function creates a SafRetL retransmitted data message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID, receiver ID
 * and confirmed_time_stamp) and message payload (::srtyp_SrMessagePayload containing payload size and the payload). The caller must provide a pointer to a
 * memory block, where the function can store the newly created message. The function sets all RetrData information, including the message length, message type,
 * sender & receiver ID, confirmed time stamp, payload length and payload data.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-624} Create Retransmitted Data Message Function
 * @implementsReq{RASW-196} Retransmitted Data Message Structure
 * @implementsReq{RASW-192} Message Length
 * @implementsReq{RASW-193} Size of Message Data
 * @implementsReq{RASW-194} Data Message
 *
 * @param [in] message_header Message header data to create a new message. For all sub-parameters of the header, the full value range is valid and usable.
 * @param [in] message_payload Pointer to message payload structure. The payload size has a valid range from ::srcty_kMinSrLayerPayloadDataSize <= value <=
 * ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, a ::radef_kInvalidParameter fatal error is thrown otherwise. For the message payload the full value range is valid
 * and usable.
 * @param [out] sr_message Pointer to memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateRetrDataMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload *const message_payload,
                                 srtyp_SrMessage *const sr_message);

/**
 * @brief Create a new SafRetL retransmission request message.
 *
 * This function creates a SafRetL retransmitted request message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID, receiver
 * ID and confirmed_time_stamp). The caller must provide a pointer to a memory block, where the function can store the newly created message. The function sets
 * all RetrReq information, including the message length, message type, sender & receiver ID and confirmed time stamp.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-619} Create Retransmission Request Message Function
 * @implementsReq{RASW-177} Retransmission Request Message Structure
 * @implementsReq{RASW-178} Message Length
 *
 * @param [in] message_header Message header data to create a new message. For all sub-parameters of the header, the full value range is valid and usable.
 * @param [out] sr_message Pointer to memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateRetrReqMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage *const sr_message);

/**
 * @brief Create a new SafRetL retransmission response message.
 *
 * This function creates a SafRetL retransmitted response message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID, receiver
 * ID and confirmed_time_stamp). The caller must provide a pointer to a memory block, where the function can store the newly created message. The function sets
 * all RetrResp information, including the message length, message type, sender & receiver ID and confirmed time stamp.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-620} Create Retransmission Response Message Function
 * @implementsReq{RASW-180} Retransmission Response Message Structure
 * @implementsReq{RASW-178} Message Length
 *
 * @param [in] message_header Message header data to create a new message. For all sub-parameters of the header, the full value range is valid and usable.
 * @param [out] sr_message Pointer to memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateRetrRespMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage *const sr_message);

/**
 * @brief Create a new SafRetL heartbeat message.
 *
 * This function creates a SafRetL heartbeat message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID,
 * receiver ID and confirmed_time_stamp). The caller must provide a pointer to a memory block, where the function can store the newly created message. The
 * function sets all HB information, including the message length, message type, sender & receiver ID and confirmed time stamp.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-622} Create Heartbeat Message Function
 * @implementsReq{RASW-188} Heartbeat Message Structure
 * @implementsReq{RASW-189} Message Length
 *
 * @param [in] message_header Message header data to create a new message. For all sub-parameters of the header, the full value range is valid and usable.
 * @param [out] sr_message Pointer to memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateHeartbeatMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage *const sr_message);

/**
 * @brief Create a new SafRetL disconnection request message.
 *
 * This function creates a SafRetL disconnection request message with the passed header structure (::srtyp_SrMessageHeaderCreate containing sender ID, receiver
 * ID and confirmed_time_stamp), detailed disconnection information and a disconnect reason (::sraty_DiscReason). The caller must provide a pointer to a memory
 * block, where the function can store the newly created message. The function sets all DiscReq information, including the message length, message type, sender
 * & receiver ID, confirmed time stamp, detailed disconnection information and reason.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-621} Create Disconnection Request Message Function
 * @implementsReq{RASW-183} Disconnection Request Message Structure
 * @implementsReq{RASW-184} Message Length
 * @implementsReq{RASW-185} Detailed Information regarding the Reason for the Disconnection Request
 * @implementsReq{RASW-186} Reason for Disconnection Request
 *
 * @param [in] message_header Message header data to create a new message. For all sub-parameters of the header, the full value range is valid and usable.
 * @param [in] detailed_reason Detailed reason for disconnection from application. This is specific to a RaSTA network and can be defined for all parties.
 * The full value range is valid and usable.
 * @param [in] reason Reason for disconnection. The reason has a valid range from ::sraty_kDiscReasonMin <= value < ::sraty_kDiscReasonMax. If the value is
 * outside this range, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] sr_message Pointer to a memory block to save the new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_CreateDiscReqMessage(const srtyp_SrMessageHeaderCreate message_header, const uint16_t detailed_reason, const sraty_DiscReason reason,
                                srtyp_SrMessage *const sr_message);

/**
 * @brief Update a SafRetL message header and calculate the safety code to prepare the message for sending.
 *
 * This function updates the header data (::srtyp_SrMessageHeaderUpdate containing the sequence number, confirmed sequence number and timestamp) in the provided
 * SafRetL message and then calculates and sets the safety code, if it is configured.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-630} Update Message Header Function
 * @implementsReq{RASW-164} Sequence Number
 * @implementsReq{RASW-165} Confirmed Sequence Number
 * @implementsReq{RASW-166} Time Stamp
 * @implementsReq{RASW-168} Safety Code
 *
 * @param [in] message_header_update Message header data to update. For all sub-parameters of the ::srtyp_SrMessageHeaderUpdate structure, the full value range
 * is valid and usable.
 * @param [inout] sr_message Pointer to a memory block to save new message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown. The
 * message size is checked if it is in the valid range of (::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + safety code length) <= value <=
 * ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, otherwise a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_UpdateMessageHeader(const srtyp_SrMessageHeaderUpdate message_header_update, srtyp_SrMessage *const sr_message);

/**
 * @brief Check MD4, message type and message size of a SafRetL PDU message.
 *
 * This function checks the validity of a provided SafRetL message. This means checking the following:
 * - Safety code (if it is configured). The Safety code inside the message must be identical to the calculated one.
 * - Message type. The message type must match to one of the known ::srtyp_SrMessageType enum entries.
 * - Message size. The message size must match the expected size following the set message type.
 * - In case of Data/RetrData: Message payload data must match related to the passed message size (message payload size = message size -
 * ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE - ::RADEF_SR_LAYER_APPLICATION_MESSAGE_LENGTH_SIZE - safety code length).
 * .
 * All tests must be successful for a message to be valid.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-160} General PDU Message Structure
 * @implementsReq{RASW-616} Check Message Function
 *
 * @param [in] sr_message pointer to message to check. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown. The message size is checked if
 * it is in the valid range of (::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + safety code length) <= value <= (::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE +
 * ::RADEF_SR_LAYER_APPLICATION_MESSAGE_LENGTH_SIZE + ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE + safety code length), otherwise a
 * ::radef_kInvalidParameter fatal error is thrown.
 * @return ::radef_kNoError -> If all test passed and the message is valid.
 * @return ::radef_kInvalidMessageMd4 -> If the calculated MD4 doesn't match with the MD4 saved in the message.
 * @return ::radef_kInvalidMessageType -> If the message type is not known or invalid.
 * @return ::radef_kInvalidMessageSize -> If the message size is not correct.
 */
radef_RaStaReturnCode srmsg_CheckMessage(const srtyp_SrMessage *const sr_message);

/**
 * @brief Get the header of a SafRetL PDU message.
 *
 * This function extracts the header data (::srtyp_SrMessageHeader containing the message length, message type, receiver & sender ID, sequence number, confirmed
 * sequence number, time stamp and confirmed time stamp) from the passed SafRetL message.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-628} Get Message Header Function
 * @implementsReq{RASW-160} General PDU Message Structure
 * @implementsReq{RASW-161} Message Type
 * @implementsReq{RASW-162} Receiver Identification
 * @implementsReq{RASW-163} Sender Identification
 * @implementsReq{RASW-164} Sequence Number
 * @implementsReq{RASW-165} Confirmed Sequence Number
 * @implementsReq{RASW-166} Time Stamp
 * @implementsReq{RASW-167} Confirmed Time Stamp
 *
 * @param [in] sr_message Pointer to a memory block containing a message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] message_header Pointer to memory block for storing header data. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_GetMessageHeader(const srtyp_SrMessage *const sr_message, srtyp_SrMessageHeader *const message_header);

/**
 * @brief Get the message type of a SafRetL PDU message.
 *
 * This function extracts and returns the message type from the passed SafRetL message.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-824} Get Message Type Function
 *
 * @param sr_message Pointer to a memory block containing a message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @return srtyp_SrMessageType Message type from the passed message
 */
srtyp_SrMessageType srmsg_GetMessageType(const srtyp_SrMessage *const sr_message);

/**
 * @brief Get the sequence number of a SafRetL PDU message.
 *
 * This function extracts and returns the sequence number from the passed SafRetL message.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-825} Get Message Sequence Number Function
 *
 * @param sr_message Pointer to a memory block containing a message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @return uint32_t Sequence number from the passed message
 */
uint32_t srmsg_GetMessageSequenceNumber(const srtyp_SrMessage *const sr_message);

/**
 * @brief Get the payload of a SafRetL data or retransmitted data message.
 *
 * This function extracts the message payload size & data from a passed ::srtyp_kSrMessageData or ::srtyp_kSrMessageRetrData message. If the message contains
 * more data then allowed (::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE), a ::radef_kInvalidParameter fatal error is thrown.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-626} Get Data Message Payload Function
 * @implementsReq{RASW-193} Size of Message Data
 * @implementsReq{RASW-194} Data Message
 *
 * @param [in] sr_message Pointer to a memory block containing a message. If the pointer is NULL or the message is an other message type then
 * ::srtyp_kSrMessageData or ::srtyp_kSrMessageRetrData, a ::radef_kInvalidParameter fatal error is thrown. For the message payload size, the following range is
 * valid: ::srcty_kMinSrLayerPayloadDataSize <= value <= ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE.
 * @param [out] message_payload Pointer to memory block for storing the message payload. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is
 * thrown.
 */
void srmsg_GetDataMessagePayload(const srtyp_SrMessage *const sr_message, srtyp_SrMessagePayload *const message_payload);

/**
 * @brief Get the data of a SafRetL connection request or connection response message.
 *
 * This function extracts the connection message data (::srtyp_ProtocolVersion and receive buffer size from opposite side) from a passed
 * ::srtyp_kSrMessageConnReq or ::srtyp_kSrMessageConnResp message.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-625} Get Connection Message Data Function
 * @implementsReq{RASW-173} Protocol Version
 * @implementsReq{RASW-174} Nsendmax
 *
 * @param [in] sr_message Pointer to a memory block containing a message. If the pointer is NULL or the message is an other message type then
 * ::srtyp_kSrMessageConnReq or ::srtyp_kSrMessageConnResp, a ::radef_kInvalidParameter fatal error is thrown. Every digit of the protocol version should be in
 * the valid ASCII range of ::srcty_kProtocolVersionMinValue <= value <= ::srcty_kProtocolVersionMaxValue, otherwise a ::radef_kInvalidParameter fatal error is
 * thrown.
 * @param [out] protocol_version Pointer to memory block containing ::srtyp_ProtocolVersion structure. If the pointer is NULL, a ::radef_kInvalidParameter fatal
 * error is thrown.
 * @param [out] n_send_max Pointer to a memory block containing Nsendmax (receive buffer size). If the pointer is NULL, a ::radef_kInvalidParameter fatal error
 * is thrown.
 */
void srmsg_GetConnMessageData(const srtyp_SrMessage *const sr_message, srtyp_ProtocolVersion *const protocol_version, uint16_t *const n_send_max);

/**
 * @brief Get the data of a SafRetL disconnection request message.
 *
 * This function extracts the disconnection request message data (detailed reason and ::sraty_DiscReason) from a passed ::srtyp_kSrMessageDiscReq message. If
 * the message is an other message type, a ::radef_kInvalidParameter fatal error is thrown. When the extracted reason is not in the valid range of
 * ::sraty_kDiscReasonMin <= value < ::sraty_kDiscReasonMax, a ::radef_kInvalidParameter fatal error is thrown.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-627} Get Disconnection Message Data Function
 * @implementsReq{RASW-185} Detailed Information regarding the Reason for the Disconnection Request
 * @implementsReq{RASW-186} Reason for Disconnection Request
 *
 * @param [in] sr_message Pointer to a memory block containing a message. If the pointer is NULL or the message is an other message type then
 * ::srtyp_kSrMessageDiscReq, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] detailed_reason Pointer to a memory block for storing the detailed reason for disconnection from application. If the pointer is NULL, a
 * ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] reason Pointer to a memory block for storing the ::sraty_DiscReason. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmsg_GetDiscMessageData(const srtyp_SrMessage *const sr_message, uint16_t *const detailed_reason, sraty_DiscReason *const reason);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRMSG_SAFETY_RETRANSMISSION_MESSAGES_H_
