/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_rahlp.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the rasta helper module of the common package.
 */

#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RAHLP_UNIT_TEST_RAHLP_HH_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RAHLP_UNIT_TEST_RAHLP_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "rasta_common/rahlp_rasta_helper.h"
#include "rasys_mock.hh"
// clang-format on

using testing::Invoke;
using testing::StrictMock;
using testing::NaggyMock;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Class Definition
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the rasta helper module
 *
 */
class rahlpTest : public ::testing::Test
{
public:
  rahlpTest() {
  }
  ~rahlpTest() {
  }


  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  StrictMock<rasysMock>   rasys_mock;       ///< mock for the rasta system adapter class
};

#endif  // SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RAHLP_UNIT_TEST_RAHLP_HH_
