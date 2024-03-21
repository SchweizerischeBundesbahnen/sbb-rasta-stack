/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file raas_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the raas module
 */

#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_MOCKS_RAAS_MOCK_HH_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_MOCKS_RAAS_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta raas class
 */
class raasMock {
private:
  static raasMock *instance;  ///< raasMock instance

public:
  /**
  * @brief raasMock constructor
  */
  raasMock();
  /**
  * @brief raasMock destructor
  */
  virtual ~raasMock();

  /**
  * @brief Mock Method object for the raas_AssertNotNull function
  */
  MOCK_METHOD(void, raas_AssertNotNull, (const void * pointer, const radef_RaStaReturnCode error_reason));

  /**
  * @brief Mock Method object for the raas_AssertTrue function
  */
  MOCK_METHOD(void, raas_AssertTrue, (const bool condition, const radef_RaStaReturnCode error_reason));

  /**
  * @brief Mock Method object for the raas_AssertU8InRange function
  */
  MOCK_METHOD(void, raas_AssertU8InRange, (const uint8_t value, const uint8_t min_value, const uint8_t max_value, const radef_RaStaReturnCode error_reason));

  /**
  * @brief Mock Method object for the raas_AssertU16InRange function
  */
  MOCK_METHOD(void, raas_AssertU16InRange, (const uint16_t value, const uint16_t min_value, const uint16_t max_value, const radef_RaStaReturnCode error_reason));

  /**
  * @brief Mock Method object for the raas_AssertU32InRange function
  */
  MOCK_METHOD(void, raas_AssertU32InRange, (const uint32_t value, const uint32_t min_value, const uint32_t max_value, const radef_RaStaReturnCode error_reason));

  /**
  * @brief Get the Instance object
  *
  * @return raasMock*
  */
  static raasMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_COMMON_TESTS_MOCKS_RAAS_MOCK_HH_
