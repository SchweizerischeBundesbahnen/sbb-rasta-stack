/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srsend.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the send buffer module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRSEND_UNIT_TEST_SRSEND_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRSEND_UNIT_TEST_SRSEND_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/srsend_sr_send_buffer.h"
#include "rasta_common/raas_rasta_assert.h"
#include "src/srmsg_sr_messages.h"
#include "rasys_mock.hh"
#include "test_helper.hh"
// clang-format on

using testing::_;
using testing::SetArgPointee;
using testing::Return;
using testing::Invoke;
using testing::InSequence;
using testing::Throw;
using testing::StrictMock;
using testing::NaggyMock;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRSEND_MIN_NUM_CONN 1            ///< minimum number of rasta connections
#define UT_SRSEND_MAX_NUM_CONN 2            ///< maximum number of rasta connections
#define UT_SRSEND_MIN_IDX_CONN 0            ///< minimum rasta connection index
#define UT_SRSEND_MAX_IDX_CONN 1            ///< maximum rasta connection index
#define UT_SRSEND_NSENDMAX_MIN 2            ///< minimum value for nSendMax
#define UT_SRSEND_NSENDMAX_MAX 20           ///< maximum value for nSendMax
#define UT_SRSEND_MESSAGE_LEN_MAX 1101      ///< maximum message length (full data message with full MD4)
#define UT_SRSEND_DATA_LEN_MAX 1055         ///< maximum value for payload data length

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

extern uint8_t test_message_data_u8[UT_SRSEND_MESSAGE_LEN_MAX];

extern bool srsend_initialized;                         ///< initialization state of the send buffer module
extern bool srmsg_initialized;                          ///< initialization state of the messages module

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the send buffer module
 *
 */
class srsendTest : public ::testing::Test
{
public:
    srsendTest()
    {
        // reset initialization state
        srsend_initialized = false;
        srmsg_initialized = false;
        // initialize messages module
        srmsg_Init(srcty_kSafetyCodeTypeFullMd4, {0, 0, 0, 0});
    }
    ~srsendTest()
    {

    }

    static const uint8_t expected_message_length_pos    = 0;    ///< expected byte position of the Message Length (according to RASW-160)
    static const uint8_t expected_message_type_pos      = 2;    ///< expected byte position of the Message Type (according to RASW-160)
    static const uint8_t expected_receiver_id_pos       = 4;    ///< expected byte position of the Receiver ID (according to RASW-160)
    static const uint8_t expected_sender_id_pos         = 8;    ///< expected byte position of the Sender ID (according to RASW-160)
    static const uint8_t expected_sequence_nbr_pos      = 12;   ///< expected byte position of the Sequence Number (according to RASW-160)
    static const uint8_t expected_conf_seq_nbr_pos      = 16;   ///< expected byte position of the Conf. Sequence Number (according to RASW-160)
    static const uint8_t expected_timestamp_pos         = 20;   ///< expected byte position of the Timestamp (according to RASW-160)
    static const uint8_t expected_conf_timestamp_pos    = 24;   ///< expected byte position of the Conf. Timestamp (according to RASW-160)
    static const uint8_t expected_payload_size_pos      = 28;   ///< expected byte position of the Payload Size (according to RASW-191)
    static const uint8_t expected_payload_pos           = 30;   ///< expected byte position of the Payload (according to RASW-191)

    static const uint16_t expected_type_conn_req        = 6200; ///< expected Message Type for a Connection Request Message (according to RASW-161)
    static const uint16_t expected_type_conn_resp       = 6201; ///< expected Message Type for a Connection Response Message (according to RASW-161)
    static const uint16_t expected_type_retr_req        = 6212; ///< expected Message Type for a Retransmission Request Message (according to RASW-161)
    static const uint16_t expected_type_retr_resp       = 6213; ///< expected Message Type for a Retransmission Response Message (according to RASW-161)
    static const uint16_t expected_type_disc_req        = 6216; ///< expected Message Type for a Disconnection Request Message (according to RASW-161)
    static const uint16_t expected_type_heartbeat       = 6220; ///< expected Message Type for a Heartbeat Message (according to RASW-161)
    static const uint16_t expected_type_data            = 6240; ///< expected Message Type for a Data Message (according to RASW-161)
    static const uint16_t expected_type_retr_data       = 6241; ///< expected Message Type for a Retransmitted Data Message (according to RASW-161)

    /**
     * @brief Throw invalid argument exception
     * This function is used to throw an invalid argument exception mocked function call action.
     */
    void invalidArgumentException(){
        throw std::invalid_argument("Invalid argument!");
    }

    StrictMock<rasysMock>   rasys_mock;         ///< mock for the rasta system adapter class
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRSEND_UNIT_TEST_SRSEND_HH_
