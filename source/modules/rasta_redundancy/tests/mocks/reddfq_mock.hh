/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddfq_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the reddfq module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDDFQ_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDDFQ_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta reddfq class
 */
class reddfqMock {
private:
  static reddfqMock *instance;  ///< reddfqMock instance

public:
  /**
  * @brief reddfqMock constructor
  */
  reddfqMock();
  /**
  * @brief reddfqMock destructor
  */
  virtual ~reddfqMock();

  /**
  * @brief Mock Method object for the reddfq_Init function
  */
  MOCK_METHOD(void, reddfq_Init, (const uint32_t configured_red_channels, const uint32_t configured_defer_queue_size, const uint32_t configured_t_seq));

  /**
  * @brief Mock Method object for the reddfq_InitDeferQueue function
  */
  MOCK_METHOD(void, reddfq_InitDeferQueue, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the reddfq_AddMessage function
  */
  MOCK_METHOD(void, reddfq_AddMessage, (const uint32_t red_channel_id, const redtyp_RedundancyMessage * redundancy_message));

  /**
  * @brief Mock Method object for the reddfq_GetMessage function
  */
  MOCK_METHOD(void, reddfq_GetMessage, (const uint32_t red_channel_id, const uint32_t sequence_number, redtyp_RedundancyMessage * redundancy_message));

  /**
  * @brief Mock Method object for the reddfq_IsTimeout function
  */
  MOCK_METHOD(bool, reddfq_IsTimeout, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the reddfq_Contains function
  */
  MOCK_METHOD(bool, reddfq_Contains, (const uint32_t red_channel_id, const uint32_t sequence_number));

  /**
  * @brief Mock Method object for the reddfq_GetOldestSequenceNumber function
  */
  MOCK_METHOD(uint32_t, reddfq_GetOldestSequenceNumber, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the reddfq_GetUsedEntries function
  */
  MOCK_METHOD(uint32_t, reddfq_GetUsedEntries, (const uint32_t red_channel_id));

  /**
  * @brief Mock Method object for the reddfq_IsSequenceNumberOlder function
  */
  MOCK_METHOD(bool, reddfq_IsSequenceNumberOlder, (const uint32_t sequence_number_to_compare, const uint32_t sequence_number_reference));

  /**
  * @brief Get the Instance object
  *
  * @return reddfqMock*
  */
  static reddfqMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDDFQ_MOCK_HH_
