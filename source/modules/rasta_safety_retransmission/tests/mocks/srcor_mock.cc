/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcor_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srcor module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srcor_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srcorMock * srcorMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srcorMock::srcorMock() {
  instance = this;
}

srcorMock::~srcorMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  bool srcor_IsConfigurationValid(const srcty_SafetyRetransmissionConfiguration * sr_layer_configuration){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_IsConfigurationValid(sr_layer_configuration);
  }
  void srcor_Init(const srcty_SafetyRetransmissionConfiguration * sr_layer_configuration){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_Init(sr_layer_configuration);
  }
  radef_RaStaReturnCode srcor_GetConnectionId(const uint32_t sender_id, const uint32_t receiver_id, uint32_t * connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_GetConnectionId(sender_id, receiver_id, connection_id);
  }
  void srcor_InitRaStaConnData(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_InitRaStaConnData(connection_id);
  }
  void srcor_CloseRedundancyChannel(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_CloseRedundancyChannel(connection_id);
  }
  void srcor_ReceiveMessage(const uint32_t connection_id, srtyp_ConnectionEvents * connection_event, bool * sequence_number_in_seq, bool * confirmed_time_stamp_in_seq){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_ReceiveMessage(connection_id, connection_event, sequence_number_in_seq, confirmed_time_stamp_in_seq);
  }
  bool srcor_ProcessReceivedMessage(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_ProcessReceivedMessage(connection_id);
  }
  void srcor_UpdateConfirmedTxSequenceNumber(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_UpdateConfirmedTxSequenceNumber(connection_id);
  }
  void srcor_UpdateConfirmedRxSequenceNumber(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_UpdateConfirmedRxSequenceNumber(connection_id);
  }
  bool srcor_IsProtocolVersionAccepted(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_IsProtocolVersionAccepted(connection_id);
  }
  void srcor_SetReceivedMessagePendingFlag(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SetReceivedMessagePendingFlag(connection_id);
  }
  bool srcor_GetReceivedMessagePendingFlag(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_GetReceivedMessagePendingFlag(connection_id);
  }
  void srcor_WriteMessagePayloadToTemporaryBuffer(const uint32_t connection_id, const uint16_t message_payload_size, const uint8_t * message_payload){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_WriteMessagePayloadToTemporaryBuffer(connection_id, message_payload_size, message_payload);
  }
  void srcor_ClearInputBufferMessagePendingFlag(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_ClearInputBufferMessagePendingFlag(connection_id);
  }
  void srcor_SendDataMessage(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendDataMessage(connection_id);
  }
  void srcor_SendConnReqMessage(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendConnReqMessage(connection_id);
  }
  void srcor_SendConnRespMessage(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendConnRespMessage(connection_id);
  }
  void srcor_SendDiscReqMessage(const uint32_t connection_id, const sraty_DiscReason disconnect_reason){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendDiscReqMessage(connection_id, disconnect_reason);
  }
  void srcor_SetDiscDetailedReason(const uint32_t connection_id, const uint16_t detailed_disconnect_reason){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SetDiscDetailedReason(connection_id, detailed_disconnect_reason);
  }
  void srcor_SendHbMessage(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendHbMessage(connection_id);
  }
  void srcor_SendRetrReqMessage(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendRetrReqMessage(connection_id);
  }
  void srcor_HandleRetrReq(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_HandleRetrReq(connection_id);
  }
  bool srcor_IsRetrReqSequenceNumberAvailable(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_IsRetrReqSequenceNumberAvailable(connection_id);
  }
  bool srcor_IsConnRoleServer(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_IsConnRoleServer(connection_id);
  }
  bool srcor_IsMessageTimeout(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_IsMessageTimeout(connection_id);
  }
  bool srcor_IsHeartbeatInterval(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_IsHeartbeatInterval(connection_id);
  }
  bool srcor_IsReceivedMsgPendingAndBuffersNotFull(const uint32_t connection_id){
    if(srcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srcorMock::getInstance()->srcor_IsReceivedMsgPendingAndBuffersNotFull(connection_id);
  }
  void srcor_SendPendingMessages(const uint32_t connection_id){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendPendingMessages(connection_id);
  }
  void srcor_SendConnectionStateNotification(const uint32_t connection_id, const sraty_ConnectionStates connection_state, const sraty_DiscReason disconnect_reason){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_SendConnectionStateNotification(connection_id, connection_state, disconnect_reason);
  }
  void srcor_GetBufferSizeAndUtilisation(const uint32_t connection_id, sraty_BufferUtilisation * buffer_utilisation, uint16_t * opposite_buffer_size){
    ASSERT_NE(srcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    srcorMock::getInstance()->srcor_GetBufferSizeAndUtilisation(connection_id, buffer_utilisation, opposite_buffer_size);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
