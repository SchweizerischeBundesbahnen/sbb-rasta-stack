/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rednot_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the rednot module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rednot_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
rednotMock * rednotMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
rednotMock::rednotMock() {
  instance = this;
}

rednotMock::~rednotMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void rednot_MessageReceivedNotification(const uint32_t red_channel_id){
    ASSERT_NE(rednotMock::getInstance(), nullptr) << "Mock object not initialized!";
    rednotMock::getInstance()->rednot_MessageReceivedNotification(red_channel_id);
  }
  void rednot_DiagnosticNotification(const uint32_t red_channel_id, const uint32_t tr_channel_id, const radef_TransportChannelDiagnosticData TransportChannelDiagnosticData){
    ASSERT_NE(rednotMock::getInstance(), nullptr) << "Mock object not initialized!";
    rednotMock::getInstance()->rednot_DiagnosticNotification(red_channel_id, tr_channel_id, TransportChannelDiagnosticData);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
