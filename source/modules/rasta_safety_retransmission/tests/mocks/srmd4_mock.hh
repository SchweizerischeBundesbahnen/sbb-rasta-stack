/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmd4_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srmd4 module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRMD4_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRMD4_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "srmd4_sr_md4.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta srmd4 class
 */
class srmd4Mock {
private:
  static srmd4Mock *instance;  ///< srmd4Mock instance

public:
  /**
  * @brief srmd4Mock constructor
  */
  srmd4Mock();
  /**
  * @brief srmd4Mock destructor
  */
  virtual ~srmd4Mock();

  /**
  * @brief Mock Method object for the srmd4_CalculateMd4 function
  */
  MOCK_METHOD(void, srmd4_CalculateMd4, (const srcty_Md4InitValue md4_initial_value, const uint16_t data_size, const uint8_t * data_buffer, srmd4_Md4 * calculated_md4));

  /**
  * @brief Get the Instance object
  *
  * @return srmd4Mock*
  */
  static srmd4Mock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRMD4_MOCK_HH_
