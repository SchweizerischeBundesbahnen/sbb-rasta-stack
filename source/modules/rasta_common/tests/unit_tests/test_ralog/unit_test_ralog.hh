/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_ralog.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the logger module.
 */

#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RALOG_UNIT_TEST_RALOG_HH_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RALOG_UNIT_TEST_RALOG_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// clang-format on

using testing::_;
using testing::Throw;

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
 * @brief Test fixture class for the logger module
 *
 */
class ralogTest: public ::testing::Test
{
public:
  ralogTest() {
  }

  ~ralogTest() {
  }
};

#endif  // SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RALOG_UNIT_TEST_RALOG_HH_
