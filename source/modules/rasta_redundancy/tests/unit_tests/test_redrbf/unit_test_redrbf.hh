/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redrbf.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the receive buffer module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDRBF_UNIT_TEST_REDRBF_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDRBF_UNIT_TEST_REDRBF_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/redrbf_red_received_buffer.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "test_helper.hh"
#include "rednot_mock.hh"

using testing::NaggyMock;
using testing::StrictMock;
using testing::_;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_REDRBF_MIN_NUM_OF_RED_CHANNELS 1U              ///< minimum number of redundancy channels
#define UT_REDRBF_MAX_NUM_OF_RED_CHANNELS 2U              ///< maximum number of redundancy channels
#define UT_REDRBF_DATA_LEN_MIN 28U                        ///< minimum value for payload data length (min sr message size)
#define UT_REDRBF_DATA_LEN_MAX 1101U                      ///< maximum payload length of a data message (max sr message size)
#define UT_REDRBF_BUFFER_LEN_MIN UT_REDRBF_DATA_LEN_MIN   ///< minimum buffer length (min sr message size)
#define UT_REDRBF_BUFFER_LEN_MAX UT_REDRBF_DATA_LEN_MAX   ///< maximum buffer length (max sr message size)
#define UT_REDRBF_N_SEND_MAX 20U                          ///< maximum receive buffer size

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern uint8_t test_red_payload_data_u8[UT_REDRBF_DATA_LEN_MAX];

extern bool redrbf_initialized;                            ///< initialization state (local variable)

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
 * @brief Test fixture class for the received buffer module
 *
 */
class redrbfTest : public ::testing::Test
{
public:
  redrbfTest()
  {
    // reset initialization state
    redrbf_initialized = false;
  }
  ~redrbfTest()
  {

  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  StrictMock<rasysMock>   rasys_mock;      ///< mock for the rasta system adapter class
  NaggyMock<rednotMock>   rednot_mock;     ///< mock for the redundancy notifications class
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDRBF_UNIT_TEST_REDRBF_HH_
