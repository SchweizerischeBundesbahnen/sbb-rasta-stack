/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redstm_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the redstm module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redstm_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
redstmMock * redstmMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
redstmMock::redstmMock() {
  instance = this;
}

redstmMock::~redstmMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void redstm_Init(const uint32_t configured_red_channels){
    ASSERT_NE(redstmMock::getInstance(), nullptr) << "Mock object not initialized!";
    redstmMock::getInstance()->redstm_Init(configured_red_channels);
  }
  void redstm_ProcessChannelStateMachine(const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event){
    ASSERT_NE(redstmMock::getInstance(), nullptr) << "Mock object not initialized!";
    redstmMock::getInstance()->redstm_ProcessChannelStateMachine(red_channel_id, event);
  }
  redstm_RedundancyChannelStates redstm_GetChannelState(const uint32_t red_channel_id){
    if(redstmMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redstmMock::getInstance()->redstm_GetChannelState(red_channel_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
