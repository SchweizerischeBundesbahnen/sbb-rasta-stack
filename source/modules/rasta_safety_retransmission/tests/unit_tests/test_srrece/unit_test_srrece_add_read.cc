/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srrece_add_read.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the received buffer module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srrece.hh"

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized AddToBuffer test
 *
 * Parameter order:
 * - do init buffer (bool)
 * - maximum number of connections (uint32_t)
 * - connection ID (uint32_t)
 * - payload size (uint16_t)
 * - payload data pointer (uint8_t *)
 * - nSendMax (uint8_t)
 * - expected buffer length after add (uint8_t)
 * - do read back check (bool)
 * - expected error code (radef_RaStaReturnCode)
 * .
 */
class AddToBufferTestWithParam :    public srreceTest,
                                    public testing::WithParamInterface<std::tuple<bool, uint32_t, uint32_t, uint16_t, uint8_t *, uint8_t, uint8_t, bool, radef_RaStaReturnCode>>
{
    public:
    bool do_initialize =                std::get<0>(GetParam());        ///< initialize module
    uint32_t n_max_conn =               std::get<1>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =            std::get<2>(GetParam());        ///< connection ID
    srtyp_SrMessagePayload payload =    {std::get<3>(GetParam()), {}};  ///< message payload
    uint8_t * payload_data_ptr =        std::get<4>(GetParam());        ///< pointer to payload data
    uint8_t n_send_max =                std::get<5>(GetParam());        ///< buffer size
    uint8_t exp_buffer_length =         std::get<6>(GetParam());        ///< expected buffer length after write
    bool do_read_back_check =           std::get<7>(GetParam());        ///< indicates if a read back check is performed
    radef_RaStaReturnCode error =       std::get<8>(GetParam());        ///< expected error code
};

/**
 * @brief Test class for value-parametrized ReadFromBuffer test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection ID (uint32_t)
 * - payload size (uint16_t)
 * - payload data pointer (uint8_t *)
 * - do add message (bool)
 * - expect fatal error (bool)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class ReadFromBufferTestWithParam : public srreceTest,
                                    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, uint8_t *, bool, bool, radef_RaStaReturnCode>>
{
    public:
    uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
    srtyp_SrMessagePayload payload =    {std::get<2>(GetParam()), {}};  ///< message payload
    uint8_t * payload_data_ptr =        std::get<3>(GetParam());        ///< pointer to payload data
    bool do_add_message =               std::get<4>(GetParam());        ///< indicates if a message shall be added to the buffer
    bool expect_fatal_error =           std::get<5>(GetParam());        ///< indicates if a fatal error is expected
    radef_RaStaReturnCode exp_retval =  std::get<6>(GetParam());        ///< expected return value
};

/**
 * @brief Test class for value-parametrized buffer test
 *
 * Parameter order:
 * - connection ID (uint32_t)
 * .
 */
class CompleteBufferTestWithParam : public srreceTest,
                                    public testing::WithParamInterface<uint32_t>
{
    public:
    uint32_t connection_id =            GetParam();  ///< connection ID
};

/**
 * @brief Test class for value-parametrized GetPayloadSizeOfNextMessageToRead test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection ID (uint32_t)
 * - payload size (uint16_t)
 * - do add message (bool)
 * - expect fatal error (bool)
 * - expected return value (uint16_t)
 * .
 */
class GetNextPayloadSizeTestWithParam : public srreceTest,
                                        public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint16_t, bool, bool, uint16_t>>
{
    public:
    uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
    srtyp_SrMessagePayload payload =    {std::get<2>(GetParam()), {}};  ///< message payload
    bool do_add_message =               std::get<3>(GetParam());        ///< indicates if a message shall be added to the buffer
    bool expect_fatal_error =           std::get<4>(GetParam());        ///< indicates if a fatal error is expected
    uint16_t exp_retval =               std::get<5>(GetParam());        ///< expected return value
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_receiveBuf
 * @{
 */

/**
 * @test        @ID{srreceTest006} Verify the AddToBuffer function.
 *
 * @details     This test verifies that correct behaviour of the AddToBuffer function.
 *
 * Test steps:
 * - initialize the module with given nSendMax parameter and given maximum number of connections
 * - set the fatal error expectation based on the test parameter
 * - verify the buffer length and free entries before the add
 * - call the AddToBuffer function
 * - verify the buffer length and free entries after the add
 * - verify that the message received notification is sent
 * - perform a read back check using the ReadFromBuffer function and verify the data correctness
 * - verify the buffer length and free entries after the read
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Do init: Indicates if module gets initialized
 * - Max. Num. Conn.: Number of configured connections
 * - Data Size: Payload size in bytes (valid range: 0 - 1055)
 * - Data Pointer: Pointer to buffer which contains test message
 * - nSendMax: Configured nSendMax (buffersize)
 * - Read back check: Indicates if a read operation gets performed after calling AddToBuffer function
 * - Exp. Buf. Length: Expected buffer length after calling AddToBuffer function
 * - Exp. Error Code: Expected error code when calling function
 * .
 * | Test Run | Input parameter | Test config                                                                          |||||| Expected values                                                |||
 * |----------|-----------------|---------|-----------------|-----------|----------------------|----------|-----------------|------------------|--------------------------|--------------------|
 * |          | Connection Id   | Do init | Max. Num. Conn. | Data Size | Data Pointer         | nSendMax | Read back check | Exp. Buf. Length | Exp. Error Code          | Test Result        |
 * | 0        | 0               | true    | 2               | 1055      | test_payload_data_u8 | 5        | true            | 1                | radef_kNoError           | Normal operation   |
 * | 1        | 0               | true    | 2               | 1055      | test_payload_data_u8 | 5        | false           | 1                | radef_kNoError           | Normal operation   |
 * | 2        | 0               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 2                | radef_kNoError           | Normal operation   |
 * | 3        | 0               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 3                | radef_kNoError           | Normal operation   |
 * | 4        | 0               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 4                | radef_kNoError           | Normal operation   |
 * | 5        | 0               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 5                | radef_kNoError           | Normal operation   |
 * | 6        | 0               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 5                | radef_kReceiveBufferFull | Fatal error raised |
 * | 7        | 1               | true    | 2               | 1055      | test_payload_data_u8 | 5        | true            | 1                | radef_kNoError           | Normal operation   |
 * | 8        | 1               | true    | 2               | 1055      | test_payload_data_u8 | 5        | false           | 1                | radef_kNoError           | Normal operation   |
 * | 9        | 1               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 2                | radef_kNoError           | Normal operation   |
 * | 10       | 1               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 3                | radef_kNoError           | Normal operation   |
 * | 11       | 1               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 4                | radef_kNoError           | Normal operation   |
 * | 12       | 1               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 5                | radef_kNoError           | Normal operation   |
 * | 13       | 1               | false   | 2               | 1055      | test_payload_data_u8 | 5        | false           | 5                | radef_kReceiveBufferFull | Fatal error raised |
 * | 14       | 2               | true    | 2               | 1055      | test_payload_data_u8 | 5        | false           | 1                | radef_kInvalidParameter  | Fatal error raised |
 * | 15       | 1               | true    | 1               | 1055      | test_payload_data_u8 | 5        | false           | 1                | radef_kInvalidParameter  | Fatal error raised |
 * | 16       | 0               | true    | 2               | 0         | test_payload_data_u8 | 5        | false           | 1                | radef_kInvalidParameter  | Fatal error raised |
 * | 17       | 0               | true    | 2               | 1056      | test_payload_data_u8 | 5        | false           | 1                | radef_kInvalidParameter  | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-608} Add to Buffer Function
 * @verifyReq{RASW-609} Get Used Buffer Entries Function
 * @verifyReq{RASW-610} Get Free Buffer Entries Function
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-554} Message Received Notification
 * @verifyReq{RASW-279} Message Received Notification Structure
 * @verifyReq{RASW-302} Connection Identification
 */
TEST_P(AddToBufferTestWithParam, srreceTest006VerifyAddToBufferFunction)
{
    uint16_t i = 0;                                     // loop iterator
    srtyp_SrMessagePayload read_payload = {0, {}};       // payload read from the buffer

    // copy payload data
    for (i = 0; i < payload.payload_size; i++)
    {
        payload.payload[i] = payload_data_ptr[i];
    }

    // initialize module
    if(do_initialize)
    {
        srrece_Init(n_max_conn, n_send_max);
    }
    else
    {
        srrece_initialized = true;
    }

    if (error != radef_kNoError)
    {
        // call the addToBuffer function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(error)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));
        EXPECT_THROW(srrece_AddToBuffer(connection_id, &payload), std::invalid_argument);
    }
    else
    {
        // verify the buffer length and number of free entries before the add
        EXPECT_EQ((exp_buffer_length - 1), srrece_GetUsedBufferEntries(connection_id));
        EXPECT_EQ((n_send_max - exp_buffer_length + 1), srrece_GetFreeBufferEntries(connection_id));

        // expect the message received notification to be sent
        EXPECT_CALL(srnot_mock, srnot_MessageReceivedNotification(connection_id)).Times(1);

        // call the AddToBuffer function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_NO_THROW(srrece_AddToBuffer(connection_id, &payload));

        // verify the buffer length and number of free entries after the add
        EXPECT_EQ(exp_buffer_length, srrece_GetUsedBufferEntries(connection_id));
        EXPECT_EQ((n_send_max - exp_buffer_length), srrece_GetFreeBufferEntries(connection_id));

        if (do_read_back_check)
        {
            // call the readFromBuffer function
            EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id, &read_payload));

            // verify the buffer length and number of free entries after the read
            EXPECT_EQ((exp_buffer_length - 1), srrece_GetUsedBufferEntries(connection_id));
            EXPECT_EQ((n_send_max - exp_buffer_length + 1), srrece_GetFreeBufferEntries(connection_id));

            // verify the data
            ASSERT_EQ(payload.payload_size, read_payload.payload_size);
            for (i = 0; i < payload.payload_size; i++)
            {
                EXPECT_EQ(payload.payload[i], read_payload.payload[i]);
            }
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyAddToBufferFunction,
    AddToBufferTestWithParam,
    ::testing::Values(
        std::make_tuple(true, 2, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 1, true, radef_kNoError),
        std::make_tuple(true, 2, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 1, false, radef_kNoError),
        std::make_tuple(false, 2, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 2, false, radef_kNoError),
        std::make_tuple(false, 2, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 3, false, radef_kNoError),
        std::make_tuple(false, 2, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 4, false, radef_kNoError),
        std::make_tuple(false, 2, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 5, false, radef_kNoError),
        std::make_tuple(false, 2, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 5, false, radef_kReceiveBufferFull),
        std::make_tuple(true, 2, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 1, true, radef_kNoError),
        std::make_tuple(true, 2, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 1, false, radef_kNoError),
        std::make_tuple(false, 2, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 2, false, radef_kNoError),
        std::make_tuple(false, 2, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 3, false, radef_kNoError),
        std::make_tuple(false, 2, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 4, false, radef_kNoError),
        std::make_tuple(false, 2, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 5, false, radef_kNoError),
        std::make_tuple(false, 2, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 5, false, radef_kReceiveBufferFull),
        std::make_tuple(true, 2, 2, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 1, false, radef_kInvalidParameter),
        std::make_tuple(true, 1, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, 5, 1, false, radef_kInvalidParameter),
        std::make_tuple(true, 2, 0, (UT_SRRECE_DATA_LEN_MIN - 1), test_payload_data_u8, 5, 1, false, radef_kInvalidParameter),
        std::make_tuple(true, 2, 0, (UT_SRRECE_DATA_LEN_MAX + 1), test_payload_data_u8, 5, 1, false, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{srreceTest007} Verify the ReadFromBuffer function.
 *
 * @details     This test verifies that correct behaviour of the ReadFromBuffer function.
 *
 * Test steps:
 * - initialize the module with maximum nSendMax and given maximum number of connections
 * - set the fatal error expectation based on the test parameter
 * - add a message and verify that the buffer length is set to 1 (if activated by the test parameter)
 * - call the ReadFromBuffer function
 * - verify the buffer length and free entries after the read
 * - verify the data correctness (if message has been added)
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Max. Num. Conn.: Number of configured connections
 * - Data Size: Payload size in bytes
 * - Data Pointer: Pointer to buffer which contains test message
 * - Add Message: Indicates if a message gets added to buffer before read
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * - Exp. Return Value: Expected return value when calling srrece_ReadFromBuffer function
 * .
 * | Test Run | Input parameter | Test config                                                   |||| Expected values                                                  |||
 * |----------|-----------------|-----------------|-----------|----------------------|-------------|--------------------|--------------------------|--------------------|
 * |          | Connection Id   | Max. Num. Conn. | Data Size | Data Pointer         | Add Message | Expect Fatal Error | Exp. Return Value        | Test Result        |
 * | 0        | 0               | 2               | 1055      | test_payload_data_u8 | false       | false              | radef_kNoMessageReceived | Normal operation   |
 * | 1        | 0               | 2               | 1055      | test_payload_data_u8 | true        | false              | radef_kNoError           | Normal operation   |
 * | 2        | 1               | 2               | 1055      | test_payload_data_u8 | false       | false              | radef_kNoMessageReceived | Normal operation   |
 * | 3        | 1               | 2               | 1055      | test_payload_data_u8 | true        | false              | radef_kNoError           | Normal operation   |
 * | 4        | 2               | 2               | 1055      | test_payload_data_u8 | false       | true               | radef_kInvalidParameter  | Fatal error raised |
 * | 5        | 1               | 1               | 1055      | test_payload_data_u8 | false       | true               | radef_kInvalidParameter  | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-613} Read from Buffer Function
 * @verifyReq{RASW-609} Get Used Buffer Entries Function
 * @verifyReq{RASW-610} Get Free Buffer Entries Function
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-554} Message Received Notification
 * @verifyReq{RASW-279} Message Received Notification Structure
 * @verifyReq{RASW-302} Connection Identification
 *
 */
TEST_P(ReadFromBufferTestWithParam, srreceTest007VerifyReadFromBufferFunction)
{
    uint16_t i = 0;                                     // loop iterator
    srtyp_SrMessagePayload read_payload = {0, {}};      // payload read from the buffer

    // copy payload data
    for (i = 0; i < payload.payload_size; i++)
    {
        payload.payload[i] = payload_data_ptr[i];
    }

    // initialize module
    srrece_Init(n_max_conn, UT_SRRECE_NSENDMAX_MAX);

    if (expect_fatal_error)
    {
        // call the ReadFromBuffer function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(exp_retval)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));
        EXPECT_THROW(srrece_ReadFromBuffer(connection_id, &read_payload), std::invalid_argument);
    }
    else
    {
        if (do_add_message)
        {
            // add a message to the buffer
            EXPECT_CALL(srnot_mock, srnot_MessageReceivedNotification(connection_id)).Times(1);
            srrece_AddToBuffer(connection_id, &payload);

            // verify the buffer length and number of free entries before the read
            EXPECT_EQ(1, srrece_GetUsedBufferEntries(connection_id));
            EXPECT_EQ((UT_SRRECE_NSENDMAX_MAX - 1), srrece_GetFreeBufferEntries(connection_id));
        }

        // call the ReadFromBuffer function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_EQ(exp_retval, srrece_ReadFromBuffer(connection_id, &read_payload));

        // verify the buffer length and number of free entries after the read
        EXPECT_EQ(0, srrece_GetUsedBufferEntries(connection_id));
        EXPECT_EQ(UT_SRRECE_NSENDMAX_MAX, srrece_GetFreeBufferEntries(connection_id));

        if (do_add_message)
        {
            // verify the data
            ASSERT_EQ(payload.payload_size, read_payload.payload_size);
            for (i = 0; i < payload.payload_size; i++)
            {
                EXPECT_EQ(payload.payload[i], read_payload.payload[i]);
            }
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyReadFromBufferFunction,
    ReadFromBufferTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, false, false, radef_kNoMessageReceived),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, true, false, radef_kNoError),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, false, false, radef_kNoMessageReceived),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, true, false, radef_kNoError),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 2, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, false, true, radef_kInvalidParameter),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 1, UT_SRRECE_DATA_LEN_MAX, test_payload_data_u8, false, true, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{srreceTest008} Verify complete buffer
 *
 * @details     This test verifies add / read operations on the full buffer.
 *
 * Test steps:
 * - initialize the module with maximum nSendMax and maximum number of connections
 * - add the maximum number of messages to the buffer
 * - verify that the buffer is full
 * - read the maximum number of messages from the buffer
 * - verify that the buffer is empty
 * - verify the correctness of the data
 * - verify that no more message is to be read
 * .
 *
 * @testParameter
 * - Conn. Id: Connection identification to perform operations on
 * .
 * | Test Run | Test config    | Expected Result  |
 * |----------|----------------|------------------|
 * |          | Conn. Id       |                  |
 * | 0        | 0              | Normal operation |
 * | 1        | 1              | Normal operation |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-608} Add to Buffer Function
 * @verifyReq{RASW-609} Get Used Buffer Entries Function
 * @verifyReq{RASW-610} Get Free Buffer Entries Function
 * @verifyReq{RASW-613} Read from Buffer Function
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-554} Message Received Notification
 * @verifyReq{RASW-279} Message Received Notification Structure
 * @verifyReq{RASW-302} Connection Identification
 */
TEST_P(CompleteBufferTestWithParam, srreceTest008VerifyCompleteBuffer)
{
    uint8_t i = 0;                                                              // outer loop iterator
    uint16_t j = 0;                                                             // inner loop iterator
    srtyp_SrMessagePayload add_payload[UT_SRRECE_NSENDMAX_MAX] = {{0, {}}};     // array of payload data to add to the buffer
    srtyp_SrMessagePayload read_payload[UT_SRRECE_NSENDMAX_MAX] = {{0, {}}};    // array of payload data to be read from the buffer

    // generate payload data
    for (i = 0; i < UT_SRRECE_NSENDMAX_MAX; i++)
    {
        add_payload[i].payload_size = UT_SRRECE_DATA_LEN_MAX;
        for (j = 0; j < UT_SRRECE_DATA_LEN_MAX; j++)
        {
            add_payload[i].payload[j] = (uint8_t)(connection_id << 4) | i;
        }
    }

    // initialize module
    srrece_Init(UT_SRRECE_MAX_NUM_CONN, UT_SRRECE_NSENDMAX_MAX);

    // add the maximum number of messages to the buffer
    EXPECT_CALL(srnot_mock, srnot_MessageReceivedNotification(connection_id)).Times(UT_SRRECE_NSENDMAX_MAX);
    for (i = 0; i < UT_SRRECE_NSENDMAX_MAX; i++)
    {
        srrece_AddToBuffer(connection_id, &(add_payload[i]));
    }

    // verify that the buffer is full
    EXPECT_EQ(UT_SRRECE_NSENDMAX_MAX, srrece_GetUsedBufferEntries(connection_id));
    EXPECT_EQ(0, srrece_GetFreeBufferEntries(connection_id));

    // read the maximum number of messages from the buffer
    for (i = 0; i < UT_SRRECE_NSENDMAX_MAX; i++)
    {
        srrece_ReadFromBuffer(connection_id, &(read_payload[i]));
    }

    // verify that the buffer is empty
    EXPECT_EQ(0, srrece_GetUsedBufferEntries(connection_id));
    EXPECT_EQ(UT_SRRECE_NSENDMAX_MAX, srrece_GetFreeBufferEntries(connection_id));

    // verify the correctness of the data
    for (i = 0; i < UT_SRRECE_NSENDMAX_MAX; i++)
    {
        for (j = 0; j < UT_SRRECE_DATA_LEN_MAX; j++)
        {
            EXPECT_EQ(add_payload[i].payload[j], read_payload[i].payload[j]);
        }
    }

    // verify that no more message is to be read
    EXPECT_EQ(radef_kNoMessageReceived, srrece_ReadFromBuffer(connection_id, &(read_payload[0])));

}

INSTANTIATE_TEST_SUITE_P(verifyCompleteBuffer, CompleteBufferTestWithParam, ::testing::Values(0, 1));

/**
 * @test        @ID{srreceTest009} Verify interleaved add / read operations
 *
 * @details     This test verifies interleaved add / read operations on the full buffer.
 *
 * Test steps:
 * - generate test messages
 * - initialize the module with nSendMax = 3 and maximum number of connections
 * - set the message received notification expectation
 * - call the AddToBuffer and ReadFromBuffer functions 5 times for every connection index, interleaved
 * - verify that the data has been read back correctly
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-608} Add to Buffer Function
 * @verifyReq{RASW-613} Read from Buffer Function
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-554} Message Received Notification
 * @verifyReq{RASW-279} Message Received Notification Structure
 * @verifyReq{RASW-302} Connection Identification
 */
TEST_F(srreceTest, srreceTest009VerifyInterleavedAddRead)
{
    const uint16_t n_send_max = 3;                                      // max. buffer size
    const uint16_t number_of_adds = 5;                                  // number of messages to add
    const uint32_t connection_id_idx_0 = 0;                             // connection ID index 0
    const uint32_t connection_id_idx_1 = 1;                             // connection ID index 1
    uint8_t i = 0;                                                      // outer loop iterator
    uint16_t j = 0;                                                     // inner loop iterator
    srtyp_SrMessagePayload add_payload0[number_of_adds] = {{0, {}}};    // array of payload data to add to the buffer (index 0)
    srtyp_SrMessagePayload add_payload1[number_of_adds] = {{0, {}}};    // array of payload data to add to the buffer (index 1)
    srtyp_SrMessagePayload read_payload0[number_of_adds] = {{0, {}}};   // array of payload data to be read from the buffer (index 0)
    srtyp_SrMessagePayload read_payload1[number_of_adds] = {{0, {}}};   // array of payload data to be read from the buffer (index 1)

    // generate payload data
    for (i = 0; i < number_of_adds; i++)
    {
        add_payload0[i].payload_size = UT_SRRECE_DATA_LEN_MAX;
        add_payload1[i].payload_size = UT_SRRECE_DATA_LEN_MAX;
        for (j = 0; j < UT_SRRECE_DATA_LEN_MAX; j++)
        {
            add_payload0[i].payload[j] = (uint8_t)(0xA << 4) | i;
            add_payload1[i].payload[j] = (uint8_t)(0xB << 4) | i;
        }
    }

    // initialize module
    srrece_Init(UT_SRRECE_MAX_NUM_CONN, n_send_max);

    // add messages to the buffer and read them back, interleaved and on both connections
    EXPECT_CALL(srnot_mock, srnot_MessageReceivedNotification(connection_id_idx_0)).Times(number_of_adds);
    EXPECT_CALL(srnot_mock, srnot_MessageReceivedNotification(connection_id_idx_1)).Times(number_of_adds);

    srrece_AddToBuffer(connection_id_idx_0, &(add_payload0[0]));
    srrece_AddToBuffer(connection_id_idx_1, &(add_payload1[0]));
    srrece_AddToBuffer(connection_id_idx_1, &(add_payload1[1]));
    srrece_AddToBuffer(connection_id_idx_0, &(add_payload0[1]));
    srrece_AddToBuffer(connection_id_idx_0, &(add_payload0[2]));

    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_1, &(read_payload1[0])));
    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_0, &(read_payload0[0])));
    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_0, &(read_payload0[1])));

    srrece_AddToBuffer(connection_id_idx_1, &(add_payload1[2]));
    srrece_AddToBuffer(connection_id_idx_0, &(add_payload0[3]));

    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_0, &(read_payload0[2])));
    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_1, &(read_payload1[1])));
    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_1, &(read_payload1[2])));

    srrece_AddToBuffer(connection_id_idx_0, &(add_payload0[4]));
    srrece_AddToBuffer(connection_id_idx_1, &(add_payload1[3]));
    srrece_AddToBuffer(connection_id_idx_1, &(add_payload1[4]));

    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_0, &(read_payload0[3])));
    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_1, &(read_payload1[3])));
    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_1, &(read_payload1[4])));
    EXPECT_EQ(radef_kNoError, srrece_ReadFromBuffer(connection_id_idx_0, &(read_payload0[4])));


    // verify the correctness of the data
    for (i = 0; i < number_of_adds; i++)
    {
        for (j = 0; j < UT_SRRECE_DATA_LEN_MAX; j++)
        {
            EXPECT_EQ(add_payload0[i].payload[j], read_payload0[i].payload[j]);
            EXPECT_EQ(add_payload1[i].payload[j], read_payload1[i].payload[j]);
        }
    }
}

/**
 * @test        @ID{srreceTest012} Verify the GetPayloadSizeOfNextMessageToRead function.
 *
 * @details     This test verifies the correct behaviour of the GetPayloadSizeOfNextMessageToRead function.
 *
 * Test steps:
 * - initialize the module with maximum nSendMax and given maximum number of connections
 * - set the fatal error expectation based on the test parameter
 * - add a message (if activated by the test parameter)
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the GetPayloadSizeOfNextMessageToRead function
 * - verify the payload size returned
 * .
 *
 * @testParameter
 * - Conn. Id: RaSTA connection identification
 * - Max. Num. Conn.: Number of configured connections
 * - Data Size: Payload size of message
 * - Add Message: Indicates if a message gets added to buffer before calling srrece_GetPayloadSizeOfNextMessageToRead function
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * - Exp. Return Value: Expected return value from srrece_GetPayloadSizeOfNextMessageToRead function call
 * .
 * | Test Run | Input parameter | Test config                             ||| Expected values                                           |||
 * |----------|-----------------|-----------------|-----------|-------------|--------------------|-------------------|--------------------|
 * |          | Conn. Id        | Max. Num. Conn. | Data Size | Add Message | Expect Fatal Error | Exp. Return Value | Test Result        |
 * | 0        | 0               | 2               | 1055      | false       | false              | 0                 | Normal operation   |
 * | 1        | 0               | 2               | 1         | true        | false              | 1                 | Normal operation   |
 * | 2        | 0               | 2               | 1055      | true        | false              | 1055              | Normal operation   |
 * | 3        | 1               | 2               | 1055      | false       | false              | 0                 | Normal operation   |
 * | 4        | 1               | 2               | 1         | true        | false              | 1                 | Normal operation   |
 * | 5        | 1               | 2               | 1055      | true        | false              | 1055              | Normal operation   |
 * | 6        | 2               | 2               | 1055      | false       | true               | 0                 | Fatal error raised |
 * | 7        | 1               | 1               | 1055      | false       | true               | 0                 | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-823} Get Payload Size of Next Message To Read Function
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-554} Message Received Notification
 * @verifyReq{RASW-279} Message Received Notification Structure
 * @verifyReq{RASW-302} Connection Identification
 */
TEST_P(GetNextPayloadSizeTestWithParam, srreceTest012VerifyGetPayloadSizeOfNextMessageToReadFunction)
{
    uint16_t i = 0;                                     // loop iterator

    // initialize module
    srrece_Init(n_max_conn, UT_SRRECE_NSENDMAX_MAX);

    if (expect_fatal_error)
    {
        // call the GetPayloadSizeOfNextMessageToRead function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));
        EXPECT_THROW(srrece_GetPayloadSizeOfNextMessageToRead(connection_id), std::invalid_argument);
    }
    else
    {
        if (do_add_message)
        {
            // add a message to the buffer
            EXPECT_CALL(srnot_mock, srnot_MessageReceivedNotification(connection_id)).Times(1);
            srrece_AddToBuffer(connection_id, &payload);
        }

        // call the GetPayloadSizeOfNextMessageToRead function
        EXPECT_EQ(exp_retval, srrece_GetPayloadSizeOfNextMessageToRead(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetPayloadSizeOfNextMessageToReadFunction,
    GetNextPayloadSizeTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, UT_SRRECE_DATA_LEN_MAX, false, false, 0),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, UT_SRRECE_DATA_LEN_MIN, true, false, UT_SRRECE_DATA_LEN_MIN),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, UT_SRRECE_DATA_LEN_MAX, true, false, UT_SRRECE_DATA_LEN_MAX),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, UT_SRRECE_DATA_LEN_MAX, false, false, 0),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, UT_SRRECE_DATA_LEN_MIN, true, false, UT_SRRECE_DATA_LEN_MIN),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, UT_SRRECE_DATA_LEN_MAX, true, false, UT_SRRECE_DATA_LEN_MAX),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 2, UT_SRRECE_DATA_LEN_MAX, false, true, 0),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 1, UT_SRRECE_DATA_LEN_MAX, false, true, 0)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
