/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srsend_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srsend module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srsend_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srsendMock * srsendMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srsendMock::srsendMock() {
  instance = this;
}

srsendMock::~srsendMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void srsend_Init(const uint32_t configured_connections){
    ASSERT_NE(srsendMock::getInstance(), nullptr) << "Mock object not initialized!";
    srsendMock::getInstance()->srsend_Init(configured_connections);
  }
  void srsend_InitBuffer(const uint32_t connection_id){
    ASSERT_NE(srsendMock::getInstance(), nullptr) << "Mock object not initialized!";
    srsendMock::getInstance()->srsend_InitBuffer(connection_id);
  }
  void srsend_AddToBuffer(const uint32_t connection_id, const srtyp_SrMessage * message){
    ASSERT_NE(srsendMock::getInstance(), nullptr) << "Mock object not initialized!";
    srsendMock::getInstance()->srsend_AddToBuffer(connection_id, message);
  }
  radef_RaStaReturnCode srsend_ReadMessageToSend(const uint32_t connection_id, srtyp_SrMessage * message){
    if(srsendMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srsendMock::getInstance()->srsend_ReadMessageToSend(connection_id, message);
  }
  void srsend_PrepareBufferForRetr(const uint32_t connection_id, const uint32_t sequence_number_for_retransmission, const srtyp_SrMessageHeaderCreate message_header, uint32_t * new_current_sequence_number){
    ASSERT_NE(srsendMock::getInstance(), nullptr) << "Mock object not initialized!";
    srsendMock::getInstance()->srsend_PrepareBufferForRetr(connection_id, sequence_number_for_retransmission, message_header, new_current_sequence_number);
  }
  radef_RaStaReturnCode srsend_IsSequenceNumberInBuffer(const uint32_t connection_id, const uint32_t sequence_number){
    if(srsendMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srsendMock::getInstance()->srsend_IsSequenceNumberInBuffer(connection_id, sequence_number);
  }
  void srsend_RemoveFromBuffer(const uint32_t connection_id, const uint32_t confirmed_sequence_number){
    ASSERT_NE(srsendMock::getInstance(), nullptr) << "Mock object not initialized!";
    srsendMock::getInstance()->srsend_RemoveFromBuffer(connection_id, confirmed_sequence_number);
  }
  uint16_t srsend_GetFreeBufferEntries(const uint32_t connection_id){
    if(srsendMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srsendMock::getInstance()->srsend_GetFreeBufferEntries(connection_id);
  }
  uint16_t srsend_GetUsedBufferEntries(const uint32_t connection_id){
    if(srsendMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srsendMock::getInstance()->srsend_GetUsedBufferEntries(connection_id);
  }
  uint16_t srsend_GetNumberOfMessagesToSend(const uint32_t connection_id){
    if(srsendMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srsendMock::getInstance()->srsend_GetNumberOfMessagesToSend(connection_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
