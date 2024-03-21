/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srstm.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the state machine module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRSTM_UNIT_TEST_SRSTM_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRSTM_UNIT_TEST_SRSTM_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/srstm_sr_state_machine.h"
#include "src/srcor_sr_core.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "srcor_mock.hh"
// clang-format on

using testing::NaggyMock;
using testing::StrictMock;
using testing::_;
using testing::SetArgPointee;
using testing::Return;
using testing::Invoke;
using testing::InSequence;
using testing::Throw;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRSTM_MIN_NUM_CONN 1         ///< minimum number of rasta connections
#define UT_SRSTM_MAX_NUM_CONN 2         ///< maximum number of rasta connections
#define UT_SRSTM_MIN_IDX_CONN 0         ///< minimum rasta connection index
#define UT_SRSTM_MAX_IDX_CONN 1         ///< maximum rasta connection index

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern bool srstm_initialized;          ///< initialization state (local variable)
extern sraty_ConnectionStates srstm_connection_states[UT_SRSTM_MAX_NUM_CONN]; ///< connection states (local variable)

// -----------------------------------------------------------------------------
// Test Class Definition
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the state machine module
 *
 */
class srstmTest : public ::testing::Test
{
public:
    srstmTest()
    {
        // reset initialization state
        srstm_initialized = false;
    }
    ~srstmTest()
    {

    }

    /**
     * @brief Throw invalid argument exception
     * This function is used to throw an invalid argument exception mocked function call action.
     */
    void invalidArgumentException(){
        throw std::invalid_argument("Invalid argument!");
    }

    StrictMock<rasysMock>   rasys_mock;     ///< mock for the rasta system adapter class
    StrictMock<srcorMock>   srcor_mock;     ///< mock for the rasta safety and retransmission core class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRSTM_UNIT_TEST_SRSTM_HH_
