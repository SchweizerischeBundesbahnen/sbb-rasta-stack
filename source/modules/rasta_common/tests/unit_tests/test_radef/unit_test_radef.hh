/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_radef.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of rasta definitions.
 */

#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RADEF_UNIT_TEST_RADEF_HH_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RADEF_UNIT_TEST_RADEF_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "rasta_common/radef_rasta_definitions.h"
// clang-format on

using testing::_;
using testing::Throw;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_RADEF_CONNECTION_MAX                     2U         ///< maximum number of RaSTA connections
#define UT_RADEF_MSG_LENGTH_MAX                     1055U       ///< maximum number of application message length
#define UT_RADEF_SIM_RED_CHANNELS_MAX               2U         ///< maximum number of simultaneous redundancy channels
#define UT_RADEF_TR_CHANNEL_PER_RED_CHANNEL_MAX     2U         ///< maximum number of transport channels per redundancy channel

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
 * @brief Test fixture class for rasta definition tests
 *
 */
class radefTest: public ::testing::Test
{
public:
  radefTest() {
  }

  ~radefTest() {
  }
};

#endif  // SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RADEF_UNIT_TEST_RADEF_HH_
