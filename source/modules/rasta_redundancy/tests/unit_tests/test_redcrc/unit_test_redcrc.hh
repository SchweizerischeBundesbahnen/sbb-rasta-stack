/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redcrc.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the crc module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDCRC_UNIT_TEST_REDCRC_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDCRC_UNIT_TEST_REDCRC_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/redcrc_red_crc.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "test_helper.hh"
// clang-format on

using testing::_;
using testing::SetArgPointee;
using testing::Return;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_REDCRC_DATA_LEN_MAX 1109U                ///< maximum value for data (Header and Payload)
#define UT_REDCRC_DATA_LEN_MIN 36U                  ///< minimum value for data (Header and Payload)

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern bool redcrc_initialized;    ///< initialization state (local variable)

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
 * @brief Test fixture class for the crc module
 *
 */
class redcrcTest : public ::testing::Test
{
public:
  redcrcTest()
  {
    // reset initialization state
    redcrc_initialized = false;
  }
  ~redcrcTest()
  {
  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  rasysMock rasys_mock;        ///< mock for the rasta system adapter class
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDCRC_UNIT_TEST_REDCRC_HH_
