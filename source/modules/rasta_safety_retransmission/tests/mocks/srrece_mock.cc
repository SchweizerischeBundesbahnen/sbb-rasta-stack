/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srrece_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srrece module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srrece_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srreceMock * srreceMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srreceMock::srreceMock() {
  instance = this;
}

srreceMock::~srreceMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void srrece_Init(const uint32_t configured_connections, const uint16_t configured_n_send_max){
    ASSERT_NE(srreceMock::getInstance(), nullptr) << "Mock object not initialized!";
    srreceMock::getInstance()->srrece_Init(configured_connections, configured_n_send_max);
  }
  void srrece_InitBuffer(const uint32_t connection_id){
    ASSERT_NE(srreceMock::getInstance(), nullptr) << "Mock object not initialized!";
    srreceMock::getInstance()->srrece_InitBuffer(connection_id);
  }
  void srrece_AddToBuffer(const uint32_t connection_id, const srtyp_SrMessagePayload * message_payload){
    ASSERT_NE(srreceMock::getInstance(), nullptr) << "Mock object not initialized!";
    srreceMock::getInstance()->srrece_AddToBuffer(connection_id, message_payload);
  }
  radef_RaStaReturnCode srrece_ReadFromBuffer(const uint32_t connection_id, srtyp_SrMessagePayload * message_payload){
    if(srreceMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srreceMock::getInstance()->srrece_ReadFromBuffer(connection_id, message_payload);
  }
  uint32_t srrece_GetPayloadSizeOfNextMessageToRead(const uint32_t connection_id){
    if(srreceMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srreceMock::getInstance()->srrece_GetPayloadSizeOfNextMessageToRead(connection_id);
  }
  uint16_t srrece_GetFreeBufferEntries(const uint32_t connection_id){
    if(srreceMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srreceMock::getInstance()->srrece_GetFreeBufferEntries(connection_id);
  }
  uint16_t srrece_GetUsedBufferEntries(const uint32_t connection_id){
    if(srreceMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srreceMock::getInstance()->srrece_GetUsedBufferEntries(connection_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
