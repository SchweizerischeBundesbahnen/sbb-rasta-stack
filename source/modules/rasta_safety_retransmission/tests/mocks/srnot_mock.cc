/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srnot_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srnot module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srnot_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srnotMock * srnotMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srnotMock::srnotMock() {
  instance = this;
}

srnotMock::~srnotMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void srnot_MessageReceivedNotification(const uint32_t connection_id){
    ASSERT_NE(srnotMock::getInstance(), nullptr) << "Mock object not initialized!";
    srnotMock::getInstance()->srnot_MessageReceivedNotification(connection_id);
  }
  void srnot_ConnectionStateNotification(const uint32_t connection_id, const sraty_ConnectionStates connection_state, const sraty_BufferUtilisation buffer_utilisation, const uint16_t opposite_buffer_size, const sraty_DiscReason disconnect_reason, const uint16_t detailed_disconnect_reason){
    ASSERT_NE(srnotMock::getInstance(), nullptr) << "Mock object not initialized!";
    srnotMock::getInstance()->srnot_ConnectionStateNotification(connection_id, connection_state, buffer_utilisation, opposite_buffer_size, disconnect_reason, detailed_disconnect_reason);
  }
  void srnot_SrDiagnosticNotification(const uint32_t connection_id, const sraty_ConnectionDiagnosticData connection_diagnostic_data){
    ASSERT_NE(srnotMock::getInstance(), nullptr) << "Mock object not initialized!";
    srnotMock::getInstance()->srnot_SrDiagnosticNotification(connection_id, connection_diagnostic_data);
  }
  void srnot_RedDiagnosticNotification(const uint32_t connection_id, const sraty_RedundancyChannelDiagnosticData redundancy_channel_diagnostic_data){
    ASSERT_NE(srnotMock::getInstance(), nullptr) << "Mock object not initialized!";
    srnotMock::getInstance()->srnot_RedDiagnosticNotification(connection_id, redundancy_channel_diagnostic_data);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
