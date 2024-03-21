/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcrc_mock.h
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the redcrc module
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_MOCKS_REDCRC_MOCK_H_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_MOCKS_REDCRC_MOCK_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_redundancy/redcty_red_config_types.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta redcrc class
 */
class redcrcMock {
private:
  static redcrcMock *instance;  ///< redcrcMock instance

public:
  /**
  * @brief redcrcMock constructor
  */
  redcrcMock();
  /**
  * @brief redcrcMock destructor
  */
  virtual ~redcrcMock();

  /**
  * @brief Mock Method object for the redcrc_Init function
  */
  MOCK_METHOD(void, redcrc_Init, (const redcty_CheckCodeType configured_check_code_type));

  /**
  * @brief Mock Method object for the redcrc_CalculateCrc function
  */
  MOCK_METHOD(void, redcrc_CalculateCrc, (const uint16_t data_size, const uint8_t * data_buffer, uint32_t * calculated_crc));

  /**
  * @brief Get the Instance object
  *
  * @return redcrcMock*
  */
  static redcrcMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_MOCKS_REDCRC_MOCK_H_
