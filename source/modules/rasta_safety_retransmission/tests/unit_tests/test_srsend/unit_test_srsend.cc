/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srsend.cc
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
 * @brief Test class for value-parametrized init function test
 *
 * Parameter order:
 * - configured connections (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public srsendTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
    public:
    uint32_t configured_connections =   std::get<0>(GetParam());        ///< number of configured connections
    bool expect_fatal_error =           std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized init buffer function test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection id (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitBufferTestWithParam : public srsendTest,
                                public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
    public:
    uint32_t n_max_conn =           std::get<0>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =        std::get<1>(GetParam());        ///< connection id
    bool expect_fatal_error =       std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized buffer state function tests
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection id (uint32_t)
 * .
 */
class BufferStateTestWithParam :    public srsendTest,
                                    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t>>
{
    public:
    uint32_t n_max_conn =            std::get<0>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =         std::get<1>(GetParam());        ///< connection id
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_sendBuf
 * @{
 */

/**
 * @test        @ID{srsendTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the number of configured
 *              connections correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the init function with given test parameter
 * - verify if the number of connections is set correctly
 * - verify that the module is set to initialized state
 * .
 *
 * @testParameter
 * - Configured Connections: Number of connections
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter        | Test config        | Expected values    |
 * |----------|------------------------|--------------------|--------------------|
 * |          | Configured Connections | Expect Fatal Error | Test Result        |
 * | 0        | 0                      | True               | Fatal error raised |
 * | 1        | 1                      | False              | Normal operation   |
 * | 2        | 2                      | False              | Normal operation   |
 * | 3        | 3                      | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-600} Init sr_send_buffer Function
 */
TEST_P(InitTestWithParam, srsendTest001VerifyInitFunction)
{
    extern uint32_t srsend_number_of_connections;

    if (expect_fatal_error)
    {
        // call the init function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));
        EXPECT_THROW(srsend_Init(configured_connections), std::invalid_argument);
    }
    else
    {
        // call the init function and verify if parameter are set correctly
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_NO_THROW(srsend_Init(configured_connections));

        EXPECT_EQ(srsend_number_of_connections, configured_connections);
        EXPECT_TRUE(srsend_initialized);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyInitFunction,
    InitTestWithParam,
    ::testing::Values(
        std::make_tuple((UT_SRSEND_MIN_NUM_CONN - 1), true),
        std::make_tuple(UT_SRSEND_MIN_NUM_CONN, false),
        std::make_tuple(UT_SRSEND_MAX_NUM_CONN, false),
        std::make_tuple((UT_SRSEND_MAX_NUM_CONN + 1), true)
    ));

/**
 * @test        @ID{srsendTest002} Verify the init buffer function.
 *
 * @details     This test verifies that the init buffer function initializes
 *              the dedicated send buffer correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the init buffer function with given test parameter and verify that it completes successfully.
 * - verify that the buffer is empty
 * .
 *
 * @testParameter
 * - Connection ID: RaSTA connection identification
 * - Max. Connections: Number of configured connections
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config      | Expected values                        ||
 * |----------|-----------------|------------------|--------------------|--------------------|
 * |          | Connection ID   | Max. Connections | Expect Fatal Error | Test Result        |
 * | 0        | 0               | 2                | False              | Normal operation   |
 * | 1        | 1               | 2                | False              | Normal operation   |
 * | 2        | 1               | 1                | True               | Fatal error raised |
 * | 3        | 2               | 2                | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-601} Init Buffer Function
 */
TEST_P(InitBufferTestWithParam, srsendTest002VerifyInitBufferFunction)
{
    // initialize the module
    srsend_Init(n_max_conn);

    if (expect_fatal_error)
    {
        // call the init function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));
        EXPECT_THROW(srsend_InitBuffer(connection_id), std::invalid_argument);
    }
    else
    {
        // call the init function and verify if it completes successfully
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_NO_THROW(srsend_InitBuffer(connection_id));

        // verify that the buffer is empty
        EXPECT_EQ(srsend_GetFreeBufferEntries(connection_id), UT_SRSEND_NSENDMAX_MAX);
        EXPECT_EQ(srsend_GetUsedBufferEntries(connection_id), 0);
        EXPECT_EQ(srsend_GetNumberOfMessagesToSend(connection_id), 0);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyInitBufferFunction,
    InitBufferTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRSEND_MAX_NUM_CONN, 0, false),
        std::make_tuple(UT_SRSEND_MAX_NUM_CONN, 1, false),
        std::make_tuple(UT_SRSEND_MIN_NUM_CONN, 1, true),
        std::make_tuple(UT_SRSEND_MAX_NUM_CONN, 2, true)
    ));

/**
 * @test        @ID{srsendTest003} Verify the number of init function calls
 *
 * @details     This test verifies that the init function can only be called once.
 *
 * Test steps:
 * - call the init function and verify that it completes successfully
 * - call the init function again and verify that a radef_kAlreadyInitialized error is raised
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-601} Init Buffer Function
 */
TEST_F(srsendTest, srsendTest003VerifyInitFunctionCalls)
{
    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &srsendTest::invalidArgumentException));

    // call the init function and verify that it completes successfully
    EXPECT_NO_THROW(srsend_Init(UT_SRSEND_MIN_NUM_CONN));

    // call the init function again and verify that an assertion violations is raised as the module is already initialized
    EXPECT_THROW(srsend_Init(UT_SRSEND_MIN_NUM_CONN), std::invalid_argument);
}

/**
 * @test        @ID{srsendTest004} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - initialize the module
 * - set expectation for the fatal error mock
 * - call the AddToBuffer function with NULL pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the ReadMessageToSend function with NULL pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the PrepareBufferForRetr function with NULL pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-520} Error Handling
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-596} Add to Buffer Function
 * @verifyReq{RASW-604} Read Message to Send Function
 * @verifyReq{RASW-603} Prepare Buffer for Retransmission Function
 */
TEST_F(srsendTest, srsendTest004VerifyNullPointerCheck)
{
    const uint8_t nbr_of_function_calls = 3;        // number of function calls

    // Test variables
    uint32_t connection_id = UT_SRSEND_MIN_NUM_CONN;
    uint32_t sequence_number_for_retransmission = 0;
    srtyp_SrMessageHeaderCreate create_header = {};

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srsendTest::invalidArgumentException));

    // initialize the module
    srsend_Init(UT_SRSEND_MIN_NUM_CONN);

    // call the functions
    EXPECT_THROW(srsend_AddToBuffer(connection_id, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srsend_ReadMessageToSend(connection_id, (srtyp_SrMessage *)NULL), std::invalid_argument);
    EXPECT_THROW(srsend_PrepareBufferForRetr(connection_id, sequence_number_for_retransmission, create_header, (uint32_t *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{srsendTest005} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the InitBuffer function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the AddToBuffer function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ReadMessageToSend function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the PrepareBufferForRetr function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the IsSequenceNumberInBuffer function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the RemoveFromBuffer function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetFreeBufferEntries function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetUsedBufferEntries function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetNumberOfMessagesToSend function without prior initialization and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-595} Component sr_send_buffer Overview
 * @verifyReq{RASW-596} Add to Buffer Function
 * @verifyReq{RASW-597} Get Used Buffer Entries Function
 * @verifyReq{RASW-598} Get Free Buffer Entries Function
 * @verifyReq{RASW-599} Get Number of Messages to Send Function
 * @verifyReq{RASW-601} Init Buffer Function
 * @verifyReq{RASW-602} Is Sequence Number in Buffer Function
 * @verifyReq{RASW-603} Prepare Buffer for Retransmission Function
 * @verifyReq{RASW-604} Read Message to Send Function
 * @verifyReq{RASW-605} Remove from Buffer Function
 */
TEST_F(srsendTest, srsendTest005VerifyInitializationCheck)
{
    uint8_t nbr_of_function_calls = 9;        // number of function calls
    extern bool srsend_initialized;
    srsend_initialized = false;

    // Test variables
    uint32_t connection_id_index = 0;
    srtyp_SrMessage message = {};
    uint32_t sequence_number = 0;
    srtyp_SrMessageHeaderCreate message_header_create = {};

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srsendTest::invalidArgumentException));

    // call the functions without initializing the module
    EXPECT_THROW(srsend_InitBuffer(connection_id_index), std::invalid_argument);
    EXPECT_THROW(srsend_AddToBuffer(connection_id_index, &message), std::invalid_argument);
    EXPECT_THROW(srsend_ReadMessageToSend(connection_id_index, &message), std::invalid_argument);
    EXPECT_THROW(srsend_PrepareBufferForRetr(connection_id_index, sequence_number, message_header_create, &sequence_number), std::invalid_argument);
    EXPECT_THROW(srsend_IsSequenceNumberInBuffer(connection_id_index, sequence_number), std::invalid_argument);
    EXPECT_THROW(srsend_RemoveFromBuffer(connection_id_index, sequence_number), std::invalid_argument);
    EXPECT_THROW(srsend_GetFreeBufferEntries(connection_id_index), std::invalid_argument);
    EXPECT_THROW(srsend_GetUsedBufferEntries(connection_id_index), std::invalid_argument);
    EXPECT_THROW(srsend_GetNumberOfMessagesToSend(connection_id_index), std::invalid_argument);
}

/**
 * @test        @ID{srsendTest014} Verify the buffer state functions with invalid cases
 *
 * @details     This test verifies that the GetFreeBufferEntries, GetUsedBufferEntries and
 *              GetNumberOfMessagesToSend functions raise a fatal error when they are called
 *              with invalid connection id.
 *
 * Test steps:
 * - call the three functions and verify that a radef_kInvalidParameter error is raised for every function
 * .
 *
 * @testParameter
 * - Connection ID: RaSTA connection identification
 * - Max. Conn.: Number of configured connections
 * .
 * | Test Run | Input parameter | Test config | Expected values    |
 * |----------|-----------------|-------------|--------------------|
 * |          | Connection ID   | Max. Conn.  | Test Result        |
 * | 0        | 1               | 1           | Fatal error raised |
 * | 1        | 2               | 2           | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-597} Get Used Buffer Entries Function
 * @verifyReq{RASW-598} Get Free Buffer Entries Function
 * @verifyReq{RASW-599} Get Number of Messages to Send Function
 */
TEST_P(BufferStateTestWithParam, srsendTest014VerifyBufferStateFunctions)
{
    const uint8_t n_function_calls = 3;

    // initialize the module
    srsend_Init(n_max_conn);

    // expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
    .Times(n_function_calls)
    .WillRepeatedly(Invoke(this, &srsendTest::invalidArgumentException));

    // call the init function and expect a fatal error
    EXPECT_THROW(srsend_GetUsedBufferEntries(connection_id), std::invalid_argument);
    EXPECT_THROW(srsend_GetFreeBufferEntries(connection_id), std::invalid_argument);
    EXPECT_THROW(srsend_GetNumberOfMessagesToSend(connection_id), std::invalid_argument);
}

INSTANTIATE_TEST_SUITE_P(
    verifyBufferStateFunctions,
    BufferStateTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRSEND_MIN_NUM_CONN, 1),
        std::make_tuple(UT_SRSEND_MAX_NUM_CONN, 2)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
