/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redstm.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for the state machine module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDSTM_UNIT_TEST_REDSTM_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDSTM_UNIT_TEST_REDSTM_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/redcor_red_core.h"
#include "src/redstm_red_state_machine.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "redcor_mock.hh"
// #include "test_helper.hh"

using testing::StrictMock;
using testing::_;
using testing::SetArgPointee;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_REDSTM_MIN_NUMBER_OF_RED_CHANNELS 1U    ///< minimum number of redundancy channels
#define UT_REDSTM_MAX_NUMBER_OF_RED_CHANNELS 2U    ///< minimum number of redundancy channels
#define UT_REDSTM_MIN_ID_CHANNEL 0U                ///< minimum id of redundancy channel
#define UT_REDSTM_MAX_ID_CHANNEL 1U                ///< maximum id of redundancy channel

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern bool redstm_initialized;             ///< initialization state (local variable)

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
 * @brief Test fixture class for the state machine module
 *
 */
class redstmTest : public ::testing::Test
{
public:
  redstmTest()
  {
    // reset initialization state
    redstm_initialized = false;
  }
  ~redstmTest()
  {

  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  StrictMock<rasysMock> rasys_mock;        ///< mock for the rasta system adapter class
  StrictMock<redcorMock> redcor_mock;      ///< mock for the rasta safety and retransmission core class

};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDSTM_UNIT_TEST_REDSTM_HH_
