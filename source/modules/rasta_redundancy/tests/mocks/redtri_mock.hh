/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redtri_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the redtri module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDTRI_MOCK_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDTRI_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta redtri class
 */
class redtriMock {
private:
  static redtriMock *instance;  ///< redtriMock instance

public:
  /**
  * @brief redtriMock constructor
  */
  redtriMock();
  /**
  * @brief redtriMock destructor
  */
  virtual ~redtriMock();

  /**
  * @brief Mock Method object for the redtri_Init function
  */
  MOCK_METHOD(void, redtri_Init, ());

  /**
  * @brief Mock Method object for the redtri_SendMessage function
  */
  MOCK_METHOD(void, redtri_SendMessage, (const uint32_t transport_channel_id, const uint16_t message_size, const uint8_t * message_data));

  /**
  * @brief Mock Method object for the redtri_ReadMessage function
  */
  MOCK_METHOD(radef_RaStaReturnCode, redtri_ReadMessage, (const uint32_t transport_channel_id, const uint16_t buffer_size, uint16_t * message_size, uint8_t * message_buffer));

  /**
  * @brief Get the Instance object
  *
  * @return redtriMock*
  */
  static redtriMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_MOCKS_REDTRI_MOCK_HH_
