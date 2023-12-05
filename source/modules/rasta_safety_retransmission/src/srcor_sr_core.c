/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcor_sr_core.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 420e705e9ad1b43bfa1c0be45f837e0fe2573bd4
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4933: Duplicated sent of diagnostic notification in sr_core when closing the connection,06.12.2022, N.
 * Andres}
 *
 * @brief Implementation of RaSTA SafRetL core module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srcor_sr_core.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/rahlp_rasta_helper.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/sradin_sr_adapter_interface.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "rasta_safety_retransmission/srnot_sr_notifications.h"
#include "srdia_sr_diagnostics.h"       // NOLINT(build/include_subdir)
#include "srmsg_sr_messages.h"          // NOLINT(build/include_subdir)
#include "srrece_sr_received_buffer.h"  // NOLINT(build/include_subdir)
#include "srsend_sr_send_buffer.h"      // NOLINT(build/include_subdir)
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
PRIVATE bool srcor_initialized = false;

/**
 * @brief Pointer to SafRetL configuration.
 */
PRIVATE const srcty_SafetyRetransmissionConfiguration *srcor_sr_configuration = NULL;

/**
 * @brief Process data of the rasta connections.
 */
PRIVATE srcor_RaStaConnectionData srcor_rasta_connections[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

/**
 * @brief Granularity of the system timer [ms].
 */
PRIVATE uint32_t srcor_timer_granularity = 0U;

/**
 * @brief ID of the sr_core debug logger.
 */
PRIVATE uint16_t srcor_logger_id;  //lint !e551 (static variable is used only in the DEBUG build for the debug logger)

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup sr_core
 * @{
 */

/**
 * @brief Check if the connection configurations are valid
 *
 * This internal function checks if the connection configurations of a SafRetL configuration are valid. All ranges for the SafRetL connection configurations are
 * described in srcty_SafetyRetransmissionConfiguration::connection_configurations. Additionally, it is checked that the sender id is not identical to the
 * receiver id and the connection identification must match the connection index in the array.
 *
 * @implementsReq{RASW-573} Is Configuration Valid Function
 *
 * @param [in] number_of_connections Number of configured connections. Valid range of ::srcty_kMinNumberOfRaStaConnections <= value <=
 * ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
 * @param [in] connection_configurations Configurations of connections. All ranges for the individual structs are described in ::srcty_ConnectionConfiguration.
 * Their combination of sender, receiver and connection will be checked by this function for their validity.
 * @return true, if the connection configuration is valid.
 * @return false, if the connection configuration is not valid.
 */
static bool CheckConnectionConfigurations(const uint32_t number_of_connections, const srcty_ConnectionConfiguration *const connection_configurations);

/**
 * @brief Checks if a message is timeout related or not
 *
 * This internal functions checks if a message is timeout related or not. This is checked with its message type. Timeout related message types are:
 * - heartbeat
 * - data
 * - retransmitted data
 * .
 *
 * @implementsReq{RASW-579} Process Received Messages Function
 *
 * @param [in] message_type Message type to check. Valid range: ::srtyp_kSrMessageMin <= value <= ::srtyp_kSrMessageMax - 1.
 * @return true, if message is timeout related.
 * @return false, if message is not timeout related.
 */
static bool IsMessageTimeoutRelated(const srtyp_SrMessageType message_type);

/**
 * @brief Get the Current Sequence Number SN_T and Increment Number
 *
 * This internal function returns the current sequence number tx (SN_T) of a specific RaSTA connection and increments the sequence number counter afterwards.
 *
 * @implementsReq{RASW-584} Send Data Message Function
 * @implementsReq{RASW-582} Send ConnReq Message Function
 * @implementsReq{RASW-583} Send ConnResp Message Function
 * @implementsReq{RASW-585} Send DiscReq Message Function
 * @implementsReq{RASW-586} Send Heartbeat Message Function
 * @implementsReq{RASW-588} Send RetrReq Message Function
 * @implementsReq{RASW-570} Handle Retransmission Request Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return uint32_t Current sequence number of the passed RaSTA Connection
 */
static uint32_t GetCurrentSequenceNumberAndIncrementNumber(const uint32_t connection_id);

/**
 * @brief Checks if the flow control allows to send message to the opposite side
 *
 * This internal function checks if there are messages in the send buffer that wait to be transmitted and if the flow control allows new messages to be send.
 * For the flow control to allow sending, the number of unconfirmed send messages in the send buffer should be smaller then the opposite send buffer
 * (N_sendMax). Formula: not confirmed messages in send buffer < opposite N_sendMax. This parameter is exchanged during connection establishment and saved in
 * the internal structure.
 *
 * @implementsReq{RASW-587} Send Pending Messages Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if the flow control allow sending messages.
 * @return false, if the flow control prevents sending messages.
 */
static bool SendPendingMessagesWithFlowControlAllowed(const uint32_t connection_id);

/**
 * @brief Perform the received flow control check
 *
 * This internal function checks and performs the received flow control. The check is only performed for HB, RetrResp, Data & RetrData messages, as for this
 * message types no answer from the opposite side is assumed. For other message types nothing is done. If the not confirmed sequence numbers are bigger then the
 * MWA from the configuration, a message must be sent to the opposite side to confirm the sequence number. If messages are available in the send buffer, they
 * are send using the ::srcor_SendPendingMessages function, otherwise a HB message is send with ::srcor_SendHbMessage. The not confirmed sequence number is the
 * difference between the last received sequence number to be confirmed (CS_T) and the last effective send confirmed sequence number
 * (srcor_RaStaConnectionData::confirmed_sequence_number_tx - srcor_RaStaConnectionData::last_send_confirmed_sequence_number_tx).
 *
 * @implementsReq{RASW-579} Process Received Messages Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] message_type Message type of the processed message received before the check. Valid range: ::srtyp_kSrMessageMin <= value <=
 * ::srtyp_kSrMessageMax -1.
 */
static void ReceivedFlowControlCheck(const uint32_t connection_id, const srtyp_SrMessageType message_type);

/**
 * @brief Check the general part of a message
 *
 * This internal functions checks the general part of a message. This contains the MD4, message length, message type and is checked with ::srmsg_CheckMessage.
 * If all checks pass, the message header is extracted using ::srmsg_GetMessageHeader and returned via the output parameter msg_hdr. Additionally the
 * authenticity of the message is then checked. If a check fails, the corresponding error counter is incremented (::srdia_IncSafetyCodeErrorCounter,
 * ::srdia_IncTypeErrorCounter & ::srdia_IncAddressErrorCounter) and false is returned to indicate that a test failed.
 *
 * @implementsReq{RASW-580} Receive Message Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] msg Pointer to a memory block containing a message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [out] msg_hdr Pointer to a memory block for the message header. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return true, if all checks passed.
 * @return false, if one of the checks fails.
 */
static bool GeneralMessageCheck(const uint32_t connection_id, const srtyp_SrMessage *const msg, srtyp_SrMessageHeader *const msg_hdr);

/**
 * @brief Check the sequence number range of a new received message
 *
 * This internal function checks the sequence number range of a new received message. ConnReq, ConnResp and RetrResp are always accepted. For all other
 * message type, the following check is done: 0 <= SN_PDU - SN_R <= 10* N_sendMax. If a message doesn't pass this test, it must be discarded.
 *
 * @implementsReq{RASW-580} Receive Message Function
 * @implementsReq{RASW-803} Sequence Number Range Check
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] msg_hdr Pointer to message header from new received message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return true, if the sequence number range check passed.
 * @return false, if the sequence number range check failed.
 */
static bool CheckSequenceNumberRange(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr);

/**
 * @brief Check the sequence number of a new received message
 *
 * This internal function checks if the sequence number of a new received message is in sequence. ConnReq, ConnResp, RetrResp and DiscReq are always accepted.
 * For all other message type, the following check is done: SN_R == SN_PDU. The result of this check is returned as it is required for processing in the state
 * machine (sequence number in sequence or not).
 *
 * @implementsReq{RASW-580} Receive Message Function
 * @implementsReq{RASW-805} Sequence Number Check
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] msg_hdr Pointer to message header from new received message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return true, if sequence number in sequence.
 * @return false, if sequence number not in sequence.
 */
static bool CheckSequenceNumber(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr);

/**
 * @brief Check the confirmed sequence number of a new received message
 *
 * This internal function checks if the confirmed sequence number of a new received message is in sequence. The following checks are done:
 * - ConnReq: CS_PDU == 0
 * - ConnResp: CS_PDU == SN_T -1
 * - All other message types: 0 <= CS_PDU - CS_R < SN_T - CS_R
 * .
 * If a message doesn't pass this test, it must be discarded.
 *
 * @implementsReq{RASW-580} Receive Message Function
 * @implementsReq{RASW-804} Sequence Integrity of the Confirmed Sequence Number
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] msg_hdr Pointer to message header from new received message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return true, if confirmed sequence number in sequence.
 * @return false, if confirmed sequence number not in sequence.
 */
static bool CheckConfirmedSequenceNumber(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr);

/**
 * @brief Checks the time stamp of a new received message
 *
 * This internal function checks the time stamp of a new received message. ConnReq, ConnResp, RetrReq, RetrResp and DiscReq are always accepted. For all other
 * message types, the time stamp of a subsequent message (TS_PDU) cannot be before the time stamp of the previously received message (TS_R). The following
 * check is done: 0 <= TS_PDU - TS_R < T_max. If a message doesn't pass this test, it must be discarded.
 *
 * @implementsReq{RASW-580} Receive Message Function
 * @implementsReq{RASW-806} Timeliness Check
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] msg_hdr Pointer to message header from new received message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return true, if time stamp in sequence.
 * @return false, if time stamp not in sequence.
 */
static bool CheckTimeStamp(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr);

/**
 * @brief Checks the confirmed time stamp of a new received message
 *
 * This internal function checks the confirmed time stamp of a new received message. ConnReq, ConnResp, RetrReq, RetrResp and DiscReq are always accepted.
 * For all other message type, the following check is done: 0 <= CTS_PDU - CTS_R < T_max. The result of this check is returned as is required for
 * processing in the state machine (confirmed time stamp in sequence or not).
 *
 * @implementsReq{RASW-580} Receive Message Function
 * @implementsReq{RASW-822} Sequence Integrity of the Confirmed Time Stamp
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] msg_hdr Pointer to message header from new received message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @return true, if confirmed time stamp in sequence.
 * @return false, if confirmed time stamp not in sequence.
 */
static bool CheckConfirmedTimeStamp(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr);

/**
 * @brief Calculate the timeliness of a new received message
 *
 * This internal function calculated the new values for the round trip time (T_rtd) and the alive time (T_alive). With this updates values, the timer T_i is
 * then set. Following calculation are used:
 * - T_rtd = T_local + T_granularity - CTS_PDU
 * - T_alive = T_local - CTS_R
 * - T_i = T_max - T_rtd
 * .
 * If T_rtd is greater then T_max, the timeliness of new messages can no longer be guaranteed. Timer T_i is set to 0.
 * This function shall only be called for time-out related messages.
 *
 * @implementsReq{RASW-580} Receive Message Function
 * @implementsReq{RASW-806} Timeliness Check
 * @implementsReq{RASW-808} Timer Ti
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] msg_hdr Pointer to message header from new received message. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 * @param [in] current_time Current time used for calculation of T_rtd & T_alive. The full value range is valid and usable.
 * @return true -> timeliness of new messages guaranteed
 * @return false -> timeliness of new messages can no longer be guaranteed
 */
static bool CalculateTimeliness(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr, const uint32_t current_time);

/**
 * @brief Set the connection event according to the message type
 *
 * This internal functions set the ::srtyp_ConnectionEvents for a new received message according to the passed ::srtyp_SrMessageType. Always the matching
 * connection event is set for every message type. This connection event is used to pass the state machine the matching event for a received message.
 *
 * @implementsReq{RASW-580} Receive Message Function
 *
 * @param [in] message_type Message type. Valid range: ::srtyp_kSrMessageMin <= value < ::srtyp_kSrMessageMax. Only defined values of the message type are
 * accepted.
 * @param [out] connection_event Pointer to the connection event. If the pointer is NULL, a ::radef_kInternalError fatal error is thrown.
 */
static void SetConnectionEvent(const srtyp_SrMessageType message_type, srtyp_ConnectionEvents *const connection_event);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Definition of RaSTA protocol version 03.03 (all 4 bytes are decimal digits in ASCII)
 */
const srtyp_ProtocolVersion srcor_kProtocolVersion = {
    .version = {(uint8_t)'0', (uint8_t)'3', (uint8_t)'0', (uint8_t)'3'},
};

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

bool srcor_IsConfigurationValid(const srcty_SafetyRetransmissionConfiguration *const sr_layer_configuration) {
  // Input parameter check
  raas_AssertNotNull(sr_layer_configuration, radef_kInvalidParameter);

  bool config_valid = true;

  // rasta_network_id and md4_initial_value are not checked, since the full range of all their parameters is valid
  if (!rahlp_IsU32InRange(sr_layer_configuration->t_max, srcty_kMinTMax, srcty_kMaxTMax)) {
    config_valid = false;
  } else if (!rahlp_IsU32InRange(sr_layer_configuration->t_h, srcty_kMinTHeartbeat, srcty_kMaxTHeartbeat)) {
    config_valid = false;
  } else if (!rahlp_IsU16InRange((uint16_t)sr_layer_configuration->safety_code_type, (uint16_t)srcty_kSafetyCodeTypeMin,
                                 (uint16_t)(srcty_kSafetyCodeTypeMax)-1U)) {
    config_valid = false;
  } else if (!rahlp_IsU16InRange(sr_layer_configuration->m_w_a, srcty_kMinMWA, srcty_kMaxMWA)) {
    config_valid = false;
  } else if (!rahlp_IsU16InRange(sr_layer_configuration->n_send_max, srcty_kMinNSendMax, RADEF_MAX_N_SEND_MAX)) {
    config_valid = false;
  } else if (sr_layer_configuration->m_w_a >= sr_layer_configuration->n_send_max) {
    config_valid = false;
  } else if (sr_layer_configuration->n_max_packet != srcty_kNMaxPacket) {
    config_valid = false;
  } else if (!rahlp_IsU32InRange(sr_layer_configuration->n_diag_window, srcty_kMinNDiagWindow, srcty_kMaxNDiagWindow)) {
    config_valid = false;
  } else if (!rahlp_IsU32InRange(sr_layer_configuration->number_of_connections, srcty_kMinNumberOfRaStaConnections, RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS)) {
    config_valid = false;
  } else if (!srdia_AreDiagnosticTimingIntervalsValid(sr_layer_configuration->t_max, sr_layer_configuration->diag_timing_distr_intervals)) {
    config_valid = false;
  } else if (!CheckConnectionConfigurations(sr_layer_configuration->number_of_connections, sr_layer_configuration->connection_configurations)) {
    config_valid = false;
  } else {
    // Nothing to do here.
  }

  return config_valid;
}

void srcor_Init(const srcty_SafetyRetransmissionConfiguration *const sr_layer_configuration) {
  // Input parameter check
  raas_AssertTrue(!srcor_initialized, radef_kAlreadyInitialized);
  raas_AssertNotNull(sr_layer_configuration, radef_kInvalidParameter);
  raas_AssertTrue(srcor_IsConfigurationValid(sr_layer_configuration), radef_kInvalidConfiguration);

  srcor_logger_id = ralog_INIT_LOGGER(ralog_kLogLevelNone);
  ralog_LOG_DEBUG(srcor_logger_id, "SafRetL core init ...\n");

  srcor_sr_configuration = sr_layer_configuration;

  srcor_timer_granularity = rasys_GetTimerGranularity();

  srmsg_Init(srcor_sr_configuration->safety_code_type, srcor_sr_configuration->md4_initial_value);
  srdia_Init(srcor_sr_configuration->number_of_connections, srcor_sr_configuration->t_max, srcor_sr_configuration->n_diag_window,
             srcor_sr_configuration->diag_timing_distr_intervals);
  srsend_Init(srcor_sr_configuration->number_of_connections);
  srrece_Init(srcor_sr_configuration->number_of_connections, srcor_sr_configuration->n_send_max);

  // Initialize internal data
  for (uint16_t index = 0; index < srcor_sr_configuration->number_of_connections; ++index) {
    srcor_rasta_connections[index].sequence_number_tx = 0U;
    srcor_rasta_connections[index].sequence_number_rx = 0U;

    srcor_rasta_connections[index].confirmed_sequence_number_tx = 0U;
    srcor_rasta_connections[index].last_send_confirmed_sequence_number_tx = 0U;
    srcor_rasta_connections[index].confirmed_sequence_number_rx = 0U;

    srcor_rasta_connections[index].time_stamp_rx = 0U;
    srcor_rasta_connections[index].confirmed_time_stamp_rx = 0U;
    srcor_rasta_connections[index].time_stamp_tx = 0U;

    srcor_rasta_connections[index].detailed_disconnect_reason = 0U;
    srcor_rasta_connections[index].opposite_receive_buffer_size =
        srcor_sr_configuration->n_send_max;  // Initialized with own buffer size, it will be updated with opposite buffer size at connection establishment

    srcor_rasta_connections[index].received_data_pending = false;

    srcor_rasta_connections[index].input_buffer.message_in_buffer = false;
    srcor_rasta_connections[index].temporary_send_buffer.message_in_buffer = false;

    srcor_rasta_connections[index].timer_t_i = 0U;
    srcor_rasta_connections[index].t_rtd = 0U;
    srcor_rasta_connections[index].t_alive = 0U;
  }

  srcor_initialized = true;
}

radef_RaStaReturnCode srcor_GetConnectionId(const uint32_t sender_id, const uint32_t receiver_id, uint32_t *const connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertNotNull(connection_id, radef_kInvalidParameter);

  radef_RaStaReturnCode ret = radef_kInvalidParameter;

  for (uint32_t index = 0; index < srcor_sr_configuration->number_of_connections; ++index) {
    if ((receiver_id == srcor_sr_configuration->connection_configurations[index].receiver_id) &&
        (sender_id == srcor_sr_configuration->connection_configurations[index].sender_id)) {
      *connection_id = srcor_sr_configuration->connection_configurations[index].connection_id;
      ret = radef_kNoError;
      break;
    }
  }

  return ret;
}

void srcor_InitRaStaConnData(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Data init. Connection id: %u\n", connection_id);

  // Initialize internal data
  srcor_rasta_connections[connection_id].sequence_number_rx = 0U;

  srcor_rasta_connections[connection_id].last_send_confirmed_sequence_number_tx = 0U;
  srcor_rasta_connections[connection_id].confirmed_sequence_number_rx = 0U;

  srcor_rasta_connections[connection_id].time_stamp_rx = 0U;
  srcor_rasta_connections[connection_id].time_stamp_tx = 0U;

  srcor_rasta_connections[connection_id].detailed_disconnect_reason = 0U;
  srcor_rasta_connections[connection_id].opposite_receive_buffer_size =
      srcor_sr_configuration->n_send_max;  // Initialized with own buffer size, it will be updated with opposite buffer size at connection establishment

  srcor_rasta_connections[connection_id].received_data_pending = false;

  srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = false;
  srcor_rasta_connections[connection_id].temporary_send_buffer.message_in_buffer = false;

  srcor_rasta_connections[connection_id].t_rtd = 0U;
  srcor_rasta_connections[connection_id].t_alive = 0U;

  // For client & server: SN_T = random, t_i = t_max
  srcor_rasta_connections[connection_id].sequence_number_tx = rasys_GetRandomNumber();
  srcor_rasta_connections[connection_id].timer_t_i = srcor_sr_configuration->t_max;

  // In case of client: CS_T = 0 & CTS_R = Tlocal
  srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = 0;
  if (!srcor_IsConnRoleServer(connection_id)) {
    srcor_rasta_connections[connection_id].confirmed_time_stamp_rx = rasys_GetTimerValue();
  } else {
    srcor_rasta_connections[connection_id].confirmed_time_stamp_rx = 0U;
  }

  // Init related module for this connection
  srdia_InitConnectionDiagnostics(connection_id);
  srsend_InitBuffer(connection_id);
  srrece_InitBuffer(connection_id);

  const uint32_t kRedChannelId = connection_id;  // 1:1 mapping between the connection id and redundancy channel id
  sradin_OpenRedundancyChannel(kRedChannelId);
}

void srcor_CloseRedundancyChannel(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  const uint32_t kRedChannelId = connection_id;  // 1:1 mapping between the connection id and redundancy channel id
  sradin_CloseRedundancyChannel(kRedChannelId);
  srdia_SendDiagnosticNotification(connection_id);
}

void srcor_ReceiveMessage(const uint32_t connection_id, srtyp_ConnectionEvents *const connection_event, bool *const sequence_number_in_seq,
                          bool *const confirmed_time_stamp_in_seq) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);
  raas_AssertNotNull(connection_event, radef_kInvalidParameter);
  raas_AssertNotNull(sequence_number_in_seq, radef_kInvalidParameter);
  raas_AssertNotNull(confirmed_time_stamp_in_seq, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  bool ok = true;

  // Init passed variables
  *connection_event = srtyp_kConnEventNone;
  *sequence_number_in_seq = false;
  *confirmed_time_stamp_in_seq = false;

  const uint32_t kRedChannelId = connection_id;  // 1:1 mapping between the connection id and redundancy channel id

  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].input_buffer.message_buffer;
  srtyp_SrMessageHeader *const msg_hdr = &srcor_rasta_connections[connection_id].input_buffer.message_header;

  radef_RaStaReturnCode ret = sradin_ReadMessage(kRedChannelId, RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, &msg->message_size, msg->message);
  raas_AssertTrue((ret == radef_kNoError) || (ret == radef_kNoMessageReceived),
                  radef_kInternalError);  // Other return values are not expected for sradin_ReadMessage()
  if (ret == radef_kNoMessageReceived) {
    srcor_rasta_connections[connection_id].received_data_pending = false;
    ok = false;
  }

  if (ok) {
    // 1), 2), 3) General message check (MD4, message type, message size, authenticity of message)
    ok = GeneralMessageCheck(connection_id, msg, msg_hdr);
  }

  if (ok) {
    // 4) Check sequence number range
    ok = CheckSequenceNumberRange(connection_id, msg_hdr);
  }

  if (ok) {
    // 4) Check confirmed sequence number
    ok = CheckConfirmedSequenceNumber(connection_id, msg_hdr);
  }

  if (ok) {
    // Check correct timestamp order
    ok = CheckTimeStamp(connection_id, msg_hdr);
  }

  if (ok) {
    // 5) Check sequence number
    *sequence_number_in_seq = CheckSequenceNumber(connection_id, msg_hdr);

    // 6) Check confirmed time stamp
    *confirmed_time_stamp_in_seq = CheckConfirmedTimeStamp(connection_id, msg_hdr);

    // Set ouput parameter according to received message
    SetConnectionEvent(msg_hdr->message_type, connection_event);

    // Indicate that the input_buffer contains a newly received message
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = true;

    ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u, SN in seq: %B, CSN in seq: %B, connection event: %u\n", connection_id, &sequence_number_in_seq,
                    &confirmed_time_stamp_in_seq, &connection_event);
  }
}

bool srcor_ProcessReceivedMessage(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  bool timeliness_respected = true;

  // Check if message in input buffer
  raas_AssertTrue(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer, radef_kInternalError);

  const srtyp_SrMessageHeader *const msg_header = &srcor_rasta_connections[connection_id].input_buffer.message_header;

  // Get current time
  const uint32_t kCurrentTime = rasys_GetTimerValue();

  // 10) Setup timers for adaptive channel monitoring
  if (IsMessageTimeoutRelated(msg_header->message_type)) {
    timeliness_respected = CalculateTimeliness(connection_id, msg_header, kCurrentTime);
  }

  // Only continue if message timeliness is respected
  if (timeliness_respected) {
    // Update sequence number & time stamp
    bool confirmed_sequence_number_changed = false;
    srcor_rasta_connections[connection_id].sequence_number_rx = msg_header->sequence_number + 1U;
    srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = msg_header->sequence_number;
    if (srcor_rasta_connections[connection_id].confirmed_sequence_number_rx != msg_header->confirmed_sequence_number) {
      srcor_rasta_connections[connection_id].confirmed_sequence_number_rx = msg_header->confirmed_sequence_number;
      confirmed_sequence_number_changed = true;
    }
    srcor_rasta_connections[connection_id].time_stamp_rx = msg_header->time_stamp;
    if (IsMessageTimeoutRelated(msg_header->message_type)) {
      srcor_rasta_connections[connection_id].confirmed_time_stamp_rx = msg_header->confirmed_time_stamp;
    } else if (msg_header->message_type == srtyp_kSrMessageConnReq) {
      // [3] Set CS_R = SN_T - 1 & CTS_R = T_local
      srcor_rasta_connections[connection_id].confirmed_sequence_number_rx = srcor_rasta_connections[connection_id].sequence_number_tx - 1U;
      srcor_rasta_connections[connection_id].confirmed_time_stamp_rx = kCurrentTime;
    } else {
      // Nothing to do here
    }

    // Remove confirmed messages from send buffer if confirmed sequence number changed and message type is not ConnReq
    // ConnReq can not confirm anything since its the first message exchanged.
    if (confirmed_sequence_number_changed && (msg_header->message_type != srtyp_kSrMessageConnReq)) {
      srsend_RemoveFromBuffer(connection_id, srcor_rasta_connections[connection_id].confirmed_sequence_number_rx);
    }

    // Handle received ConnReq & ConnResp
    if ((msg_header->message_type == srtyp_kSrMessageConnReq) || (msg_header->message_type == srtyp_kSrMessageConnResp)) {
      // Save received opposite buffer size
      srtyp_ProtocolVersion protocol_version;  // Only needed for srmsg_GetConnMessageData() call to extract opposite buffer size. Value never used.
      srmsg_GetConnMessageData(&srcor_rasta_connections[connection_id].input_buffer.message_buffer, &protocol_version,
                               &srcor_rasta_connections[connection_id].opposite_receive_buffer_size);
    }

    // Handle received data message
    if ((msg_header->message_type == srtyp_kSrMessageData) || (msg_header->message_type == srtyp_kSrMessageRetrData)) {
      srmsg_GetDataMessagePayload(&srcor_rasta_connections[connection_id].input_buffer.message_buffer,
                                  &srcor_rasta_connections[connection_id].scratch_message_payload);

      srrece_AddToBuffer(connection_id, &srcor_rasta_connections[connection_id].scratch_message_payload);
    }

    // 11) Receive flow control
    ReceivedFlowControlCheck(connection_id, msg_header->message_type);

    // 12) Update connection diagnostics
    if (IsMessageTimeoutRelated(msg_header->message_type)) {
      srdia_UpdateConnectionDiagnostics(connection_id, srcor_rasta_connections[connection_id].t_rtd, srcor_rasta_connections[connection_id].t_alive);
    }
  }

  // Indicate that there is no longer a message in the input_buffer
  srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = false;

  return timeliness_respected;
}

void srcor_UpdateConfirmedTxSequenceNumber(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  // Only update sequence number to be confirmed (CS_T) when message in input buffer, not from CheckTimings-function
  if (srcor_rasta_connections[connection_id].input_buffer.message_in_buffer) {
    srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = srcor_rasta_connections[connection_id].input_buffer.message_header.sequence_number;

    // Reset flag for message in input buffer
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = false;

    ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u, CS_T: %u\n", connection_id, srcor_rasta_connections[connection_id].confirmed_sequence_number_tx);
  }
}

void srcor_UpdateConfirmedRxSequenceNumber(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  // Only update last confirmed sequence number (CS_R) when message in input buffer
  if (srcor_rasta_connections[connection_id].input_buffer.message_in_buffer) {
    srcor_rasta_connections[connection_id].confirmed_sequence_number_rx =
        srcor_rasta_connections[connection_id].input_buffer.message_header.confirmed_sequence_number;

    srsend_RemoveFromBuffer(connection_id, srcor_rasta_connections[connection_id].confirmed_sequence_number_rx);

    // Reset flag for message in input buffer
    srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = false;

    ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u, CS_R: %u\n", connection_id, srcor_rasta_connections[connection_id].confirmed_sequence_number_rx);
  }
}

bool srcor_IsProtocolVersionAccepted(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  bool proto_verion_accepted = true;

  // Assert if no message in input buffer to check protocol version
  raas_AssertTrue(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer, radef_kInternalError);

  srtyp_ProtocolVersion proto_version;
  uint16_t n_send_max;
  srmsg_GetConnMessageData(&srcor_rasta_connections[connection_id].input_buffer.message_buffer, &proto_version, &n_send_max);

  uint32_t index = 0U;
  bool check_ongoing = true;

  // Check if protocol version is on the same or higher version then itself. Start comparing from major to minor version, digit for digit.
  while ((index < SRTYP_PROTOCOL_VERSION_SIZE) && check_ongoing) {
    if (proto_version.version[index] > srcor_kProtocolVersion.version[index]) {
      check_ongoing = false;  // If a greater received version digit is found, the check can be stopped and the version is accepted
    } else if (proto_version.version[index] < srcor_kProtocolVersion.version[index]) {
      proto_verion_accepted = false;  // If a smaller received version digit is found, the check can be stopped and false is returned.
      check_ongoing = false;
    } else {
      // Similar received version digit found. Nothing to do here. Continue with check.
    }

    ++index;
  }

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u, proto version accepted: %B\n", connection_id, proto_verion_accepted);

  return proto_verion_accepted;
}

void srcor_SetReceivedMessagePendingFlag(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  srcor_rasta_connections[connection_id].received_data_pending = true;

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);
}

bool srcor_GetReceivedMessagePendingFlag(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u, Receive message pending flag: %B\n", connection_id,
                  srcor_rasta_connections[connection_id].received_data_pending);

  return srcor_rasta_connections[connection_id].received_data_pending;
}

void srcor_WriteMessagePayloadToTemporaryBuffer(const uint32_t connection_id, const uint16_t message_payload_size, const uint8_t *const message_payload) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);
  raas_AssertU16InRange(message_payload_size, srcty_kMinSrLayerPayloadDataSize, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE, radef_kInvalidParameter);
  raas_AssertNotNull(message_payload, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  srcor_TemporaryBuffer *const temp_buffer = &srcor_rasta_connections[connection_id].temporary_send_buffer;
  raas_AssertTrue(!temp_buffer->message_in_buffer, radef_kInternalError);

  temp_buffer->message_in_buffer = true;
  temp_buffer->message_payload_buffer.payload_size = message_payload_size;
  for (uint16_t index = 0U; index < message_payload_size; ++index) {
    temp_buffer->message_payload_buffer.payload[index] = message_payload[index];
  }
}

void srcor_ClearInputBufferMessagePendingFlag(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  srcor_rasta_connections[connection_id].input_buffer.message_in_buffer = false;
}

void srcor_SendDataMessage(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);
  raas_AssertTrue(srcor_rasta_connections[connection_id].temporary_send_buffer.message_in_buffer, radef_kNoMessageToSend);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  srtyp_SrMessageHeaderCreate *const msg_hdr_create = &srcor_rasta_connections[connection_id].scratch_msg_header_create;
  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].scratch_message;

  msg_hdr_create->sender_id = srcor_sr_configuration->connection_configurations[connection_id].sender_id;
  msg_hdr_create->receiver_id = srcor_sr_configuration->connection_configurations[connection_id].receiver_id;
  msg_hdr_create->sequence_number = GetCurrentSequenceNumberAndIncrementNumber(connection_id);
  msg_hdr_create->confirmed_time_stamp = srcor_rasta_connections[connection_id].time_stamp_rx;

  srmsg_CreateDataMessage(*msg_hdr_create, &srcor_rasta_connections[connection_id].temporary_send_buffer.message_payload_buffer, msg);

  srcor_rasta_connections[connection_id].temporary_send_buffer.message_in_buffer = false;

  srsend_AddToBuffer(connection_id, msg);

  srcor_SendPendingMessages(connection_id);
}

void srcor_SendConnReqMessage(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  srtyp_SrMessageHeaderCreate *const msg_hdr_create = &srcor_rasta_connections[connection_id].scratch_msg_header_create;
  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].scratch_message;

  msg_hdr_create->sender_id = srcor_sr_configuration->connection_configurations[connection_id].sender_id;
  msg_hdr_create->receiver_id = srcor_sr_configuration->connection_configurations[connection_id].receiver_id;
  msg_hdr_create->sequence_number = GetCurrentSequenceNumberAndIncrementNumber(connection_id);
  srcor_rasta_connections[connection_id].time_stamp_rx = 0U;  // Set to 0 for ConnRequest
  msg_hdr_create->confirmed_time_stamp = srcor_rasta_connections[connection_id].time_stamp_rx;

  srmsg_CreateConnReqMessage(*msg_hdr_create, srcor_kProtocolVersion, srcor_sr_configuration->n_send_max, msg);

  srsend_AddToBuffer(connection_id, msg);

  srcor_SendPendingMessages(connection_id);
}

void srcor_SendConnRespMessage(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  srtyp_SrMessageHeaderCreate *const msg_hdr_create = &srcor_rasta_connections[connection_id].scratch_msg_header_create;
  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].scratch_message;

  msg_hdr_create->sender_id = srcor_sr_configuration->connection_configurations[connection_id].sender_id;
  msg_hdr_create->receiver_id = srcor_sr_configuration->connection_configurations[connection_id].receiver_id;
  msg_hdr_create->sequence_number = GetCurrentSequenceNumberAndIncrementNumber(connection_id);
  msg_hdr_create->confirmed_time_stamp = srcor_rasta_connections[connection_id].time_stamp_rx;

  srmsg_CreateConnRespMessage(*msg_hdr_create, srcor_kProtocolVersion, srcor_sr_configuration->n_send_max, msg);

  srsend_AddToBuffer(connection_id, msg);

  srcor_SendPendingMessages(connection_id);
}

void srcor_SendDiscReqMessage(const uint32_t connection_id, const sraty_DiscReason disconnect_reason) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);
  raas_AssertU32InRange((uint32_t)disconnect_reason, (uint32_t)sraty_kDiscReasonMin, (uint32_t)sraty_kDiscReasonMax - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  const uint32_t kRedChannelId = connection_id;  // 1:1 mapping between the connection id and redundancy channel id
  srtyp_SrMessageHeaderCreate *const msg_hdr_create = &srcor_rasta_connections[connection_id].scratch_msg_header_create;
  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].scratch_message;

  msg_hdr_create->sender_id = srcor_sr_configuration->connection_configurations[connection_id].sender_id;
  msg_hdr_create->receiver_id = srcor_sr_configuration->connection_configurations[connection_id].receiver_id;
  msg_hdr_create->sequence_number = GetCurrentSequenceNumberAndIncrementNumber(connection_id);
  msg_hdr_create->confirmed_time_stamp = srcor_rasta_connections[connection_id].time_stamp_rx;

  srmsg_CreateDiscReqMessage(*msg_hdr_create, srcor_rasta_connections[connection_id].detailed_disconnect_reason, disconnect_reason, msg);

  srtyp_SrMessageHeaderUpdate *const msg_hdr_update = &srcor_rasta_connections[connection_id].scratch_msg_header_update;
  msg_hdr_update->time_stamp = rasys_GetTimerValue();
  msg_hdr_update->confirmed_sequence_number = srcor_rasta_connections[connection_id].confirmed_sequence_number_tx;
  srmsg_UpdateMessageHeader(*msg_hdr_update, msg);

  sradin_SendMessage(kRedChannelId, msg->message_size, msg->message);
  srsend_InitBuffer(connection_id);  // Reset send buffer of this connection to delete all not send messages

  srcor_CloseRedundancyChannel(connection_id);
}

void srcor_SetDiscDetailedReason(const uint32_t connection_id, const uint16_t detailed_disconnect_reason) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  srcor_rasta_connections[connection_id].detailed_disconnect_reason = detailed_disconnect_reason;
}

void srcor_SendHbMessage(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  srtyp_SrMessageHeaderCreate *const msg_hdr_create = &srcor_rasta_connections[connection_id].scratch_msg_header_create;
  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].scratch_message;

  msg_hdr_create->sender_id = srcor_sr_configuration->connection_configurations[connection_id].sender_id;
  msg_hdr_create->receiver_id = srcor_sr_configuration->connection_configurations[connection_id].receiver_id;
  msg_hdr_create->sequence_number = GetCurrentSequenceNumberAndIncrementNumber(connection_id);
  msg_hdr_create->confirmed_time_stamp = srcor_rasta_connections[connection_id].time_stamp_rx;

  srmsg_CreateHeartbeatMessage(*msg_hdr_create, msg);

  srsend_AddToBuffer(connection_id, msg);

  srcor_SendPendingMessages(connection_id);
}

void srcor_SendRetrReqMessage(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  srtyp_SrMessageHeaderCreate *const msg_hdr_create = &srcor_rasta_connections[connection_id].scratch_msg_header_create;
  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].scratch_message;

  msg_hdr_create->sender_id = srcor_sr_configuration->connection_configurations[connection_id].sender_id;
  msg_hdr_create->receiver_id = srcor_sr_configuration->connection_configurations[connection_id].receiver_id;
  msg_hdr_create->sequence_number = GetCurrentSequenceNumberAndIncrementNumber(connection_id);
  msg_hdr_create->confirmed_time_stamp = srcor_rasta_connections[connection_id].time_stamp_rx;

  srmsg_CreateRetrReqMessage(*msg_hdr_create, msg);

  srsend_AddToBuffer(connection_id, msg);

  srcor_SendPendingMessages(connection_id);
}

void srcor_HandleRetrReq(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  uint32_t next_sequence_number = GetCurrentSequenceNumberAndIncrementNumber(connection_id);

  srtyp_SrMessageHeaderCreate *const msg_hdr_create = &srcor_rasta_connections[connection_id].scratch_msg_header_create;
  msg_hdr_create->sender_id = srcor_sr_configuration->connection_configurations[connection_id].sender_id;
  msg_hdr_create->receiver_id = srcor_sr_configuration->connection_configurations[connection_id].receiver_id;
  msg_hdr_create->sequence_number = next_sequence_number;
  msg_hdr_create->confirmed_time_stamp = srcor_rasta_connections[connection_id].time_stamp_rx;

  srsend_PrepareBufferForRetr(connection_id, srcor_rasta_connections[connection_id].confirmed_sequence_number_rx, *msg_hdr_create, &next_sequence_number);

  srcor_rasta_connections[connection_id].sequence_number_tx = next_sequence_number;  // Save back current sequence number after reworking send buffer
}

bool srcor_IsRetrReqSequenceNumberAvailable(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  bool sequence_number_available = false;

  raas_AssertTrue(srcor_rasta_connections[connection_id].input_buffer.message_in_buffer, radef_kInternalError);

  const srtyp_SrMessageHeader *const msg_header = &srcor_rasta_connections[connection_id].input_buffer.message_header;

  if (msg_header->message_type == srtyp_kSrMessageRetrReq) {
    // Check if the next sequence number after the last received confirmed sequence number is in buffer
    const radef_RaStaReturnCode kResult = srsend_IsSequenceNumberInBuffer(connection_id, msg_header->confirmed_sequence_number + 1U);
    if (kResult == radef_kNoError) {
      sequence_number_available = true;
    }
  }

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u, SN available: %B\n", connection_id, sequence_number_available);

  return sequence_number_available;
}

bool srcor_IsConnRoleServer(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  bool is_server = true;

  if (srcor_sr_configuration->connection_configurations[connection_id].sender_id <
      srcor_sr_configuration->connection_configurations[connection_id].receiver_id) {
    is_server = false;
  }

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u, Is server: %B\n", connection_id, is_server);

  return is_server;
}

bool srcor_IsMessageTimeout(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  bool msg_timeout = false;

  // Unsigned integer wrap around allowed here
  if ((rasys_GetTimerValue() - srcor_rasta_connections[connection_id].confirmed_time_stamp_rx) > srcor_rasta_connections[connection_id].timer_t_i) {
    msg_timeout = true;
  }

  return msg_timeout;
}

bool srcor_IsHeartbeatInterval(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  bool hb_interval = false;

  // Unsigned integer wrap around allowed here
  if ((rasys_GetTimerValue() - srcor_rasta_connections[connection_id].time_stamp_tx) >= srcor_sr_configuration->t_h) {
    hb_interval = true;
  }

  return hb_interval;
}

bool srcor_IsReceivedMsgPendingAndBuffersNotFull(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  bool ret = srcor_GetReceivedMessagePendingFlag(connection_id);
  ret = (srrece_GetFreeBufferEntries(connection_id) >= srcty_kMinFreeEntriesReceivedBufferForReceive) && ret;
  ret = (srsend_GetFreeBufferEntries(connection_id) >= srcty_kMinFreeEntriesSendBufferForRetr) && ret;

  return ret;
}

void srcor_SendPendingMessages(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  uint32_t kRedChannelId = connection_id;  // 1:1 mapping between the connection id and redundancy channel id

  srtyp_SrMessage *const msg = &srcor_rasta_connections[connection_id].scratch_message;
  srtyp_SrMessageHeader *const msg_hdr = &srcor_rasta_connections[connection_id].scratch_msg_header;
  srtyp_SrMessageHeaderUpdate *const msg_hdr_update = &srcor_rasta_connections[connection_id].scratch_msg_header_update;

  while (SendPendingMessagesWithFlowControlAllowed(connection_id)) {
    if (srsend_ReadMessageToSend(connection_id, msg) != radef_kNoError) {
      rasys_FatalError(radef_kInternalError);  // No message could be read although previously checked if messages are in buffer
    }

    // Update current time stamp
    srcor_rasta_connections[connection_id].time_stamp_tx = rasys_GetTimerValue();
    msg_hdr_update->time_stamp = srcor_rasta_connections[connection_id].time_stamp_tx;

    // Update confirmed sequence number CS_T
    srmsg_GetMessageHeader(msg, msg_hdr);
    if (msg_hdr->message_type == srtyp_kSrMessageConnReq) {
      srcor_rasta_connections[connection_id].confirmed_sequence_number_tx = 0;  // For a ConnReq, CS_T must be set to 0
    }
    msg_hdr_update->confirmed_sequence_number = srcor_rasta_connections[connection_id].confirmed_sequence_number_tx;
    srcor_rasta_connections[connection_id].last_send_confirmed_sequence_number_tx =
        msg_hdr_update->confirmed_sequence_number;  // Backup last send confirmed sequence number

    srmsg_UpdateMessageHeader(*msg_hdr_update, msg);

    sradin_SendMessage(kRedChannelId, msg->message_size, msg->message);
  }
}

void srcor_SendConnectionStateNotification(const uint32_t connection_id, const sraty_ConnectionStates connection_state,
                                           const sraty_DiscReason disconnect_reason) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);
  raas_AssertU32InRange((uint32_t)connection_state, (uint32_t)sraty_kConnectionMin, (uint32_t)sraty_kConnectionMax - 1U, radef_kInvalidParameter);
  raas_AssertU32InRange((uint32_t)disconnect_reason, (uint32_t)sraty_kDiscReasonMin, (uint32_t)sraty_kDiscReasonMax - 1U, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srcor_logger_id, "Connection id: %u\n", connection_id);

  // Collect notification data
  sraty_BufferUtilisation buffer_util;
  uint16_t opposite_buffer_size;
  srcor_GetBufferSizeAndUtilisation(connection_id, &buffer_util, &opposite_buffer_size);

  srnot_ConnectionStateNotification(connection_id, connection_state, buffer_util, opposite_buffer_size, disconnect_reason,
                                    srcor_rasta_connections[connection_id].detailed_disconnect_reason);
}

void srcor_GetBufferSizeAndUtilisation(const uint32_t connection_id, sraty_BufferUtilisation *const buffer_utilisation, uint16_t *const opposite_buffer_size) {
  // Input parameter check
  raas_AssertTrue(srcor_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInvalidParameter);
  raas_AssertNotNull(buffer_utilisation, radef_kInvalidParameter);
  raas_AssertNotNull(opposite_buffer_size, radef_kInvalidParameter);

  // Get informations from receive and send buffer
  buffer_utilisation->receive_buffer_free = srrece_GetFreeBufferEntries(connection_id);
  buffer_utilisation->receive_buffer_used = srrece_GetUsedBufferEntries(connection_id);
  buffer_utilisation->send_buffer_free = srsend_GetFreeBufferEntries(connection_id);
  buffer_utilisation->send_buffer_used = srsend_GetUsedBufferEntries(connection_id);

  // Get opposite buffer size
  *opposite_buffer_size = srcor_rasta_connections[connection_id].opposite_receive_buffer_size;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static bool CheckConnectionConfigurations(const uint32_t number_of_connections, const srcty_ConnectionConfiguration *const connection_configurations) {
  // Input parameter check
  raas_AssertU32InRange(number_of_connections, srcty_kMinNumberOfRaStaConnections, RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS, radef_kInternalError);
  raas_AssertNotNull(connection_configurations, radef_kInternalError);

  bool connection_config_valid = true;

  for (uint32_t conn_index = 0U; conn_index < number_of_connections; ++conn_index) {
    // Check that connection index is in ascending order starting from 0
    if (connection_configurations[conn_index].connection_id != conn_index) {
      connection_config_valid = false;
    } else if (connection_configurations[conn_index].sender_id == connection_configurations[conn_index].receiver_id) {
      connection_config_valid = false;
    } else {
      // Nothing to do here
    }
  }

  return connection_config_valid;
}

static bool IsMessageTimeoutRelated(const srtyp_SrMessageType message_type) {
  // Input parameter check
  raas_AssertU32InRange((uint32_t)message_type, (uint32_t)srtyp_kSrMessageMin, (uint32_t)srtyp_kSrMessageMax - 1U, radef_kInternalError);

  bool is_timeout_related = false;

  if ((message_type == srtyp_kSrMessageHb) || (message_type == srtyp_kSrMessageData) || (message_type == srtyp_kSrMessageRetrData)) {
    is_timeout_related = true;
  }

  return is_timeout_related;
}

static uint32_t GetCurrentSequenceNumberAndIncrementNumber(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);

  const uint32_t kCurrentSequenceNumber = srcor_rasta_connections[connection_id].sequence_number_tx;

  // Increment sequence
  ++srcor_rasta_connections[connection_id].sequence_number_tx;

  return kCurrentSequenceNumber;
}

static bool SendPendingMessagesWithFlowControlAllowed(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);

  uint16_t not_confirmed_messages = srsend_GetUsedBufferEntries(connection_id);
  not_confirmed_messages -= srsend_GetNumberOfMessagesToSend(connection_id);

  bool sending_allowed = (srsend_GetNumberOfMessagesToSend(connection_id) > 0U);
  sending_allowed = (srcor_rasta_connections[connection_id].opposite_receive_buffer_size > not_confirmed_messages) &&
                    sending_allowed;  // Flow control, rule 1: Sending allowed, when not confirmed messages in send buffer < opposite N_sendMax

  return sending_allowed;
}

static void ReceivedFlowControlCheck(const uint32_t connection_id, const srtyp_SrMessageType message_type) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertU32InRange((uint32_t)message_type, (uint32_t)srtyp_kSrMessageMin, (uint32_t)srtyp_kSrMessageMax - 1U, radef_kInternalError);

  // Received flow control is only applied for HB, RetrResp, Data & RetrData messages, as for this message types no  message from the opposite side is assumed
  if ((message_type == srtyp_kSrMessageHb) || (message_type == srtyp_kSrMessageRetrResp) || (message_type == srtyp_kSrMessageData) ||
      (message_type == srtyp_kSrMessageRetrData)) {
    uint32_t not_confirmed_sequence_numbers =
        srcor_rasta_connections[connection_id].confirmed_sequence_number_tx - srcor_rasta_connections[connection_id].last_send_confirmed_sequence_number_tx;

    if (not_confirmed_sequence_numbers >= srcor_sr_configuration->m_w_a) {
      if (srsend_GetNumberOfMessagesToSend(connection_id) == 0U) {
        srcor_SendHbMessage(connection_id);  // Send a HB message if no other messages to send to confirm sequence number
      } else {
        srcor_SendPendingMessages(connection_id);  // Send messages in send buffer to confirm sequence number
      }
    }
  }
}

static bool GeneralMessageCheck(const uint32_t connection_id, const srtyp_SrMessage *const msg, srtyp_SrMessageHeader *const msg_hdr) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertNotNull(msg, radef_kInternalError);
  raas_AssertNotNull(msg_hdr, radef_kInternalError);

  bool ok = true;

  const radef_RaStaReturnCode kResult = srmsg_CheckMessage(msg);

  if (kResult == radef_kNoError) {
    srmsg_GetMessageHeader(msg, msg_hdr);
  } else if (kResult == radef_kInvalidMessageSize) {
    ok = false;
  } else if (kResult == radef_kInvalidMessageMd4) {
    srdia_IncSafetyCodeErrorCounter(connection_id);
    ok = false;
  } else if (kResult == radef_kInvalidMessageType) {
    srdia_IncTypeErrorCounter(connection_id);
    ok = false;
  } else {
    rasys_FatalError(radef_kInternalError);  // Other return values are not expected for srmsg_CheckMessage()
  }

  // Check authenticity of message
  if ((msg_hdr->receiver_id != srcor_sr_configuration->connection_configurations[connection_id].sender_id) ||
      (msg_hdr->sender_id != srcor_sr_configuration->connection_configurations[connection_id].receiver_id)) {
    srdia_IncAddressErrorCounter(connection_id);
    ok = false;
  }

  return ok;
}

static bool CheckSequenceNumberRange(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertNotNull(msg_hdr, radef_kInternalError);

  bool ok = false;
  uint32_t value_difference;

  switch (msg_hdr->message_type) {
    case srtyp_kSrMessageConnReq:   // fall-through
    case srtyp_kSrMessageConnResp:  // fall-through
    case srtyp_kSrMessageRetrResp:
      ok = true;  // Sequence number range always accepted
      break;
    case srtyp_kSrMessageRetrReq:  // fall-through
    case srtyp_kSrMessageDiscReq:  // fall-through
    case srtyp_kSrMessageHb:       // fall-through
    case srtyp_kSrMessageData:     // fall-through
    case srtyp_kSrMessageRetrData:
      value_difference = msg_hdr->sequence_number - srcor_rasta_connections[connection_id].sequence_number_rx;  // Unsigned integer wrap around allowed here
      ok = rahlp_IsU32InRange(value_difference, 0U, 10U * (uint32_t)srcor_sr_configuration->n_send_max);
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);  // Can not happen because message type was validated before
      break;
      // no coverage end
  }

  return ok;
}

static bool CheckSequenceNumber(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertNotNull(msg_hdr, radef_kInternalError);

  bool sequence_number_in_seq = false;

  switch (msg_hdr->message_type) {
    case srtyp_kSrMessageConnReq:   // fall-through
    case srtyp_kSrMessageConnResp:  // fall-through
    case srtyp_kSrMessageRetrResp:  // fall-through
    case srtyp_kSrMessageDiscReq:
      sequence_number_in_seq = true;  // Sequence number always accepted
      break;
    case srtyp_kSrMessageRetrReq:  // fall-through
    case srtyp_kSrMessageHb:       // fall-through
    case srtyp_kSrMessageData:     // fall-through
    case srtyp_kSrMessageRetrData:
      if (msg_hdr->sequence_number == srcor_rasta_connections[connection_id].sequence_number_rx) {
        sequence_number_in_seq = true;
      }
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);  // Can not happen as message type was validated before
      break;
      // no coverage end
  }

  // Increment error counter if check failed
  if (!sequence_number_in_seq) {
    srdia_IncSequenceNumberErrorCounter(connection_id);
  }

  return sequence_number_in_seq;
}

static bool CheckConfirmedSequenceNumber(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertNotNull(msg_hdr, radef_kInternalError);

  bool confirmed_sequence_nbr_in_seq = false;
  uint32_t value_difference;
  uint32_t max_value_difference;

  switch (msg_hdr->message_type) {
    case srtyp_kSrMessageConnReq:
      if (msg_hdr->confirmed_sequence_number == 0U) {
        confirmed_sequence_nbr_in_seq = true;
      }
      break;
    case srtyp_kSrMessageConnResp:
      if (msg_hdr->confirmed_sequence_number == (srcor_rasta_connections[connection_id].sequence_number_tx - 1U)) {
        confirmed_sequence_nbr_in_seq = true;
      }
      break;
    case srtyp_kSrMessageRetrReq:   // fall-through
    case srtyp_kSrMessageRetrResp:  // fall-through
    case srtyp_kSrMessageDiscReq:   // fall-through
    case srtyp_kSrMessageHb:        // fall-through
    case srtyp_kSrMessageData:      // fall-through
    case srtyp_kSrMessageRetrData:
      value_difference = msg_hdr->confirmed_sequence_number -
                         srcor_rasta_connections[connection_id].confirmed_sequence_number_rx;  // Unsigned integer wrap around allowed here
      max_value_difference = srcor_rasta_connections[connection_id].sequence_number_tx -
                             (srcor_rasta_connections[connection_id].confirmed_sequence_number_rx - 1U);  // Unsigned integer wrap around allowed here
      confirmed_sequence_nbr_in_seq = rahlp_IsU32InRange(value_difference, 0U, max_value_difference);
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);  // Can not happen as message type was validated before
      break;
      // no coverage end
  }

  // Increment error counter if check failed
  if (!confirmed_sequence_nbr_in_seq) {
    srdia_IncConfirmedSequenceNumberErrorCounter(connection_id);
  }

  return confirmed_sequence_nbr_in_seq;
}

static bool CheckTimeStamp(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertNotNull(msg_hdr, radef_kInternalError);

  bool time_stamp_in_seq = false;
  uint32_t difference_time_stamp;

  switch (msg_hdr->message_type) {
    case srtyp_kSrMessageConnReq:   // fall-through
    case srtyp_kSrMessageConnResp:  // fall-through
    case srtyp_kSrMessageRetrReq:   // fall-through
    case srtyp_kSrMessageRetrResp:  // fall-through
    case srtyp_kSrMessageDiscReq:
      time_stamp_in_seq = true;  // Time stamp not checked
      break;
    case srtyp_kSrMessageHb:    // fall-through
    case srtyp_kSrMessageData:  // fall-through
    case srtyp_kSrMessageRetrData:
      difference_time_stamp = msg_hdr->time_stamp - srcor_rasta_connections[connection_id].time_stamp_rx;  // Unsigned integer wrap around allowed here
      time_stamp_in_seq = rahlp_IsU32InRange(difference_time_stamp, 0U, srcor_sr_configuration->t_max - 1U);
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);  // Can not happen as message type was validated before
      break;
      // no coverage end
  }

  return time_stamp_in_seq;
}

static bool CheckConfirmedTimeStamp(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertNotNull(msg_hdr, radef_kInternalError);

  bool confirmed_time_stamp_in_seq = false;
  uint32_t difference_confirmed_time_stamp;

  switch (msg_hdr->message_type) {
    case srtyp_kSrMessageConnReq:   // fall-through
    case srtyp_kSrMessageConnResp:  // fall-through
    case srtyp_kSrMessageRetrReq:   // fall-through
    case srtyp_kSrMessageRetrResp:  // fall-through
    case srtyp_kSrMessageDiscReq:
      confirmed_time_stamp_in_seq = true;  // Confirmed time stamp not checked
      break;
    case srtyp_kSrMessageHb:    // fall-through
    case srtyp_kSrMessageData:  // fall-through
    case srtyp_kSrMessageRetrData:
      difference_confirmed_time_stamp =
          msg_hdr->confirmed_time_stamp - srcor_rasta_connections[connection_id].confirmed_time_stamp_rx;  // Unsigned integer wrap around allowed here
      confirmed_time_stamp_in_seq = rahlp_IsU32InRange(difference_confirmed_time_stamp, 0U, srcor_sr_configuration->t_max - 1U);
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);  // Can not happen as message type was validated before
      break;
      // no coverage end
  }

  return confirmed_time_stamp_in_seq;
}

static bool CalculateTimeliness(const uint32_t connection_id, const srtyp_SrMessageHeader *const msg_hdr, const uint32_t current_time) {
  // Input parameter check
  raas_AssertTrue(connection_id < srcor_sr_configuration->number_of_connections, radef_kInternalError);
  raas_AssertNotNull(msg_hdr, radef_kInternalError);

  bool timeliness_respected = true;

  // Calculate T_rtd & T_alive
  srcor_rasta_connections[connection_id].t_rtd =
      current_time + srcor_timer_granularity - msg_hdr->confirmed_time_stamp;  // Unsigned integer wrap around allowed here
  srcor_rasta_connections[connection_id].t_alive =
      current_time - srcor_rasta_connections[connection_id].confirmed_time_stamp_rx;  // Unsigned integer wrap around allowed here

  // Calculate new timer T_i value
  if (srcor_sr_configuration->t_max >= srcor_rasta_connections[connection_id].t_rtd) {
    srcor_rasta_connections[connection_id].timer_t_i = srcor_sr_configuration->t_max - srcor_rasta_connections[connection_id].t_rtd;
  } else {
    // Timeliness of messages can no longer be guaranteed
    srcor_rasta_connections[connection_id].timer_t_i = 0U;
    timeliness_respected = false;
  }

  return timeliness_respected;
}

static void SetConnectionEvent(const srtyp_SrMessageType message_type, srtyp_ConnectionEvents *const connection_event) {
  // Input parameter check
  raas_AssertU32InRange((uint32_t)message_type, (uint32_t)srtyp_kSrMessageMin, (uint32_t)srtyp_kSrMessageMax - 1U, radef_kInternalError);
  raas_AssertNotNull(connection_event, radef_kInternalError);

  switch (message_type) {
    case srtyp_kSrMessageConnReq:
      *connection_event = srtyp_kConnEventConnReqReceived;
      break;
    case srtyp_kSrMessageConnResp:
      *connection_event = srtyp_kConnEventConnRespReceived;
      break;
    case srtyp_kSrMessageRetrReq:
      *connection_event = srtyp_kConnEventRetrReqReceived;
      break;
    case srtyp_kSrMessageRetrResp:
      *connection_event = srtyp_kConnEventRetrRespReceived;
      break;
    case srtyp_kSrMessageDiscReq:
      *connection_event = srtyp_kConnEventDiscReqReceived;
      break;
    case srtyp_kSrMessageHb:
      *connection_event = srtyp_kConnEventHbReceived;
      break;
    case srtyp_kSrMessageData:
      *connection_event = srtyp_kConnEventDataReceived;
      break;
    case srtyp_kSrMessageRetrData:
      *connection_event = srtyp_kConnEventRetrDataReceived;
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);  // Can not happen as message type was validated before
      break;
      // no coverage end
  }
}
