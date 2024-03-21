/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcor_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the redcor module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redcor_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
redcorMock * redcorMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
redcorMock::redcorMock() {
  instance = this;
}

redcorMock::~redcorMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  bool redcor_IsConfigurationValid(const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration){
    if(redcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redcorMock::getInstance()->redcor_IsConfigurationValid(redundancy_layer_configuration);
  }
  void redcor_Init(const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_Init(redundancy_layer_configuration);
  }
  void redcor_InitRedundancyChannelData(const uint32_t red_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_InitRedundancyChannelData(red_channel_id);
  }
  void redcor_DeferQueueTimeout(const uint32_t red_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_DeferQueueTimeout(red_channel_id);
  }
  void redcor_WriteReceivedMessageToInputBuffer(const uint32_t red_channel_id, const uint32_t transport_channel_id, const redtyp_RedundancyMessage * received_message){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_WriteReceivedMessageToInputBuffer(red_channel_id, transport_channel_id, received_message);
  }
  void redcor_ClearInputBufferMessagePendingFlag(const uint32_t red_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_ClearInputBufferMessagePendingFlag(red_channel_id);
  }
  void redcor_ProcessReceivedMessage(const uint32_t red_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_ProcessReceivedMessage(red_channel_id);
  }
  void redcor_SetMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_SetMessagePendingFlag(red_channel_id, transport_channel_id);
  }
  bool redcor_GetMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id){
    if(redcorMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return redcorMock::getInstance()->redcor_GetMessagePendingFlag(red_channel_id, transport_channel_id);
  }
  void redcor_ClearMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_ClearMessagePendingFlag(red_channel_id, transport_channel_id);
  }
  void redcor_WriteMessagePayloadToSendBuffer(const uint32_t red_channel_id, const uint16_t payload_size, const uint8_t * payload_data){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_WriteMessagePayloadToSendBuffer(red_channel_id, payload_size, payload_data);
  }
  void redcor_ClearSendBufferMessagePendingFlag(const uint32_t red_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_ClearSendBufferMessagePendingFlag(red_channel_id);
  }
  void redcor_SendMessage(const uint32_t red_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_SendMessage(red_channel_id);
  }
  void redcor_GetAssociatedRedundancyChannel(const uint32_t transport_channel_id, uint32_t * red_channel_id){
    ASSERT_NE(redcorMock::getInstance(), nullptr) << "Mock object not initialized!";
    redcorMock::getInstance()->redcor_GetAssociatedRedundancyChannel(transport_channel_id, red_channel_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
