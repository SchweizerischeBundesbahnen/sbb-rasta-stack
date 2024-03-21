/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file sradin_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the sradin module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "sradin_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
sradinMock * sradinMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
sradinMock::sradinMock() {
  instance = this;
}

sradinMock::~sradinMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void sradin_Init(void){
    ASSERT_NE(sradinMock::getInstance(), nullptr) << "Mock object not initialized!";
    sradinMock::getInstance()->sradin_Init();
  }
  void sradin_OpenRedundancyChannel(const uint32_t redundancy_channel_id){
    ASSERT_NE(sradinMock::getInstance(), nullptr) << "Mock object not initialized!";
    sradinMock::getInstance()->sradin_OpenRedundancyChannel(redundancy_channel_id);
  }
  void sradin_CloseRedundancyChannel(const uint32_t redundancy_channel_id){
    ASSERT_NE(sradinMock::getInstance(), nullptr) << "Mock object not initialized!";
    sradinMock::getInstance()->sradin_CloseRedundancyChannel(redundancy_channel_id);
  }
  void sradin_SendMessage(const uint32_t redundancy_channel_id, const uint16_t message_size, const uint8_t * message_data){
    ASSERT_NE(sradinMock::getInstance(), nullptr) << "Mock object not initialized!";
    sradinMock::getInstance()->sradin_SendMessage(redundancy_channel_id, message_size, message_data);
  }
  radef_RaStaReturnCode sradin_ReadMessage(const uint32_t redundancy_channel_id, const uint16_t buffer_size, uint16_t * message_size, uint8_t * message_buffer){
    if(sradinMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return sradinMock::getInstance()->sradin_ReadMessage(redundancy_channel_id, buffer_size, message_size, message_buffer);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
