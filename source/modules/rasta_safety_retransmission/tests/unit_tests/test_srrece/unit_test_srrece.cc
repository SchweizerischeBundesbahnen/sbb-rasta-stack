/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srrece.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 44009c6f891958c1437226727a79ec79b3d4ab29
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4944: Fixed VerifyInitFunction test uses wrong datatype for extern variable,05.12.2022,N. Fricker}
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
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - configured connections (uint32_t)
 * - configured nSendMax (uint16_t)
 * - expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public srreceTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, uint16_t, bool>>
{
    public:
    uint32_t configured_connections =   std::get<0>(GetParam());        ///< number of configured connections
    uint16_t exp_n_send_max =           std::get<1>(GetParam());        ///< expected nSendMax
    bool expect_fatal_error =           std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized init buffer test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection id (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitBufferTestWithParam : public srreceTest,
                                public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
    public:
    uint32_t n_max_conn =           std::get<0>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =        std::get<1>(GetParam());        ///< connection id
    bool expect_fatal_error =       std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_receiveBuf
 * @{
 */

/**
 * @test        @ID{srreceTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the number of configured
 *              connections and the nSendMax parameter correctly.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the init function with given test parameter
 * - verify if the number of connections is set correctly
 * - verify that the nSendMax parameter is set correctly
 * - verify that the module is set to initialized state
 * .
 *
 * @testParameter
 * - Configured Connections: Number of connections
 * - nSendMax: Configured receive buffer size
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                  || Expected values                        ||
 * |----------|------------------------|----------|--------------------|--------------------|
 * |          | Configured Connections | nSendMax | Expect Fatal Error | Test Result        |
 * | 0        | 0                      | 2        | True               | Fatal error raised |
 * | 1        | 1                      | 2        | False              | Normal operation   |
 * | 2        | 2                      | 2        | False              | Normal operation   |
 * | 3        | 3                      | 2        | True               | Fatal error raised |
 * | 4        | 1                      | 1        | True               | Fatal error raised |
 * | 5        | 1                      | 3        | False              | Normal operation   |
 * | 6        | 1                      | 19       | False              | Normal operation   |
 * | 7        | 1                      | 20       | False              | Normal operation   |
 * | 8        | 1                      | 21       | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-611} Init sr_received_buffer Function
 */
TEST_P(InitTestWithParam, srreceTest001VerifyInitFunction)
{
    extern uint32_t srrece_number_of_connections;
    extern uint16_t srrece_n_send_max;

    if (expect_fatal_error)
    {
        // call the init function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));
        EXPECT_THROW(srrece_Init(configured_connections, exp_n_send_max), std::invalid_argument);
    }
    else
    {
        // call the init function and verify if parameter are set correctly
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_NO_THROW(srrece_Init(configured_connections, exp_n_send_max));

        EXPECT_EQ(srrece_number_of_connections, configured_connections);
        EXPECT_EQ(srrece_n_send_max, exp_n_send_max);
        EXPECT_TRUE(srrece_initialized);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyInitFunction,
    InitTestWithParam,
    ::testing::Values(
        std::make_tuple((UT_SRRECE_MIN_NUM_CONN - 1), UT_SRRECE_NSENDMAX_MIN, true),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, UT_SRRECE_NSENDMAX_MIN, false),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, UT_SRRECE_NSENDMAX_MIN, false),
        std::make_tuple((UT_SRRECE_MAX_NUM_CONN + 1), UT_SRRECE_NSENDMAX_MIN, true),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, (UT_SRRECE_NSENDMAX_MIN - 1), true),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, (UT_SRRECE_NSENDMAX_MIN + 1), false),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, (UT_SRRECE_NSENDMAX_MAX - 1), false),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, UT_SRRECE_NSENDMAX_MAX, false),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, (UT_SRRECE_NSENDMAX_MAX + 1), true)
    ));

/**
 * @test        @ID{srreceTest002} Verify the init buffer function.
 *
 * @details     This test verifies that the init buffer function initializes
 *              the dedicated receive buffer correctly.
 *
 * Test steps:
 * - initialize the module
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - add a message to the buffer
 * - verify that the buffer size is set to 1
 * - call the init buffer function with given test parameter and verify that it completes successfully
 * - verify that the buffer size is reset to 0
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Max. Num. Conn.: Number of configured connections
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config     | Expected values                        ||
 * |----------|-----------------|-----------------|--------------------|--------------------|
 * |          | Connection Id   | Max. Num. Conn. | Expect Fatal Error | Test Result        |
 * | 0        | 0               | 1               | False              | Normal operation   |
 * | 1        | 0               | 2               | False              | Normal operation   |
 * | 2        | 1               | 2               | False              | Normal operation   |
 * | 3        | 1               | 1               | True               | Fatal error raised |
 * | 4        | 2               | 2               | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-612} Init Buffer Function
 * @verifyReq{RASW-553} Component sr_notifications Overview
 * @verifyReq{RASW-554} Message Received Notification
 * @verifyReq{RASW-279} Message Received Notification Structure
 * @verifyReq{RASW-302} Connection Identification
 */
TEST_P(InitBufferTestWithParam, srreceTest002VerifyInitBufferFunction)
{
    srtyp_SrMessagePayload dummy_payload = {5, {0}};

    // initialize the module
    srrece_Init(n_max_conn, UT_SRRECE_NSENDMAX_MAX);

    if (expect_fatal_error)
    {
        // call the init function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));
        EXPECT_THROW(srrece_InitBuffer(connection_id), std::invalid_argument);
    }
    else
    {
        // add a message to the buffer to increment buffer size
        EXPECT_CALL(srnot_mock, srnot_MessageReceivedNotification(connection_id)).Times(1);
        srrece_AddToBuffer(connection_id, &dummy_payload);

        // verify that the buffer size is set to 1
        EXPECT_EQ(1, srrece_GetUsedBufferEntries(connection_id));

        // call the initBuffer function and verify that it completes successfully
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_NO_THROW(srrece_InitBuffer(connection_id));

        // verify that the buffer size is reset
        EXPECT_EQ(0, srrece_GetUsedBufferEntries(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyInitBufferFunction,
    InitBufferTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 0, false),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, false),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, false),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 1, true),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 2, true)
    ));

/**
 * @test        @ID{srreceTest003} Verify the number of init function calls.
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
 * @verifyReq{RASW-611} Init sr_received_buffer Function
 */
TEST_F(srreceTest, srreceTest003VerifyInitFunctionCalls)
{
    // expect fatal error function to be called
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));

    // call the init function and verify that it completes successfully
    EXPECT_NO_THROW(srrece_Init(UT_SRRECE_MIN_NUM_CONN, UT_SRRECE_NSENDMAX_MIN));

    // call the init function again and verify that an assertion violations is raised as the module is already initialized
    EXPECT_THROW(srrece_Init(UT_SRRECE_MIN_NUM_CONN, UT_SRRECE_NSENDMAX_MIN), std::invalid_argument);
}

/**
 * @test        @ID{srreceTest004} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - call the AddToBuffer function and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the ReadFromBuffer function and verify that a fatal error with error code radef_kInvalidParameter gets thrown
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
 * @verifyReq{RASW-608} Add to Buffer Function
 * @verifyReq{RASW-613} Read from Buffer Function
 */
TEST_F(srreceTest, srreceTest004VerifyNullPointerCheck)
{
    uint8_t nbr_of_function_calls = 2;          // number of function calls

    // Test variables
    uint32_t connection_id_index = UT_SRRECE_MIN_NUM_CONN;

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srreceTest::invalidArgumentException));

    // initialize the module
    srrece_Init(UT_SRRECE_MIN_NUM_CONN, UT_SRRECE_NSENDMAX_MIN);

    // call the functions
    EXPECT_THROW(srrece_AddToBuffer(connection_id_index, (srtyp_SrMessagePayload *)NULL), std::invalid_argument);
    EXPECT_THROW(srrece_ReadFromBuffer(connection_id_index, (srtyp_SrMessagePayload *)NULL), std::invalid_argument);
}

/**
 * @test        @ID{srreceTest005} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the InitBuffer function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the AddToBuffer function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the ReadFromBuffer function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetFreeBufferEntries function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetUsedBufferEntries function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetPayloadSizeOfNextMessageToRead function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-607} Component sr_received_buffer Overview
 * @verifyReq{RASW-608} Add to Buffer Function
 * @verifyReq{RASW-609} Get Used Buffer Entries Function
 * @verifyReq{RASW-610} Get Free Buffer Entries Function
 * @verifyReq{RASW-612} Init Buffer Function
 * @verifyReq{RASW-613} Read from Buffer Function
 * @verifyReq{RASW-823} Get Payload Size of Next Message To Read Function
 */
TEST_F(srreceTest, srreceTest005VerifyInitializationCheck)
{
    uint8_t nbr_of_function_calls = 6;      // number of function calls

    // Test variables
    uint32_t connection_id_index = UT_SRRECE_MIN_NUM_CONN;
    srtyp_SrMessagePayload message_payload = {};

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srreceTest::invalidArgumentException));

    // call the functions without initializing the module
    EXPECT_THROW(srrece_InitBuffer(connection_id_index), std::invalid_argument);
    EXPECT_THROW(srrece_AddToBuffer(connection_id_index, &message_payload), std::invalid_argument);
    EXPECT_THROW(srrece_ReadFromBuffer(connection_id_index, &message_payload), std::invalid_argument);
    EXPECT_THROW(srrece_GetFreeBufferEntries(connection_id_index), std::invalid_argument);
    EXPECT_THROW(srrece_GetUsedBufferEntries(connection_id_index), std::invalid_argument);
    EXPECT_THROW(srrece_GetPayloadSizeOfNextMessageToRead(connection_id_index), std::invalid_argument);
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
