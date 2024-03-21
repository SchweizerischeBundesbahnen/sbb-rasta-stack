/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srrece.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the receive buffer module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRRECE_UNIT_TEST_SRRECE_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRRECE_UNIT_TEST_SRRECE_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/srrece_sr_received_buffer.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "srnot_mock.hh"
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
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRRECE_MIN_NUM_CONN 1        ///< minimum number of rasta connections
#define UT_SRRECE_MAX_NUM_CONN 2        ///< maximum number of rasta connections
#define UT_SRRECE_MIN_IDX_CONN 0        ///< minimum rasta connection index
#define UT_SRRECE_MAX_IDX_CONN 1        ///< maximum rasta connection index
#define UT_SRRECE_NSENDMAX_MIN 2        ///< minimum value for nSendMax
#define UT_SRRECE_NSENDMAX_MAX 20       ///< maximum value for nSendMax
#define UT_SRRECE_DATA_LEN_MIN 1        ///< minimum payload length of a data message
#define UT_SRRECE_DATA_LEN_MAX 1055     ///< maximum payload length of a data message

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

extern uint8_t test_payload_data_u8[UT_SRRECE_DATA_LEN_MAX];

extern bool srrece_initialized;                            ///< initialization state (local variable)

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the received buffer module
 *
 */
class srreceTest : public ::testing::Test
{
public:
    srreceTest()
    {
        // reset initialization state
        srrece_initialized = false;
    }
    ~srreceTest()
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
    NaggyMock<srnotMock>    srnot_mock;     ///< mock for the rasta safety and retransmission notifications class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRRECE_UNIT_TEST_SRRECE_HH_
