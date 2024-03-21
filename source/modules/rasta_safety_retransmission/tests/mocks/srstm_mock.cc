/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srstm_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srstm module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srstm_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srstmMock * srstmMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srstmMock::srstmMock() {
  instance = this;
}

srstmMock::~srstmMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void srstm_Init(const uint32_t configured_connections){
    ASSERT_NE(srstmMock::getInstance(), nullptr) << "Mock object not initialized!";
    srstmMock::getInstance()->srstm_Init(configured_connections);
  }
  void srstm_ProcessConnectionStateMachine(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq, const bool confirmed_time_stamp_in_seq){
    ASSERT_NE(srstmMock::getInstance(), nullptr) << "Mock object not initialized!";
    srstmMock::getInstance()->srstm_ProcessConnectionStateMachine(connection_id, event, sequence_number_in_seq, confirmed_time_stamp_in_seq);
  }
  sraty_ConnectionStates srstm_GetConnectionState(const uint32_t connection_id){
    if(srstmMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srstmMock::getInstance()->srstm_GetConnectionState(connection_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
