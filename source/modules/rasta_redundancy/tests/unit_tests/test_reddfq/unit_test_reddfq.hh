/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_reddfq.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version b4d4bd413ce5fa54d373eb00890116763cac504a
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 *
 * @brief Header file for unit tests of the defer queue module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDDFQ_UNIT_TEST_REDDFQ_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDDFQ_UNIT_TEST_REDDFQ_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/reddfq_red_defer_queue.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "redmsg_mock.hh"
#include "test_helper.hh"
// clang-format on

using testing::_;
using testing::SetArgPointee;
using testing::Return;
using testing::Invoke;
using testing::InSequence;
using testing::Throw;

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define UT_REDDFQ_MSG_LEN_MAX 1113U                                        ///< maximum value for message length
#define UT_REDDFQ_MSG_LEN_MIN 36U                                          ///< maximum value for message length
#define UT_REDDFQ_CHANNEL_NUM_MIN 1U                                       ///< minimum amount of channels
#define UT_REDDFQ_CHANNEL_NUM_MAX 2U                                       ///< maximum amount of channels
#define UT_REDDFQ_CHANNEL_ID_MIN 0U                                        ///< minimum channel id
#define UT_REDDFQ_CHANNEL_ID_MAX 1U                                        ///< maximum channel id
#define UT_REDDFQ_QUEUE_SIZE_MIN 4U                                        ///< minimum size of the defer queue
#define UT_REDDFQ_QUEUE_SIZE_MAX 10U                                       ///< maximum size of the defer queue
#define UT_REDDFQ_T_SEQ_MIN 50U                                           ///< minimum time period (in ms) how long a message is buffered
#define UT_REDDFQ_T_SEQ_MAX 500U                                          ///< maximum time period (in ms) how long a message is buffered

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Variable Declarations
// -----------------------------------------------------------------------------

extern uint8_t test_red_message_data_u8[UT_REDDFQ_MSG_LEN_MAX];
extern bool reddfq_initialized;                                         ///< initialization state (local variable)

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
 * @brief Test fixture class for the defer queue module
 *
 */
class reddfqTest : public ::testing::Test
{
public:
  reddfqTest()
  {
    reddfq_initialized = false;
  }

  ~reddfqTest()
  {

  }

  static const uint8_t expected_sequence_nbr_pos      = 4U;               ///< expected byte position of the Sequence Number (according to RASW-202)

  /**
    * @brief Return message sequence number
    * This function returns the sequence number of a redundancy message
  */
  uint32_t GetMessageSequenceNumber(const redtyp_RedundancyMessage *const redundancy_message)
  {
    return getDataFromBuffer_u32(redundancy_message->message, expected_sequence_nbr_pos);
  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }

  rasysMock rasys_mock;        ///< mock for the rasta system adapter class
  redmsgMock redmsg_mock;      ///< mock for the message module class
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDDFQ_UNIT_TEST_REDDFQ_HH_
