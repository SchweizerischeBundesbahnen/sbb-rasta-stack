/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddfq_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the reddfq module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "reddfq_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
reddfqMock * reddfqMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
reddfqMock::reddfqMock() {
  instance = this;
}

reddfqMock::~reddfqMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  void reddfq_Init(const uint32_t configured_red_channels, const uint32_t configured_defer_queue_size, const uint32_t configured_t_seq){
    ASSERT_NE(reddfqMock::getInstance(), nullptr) << "Mock object not initialized!";
    reddfqMock::getInstance()->reddfq_Init(configured_red_channels, configured_defer_queue_size, configured_t_seq);
  }
  void reddfq_InitDeferQueue(const uint32_t red_channel_id){
    ASSERT_NE(reddfqMock::getInstance(), nullptr) << "Mock object not initialized!";
    reddfqMock::getInstance()->reddfq_InitDeferQueue(red_channel_id);
  }
  void reddfq_AddMessage(const uint32_t red_channel_id, const redtyp_RedundancyMessage * redundancy_message){
    ASSERT_NE(reddfqMock::getInstance(), nullptr) << "Mock object not initialized!";
    reddfqMock::getInstance()->reddfq_AddMessage(red_channel_id, redundancy_message);
  }
  void reddfq_GetMessage(const uint32_t red_channel_id, const uint32_t sequence_number, redtyp_RedundancyMessage * redundancy_message){
    ASSERT_NE(reddfqMock::getInstance(), nullptr) << "Mock object not initialized!";
    reddfqMock::getInstance()->reddfq_GetMessage(red_channel_id, sequence_number, redundancy_message);
  }
  bool reddfq_IsTimeout(const uint32_t red_channel_id){
    if(reddfqMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return reddfqMock::getInstance()->reddfq_IsTimeout(red_channel_id);
  }
  bool reddfq_Contains(const uint32_t red_channel_id, const uint32_t sequence_number){
    if(reddfqMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return reddfqMock::getInstance()->reddfq_Contains(red_channel_id, sequence_number);
  }
  uint32_t reddfq_GetOldestSequenceNumber(const uint32_t red_channel_id){
    if(reddfqMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return reddfqMock::getInstance()->reddfq_GetOldestSequenceNumber(red_channel_id);
  }
  uint32_t reddfq_GetUsedEntries(const uint32_t red_channel_id){
    if(reddfqMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return reddfqMock::getInstance()->reddfq_GetUsedEntries(red_channel_id);
  }
  bool reddfq_IsSequenceNumberOlder(const uint32_t sequence_number_to_compare, const uint32_t sequence_number_reference){
    if(reddfqMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return reddfqMock::getInstance()->reddfq_IsSequenceNumberOlder(sequence_number_to_compare, sequence_number_reference);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
