/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redmsg_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the redmsg module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redmsg_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
redmsgMock * redmsgMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
redmsgMock::redmsgMock() {
  instance = this;
}

redmsgMock::~redmsgMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void redmsg_Init(const redcty_CheckCodeType configured_check_code_type){
    ASSERT_NE(redmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    redmsgMock::getInstance()->redmsg_Init(configured_check_code_type);
  }
  void redmsg_CreateMessage(const uint32_t sequence_number, const redtyp_RedundancyMessagePayload * message_payload, redtyp_RedundancyMessage * redundancy_message){
    ASSERT_NE(redmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    redmsgMock::getInstance()->redmsg_CreateMessage(sequence_number, message_payload, redundancy_message);
  }
  radef_RaStaReturnCode redmsg_CheckMessageCrc(const redtyp_RedundancyMessage * redundancy_message){
    if(redmsgMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redmsgMock::getInstance()->redmsg_CheckMessageCrc(redundancy_message);
  }
  uint32_t redmsg_GetMessageSequenceNumber(const redtyp_RedundancyMessage * redundancy_message){
    if(redmsgMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redmsgMock::getInstance()->redmsg_GetMessageSequenceNumber(redundancy_message);
  }
  void redmsg_GetMessagePayload(const redtyp_RedundancyMessage * redundancy_message, redtyp_RedundancyMessagePayload * message_payload){
    ASSERT_NE(redmsgMock::getInstance(), nullptr) << "Mock object not initialized!";
    redmsgMock::getInstance()->redmsg_GetMessagePayload(redundancy_message, message_payload);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
