/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rasys_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the rasys module
 */

#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_MOCKS_RASYS_MOCK_HH_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_MOCKS_RASYS_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta rasys class
 */
class rasysMock {
private:
  static rasysMock *instance;  ///< rasysMock instance

public:
  /**
  * @brief rasysMock constructor
  */
  rasysMock();
  /**
  * @brief rasysMock destructor
  */
  virtual ~rasysMock();

  /**
  * @brief Mock Method object for the rasys_GetTimerValue function
  */
  MOCK_METHOD(uint32_t, rasys_GetTimerValue, ());

  /**
  * @brief Mock Method object for the rasys_GetTimerGranularity function
  */
  MOCK_METHOD(uint32_t, rasys_GetTimerGranularity, ());

  /**
  * @brief Mock Method object for the rasys_GetRandomNumber function
  */
  MOCK_METHOD(uint32_t, rasys_GetRandomNumber, ());

  /**
  * @brief Mock Method object for the rasys_FatalError function
  */
  MOCK_METHOD(void, rasys_FatalError, (const radef_RaStaReturnCode error_reason));

  /**
  * @brief Get the Instance object
  *
  * @return rasysMock*
  */
  static rasysMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_COMMON_TESTS_MOCKS_RASYS_MOCK_HH_
