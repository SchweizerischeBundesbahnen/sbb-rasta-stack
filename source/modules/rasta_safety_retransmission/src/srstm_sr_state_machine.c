/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srstm_sr_state_machine.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA SafRetL state machine module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srstm_sr_state_machine.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "srcor_sr_core.h"   // NOLINT(build/include_subdir)
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
PRIVATE bool srstm_initialized = false;

/**
 * @brief Number of configured RaSTA connections.
 */
PRIVATE uint32_t srstm_number_of_connections = 0U;

/**
 * @brief State machine states of the RaSTA connections.
 */
PRIVATE sraty_ConnectionStates srstm_connection_states[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

/**
 * @brief Old state machine states of the RaSTA connections.
 */
PRIVATE sraty_ConnectionStates srstm_connection_states_old[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup sr_sm
 * @{
 */

/**
 * @brief Process events in state Closed.
 *
 * This internal function is used to process all incoming events when the state machine is in the "closed"-state. Detailed information about transitions can be
 * found in
 * ::srstm_ProcessConnectionStateMachine.
 *
 * @pre It is checked that the state machine of the given connection is in the ::sraty_kConnectionClosed state, otherwise an ::radef_kInternalError is
 * thrown.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-560} sr_state_machine Events
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] event Received event to process. The event has a valid range from ::srtyp_kConnEventMin <= value < ::srtyp_kConnEventMax, otherwise a
 * ::radef_kInternalError fatal error is thrown.
 */
static void ProcessStateClosedEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event);

/**
 * @brief Process events in state Down.
 *
 * This internal function is used to process all incoming events when the state machine is in the "down"-state. Detailed information about transitions can be
 * found in ::srstm_ProcessConnectionStateMachine.
 *
 * @pre It is checked that the state machine of the given connection is in the ::sraty_kConnectionDown state, otherwise an ::radef_kInternalError is
 * thrown.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-560} sr_state_machine Events
 * @implementsReq{RASW-755} Process State Machine Receive ConnReq Message Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] event Received event to process. The event has a valid range from ::srtyp_kConnEventMin <= value < ::srtyp_kConnEventMax, otherwise a
 * ::radef_kInternalError fatal error is thrown.
 */
static void ProcessStateDownEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event);

/**
 * @brief Process events in state Start.
 *
 * This internal function is used to process all incoming events when the state machine is in the "start"-state. Detailed information about transitions can be
 * found in ::srstm_ProcessConnectionStateMachine.
 *
 * @pre It is checked that the state machine of the given connection is in the ::sraty_kConnectionStart state, otherwise an ::radef_kInternalError is
 * thrown.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-560} sr_state_machine Events
 * @implementsReq{RASW-757} Process State Machine Receive ConnResp Message Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] event Received event to process. The event has a valid range from ::srtyp_kConnEventMin <= value < ::srtyp_kConnEventMax, otherwise a
 * ::radef_kInternalError fatal error is thrown.
 * @param [in] sequence_number_in_seq true, if the received sequence number is in sequence
 * @param [in] confirmed_time_stamp_in_seq true, if the confirmed timestamp is in sequence
 */
static void ProcessStateStartEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                    const bool confirmed_time_stamp_in_seq);

/**
 * @brief Process events in state Up.
 *
 * This internal function is used to process all incoming events when the state machine is in the "up"-state. Detailed information about transitions
 * can be found in ::srstm_ProcessConnectionStateMachine.
 *
 * @pre It is checked that the state machine of the given connection is in the ::sraty_kConnectionUp state, otherwise an ::radef_kInternalError is
 * thrown.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-560} sr_state_machine Events
 * @implementsReq{RASW-759} Process State Machine Receive Data Message (in Sequence) Sequence
 * @implementsReq{RASW-761} Process State Machine Receive RetrReq Message (in Sequence) Sequence
 * @implementsReq{RASW-763} Process State Machine Receive RetrReq Message (not in Sequence) Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] event Received event to process. The event has a valid range from ::srtyp_kConnEventMin <= value < ::srtyp_kConnEventMax, otherwise a
 * ::radef_kInternalError fatal error is thrown.
 * @param [in] sequence_number_in_seq true, if the received sequence number is in sequence
 * @param [in] confirmed_time_stamp_in_seq true, if the confirmed timestamp is in sequence
 */
static void ProcessStateUpEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                 const bool confirmed_time_stamp_in_seq);

/**
 * @brief Process events in state RetransRequest.
 *
 * This internal function is used to process all incoming events when the state machine is in the "retransmission request"-state. Detailed information about
 * transitions can be found in ::srstm_ProcessConnectionStateMachine.
 *
 * @pre It is checked that the state machine of the given connection is in the ::sraty_kConnectionRetransRequest state, otherwise an
 * ::radef_kInternalError is thrown.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-560} sr_state_machine Events
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] event Received event to process. The event has a valid range from ::srtyp_kConnEventMin <= value < ::srtyp_kConnEventMax, otherwise a
 * ::radef_kInternalError fatal error is thrown.
 * @param [in] sequence_number_in_seq true, if the received sequence number is in sequence
 */
static void ProcessStateRetransRequestEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq);

/**
 * @brief Process events in state RetransRunning.
 *
 * This internal function is used to process all incoming events when the state machine is in the "retransmission running"-state. Detailed information about
 * transitions can be found in ::srstm_ProcessConnectionStateMachine.
 *
 * @pre It is checked that the state machine of the given connection is in the ::sraty_kConnectionRetransRunning state, otherwise an
 * ::radef_kInternalError is thrown.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-560} sr_state_machine Events
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] event Received event to process. The event has a valid range from ::srtyp_kConnEventMin <= value < ::srtyp_kConnEventMax, otherwise a
 * ::radef_kInternalError fatal error is thrown.
 * @param [in] sequence_number_in_seq true, if the received sequence number is in sequence
 * @param [in] confirmed_time_stamp_in_seq true, if the confirmed timestamp is in sequence
 */
static void ProcessStateRetransRunningEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                             const bool confirmed_time_stamp_in_seq);

/**
 * @brief Process received message and evaluate timeliness
 *
 * This internal function is used to process a received message by calling ::srcor_ProcessReceivedMessage. If the message timeliness is no longer guaranteed,
 * the connection is closed using ::CloseConnection with an ::sraty_kDiscReasonTimeout as disconnect reason. Finally, the result of the
 * ::srcor_ProcessReceivedMessage is returned.
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return true, if message timeliness is respected.
 * @return false, if message timeliness is no longer guaranteed.
 */
static bool ProcessReceivedMessage(const uint32_t connection_id);

/**
 * @brief Close the connection of a specific RaSTA connection
 *
 * This internal function is used to close a connection of a specific connection. It reflects the action [1] of the state machine. In case of an incoming
 * message, the confirmed sequence number TX is updated. After that in all cases a DiscReq message is send and the connection state updated to
 * ::sraty_kConnectionClosed.
 *
 * @remark This function shall only be used if the RaSTA connection is already established to the opposite side.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] disconnect_reason Disconnect reason in case of a change into the closed state. The reason has a valid range from ::sraty_kDiscReasonMin <=
 * value < ::sraty_kDiscReasonMax. If the value is outside this range, a ::radef_kInternalError fatal error is thrown.
 * @param [in] is_incoming_message True if triggered by an incoming message.
 */
static void CloseConnection(const uint32_t connection_id, const sraty_DiscReason disconnect_reason, const bool is_incoming_message);

/**
 * @brief Close the redundancy channel of a specific RaSTA connection
 *
 * This internal function is used to close a redundancy channel of a specific connection. It reflects the action [1] of the state machine. In case of an
 * incoming message, the confirmed sequence number TX is updated. Finally the connection state is updated to ::sraty_kConnectionClosed.
 *
 * @remark This function shall only be used if the RaSTA connection was not yet established to the opposite side and only the redundancy channels are open.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] is_incoming_message True if triggered by an incoming message.
 */
static void CloseRedundancyChannel(const uint32_t connection_id, const bool is_incoming_message);

/**
 * @brief Start a retransmission
 *
 * This internal function is used to start a retransmission for a specific connection. It reflects a part of action [4] of the state machine by updating the
 * confirmed sequence number CS_R with the received sequence number CS_PDU. The core module is triggered to handle the retransmission request. If missing
 * messages were detected on this side, a retransmission request is send to the opposite side. Finally the state is updated, if a state change is detected.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] new_state New state to change into. The state has a valid range from ::sraty_kConnectionMin <= value < ::sraty_kConnectionMax. If the value is
 * outside this range, a ::radef_kInternalError fatal error is thrown.
 * @param [in] retransmission_requested True if a retransmission request must be send to the opposite side
 */
static void StartRetransmission(const uint32_t connection_id, const sraty_ConnectionStates new_state, bool retransmission_requested);

/**
 * @brief Update the connection state and send a connection state notification to the application layer
 *
 * This internal function is used to update the connection state of a specific RaSTA connection. Then a connection state notification with the updated data is
 * send to the application.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-749} Connection State Notification Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] new_state New state to change into. The state has a valid range from ::sraty_kConnectionMin <= value < ::sraty_kConnectionMax. If the value is
 * outside this range, a ::radef_kInternalError fatal error is thrown.
 */
static void UpdateConnectionState(const uint32_t connection_id, const sraty_ConnectionStates new_state);

/**
 * @brief Update the connection state and send a connection state notification to the application layer
 *
 * This internal function is used to update the connection state of a specific RaSTA connection It is used when the connection is closed and a specific
 * disconnection reason shall be passed. Then a connection state notification with the updated data is send to the application.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-749} Connection State Notification Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] new_state New state to change into. The state has a valid range from ::sraty_kConnectionMin <= value < ::sraty_kConnectionMax. If the value is
 * outside this range, a ::radef_kInternalError fatal error is thrown.
 * @param [in] disconnect_reason Disconnect reason in case of a change into the closed state. The reason has a valid range from ::sraty_kDiscReasonMin <= value
 * < ::sraty_kDiscReasonMax. If the value is outside this range, a ::radef_kInternalError fatal error is thrown.
 */
static void UpdateConnectionStateWithDiscReason(const uint32_t connection_id, const sraty_ConnectionStates new_state, const sraty_DiscReason disconnect_reason);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void srstm_Init(const uint32_t configured_connections) {
  // Input parameter check
  raas_AssertTrue(!srstm_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange(configured_connections, 1, RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS, radef_kInvalidParameter);

  srstm_number_of_connections = configured_connections;
  for (uint32_t index = 0U; index < RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS; ++index) {
    srstm_connection_states[index] = sraty_kConnectionClosed;
    srstm_connection_states_old[index] = sraty_kConnectionClosed;
  }
  srstm_initialized = true;
}

void srstm_ProcessConnectionStateMachine(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                         const bool confirmed_time_stamp_in_seq) {
  // Input parameter check
  raas_AssertTrue(srstm_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInvalidParameter);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)srtyp_kConnEventMin, (uint16_t)srtyp_kConnEventMax - 1U, radef_kInvalidParameter);

  // Save current state
  srstm_connection_states_old[connection_id] = srstm_connection_states[connection_id];

  switch (srstm_connection_states[connection_id]) {
    case sraty_kConnectionClosed:
      ProcessStateClosedEvents(connection_id, event);
      break;
    case sraty_kConnectionDown:
      ProcessStateDownEvents(connection_id, event);
      break;
    case sraty_kConnectionStart:
      ProcessStateStartEvents(connection_id, event, sequence_number_in_seq, confirmed_time_stamp_in_seq);
      break;
    case sraty_kConnectionUp:
      ProcessStateUpEvents(connection_id, event, sequence_number_in_seq, confirmed_time_stamp_in_seq);
      break;
    case sraty_kConnectionRetransRequest:
      ProcessStateRetransRequestEvents(connection_id, event, sequence_number_in_seq);
      break;
    case sraty_kConnectionRetransRunning:
      ProcessStateRetransRunningEvents(connection_id, event, sequence_number_in_seq, confirmed_time_stamp_in_seq);
      break;
    case sraty_kConnectionNotInitialized:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

sraty_ConnectionStates srstm_GetConnectionState(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srstm_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInvalidParameter);

  return srstm_connection_states[connection_id];
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static void ProcessStateClosedEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertTrue(srstm_connection_states[connection_id] == sraty_kConnectionClosed, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)srtyp_kConnEventMin, (uint16_t)srtyp_kConnEventMax - 1U, radef_kInternalError);

  switch (event) {
    case srtyp_kConnEventOpen:
      srcor_InitRaStaConnData(connection_id);  // [4] SN_T = random, Only Client: CS_T = 0 & CTS_R = Tlocal
      if (srcor_IsConnRoleServer(connection_id)) {
        UpdateConnectionState(connection_id, sraty_kConnectionDown);
      } else {
        srcor_SendConnReqMessage(connection_id);
        UpdateConnectionState(connection_id, sraty_kConnectionStart);
      }
      break;
    case srtyp_kConnEventClose:             // fall-through
    case srtyp_kConnEventSendData:          // fall-through
    case srtyp_kConnEventConnReqReceived:   // fall-through
    case srtyp_kConnEventConnRespReceived:  // fall-through
    case srtyp_kConnEventRetrReqReceived:   // fall-through
    case srtyp_kConnEventRetrRespReceived:  // fall-through
    case srtyp_kConnEventDiscReqReceived:   // fall-through
    case srtyp_kConnEventHbReceived:        // fall-through
    case srtyp_kConnEventDataReceived:      // fall-through
    case srtyp_kConnEventRetrDataReceived:  // fall-through
    case srtyp_kConnEventSendHb:            // fall-through
    case srtyp_kConnEventTimeout:
      // [2] Ignore event and remain in same state
      break;
    case srtyp_kConnEventNone:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

static void ProcessStateDownEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertTrue(srstm_connection_states[connection_id] == sraty_kConnectionDown, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)srtyp_kConnEventMin, (uint16_t)srtyp_kConnEventMax - 1U, radef_kInternalError);

  switch (event) {
    case srtyp_kConnEventOpen:   // fall-through
    case srtyp_kConnEventClose:  // fall-through
    case srtyp_kConnEventSendData:
      CloseRedundancyChannel(connection_id, false);
      break;
    case srtyp_kConnEventConnReqReceived:
      if (srcor_IsProtocolVersionAccepted(connection_id)) {
        if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
          srcor_SendConnRespMessage(connection_id);
          UpdateConnectionState(connection_id, sraty_kConnectionStart);
        }
      } else {
        CloseConnection(connection_id, sraty_kDiscReasonProtocolVersionError, true);  // [1] Close connection
      }
      break;
    case srtyp_kConnEventConnRespReceived:  // fall-through
    case srtyp_kConnEventRetrReqReceived:   // fall-through
    case srtyp_kConnEventRetrRespReceived:  // fall-through
    case srtyp_kConnEventDiscReqReceived:   // fall-through
    case srtyp_kConnEventHbReceived:        // fall-through
    case srtyp_kConnEventDataReceived:      // fall-through
    case srtyp_kConnEventRetrDataReceived:  // fall-through
    case srtyp_kConnEventSendHb:            // fall-through
    case srtyp_kConnEventTimeout:
      // [2] Ignore event and remain in same state
      break;
    case srtyp_kConnEventNone:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

static void ProcessStateStartEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                    const bool confirmed_time_stamp_in_seq) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertTrue(srstm_connection_states[connection_id] == sraty_kConnectionStart, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)srtyp_kConnEventMin, (uint16_t)srtyp_kConnEventMax - 1U, radef_kInternalError);

  switch (event) {
    case srtyp_kConnEventOpen:  // fall-through
    case srtyp_kConnEventSendData:
      CloseConnection(connection_id, sraty_kDiscReasonServiceNotAllowed, false);  // [1] Close connection
      break;
    case srtyp_kConnEventClose:
      CloseConnection(connection_id, sraty_kDiscReasonUserRequest, false);  // [1] Close connection
      break;
    case srtyp_kConnEventConnReqReceived:   // fall-through
    case srtyp_kConnEventRetrReqReceived:   // fall-through
    case srtyp_kConnEventRetrRespReceived:  // fall-through
    case srtyp_kConnEventDataReceived:      // fall-through
    case srtyp_kConnEventRetrDataReceived:
      CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
      break;
    case srtyp_kConnEventConnRespReceived:
      if (srcor_IsConnRoleServer(connection_id)) {
        CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
      } else {
        if (srcor_IsProtocolVersionAccepted(connection_id)) {
          if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
            srcor_SendHbMessage(connection_id);
            UpdateConnectionState(connection_id, sraty_kConnectionUp);
          }
        } else {
          CloseConnection(connection_id, sraty_kDiscReasonProtocolVersionError, true);  // [1] Close connection
        }
      }
      break;
    case srtyp_kConnEventDiscReqReceived:
      CloseRedundancyChannel(connection_id, true);
      break;
    case srtyp_kConnEventHbReceived:
      if (sequence_number_in_seq) {
        if (srcor_IsConnRoleServer(connection_id)) {
          if (confirmed_time_stamp_in_seq) {
            if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
              UpdateConnectionState(connection_id, sraty_kConnectionUp);
            }
          } else {
            CloseConnection(connection_id, sraty_kDiscReasonProtocolSequenceError, true);  // [1] Close connection
          }
        } else {
          CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
        }
      } else {
        if (srcor_IsConnRoleServer(connection_id)) {
          CloseConnection(connection_id, sraty_kDiscReasonSequenceNumberError, true);  // [1] Close connection
        } else {
          CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
        }
      }
      break;
    case srtyp_kConnEventSendHb:
      if (srcor_IsConnRoleServer(connection_id)) {
        srcor_SendHbMessage(connection_id);  // [4] Further event treatment
        // Remain in same state
      } else {
        // [2] Ignore event and remain in same state
      }
      break;
    case srtyp_kConnEventTimeout:
      CloseConnection(connection_id, sraty_kDiscReasonTimeout, true);  // [1] Close connection
      break;
    case srtyp_kConnEventNone:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

static void ProcessStateUpEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                 const bool confirmed_time_stamp_in_seq) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertTrue(srstm_connection_states[connection_id] == sraty_kConnectionUp, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)srtyp_kConnEventMin, (uint16_t)srtyp_kConnEventMax - 1U, radef_kInternalError);

  switch (event) {
    case srtyp_kConnEventOpen:
      CloseConnection(connection_id, sraty_kDiscReasonServiceNotAllowed, false);  // [1] Close connection
      break;
    case srtyp_kConnEventClose:
      CloseConnection(connection_id, sraty_kDiscReasonUserRequest, false);  // [1] Close connection
      break;
    case srtyp_kConnEventSendData:
      srcor_SendDataMessage(connection_id);  // [4] Further event treatment
      // Remain in same state
      break;
    case srtyp_kConnEventConnReqReceived:   // fall-through
    case srtyp_kConnEventConnRespReceived:  // fall-through
    case srtyp_kConnEventRetrRespReceived:  // fall-through
    case srtyp_kConnEventRetrDataReceived:
      CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
      break;
    case srtyp_kConnEventRetrReqReceived:
      if (srcor_IsRetrReqSequenceNumberAvailable(connection_id)) {
        if (sequence_number_in_seq) {
          if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
            srcor_HandleRetrReq(connection_id);
            // Remain in same state
          }
        } else {
          StartRetransmission(connection_id, sraty_kConnectionRetransRequest, true);  // [4] Further event treatment
        }
      } else {
        CloseConnection(connection_id, sraty_kDiscReasonRetransmissionFailed, true);  // [1] Close connection
        break;
      }
      break;
    case srtyp_kConnEventDiscReqReceived:
      CloseRedundancyChannel(connection_id, true);
      break;
    case srtyp_kConnEventHbReceived:  // fall-through
    case srtyp_kConnEventDataReceived:
      if (sequence_number_in_seq) {
        if (confirmed_time_stamp_in_seq) {
          if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
            // Remain in same state
          } else {
            // Nothing to do here
          }
        } else {
          CloseConnection(connection_id, sraty_kDiscReasonProtocolSequenceError, true);  // [1] Close connection
        }
      } else {
        srcor_ClearInputBufferMessagePendingFlag(connection_id);  // [4] Discard message
        srcor_SendRetrReqMessage(connection_id);
        UpdateConnectionState(connection_id, sraty_kConnectionRetransRequest);
      }
      break;
    case srtyp_kConnEventSendHb:
      srcor_SendHbMessage(connection_id);  // [4] Further event treatment
      // Remain in same state
      break;
    case srtyp_kConnEventTimeout:
      CloseConnection(connection_id, sraty_kDiscReasonTimeout, true);  // [1] Close connection
      break;
    case srtyp_kConnEventNone:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

static void ProcessStateRetransRequestEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertTrue(srstm_connection_states[connection_id] == sraty_kConnectionRetransRequest, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)srtyp_kConnEventMin, (uint16_t)srtyp_kConnEventMax - 1U, radef_kInternalError);

  switch (event) {
    case srtyp_kConnEventOpen:
      CloseConnection(connection_id, sraty_kDiscReasonServiceNotAllowed, false);  // [1] Close connection
      break;
    case srtyp_kConnEventClose:
      CloseConnection(connection_id, sraty_kDiscReasonUserRequest, false);  // [1] Close connection
      break;
    case srtyp_kConnEventSendData:
      srcor_SendDataMessage(connection_id);  // [4] Further event treatment
      // Remain in same state
      break;
    case srtyp_kConnEventConnReqReceived:  // fall-through
    case srtyp_kConnEventConnRespReceived:
      CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
      break;
    case srtyp_kConnEventRetrReqReceived:
      if (srcor_IsRetrReqSequenceNumberAvailable(connection_id)) {
        if (sequence_number_in_seq) {
          if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
            srcor_HandleRetrReq(connection_id);
            // Remain in same state
          }
        } else {
          StartRetransmission(connection_id, sraty_kConnectionRetransRequest, false);  // [4] Further event treatment
          // Remain in same state
        }
      } else {
        CloseConnection(connection_id, sraty_kDiscReasonRetransmissionFailed, true);  // [1] Close connection
      }
      break;
    case srtyp_kConnEventRetrRespReceived:
      if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
        UpdateConnectionState(connection_id, sraty_kConnectionRetransRunning);
      }
      break;
    case srtyp_kConnEventDiscReqReceived:
      CloseRedundancyChannel(connection_id, true);
      break;
    case srtyp_kConnEventHbReceived:    // fall-through
    case srtyp_kConnEventDataReceived:  // fall-through
    case srtyp_kConnEventRetrDataReceived:
      // [2] Ignore event and remain in same state
      break;
    case srtyp_kConnEventSendHb:
      srcor_SendHbMessage(connection_id);  // [4] Further event treatment
      // Remain in same state
      break;
    case srtyp_kConnEventTimeout:
      CloseConnection(connection_id, sraty_kDiscReasonTimeout, true);  // [1] Close connection
      break;
    case srtyp_kConnEventNone:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

static void ProcessStateRetransRunningEvents(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                             const bool confirmed_time_stamp_in_seq) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertTrue(srstm_connection_states[connection_id] == sraty_kConnectionRetransRunning, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)srtyp_kConnEventMin, (uint16_t)srtyp_kConnEventMax - 1U, radef_kInternalError);

  switch (event) {
    case srtyp_kConnEventOpen:
      CloseConnection(connection_id, sraty_kDiscReasonServiceNotAllowed, false);  // [1] Close connection
      break;
    case srtyp_kConnEventClose:
      CloseConnection(connection_id, sraty_kDiscReasonUserRequest, false);  // [1] Close connection
      break;
    case srtyp_kConnEventSendData:
      srcor_SendDataMessage(connection_id);  // [4] Further event treatment
      // Remain in same state
      break;
    case srtyp_kConnEventConnReqReceived:  // fall-through
    case srtyp_kConnEventConnRespReceived:
      CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
      break;
    case srtyp_kConnEventRetrReqReceived:
      if (sequence_number_in_seq) {
        CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
      } else {
        if (srcor_IsRetrReqSequenceNumberAvailable(connection_id)) {
          StartRetransmission(connection_id, sraty_kConnectionRetransRequest, true);  // [4] Further event treatment
        } else {
          CloseConnection(connection_id, sraty_kDiscReasonRetransmissionFailed, true);  // [1] Close connection
        }
      }
      break;
    case srtyp_kConnEventRetrRespReceived:
      CloseConnection(connection_id, sraty_kDiscReasonUnexpectedMessage, true);  // [1] Close connection
      break;
    case srtyp_kConnEventDiscReqReceived:
      CloseRedundancyChannel(connection_id, true);
      break;
    case srtyp_kConnEventHbReceived:    // fall-through
    case srtyp_kConnEventDataReceived:  // fall-through
    case srtyp_kConnEventRetrDataReceived:
      if (sequence_number_in_seq) {
        if (confirmed_time_stamp_in_seq) {
          if (ProcessReceivedMessage(connection_id)) {  // [3] Regular processing of the message received
            if (event != srtyp_kConnEventRetrDataReceived) {
              UpdateConnectionState(connection_id, sraty_kConnectionUp);
            } else {
              // Remain in same state
            }
          }
        } else {
          CloseConnection(connection_id, sraty_kDiscReasonProtocolSequenceError, true);  // [1] Close connection
        }
      } else {
        srcor_ClearInputBufferMessagePendingFlag(connection_id);  // [4] Discard message
        srcor_SendRetrReqMessage(connection_id);
        UpdateConnectionState(connection_id, sraty_kConnectionRetransRequest);
      }
      break;
    case srtyp_kConnEventSendHb:
      srcor_SendHbMessage(connection_id);  // [4] Further event treatment
      // Remain in same state
      break;
    case srtyp_kConnEventTimeout:
      CloseConnection(connection_id, sraty_kDiscReasonTimeout, true);  // [1] Close connection
      break;
    case srtyp_kConnEventNone:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

static bool ProcessReceivedMessage(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);

  const bool kTimelinessRespected = srcor_ProcessReceivedMessage(connection_id);  // [3] Regular processing of the message received
  if (!kTimelinessRespected) {
    CloseConnection(connection_id, sraty_kDiscReasonTimeout, true);  // [1] Close connection
  }

  return kTimelinessRespected;
}

static void CloseConnection(const uint32_t connection_id, const sraty_DiscReason disconnect_reason, const bool is_incoming_message) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)disconnect_reason, (uint16_t)sraty_kDiscReasonMin, (uint16_t)sraty_kDiscReasonMax - 1U, radef_kInternalError);

  if (is_incoming_message) {
    srcor_UpdateConfirmedTxSequenceNumber(connection_id);  // [1] Set CS_T = SN_PDU for incoming messages
  }

  srcor_SendDiscReqMessage(connection_id, disconnect_reason);  // [1] Close connection
  UpdateConnectionStateWithDiscReason(connection_id, sraty_kConnectionClosed, disconnect_reason);
}

static void CloseRedundancyChannel(const uint32_t connection_id, const bool is_incoming_message) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);

  if (is_incoming_message) {
    srcor_UpdateConfirmedTxSequenceNumber(connection_id);  // [1] Set CS_T = SN_PDU for incoming messages
  }

  srcor_CloseRedundancyChannel(connection_id);  // [1] Close connection/redundancy channel

  UpdateConnectionState(connection_id, sraty_kConnectionClosed);
}

static void StartRetransmission(const uint32_t connection_id, const sraty_ConnectionStates new_state, bool retransmission_requested) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)new_state, (uint16_t)sraty_kConnectionMin, (uint16_t)sraty_kConnectionMax - 1U, radef_kInternalError);

  srcor_UpdateConfirmedRxSequenceNumber(connection_id);  // [4] Further event treatment (CR_R = CS_PDU)
  srcor_HandleRetrReq(connection_id);
  if (retransmission_requested) {
    srcor_SendRetrReqMessage(connection_id);
  }
  UpdateConnectionState(connection_id, new_state);
}

static void UpdateConnectionState(const uint32_t connection_id, const sraty_ConnectionStates new_state) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)new_state, (uint16_t)sraty_kConnectionMin, (uint16_t)sraty_kConnectionMax - 1U, radef_kInternalError);

  UpdateConnectionStateWithDiscReason(connection_id, new_state, sraty_kDiscReasonNotInUse);
}

static void UpdateConnectionStateWithDiscReason(const uint32_t connection_id, const sraty_ConnectionStates new_state,
                                                const sraty_DiscReason disconnect_reason) {
  // Input parameter check
  raas_AssertTrue(connection_id < srstm_number_of_connections, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)new_state, (uint16_t)sraty_kConnectionMin, (uint16_t)sraty_kConnectionMax - 1U, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)disconnect_reason, (uint16_t)sraty_kDiscReasonMin, (uint16_t)sraty_kDiscReasonMax - 1U, radef_kInternalError);

  if (new_state != srstm_connection_states_old[connection_id]) {
    srstm_connection_states[connection_id] = new_state;
    srcor_SendConnectionStateNotification(connection_id, srstm_connection_states[connection_id], disconnect_reason);
  }
}
