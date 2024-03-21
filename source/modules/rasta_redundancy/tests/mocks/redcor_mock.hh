/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcor_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the redcor module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDCOR_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDCOR_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_redundancy/redcty_red_config_types.h"
#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta redcor class
 */
class redcorMock {
private:
  static redcorMock *instance;  ///< redcorMock instance

public:
  /**
  * @brief redcorMock constructor
  */
  redcorMock();
  /**
  * @brief redcorMock destructor
  */
  virtual ~redcorMock();

  /**
  * @brief Mock Method object for the redcor_IsConfigurationValid function
  */
  MOCK_METHOD(bool, redcor_IsConfigurationValid, (const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration));

  /**
  * @brief Mock Method object for the redcor_Init function
  */
  MOCK_METHOD(void, redcor_Init, (const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration));

  /**
  * @brief Mock Method object for the redcor_InitRedundancyChannelData function
  */
  MOCK_METHOD(void, redcor_InitRedundancyChannelData, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the redcor_DeferQueueTimeout function
  */
  MOCK_METHOD(void, redcor_DeferQueueTimeout, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the redcor_WriteReceivedMessageToInputBuffer function
  */
  MOCK_METHOD(void, redcor_WriteReceivedMessageToInputBuffer, (const uint32_t red_channel_id, const uint32_t transport_channel_id, const redtyp_RedundancyMessage * received_message));

  /**
  * @brief Mock Method object for the redcor_ClearInputBufferMessagePendingFlag function
  */
  MOCK_METHOD(void, redcor_ClearInputBufferMessagePendingFlag, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the redcor_ProcessReceivedMessage function
  */
  MOCK_METHOD(void, redcor_ProcessReceivedMessage, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the redcor_SetMessagePendingFlag function
  */
  MOCK_METHOD(void, redcor_SetMessagePendingFlag, (const uint32_t red_channel_id, const uint32_t transport_channel_id));

  /**
  * @brief Mock Method object for the redcor_GetMessagePendingFlag function
  */
  MOCK_METHOD(bool, redcor_GetMessagePendingFlag, (const uint32_t red_channel_id, const uint32_t transport_channel_id));

  /**
  * @brief Mock Method object for the redcor_ClearMessagePendingFlag function
  */
  MOCK_METHOD(void, redcor_ClearMessagePendingFlag, (const uint32_t red_channel_id, const uint32_t transport_channel_id));

  /**
  * @brief Mock Method object for the redcor_WriteMessagePayloadToSendBuffer function
  */
  MOCK_METHOD(void, redcor_WriteMessagePayloadToSendBuffer, (const uint32_t red_channel_id, const uint16_t payload_size, const uint8_t * payload_data));

  /**
  * @brief Mock Method object for the redcor_ClearSendBufferMessagePendingFlag function
  */
  MOCK_METHOD(void, redcor_ClearSendBufferMessagePendingFlag, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the redcor_SendMessage function
  */
  MOCK_METHOD(void, redcor_SendMessage, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the redcor_GetAssociatedRedundancyChannel function
  */
  MOCK_METHOD(void, redcor_GetAssociatedRedundancyChannel, (const uint32_t transport_channel_id, uint32_t * red_channel_id));

  /**
  * @brief Get the Instance object
  *
  * @return redcorMock*
  */
  static redcorMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDCOR_MOCK_HH_
