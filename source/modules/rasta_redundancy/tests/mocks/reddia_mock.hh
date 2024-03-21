/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddia_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the reddia module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDDIA_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDDIA_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_redundancy/redcty_red_config_types.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta reddia class
 */
class reddiaMock {
private:
  static reddiaMock *instance;  ///< reddiaMock instance

public:
  /**
  * @brief reddiaMock constructor
  */
  reddiaMock();
  /**
  * @brief reddiaMock destructor
  */
  virtual ~reddiaMock();

  /**
  * @brief Mock Method object for the reddia_InitRedundancyLayerDiagnostics function
  */
  MOCK_METHOD(void, reddia_InitRedundancyLayerDiagnostics, (const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration));

  /**
  * @brief Mock Method object for the reddia_InitRedundancyChannelDiagnostics function
  */
  MOCK_METHOD(void, reddia_InitRedundancyChannelDiagnostics, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the reddia_UpdateRedundancyChannelDiagnostics function
  */
  MOCK_METHOD(void, reddia_UpdateRedundancyChannelDiagnostics, (const uint32_t red_channel_id, const uint32_t transport_channel_id, const uint32_t message_sequence_number));

  /**
  * @brief Mock Method object for the reddia_IsConfigurationValid function
  */
  MOCK_METHOD(bool, reddia_IsConfigurationValid, (const redcty_RedundancyLayerConfiguration * redundancy_layer_configuration));

  /**
  * @brief Mock Method object for the reddia_IsTransportChannelIdValid function
  */
  MOCK_METHOD(bool, reddia_IsTransportChannelIdValid, (const uint32_t red_channel_id, const uint32_t transport_channel_id));

  /**
  * @brief Get the Instance object
  *
  * @return reddiaMock*
  */
  static reddiaMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDDIA_MOCK_HH_
