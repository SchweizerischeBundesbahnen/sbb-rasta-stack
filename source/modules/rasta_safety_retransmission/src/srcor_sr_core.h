/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcor_sr_core.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 420e705e9ad1b43bfa1c0be45f837e0fe2573bd4
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4933: Duplicated sent of diagnostic notification in sr_core when closing the connection,06.12.2022, N.
 * Andres}
 *
 * @addtogroup sr_core
 * @{
 *
 * @brief Interface of RaSTA SafRetL core module.
 *
 * This module provides many of the core logic functionality for SafRetL. This contains the following:
 * - send/receive SafRetL messages
 * - read messages when available
 * - handle send & receive buffer
 * - handle sequence number and confirmed sequence number
 * - handle protocol version
 * - handle message timings, heartbeat timings and timeouts
 * - handle retransmissions
 * .
 *
 * @implementsReq{RASW-565} Component sr_core Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRCOR_SAFETY_RETRANSMISSION_CORE_H_
#define SRCOR_SAFETY_RETRANSMISSION_CORE_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
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

/**
 * @brief Struct for the newly received message input buffer.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  bool message_in_buffer;                ///< Flag which indicates, that a new unprocessed message is in the message input buffer.
  srtyp_SrMessageHeader message_header;  ///< Message header extracted from message in buffer. Valid range as described in ::srtyp_SrMessageHeader.
  srtyp_SrMessage message_buffer;        ///< Input buffer for newly received message. Valid range as described in ::srtyp_SrMessage.
} srcor_InputBuffer;
//lint -restore

/**
 * @brief Struct for the message payload temporary buffer.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  bool message_in_buffer;                         ///< Flag which indicates, that a unprocessed message is in the temporary buffer.
  srtyp_SrMessagePayload message_payload_buffer;  ///< Buffer for message payload to send. Valid range as described in ::srtyp_SrMessagePayload.
} srcor_TemporaryBuffer;
//lint -restore

/**
 * @brief Struct for the process data of a RaSTA connection.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint32_t sequence_number_tx;  ///< SN_T: Sequence number of PDU message to be sent next. The full value range is valid and usable.
  uint32_t sequence_number_rx;  ///< SN_R: Expected sequence number of the next received PDU message. The full value range is valid and usable.

  uint32_t confirmed_sequence_number_tx;  ///< CS_T: Sequence number to be confirmed (which is transmitted at the next PDU message to be sent).  The full value
                                          ///< range is valid and usable.
  uint32_t last_send_confirmed_sequence_number_tx;  ///< Last effective confirmed send sequence number of CS_T. The full value range is valid and usable.
  uint32_t confirmed_sequence_number_rx;            ///< CS_R: Last received confirmed sequence number. The full value range is valid and usable.

  uint32_t time_stamp_rx;            ///< TS_R: Time stamp o f the time-out related PDU message received last [ms]. The full value range is valid and usable.
  uint32_t confirmed_time_stamp_rx;  ///< CTS_R: Confirmed time stamp of the time-out related PDU message received last [ms]. The full value range is valid
                                     ///< and usable.
  uint32_t time_stamp_tx;            ///< Last time stamp send (for calculation of T_h) [ms]. The full value range is valid and usable.

  uint16_t detailed_disconnect_reason;    ///< Detailed disconnect reason from the application. The full value range is valid and usable.
  uint16_t opposite_receive_buffer_size;  ///< Opposite receive buffer (NsendMax) size [messages]. The full value range is valid and usable.

  bool received_data_pending;  ///< True, if received data is pending on the redundancy channel

  srcor_InputBuffer input_buffer;               ///< Input buffer for newly received message. Valid range as described in ::srcor_InputBuffer.
  srcor_TemporaryBuffer temporary_send_buffer;  ///< Temporary send buffer for new message payload to send. Valid range as described in ::srcor_TemporaryBuffer.

  uint32_t timer_t_i;  ///< Timer T_i [ms]. Dynamicly calculated at receipt of time out related messages: T_i = T_max - T_rtd. Valid range: 0 <= value <= T_max.
  uint32_t t_rtd;      ///< Round trip delay of a message [ms]. Round trip time of a message. Only calculated for time-out relevant messages. Valid range: 0 <=
                       ///< value <= T_max. The full value range is valid and usable.
  uint32_t t_alive;  ///< Alive time of a message [ms]. Provides a statement to what extend the adaptive channel monitoring has exhausted T_max. Only calculated
                     ///< for time-out relevant messages. The full value range is valid and usable.

  srtyp_SrMessageHeaderCreate scratch_msg_header_create;  ///< Memory used for message creation. Valid range as described in ::srtyp_SrMessageHeaderCreate.
  srtyp_SrMessageHeaderUpdate scratch_msg_header_update;  ///< Memory used for message update. Valid range as described in ::srtyp_SrMessageHeader.
  srtyp_SrMessageHeader scratch_msg_header;               ///< Memory used for holding header. Valid range as described in ::srcor_InputBuffer.
  srtyp_SrMessage scratch_message;                        ///< Memory to transfer messages. Valid range as described in ::srtyp_SrMessage.
  srtyp_SrMessagePayload scratch_message_payload;         ///< Memory to transfer messages payload. Valid range as described in ::srtyp_SrMessagePayload.
} srcor_RaStaConnectionData;
//lint -restore

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Definition of RaSTA protocol version.
 */
extern const srtyp_ProtocolVersion srcor_kProtocolVersion;

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Checks if a SafRetL configuration is valid.
 *
 * This function is used to check if a SafRetL configuration is valid or not. Therefore the configuration is checked if all parameter are within the defined
 * ranges. All ranges for the SafRetL configuration are described in ::srcty_SafetyRetransmissionConfiguration. Additionally, a few checks on the configuration
 * are done:
 * - received flow control: MWA < NsendMax
 * - sender id != receiver id
 * .
 *
 * @implementsReq{RASW-573} Is Configuration Valid Function
 *
 * @param [in] sr_layer_configuration Pointer to SafRetL configuration. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @return true, if the configuration is valid.
 * @return false, if the configuration is invalid.
 */
bool srcor_IsConfigurationValid(const srcty_SafetyRetransmissionConfiguration *const sr_layer_configuration);

/**
 * @brief Initialize SafRetL core module.
 *
 * This function is used to initialize the core module. After checking the configuration for validity, the pointer to the configuration is saved internally. If
 * it is not valid, a ::radef_kInvalidConfiguration fatal error is thrown. The configuration is then used to initialize all modules used by the core module:
 * - messages
 * - diagnostics
 * - send buffer
 * - receive buffer
 * .
 * Finally, the internal structures are also properly initalized.
 *
 * @pre The core module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-571} Init sr_core Function
 *
 * @param [in] sr_layer_configuration Pointer to SafRetL configuration. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srcor_Init(const srcty_SafetyRetransmissionConfiguration *const sr_layer_configuration);

/**
 * @brief Get the connection identification associated with the specified sender and receiver identification.
 *
 * This function searches in the saved configuration for a matching connection id to the passed pair of sender and receiver id. If a matching entry is found,
 * the connection id is returned via the out parameter. Otherwise a ::radef_kInvalidParameter is returned.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-568} Get Connection ID Function
 *
 * @param [in] sender_id Sender identification. The full value range is valid and usable.
 * @param [in] receiver_id Receiver identification. The full value range is valid and usable.
 * @param [out] connection_id Pointer to RaSTA connection identification. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @return radef_kNoError if an matching connection ID is in the configuration
 * @return radef_kInvalidParameter if no matching connection ID is in the configuration
 */
radef_RaStaReturnCode srcor_GetConnectionId(const uint32_t sender_id, const uint32_t receiver_id, uint32_t *const connection_id);

/**
 * @brief Initialize the SafRetL core data of a dedicated RaSTA connection.
 *
 * This function is used to initialize the core data of a specific connection. This means it initializes SN_T with a random number. In case of client, CS_T is
 * set to 0 and CTS_R is set to the current time. Additionally, the corresponding diagnostic module, send and receive buffer are initalized for this connection
 * and the redundancy channel opened.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-572} Init RaSTA Connection Data Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_InitRaStaConnData(const uint32_t connection_id);

/**
 * @brief Close the redundancy channel of a rasta connection and send a diagnostic notification
 *
 * This function closes the matching redundancy channel to the passed connection (1:1 mapping between the connection id and redundancy channel id). Finally a
 * diagnostic notification is send to the application layer.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-826} Close Redundancy Channel Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_CloseRedundancyChannel(const uint32_t connection_id);

/**
 * @brief Read and analyze a received SafRetL message.
 *
 * This function receives and checks a message of a dedicated connection. First, a message is read from the SafRetL adapter interface (::sradin_ReadMessage).
 * If ::radef_kNoMessageReceived is returned, the received_data_pending flag is reset and the function finishes. Otherwise a set of checks is now started to
 * validate the received message:
 * - general checks are done (::GeneralMessageCheck), stop if check fails
 *   - MD4
 *   - message type
 *   - message size
 *   - authenticity of the message
 * - check sequence number range (::CheckSequenceNumberRange), stop if check fails
 * - check confirmed sequence number (::CheckConfirmedSequenceNumber), stop if check fails
 * - check if sequence number is in sequence (::CheckSequenceNumber) and save result for state machine (sequence number in sequence)
 * - check if confirmed time stamp is in sequence (::CheckConfirmedTimeStamp) and save result for state machine (confirmed time stamp in sequence)
 * - in case of error for safety code, msg type, address, sequence number or confirmed sequence number checks, the corresponding error counter in the
 * diagnostics module is incremented
 * - set the matching connection event according to message type (::SetConnectionEvent)
 * - if all necessary checks pass, set flag that message is in input buffer
 * .
 *
 * @implementsReq{RASW-580} Receive Message Function
 * @implementsReq{RASW-771} Receive Message Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [out] connection_event RaSTA connection event to be processed by the connection state machine (srtyp_kConnEventNone is returned, if there is nothing
 * to do for the state machine). If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] sequence_number_in_seq True, if the received sequence number is in sequence. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is
 * thrown.
 * @param [out] confirmed_time_stamp_in_seq True, if the confirmed timestamp is in sequence. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is
 * thrown.
 */
void srcor_ReceiveMessage(const uint32_t connection_id, srtyp_ConnectionEvents *const connection_event, bool *const sequence_number_in_seq,
                          bool *const confirmed_time_stamp_in_seq);

/**
 * @brief Process a successfully received message of a dedicated connection.
 *
 * This function processes a received message that was copied before in the input buffer of the core module. First it is checked if a message is in the input
 * buffer. If not, a ::radef_kInternalError is thrown. Then the following checks are performed:
 * - message header is extracted from the message in the input buffer
 * - current time is obtained with ::rasys_GetTimerValue
 * - for time-out relevant messages (::IsMessageTimeoutRelated), timeliness is checked and timer T_i updated with ::CalculateTimeliness
 * .
 * All following checks are only performed if the message timeliness is still guaranteed (timer T_i > 0).
 * - update local sequence numbers and timestamps
 *   - for all message types: SN_R = SN_PDU+1, CS_T = SN_PDU, CS_R = CS_PDU, TS_R = TS_PDU
 *   - for timeout relevant messages (::IsMessageTimeoutRelated): CTS_R = CTS_PDU
 *   - for ConnReq additionally set: CS_R = SN_T-1, CTS_R = current time
 * - remove confirmed messages from the send buffer (::srsend_RemoveFromBuffer), if the confirmed sequence number rx (CS_R) changed and the message is not a
 * ::srtyp_kSrMessageConnReq.
 * - for ConnReq/ConnResp, save the opposite receive buffer size (N_sendMax)
 * - add Data/RetrData message payload to received buffer (::srrece_AddToBuffer)
 * - apply received flow control for timeout related messages (::ReceivedFlowControlCheck)
 * - finally for time-out relevant messages (::IsMessageTimeoutRelated), update connection diagnostics (::srdia_UpdateConnectionDiagnostics)
 * .
 * In all cases, the message in input buffer flag is cleared.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-579} Process Received Messages Function
 * @implementsReq{RASW-775} Process Received Message Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if message timeliness is respected (Timer T_i > 0).
 * @return false, if message timeliness is no longer guaranteed (Timer T_i == 0). The caller must close the connection.
 */
bool srcor_ProcessReceivedMessage(const uint32_t connection_id);

/**
 * @brief Update confirmed sequence number to transmit from the message in the input buffer.
 *
 * When a message is in the internal input buffer, this function updated the confirmed sequence number to be confirmed with the sequence number of the last
 * received PDU message. (CS_T = SN_PDU, is part of action [1] in state event matrix).
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-592} Update Confirmed Tx Sequence Number Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_UpdateConfirmedTxSequenceNumber(const uint32_t connection_id);

/**
 * @brief Update confirmed sequence number receive from message in input buffer.
 *
 * When a message is in the internal input buffer, this function updates the last received confirmed sequence number with the confirmed sequence number received
 * in the last PDU message (CS_R = CS_PDU, is part of action [4] in state event matrix).
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-591} Update Confirmed Rx Sequence Number Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_UpdateConfirmedRxSequenceNumber(const uint32_t connection_id);

/**
 * @brief Checks if the requested protocol version is accepted.
 *
 * Checks if the protocol version from a received ConnReq or ConnResp message in the input buffer is on the same or higher version then itself (see RASW-109 for
 * defined version). If the version is smaller, the connection is rejected.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-577} Is Protocol Version Accepted Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if the protocol version is accepted.
 * @return false, if the protocol version is not accepted.
 */
bool srcor_IsProtocolVersionAccepted(const uint32_t connection_id);

/**
 * @brief Set the message pending flag of a dedicated RaSTA connection.
 *
 * This function sets the received message pending flag of a dedicated connection. This indicates, that a new message from the RedL is pending to be read.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-590} Set Received Message Pending Flag Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_SetReceivedMessagePendingFlag(const uint32_t connection_id);

/**
 * @brief Get the received message pending flag for a dedicated RaSTA connection.
 *
 * This function returns the received message pending flag of a dedicated connection.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-569} Get Received Message Pending Flag Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if a received message is pending.
 * @return false, if no received message is pending.
 */
bool srcor_GetReceivedMessagePendingFlag(const uint32_t connection_id);

/**
 * @brief Write message payload to temporary buffer for messages to send.
 *
 * This functions copies the passed message payload structure to the internal temporary buffer and sets the message_in_buffer flag to true. If this
 * function is called and the message_in_buffer flag is still set, a ::radef_kInternalError error is thrown.
 *
 * @implementsReq{RASW-593} Write Message Payload to Temporary Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] message_payload_size Size of the message payload [bytes]. Valid range: ::srcty_kMinSrLayerPayloadDataSize <= value <=
 * ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE.
 * @param [in] message_payload Pointer to message payload data. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srcor_WriteMessagePayloadToTemporaryBuffer(const uint32_t connection_id, const uint16_t message_payload_size, const uint8_t *const message_payload);

/**
 * @brief Clear input buffer messages pending flag.
 *
 * This function is used to clear the message pending flag in the input buffer.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-566} Clear Input Buffer Message Pending Flag Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_ClearInputBufferMessagePendingFlag(const uint32_t connection_id);

/**
 * @brief Create and send a SafRetL data message from the temporary buffer.
 *
 * This function prepares a new message header that is used to create a new data message with the message saved in the temporary buffer. This message is then
 * added to the send buffer and send with ::srcor_SendPendingMessages. Finally, the flag for a message in the temporary buffer is reset.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 * @pre A message must be in the temporary send buffer, otherwise a ::radef_kNoMessageToSend fatal error is thrown.
 *
 * @implementsReq{RASW-584} Send Data Message Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_SendDataMessage(const uint32_t connection_id);

/**
 * @brief Create and send a SafRetL connection request message.
 *
 * This function prepares a new message header (TS_R must be set to 0) that is used to create a new connection request message. This message is then added to
 * the send buffer and send with ::srcor_SendPendingMessages.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-582} Send ConnReq Message Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_SendConnReqMessage(const uint32_t connection_id);

/**
 * @brief Create and send a SafRetL connection response message.
 *
 * This function prepares a new message header that is used to create a new connection response message. This message is then added to the send buffer and send
 * with ::srcor_SendPendingMessages.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-583} Send ConnResp Message Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_SendConnRespMessage(const uint32_t connection_id);

/**
 * @brief Create and send a SafRetL disconnection request message.
 *
 * This function prepares a new message header that is used to create a disconnection request message. The message is updated with the current timestamp and
 * confirmed sequence number, so it can be send directly (::sradin_SendMessage) without passing through the send buffer. The send buffer of this connection is
 * then initialized (::srsend_InitBuffer) to properly clean all not send messages, since the connection was closed. Finally, the redundancy channel of this
 * connection is closed and a diagnostic notification is send to the application layer (by calling ::srcor_CloseRedundancyChannel).
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-585} Send DiscReq Message Function
 * @implementsReq{RASW-767} Send Disconnection Request Message Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] disconnect_reason Reason for disconnection.  Valid range: ::sraty_kDiscReasonMin <= value < ::sraty_kDiscReasonMax.
 */
void srcor_SendDiscReqMessage(const uint32_t connection_id, const sraty_DiscReason disconnect_reason);

/**
 * @brief Set detailed reason for disconnection request message.
 *
 * When a disconnection was requested from the application layer, the passed detailed reason can be stored in the core layer. This is needed to send the
 * the disconnection request to the opposite side.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-589} Set Disconnection Detailed Reason Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] detailed_disconnect_reason Detailed disconnect reason from the application. The full value range is valid and usable.
 */
void srcor_SetDiscDetailedReason(const uint32_t connection_id, const uint16_t detailed_disconnect_reason);

/**
 * @brief Create and send a SafRetL heartbeat message.
 *
 * This function creates a new HB message, adds it to the send buffer and send the pending messages with ::srcor_SendPendingMessages.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-586} Send Heartbeat Message Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_SendHbMessage(const uint32_t connection_id);

/**
 * @brief Create and send a SafRetL retransmission request message.
 *
 * This function creates a new RetrReq message, adds it to the send buffer and send the pending messages with ::srcor_SendPendingMessages.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-588} Send RetrReq Message Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_SendRetrReqMessage(const uint32_t connection_id);

/**
 * @brief Handle a retransmission request and send a SafRetL retransmission response, heartbeat or data message.
 *
 * This function get the current sequence number tx (SN_T) and prepares a message header for new messages. This is then passed to the
 * ::srsend_PrepareBufferForRetr function to prepare the send buffer for a retransmission. If the preparation of the send buffer succeeded, the final sequence
 * number is saved back in the local variable. Otherwise a ::radef_kInvalidSequenceNumber fatal error is thrown.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-570} Handle Retransmission Request Function
 * @implementsReq{RASW-777} Handle Retransmission Request Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_HandleRetrReq(const uint32_t connection_id);

/**
 * @brief Checks if a requested retransmission sequence number is available in the send buffer.
 *
 * This function checks if the following sequence number of the last received confirmed sequence number (taken from the saved RetrReq message in the input
 * buffer) is available for retransmission in the send buffer.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-578} Is Retransmission Request Sequence Number Available Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if the requested sequence number is available.
 * @return false, if the requested sequence number is not available or no message in input buffer.
 */
bool srcor_IsRetrReqSequenceNumberAvailable(const uint32_t connection_id);

/**
 * @brief Returns true, if the own connection role is server.
 *
 * This function checks if the own connection role is server or client. The communication partner with the higher value is server, the one with the lower value
 * client.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-574} Is Connection Role Server Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if the connection role is server.
 * @return false, if the connection role is client.
 */
bool srcor_IsConnRoleServer(const uint32_t connection_id);

/**
 * @brief Checks if a message timeout occurred.
 *
 * This function checks if a message timeout for a dedicated RaSTA connection occurred. This is the case, if the current time - last received time-out related
 * timestamp is bigger than maximum accepted age of a message. Formula: current time - CTS_R > T_i (T_i is dynamicly calculated when receiving a timeout related
 * message T_i = t_max - t_rtd)
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-576} Is Message Timeout Function
 * @implementsReq{RASW-808} Timer Ti
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if a message timeout occurred.
 * @return false, if no message timeout occurred.
 */
bool srcor_IsMessageTimeout(const uint32_t connection_id);

/**
 * @brief Checks if the heartbeat interval is elapsed.
 *
 * This function checks if a heartbeat timeout for a dedicated RaSTA connection occurred. This is the case, if the current time - last send timestamp is bigger
 * than the heartbeat period. Formula: current time - last send timestamp > T_h.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-575} Is Heartbeat Interval Function
 * @implementsReq{RASW-807} Timer Th
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if the heartbeat interval is elapsed.
 * @return false, if the heartbeat interval is not elapsed.
 */
bool srcor_IsHeartbeatInterval(const uint32_t connection_id);

/**
 * @brief Check if received messages are pending and send & received buffer are not full.
 *
 * This function checks if:
 * - received messages pending
 * - at least three free entires in send buffer (in worst case of a retransmission of a retransmission, this adds a RetrResp, HB & RetrReq to the send
 * buffer to the already existing data)
 * - at least one free entry in received buffer
 * .
 * Returns true if all 3 conditions are true.
 *
 * @implementsReq{RASW-830} Is Received Message Pending And Buffers Not Full Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true -> message pending and free space in buffers
 * @return false -> one or more of the 3 conditions are not given anymore
 */
bool srcor_IsReceivedMsgPendingAndBuffersNotFull(const uint32_t connection_id);

/**
 * @brief Send pending messages from the send buffer, if the flow control allows to send.
 *
 * This function sends available messages from the send buffer, while there are messages in the send buffer and if the flow control allows to send messages
 * (::SendPendingMessagesWithFlowControlAllowed). While this is true, following steps are performed:
 * - a message is read from the send buffer
 * - last send timestamp (time_stamp_tx) is updated with the current time
 * - a ::srtyp_SrMessageHeaderUpdate structure is prepared
 *   - set srcor_RaStaConnectionData::time_stamp_tx as timestamp (TS_PDU)
 *   - in case of a ConnReq message, srcor_RaStaConnectionData::confirmed_sequence_number_tx (CS_T) must be set to 0
 *   - set srcor_RaStaConnectionData::confirmed_sequence_number_tx (CS_T) as confirmed sequence number (CS_PDU)
 *   - srcor_RaStaConnectionData::last_send_confirmed_sequence_number_tx is updated with confirmed sequence number (CS_PDU)
 * - The message is then updated with this ::srtyp_SrMessageHeaderUpdate structure and finally send with ::sradin_SendMessage.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-587} Send Pending Messages Function
 * @implementsReq{RASW-765} Send Pending Messages Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srcor_SendPendingMessages(const uint32_t connection_id);

/**
 * @brief Send a connection state changed notification to the application layer.
 *
 * This function collects all necessary data for a connection notification and send this to the application layer. This data includes the utilisation of send
 * and receive buffer, opposite buffer size, connection state and detailed disconnection reason. Detailed disconnection reason is only valid if the connection
 * state is ::sraty_kConnectionClosed.
 *
 * @implementsReq{RASW-581} Send Connection State Notification Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] connection_state Connection state. Valid range: ::sraty_kConnectionMin <= value < ::sraty_kConnectionMax.
 * @param [in] disconnect_reason Disconnect reason (only valid in disconnected state). Valid range: ::sraty_kDiscReasonMin <= value < ::sraty_kDiscReasonMax.
 */
void srcor_SendConnectionStateNotification(const uint32_t connection_id, const sraty_ConnectionStates connection_state,
                                           const sraty_DiscReason disconnect_reason);

/**
 * @brief Get the opposite receive buffer size and the own buffer utilisation for the connection state request.
 *
 * This function gets the utilisation information of the send and receive buffer as well as the size of the opposite input buffer (If opposite buffer is 0,
 * connection establishment was not yet executed during which the input buffer size is exchanged).
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-567} Get Buffer Size and Utilisation Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 *
 * @param [out] buffer_utilisation Pointer to buffer utilization structure. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] opposite_buffer_size Pointer to pass size of the receive buffer of the connection party. If the pointer is NULL, a ::radef_kInvalidParameter
 * fatal error is thrown.
 */
void srcor_GetBufferSizeAndUtilisation(const uint32_t connection_id, sraty_BufferUtilisation *const buffer_utilisation, uint16_t *const opposite_buffer_size);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRCOR_SAFETY_RETRANSMISSION_CORE_H_
