/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rednot_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the rednot module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDNOT_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDNOT_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta rednot class
 */
class rednotMock {
private:
  static rednotMock *instance;  ///< rednotMock instance

public:
  /**
  * @brief rednotMock constructor
  */
  rednotMock();
  /**
  * @brief rednotMock destructor
  */
  virtual ~rednotMock();

  /**
  * @brief Mock Method object for the rednot_MessageReceivedNotification function
  */
  MOCK_METHOD(void, rednot_MessageReceivedNotification, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the rednot_DiagnosticNotification function
  */
  MOCK_METHOD(void, rednot_DiagnosticNotification, (const uint32_t red_channel_id, const uint32_t tr_channel_id, const radef_TransportChannelDiagnosticData TransportChannelDiagnosticData));

  /**
  * @brief Get the Instance object
  *
  * @return rednotMock*
  */
  static rednotMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDNOT_MOCK_HH_
