/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redmsg.hh
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the messages module of the redundancy layer.
 */

#ifndef SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDMSG_UNIT_TEST_REDMSG_HH_
#define SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDMSG_UNIT_TEST_REDMSG_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/redmsg_red_messages.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "redcrc_mock.hh"
#include "test_helper.hh"
// clang-format on

using testing::_;
using testing::InSequence;
using ::testing::SetArgPointee;
using testing::Invoke;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_REDMSG_DATA_LEN_MIN 28U                                              ///< minimum value for payload data length (min sr message size)
#define UT_REDMSG_DATA_LEN_MAX 1101U                                            ///< maximum value for payload data length (max sr message size)
#define UT_REDMSG_MESSAGE_LEN_MIN 36U                                           ///< minimum value for message length (header + min sr message size (no crc))
#define UT_REDMSG_MESSAGE_LEN_MAX 1113U                                         ///< maximum value for message length (header + max sr message size + crc)
#define UT_REDMSG_HEADER_SIZE 8U                                                ///< size of the PDU Message header
#define UT_REDMSG_CHECK_CODE_MAX_LEN 4U                                         ///< maximum length of the check code (CRC 32)
#define UT_REDMSG_CHECK_CODE_HALF_LEN 2U                                        ///< default length of the check code (CRC 16)
#define UT_REDMSG_CHECK_CODE_NONE_LEN 0U                                        ///< default length of the check code (none)

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

extern uint8_t test_red_payload_data_u8[UT_REDMSG_DATA_LEN_MAX];
extern bool redmsg_initialized;                                                 ///< initialization state (local variable)
extern redcty_CheckCodeType redmsg_check_code_type;                             ///< configured checkcode type

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the messages module
 *
 */
class redmsgTest : public ::testing::Test
{
public:
  redmsgTest() {
    // reset initialization state
    redmsg_initialized = false;
  }
  ~redmsgTest() {
  }

  static const uint8_t expected_message_length_pos    = 0U;    ///< expected Byteposition of the Message Lengh (according to RASW-202)
  static const uint8_t expected_reserve_pos           = 2U;    ///< expected Byteposition of the Message Type (according to RASW-202)
  static const uint8_t expected_sequence_nbr_pos      = 4U;    ///< expected Byteposition of the Sequence Nmber (according to RASW-202)

  /**
    * @brief Set crc check code type
    * This function sets the given checkcode type
    */
  void crc_Init(const redcty_CheckCodeType configured_check_code_type)
  {
    redmsg_check_code_type = configured_check_code_type;
  }

  /**
   * @brief Throw invalid argument exception
   * This function is used to throw an invalid argument exception mocked function call action.
   */
  void invalidArgumentException(){
    throw std::invalid_argument("Invalid argument!");
  }


  rasysMock rasys_mock;        ///< mock for the rasta system adapter class
  redcrcMock redcrc_mock;      ///< mock for the rasta crc class
};

#endif  // SOURCE_MODULES_RASTA_REDUNDANCY_TESTS_UNIT_TESTS_TEST_REDMSG_UNIT_TEST_REDMSG_HH_
