/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srdia.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for the diagnostics module unit tests of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRDIA_UNIT_TEST_SRDIA_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRDIA_UNIT_TEST_SRDIA_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "rasys_mock.hh"
#include "srdia_sr_diagnostics.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "srnot_mock.hh"

using testing::NaggyMock;
using testing::StrictMock;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRDIA_CONFIGURED_CONNECTIONS_MIN 1U     ///< min amount of configured connections
#define UT_SRDIA_CONFIGURED_CONNECTIONS_MAX 2U     ///< max amount of configured connections
#define UT_SRDIA_DIAG_TIMINGS_DIST_INTERVALS 5U    ///< amount of diagnostics timing intervals
#define UT_SRDIA_N_DIAG_WINDOW_MIN 100U            ///< min size of diagnostic window
#define UT_SRDIA_N_DIAG_WINDOW_MAX 10000U          ///< max size of diagnostic window
#define UT_SRDIA_T_MIN 750U                        ///< tmax min value
#define UT_SRDIA_T_MAX 2000U                       ///< tmax max value

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern bool srdia_initialized; ///< initialization state (local variable)
extern srdia_SrConnectionDiagnostics srdia_connection_diagnostics[UT_SRDIA_CONFIGURED_CONNECTIONS_MAX]; ///< connection diagnostics data

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
 * @brief Test fixture class for the diagnostics module
 *
 */
class srdiaTest: public ::testing::Test
{
public:
  srdiaTest() {
    // reset initialisation state
    srdia_initialized = false;
  }

  ~srdiaTest() {
  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  sraty_ConnectionDiagnosticData diag_noti; ///< temporary buffer for the received diagnostic notification

  const uint32_t distribution_interval_1 = 150U;  ///< default diagnostic interval 1
  const uint32_t distribution_interval_2 = 300U;  ///< default diagnostic interval 2
  const uint32_t distribution_interval_3 = 450U;  ///< default diagnostic interval 3
  const uint32_t distribution_interval_4 = 600U;  ///< default diagnostic interval 4

  const uint32_t diag_timings_distribution_intervals[4] = {
    distribution_interval_1,
    distribution_interval_2,
    distribution_interval_3,
    distribution_interval_4
  }; ///< default diagnostics intervals from config

  StrictMock<rasysMock> rasys_mock; ///< mock for the rasta system adapter class
  NaggyMock<srnotMock> srnot_mock; ///< mock for the rasta safety and retransmission notifications class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRDIA_UNIT_TEST_SRDIA_HH_
