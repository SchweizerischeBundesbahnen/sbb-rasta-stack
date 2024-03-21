/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmsg_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srmsg module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srmsg_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srmsgMock * srmsgMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srmsgMock::srmsgMock() {
  instance = this;
}

srmsgMock::~srmsgMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void srmsg_Init(const srcty_SafetyCodeType configured_safety_code_type, const srcty_Md4InitValue configured_md4_initial_value){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_Init(configured_safety_code_type, configured_md4_initial_value);
  }
  void srmsg_CreateConnReqMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateConnReqMessage(message_header, protocol_version, n_send_max, sr_message);
  }
  void srmsg_CreateConnRespMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateConnRespMessage(message_header, protocol_version, n_send_max, sr_message);
  }
  void srmsg_CreateDataMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload * message_payload, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateDataMessage(message_header, message_payload, sr_message);
  }
  void srmsg_CreateRetrDataMessage(const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload * message_payload, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateRetrDataMessage(message_header, message_payload, sr_message);
  }
  void srmsg_CreateRetrReqMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateRetrReqMessage(message_header, sr_message);
  }
  void srmsg_CreateRetrRespMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateRetrRespMessage(message_header, sr_message);
  }
  void srmsg_CreateHeartbeatMessage(const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateHeartbeatMessage(message_header, sr_message);
  }
  void srmsg_CreateDiscReqMessage(const srtyp_SrMessageHeaderCreate message_header, const uint16_t detailed_reason, const sraty_DiscReason reason, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_CreateDiscReqMessage(message_header, detailed_reason, reason, sr_message);
  }
  void srmsg_UpdateMessageHeader(const srtyp_SrMessageHeaderUpdate message_header_update, srtyp_SrMessage * sr_message){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_UpdateMessageHeader(message_header_update, sr_message);
  }
  radef_RaStaReturnCode srmsg_CheckMessage(const srtyp_SrMessage * sr_message){
    if(srmsgMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srmsgMock::getInstance()->srmsg_CheckMessage(sr_message);
  }
  void srmsg_GetMessageHeader(const srtyp_SrMessage * sr_message, srtyp_SrMessageHeader * message_header){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_GetMessageHeader(sr_message, message_header);
  }
  srtyp_SrMessageType srmsg_GetMessageType(const srtyp_SrMessage * sr_message){
    if(srmsgMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srmsgMock::getInstance()->srmsg_GetMessageType(sr_message);
  }
  uint32_t srmsg_GetMessageSequenceNumber(const srtyp_SrMessage * sr_message){
    if(srmsgMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srmsgMock::getInstance()->srmsg_GetMessageSequenceNumber(sr_message);
  }
  void srmsg_GetDataMessagePayload(const srtyp_SrMessage * sr_message, srtyp_SrMessagePayload * message_payload){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_GetDataMessagePayload(sr_message, message_payload);
  }
  void srmsg_GetConnMessageData(const srtyp_SrMessage * sr_message, srtyp_ProtocolVersion * protocol_version, uint16_t * n_send_max){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_GetConnMessageData(sr_message, protocol_version, n_send_max);
  }
  void srmsg_GetDiscMessageData(const srtyp_SrMessage * sr_message, uint16_t * detailed_reason, sraty_DiscReason * reason){
    ASSERT_NE(srmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    srmsgMock::getInstance()->srmsg_GetDiscMessageData(sr_message, detailed_reason, reason);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
