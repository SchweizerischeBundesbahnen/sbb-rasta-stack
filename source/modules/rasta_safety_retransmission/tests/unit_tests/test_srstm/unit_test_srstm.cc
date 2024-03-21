/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srstm.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the state machine module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srstm.hh"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Class Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - configured connections (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public srstmTest,
                            public testing::WithParamInterface<std::tuple<uint32_t, bool>>
{
    public:
    uint32_t configured_connections =   std::get<0>(GetParam());        ///< number of configured connections
    bool expect_fatal_error =           std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized GetConnectionState test
 *
 * Parameter order:
 * - connection ID (uint32_t)
 * - expected connection state (sraty_ConnectionStates)
 * - expect fatal error (bool)
 * .
 */
class GetConnStateTestWithParam :   public srstmTest,
                                    public testing::WithParamInterface<std::tuple<uint32_t, sraty_ConnectionStates, bool>>
{
    public:
    uint32_t connection_id =                    std::get<0>(GetParam());        ///< index of the connection id
    sraty_ConnectionStates connection_state =   std::get<1>(GetParam());        ///< expected connection state
    bool expect_fatal_error =                   std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized ProcessConnectionStateMachine test
 *
 * Parameter order:
 * - configured connections (uint32_t)
 * - connection ID (uint32_t)
 * - event (srtyp_ConnectionEvents)
 * - reset state to NotInitialized (bool)
 * - expected error code (radef_RaStaReturnCode)
 * .
 */
class ProcessStateMachineTestWithParam :    public srstmTest,
                                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, srtyp_ConnectionEvents, bool, radef_RaStaReturnCode>>
{
    public:
    uint32_t configured_connections =   std::get<0>(GetParam());        ///< number of configured connections
    uint32_t connection_id =            std::get<1>(GetParam());        ///< connection id
    srtyp_ConnectionEvents event =      std::get<2>(GetParam());        ///< event to be processed
    bool reset_state_uninitialized =    std::get<3>(GetParam());        ///< reset the state to NotInitialized
    radef_RaStaReturnCode error_code =  std::get<4>(GetParam());        ///< expected error code
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_sm
 * @{
 */

/**
 * @test        @ID{srstmTest001} Verify the init function.
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
 * | Test Run | Input parameter        | Expected values                        ||
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
 * @verifyReq{RASW-562} Init sr_state_machine Function
 */
TEST_P(InitTestWithParam, srstmTest001VerifyInitFunction)
{
    extern uint32_t srstm_number_of_connections;

    if (expect_fatal_error)
    {
        // call the init function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_Init(configured_connections), std::invalid_argument);
    }
    else
    {
        // call the init function and verify if parameter are set correctly
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_NO_THROW(srstm_Init(configured_connections));

        EXPECT_EQ(srstm_number_of_connections, configured_connections);
        EXPECT_TRUE(srstm_initialized);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyInitFunction,
    InitTestWithParam,
    ::testing::Values(
        std::make_tuple(0, true),
        std::make_tuple(1, false),
        std::make_tuple(2, false),
        std::make_tuple(3, true)
    ));

/**
 * @test        @ID{srstmTest002} Verify the number of init function calls.
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
 * @verifyReq{RASW-562} Init sr_state_machine Function
 */
TEST_F(srstmTest, srstmTest002VerifyInitFunctionCalls)
{
    // expect fatal error function to be called
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));

    // call the init function and verify that it completes successfully
    EXPECT_NO_THROW(srstm_Init(UT_SRSTM_MAX_NUM_CONN));

    // call the init function again and verify that an assertion violation is raised as the module is already initialized
    EXPECT_THROW(srstm_Init(UT_SRSTM_MAX_NUM_CONN), std::invalid_argument);
}

/**
 * @test        @ID{srstmTest003} Initialization checks
 *
 * @details     This test verifies that the fatal error function is executed whenever a function is called
 *              without proper initialization of the module.
 *
 * Test steps:
 * - call the ProcessConnectionStateMachine function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * - call the GetConnectionState function and verify that a fatal error with error code radef_kNotInitialized gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-559} Component sr_state_machine Overview
 * @verifyReq{RASW-561} Get Connection State Function
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_F(srstmTest, srstmTest003VerifyInitializationCheck)
{
    const uint8_t nbr_of_function_calls = 2;        // number of function calls

    // Test variables
    uint32_t connection_id_index = UT_SRSTM_MIN_IDX_CONN;

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kNotInitialized))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srstmTest::invalidArgumentException));

    // call the functions without initializing the module
    EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id_index, srtyp_kConnEventNone, false, false), std::invalid_argument);
    EXPECT_THROW(srstm_GetConnectionState(connection_id_index), std::invalid_argument);
}

/**
 * @test        @ID{srstmTest004} Verify the GetConnectionState function.
 *
 * @details     This test verifies the behaviour of the GetConnectionState function.
 *
 * Test steps:
 * - initialize the module for maximum number of connection
 * - set the state of the connection according to test parameter
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the GetConnectionState function with given test parameter
 * - verify if the correct state gets returned
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Expected connection state: Expected connection state
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Expected values                                                         |||
 * |----------|-----------------|---------------------------------|--------------------|--------------------|
 * |          | Connection Id   | Expected connection state       | Expect Fatal Error | Test Result        |
 * | 0        | 0               | sraty_kConnectionNotInitialized | False              | Normal operation   |
 * | 1        | 0               | sraty_kConnectionClosed         | False              | Normal operation   |
 * | 2        | 0               | sraty_kConnectionDown           | False              | Normal operation   |
 * | 3        | 0               | sraty_kConnectionStart          | False              | Normal operation   |
 * | 4        | 0               | sraty_kConnectionUp             | False              | Normal operation   |
 * | 5        | 0               | sraty_kConnectionRetransRequest | False              | Normal operation   |
 * | 6        | 0               | sraty_kConnectionRetransRunning | False              | Normal operation   |
 * | 7        | 1               | sraty_kConnectionNotInitialized | False              | Normal operation   |
 * | 8        | 2               | sraty_kConnectionNotInitialized | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-561} Get Connection State Function
 */
TEST_P(GetConnStateTestWithParam, srstmTest004VerifyGetConnectionStateFunction)
{
    // initialize the module
    srstm_Init(UT_SRSTM_MAX_NUM_CONN);

    // set connection state
    srstm_connection_states[connection_id] = connection_state;

    if (expect_fatal_error)
    {
        // call the GetConnectionState function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
        EXPECT_THROW(srstm_GetConnectionState(connection_id), std::invalid_argument);
    }
    else
    {
        // call the GetConnectionState function and verify if correct state is returned
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_EQ(connection_state, srstm_GetConnectionState(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetConnectionStateFunction,
    GetConnStateTestWithParam,
    ::testing::Values(
        std::make_tuple(0, sraty_kConnectionNotInitialized, false),
        std::make_tuple(0, sraty_kConnectionClosed, false),
        std::make_tuple(0, sraty_kConnectionDown, false),
        std::make_tuple(0, sraty_kConnectionStart, false),
        std::make_tuple(0, sraty_kConnectionUp, false),
        std::make_tuple(0, sraty_kConnectionRetransRequest, false),
        std::make_tuple(0, sraty_kConnectionRetransRunning, false),
        std::make_tuple(1, sraty_kConnectionNotInitialized, false),
        std::make_tuple(2, sraty_kConnectionNotInitialized, true)
    ));

/**
 * @test        @ID{srstmTest011} Verify the ProcessConnectionStateMachine function.
 *
 * @details     This test verifies the behaviour of the ProcessConnectionStateMachine function
 *              in case of invalid parameter.
 *
 * Test steps:
 * - initialize the module with given parameter
 * - reset the state to NotInitialized (only if indicated by the test parameter)
 * - call the ProcessConnectionStateMachine function with given test parameter
 * - verify that a fatal error is raised
 * .
 *
 * @testParameter
 * - Connection ID: RaSTA connection identification
 * - Event: Event to be processed
 * - Configured Connections: Number of configured connections
 * - Reset State to NotInitialized: Indicates if state gets reset to NotInitialized before srstm_ProcessConnectionStateMachine function call
 * - Error Code: Expected error code when calling function
 * .
 * | Test Run | Input parameter                     || Test config                                           || Expected values                             ||
 * |----------|---------------|----------------------|------------------------|-------------------------------|-------------------------|--------------------|
 * |          | Connection ID | Event                | Configured Connections | Reset State to NotInitialized | Error Code              | Test Result        |
 * | 0        | 1             | srtyp_kConnEventNone | 1                      | False                         | radef_kInvalidParameter | Fatal error raised |
 * | 1        | 2             | srtyp_kConnEventNone | 2                      | False                         | radef_kInvalidParameter | Fatal error raised |
 * | 2        | 0             | srtyp_kConnEventMax  | 2                      | False                         | radef_kInvalidParameter | Fatal error raised |
 * | 3        | 0             | srtyp_kConnEventNone | 2                      | True                          | radef_kInternalError    | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-563} Process Connection State Machine Function
 */
TEST_P(ProcessStateMachineTestWithParam, srstmTest011VerifyProcessStateMachineFunction)
{
    extern sraty_ConnectionStates srstm_connection_states[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

    // initialize the module with given number of connections
    srstm_Init(configured_connections);

    if (reset_state_uninitialized)
    {
        srstm_connection_states[connection_id] = sraty_kConnectionNotInitialized;
    }

    // call the ProcessConnectionStateMachine function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(error_code)).WillOnce(Invoke(this, &srstmTest::invalidArgumentException));
    EXPECT_THROW(srstm_ProcessConnectionStateMachine(connection_id, event, false, false), std::invalid_argument);

}

INSTANTIATE_TEST_SUITE_P(
    verifyProcessStateMachineFunction,
    ProcessStateMachineTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRSTM_MIN_NUM_CONN, 1, srtyp_kConnEventNone, false, radef_kInvalidParameter),
        std::make_tuple(UT_SRSTM_MAX_NUM_CONN, 2, srtyp_kConnEventNone, false, radef_kInvalidParameter),
        std::make_tuple(UT_SRSTM_MAX_NUM_CONN, 0, srtyp_kConnEventMax, false, radef_kInvalidParameter),
        std::make_tuple(UT_SRSTM_MAX_NUM_CONN, 0, srtyp_kConnEventNone, true, radef_kInternalError)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
