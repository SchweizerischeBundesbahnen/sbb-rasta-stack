/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srmsg_update_check.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 29da60684916902916f5753e060aa2f07396d8d8
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4935: add testcases for message size and payload size,09.12.2022,M. Kuhn}
 *
 * @brief Unit test file for the messages module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srmsg.hh"

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized update message header test
 *
 * Parameter order:
 * - messageHeaderUpdate.confirmed_sequence_number (uint32_t)
 * - messageHeaderUpdate.timestamp (uint32_t)
 * - safety code type (srcty_SafetyCodeType)
 * - assert message size (uint16_t)
 * - expect fatal error (bool)
 * .
 */
class UpdMsgHeaderWithParam :   public srmsgTest,
                                public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, srcty_SafetyCodeType, bool, uint16_t, bool>>
{
    public:
    srtyp_SrMessageHeaderUpdate message_header_update = {   std::get<0>(GetParam()),
                                                            std::get<1>(GetParam())};       ///< message header update
    srcty_SafetyCodeType safety_code_type =                 std::get<2>(GetParam());        ///< safety code type
    bool verify_message_size =                              std::get<3>(GetParam());        ///< enable message size check
    uint16_t assert_message_size =                          std::get<4>(GetParam());        ///< message size to test assert
    bool expect_fatal_error =                               std::get<5>(GetParam());        ///< expect a fatal error
};

/**
 * @brief Test class for value-parametrized check message test
 *
 * Parameter order:
 * - message type (srtyp_SrMessageType)
 * - message size (uint16_t)
 * - message error type (uint8_t)
 * - md4 failed byte position (uint8_t)
 * - value to corrupt data (uint8_t)
 * - safety code type (srcty_SafetyCodeType)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class CheckMessageWithParam :   public srmsgTest,
                                public testing::WithParamInterface<std::tuple<srtyp_SrMessageType, uint16_t, uint8_t, uint8_t, uint8_t, srcty_SafetyCodeType, radef_RaStaReturnCode>>
{
    public:
    srtyp_SrMessageType message_type =          std::get<0>(GetParam());        ///< message type
    uint16_t message_size =                     std::get<1>(GetParam());        ///< message size
    uint8_t error_code =                        std::get<2>(GetParam());        ///< error code
    uint8_t md4_failed_byte =                   std::get<3>(GetParam());        ///< md4 corrupted byte number
    uint8_t value_delta =                       std::get<4>(GetParam());        ///< delta to original value
    srcty_SafetyCodeType safety_code_type =     std::get<5>(GetParam());        ///< safety code type
    radef_RaStaReturnCode return_code =         std::get<6>(GetParam());        ///< expected return value
};

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Enum for message error types
 *
 * These values are used in the createTestMessage function to create normal
 * or corrupted test messages.
 */
enum messageErrorType{
    E_MESSAGE_ERROR_NONE,
    E_MESSAGE_ERROR_MD4_BYTE,
    E_MESSAGE_ERROR_MESSAGE_TYPE,
    E_MESSAGE_ERROR_MESSAGE_PAYLOAD_SIZE,
    E_MESSAGE_ERROR_MESSAGE_SIZES_NOT_EQUAL,
    E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH,
};

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/**
 * @brief Create a Test Message
 *
 * This function creates a test message of a specific message type with given safety code length.
 * It also allows to inject an error of a specific value with given delta.
 *
 * @param msg               pointer to the message to be created
 * @param type              type of the message to be created
 * @param safetyCodeLen     length of the safety code (0, 8 or 16 bytes)
 * @param errorType         error to be injected (use enum messageErrorType)
 * @param md4Bytepos        position of the md4 safety code
 * @param delta             delta between corrupted and original value
 * @return message length (uint16_t)
 */
uint16_t createTestMessage(srtyp_SrMessage * msg, srtyp_SrMessageType type, uint8_t safetyCodeLen, uint8_t errorType, uint8_t md4Bytepos, int8_t delta);

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_messages
 * @{
 */

/**
 * @test        @ID{srmsgTest009} Verify the updateMessageHeader function.
 *
 * @details     This test verifies the correct behaviour of the updateMessageHeader function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - create a test message
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the updateMessageHeader function with given test parameter
 * - verify that the update is applied and nothing else has been changed
 * .
 *
 * @testParameter
 * - Conf. Seq. Number: Confirmed sequence number to update header with
 * - Timestamp: Timestamp to update header with
 * - Safety Code Type: Configured safety code type
 * - Verify msg size: Indicates if message size gets verified
 * - Assert msg size: Expected message size to verify
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter               || Test config                       || Expected values                                       |||
 * |----------|-------------------|------------|------------------|-----------------|-----------------|--------------------|------------------|
 * |          | Conf. Seq. Number | Timestamp  | Safety Code Type | Verify msg size | Assert msg size | Expect fatal error | Test Result      |
 * | 0        | 0                 | 0          | None             | True            | 27              | True               | Fatal error      |
 * | 1        | 0                 | 0          | None             | True            | 28              | False              | Normal Operation |
 * | 2        | 0                 | 0          | Full MD4         | True            | 1102            | True               | Fatal error      |
 * | 3        | 0                 | 0          | Lower MD4        | False           | 0               | False              | Normal Operation |
 * | 4        | 0                 | 0          | Full MD4         | False           | 0               | False              | Normal Operation |
 * | 5        | 0                 | 0          | None             | False           | 0               | False              | Normal Operation |
 * | 6        | 0xffffffff        | 0          | Lower MD4        | False           | 0               | False              | Normal Operation |
 * | 7        | 0                 | 0xffffffff | Lower MD4        | False           | 0               | False              | Normal Operation |
 * | 8        | 0xffffffff        | 0xffffffff | Lower MD4        | False           | 0               | False              | Normal Operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-630} Update Message Header Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-183} Disconnection Request Message Structure
 * @verifyReq{RASW-184} Message Length
 * @verifyReq{RASW-185} Detailed Information regarding the Reason for the Disconnection Request
 * @verifyReq{RASW-186} Reason for Disconnection Request
 */
TEST_P(UpdMsgHeaderWithParam, srmsgTest009VerifyUpdateMessageHeader)
{
    uint16_t i = 0;      // loop iterator

    // Expected Values for a data message (biggest message size)
    const uint16_t expected_message_type            = 6240;             // according to RASW-161
    const uint16_t expected_message_length_nosafety = 30;               // according to RASW-192 (only overhead, without payload)
    const uint16_t expected_message_length_default  = 38;               // according to RASW-192 (only overhead, without payload)
    const uint16_t expected_message_length_fullmd4  = 46;               // according to RASW-192 (only overhead, without payload)

    const uint8_t expected_payload_size_bytepos     = 28;               // according to RASW-191
    const uint8_t expected_payload_bytepos          = 30;               // according to RASW-191

    // create reference message
    const uint32_t expected_receiver_id                     = 0x11223344;
    const uint32_t expected_sender_id                       = 0x22334455;
    const uint32_t expected_sequence_number                 = 0x33445566;
    const uint32_t previous_conf_seq_number                 = 0x44556677;
    const uint32_t previous_timestamp                       = 0x55667788;
    const uint32_t expected_conf_timestamp                  = 0x66778899;
    const uint16_t expected_payload_size                    = UT_SRMSG_DATA_LEN_MAX;
    const srcty_Md4InitValue expected_md4_init_value        = {0x11223344, 0x55667788, 0x9900AABB, 0xCCDDEEFF};
    const uint16_t expected_safety_code_bytepos             = expected_payload_bytepos + expected_payload_size;
    const uint8_t expected_safety_code[UT_SRMSG_SAFETY_CODE_MAX_LEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    // test variables
    uint16_t expected_message_length = 0;
    uint8_t expected_safety_code_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_length_default + expected_payload_size;
        expected_safety_code_length = UT_SRMSG_SAFETY_CODE_HALF_LEN;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_length_fullmd4 + expected_payload_size;
        expected_safety_code_length = UT_SRMSG_SAFETY_CODE_MAX_LEN;
    }
    else {
        expected_message_length = expected_message_length_nosafety + expected_payload_size;
        expected_safety_code_length = UT_SRMSG_SAFETY_CODE_NONE_LEN;
    }

    if (verify_message_size)
    {
        // set length of test message according to the test parameter
        expected_message_length = assert_message_size;
    }

    srtyp_SrMessage message = {0, {}};

    // set test message
    message.message_size = expected_message_length;
    setDataInBuffer_u16(&message.message, expected_message_length, expected_message_length_pos);
    setDataInBuffer_u16(&message.message, expected_message_type, expected_message_type_pos);
    setDataInBuffer_u32(&message.message, expected_receiver_id, expected_receiver_id_pos);
    setDataInBuffer_u32(&message.message, expected_sender_id, expected_sender_id_pos);
    setDataInBuffer_u32(&message.message, expected_sequence_number, expected_sequence_nbr_pos);
    setDataInBuffer_u32(&message.message, previous_conf_seq_number, expected_conf_seq_nbr_pos);
    setDataInBuffer_u32(&message.message, previous_timestamp, expected_timestamp_pos);
    setDataInBuffer_u32(&message.message, expected_conf_timestamp, expected_conf_timestamp_pos);
    setDataInBuffer_u16(&message.message, expected_payload_size, expected_payload_size_bytepos);
    for (i = 0; i < expected_payload_size; i++) {
        setDataInBuffer_u8(&message.message, test_payload_data_u8[i], (expected_payload_bytepos + i));
    }

    // initialize the module
    srmsg_Init(safety_code_type, expected_md4_init_value);        // initialize module with given safety code and MD4 initial values = expectedMd4InitValue

    if(expect_fatal_error) {
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_UpdateMessageHeader(message_header_update, &message), std::invalid_argument);
    }
    else {
        // Expect MD4 function to be called
        if (safety_code_type != srcty_kSafetyCodeTypeNone) {
            EXPECT_CALL(srmd4_mock, srmd4_CalculateMd4(_, (expected_message_length_nosafety + expected_payload_size), &(message.message[0]), _))
                .Times(1)
                .WillOnce(checkMd4InitAndProvideResult(expected_md4_init_value, (uint8_t *)expected_safety_code));
        }
        else {
            EXPECT_CALL(srmd4_mock, srmd4_CalculateMd4(_, _, _, _)).Times(0);
        }

        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_UpdateMessageHeader(message_header_update, &message);

        // verify the output of the function (only conf. sequence number and timestamp must be updated)
        EXPECT_EQ((expected_message_length), message.message_size);
        EXPECT_EQ((expected_message_length), getDataFromBuffer_u16(&message.message, expected_message_length_pos));
        EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
        EXPECT_EQ(expected_receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
        EXPECT_EQ(expected_sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
        EXPECT_EQ(expected_sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
        EXPECT_EQ(message_header_update.confirmed_sequence_number, getDataFromBuffer_u32(&message.message, expected_conf_seq_nbr_pos));
        EXPECT_EQ(message_header_update.time_stamp, getDataFromBuffer_u32(&message.message, expected_timestamp_pos));
        EXPECT_EQ(expected_conf_timestamp, getDataFromBuffer_u32(&message.message, expected_conf_timestamp_pos));
        EXPECT_EQ(expected_payload_size, getDataFromBuffer_u16(&message.message, expected_payload_size_bytepos));
        for (i = 0; i < expected_payload_size; i++) {
            EXPECT_EQ(test_payload_data_u8[i], getDataFromBuffer_u8(&message.message, (expected_payload_bytepos + i)));
        }
        for (i = 0; i < expected_safety_code_length; i++) {
            EXPECT_EQ(expected_safety_code[i], getDataFromBuffer_u8(&message.message, (expected_safety_code_bytepos + i)));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyUpdateMessageHeader,
    UpdMsgHeaderWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, srcty_kSafetyCodeTypeNone, true, (UT_SRMSG_MESSAGE_LEN_MIN - 1), true),
        std::make_tuple(0, 0, srcty_kSafetyCodeTypeNone, true, UT_SRMSG_MESSAGE_LEN_MIN, false),
        std::make_tuple(0, 0, srcty_kSafetyCodeTypeFullMd4, true, (UT_SRMSG_MESSAGE_LEN_MAX + 1), true),
        std::make_tuple(0, 0, srcty_kSafetyCodeTypeLowerMd4, false, 0, false),
        std::make_tuple(0, 0, srcty_kSafetyCodeTypeFullMd4, false, 0, false),
        std::make_tuple(0, 0, srcty_kSafetyCodeTypeNone, false, 0, false),
        std::make_tuple(UINT32_MAX, 0, srcty_kSafetyCodeTypeLowerMd4, false, 0, false),
        std::make_tuple(0, UINT32_MAX, srcty_kSafetyCodeTypeLowerMd4, false, 0, false),
        std::make_tuple(UINT32_MAX, UINT32_MAX, srcty_kSafetyCodeTypeLowerMd4, false, 0, false)
    ));

/**
 * @test        @ID{srmsgTest010} Verify the checkMessage function.
 *
 * @details     This test verifies the correct behaviour of the checkMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - create a test message with given test parameter
 * - verify the correctness of the return value
 * .
 *
 * @testParameter
 * - Message size: Message size configured in message (valid range: (28 + safety code length) - 1101)
 * - Message Type: Configured type of message to check
 * - Error Type: Error type to use when generating message
 * - MD4 Byte: Position of md4 corrupted byte (only relevant when using error type MD4 Byte)
 * - Delta: Delta to original value
 * - Safety Code Type: Configured safety code
 * - Exp. Return Code: Expected return value from srmsg_CheckMessage function call
 * .
 * | Test Run | Input parameter               || Test config                                            |||| Expected values                               ||
 * |----------|--------------|-----------------|--------------------|-----------|-------|------------------|---------------------------|--------------------|
 * |          | Message Size | Message Type    | Error Type         | MD4 Byte  | Delta | Safety Code Type | Exp. Return Code          | Test Result        |
 * |          | vary safety code type                                                                                            |||||||                    |
 * | 0        | 1093         | Data            | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 1        | 1101         | Data            | None               | 0         | 0     | Full MD4         | radef_kNoError            | Normal Operation   |
 * | 2        | 1085         | Data            | None               | 0         | 0     | None             | radef_kNoError            | Normal Operation   |
 * |          | vary md4 failed byte position                                                                                    |||||||                    |
 * | 3        | 1101         | Data            | MD4 Byte           | 0         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 4        | 1101         | Data            | MD4 Byte           | 1         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 5        | 1101         | Data            | MD4 Byte           | 2         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 6        | 1101         | Data            | MD4 Byte           | 3         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 7        | 1101         | Data            | MD4 Byte           | 4         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 8        | 1101         | Data            | MD4 Byte           | 5         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 9        | 1101         | Data            | MD4 Byte           | 6         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 10       | 1101         | Data            | MD4 Byte           | 7         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 11       | 1101         | Data            | MD4 Byte           | 8         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 12       | 1101         | Data            | MD4 Byte           | 9         | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 13       | 1101         | Data            | MD4 Byte           | 10        | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 14       | 1101         | Data            | MD4 Byte           | 11        | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 15       | 1101         | Data            | MD4 Byte           | 12        | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 16       | 1101         | Data            | MD4 Byte           | 13        | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 17       | 1101         | Data            | MD4 Byte           | 14        | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 18       | 1101         | Data            | MD4 Byte           | 15        | 1     | Full MD4         | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 19       | 1093         | Data            | MD4 Byte           | 0         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 20       | 1093         | Data            | MD4 Byte           | 1         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 21       | 1093         | Data            | MD4 Byte           | 2         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 22       | 1093         | Data            | MD4 Byte           | 3         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 23       | 1093         | Data            | MD4 Byte           | 4         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 24       | 1093         | Data            | MD4 Byte           | 5         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 25       | 1093         | Data            | MD4 Byte           | 6         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 26       | 1093         | Data            | MD4 Byte           | 7         | 1     | Lower MD4        | radef_kInvalidMessageMd4  | Normal Operation   |
 * | 27       | 1093         | Data            | MD4 Byte           | 8         | 1     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 28       | 1085         | Data            | MD4 Byte           | 0         | 1     | None             | radef_kNoError            | Normal Operation   |
 * |          | vary message type and error "invalid message type"                                                               |||||||                    |
 * | 29       | 1093         | ConnReq         | Message Type       | 0         | -1    | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 30       | 1093         | ConnReq         | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 31       | 1093         | ConnResp        | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 32       | 1093         | ConnResp        | Message Type       | 0         | 1     | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 33       | 1093         | RetrReq         | Message Type       | 0         | -1    | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 34       | 1093         | RetrReq         | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 35       | 1093         | RetrResp        | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 36       | 1093         | RetrResp        | Message Type       | 0         | 1     | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 37       | 1093         | DiscReq         | Message Type       | 0         | -1    | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 38       | 1093         | DiscReq         | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 39       | 1093         | DiscReq         | Message Type       | 0         | 1     | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 40       | 1093         | Heartbeat       | Message Type       | 0         | -1    | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 41       | 1093         | Heartbeat       | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 42       | 1093         | Heartbeat       | Message Type       | 0         | 1     | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 43       | 1093         | Data            | Message Type       | 0         | -1    | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 44       | 1093         | Data            | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 45       | 1093         | RetrData        | None               | 0         | 0     | Lower MD4        | radef_kNoError            | Normal Operation   |
 * | 46       | 1093         | RetrData        | Message Type       | 0         | 1     | Lower MD4        | radef_kInvalidMessageType | Normal Operation   |
 * | 47       | 1093         | Data            | Sizes Not Equal    | 0         | 1     | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * |          | vary message error type                                                                                          |||||||                    |
 * | 48       | 1093         | ConnReq         | Size Type Mismatch | 0         | 1     | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 49       | 1093         | ConnResp        | Size Type Mismatch | 0         | 1     | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 50       | 1093         | RetrReq         | Size Type Mismatch | 0         | 1     | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 51       | 1093         | RetrResp        | Size Type Mismatch | 0         | 1     | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 52       | 1093         | DiscReq         | Size Type Mismatch | 0         | 1     | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 53       | 1093         | Heartbeat       | Size Type Mismatch | 0         | 1     | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 54       | 1093         | Data            | Size Type Mismatch | 0         | -1    | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 55       | 1093         | Retr Data       | Size Type Mismatch | 0         | -1    | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * |          | vary message size                                                                                                |||||||                    |
 * | 56       | 28           | Data            | None               | 0         | 0     | None             | radef_kNoError            | Normal Operation   |
 * | 57       | 27           | Data            | None               | 0         | 0     | None             | radef_kInvalidParameter   | Fatal error raised |
 * | 58       | 1086         | Data            | None               | 0         | 0     | None             | radef_kInvalidParameter   | Fatal error raised |
 * | 59       | 35           | Data            | None               | 0         | 0     | Lower MD4        | radef_kInvalidParameter   | Fatal error raised |
 * | 60       | 1094         | Data            | None               | 0         | 0     | Lower MD4        | radef_kInvalidParameter   | Fatal error raised |
 * | 61       | 43           | Data            | None               | 0         | 0     | Full MD4         | radef_kInvalidParameter   | Fatal error raised |
 * | 62       | 1102         | Data            | None               | 0         | 0     | Full MD4         | radef_kInvalidParameter   | Fatal error raised |
 * |          | vary message payload size                                                                                                |||||||                    |
 * | 63       | 1085         | Data            | Payload Size       | 0         | -1    | None             | radef_kInvalidMessageSize | Normal Operation   |
 * | 64       | 1093         | Data            | Payload Size       | 0         | -1    | Lower MD4        | radef_kInvalidMessageSize | Normal Operation   |
 * | 65       | 1101         | Data            | Payload Size       | 0         | -1    | Full MD4         | radef_kInvalidMessageSize | Normal Operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-616} Check Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-168} Safety Code
 */
TEST_P(CheckMessageWithParam, srmsgTest010VerifyCheckMessage)
{
    const srcty_Md4InitValue expected_md4_init_value                = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};
    const uint8_t expected_safety_code[UT_SRMSG_SAFETY_CODE_MAX_LEN] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    // test variables
    srtyp_SrMessage message = {message_size, {}};
    bool message_size_valid = true;
    uint8_t expected_safety_code_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_safety_code_length = UT_SRMSG_SAFETY_CODE_HALF_LEN;
        message_size_valid = (message_size >= UT_SRMSG_MESSAGE_LEN_MIN_HALF_MD4) && (message_size <= UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4);
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_safety_code_length = UT_SRMSG_SAFETY_CODE_MAX_LEN;
        message_size_valid = (message_size >= UT_SRMSG_MESSAGE_LEN_MIN_FULL_MD4) && (message_size <= UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4);
    }
    else {
        expected_safety_code_length = UT_SRMSG_SAFETY_CODE_NONE_LEN;
        message_size_valid = (message_size >= UT_SRMSG_MESSAGE_LEN_MIN_NONE_MD4) && (message_size <= UT_SRMSG_MESSAGE_LEN_MAX_NONE_MD4);
    }

    uint16_t message_len = 0;

    // initialize the module
    srmsg_Init(safety_code_type, expected_md4_init_value);        // safety code lower MD4 and MD4 initial values = expectedMd4InitValue

    if(message_size_valid){
        // create a test message
        message_len = createTestMessage(&message, message_type, expected_safety_code_length, error_code, md4_failed_byte, value_delta);

        // Expect MD4 function to be called
        if (safety_code_type != srcty_kSafetyCodeTypeNone && return_code != radef_kInvalidParameter) {
            EXPECT_CALL(srmd4_mock, srmd4_CalculateMd4(_, (message_len - expected_safety_code_length), &(message.message[0]), _))
                .Times(1)
                .WillOnce(checkMd4InitAndProvideResult(expected_md4_init_value, (uint8_t *)expected_safety_code));
        }
    }
    else {
        EXPECT_CALL(srmd4_mock, srmd4_CalculateMd4(_, _, _, _)).Times(0);
    }

    // message.message_size = message_size;

    if( return_code == radef_kInvalidParameter) {
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_CheckMessage(&message), std::invalid_argument);
    }
    else {
        // call the function and verify the return code
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_EQ(return_code, srmsg_CheckMessage(&message));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyCheckMessageFunction,
    CheckMessageWithParam,
    ::testing::Values(
        // vary safety code type
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeFullMd4,   radef_kNoError),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_NONE_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeNone,      radef_kNoError),
        // vary md4 failed byte position
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   0, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   1, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   2, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   3, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   4, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   5, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   6, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   7, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   8, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   9, 1,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   10, 1,  srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   11, 1,  srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   12, 1,  srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   13, 1,  srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   14, 1,  srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   15, 1,  srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   1, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   2, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   3, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   4, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   5, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   6, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   7, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageMd4),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   8, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_NONE_MD4,          E_MESSAGE_ERROR_MD4_BYTE,                   0, 1,   srcty_kSafetyCodeTypeNone,      radef_kNoError),
        // vary message type and error "invalid message type"
        std::make_tuple(srtyp_kSrMessageConnReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageConnReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageConnResp,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageConnResp,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageRetrReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageRetrReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageRetrResp,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageRetrResp,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageDiscReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageDiscReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageDiscReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageHb,         UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageHb,         UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageHb,         UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageRetrData,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kNoError),
        std::make_tuple(srtyp_kSrMessageRetrData,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_TYPE,               0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageType),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZES_NOT_EQUAL,    0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        // vary message error type
        std::make_tuple(srtyp_kSrMessageConnReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageConnResp,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageRetrReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageRetrResp,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageDiscReq,    UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageHb,         UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, 1,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageRetrData,   UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH, 0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        // vary message size
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MIN_NONE_MD4,          E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeNone,      radef_kNoError),
        std::make_tuple(srtyp_kSrMessageData,       (UT_SRMSG_MESSAGE_LEN_MIN_NONE_MD4 - 1),    E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeNone,      radef_kInvalidParameter),
        std::make_tuple(srtyp_kSrMessageData,       (UT_SRMSG_MESSAGE_LEN_MAX_NONE_MD4 + 1),    E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeNone,      radef_kInvalidParameter),
        std::make_tuple(srtyp_kSrMessageData,       (UT_SRMSG_MESSAGE_LEN_MIN_HALF_MD4 - 1),    E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidParameter),
        std::make_tuple(srtyp_kSrMessageData,       (UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4 + 1),    E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidParameter),
        std::make_tuple(srtyp_kSrMessageData,       (UT_SRMSG_MESSAGE_LEN_MIN_FULL_MD4 - 1),    E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidParameter),
        std::make_tuple(srtyp_kSrMessageData,       (UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4 + 1),    E_MESSAGE_ERROR_NONE,                       0, 0,   srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidParameter),
        // vary message payload size
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_NONE_MD4,          E_MESSAGE_ERROR_MESSAGE_PAYLOAD_SIZE,       0, -1,  srcty_kSafetyCodeTypeNone,      radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_HALF_MD4,          E_MESSAGE_ERROR_MESSAGE_PAYLOAD_SIZE,       0, -1,  srcty_kSafetyCodeTypeLowerMd4,  radef_kInvalidMessageSize),
        std::make_tuple(srtyp_kSrMessageData,       UT_SRMSG_MESSAGE_LEN_MAX_FULL_MD4,          E_MESSAGE_ERROR_MESSAGE_PAYLOAD_SIZE,       0, -1,  srcty_kSafetyCodeTypeFullMd4,   radef_kInvalidMessageSize)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

uint16_t createTestMessage(srtyp_SrMessage * msg, srtyp_SrMessageType type, uint8_t safetyCodeLen, uint8_t errorType, uint8_t md4Bytepos, int8_t delta){

    uint16_t retVal = 0;                            // return value (message length)

    const uint16_t kMinMsgLengthConnReqResp = 42;   // Minimum message length for connection request & response message [byte]
    const uint16_t kMinMsgLengthRetrReqResp = 28;   // Minimum message length for retransmission request & response message [byte]
    const uint16_t kMinMsgLengthDiscReq = 32;       // Minimum message length for disconnection request message [byte]
    const uint16_t kMinMsgLengthHeartbeat = 28;     // Minimum message length for heartbeat message [byte]
    const uint16_t kMinMsgLengthEmptyDataMsg = 30;  // Minimum message length for an empty data or retransmitted data message [byte]

    const uint16_t kPayloadBytepos = 28;            // Byte position of payload data in a PDU message
    const uint16_t kPayloadLengthConnReqResp = 14;  // Length of the payload data for Connection Request / Response messages
    const uint16_t kPayloadLengthRetrReqResp = 0;   // Length of the payload data for Retransmission Request / Response messages
    const uint16_t kPayloadLengthDiscReq = 4;       // Length of the payload data for Disconnection Request messages
    const uint16_t kPayloadLengthData = 1055;       // Length of the payload data for Data messages
    const uint16_t kPayloadSizeDataMsg = 2;         // Length of the payload size for Data messages
    const uint16_t kPayloadLengthHeartbeat = 0;     // Length of the payload data for Heartbeat messages


    const uint8_t expectedSafetyCode[UT_SRMSG_SAFETY_CODE_MAX_LEN]  = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    uint32_t messageLen = 0;
    uint16_t safetyCodeBytepos = 0;

    // setup correct message
    switch (type) {
        case srtyp_kSrMessageConnReq:
        case srtyp_kSrMessageConnResp:
            messageLen = kMinMsgLengthConnReqResp + safetyCodeLen;
            safetyCodeBytepos = kPayloadBytepos + kPayloadLengthConnReqResp;
            break;
        case srtyp_kSrMessageRetrReq:
        case srtyp_kSrMessageRetrResp:
            messageLen = kMinMsgLengthRetrReqResp + safetyCodeLen;
            safetyCodeBytepos = kPayloadBytepos + kPayloadLengthRetrReqResp;
            break;
        case srtyp_kSrMessageDiscReq:
            messageLen = kMinMsgLengthDiscReq + safetyCodeLen;
            safetyCodeBytepos = kPayloadBytepos + kPayloadLengthDiscReq;
            break;
        case srtyp_kSrMessageHb:
            messageLen = kMinMsgLengthHeartbeat + safetyCodeLen;
            safetyCodeBytepos = kPayloadBytepos + kPayloadLengthHeartbeat;
            break;
        case srtyp_kSrMessageData:
        case srtyp_kSrMessageRetrData:
            messageLen = kMinMsgLengthEmptyDataMsg + kPayloadLengthData + safetyCodeLen;
            safetyCodeBytepos = kPayloadBytepos + kPayloadLengthData + kPayloadSizeDataMsg;
            setDataInBuffer_u16(msg->message, kPayloadLengthData, kPayloadBytepos);
            break;
        default:
            // nothing to do here
            break;
    }

    msg->message_size = messageLen;
    setDataInBuffer_u16(msg->message, messageLen, srmsgTest::expected_message_length_pos);
    setDataInBuffer_u16(msg->message, type, srmsgTest::expected_message_type_pos);
    retVal = messageLen;

    for (uint16_t i = 0; i < safetyCodeLen; i++) {
        if (errorType == E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH) {
            setDataInBuffer_u8(msg->message, expectedSafetyCode[i], (safetyCodeBytepos + i + delta));
        }
        else {
            setDataInBuffer_u8(msg->message, expectedSafetyCode[i], (safetyCodeBytepos + i));
        }
    }

    // corrupt message
    switch (errorType)
    {
        case E_MESSAGE_ERROR_MD4_BYTE:
            setDataInBuffer_u8(msg->message, (expectedSafetyCode[md4Bytepos] + delta), (safetyCodeBytepos + md4Bytepos));
            break;
        case E_MESSAGE_ERROR_MESSAGE_TYPE:
            setDataInBuffer_u16(msg->message, (uint16_t)(type + delta), srmsgTest::expected_message_type_pos);
            break;
        case E_MESSAGE_ERROR_MESSAGE_PAYLOAD_SIZE:
            setDataInBuffer_u16(msg->message, (kPayloadLengthData + delta), kPayloadBytepos);
            break;
        case E_MESSAGE_ERROR_MESSAGE_SIZES_NOT_EQUAL:
            setDataInBuffer_u16(msg->message, (uint16_t)(messageLen + delta), srmsgTest::expected_message_length_pos);
            break;
        case E_MESSAGE_ERROR_MESSAGE_SIZE_TYPE_MISMATCH:
            msg->message_size = (uint16_t)(messageLen + delta);
            setDataInBuffer_u16(msg->message, (uint16_t)(messageLen + delta), srmsgTest::expected_message_length_pos);
            retVal = (uint16_t)(messageLen + delta);
            break;
        default:
            // do nothing here
            break;
    }

    return retVal;
}
