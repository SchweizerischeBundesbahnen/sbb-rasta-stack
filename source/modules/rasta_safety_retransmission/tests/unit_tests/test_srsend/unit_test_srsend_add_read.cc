/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srsend_add_read.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the send buffer module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srsend.hh"

// -----------------------------------------------------------------------------
// Test class definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized AddToBuffer test
 *
 * Parameter order:
 * - do init buffer (bool)
 * - connection ID (uint32_t)
 * - message size (uint16_t)
 * - message data pointer (uint8_t *)
 * - expected buffer length after add (uint8_t)
 * - do read back check (bool)
 * - expected error code (radef_RaStaReturnCode)
 * .
 */
class AddToBufferTestWithParam :    public srsendTest,
                                    public testing::WithParamInterface<std::tuple<bool, uint32_t, uint16_t, uint8_t *, uint8_t, bool, radef_RaStaReturnCode>>
{
    public:
    bool do_initialize =                std::get<0>(GetParam());        ///< initialize module
    uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
    srtyp_SrMessage message =           {std::get<2>(GetParam()), {}};  ///< message
    uint8_t * message_data_ptr =        std::get<3>(GetParam());        ///< pointer to message data
    uint8_t exp_buffer_length =         std::get<4>(GetParam());        ///< expected buffer length after write
    bool do_read_back_check =           std::get<5>(GetParam());        ///< indicates if a read back check is performed
    radef_RaStaReturnCode error =       std::get<6>(GetParam());        ///< expected error code
};

/**
 * @brief Test class for value-parametrized ReadMessageToSend test
 *
 * Parameter order:
 * - do init buffer (bool)
 * - connection ID (uint32_t)
 * - message size (uint16_t)
 * - message data pointer (uint8_t *)
 * - do add message (bool)
 * - expect fatal error (bool)
 * - expected return value (radef_RaStaReturnCode)
 * .
 */
class ReadMessageToSendTestWithParam :  public srsendTest,
                                        public testing::WithParamInterface<std::tuple<bool, uint32_t, uint16_t, uint8_t *, bool, bool, radef_RaStaReturnCode>>
{
    public:
    bool do_initialize =                std::get<0>(GetParam());        ///< initialize module
    uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID index
    srtyp_SrMessage message =           {std::get<2>(GetParam()), {}};  ///< message
    uint8_t * message_data_ptr =        std::get<3>(GetParam());        ///< pointer to message data
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
class CompleteBufferTestWithParam : public srsendTest,
                                    public testing::WithParamInterface<uint32_t>
{
    public:
    uint32_t connection_id =            GetParam();    ///< connection ID
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_sendBuf
 * @{
 */

/**
 * @test        @ID{srsendTest006} Verify the AddToBuffer function.
 *
 * @details     This test verifies that correct behaviour of the AddToBuffer function.
 *
 * Test steps:
 * - initialize the module with maximum number of connections
 * - set the fatal error expectation based on the test parameter
 * - verify the buffer length and free entries before the add
 * - call the AddToBuffer function
 * - verify the buffer length and free entries after the add
 * - perform a read back check using the ReadFromBuffer function and verify the data correctness
 * - verify the buffer length and free entries after the read
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Do init: Indicates if module gets initialized
 * - Data Size: Size configured in message
 * - Data Pointer: Pointer to buffer which contains test message
 * - Read back check: Indicates if a read back gets performed after calling srsend_AddToBuffer function
 * - Exp. Buf. Length: Expected buffer length after calling srsend_AddToBuffer
 * - Exp. Error Code: Expected error code when calling srsend_AddToBuffer function
 * .
 * | Test Run | Input parameter | Test config                      ||| Expected values                                                                 ||||
 * |----------|-----------------|---------|-----------|--------------|-----------------|------------------|--------------------------|--------------------|
 * |          | Connection Id   | Do init | Data Size | Data Pointer | Read back check | Exp. Buf. Length | Exp. Error Code          | Test Result        |
 * | 0        | 0               | true    | 1055      | testDataU8   | true            | 1                | radef_kNoError           | Normal operation   |
 * | 1        | 0               | true    | 1055      | testDataU8   | false           | 1                | radef_kNoError           | Normal operation   |
 * | 2        | 0               | false   | 1055      | testDataU8   | false           | 2                | radef_kNoError           | Normal operation   |
 * | 3        | 0               | false   | 1055      | testDataU8   | false           | 3                | radef_kNoError           | Normal operation   |
 * | 4        | 0               | false   | 1055      | testDataU8   | false           | 4                | radef_kNoError           | Normal operation   |
 * | 5        | 0               | false   | 1055      | testDataU8   | false           | 5                | radef_kNoError           | Normal operation   |
 * | 6        | 0               | false   | 1055      | testDataU8   | false           | 6                | radef_kNoError           | Normal operation   |
 * | 7        | 0               | false   | 1055      | testDataU8   | false           | 7                | radef_kNoError           | Normal operation   |
 * | 8        | 0               | false   | 1055      | testDataU8   | false           | 8                | radef_kNoError           | Normal operation   |
 * | 9        | 0               | false   | 1055      | testDataU8   | false           | 9                | radef_kNoError           | Normal operation   |
 * | 10       | 0               | false   | 1055      | testDataU8   | false           | 10               | radef_kNoError           | Normal operation   |
 * | 11       | 0               | false   | 1055      | testDataU8   | false           | 11               | radef_kNoError           | Normal operation   |
 * | 12       | 0               | false   | 1055      | testDataU8   | false           | 12               | radef_kNoError           | Normal operation   |
 * | 13       | 0               | false   | 1055      | testDataU8   | false           | 13               | radef_kNoError           | Normal operation   |
 * | 14       | 0               | false   | 1055      | testDataU8   | false           | 14               | radef_kNoError           | Normal operation   |
 * | 15       | 0               | false   | 1055      | testDataU8   | false           | 15               | radef_kNoError           | Normal operation   |
 * | 16       | 0               | false   | 1055      | testDataU8   | false           | 16               | radef_kNoError           | Normal operation   |
 * | 17       | 0               | false   | 1055      | testDataU8   | false           | 17               | radef_kNoError           | Normal operation   |
 * | 18       | 0               | false   | 1055      | testDataU8   | false           | 18               | radef_kNoError           | Normal operation   |
 * | 19       | 0               | false   | 1055      | testDataU8   | false           | 19               | radef_kNoError           | Normal operation   |
 * | 20       | 0               | false   | 1055      | testDataU8   | false           | 20               | radef_kNoError           | Normal operation   |
 * | 21       | 0               | false   | 1055      | testDataU8   | false           | 20               | radef_kSendBufferFull    | Fatal error raised |
 * |          | vary connection id                                                                                             |||||||                    |
 * | 22       | 1               | true    | 1055      | testDataU8   | true            | 1                | radef_kNoError           | Normal operation   |
 * | 23       | 1               | true    | 1055      | testDataU8   | false           | 1                | radef_kNoError           | Normal operation   |
 * | 24       | 1               | false   | 1055      | testDataU8   | false           | 2                | radef_kNoError           | Normal operation   |
 * | 25       | 1               | false   | 1055      | testDataU8   | false           | 3                | radef_kNoError           | Normal operation   |
 * | 26       | 1               | false   | 1055      | testDataU8   | false           | 4                | radef_kNoError           | Normal operation   |
 * | 27       | 1               | false   | 1055      | testDataU8   | false           | 5                | radef_kNoError           | Normal operation   |
 * | 28       | 1               | false   | 1055      | testDataU8   | false           | 6                | radef_kNoError           | Normal operation   |
 * | 29       | 1               | false   | 1055      | testDataU8   | false           | 7                | radef_kNoError           | Normal operation   |
 * | 30       | 1               | false   | 1055      | testDataU8   | false           | 8                | radef_kNoError           | Normal operation   |
 * | 31       | 1               | false   | 1055      | testDataU8   | false           | 9                | radef_kNoError           | Normal operation   |
 * | 32       | 1               | false   | 1055      | testDataU8   | false           | 10               | radef_kNoError           | Normal operation   |
 * | 33       | 1               | false   | 1055      | testDataU8   | false           | 11               | radef_kNoError           | Normal operation   |
 * | 34       | 1               | false   | 1055      | testDataU8   | false           | 12               | radef_kNoError           | Normal operation   |
 * | 35       | 1               | false   | 1055      | testDataU8   | false           | 13               | radef_kNoError           | Normal operation   |
 * | 36       | 1               | false   | 1055      | testDataU8   | false           | 14               | radef_kNoError           | Normal operation   |
 * | 37       | 1               | false   | 1055      | testDataU8   | false           | 15               | radef_kNoError           | Normal operation   |
 * | 38       | 1               | false   | 1055      | testDataU8   | false           | 16               | radef_kNoError           | Normal operation   |
 * | 39       | 1               | false   | 1055      | testDataU8   | false           | 17               | radef_kNoError           | Normal operation   |
 * | 40       | 1               | false   | 1055      | testDataU8   | false           | 18               | radef_kNoError           | Normal operation   |
 * | 41       | 1               | false   | 1055      | testDataU8   | false           | 19               | radef_kNoError           | Normal operation   |
 * | 42       | 1               | false   | 1055      | testDataU8   | false           | 20               | radef_kNoError           | Normal operation   |
 * | 43       | 1               | false   | 1055      | testDataU8   | false           | 20               | radef_kSendBufferFull    | Fatal error raised |
 * | 44       | 2               | false   | 1055      | testDataU8   | false           | 1                | radef_kInvalidParameter  | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-596} Add to Buffer Function
 * @verifyReq{RASW-597} Get Used Buffer Entries Function
 * @verifyReq{RASW-598} Get Free Buffer Entries Function
 */
TEST_P(AddToBufferTestWithParam, srsendTest006VerifyAddToBufferFunction)
{
    uint16_t i = 0;                                     // loop iterator
    srtyp_SrMessage read_message = {0, {}};             // payload read from the buffer

    // copy payload data
    for (i = 0; i < message.message_size; i++)
    {
        message.message[i] = message_data_ptr[i];
    }

    // initialize module
    if(do_initialize)
    {
        srsend_Init(UT_SRSEND_MAX_NUM_CONN);
    }
    else
    {
        srsend_initialized = true;
    }

    if (error != radef_kNoError)
    {
        // call the addToBuffer function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(error)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));
        EXPECT_THROW(srsend_AddToBuffer(connection_id, &message), std::invalid_argument);
    }
    else
    {
        // verify the buffer length and number of free entries before the add
        EXPECT_EQ((exp_buffer_length - 1), srsend_GetUsedBufferEntries(connection_id));
        EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - exp_buffer_length + 1), srsend_GetFreeBufferEntries(connection_id));

        // call the AddToBuffer function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_NO_THROW(srsend_AddToBuffer(connection_id, &message));

        // verify the buffer length and number of free entries after the add
        EXPECT_EQ(exp_buffer_length, srsend_GetUsedBufferEntries(connection_id));
        EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - exp_buffer_length), srsend_GetFreeBufferEntries(connection_id));

        if (do_read_back_check)
        {
            // call the readFromBuffer function
            EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id, &read_message));

            // verify the buffer length and number of free entries after the read
            EXPECT_EQ(exp_buffer_length, srsend_GetUsedBufferEntries(connection_id));
            EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - exp_buffer_length), srsend_GetFreeBufferEntries(connection_id));

            // verify the data
            ASSERT_EQ(message.message_size, read_message.message_size);
            for (i = 0; i < message.message_size; i++)
            {
                EXPECT_EQ(message.message[i], read_message.message[i]);
            }
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyAddToBufferFunction,
    AddToBufferTestWithParam,
    ::testing::Values(
        std::make_tuple(true, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 1, true, radef_kNoError),
        std::make_tuple(true, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 1, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 2, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 3, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 4, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 5, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 6, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 7, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 8, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 9, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 10, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 11, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 12, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 13, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 14, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 15, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 16, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 17, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 18, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 19, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 20, false, radef_kNoError),
        std::make_tuple(false, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 20, false, radef_kSendBufferFull),
        // vary connection id
        std::make_tuple(true, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 1, true, radef_kNoError),
        std::make_tuple(true, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 1, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 2, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 3, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 4, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 5, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 6, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 7, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 8, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 9, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 10, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 11, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 12, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 13, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 14, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 15, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 16, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 17, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 18, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 19, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 20, false, radef_kNoError),
        std::make_tuple(false, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 20, false, radef_kSendBufferFull),
        std::make_tuple(false, 2, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, 1, false, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{srsendTest007} Verify the ReadMessageToSend function.
 *
 * @details     This test verifies that correct behaviour of the ReadMessageToSend function.
 *
 * Test steps:
 * - initialize the module with maximum number of connections if given by test parameter
 * - set the fatal error expectation based on the test parameter
 * - if activated by the test parameter, add a message via AddToBuffer function and verify that the buffer size is incremented
 * - call the ReadMessageToSend function and verify that the buffer size is not decremented after the read
 * - verify that the read data matches the written data
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Do init: Indicates if module gets initialized
 * - Data Size: Message size of message to read
 * - Message Pointer: Pointer to buffer which contains test message
 * - Add Message: Indicates if a message gets added before calling srsend_ReadMessageToSend function
 * - Exp. Fatal Error: Indicates if a fatal error is expected
 * - Exp. Return Value: Expected return code from srsend_ReadMessageToSend function
 * .
 * | Test Run | Input parameter | Test config                                        |||| Expected values                                               |||
 * |----------|-----------------|---------|-----------|-------------------|-------------|------------------|-------------------------|--------------------|
 * |          | Connection Id   | Do init | Data Size | Message Pointer   | Add Message | Exp. Fatal Error | Exp. Return Value       | Test Result        |
 * | 0        | 0               | true    | 1055      | testMessageDataU8 | false       | false            | radef_kNoMessageToSend  | Normal operation   |
 * | 1        | 0               | true    | 1055      | testMessageDataU8 | true        | false            | radef_kNoError          | Normal operation   |
 * | 2        | 1               | true    | 1055      | testMessageDataU8 | false       | false            | radef_kNoMessageToSend  | Normal operation   |
 * | 3        | 1               | true    | 1055      | testMessageDataU8 | true        | false            | radef_kNoError          | Normal operation   |
 * | 4        | 2               | false   | 1055      | testMessageDataU8 | false       | true             | radef_kInvalidParameter | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-604} Read Message to Send Function
 * @verifyReq{RASW-597} Get Used Buffer Entries Function
 * @verifyReq{RASW-598} Get Free Buffer Entries Function
 */
TEST_P(ReadMessageToSendTestWithParam, srsendTest007VerifyReadFromBufferFunction)
{
    uint16_t i = 0;                                     // loop iterator
    uint8_t expected_buffer_size = 0;                   // expected buffer size
    srtyp_SrMessage read_message = {0, {}};             // payload read from the buffer

    // copy payload data
    for (i = 0; i < message.message_size; i++)
    {
        message.message[i] = message_data_ptr[i];
    }

    // initialize module
    if(do_initialize)
    {
        srsend_Init(UT_SRSEND_MAX_NUM_CONN);
    }
    else
    {
        srsend_initialized = true;
    }

    if (expect_fatal_error)
    {
        // call the ReadMessageToSend function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));
        EXPECT_THROW(srsend_ReadMessageToSend(connection_id, &read_message), std::invalid_argument);
    }
    else
    {
        if (do_add_message)
        {
            // add a message to the buffer
            srsend_AddToBuffer(connection_id, &message);
            expected_buffer_size = 1;

            // verify the buffer length and number of free entries before the read
            EXPECT_EQ(expected_buffer_size, srsend_GetUsedBufferEntries(connection_id));
            EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - expected_buffer_size), srsend_GetFreeBufferEntries(connection_id));
        }

        // call the ReadMessageToSend function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_EQ(exp_retval, srsend_ReadMessageToSend(connection_id, &read_message));

        // verify the buffer length and number of free entries after the read
        EXPECT_EQ(expected_buffer_size, srsend_GetUsedBufferEntries(connection_id));
        EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - expected_buffer_size), srsend_GetFreeBufferEntries(connection_id));

        if (do_add_message)
        {
            // verify the data
            ASSERT_EQ(message.message_size, read_message.message_size);
            for (i = 0; i < message.message_size; i++)
            {
                EXPECT_EQ(message.message[i], read_message.message[i]);
            }
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyReadFromBufferFunction,
    ReadMessageToSendTestWithParam,
    ::testing::Values(
        std::make_tuple(true, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, false, false, radef_kNoMessageToSend),
        std::make_tuple(true, 0, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, true, false, radef_kNoError),
        std::make_tuple(true, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, false, false, radef_kNoMessageToSend),
        std::make_tuple(true, 1, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, true, false, radef_kNoError),
        std::make_tuple(false, 2, UT_SRSEND_MESSAGE_LEN_MAX, test_message_data_u8, false, true, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{srsendTest008} Verify complete buffer
 *
 * @details     This test verifies add / read operations on the full buffer.
 *
 * Test steps:
 * - initialize the module with maximum number of connections
 * - add the maximum number of messages to the buffer using the AddToBuffer function
 * - verify that the buffer is full
 * - read the maximum number of messages from the buffer using the ReadMessageToSend function
 * - verify the that the buffer is still full
 * - verify that the read data matches the written data
 * .
 *
 * @testParameter
 * - Connection Id: Connection identification to perform operations on
 * .
 * | Test Run | Test Config    | Expected Result  |
 * |----------|----------------|------------------|
 * |          | Connection Id  |                  |
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
 * @verifyReq{RASW-596} Add to Buffer Function
 * @verifyReq{RASW-597} Get Used Buffer Entries Function
 * @verifyReq{RASW-598} Get Free Buffer Entries Function
 * @verifyReq{RASW-599} Get Number of Messages to Send Function
 * @verifyReq{RASW-604} Read Message to Send Function
 */
TEST_P(CompleteBufferTestWithParam, srsendTest008VerifyCompleteBuffer)
{
    uint8_t i = 0;                                                      // outer loop iterator
    uint16_t j = 0;                                                     // inner loop iterator
    srtyp_SrMessage add_message[UT_SRSEND_NSENDMAX_MAX] = {{0, {}}};    // array of messages to add to the buffer
    srtyp_SrMessage read_message[UT_SRSEND_NSENDMAX_MAX] = {{0, {}}};   // array of messages to be read from the buffer

    // generate message data
    for (i = 0; i < UT_SRSEND_NSENDMAX_MAX; i++)
    {
        add_message[i].message_size = UT_SRSEND_MESSAGE_LEN_MAX;
        for (j = 0; j < UT_SRSEND_MESSAGE_LEN_MAX; j++)
        {
            add_message[i].message[j] = (uint8_t)(connection_id << 4) | i;
        }
    }

    // initialize module
    srsend_Init(UT_SRSEND_MAX_NUM_CONN);

    // add the maximum number of messages to the buffer
    for (i = 0; i < UT_SRSEND_NSENDMAX_MAX; i++)
    {
        srsend_AddToBuffer(connection_id, &(add_message[i]));
        // verify that the number of messages to send and the buffer elements are increasing
        EXPECT_EQ((i + 1), srsend_GetNumberOfMessagesToSend(connection_id));
        EXPECT_EQ((i + 1), srsend_GetUsedBufferEntries(connection_id));
        // verify that the free buffer entries are decreasing
        EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - i - 1), srsend_GetFreeBufferEntries(connection_id));
    }

    // verify that the buffer is full
    EXPECT_EQ(UT_SRSEND_NSENDMAX_MAX, srsend_GetUsedBufferEntries(connection_id));
    EXPECT_EQ(0, srsend_GetFreeBufferEntries(connection_id));

    // read the maximum number of messages from the buffer
    for (i = 0; i < UT_SRSEND_NSENDMAX_MAX; i++)
    {
        srsend_ReadMessageToSend(connection_id, &(read_message[i]));
        // verify that the number of messages to send is decreasing
        EXPECT_EQ((UT_SRSEND_NSENDMAX_MAX - i - 1), srsend_GetNumberOfMessagesToSend(connection_id));
    }

    // verify that the buffer is still full
    EXPECT_EQ(UT_SRSEND_NSENDMAX_MAX, srsend_GetUsedBufferEntries(connection_id));
    EXPECT_EQ(0, srsend_GetFreeBufferEntries(connection_id));

    // verify the correctness of the data
    for (i = 0; i < UT_SRSEND_NSENDMAX_MAX; i++)
    {
        for (j = 0; j < UT_SRSEND_MESSAGE_LEN_MAX; j++)
        {
            EXPECT_EQ(add_message[i].message[j], read_message[i].message[j]);
        }
    }

}

INSTANTIATE_TEST_SUITE_P(verifyCompleteBuffer, CompleteBufferTestWithParam, ::testing::Values(0, 1));

/**
 * @test        @ID{srsendTest009} Verify interleaved add / read operations
 *
 * @details     This test verifies interleaved add / read operations on the full buffer.
 *
 * Test steps:
 * - generate test messages
 * - initialize the module with the maximum number of connections
 * - call the AddToBuffer and ReadMessageToSend functions 20 times for every connection index, interleaved
 * - verify that the read data matches the written data
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-596} Add to Buffer Function
 * @verifyReq{RASW-604} Read Message to Send Function
 */
TEST_F(srsendTest, srsendTest009VerifyInterleavedAddRead)
{
    const uint16_t number_of_adds = UT_SRSEND_NSENDMAX_MAX;         // number of messages to add
    const uint32_t connection_id_idx_0 = 0;                         // connection ID index 0
    const uint32_t connection_id_idx_1 = 1;                         // connection ID index 1
    uint8_t i = 0;                                                  // outer loop iterator
    uint16_t j = 0;                                                 // inner loop iterator
    srtyp_SrMessage add_message0[number_of_adds] = {{0, {}}};       // array of messages to add to the buffer (index 0)
    srtyp_SrMessage add_message1[number_of_adds] = {{0, {}}};       // array of messages to add to the buffer (index 1)
    srtyp_SrMessage read_message0[number_of_adds] = {{0, {}}};      // array of messages to be read from the buffer (index 0)
    srtyp_SrMessage read_message1[number_of_adds] = {{0, {}}};      // array of messages to be read from the buffer (index 1)

    // generate test messages
    for (i = 0; i < number_of_adds; i++)
    {
        add_message0[i].message_size = UT_SRSEND_MESSAGE_LEN_MAX;
        add_message1[i].message_size = UT_SRSEND_MESSAGE_LEN_MAX;
        for (j = 0; j < UT_SRSEND_MESSAGE_LEN_MAX; j++)
        {
            add_message0[i].message[j] = (uint8_t)(0x1 << 6) | i;
            add_message1[i].message[j] = (uint8_t)(0x3 << 6) | i;
        }
    }

    // initialize module
    srsend_Init(UT_SRSEND_MAX_NUM_CONN);

    // add the maximum number of messages to the buffer and read them back
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[0]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[0]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[1]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[1]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[2]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[2]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[3]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[3]));

    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[0])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[1])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[2])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[3])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[0])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[1])));

    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[4]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[4]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[5]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[5]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[6]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[6]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[7]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[8]));

    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[2])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[4])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[5])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[3])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[4])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[5])));

    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[9]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[7]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[8]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[9]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[10]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[11]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[12]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[13]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[14]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[15]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[10]));

    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[6])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[6])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[7])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[7])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[8])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[9])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[10])));

    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[11]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[12]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[16]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[13]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[14]));

    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[11])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[12])));

    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[15]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[16]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[17]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[18]));

    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[8])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[9])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[10])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[11])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[13])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[14])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[15])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[12])));

    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[17]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[18]));
    srsend_AddToBuffer(connection_id_idx_1, &(add_message1[19]));
    srsend_AddToBuffer(connection_id_idx_0, &(add_message0[19]));

    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[13])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[14])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[15])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[16])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[16])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[17])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[18])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_0, &(read_message0[19])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[17])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[18])));
    EXPECT_EQ(radef_kNoError, srsend_ReadMessageToSend(connection_id_idx_1, &(read_message1[19])));

    // verify the correctness of the data
    for (i = 0; i < number_of_adds; i++)
    {
        for (j = 0; j < UT_SRSEND_MESSAGE_LEN_MAX; j++)
        {
            EXPECT_EQ(add_message0[i].message[j], read_message0[i].message[j]);
            EXPECT_EQ(add_message1[i].message[j], read_message1[i].message[j]);
        }
    }
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
