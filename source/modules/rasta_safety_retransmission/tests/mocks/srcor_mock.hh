/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcor_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srcor module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRCOR_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRCOR_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta srcor class
 */
class srcorMock {
private:
  static srcorMock *instance;  ///< srcorMock instance

public:
  /**
  * @brief srcorMock constructor
  */
  srcorMock();
  /**
  * @brief srcorMock destructor
  */
  virtual ~srcorMock();

  /**
  * @brief Mock Method object for the srcor_IsConfigurationValid function
  */
  MOCK_METHOD(bool, srcor_IsConfigurationValid, (const srcty_SafetyRetransmissionConfiguration * sr_layer_configuration));

  /**
  * @brief Mock Method object for the srcor_Init function
  */
  MOCK_METHOD(void, srcor_Init, (const srcty_SafetyRetransmissionConfiguration * sr_layer_configuration));

  /**
  * @brief Mock Method object for the srcor_GetConnectionId function
  */
  MOCK_METHOD(radef_RaStaReturnCode, srcor_GetConnectionId, (const uint32_t sender_id, const uint32_t receiver_id, uint32_t * connection_id));

  /**
  * @brief Mock Method object for the srcor_InitRaStaConnData function
  */
  MOCK_METHOD(void, srcor_InitRaStaConnData, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_CloseRedundancyChannel function
  */
  MOCK_METHOD(void, srcor_CloseRedundancyChannel, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_ReceiveMessage function
  */
  MOCK_METHOD(void, srcor_ReceiveMessage, (const uint32_t connection_id, srtyp_ConnectionEvents * connection_event, bool * sequence_number_in_seq, bool * confirmed_time_stamp_in_seq));

  /**
  * @brief Mock Method object for the srcor_ProcessReceivedMessage function
  */
  MOCK_METHOD(bool, srcor_ProcessReceivedMessage, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_UpdateConfirmedTxSequenceNumber function
  */
  MOCK_METHOD(void, srcor_UpdateConfirmedTxSequenceNumber, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_UpdateConfirmedRxSequenceNumber function
  */
  MOCK_METHOD(void, srcor_UpdateConfirmedRxSequenceNumber, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_IsProtocolVersionAccepted function
  */
  MOCK_METHOD(bool, srcor_IsProtocolVersionAccepted, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SetReceivedMessagePendingFlag function
  */
  MOCK_METHOD(void, srcor_SetReceivedMessagePendingFlag, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_GetReceivedMessagePendingFlag function
  */
  MOCK_METHOD(bool, srcor_GetReceivedMessagePendingFlag, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_WriteMessagePayloadToTemporaryBuffer function
  */
  MOCK_METHOD(void, srcor_WriteMessagePayloadToTemporaryBuffer, (const uint32_t connection_id, const uint16_t message_payload_size, const uint8_t * message_payload));

  /**
  * @brief Mock Method object for the srcor_ClearInputBufferMessagePendingFlag function
  */
  MOCK_METHOD(void, srcor_ClearInputBufferMessagePendingFlag, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SendDataMessage function
  */
  MOCK_METHOD(void, srcor_SendDataMessage, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SendConnReqMessage function
  */
  MOCK_METHOD(void, srcor_SendConnReqMessage, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SendConnRespMessage function
  */
  MOCK_METHOD(void, srcor_SendConnRespMessage, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SendDiscReqMessage function
  */
  MOCK_METHOD(void, srcor_SendDiscReqMessage, (const uint32_t connection_id, const sraty_DiscReason disconnect_reason));

  /**
  * @brief Mock Method object for the srcor_SetDiscDetailedReason function
  */
  MOCK_METHOD(void, srcor_SetDiscDetailedReason, (const uint32_t connection_id, const uint16_t detailed_disconnect_reason));

  /**
  * @brief Mock Method object for the srcor_SendHbMessage function
  */
  MOCK_METHOD(void, srcor_SendHbMessage, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SendRetrReqMessage function
  */
  MOCK_METHOD(void, srcor_SendRetrReqMessage, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_HandleRetrReq function
  */
  MOCK_METHOD(void, srcor_HandleRetrReq, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_IsRetrReqSequenceNumberAvailable function
  */
  MOCK_METHOD(bool, srcor_IsRetrReqSequenceNumberAvailable, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_IsConnRoleServer function
  */
  MOCK_METHOD(bool, srcor_IsConnRoleServer, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_IsMessageTimeout function
  */
  MOCK_METHOD(bool, srcor_IsMessageTimeout, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_IsHeartbeatInterval function
  */
  MOCK_METHOD(bool, srcor_IsHeartbeatInterval, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_IsReceivedMsgPendingAndBuffersNotFull function
  */
  MOCK_METHOD(bool, srcor_IsReceivedMsgPendingAndBuffersNotFull, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SendPendingMessages function
  */
  MOCK_METHOD(void, srcor_SendPendingMessages, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srcor_SendConnectionStateNotification function
  */
  MOCK_METHOD(void, srcor_SendConnectionStateNotification, (const uint32_t connection_id, const sraty_ConnectionStates connection_state, const sraty_DiscReason disconnect_reason));

  /**
  * @brief Mock Method object for the srcor_GetBufferSizeAndUtilisation function
  */
  MOCK_METHOD(void, srcor_GetBufferSizeAndUtilisation, (const uint32_t connection_id, sraty_BufferUtilisation * buffer_utilisation, uint16_t * opposite_buffer_size));

  /**
  * @brief Get the Instance object
  *
  * @return srcorMock*
  */
  static srcorMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRCOR_MOCK_HH_
