/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redstm_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the redstm module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDSTM_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDSTM_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "redstm_red_state_machine.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta redstm class
 */
class redstmMock {
private:
  static redstmMock *instance;  ///< redstmMock instance

public:
  /**
  * @brief redstmMock constructor
  */
  redstmMock();
  /**
  * @brief redstmMock destructor
  */
  virtual ~redstmMock();

  /**
  * @brief Mock Method object for the redstm_Init function
  */
  MOCK_METHOD(void, redstm_Init, (const uint32_t configured_red_channels));

  /**
  * @brief Mock Method object for the redstm_ProcessChannelStateMachine function
  */
  MOCK_METHOD(void, redstm_ProcessChannelStateMachine, (const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event));

  /**
  * @brief Mock Method object for the redstm_GetChannelState function
  */
  MOCK_METHOD(redstm_RedundancyChannelStates, redstm_GetChannelState, (const uint32_t red_channel_id));

  /**
  * @brief Get the Instance object
  *
  * @return redstmMock*
  */
  static redstmMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDSTM_MOCK_HH_
