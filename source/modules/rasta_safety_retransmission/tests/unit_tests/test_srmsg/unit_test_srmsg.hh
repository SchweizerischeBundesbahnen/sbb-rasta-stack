/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srmsg.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 29da60684916902916f5753e060aa2f07396d8d8
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4935: add testcases for message size and payload size,09.12.2022,M. Kuhn}
 *
 * @brief Header file for unit tests of the messages module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRMSG_UNIT_TEST_SRMSG_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRMSG_UNIT_TEST_SRMSG_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/srmsg_sr_messages.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
#include "srmd4_mock.hh"
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

#define UT_SRMSG_PROT_MIN 0x30                                      ///< minimum value for protocol version
#define UT_SRMSG_PROT_MAX 0x39                                      ///< maximum value for protocol version
#define UT_SRMSG_NSENDMAX_MIN 2                                     ///< minimum value for nSendMax
#define UT_SRMSG_NSENDMAX_MAX 20                                    ///< maximum value for nSendMax
#define UT_SRMSG_DATA_LEN_MIN 1                                     ///< minimum value for payload data length
#define UT_SRMSG_DATA_LEN_MAX 1055                                  ///< maximum value for payload data length
#define UT_SRMSG_MESSAGE_LEN_MIN 28                                 ///< minimum value for general message length
#define UT_SRMSG_MESSAGE_LEN_MAX 1101                               ///< maximum value for general message length
#define UT_SRMSG_MESSAGE_LEN_MIN_NONE_MD4 28                        ///< minimum value for general message length
#define UT_SRMSG_MESSAGE_LEN_MIN_HALF_MD4 36                        ///< minimum value for general message length
#define UT_SRMSG_MESSAGE_LEN_MIN_FULL_MD4 44                        ///< minimum value for general message length
#define UT_SRMSG_MESSAGE_LEN_MAX_NONE_MD4 1085                      ///< maximum value for general message length with no safety code
#define UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4 1093                      ///< maximum value for general message length with half md4 safety code
#define UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4 1101                      ///< maximum value for general message length with full md4 safety code
#define UT_SRMSG_HEADER_SIZE 28                                     ///< size of the PDU Message header
#define UT_SRMSG_REASON_MIN sraty_kDiscReasonUserRequest            ///< minimum value for disconnection reason
#define UT_SRMSG_REASON_MAX sraty_kDiscReasonProtocolSequenceError  ///< maximum value for disconnection reason
#define UT_SRMSG_SAFETY_CODE_MAX_LEN 16                             ///< maximum length of the safety code (full md4)
#define UT_SRMSG_SAFETY_CODE_HALF_LEN 8                             ///< default length of the safety code (lower md4)
#define UT_SRMSG_SAFETY_CODE_NONE_LEN 0                             ///< default length of the safety code (no safety)

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

extern uint8_t test_payload_data_u8[UT_SRMSG_DATA_LEN_MAX];

extern bool srmsg_initialized;                            ///< initialization state (local variable)

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the messages module
 *
 */
class srmsgTest : public ::testing::Test
{
public:
    srmsgTest()
    {
        // reset initialization state
        srmsg_initialized = false;
    }
    ~srmsgTest()
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

    /**
     * @brief Check MD4 init values and provide calculation result
     * This function verifies the initial values during a mocked MD4 function call
     * and provides an MD4 result.
     */
    struct checkMd4InitAndProvideResult {
        /**
         * @brief Mocked CalculateMd4 function
         *
         * @param md4_initial_value initial values
         * @param data_size         size of the input data
         * @param data_buffer       pointer to the data buffer
         * @param calculated_md4    pointer to the MD4 result data
         */
        void operator()(const srcty_Md4InitValue md4_initial_value,
                        const uint16_t data_size,
                        const uint8_t *const data_buffer,
                        srmd4_Md4 *const calculated_md4)
        {
            EXPECT_EQ(expected_init_values.init_a, md4_initial_value.init_a);
            EXPECT_EQ(expected_init_values.init_b, md4_initial_value.init_b);
            EXPECT_EQ(expected_init_values.init_c, md4_initial_value.init_c);
            EXPECT_EQ(expected_init_values.init_d, md4_initial_value.init_d);

            for (uint8_t i = 0; i < UT_SRMSG_SAFETY_CODE_MAX_LEN; i++)
            {
                calculated_md4->md4[i] = result_values[i];
            }

        }

        /**
         * @brief Construct a new check Md4 Init And Provide Result object
         *
         * @param init_values    initial values to be tested against
         * @param result_data    pointer to the MD4 result data to be returned
         */
        checkMd4InitAndProvideResult(srcty_Md4InitValue init_values, uint8_t* result_data){
            expected_init_values = init_values;
            result_values = result_data;
        }

        srcty_Md4InitValue expected_init_values;    ///< expected MD4 init values to be tested against
        uint8_t * result_values;                    ///< pointer to the MD4 result to be returned
    };

    /**
     * @brief Throw invalid argument exception
     * This function is used to throw an invalid argument exception mocked function call action.
     */
    void invalidArgumentException(){
        throw std::invalid_argument("Invalid argument!");
    }

    StrictMock<rasysMock>   rasys_mock;     ///< mock for the rasta system adapter class
    NaggyMock<srmd4Mock>    srmd4_mock;     ///< mock for the rasta md4 class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRMSG_UNIT_TEST_SRMSG_HH_
