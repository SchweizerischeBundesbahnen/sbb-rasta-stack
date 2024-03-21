/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_sradno.hh
 *
 * @author Patrick Haldi, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the adapter notifications module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRADNO_UNIT_TEST_SRADNO_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRADNO_UNIT_TEST_SRADNO_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "rasys_mock.hh"
#include "srcor_mock.hh"
#include "srnot_mock.hh"
#include "srrece_mock.hh"
#include "srsend_mock.hh"
#include "srstm_mock.hh"

using testing::NaggyMock;
using testing::StrictMock;
using testing::InSequence;

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
 * @brief Test fixture class for the diagnostics module
 *
 */
class sradnoTest: public ::testing::Test
{
public:
  sradnoTest() {
  }

  ~sradnoTest() {
  }


  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  StrictMock<rasysMock> rasys_mock; ///< mock for the rasta system adapter class
  StrictMock<srcorMock> srcor_mock; ///< mock for the rasta safety and retransmission core class
  StrictMock<srnotMock> srnot_mock; ///< mock for the rasta safety and retransmission notifications class
  StrictMock<srreceMock> srrece_mock; ///< mock for the rasta safety and retransmission received buffer class
  StrictMock<srsendMock> srsend_mock; ///< mock for the rasta safety and retransmission send buffer class
  StrictMock<srstmMock> srstm_mock; ///< mock for the rasta safety and retransmission state machine buffer class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRADNO_UNIT_TEST_SRADNO_HH_
