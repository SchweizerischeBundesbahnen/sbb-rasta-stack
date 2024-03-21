/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redtrn.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the transport notifications module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDTRN_UNIT_TEST_REDTRN_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDTRN_UNIT_TEST_REDTRN_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "inc/rasta_redundancy/redtrn_transport_notifications.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "redcor_mock.hh"
#include "reddfq_mock.hh"
#include "redrbf_mock.hh"
#include "redstm_mock.hh"
#include "redtri_mock.hh"
#include "test_helper.hh"
// clang-format on

using testing::_;
using testing::StrictMock;
using testing::Invoke;
using testing::Return;
using testing::SetArgPointee;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_REDTRN_RED_CHANNEL_ID_MIN 0U      ///< minimum id of redundancy channel
#define UT_REDTRN_RED_CHANNEL_ID_MAX 1U      ///< maximum id of redundancy channel
#define UT_REDTRN_TR_CHANNEL_ID_MIN 0U       ///< minimum id of transport channel
#define UT_REDTRN_TR_CHANNEL_ID_MAX 3U       ///< maximum id of transport channel
#define UT_REDTRN_DFQ_ENTRIES_MIN 0U         ///< minimum size of the defer queue
#define UT_REDTRN_DFQ_ENTRIES_MAX 10U        ///< maximum size of the defer queue
#define UT_REDTRN_BFR_ENTRIES_MIN 0U         ///< minimum size of received buffer
#define UT_REDTRN_BFR_ENTRIES_MAX 20U        ///< maximum size of received buffer

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the transport notifications module
 *
 */
class redtrnTest : public ::testing::Test
{
public:
  redtrnTest() {
  }
  ~redtrnTest() {
  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  StrictMock<rasysMock> rasys_mock;       ///< mock for the rasta system adapter class
  redcorMock redcor_mock;                 ///< mock for the redundancy core class
  redrbfMock redrbf_mock;                 ///< mock for the redundancy received buffer class
  reddfqMock reddfq_mock;                 ///< mock for the redundancy defer queue class
  redtriMock redtri_mock;                 ///< mock for the redundancy transport interface class
  redstmMock redstm_mock;                 ///< mock for the redundancy state machine class
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDTRN_UNIT_TEST_REDTRN_HH_
