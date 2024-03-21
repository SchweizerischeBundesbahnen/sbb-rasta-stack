/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddia_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the reddia module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "reddia_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
reddiaMock * reddiaMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
reddiaMock::reddiaMock() {
  instance = this;
}

reddiaMock::~reddiaMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void reddia_InitRedundancyLayerDiagnostics(const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration){
    ASSERT_NE(reddiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    reddiaMock::getInstance()->reddia_InitRedundancyLayerDiagnostics(redundancy_layer_configuration);
  }
  void reddia_InitRedundancyChannelDiagnostics(const uint32_t red_channel_id){
    ASSERT_NE(reddiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    reddiaMock::getInstance()->reddia_InitRedundancyChannelDiagnostics(red_channel_id);
  }
  void reddia_UpdateRedundancyChannelDiagnostics(const uint32_t red_channel_id, const uint32_t transport_channel_id, const uint32_t message_sequence_number){
    ASSERT_NE(reddiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    reddiaMock::getInstance()->reddia_UpdateRedundancyChannelDiagnostics(red_channel_id, transport_channel_id, message_sequence_number);
  }
  bool reddia_IsConfigurationValid(const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration){
    if(reddiaMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return reddiaMock::getInstance()->reddia_IsConfigurationValid(redundancy_layer_configuration);
  }
  bool reddia_IsTransportChannelIdValid(const uint32_t red_channel_id, const uint32_t transport_channel_id){
    if(reddiaMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return reddiaMock::getInstance()->reddia_IsTransportChannelIdValid(red_channel_id, transport_channel_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
