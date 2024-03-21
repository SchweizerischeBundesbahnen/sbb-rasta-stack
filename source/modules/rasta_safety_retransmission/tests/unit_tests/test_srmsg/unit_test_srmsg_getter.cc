/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srmsg_getter.cc
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
 * @brief Test class for value-parametrized get message header test
 *
 * Parameter order:
 * - message length (uint16_t)
 * - message type (uint16_t)
 * - receiver ID (uint32_t)
 * - sender ID (uint32_t)
 * - sequence number (uint32_t)
 * - confirmed sequence number (uint32_t)
 * - time stamp (uint32_t)
 * - confirmed time stamp (uint32_t)
 * .
 */
class GetMessageHeaderWithParam :   public srmsgTest,
                                    public testing::WithParamInterface<std::tuple<uint16_t, uint16_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t>>
{
    public:
    uint16_t expected_message_length =          std::get<0>(GetParam());    ///< expected message length
    uint16_t expected_message_type =            std::get<1>(GetParam());    ///< expected message type
    uint32_t expected_receiver_id =             std::get<2>(GetParam());    ///< expected receiver ID
    uint32_t expected_sender_id =               std::get<3>(GetParam());    ///< expected sender ID
    uint32_t expected_sequence_number =         std::get<4>(GetParam());    ///< expected sequence number
    uint32_t expected_conf_seq_number =         std::get<5>(GetParam());    ///< expected conf. sequence number
    uint32_t expected_timestamp =               std::get<6>(GetParam());    ///< expected time stamp
    uint32_t expected_conf_timestamp =          std::get<7>(GetParam());    ///< expected conf. time stamp
};

/**
 * @brief Test class for value-parametrized get data message payload test
 *
 * Parameter order:
 * - message type (srtyp_SrMessageType)
 * - expected data size (uint16_t)
 * - expect a fatal error (bool)
 * - test data pointer (uint8_t *)
 * .
 */
class GetDataMsgPayloadWithParam :  public srmsgTest,
                                    public testing::WithParamInterface<std::tuple<srtyp_SrMessageType, uint16_t, bool, uint8_t *>>
{
    public:
    srtyp_SrMessageType message_type =  std::get<0>(GetParam());        ///< message type
    uint16_t expected_data_size =       std::get<1>(GetParam());        ///< expected data size
    bool expect_fatal_error =           std::get<2>(GetParam());        ///< indicates if a fatal error is expected
    uint8_t * test_data_ptr =           std::get<3>(GetParam());        ///< pointer to test data
};

/**
 * @brief Test class for value-parametrized get connection message data test
 *
 * Parameter order:
 * - message type (srtyp_SrMessageType)
 * - message type valid (bool)
 * - protocol version byte 0 (uint8_t)
 * - protocol version byte 1 (uint8_t)
 * - protocol version byte 2 (uint8_t)
 * - protocol version byte 3 (uint8_t)
 * - nSendMax (uint16_t)
 * .
 */
class GetConnMessageDataWithParam : public srmsgTest,
                                    public testing::WithParamInterface<std::tuple<srtyp_SrMessageType, bool, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t>>
{
    public:
    srtyp_SrMessageType message_type =                  std::get<0>(GetParam());        ///< message type
    bool message_type_valid =                           std::get<1>(GetParam());        ///< message type valid
    srtyp_ProtocolVersion expected_protocol_version =   {std::get<2>(GetParam()),
                                                        std::get<3>(GetParam()),
                                                        std::get<4>(GetParam()),
                                                        std::get<5>(GetParam())};       ///< expected protocol version
    uint16_t expected_nsendmax =                        std::get<6>(GetParam());        ///< expected nSendMax
};

/**
 * @brief Test class for value-parametrized get disconnection message data test
 *
 * Parameter order:
 * - message type (srtyp_SrMessageType)
 * - message type valid (bool)
 * - detailed reason (uint16_t)
 * - reason (sraty_DiscReason)
 * - expect fatal error (bool)
 * .
 */
class GetDiscMessageDataWithParam : public srmsgTest,
                                    public testing::WithParamInterface<std::tuple<srtyp_SrMessageType, bool, uint16_t, sraty_DiscReason, bool>>
{
    public:
    srtyp_SrMessageType message_type =          std::get<0>(GetParam());        ///< message type
    bool message_type_valid =                   std::get<1>(GetParam());        ///< message type valid
    uint16_t expected_detailed_reason =         std::get<2>(GetParam());        ///< expected detailed reason
    sraty_DiscReason expected_disc_reason =     std::get<3>(GetParam());        ///< expected disconnection reason
    bool expect_fatal =                         std::get<4>(GetParam());        ///< expect fatal error
};

/**
 * @brief Test class for value-parametrized GetMessageSequenceNumber test
 *
 * Parameter order:
 * - message sequence number (uint32_t)
 * .
 */
class GetMsgSeqNumberWithParam :    public srmsgTest,
                                    public testing::WithParamInterface<uint32_t>
{
    public:
    uint32_t seq_number =                       GetParam();                     ///< sequence number
};

/**
 * @brief Test class for value-parametrized GetMessageType test
 *
 * Parameter order:
 * - message type (uint16_t)
 * .
 */
class GetMsgTypeWithParam : public srmsgTest,
                            public testing::WithParamInterface<uint16_t>
{
    public:
    uint16_t msg_type =                         GetParam();                     ///< message type
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_messages
 * @{
 */

/**
 * @test        @ID{srmsgTest011} Verify the getMessageHeader function.
 *
 * @details     This test verifies the correct behaviour of the getMessageHeader function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module (no safety code, md4 initial values set to 0)
 * - create a test message with given test parameter
 * - verify the correctness of the message header data
 * .
 *
 * @testParameter
 * - Message Length: Configured message length in bytes
 * - Message Type: Configured message type
 * - Rx ID: Configured receiver identification
 * - Tx ID: Configured sender identification
 * - Seq. Number: Configured sequence number (SN_PDU)
 * - Conf. Seq. N.: Configured confirmed sequence number (CS_PDU)
 * - Timestamp: Configured time stamp (TS_PDU) in ms
 * - Conf. TS: Configured confirmed time stamp (CTS_PDU) in ms
 * .
 * | Test Run | Test config                                                                                              |||||||| Expected values  |
 * |----------|----------------|--------------|------------|------------|-------------|---------------|------------|------------|------------------|
 * |          | Message Length | Message Type | Rx ID      | Tx ID      | Seq. Number | Conf. Seq. N. | Timestamp  | Conf. TS   | Test Result      |
 * | 0        | 28             | 0            | 0          | 0          | 0           | 0             | 0          | 0          | Normal Operation |
 * | 1        | 65535          | 0            | 0          | 0          | 0           | 0             | 0          | 0          | Normal Operation |
 * | 2        | 28             | 65535        | 0          | 0          | 0           | 0             | 0          | 0          | Normal Operation |
 * | 3        | 28             | 0            | 0xffffffff | 0          | 0           | 0             | 0          | 0          | Normal Operation |
 * | 4        | 28             | 0            | 0          | 0xffffffff | 0           | 0             | 0          | 0          | Normal Operation |
 * | 5        | 28             | 0            | 0          | 0          | 0xffffffff  | 0             | 0          | 0          | Normal Operation |
 * | 6        | 28             | 0            | 0          | 0          | 0           | 0xffffffff    | 0          | 0          | Normal Operation |
 * | 7        | 28             | 0            | 0          | 0          | 0           | 0             | 0xffffffff | 0          | Normal Operation |
 * | 8        | 28             | 0            | 0          | 0          | 0           | 0             | 0          | 0xffffffff | Normal Operation |
 * | 9        | 65535          | 65535        | 0xffffffff | 0xffffffff | 0xffffffff  | 0xffffffff    | 0xffffffff | 0xffffffff | Normal Operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-628} Get Message Header Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-162} Receiver Identification
 * @verifyReq{RASW-163} Sender Identification
 * @verifyReq{RASW-164} Sequence Number
 * @verifyReq{RASW-165} Confirmed Sequence Number
 * @verifyReq{RASW-166} Time Stamp
 * @verifyReq{RASW-167} Confirmed Time Stamp
 */
TEST_P(GetMessageHeaderWithParam, srmsgTest011VerifyGetMessageHeader)
{
    // test variables
    srtyp_SrMessage message = {0, {}};
    srtyp_SrMessageHeader message_header = {};
    message.message_size = expected_message_length;
    setDataInBuffer_u16(&message.message, expected_message_length, expected_message_length_pos);
    setDataInBuffer_u16(&message.message, expected_message_type, expected_message_type_pos);
    setDataInBuffer_u32(&message.message, expected_receiver_id, expected_receiver_id_pos);
    setDataInBuffer_u32(&message.message, expected_sender_id, expected_sender_id_pos);
    setDataInBuffer_u32(&message.message, expected_sequence_number, expected_sequence_nbr_pos);
    setDataInBuffer_u32(&message.message, expected_conf_seq_number, expected_conf_seq_nbr_pos);
    setDataInBuffer_u32(&message.message, expected_timestamp, expected_timestamp_pos);
    setDataInBuffer_u32(&message.message, expected_conf_timestamp, expected_conf_timestamp_pos);

    // initialize the module
    srmsg_Init(srcty_kSafetyCodeTypeNone, {0, 0, 0, 0});        // no safety code and MD4 initial values set to zero

    // call the function
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    srmsg_GetMessageHeader(&message, &message_header);

    // Verify that the values are set correctly
    EXPECT_EQ(expected_message_length, message_header.message_length);
    EXPECT_EQ(expected_message_type, message_header.message_type);
    EXPECT_EQ(expected_sender_id, message_header.sender_id);
    EXPECT_EQ(expected_receiver_id, message_header.receiver_id);
    EXPECT_EQ(expected_sequence_number, message_header.sequence_number);
    EXPECT_EQ(expected_conf_seq_number, message_header.confirmed_sequence_number);
    EXPECT_EQ(expected_timestamp, message_header.time_stamp);
    EXPECT_EQ(expected_conf_timestamp, message_header.confirmed_time_stamp);
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetMessageHeader,
    GetMessageHeaderWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRMSG_HEADER_SIZE, 0, 0, 0, 0, 0, 0, 0),
        std::make_tuple(UINT16_MAX, 0, 0, 0, 0, 0, 0, 0),
        std::make_tuple(UT_SRMSG_HEADER_SIZE, UINT16_MAX, 0, 0, 0, 0, 0, 0),
        std::make_tuple(UT_SRMSG_HEADER_SIZE, 0, UINT32_MAX, 0, 0, 0, 0, 0),
        std::make_tuple(UT_SRMSG_HEADER_SIZE, 0, 0, UINT32_MAX, 0, 0, 0, 0),
        std::make_tuple(UT_SRMSG_HEADER_SIZE, 0, 0, 0, UINT32_MAX, 0, 0, 0),
        std::make_tuple(UT_SRMSG_HEADER_SIZE, 0, 0, 0, 0, UINT32_MAX, 0, 0),
        std::make_tuple(UT_SRMSG_HEADER_SIZE, 0, 0, 0, 0, 0, UINT32_MAX, 0),
        std::make_tuple(UT_SRMSG_HEADER_SIZE, 0, 0, 0, 0, 0, 0, UINT32_MAX),
        std::make_tuple(UINT16_MAX, UINT16_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX)
    ));

/**
 * @test        @ID{srmsgTest012} Verify the getDataMessagePayload function.
 *
 * @details     This test verifies the correct behaviour of the getDataMessagePayload function.
 *
 * Test steps:
 * - setup test variables and expected values
 * - initialize the module (no safety code, md4 initial values set to 0)
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - create a test message with given test parameter
 * - verify that the function can only be called for data messages
 * - verify that the payload size is below the maximum value
 * - verify the correctness of the message payload data
 * .
 *
 * @testParameter
 * - Message Type: Configured message type
 * - Data Size: Configured size of payload in bytes (valid range: 1 - 1055)
 * - Data Pointer: Pointer to buffer which contains the test data
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Test config                                           ||| Expected values                        ||
 * |----------|--------------|------------|-----------------------------|--------------------|--------------------|
 * |          | Message Type | Data Size  | Data Pointer                | Expect Fatal Error | Test Result        |
 * | 0        | Data         | 0          | &test_payload_data_u8       | true               | Fatal error raised |
 * | 1        | Data         | 1          | &test_payload_data_u8[1054] | false              | Normal Operation   |
 * | 2        | Data         | 1055       | &test_payload_data_u8       | false              | Normal Operation   |
 * | 3        | Data         | 1056       | &test_payload_data_u8       | true               | Fatal error raised |
 * | 4        | RetrData     | 1          | &test_payload_data_u8[1054] | false              | Normal Operation   |
 * | 5        | RetrData     | 1055       | &test_payload_data_u8       | false              | Normal Operation   |
 * | 6        | RetrData     | 1056       | &test_payload_data_u8       | true               | Fatal error raised |
 * | 7        | ConnReq      | 1055       | &test_payload_data_u8       | true               | Fatal error raised |
 * | 8        | ConnResp     | 1055       | &test_payload_data_u8       | true               | Fatal error raised |
 * | 9        | RetrReq      | 1055       | &test_payload_data_u8       | true               | Fatal error raised |
 * | 10       | RetrResp     | 1055       | &test_payload_data_u8       | true               | Fatal error raised |
 * | 11       | Heartbeat    | 1055       | &test_payload_data_u8       | true               | Fatal error raised |
 * | 12       | DiscReq      | 1055       | &test_payload_data_u8       | true               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-626} Get Data Message Payload Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-191} Data Message Structure
 * @verifyReq{RASW-196} Retransmitted Data Message Structure
 * @verifyReq{RASW-192} Message Length
 * @verifyReq{RASW-193} Size of Message Data
 */
TEST_P(GetDataMsgPayloadWithParam, srmsgTest012VerifyGetDataMessagePayload)
{
    uint16_t i = 0;     // loop iterator

    // Expected Values
    const uint8_t expected_payload_size_bytepos     = 28;               // according to RASW-191
    const uint8_t expected_payload_bytepos          = 30;               // according to RASW-191

    // test variables
    srtyp_SrMessage message = {0, {}};
    srtyp_SrMessagePayload message_payload = {0, {}};
    message.message_size = (expected_payload_bytepos + expected_data_size);
    setDataInBuffer_u16(&message.message,(expected_payload_bytepos + expected_data_size), expected_message_length_pos);
    setDataInBuffer_u16(&message.message, message_type, expected_message_type_pos);
    setDataInBuffer_u16(&message.message, expected_data_size, expected_payload_size_bytepos);
    for (i = 0; i < expected_data_size; i++) {
        setDataInBuffer_u8(&message.message, test_data_ptr[i], (expected_payload_bytepos + i));
    }

    // initialize the module
    srmsg_Init(srcty_kSafetyCodeTypeNone, {0, 0, 0, 0});        // no safety code and MD4 initial values set to zero

    // call the function
    if (expect_fatal_error) {
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_GetDataMessagePayload(&message, &message_payload), std::invalid_argument);
    }
    else {
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_GetDataMessagePayload(&message, &message_payload);

        // Verify that the values are set correctly
        EXPECT_EQ(expected_data_size, message_payload.payload_size);
        for (i = 0; i < expected_data_size; i++) {
            EXPECT_EQ(test_data_ptr[i], message_payload.payload[i]);
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetDataMessagePayload,
    GetDataMsgPayloadWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kSrMessageData, (UT_SRMSG_DATA_LEN_MIN - 1), true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageData, UT_SRMSG_DATA_LEN_MIN, false, &(test_payload_data_u8[UT_SRMSG_DATA_LEN_MAX - 1])),
        std::make_tuple(srtyp_kSrMessageData, UT_SRMSG_DATA_LEN_MAX, false, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageData, (UT_SRMSG_DATA_LEN_MAX + 1), true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageRetrData, UT_SRMSG_DATA_LEN_MIN, false, &(test_payload_data_u8[UT_SRMSG_DATA_LEN_MAX - 1])),
        std::make_tuple(srtyp_kSrMessageRetrData, UT_SRMSG_DATA_LEN_MAX, false, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageRetrData, (UT_SRMSG_DATA_LEN_MAX + 1), true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageConnReq, UT_SRMSG_DATA_LEN_MAX, true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageConnResp, UT_SRMSG_DATA_LEN_MAX, true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageRetrReq, UT_SRMSG_DATA_LEN_MAX, true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageRetrResp, UT_SRMSG_DATA_LEN_MAX, true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageHb, UT_SRMSG_DATA_LEN_MAX, true, test_payload_data_u8),
        std::make_tuple(srtyp_kSrMessageDiscReq, UT_SRMSG_DATA_LEN_MAX, true, test_payload_data_u8)
    ));

/**
 * @test        @ID{srmsgTest013} Verify the getConnMessageData function.
 *
 * @details     This test verifies the correct behaviour of the getConnMessageData function.
 *
 * Test steps:
 * - initialize the module (no safety code, md4 initial values set to 0)
 * - verify parameter check for the message pointer, protocol version pointer and nSendMax pointer
 * - create a test message with given test parameter
 * - expect a radef_kInvalidParameter when message type not
 * - verify that the function can only be called for connection messages
 * - verify the correctness of the connection message data
 * .
 *
 * @testParameter
 * - Message Type: Configured message type
 * - M. Type valid: Indicates if message type is valid for function call
 * - Ver.[0]: Configured version digit 1
 * - Ver.[1]: Configured version digit 2
 * - Ver.[2]: Configured version digit 3
 * - Ver.[3]: Configured version digit 4
 * - nSendMax: Configured nSendMax
 * .
 * | Test Run | Test config                                                               ||||||| Expected values    |
 * |----------|--------------|---------------|---------|---------|---------|---------|----------|--------------------|
 * |          | Message Type | M. Type valid | Ver.[0] | Ver.[1] | Ver.[2] | Ver.[3] | nSendMax | Test Result        |
 * | 0        | ConnReq      | true          | 0x30    | 0x30    | 0x30    | 0x30    | 0        | Normal operation   |
 * | 1        | ConnReq      | true          | 0x39    | 0x30    | 0x30    | 0x30    | 0        | Normal operation   |
 * | 2        | ConnReq      | true          | 0x30    | 0x39    | 0x30    | 0x30    | 0        | Normal operation   |
 * | 3        | ConnReq      | true          | 0x30    | 0x30    | 0x39    | 0x30    | 0        | Normal operation   |
 * | 4        | ConnReq      | true          | 0x30    | 0x30    | 0x30    | 0x39    | 0        | Normal operation   |
 * | 5        | ConnReq      | true          | 0x30    | 0x30    | 0x30    | 0x30    | 65535    | Normal operation   |
 * | 6        | ConnReq      | true          | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Normal operation   |
 * | 7        | ConnResp     | true          | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Normal operation   |
 * | 8        | RetrReq      | false         | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Fatal error raised |
 * | 9        | RetrResp     | false         | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Fatal error raised |
 * | 10       | DiscReq      | false         | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Fatal error raised |
 * | 11       | Heartbeat    | false         | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Fatal error raised |
 * | 12       | Data         | false         | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Fatal error raised |
 * | 13       | DataReq      | false         | 0x39    | 0x39    | 0x39    | 0x39    | 65535    | Fatal error raised |
 *
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-625} Get Connection Message Data Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-170} Connection Request Message Structure
 * @verifyReq{RASW-171} Connection Response Message Structure
 * @verifyReq{RASW-172} Message Length
 * @verifyReq{RASW-173} Protocol Version
 * @verifyReq{RASW-174} Nsendmax
 */
TEST_P(GetConnMessageDataWithParam, srmsgTest013VerifyGetConnMessageData)
{
    uint16_t i = 0;     // loop iterator

    // Expected Values
    const uint8_t expected_nsendmax_bytepos         = 32;               // according to RASW-170
    const uint8_t expected_protocol_version_bytepos = 28;               // according to RASW-170
    const uint8_t expected_protocol_version_length  = 4;                // according to RASW-170
    const uint8_t expected_min_message_length       = 42;               // according to RASW-172

    // test variables
    srtyp_SrMessage message = {0, {}};
    srtyp_ProtocolVersion protocol_version = {};
    uint16_t n_send_max = 0;
    message.message_size = expected_min_message_length;
    setDataInBuffer_u16(&message.message, expected_min_message_length, expected_message_length_pos);
    setDataInBuffer_u16(&message.message, message_type, expected_message_type_pos);
    for (i = 0; i < expected_protocol_version_length; i++) {
        setDataInBuffer_u8(&message.message, expected_protocol_version.version[i], (expected_protocol_version_bytepos + i));
    }
    setDataInBuffer_u16(&message.message, expected_nsendmax, expected_nsendmax_bytepos);

    // initialize the module
    srmsg_Init(srcty_kSafetyCodeTypeNone, {0, 0, 0, 0});        // no safety code and MD4 initial values set to zero

    // call the function
    if (!message_type_valid) {
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_GetConnMessageData(&message, &protocol_version, &n_send_max), std::invalid_argument);
    }
    else {
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_GetConnMessageData(&message, &protocol_version, &n_send_max);

        // Verify that the values are set correctly
        for (i = 0; i < expected_protocol_version_length; i++) {
            EXPECT_EQ(expected_protocol_version.version[i], protocol_version.version[i]);
        }
        EXPECT_EQ(expected_nsendmax, n_send_max);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetConnMessageData,
    GetConnMessageDataWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kSrMessageConnReq, true, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, 0),
        std::make_tuple(srtyp_kSrMessageConnReq, true, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, 0),
        std::make_tuple(srtyp_kSrMessageConnReq, true, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, 0),
        std::make_tuple(srtyp_kSrMessageConnReq, true, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MIN, 0),
        std::make_tuple(srtyp_kSrMessageConnReq, true, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MAX, 0),
        std::make_tuple(srtyp_kSrMessageConnReq, true, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UT_SRMSG_PROT_MIN, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageConnReq, true, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageConnResp, true, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageRetrReq, false, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageRetrResp, false, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageDiscReq, false, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageHb, false, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageData, false, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX),
        std::make_tuple(srtyp_kSrMessageRetrData, false, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UT_SRMSG_PROT_MAX, UINT16_MAX)
    ));

/**
 * @test        @ID{srmsgTest014} Verify the getDiscMessageData function.
 *
 * @details     This test verifies the correct behaviour of the getDiscMessageData function.
 *
 * Test steps:
 * - initialize the module (no safety code, md4 initial values set to 0)
 * - verify parameter check for the message pointer, detailed reason pointer and reason pointer
 * - expect a radef_kInvalidParameter when message type not valid
 * - create a test message with given test parameter
 * - verify that the function can only be called for disconnection request messages
 * - verify the correctness of the connection message data
 * .
 *
 * @testParameter
 * - Message type: Configured message type
 * - M. Type valid: Indicates if message type is valid for function call
 * - Detailed reason: Detailed reason for disconnection from application
 * - Reason: Disconnect reason
 * - Expect fatal error: Indicates if a fatal error is expected
 * .
 * | Test Run | Test config                                                                          |||| Expected values                        ||
 * |----------|--------------|---------------|-----------------|----------------------------------------|--------------------|--------------------|
 * |          | Message Type | M. Type valid | Detailed reason | Reason                                 | Expect fatal error | Test Result        |
 * | 0        | DiscReq      | true          | 0               | sraty_kDiscReasonUserRequest           | false              | Normal Operation   |
 * | 1        | DiscReq      | true          | 65535           | sraty_kDiscReasonUserRequest           | false              | Normal Operation   |
 * | 2        | DiscReq      | true          | 0               | sraty_kDiscReasonProtocolSequenceError | false              | Normal Operation   |
 * | 3        | DiscReq      | true          | 65535           | sraty_kDiscReasonProtocolSequenceError | false              | Normal Operation   |
 * | 10       | DiscReq      | true          | 65535           | sraty_kDiscReasonMax                   | true               | Fatal error raised |
 * | 4        | ConnReq      | false         | 65535           | sraty_kDiscReasonProtocolSequenceError | true               | Fatal error raised |
 * | 5        | ConnResp     | false         | 65535           | sraty_kDiscReasonProtocolSequenceError | true               | Fatal error raised |
 * | 6        | RetrReq      | false         | 65535           | sraty_kDiscReasonProtocolSequenceError | true               | Fatal error raised |
 * | 7        | RetrResp     | false         | 65535           | sraty_kDiscReasonProtocolSequenceError | true               | Fatal error raised |
 * | 8        | Heartbeat    | false         | 65535           | sraty_kDiscReasonProtocolSequenceError | true               | Fatal error raised |
 * | 9        | Data         | false         | 65535           | sraty_kDiscReasonProtocolSequenceError | true               | Fatal error raised |
 * | 10       | RetrData     | false         | 65535           | sraty_kDiscReasonProtocolSequenceError | true               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-627} Get Disconnection Message Data Function
 * @verifyReq{RASW-157} Endian Definition
 * @verifyReq{RASW-160} General PDU Message Structure
 * @verifyReq{RASW-161} Message Type
 * @verifyReq{RASW-183} Disconnection Request Message Structure
 * @verifyReq{RASW-184} Message Length
 * @verifyReq{RASW-185} Detailed Information regarding the Reason for the Disconnection Request
 * @verifyReq{RASW-186} Reason for Disconnection Request
 */
TEST_P(GetDiscMessageDataWithParam, srmsgTest014VerifyGetDiscMessageData)
{
    uint16_t i = 0;     // loop iterator

    // Expected Values
    const uint8_t expected_detailed_reason_bytepos  = 28;               // according to RASW-183
    const uint8_t expected_reason_bytepos           = 30;               // according to RASW-183
    const uint8_t expected_min_message_length       = 32;               // according to RASW-184

    // test variables
    srtyp_SrMessage message = {0, {}};
    sraty_DiscReason disc_reason = sraty_kDiscReasonUserRequest;
    uint16_t detailed_reason = 0;
    message.message_size = expected_min_message_length;
    setDataInBuffer_u16(&message.message, expected_min_message_length, expected_message_length_pos);
    setDataInBuffer_u16(&message.message, message_type, expected_message_type_pos);
    setDataInBuffer_u16(&message.message, expected_detailed_reason, expected_detailed_reason_bytepos);
    setDataInBuffer_u16(&message.message, expected_disc_reason, expected_reason_bytepos);

    // initialize the module
    srmsg_Init(srcty_kSafetyCodeTypeNone, {0, 0, 0, 0});        // no safety code and MD4 initial values set to zero

    // call the function
    if (expect_fatal) {
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmsgTest::invalidArgumentException));
        EXPECT_THROW(srmsg_GetDiscMessageData(&message, &detailed_reason, &disc_reason), std::invalid_argument);
    }
    else {
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        srmsg_GetDiscMessageData(&message, &detailed_reason, &disc_reason);

        // Verify that the values are set correctly
        EXPECT_EQ(expected_detailed_reason, detailed_reason);
        EXPECT_EQ(expected_disc_reason, disc_reason);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetDiscMessageData,
    GetDiscMessageDataWithParam,
    ::testing::Values(
        std::make_tuple(srtyp_kSrMessageDiscReq, true, 0, UT_SRMSG_REASON_MIN, false),
        std::make_tuple(srtyp_kSrMessageDiscReq, true, UINT16_MAX, UT_SRMSG_REASON_MIN, false),
        std::make_tuple(srtyp_kSrMessageDiscReq, true, 0, UT_SRMSG_REASON_MAX, false),
        std::make_tuple(srtyp_kSrMessageDiscReq, true, UINT16_MAX, UT_SRMSG_REASON_MAX, false),
        std::make_tuple(srtyp_kSrMessageDiscReq, false, UINT16_MAX, sraty_kDiscReasonMax, true),
        std::make_tuple(srtyp_kSrMessageConnReq, false, UINT16_MAX, UT_SRMSG_REASON_MAX, true),
        std::make_tuple(srtyp_kSrMessageConnResp, false, UINT16_MAX, UT_SRMSG_REASON_MAX, true),
        std::make_tuple(srtyp_kSrMessageRetrReq, false, UINT16_MAX, UT_SRMSG_REASON_MAX, true),
        std::make_tuple(srtyp_kSrMessageRetrResp, false, UINT16_MAX, UT_SRMSG_REASON_MAX, true),
        std::make_tuple(srtyp_kSrMessageHb, false, UINT16_MAX, UT_SRMSG_REASON_MAX, true),
        std::make_tuple(srtyp_kSrMessageData, false, UINT16_MAX, UT_SRMSG_REASON_MAX, true),
        std::make_tuple(srtyp_kSrMessageRetrData, false, UINT16_MAX, UT_SRMSG_REASON_MAX, true)
    ));

/**
 * @test        @ID{srmsgTest018} Verify the GetMessageSequenceNumber function.
 *
 * @details     This test verifies the correct behaviour of the GetMessageSequenceNumber function.
 *
 * Test steps:
 * - initialize the module (no safety code, md4 initial values set to 0)
 * - create a test message with given test parameter
 * - verify the correctness of the sequence number returned
 * .
 *
 * @testParameter
 * - Seq. Number: Configured sequence number of message
 * .
 * | Test Run | Input parameter | Expected values  |
 * |----------|-----------------|------------------|
 * |          | Seq. Number     | Test Result      |
 * | 0        | 0               | Normal Operation |
 * | 1        | 0xffffffff      | Normal Operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-825} Get Message Sequence Number Function
 */
TEST_P(GetMsgSeqNumberWithParam, srmsgTest018VerifyGetMessageSequenceNumber)
{
    // create test message
    srtyp_SrMessage message = {0, {}};
    message.message_size = UT_SRMSG_HEADER_SIZE;
    setDataInBuffer_u16(&message.message, UT_SRMSG_HEADER_SIZE, expected_message_length_pos);
    setDataInBuffer_u32(&message.message, seq_number, expected_sequence_nbr_pos);

    // initialize the module
    srmsg_Init(srcty_kSafetyCodeTypeNone, {0, 0, 0, 0});        // no safety code and MD4 initial values set to zero

    // call the GetMessageSequenceNumber function and verify the sequence number returned
    EXPECT_EQ(seq_number, srmsg_GetMessageSequenceNumber(&message));
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetMessageSequenceNumber,
    GetMsgSeqNumberWithParam,
    ::testing::Values(0, UINT32_MAX));

/**
 * @test        @ID{srmsgTest019} Verify the GetMessageType function.
 *
 * @details     This test verifies the correct behaviour of the GetMessageType function.
 *
 * Test steps:
 * - initialize the module (no safety code, md4 initial values set to 0)
 * - create a test message with given test parameter
 * - verify the correctness of the message type returned
 * .
 *
 * @testParameter
 * - Message Type: Configured message type of message
 * .
 * | Test Run | Input parameter | Expected values  |
 * |----------|-----------------|------------------|
 * |          | Message Type    | Test Result      |
 * | 0        | 0               | Normal Operation |
 * | 1        | ConnReq         | Normal Operation |
 * | 2        | ConnResp        | Normal Operation |
 * | 3        | RetrReq         | Normal Operation |
 * | 4        | RetrResp        | Normal Operation |
 * | 5        | DiscReq         | Normal Operation |
 * | 6        | Heartbeat       | Normal Operation |
 * | 7        | Data            | Normal Operation |
 * | 8        | RetrData        | Normal Operation |
 * | 9        | 65535           | Normal Operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-824} Get Message Type Function
 */
TEST_P(GetMsgTypeWithParam, srmsgTest019VerifyGetMessageType)
{
    // create test message
    srtyp_SrMessage message = {0, {}};
    message.message_size = UT_SRMSG_HEADER_SIZE;
    setDataInBuffer_u16(&message.message, UT_SRMSG_HEADER_SIZE, expected_message_length_pos);
    setDataInBuffer_u16(&message.message, msg_type, expected_message_type_pos);

    // initialize the module
    srmsg_Init(srcty_kSafetyCodeTypeNone, {0, 0, 0, 0});        // no safety code and MD4 initial values set to zero

    // call the GetMessageType function and verify the message type returned
    EXPECT_EQ(msg_type, srmsg_GetMessageType(&message));
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetMessageType,
    GetMsgTypeWithParam,
    ::testing::Values(  0,
                        srtyp_kSrMessageConnReq,
                        srtyp_kSrMessageConnResp,
                        srtyp_kSrMessageRetrReq,
                        srtyp_kSrMessageRetrResp,
                        srtyp_kSrMessageDiscReq,
                        srtyp_kSrMessageHb,
                        srtyp_kSrMessageData,
                        srtyp_kSrMessageRetrData,
                        UINT16_MAX));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
