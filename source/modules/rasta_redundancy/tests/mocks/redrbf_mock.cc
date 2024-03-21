/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redrbf_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the redrbf module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redrbf_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
redrbfMock * redrbfMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
redrbfMock::redrbfMock() {
  instance = this;
}

redrbfMock::~redrbfMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void redrbf_Init(const uint32_t configured_red_channels){
    ASSERT_NE(redrbfMock::getInstance(), nullptr) << "Mock object not initialized!";
    redrbfMock::getInstance()->redrbf_Init(configured_red_channels);
  }
  void redrbf_InitBuffer(const uint32_t red_channel_id){
    ASSERT_NE(redrbfMock::getInstance(), nullptr) << "Mock object not initialized!";
    redrbfMock::getInstance()->redrbf_InitBuffer(red_channel_id);
  }
  void redrbf_AddToBuffer(const uint32_t red_channel_id, const redtyp_RedundancyMessagePayload * message_payload){
    ASSERT_NE(redrbfMock::getInstance(), nullptr) << "Mock object not initialized!";
    redrbfMock::getInstance()->redrbf_AddToBuffer(red_channel_id, message_payload);
  }
  radef_RaStaReturnCode redrbf_ReadFromBuffer(const uint32_t red_channel_id, const uint16_t buffer_size, uint16_t * message_size, uint8_t * message_buffer){
    if(redrbfMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redrbfMock::getInstance()->redrbf_ReadFromBuffer(red_channel_id, buffer_size, message_size, message_buffer);
  }
  uint16_t redrbf_GetFreeBufferEntries(const uint32_t red_channel_id){
    if(redrbfMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redrbfMock::getInstance()->redrbf_GetFreeBufferEntries(red_channel_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
