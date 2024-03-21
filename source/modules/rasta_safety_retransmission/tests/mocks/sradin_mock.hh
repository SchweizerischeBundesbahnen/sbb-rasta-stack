/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file sradin_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the sradin module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRADIN_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRADIN_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta sradin class
 */
class sradinMock {
private:
  static sradinMock *instance;  ///< sradinMock instance

public:
  /**
  * @brief sradinMock constructor
  */
  sradinMock();
  /**
  * @brief sradinMock destructor
  */
  virtual ~sradinMock();

  /**
  * @brief Mock Method object for the sradin_Init function
  */
  MOCK_METHOD(void, sradin_Init, ());

  /**
  * @brief Mock Method object for the sradin_OpenRedundancyChannel function
  */
  MOCK_METHOD(void, sradin_OpenRedundancyChannel, (const uint32_t redundancy_channel_id));

  /**
  * @brief Mock Method object for the sradin_CloseRedundancyChannel function
  */
  MOCK_METHOD(void, sradin_CloseRedundancyChannel, (const uint32_t redundancy_channel_id));

  /**
  * @brief Mock Method object for the sradin_SendMessage function
  */
  MOCK_METHOD(void, sradin_SendMessage, (const uint32_t redundancy_channel_id, const uint16_t message_size, const uint8_t * message_data));

  /**
  * @brief Mock Method object for the sradin_ReadMessage function
  */
  MOCK_METHOD(radef_RaStaReturnCode, sradin_ReadMessage, (const uint32_t redundancy_channel_id, const uint16_t buffer_size, uint16_t * message_size, uint8_t * message_buffer));

  /**
  * @brief Get the Instance object
  *
  * @return sradinMock*
  */
  static sradinMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRADIN_MOCK_HH_
