/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmsg_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srmsg module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRMSG_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRMSG_MOCK_HH_

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
 * @brief Mock class for the rasta srmsg class
 */
class srmsgMock {
private:
  static srmsgMock *instance;  ///< srmsgMock instance

public:
  /**
  * @brief srmsgMock constructor
  */
  srmsgMock();
  /**
  * @brief srmsgMock destructor
  */
  virtual ~srmsgMock();

  /**
  * @brief Mock Method object for the srmsg_Init function
  */
  MOCK_METHOD(void, srmsg_Init, (const srcty_SafetyCodeType configured_safety_code_type, const srcty_Md4InitValue configured_md4_initial_value));

  /**
  * @brief Mock Method object for the srmsg_CreateConnReqMessage function
  */
  MOCK_METHOD(void, srmsg_CreateConnReqMessage, (const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CreateConnRespMessage function
  */
  MOCK_METHOD(void, srmsg_CreateConnRespMessage, (const srtyp_SrMessageHeaderCreate message_header, const srtyp_ProtocolVersion protocol_version, const uint16_t n_send_max, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CreateDataMessage function
  */
  MOCK_METHOD(void, srmsg_CreateDataMessage, (const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload * message_payload, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CreateRetrDataMessage function
  */
  MOCK_METHOD(void, srmsg_CreateRetrDataMessage, (const srtyp_SrMessageHeaderCreate message_header, const srtyp_SrMessagePayload * message_payload, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CreateRetrReqMessage function
  */
  MOCK_METHOD(void, srmsg_CreateRetrReqMessage, (const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CreateRetrRespMessage function
  */
  MOCK_METHOD(void, srmsg_CreateRetrRespMessage, (const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CreateHeartbeatMessage function
  */
  MOCK_METHOD(void, srmsg_CreateHeartbeatMessage, (const srtyp_SrMessageHeaderCreate message_header, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CreateDiscReqMessage function
  */
  MOCK_METHOD(void, srmsg_CreateDiscReqMessage, (const srtyp_SrMessageHeaderCreate message_header, const uint16_t detailed_reason, const sraty_DiscReason reason, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_UpdateMessageHeader function
  */
  MOCK_METHOD(void, srmsg_UpdateMessageHeader, (const srtyp_SrMessageHeaderUpdate message_header_update, srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_CheckMessage function
  */
  MOCK_METHOD(radef_RaStaReturnCode, srmsg_CheckMessage, (const srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_GetMessageHeader function
  */
  MOCK_METHOD(void, srmsg_GetMessageHeader, (const srtyp_SrMessage * sr_message, srtyp_SrMessageHeader * message_header));

  /**
  * @brief Mock Method object for the srmsg_GetMessageType function
  */
  MOCK_METHOD(srtyp_SrMessageType, srmsg_GetMessageType, (const srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_GetMessageSequenceNumber function
  */
  MOCK_METHOD(uint32_t, srmsg_GetMessageSequenceNumber, (const srtyp_SrMessage * sr_message));

  /**
  * @brief Mock Method object for the srmsg_GetDataMessagePayload function
  */
  MOCK_METHOD(void, srmsg_GetDataMessagePayload, (const srtyp_SrMessage * sr_message, srtyp_SrMessagePayload * message_payload));

  /**
  * @brief Mock Method object for the srmsg_GetConnMessageData function
  */
  MOCK_METHOD(void, srmsg_GetConnMessageData, (const srtyp_SrMessage * sr_message, srtyp_ProtocolVersion * protocol_version, uint16_t * n_send_max));

  /**
  * @brief Mock Method object for the srmsg_GetDiscMessageData function
  */
  MOCK_METHOD(void, srmsg_GetDiscMessageData, (const srtyp_SrMessage * sr_message, uint16_t * detailed_reason, sraty_DiscReason * reason));

  /**
  * @brief Get the Instance object
  *
  * @return srmsgMock*
  */
  static srmsgMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRMSG_MOCK_HH_
