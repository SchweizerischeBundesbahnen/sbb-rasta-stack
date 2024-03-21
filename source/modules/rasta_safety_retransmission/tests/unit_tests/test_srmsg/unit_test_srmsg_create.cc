/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srmsg_create.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version c43d3a924e35983d1734435fa2bbfbffb1750d0c
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4949: remove endianness switch in component tests,08.12.2022,M. Kuhn}
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
 * @brief Test class for value-parametrized connection request message test
 *
 * Parameter order:
 * - messageHeader.receiver_id (uint32_t)
 * - messageHeader.sender_id (uint32_t)
 * - messageHeader.sequence_number (uint32_t)
 * - messageHeader.confirmed_timestamp (uint32_t)
 * - protocolVersion[0] (uint8_t)
 * - protocolVersion[1] (uint8_t)
 * - protocolVersion[2] (uint8_t)
 * - protocolVersion[3] (uint8_t)
 * - nSendMax (uint16_t)
 * - safety code type (srcty_SafetyCodeType)
 * - expect fatal error (bool)
 * .
 */
class ConnReqMsgWithParam : public srmsgTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t, srcty_SafetyCodeType, bool>>
{
    public:

    srtyp_SrMessageHeaderCreate message_header = {  std::get<0>(GetParam()),
                                                    std::get<1>(GetParam()),
                                                    std::get<2>(GetParam()),
                                                    std::get<3>(GetParam())};   ///< message header
    srtyp_ProtocolVersion protocol_version =     {  std::get<4>(GetParam()),
                                                    std::get<5>(GetParam()),
                                                    std::get<6>(GetParam()),
                                                    std::get<7>(GetParam())};   ///< protocol version
    uint16_t nsendmax =                             std::get<8>(GetParam());    ///< nSendMax
    srcty_SafetyCodeType safety_code_type =         std::get<9>(GetParam());    ///< safety code type
    bool expect_fatal_error =                       std::get<10>(GetParam());   ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized connection response message test
 *
 * This class simply inherits from ConnReqMsgWithParam.
 */
class ConnRespMsgWithParam : public ConnReqMsgWithParam
{};

/**
 * @brief Test class for value-parametrized data and retransmission data message test
 *
 * Parameter order:
 * - message_header.receiver_id (uint32_t)
 * - message_header.sender_id (uint32_t)
 * - message_header.sequence_number (uint32_t)
 * - message_header.confirmed_timestamp (uint32_t)
 * - payload size (uint16_t)
 * - payload data pointer (uint8_t *)
 * - safety code type (srcty_SafetyCodeType)
 * - expect fatal error (bool)
 * .
 */
class DataMsgWithParam :    public srmsgTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint16_t, uint8_t *, srcty_SafetyCodeType, bool>>
{
    public:
    srtyp_SrMessageHeaderCreate message_header = {  std::get<0>(GetParam()),
                                                    std::get<1>(GetParam()),
                                                    std::get<2>(GetParam()),
                                                    std::get<3>(GetParam())};       ///< message header
    srtyp_SrMessagePayload message_payload =    {   std::get<4>(GetParam()), {}};   ///< message payload
    uint8_t * data_ptr =                            std::get<5>(GetParam());        ///< pointer to the test data
    srcty_SafetyCodeType safety_code_type =         std::get<6>(GetParam());        ///< safety code type
    bool expect_fatal_error =                       std::get<7>(GetParam());        ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized retransmission request/response message test and heartbeat message test
 *
 * Parameter order:
 * - message_header.receiver_id (uint32_t)
 * - message_header.sender_id (uint32_t)
 * - messageHeader.sequence_number (uint32_t)
 * - message_header.confirmed_timestamp (uint32_t)
 * - safety code type (srcty_SafetyCodeType)
 * .
 */
class RetrReqRespHeartbeatMsgWithParam :    public srmsgTest,
                                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, srcty_SafetyCodeType>>
{
    public:
    srtyp_SrMessageHeaderCreate message_header = {  std::get<0>(GetParam()),
                                                    std::get<1>(GetParam()),
                                                    std::get<2>(GetParam()),
                                                    std::get<3>(GetParam())};       ///< message header
    srcty_SafetyCodeType safety_code_type =         std::get<4>(GetParam());        ///< safety code type
};

/**
 * @brief Test class for value-parametrized disconnection request message test
 *
 * Parameter order:
 * - message_header.receiver_id (uint32_t)
 * - message_header.sender_id (uint32_t)
 * - message_header.sequence_number (uint32_t)
 * - message_header.confirmed_timestamp (uint32_t)
 * - detailed reason (uint16_t)
 * - reason (sraty_DiscReason)
 * - safety code type (srcty_SafetyCodeType)
 * - expect fatal error (bool)
 * .
 */
class DiscReqMsgWithParam : public srmsgTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint16_t, sraty_DiscReason, srcty_SafetyCodeType, bool>>
{
    public:
    srtyp_SrMessageHeaderCreate message_header = {  std::get<0>(GetParam()),
                                                    std::get<1>(GetParam()),
                                                    std::get<2>(GetParam()),
                                                    std::get<3>(GetParam())};       ///< message header
    uint16_t detailed_reason =                      std::get<4>(GetParam());        ///< detailed reason
    sraty_DiscReason reason =                       std::get<5>(GetParam());        ///< disconnection reason
    srcty_SafetyCodeType safety_code_type =         std::get<6>(GetParam());        ///< safety code type
    bool expect_fatal_error =                       std::get<7>(GetParam());        ///< expect fatal error
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_messages
 * @{
 */

/**
 * @test        @ID{srmsgTest001} Verify the createConnReqMessage function.
 *
 * @details     This test verifies the correct behaviour of the createConnReqMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. TS: Confirmed time stamp in miliseconds
 * - Ver.[0]: Version position 1
 * - Ver.[1]: Version position 2
 * - Ver.[2]: Version position 3
 * - Ver.[3]: Version position 4
 * - nSendMax: Nsendmax (receive buffer size) (valid range: 2 - 20)
 * - Safety Code Type: Configured Safety code type
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                                                             ||||||||| Test config      | Expected values                  ||
 * |----------|------------|------------|-------------|----------|---------|---------|---------|---------|----------|------------------|--------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number | Conf. TS | Ver.[0] | Ver.[1] | Ver.[2] | Ver.[3] | nSendMax | Safety Code Type | Expect Fatal | Test Result        |
 * | 0        | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 1        | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Full MD4         | False        | Normal operation   |
 * | 2        | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 2        | None             | False        | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff  | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 6        | 0          | 0          | 0           | 1        | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 7        | 0          | 0          | 0           | 0        | 0x29    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 8        | 0          | 0          | 0           | 0        | 0x39    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 9        | 0          | 0          | 0           | 0        | 0x40    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 10       | 0          | 0          | 0           | 0        | 0x30    | 0x29    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 11       | 0          | 0          | 0           | 0        | 0x30    | 0x39    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 12       | 0          | 0          | 0           | 0        | 0x30    | 0x40    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 13       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x29    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 14       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x39    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 15       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x40    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 16       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x29    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 17       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x39    | 2        | Lower MD4        | False        | Normal operation   |
 * | 18       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x40    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 19       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 1        | Lower MD4        | True         | Fatal error raised |
 * | 20       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 20       | Lower MD4        | False        | Normal operation   |
 * | 21       | 0          | 0          | 0           | 0        | 0x30    | 0x30    | 0x30    | 0x30    | 21       | Lower MD4        | True         | Fatal error raised |
 * | 22       | 0xffffffff | 0xffffffff | 0xffffffff  | 0        | 0x39    | 0x39    | 0x39    | 0x39    | 20       | Lower MD4        | False        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-617} Create Connection Request Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-170} Connection Request Message Structure
 * @verifyReq{RASW-172} Message Length
 * @verifyReq{RASW-173} Protocol Version
 * @verifyReq{RASW-174} Nsendmax
 * @verifyReq{RASW-175} Reserve
 */
TEST_P(ConnReqMsgWithParam, srmsgTest001VerifyCreateConnReqMessage)
{
    uint8_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6200;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 50;               // according to RASW-172
    const uint16_t expected_message_lengthNoSafety  = 42;               // according to RASW-172
    const uint16_t expected_message_lengthFullMd4   = 58;               // according to RASW-172

    const uint8_t expected_nsendmax_bytepos         = 32;               // according to RASW-170
    const uint8_t expected_protocol_version_bytepos = 28;               // according to RASW-170
    const uint8_t expected_protocol_version_length  = 4;                // according to RASW-170
    const uint8_t expected_reserve_bytepos          = 34;               // according to RASW-170
    const uint8_t expected_reserve_length           = 8;                // according to RASW-170
    const uint8_t expected_reserve_value            = 0;                // according to RASW-175
    const uint32_t expected_conf_seq_number         = 0;                // according to RASW-170
    const uint32_t expected_conf_timestamp          = 0;                // according to RASW-170

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    if (expect_fatal_error) {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_CreateConnReqMessage(message_header, protocol_version, nsendmax, &message), std::invalid_argument);
    }
    else {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_CreateConnReqMessage(message_header, protocol_version, nsendmax, &message);

        // verify the output of the function
        EXPECT_EQ(expected_message_length, message.message_size);
        EXPECT_EQ(expected_message_length, getDataFromBuffer_u16(&message.message, expected_message_length_pos));
        EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
        EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
        EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
        EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
        EXPECT_EQ(expected_conf_seq_number, getDataFromBuffer_u32(&message.message, expected_conf_seq_nbr_pos));
        EXPECT_EQ(expected_conf_timestamp, getDataFromBuffer_u32(&message.message, expected_conf_timestamp_pos));
        for (i = 0; i < expected_protocol_version_length; i++) {
            EXPECT_EQ(protocol_version.version[i], getDataFromBuffer_u8(&message.message, (expected_protocol_version_bytepos + i)));
        }
        EXPECT_EQ(nsendmax, getDataFromBuffer_u16(&message.message, expected_nsendmax_bytepos));
        for (i = 0; i < expected_reserve_length; i++) {
            EXPECT_EQ(expected_reserve_value, getDataFromBuffer_u32(&message.message, (expected_reserve_bytepos + i)));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyConnReqMessage,
    ConnReqMsgWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeFullMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeNone, false),
        std::make_tuple(UINT32_MAX, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, UINT32_MAX, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, UINT32_MAX, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 1, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_NSENDMAX_MIN - 1), srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MAX, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_NSENDMAX_MAX + 1), srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, 0, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_NSENDMAX_MAX, srcty_kSafetyCodeTypeLowerMd4, false)
    ));

/**
 * @test        @ID{srmsgTest002} Verify the createConnRespMessage function.
 *
 * @details     This test verifies the correct behaviour of the createConnRespMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. Timestamp: Confirmed time stamp in miliseconds
 * - Ver.[0]: Version digit 1
 * - Ver.[1]: Version digit 2
 * - Ver.[2]: Version digit 3
 * - Ver.[3]: Version digit 4
 * - nSendMax: Nsendmax (receive buffer size) (valid range: 2 - 20)
 * - Safety Code Type: Configured Safety code type
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                                                                    ||||||||| Test config      | Expected values                  ||
 * |----------|------------|------------|-------------|-----------------|---------|---------|---------|---------|----------|------------------|--------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number | Conf. Timestamp | Ver.[0] | Ver.[1] | Ver.[2] | Ver.[3] | nSendMax | Safety Code Type | Expect Fatal | Test Result        |
 * | 0        | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 1        | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Full MD4         | False        | Normal operation   |
 * | 2        | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 2        | None             | False        | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff  | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 6        | 0          | 0          | 0           | 0xffffffff      | 0x30    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 7        | 0          | 0          | 0           | 0               | 0x29    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 8        | 0          | 0          | 0           | 0               | 0x39    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 9        | 0          | 0          | 0           | 0               | 0x40    | 0x30    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 10       | 0          | 0          | 0           | 0               | 0x30    | 0x29    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 11       | 0          | 0          | 0           | 0               | 0x30    | 0x39    | 0x30    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 12       | 0          | 0          | 0           | 0               | 0x30    | 0x40    | 0x30    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 13       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x29    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 14       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x39    | 0x30    | 2        | Lower MD4        | False        | Normal operation   |
 * | 15       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x40    | 0x30    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 16       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x29    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 17       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x39    | 2        | Lower MD4        | False        | Normal operation   |
 * | 18       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x40    | 2        | Lower MD4        | True         | Fatal error raised |
 * | 19       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 1        | Lower MD4        | True         | Fatal error raised |
 * | 20       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 20       | Lower MD4        | False        | Normal operation   |
 * | 21       | 0          | 0          | 0           | 0               | 0x30    | 0x30    | 0x30    | 0x30    | 21       | Lower MD4        | True         | Fatal error raised |
 * | 22       | 0xffffffff | 0xffffffff | 0xffffffff  | 0xffffffff      | 0x39    | 0x39    | 0x39    | 0x39    | 20       | Lower MD4        | False        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-618} Create Connection Response Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-171} Connection Response Message Structure
 * @verifyReq{RASW-172} Message Length
 * @verifyReq{RASW-173} Protocol Version
 * @verifyReq{RASW-174} Nsendmax
 * @verifyReq{RASW-175} Reserve
 */
TEST_P(ConnRespMsgWithParam, srmsgTest002VerifyCreateConnRespMessage)
{
    uint8_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6201;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 50;               // according to RASW-172
    const uint16_t expected_message_lengthNoSafety  = 42;               // according to RASW-172
    const uint16_t expected_message_lengthFullMd4   = 58;               // according to RASW-172

    const uint8_t expected_nsendmax_bytepos         = 32;               // according to RASW-171
    const uint8_t expected_protocol_version_bytepos = 28;               // according to RASW-171
    const uint8_t expected_protocol_version_length  = 4;                // according to RASW-171
    const uint8_t expected_reserve_bytepos          = 34;               // according to RASW-171
    const uint8_t expected_reserve_length           = 8;                // according to RASW-171
    const uint8_t expected_reserve_value            = 0;                // according to RASW-175

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    if (expect_fatal_error) {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_CreateConnRespMessage(message_header, protocol_version, nsendmax, &message), std::invalid_argument);
    }
    else {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_CreateConnRespMessage(message_header, protocol_version, nsendmax, &message);

        // verify the output of the function
        EXPECT_EQ(expected_message_length, message.message_size);
        EXPECT_EQ(expected_message_length, getDataFromBuffer_u16(&message.message, expected_message_length_pos));
        EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
        EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
        EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
        EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
        for (i = 0; i < expected_protocol_version_length; i++) {
            EXPECT_EQ(protocol_version.version[i], getDataFromBuffer_u8(&message.message, (expected_protocol_version_bytepos + i)));
        }
        EXPECT_EQ(nsendmax, getDataFromBuffer_u16(&message.message, expected_nsendmax_bytepos));
        for (i = 0; i < expected_reserve_length; i++) {
            EXPECT_EQ(expected_reserve_value, getDataFromBuffer_u32(&message.message, (expected_reserve_bytepos + i)));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyConnRespMessage,
    ConnRespMsgWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeFullMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeNone, false),
        std::make_tuple(UINT32_MAX, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, UINT32_MAX, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, UINT32_MAX, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, UINT32_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MIN - 1), UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_PROT_MAX + 1), UT_SRMSG_NSENDMAX_MIN, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_NSENDMAX_MIN - 1), srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_NSENDMAX_MAX, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, (UT_SRMSG_NSENDMAX_MAX + 1), srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_NSENDMAX_MAX, srcty_kSafetyCodeTypeLowerMd4, false)
    ));

/**
 * @test        @ID{srmsgTest003} Verify the createDataMessage function.
 *
 * @details     This test verifies the correct behaviour of the createDataMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. Ts: Confirmed time stamp in miliseconds
 * - Payload size: Size of message payload in bytes (valid range: 1 - 1055)
 * - Data Pointer: Pointer to buffer which contains the test data
 * - Safety Code Type: Configured Safety code type
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                                                            |||||| Test config      | Expected values                  ||
 * |----------|------------|------------|-------------|------------|--------------|-----------------------------|------------------|--------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number | Conf. TS   | Payload size | Data Pointer                | Safety Code Type | Expect Fatal | Test Result        |
 * | 0        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 1        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8       | Full MD4         | False        | Normal operation   |
 * | 2        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8       | None             | False        | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0           | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0           | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff  | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 6        | 0          | 0          | 0           | 0xffffffff | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 7        | 0          | 0          | 0           | 0          | 0            | &test_payload_data_u8       | Lower MD4        | True         | Fatal error raised |
 * | 8        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8[1054] | Lower MD4        | False        | Normal operation   |
 * | 9        | 0          | 0          | 0           | 0          | 1055         | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 10       | 0          | 0          | 0           | 0          | 1056         | &test_payload_data_u8       | Lower MD4        | True         | Fatal error raised |
 * | 11       | 0xffffffff | 0xffffffff | 0xffffffff  | 0xffffffff | 1055         | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-623} Create Data Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-191} Data Message Structure
 * @verifyReq{RASW-192} Message Length
 * @verifyReq{RASW-193} Size of Message Data
 * @verifyReq{RASW-194} Data Message
 */
TEST_P(DataMsgWithParam, srmsgTest003VerifyCreateDataMessage)
{
    uint16_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6240;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 38;               // according to RASW-192 (only overhead, without payload)
    const uint16_t expected_message_lengthNoSafety  = 30;               // according to RASW-192 (only overhead, without payload)
    const uint16_t expected_message_lengthFullMd4   = 46;               // according to RASW-192 (only overhead, without payload)

    const uint8_t expected_payload_size_bytepos     = 28;               // according to RASW-191
    const uint8_t expected_payload_bytepos          = 30;               // according to RASW-191

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // copy test payload
    for (i = 0; i < message_payload.payload_size; i++) {
        message_payload.payload[i] = data_ptr[i];
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    if (expect_fatal_error) {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_CreateDataMessage(message_header, &message_payload, &message), std::invalid_argument);
    }
    else {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_CreateDataMessage(message_header, &message_payload, &message);

        // verify the output of the function
        EXPECT_EQ((expected_message_length + message_payload.payload_size), message.message_size);
        EXPECT_EQ((expected_message_length + message_payload.payload_size), getDataFromBuffer_u16(&message.message, expected_message_length_pos));
        EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
        EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
        EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
        EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
        EXPECT_EQ(message_payload.payload_size, getDataFromBuffer_u16(&message.message, expected_payload_size_bytepos));
        for (i = 0; i < message_payload.payload_size; i++) {
            EXPECT_EQ(message_payload.payload[i], getDataFromBuffer_u8(&message.message, expected_payload_bytepos + i));
        }
    }
}

/**
 * @test        @ID{srmsgTest004} Verify the createRetrDataMessage function.
 *
 * @details     This test verifies the correct behaviour of the createRetrDataMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. Ts: Confirmed time stamp in miliseconds
 * - Payload size: Size of message payload in bytes (valid range: 1 - 1055)
 * - Data Pointer: Pointer to buffer which contains the test data
 * - Safety Code Type: Configured Safety code type
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                                                            |||||| Test config      | Expected values                  ||
 * |----------|------------|------------|-------------|------------|--------------|-----------------------------|------------------|--------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number | Conf. TS   | Payload size | Data Pointer                | Safety Code Type | Expect Fatal | Test Result        |
 * | 0        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 1        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8       | Full MD4         | False        | Normal operation   |
 * | 2        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8       | None             | False        | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0           | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0           | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff  | 0          | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 6        | 0          | 0          | 0           | 0xffffffff | 1            | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 7        | 0          | 0          | 0           | 0          | 0            | &test_payload_data_u8       | Lower MD4        | True         | Fatal error raised |
 * | 8        | 0          | 0          | 0           | 0          | 1            | &test_payload_data_u8[1054] | Lower MD4        | False        | Normal operation   |
 * | 9        | 0          | 0          | 0           | 0          | 1055         | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 * | 10       | 0          | 0          | 0           | 0          | 1056         | &test_payload_data_u8       | Lower MD4        | True         | Fatal error raised |
 * | 11       | 0xffffffff | 0xffffffff | 0xffffffff  | 0xffffffff | 1055         | &test_payload_data_u8       | Lower MD4        | False        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-624} Create Retransmitted Data Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-196} Retransmitted Data Message Structure
 * @verifyReq{RASW-192} Message Length
 * @verifyReq{RASW-193} Size of Message Data
 * @verifyReq{RASW-194} Data Message
 */
TEST_P(DataMsgWithParam, srmsgTest004VerifyCreateRetrDataMessage)
{
    uint16_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6241;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 38;               // according to RASW-192 (only overhead, without payload)
    const uint16_t expected_message_lengthNoSafety  = 30;               // according to RASW-192 (only overhead, without payload)
    const uint16_t expected_message_lengthFullMd4   = 46;               // according to RASW-192 (only overhead, without payload)

    const uint8_t expected_payload_size_bytepos     = 28;               // according to RASW-196
    const uint8_t expected_payload_bytepos          = 30;               // according to RASW-196

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // copy test payload
    for (i = 0; i < message_payload.payload_size; i++) {
        message_payload.payload[i] = data_ptr[i];
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    if (expect_fatal_error) {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_CreateRetrDataMessage(message_header, &message_payload, &message), std::invalid_argument);
    }
    else {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_CreateRetrDataMessage(message_header, &message_payload, &message);

        // verify the output of the function
        EXPECT_EQ((expected_message_length + message_payload.payload_size), message.message_size);
        EXPECT_EQ((expected_message_length + message_payload.payload_size), getDataFromBuffer_u16(&message.message, expected_message_length_pos));
        EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
        EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
        EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
        EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
        EXPECT_EQ(message_payload.payload_size, getDataFromBuffer_u16(&message.message, expected_payload_size_bytepos));
        for (i = 0; i < message_payload.payload_size; i++) {
            EXPECT_EQ(message_payload.payload[i], getDataFromBuffer_u8(&message.message, expected_payload_bytepos + i));
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyDataMessage,
    DataMsgWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_DATA_LEN_MIN, test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_DATA_LEN_MIN, test_payload_data_u8, srcty_kSafetyCodeTypeFullMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_DATA_LEN_MIN, test_payload_data_u8, srcty_kSafetyCodeTypeNone, false),
        std::make_tuple(UINT32_MAX, 0, 0, 0, UT_SRMSG_DATA_LEN_MIN, test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, UINT32_MAX, 0, 0, UT_SRMSG_DATA_LEN_MIN, test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, UINT32_MAX, 0, UT_SRMSG_DATA_LEN_MIN, test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, UINT32_MAX, UT_SRMSG_DATA_LEN_MIN, test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, (UT_SRMSG_DATA_LEN_MIN - 1), test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_DATA_LEN_MIN, &(test_payload_data_u8[UT_SRMSG_DATA_LEN_MAX - 1]), srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UT_SRMSG_DATA_LEN_MAX, test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, (UT_SRMSG_DATA_LEN_MAX + 1), test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UT_SRMSG_DATA_LEN_MAX, test_payload_data_u8, srcty_kSafetyCodeTypeLowerMd4, false)
    ));

/**
 * @test        @ID{srmsgTest005} Verify the createRetrReqMessage function.
 *
 * @details     This test verifies the correct behaviour of the createRetrReqMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. Ts: Confirmed time stamp in miliseconds
 * - Safety Code Type: Configured Safety code type
 * .
 * | Test Run | Input parameter                                |||| Test config      | Expected values    |
 * |----------|------------|------------|------------|------------|------------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number| Conf. TS   | Safety Code Type | Test Result        |
 * | 0        | 0          | 0          | 0          | 0          | Lower MD4        | Normal operation   |
 * | 1        | 0          | 0          | 0          | 0          | Full MD4         | Normal operation   |
 * | 2        | 0          | 0          | 0          | 0          | None             | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0          | 0          | Lower MD4        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0          | 0          | Lower MD4        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff | 0          | Lower MD4        | Normal operation   |
 * | 6        | 0          | 0          | 0          | 0xffffffff | Lower MD4        | Normal operation   |
 * | 7        | 0xffffffff | 0xffffffff | 0xffffffff | 0xffffffff | Lower MD4        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-619} Create Retransmission Request Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-177} Retransmission Request Message Structure
 * @verifyReq{RASW-178} Message Length
 */
TEST_P(RetrReqRespHeartbeatMsgWithParam, srmsgTest005VerifyCreateRetrReqMessage)
{
    uint8_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6212;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 36;               // according to RASW-178
    const uint16_t expected_message_lengthNoSafety  = 28;               // according to RASW-178
    const uint16_t expected_message_lengthFullMd4   = 44;               // according to RASW-178

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    // call the function
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    srmsg_CreateRetrReqMessage(message_header, &message);

    // verify the output of the function
    EXPECT_EQ(expected_message_length, message.message_size);
    EXPECT_EQ(expected_message_length, getDataFromBuffer_u16(&message.message, expected_message_length_pos));
    EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
    EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
    EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
    EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
}

/**
 * @test        @ID{srmsgTest006} Verify the createRetrRespMessage function.
 *
 * @details     This test verifies the correct behaviour of the createRetrRespMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. Ts: Confirmed time stamp in miliseconds
 * - Safety Code Type: Configured Safety code type
 * .
 * | Test Run | Input parameter                                 |||| Test config      | Expected values    |
 * |----------|------------|------------|-------------|------------|------------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number | Conf. TS   | Safety Code Type | Test Result        |
 * | 0        | 0          | 0          | 0           | 0          | Lower MD4        | Normal operation   |
 * | 1        | 0          | 0          | 0           | 0          | Full MD4         | Normal operation   |
 * | 2        | 0          | 0          | 0           | 0          | None             | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0           | 0          | Lower MD4        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0           | 0          | Lower MD4        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff  | 0          | Lower MD4        | Normal operation   |
 * | 6        | 0          | 0          | 0           | 0xffffffff | Lower MD4        | Normal operation   |
 * | 7        | 0xffffffff | 0xffffffff | 0xffffffff  | 0xffffffff | Lower MD4        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-620} Create Retransmission Response Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-180} Retransmission Response Message Structure
 * @verifyReq{RASW-178} Message Length
 */
TEST_P(RetrReqRespHeartbeatMsgWithParam, srmsgTest006VerifyCreateRetrRespMessage)
{
    uint8_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6213;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 36;               // according to RASW-178
    const uint16_t expected_message_lengthNoSafety  = 28;               // according to RASW-178
    const uint16_t expected_message_lengthFullMd4   = 44;               // according to RASW-178

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    // call the function
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    srmsg_CreateRetrRespMessage(message_header, &message);

    // verify the output of the function
    EXPECT_EQ(expected_message_length, message.message_size);
    EXPECT_EQ(expected_message_length, getDataFromBuffer_u16(&message.message, expected_message_length_pos));
    EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
    EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
    EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
    EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
}

/**
 * @test        @ID{srmsgTest007} Verify the createHeartbeatMessage function.
 *
 * @details     This test verifies the correct behaviour of the createHeartbeatMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. Ts: Confirmed time stamp in miliseconds
 * - Safety Code Type: Configured Safety code type
 * .
 * | Test Run | Input parameter                                 |||| Test config      | Expected values    |
 * |----------|------------|------------|-------------|------------|------------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number | Conf. TS   | Safety Code Type | Test Result        |
 * | 0        | 0          | 0          | 0           | 0          | Lower MD4        | Normal operation   |
 * | 1        | 0          | 0          | 0           | 0          | Full MD4         | Normal operation   |
 * | 2        | 0          | 0          | 0           | 0          | None             | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0           | 0          | Lower MD4        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0           | 0          | Lower MD4        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff  | 0          | Lower MD4        | Normal operation   |
 * | 6        | 0          | 0          | 0           | 0xffffffff | Lower MD4        | Normal operation   |
 * | 7        | 0xffffffff | 0xffffffff | 0xffffffff  | 0xffffffff | Lower MD4        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-622} Create Heartbeat Message Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-168} Safety Code
 * @verifyReq{RASW-188} Heartbeat Message Structure
 * @verifyReq{RASW-189} Message Length
 */
TEST_P(RetrReqRespHeartbeatMsgWithParam, srmsgTest007VerifyCreateHeartbeatMessage)
{
    uint8_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6220;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 36;               // according to RASW-180
    const uint16_t expected_message_lengthNoSafety  = 28;               // according to RASW-180
    const uint16_t expected_message_lengthFullMd4   = 44;               // according to RASW-180

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    // call the function
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    srmsg_CreateHeartbeatMessage(message_header, &message);

    // verify the output of the function
    EXPECT_EQ(expected_message_length, message.message_size);
    EXPECT_EQ(expected_message_length, getDataFromBuffer_u16(&message.message, expected_message_length_pos));
    EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
    EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
    EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
    EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
}

INSTANTIATE_TEST_SUITE_P(
    verifyRetrReqRespHeartbeatMessage,
    RetrReqRespHeartbeatMsgWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, 0, srcty_kSafetyCodeTypeLowerMd4),
        std::make_tuple(0, 0, 0, 0, srcty_kSafetyCodeTypeFullMd4),
        std::make_tuple(0, 0, 0, 0, srcty_kSafetyCodeTypeNone),
        std::make_tuple(UINT32_MAX, 0, 0, 0, srcty_kSafetyCodeTypeLowerMd4),
        std::make_tuple(0, UINT32_MAX, 0, 0, srcty_kSafetyCodeTypeFullMd4),
        std::make_tuple(0, 0, UINT32_MAX, 0, srcty_kSafetyCodeTypeNone),
        std::make_tuple(0, 0, 0, UINT32_MAX, srcty_kSafetyCodeTypeNone),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, srcty_kSafetyCodeTypeLowerMd4)
    ));

/**
 * @test        @ID{srmsgTest008} Verify the createDiscReqMessage function.
 *
 * @details     This test verifies the correct behaviour of the createDiscReqMessage function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module with given safety code option
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the create message function with given test parameter
 * - verify the correctness of the output message
 * .
 *
 * @testParameter
 * - Rx ID: Receiver identification
 * - Tx ID: Sender identification
 * - Seq. Number: Sequence number
 * - Conf. Ts: Confirmed time stamp in miliseconds
 * - Det. reason: Detailed reason for disconnection from application
 * - Reason: Reason for disconnection
 * - Safety Code Type: Configured Safety code type
 * - Expect Fatal: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                                                                          |||||| Test config      | Expected values                  ||
 * |----------|------------|------------|-------------|------------|-------------|--------------------------------------------|------------------|--------------|--------------------|
 * |          | Rx ID      | Tx ID      | Seq. Number | Conf. TS   | Det. reason | Reason                                     | Safety Code Type | Expect Fatal | Test Result        |
 * | 0        | 0          | 0          | 0           | 0          | 0           | sraty_kDiscReasonUserRequest               | Lower MD4        | False        | Normal operation   |
 * | 1        | 0          | 0          | 0           | 0          | 0           | sraty_kDiscReasonUserRequest               | Full MD4         | False        | Normal operation   |
 * | 2        | 0          | 0          | 0           | 0          | 0           | sraty_kDiscReasonUserRequest               | None             | False        | Normal operation   |
 * | 3        | 0xffffffff | 0          | 0           | 0          | 0           | sraty_kDiscReasonUserRequest               | Lower MD4        | False        | Normal operation   |
 * | 4        | 0          | 0xffffffff | 0           | 0          | 0           | sraty_kDiscReasonUserRequest               | Lower MD4        | False        | Normal operation   |
 * | 5        | 0          | 0          | 0xffffffff  | 0          | 0           | sraty_kDiscReasonUserRequest               | Lower MD4        | False        | Normal operation   |
 * | 6        | 0          | 0          | 0           | 0xffffffff | 0           | sraty_kDiscReasonUserRequest               | Lower MD4        | False        | Normal operation   |
 * | 7        | 0          | 0          | 0           | 0          | 65535       | sraty_kDiscReasonUserRequest               | Lower MD4        | False        | Normal operation   |
 * | 8        | 0          | 0          | 0           | 0          | 0           | sraty_kDiscReasonUserRequest - 1           | Lower MD4        | True         | Fatal error raised |
 * | 9        | 0          | 0          | 0           | 0          | 0           | sraty_kDiscReasonProtocolSequenceError     | Lower MD4        | False        | Normal operation   |
 * | 10       | 0          | 0          | 0           | 0          | 0           | sraty_kDiscReasonProtocolSequenceError + 1 | Lower MD4        | True         | Fatal error raised |
 * | 11       | 0xffffffff | 0xffffffff | 0xffffffff  | 0xffffffff | 65535       | sraty_kDiscReasonProtocolSequenceError     | Lower MD4        | False        | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-621} Create Disconnection Request Message Function
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
TEST_P(DiscReqMsgWithParam, srmsgTest008VerifyCreateDiscReqMessage)
{
    uint8_t i = 0;      // loop iterator

    // Expected Values
    const uint16_t expected_message_type            = 6216;             // according to RASW-161
    const uint16_t expected_message_lengthDefault   = 40;               // according to RASW-184
    const uint16_t expected_message_lengthNoSafety  = 32;               // according to RASW-184
    const uint16_t expected_message_lengthFullMd4   = 48;               // according to RASW-184

    const uint8_t expected_detailed_reason_bytepos  = 28;               // according to RASW-183
    const uint8_t expected_reason_bytepos           = 30;               // according to RASW-183

    // test variables
    srtyp_SrMessage message = {0, {}};
    uint16_t expected_message_length = 0;
    if (safety_code_type == srcty_kSafetyCodeTypeLowerMd4) {
        expected_message_length = expected_message_lengthDefault;
    }
    else if (safety_code_type == srcty_kSafetyCodeTypeFullMd4) {
        expected_message_length = expected_message_lengthFullMd4;
    }
    else {
        expected_message_length = expected_message_lengthNoSafety;
    }

    // initialize the module
    srmsg_Init(safety_code_type, {0, 0, 0, 0});        // initialize module with given safety code and MD4 initial values = 0

    if (expect_fatal_error) {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_CreateDiscReqMessage(message_header, detailed_reason, reason, &message), std::invalid_argument);
    }
    else {
        // call the function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_CreateDiscReqMessage(message_header, detailed_reason, reason, &message);

        // verify the output of the function
        EXPECT_EQ(expected_message_length, message.message_size);
        EXPECT_EQ(expected_message_length, getDataFromBuffer_u16(&message.message, expected_message_length_pos));
        EXPECT_EQ(expected_message_type, getDataFromBuffer_u16(&message.message, expected_message_type_pos));
        EXPECT_EQ(message_header.receiver_id, getDataFromBuffer_u32(&message.message, expected_receiver_id_pos));
        EXPECT_EQ(message_header.sender_id, getDataFromBuffer_u32(&message.message, expected_sender_id_pos));
        EXPECT_EQ(message_header.sequence_number, getDataFromBuffer_u32(&message.message, expected_sequence_nbr_pos));
        EXPECT_EQ(detailed_reason, getDataFromBuffer_u16(&message.message, expected_detailed_reason_bytepos));
        EXPECT_EQ(reason, getDataFromBuffer_u16(&message.message, expected_reason_bytepos));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyCreateDiscReqMessage,
    DiscReqMsgWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, 0, 0, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, 0, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeFullMd4, false),
        std::make_tuple(0, 0, 0, 0, 0, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeNone, false),
        std::make_tuple(UINT32_MAX, 0, 0, 0, 0, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, UINT32_MAX, 0, 0, 0, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, UINT32_MAX, 0, 0, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, UINT32_MAX, 0, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, UINT16_MAX, UT_SRMSG_REASON_MIN, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, 0, (sraty_DiscReason)(UT_SRMSG_REASON_MIN - 1), srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(0, 0, 0, 0, 0, UT_SRMSG_REASON_MAX, srcty_kSafetyCodeTypeLowerMd4, false),
        std::make_tuple(0, 0, 0, 0, 0, (sraty_DiscReason)(UT_SRMSG_REASON_MAX + 1), srcty_kSafetyCodeTypeLowerMd4, true),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT16_MAX, UT_SRMSG_REASON_MAX, srcty_kSafetyCodeTypeLowerMd4, false)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
