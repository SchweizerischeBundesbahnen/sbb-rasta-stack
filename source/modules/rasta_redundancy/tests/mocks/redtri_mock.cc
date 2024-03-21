/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redtri_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the redtri module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redtri_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
redtriMock * redtriMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
redtriMock::redtriMock() {
  instance = this;
}

redtriMock::~redtriMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void redtri_Init(void){
    ASSERT_NE(redtriMock::getInstance(), nullptr) << "Mock object not initialized!";
    redtriMock::getInstance()->redtri_Init();
  }
  void redtri_SendMessage(const uint32_t transport_channel_id, const uint16_t message_size, const uint8_t * message_data){
    ASSERT_NE(redtriMock::getInstance(), nullptr) << "Mock object not initialized!";
    redtriMock::getInstance()->redtri_SendMessage(transport_channel_id, message_size, message_data);
  }
  radef_RaStaReturnCode redtri_ReadMessage(const uint32_t transport_channel_id, const uint16_t buffer_size, uint16_t * message_size, uint8_t * message_buffer){
    if(redtriMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redtriMock::getInstance()->redtri_ReadMessage(transport_channel_id, buffer_size, message_size, message_buffer);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
