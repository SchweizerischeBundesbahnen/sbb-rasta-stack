/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srrece_buffer_size.cc
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
 * @brief Test class for value-parametrized GetUsedBufferEntries test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection ID (uint32_t)
 * - expect fatal error (bool)
 * .
 */
class GetUsedBufferEntriesTestWithParam :   public srreceTest,
                                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, bool>>
{
    public:
    uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
    bool expect_fatal_error =           std::get<2>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized GetFreeBufferEntries test
 *
 * Parameter order:
 * - maximum number of connections (uint32_t)
 * - connection ID (uint32_t)
 * - nSendMax (uint8_t)
 * - expRetVal (uint32_t)
 * - expectFatalError (bool)
 * .
 */
class GetFreeBufferEntriesTestWithParam :   public srreceTest,
                                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint8_t, uint32_t, bool>>
{
    public:
    uint32_t n_max_conn =               std::get<0>(GetParam());        ///< maximum number of connections
    uint32_t connection_id =            std::get<1>(GetParam());        ///< connection ID
    uint8_t n_send_max =                std::get<2>(GetParam());        ///< max. buffer size
    uint32_t exp_retval =               std::get<3>(GetParam());        ///< expected return value
    bool expect_fatal_error =           std::get<4>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_receiveBuf
 * @{
 */

/**
 * @test        @ID{srreceTest010} Verify the GetUsedBufferEntries function.
 *
 * @details     This test verifies that correct behaviour of the GetUsedBufferEntries function.
 *
 * Test steps:
 * - initialize the module
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the GetUsedBufferEntries function
 * - verify that the buffer size returned is correct
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
 * @verifyReq{RASW-609} Get Used Buffer Entries Function
 */
TEST_P(GetUsedBufferEntriesTestWithParam, srreceTest010VerifyGetUsedBufferEntries)
{
    // initialize module
    srrece_Init(n_max_conn, UT_SRRECE_NSENDMAX_MAX);

    if (expect_fatal_error)
    {
        // call the GetUsedBufferEntries function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));
        EXPECT_THROW(srrece_GetUsedBufferEntries(connection_id), std::invalid_argument);
    }
    else
    {
        // call the GetUsedBufferEntries function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_EQ(0, srrece_GetUsedBufferEntries(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetUsedBufferEntries,
    GetUsedBufferEntriesTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 0, false),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, false),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, false),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 1, true),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 2, true)
    ));

/**
 * @test        @ID{srreceTest011} Verify the GetFreeBufferEntries function.
 *
 * @details     This test verifies that correct behaviour of the GetFreeBufferEntries function.
 *
 * Test steps:
 * - initialize the module
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the GetFreeBufferEntries function
 * - verify that the number of free buffer entries returned is correct
 * .
 *
 * @testParameter
 * - Connection Id: RaSTA connection identification
 * - Max. Num. Conn.: Number of configured connections
 * - nSendMax: Configured receive buffer size
 * - Expected Return Value: Expected return value for srrece_GetFreeBufferEntries function call
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter | Test config               || Expected values                                               |||
 * |----------|-----------------|-----------------|----------|-----------------------|--------------------|--------------------|
 * |          | Connection Id   | Max. Num. Conn. | nSendMax | Expected Return Value | Expect Fatal Error | Test Result        |
 * | 0        | 0               | 1               | 5        | 5                     | False              | Normal operation   |
 * | 1        | 0               | 2               | 10       | 10                    | False              | Normal operation   |
 * | 1        | 1               | 2               | 10       | 10                    | False              | Normal operation   |
 * | 2        | 1               | 1               | 5        | 5                     | True               | Fatal error raised |
 * | 2        | 2               | 2               | 5        | 5                     | True               | Fatal error raised |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-610} Get Free Buffer Entries Function
 */
TEST_P(GetFreeBufferEntriesTestWithParam, srreceTest011VerifyGetFreeBufferEntries)
{
    // initialize module
    srrece_Init(n_max_conn, n_send_max);

    if (expect_fatal_error)
    {
        // call the GetFreeBufferEntries function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srreceTest::invalidArgumentException));
        EXPECT_THROW(srrece_GetFreeBufferEntries(connection_id), std::invalid_argument);
    }
    else
    {
        // call the srrece_GetFreeBufferEntries function
        EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
        EXPECT_EQ(exp_retval, srrece_GetFreeBufferEntries(connection_id));
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyGetFreeBufferEntries,
    GetFreeBufferEntriesTestWithParam,
    ::testing::Values(
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 0, 5, 5, false),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 0, 10, 10, false),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 1, 10, 10, false),
        std::make_tuple(UT_SRRECE_MIN_NUM_CONN, 1, 5, 5, true),
        std::make_tuple(UT_SRRECE_MAX_NUM_CONN, 2, 5, 5, true)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
