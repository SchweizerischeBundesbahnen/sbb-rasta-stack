/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_raas.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the assert module of the common package.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_raas.hh"

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized assertNotNull test
 *
 * Parameter order:
 * - pointer to be checked (void *)
 * - error reason to be set (radef_RaStaReturnCode)
 * - expected error reason (radef_RaStaReturnCode)
 * .
 */
class AssertNotNullWithParam :  public raasTest,
                                public testing::WithParamInterface<std::tuple<void *, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
    public:
    void * pointer =                            std::get<0>(GetParam());        ///< pointer under test
    radef_RaStaReturnCode set_error_reason =    std::get<1>(GetParam());        ///< error reason to be set
    radef_RaStaReturnCode exp_error_reason =    std::get<2>(GetParam());        ///< expected error reason for fatal error
};

/**
 * @brief Test class for value-parametrized assertTrue test
 *
 * Parameter order:
 * - condition to be checked (bool)
 * - error reason to be set (radef_RaStaReturnCode)
 * - expected error reason (radef_RaStaReturnCode)
 * .
 */
class AssertTrueWithParam : public raasTest,
                            public testing::WithParamInterface<std::tuple<bool, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
    public:
    bool condition =                            std::get<0>(GetParam());        ///< condition under test
    radef_RaStaReturnCode set_error_reason =    std::get<1>(GetParam());        ///< error reason to be set
    radef_RaStaReturnCode exp_error_reason =    std::get<2>(GetParam());        ///< expected error reason for fatal error
};

/**
 * @brief Test class for value-parametrized assertU8InRange test
 *
 * Parameter order:
 * - value to be checked (uint8_t)
 * - min value (uint8_t)
 * - max value (uint8_t)
 * - error reason to be set (radef_RaStaReturnCode)
 * - expected error reason (radef_RaStaReturnCode)
 * .
 */
class AssertU8InRangeWithParam :    public raasTest,
                                    public testing::WithParamInterface<std::tuple<uint8_t, uint8_t, uint8_t, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
    public:
    uint8_t value =                             std::get<0>(GetParam());        ///< value under test
    uint8_t min_value =                         std::get<1>(GetParam());        ///< minimum allowed value
    uint8_t max_value =                         std::get<2>(GetParam());        ///< maximum allowed value
    radef_RaStaReturnCode set_error_reason =    std::get<3>(GetParam());        ///< error reason to be set
    radef_RaStaReturnCode exp_error_reason =    std::get<4>(GetParam());        ///< expected error reason for fatal error
};

/**
 * @brief Test class for value-parametrized assertU16InRange test
 *
 * Parameter order:
 * - value to be checked (uint16_t)
 * - min value (uint16_t)
 * - max value (uint16_t)
 * - error reason to be set (radef_RaStaReturnCode)
 * - expected error reason (radef_RaStaReturnCode)
 * .
 */
class AssertU16InRangeWithParam :   public raasTest,
                                    public testing::WithParamInterface<std::tuple<uint16_t, uint16_t, uint16_t, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
    public:
    uint16_t value =                            std::get<0>(GetParam());        ///< value under test
    uint16_t min_value =                        std::get<1>(GetParam());        ///< minimum allowed value
    uint16_t max_value =                        std::get<2>(GetParam());        ///< maximum allowed value
    radef_RaStaReturnCode set_error_reason =    std::get<3>(GetParam());        ///< error reason to be set
    radef_RaStaReturnCode exp_error_reason =    std::get<4>(GetParam());        ///< expected error reason for fatal error
};

/**
 * @brief Test class for value-parametrized assertU32InRange test
 *
 * Parameter order:
 * - value to be checked (uint32_t)
 * - min value (uint32_t)
 * - max value (uint32_t)
 * - error reason to be set (radef_RaStaReturnCode)
 * - expected error reason (radef_RaStaReturnCode)
 * .
 */
class AssertU32InRangeWithParam :   public raasTest,
                                    public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, radef_RaStaReturnCode, radef_RaStaReturnCode>>
{
    public:
    uint32_t value =                            std::get<0>(GetParam());        ///< value under test
    uint32_t min_value =                        std::get<1>(GetParam());        ///< minimum allowed value
    uint32_t max_value =                        std::get<2>(GetParam());        ///< maximum allowed value
    radef_RaStaReturnCode set_error_reason =    std::get<3>(GetParam());        ///< error reason to be set
    radef_RaStaReturnCode exp_error_reason =    std::get<4>(GetParam());        ///< expected error reason for fatal error
};

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

uint32_t dummy_variable = 0;        ///< dummy variable for assert test

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup common_assert
 * @{
 */

/**
 * @test        @ID{raasTest001} Verify the AssertNotNull function.
 *
 * @details     This test verifies the correct behaviour of the AssertNotNull function.
 *
 * Test steps:
 * - set mock function call expectation for the fatal error function
 * - call the AssertNotNull function with given test parameter
 * .
 *
 * @testParameter
 * - Pointer: Pointer to check
 * - Error Reason: Error reason to be set
 * - Exp. Error Reason: Expected thrown error reason
 * .
 * | Test Run | Input parameter         || Expected values                               ||
 * |----------|---------|----------------|-------------------------|----------------------|
 * |          | Pointer | Error Reason   | Exp. Error Reason       | Test Result          |
 * | 0        | Valid   | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 1        | NULL    | radef_kMin + 1 | radef_kMin + 1          | Fatal Error occurred |
 * | 2        | NULL    | radef_kMax - 1 | radef_kMax - 1          | Fatal Error occurred |
 * | 3        | NULL    | radef_kMax     | radef_kInvalidParameter | Fatal Error occurred |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-533} Component rasta_assert Overview
 * @verifyReq{RASW-534} Assert not Null Function
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-417} Fatal Error Handling Function Structure
 * @verifyReq{RASW-416} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(AssertNotNullWithParam, raasTest001VerifyAssertNotNull)
{
    if (exp_error_reason != radef_kNoError) {
        // call the function and expect an assertion violation
        EXPECT_CALL(rasys_mock, rasys_FatalError(exp_error_reason)).WillOnce(Invoke(this, &raasTest::invalidArgumentException));
        EXPECT_THROW(raas_AssertNotNull(pointer, set_error_reason), std::invalid_argument);
    }
    else {
        // call the function
        raas_AssertNotNull(pointer, set_error_reason);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyAssertNotNull,
    AssertNotNullWithParam,
    ::testing::Values(
        std::make_tuple((void *)&dummy_variable, radef_kNoError, radef_kNoError),
        std::make_tuple((void *)NULL, UT_RAAS_FIRST_ERROR, UT_RAAS_FIRST_ERROR),
        std::make_tuple((void *)NULL, UT_RAAS_LAST_ERROR, UT_RAAS_LAST_ERROR),
        std::make_tuple((void *)NULL, radef_kMax, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{raasTest002} Verify the AssertTrue function.
 *
 * @details     This test verifies the correct behaviour of the AssertTrue function.
 *
 * Test steps:
 * - set mock function call expectation for the fatal error function
 * - call the AssertTrue function with given test parameter
 * .
 *
 * @testParameter
 * - Condition: Condition under test
 * - Error Reason: Error reason to be set
 * - Exp. Error Reason: Expected thrown error reason
 * .
 * | Test Run | Input parameter           || Expected values                               ||
 * |----------|-----------|----------------|-------------------------|----------------------|
 * |          | Condition | Error Reason   | Exp. Error Reason       | Test Result          |
 * | 0        | True      | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 1        | False     | radef_kMin + 1 | radef_kMin + 1          | Fatal Error occurred |
 * | 2        | False     | radef_kMax - 1 | radef_kMax - 1          | Fatal Error occurred |
 * | 3        | False     | radef_kMax     | radef_kInvalidParameter | Fatal Error occurred |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-533} Component rasta_assert Overview
 * @verifyReq{RASW-535} Assert True Function
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-417} Fatal Error Handling Function Structure
 * @verifyReq{RASW-416} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(AssertTrueWithParam, raasTest002VerifyAssertTrue)
{
    // expect mocked function calls
    if (exp_error_reason != radef_kNoError) {
        // call the function and expect an assertion violation
        EXPECT_CALL(rasys_mock, rasys_FatalError(exp_error_reason)).WillOnce(Invoke(this, &raasTest::invalidArgumentException));
        EXPECT_THROW(raas_AssertTrue(condition, set_error_reason), std::invalid_argument);
    }
    else {
        // call the function
        raas_AssertTrue(condition, set_error_reason);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyAssertTrue,
    AssertTrueWithParam,
    ::testing::Values(
        std::make_tuple(true, radef_kNoError, radef_kNoError),
        std::make_tuple(false, UT_RAAS_FIRST_ERROR, UT_RAAS_FIRST_ERROR),
        std::make_tuple(false, UT_RAAS_LAST_ERROR, UT_RAAS_LAST_ERROR),
        std::make_tuple(false, radef_kMax, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{raasTest003} Verify the AssertU8InRange function.
 *
 * @details     This test verifies the correct behaviour of the AssertU8InRange function.
 *
 * Test steps:
 * - set mock function call expectation for the fatal error function
 * - call the AssertU8InRange function with given test parameter
 * .
 *
 * @testParameter
 * - Value: Value under test
 * - Min Value: Minimum allowed value
 * - Max Value: Maximum allowed value
 * - Error Reason: Error reason to be set
 * - Exp. Error Reason: Expected thrown error reason
 * .
 * | Test Run | Input parameter                             |||| Expected values                               ||
 * |----------|-------|-----------|-----------|----------------|-------------------------|----------------------|
 * |          | Value | Min Value | Max Value | Error Reason   | Exp. Error Reason       | Test Result          |
 * | 0        | 0     | 0         | 0         | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 1        | 255   | 255       | 255       | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 2        | 9     | 10        | 20        | radef_kMin + 1 | radef_kMin + 1          | Fatal Error occurred |
 * | 3        | 10    | 10        | 20        | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 4        | 20    | 10        | 20        | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 5        | 21    | 10        | 20        | radef_kMax - 1 | radef_kMax - 1          | Fatal Error occurred |
 * | 6        | 20    | 10        | 20        | radef_kMax     | radef_kInvalidParameter | Fatal Error occurred |
 * | 7        | 20    | 20        | 19        | radef_kMin + 1 | radef_kInvalidParameter | Fatal Error occurred |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-533} Component rasta_assert Overview
 * @verifyReq{RASW-538} Assert U8 in Range Function
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-417} Fatal Error Handling Function Structure
 * @verifyReq{RASW-416} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(AssertU8InRangeWithParam, raasTest003VerifyAssertU8InRange)
{
    // expect mocked function calls
    if (exp_error_reason != radef_kNoError) {
        // call the function and expect an assertion violation
        EXPECT_CALL(rasys_mock, rasys_FatalError(exp_error_reason)).WillOnce(Invoke(this, &raasTest::invalidArgumentException));
        EXPECT_THROW(raas_AssertU8InRange(value, min_value, max_value, set_error_reason), std::invalid_argument);
    }
    else {
        // call the function
        raas_AssertU8InRange(value, min_value, max_value, set_error_reason);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyAssertU8InRange,
    AssertU8InRangeWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, radef_kNoError, radef_kNoError),
        std::make_tuple(UINT8_MAX, UINT8_MAX, UINT8_MAX, radef_kNoError, radef_kNoError),
        std::make_tuple(9, 10, 20, UT_RAAS_FIRST_ERROR, UT_RAAS_FIRST_ERROR),
        std::make_tuple(10, 10, 20, radef_kNoError, radef_kNoError),
        std::make_tuple(20, 10, 20, radef_kNoError, radef_kNoError),
        std::make_tuple(21, 10, 20, UT_RAAS_LAST_ERROR, UT_RAAS_LAST_ERROR),
        std::make_tuple(20, 10, 20, radef_kMax, radef_kInvalidParameter),
        std::make_tuple(20, 20, 19, UT_RAAS_FIRST_ERROR, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{raasTest004} Verify the AssertU16InRange function.
 *
 * @details     This test verifies the correct behaviour of the AssertU16InRange function.
 *
 * Test steps:
 * - set mock function call expectation for the fatal error function
 * - call the AssertU16InRange function with given test parameter
 * .
 *
 * @testParameter
 * - Value: Value under test
 * - Min Value: Minimum allowed value
 * - Max Value: Maximum allowed value
 * - Error Reason: Error reason to be set
 * - Exp. Error Reason: Expected thrown error reason
 * .
 * | Test Run | Input parameter                             |||| Expected values                               ||
 * |----------|-------|-----------|-----------|----------------|-------------------------|----------------------|
 * |          | Value | Min Value | Max Value | Error Reason   | Exp. Error Reason       | Test Result          |
 * | 0        | 0     | 0         | 0         | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 1        | 65535 | 65535     | 65535     | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 2        | 999   | 1000      | 1005      | radef_kMin + 1 | radef_kMin + 1          | Fatal Error occurred |
 * | 3        | 1000  | 1000      | 1005      | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 4        | 1005  | 1000      | 1005      | radef_kNoError | radef_kNoError          | Normal operation     |
 * | 5        | 1006  | 1000      | 1005      | radef_kMax - 1 | radef_kMax - 1          | Fatal Error occurred |
 * | 6        | 1005  | 1000      | 1005      | radef_kMax     | radef_kInvalidParameter | Fatal Error occurred |
 * | 7        | 1005  | 1005      | 1004      | radef_kMin + 1 | radef_kInvalidParameter | Fatal Error occurred |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-533} Component rasta_assert Overview
 * @verifyReq{RASW-536} Assert U16 in Range Function
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-417} Fatal Error Handling Function Structure
 * @verifyReq{RASW-416} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(AssertU16InRangeWithParam, raasTest004VerifyAssertU16InRange)
{
    // expect mocked function calls
    if (exp_error_reason != radef_kNoError) {
        // call the function and expect an assertion violation
        EXPECT_CALL(rasys_mock, rasys_FatalError(exp_error_reason)).WillOnce(Invoke(this, &raasTest::invalidArgumentException));
        EXPECT_THROW(raas_AssertU16InRange(value, min_value, max_value, set_error_reason), std::invalid_argument);
    }
    else {
        // call the function
        raas_AssertU16InRange(value, min_value, max_value, set_error_reason);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyAssertU16InRange,
    AssertU16InRangeWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, radef_kNoError, radef_kNoError),
        std::make_tuple(UINT16_MAX, UINT16_MAX, UINT16_MAX, radef_kNoError, radef_kNoError),
        std::make_tuple(999, 1000, 1005, UT_RAAS_FIRST_ERROR, UT_RAAS_FIRST_ERROR),
        std::make_tuple(1000, 1000, 1005, radef_kNoError, radef_kNoError),
        std::make_tuple(1005, 1000, 1005, radef_kNoError, radef_kNoError),
        std::make_tuple(1006, 1000, 1005, UT_RAAS_LAST_ERROR, UT_RAAS_LAST_ERROR),
        std::make_tuple(1005, 1000, 1005, radef_kMax, radef_kInvalidParameter),
        std::make_tuple(1005, 1005, 1004, UT_RAAS_FIRST_ERROR, radef_kInvalidParameter)
    ));

/**
 * @test        @ID{raasTest005} Verify the AssertU32InRange function.
 *
 * @details     This test verifies the correct behaviour of the AssertU32InRange function.
 *
 * Test steps:
 * - set mock function call expectation for the fatal error function
 * - call the AssertU32InRange function with given test parameter
 * .
 *
 * @testParameter
 * - Value: Value under test
 * - Min Value: Minimum allowed value
 * - Max Value: Maximum allowed value
 * - Error Reason: Error reason to be set
 * - Exp. Error Reason: Expected thrown error reason
 * .
 * | Test Run | Input parameter                                             |||| Expected values                               ||
 * |----------|------------|------------|------------|-------------------------|-------------------------|----------------------|
 * |          | Value      | Min Value  | Max Value  | Error Reason            | Exp. Error Reason       | Test Result          |
 * | 0        | 0          | 0          | 0          | radef_kNoError          | radef_kNoError          | Normal operation     |
 * | 1        | 0xffffffff | 0xffffffff | 0xffffffff | radef_kNoError          | radef_kNoError          | Normal operation     |
 * | 2        | 99999      | 100000     | 200000     | radef_kMin + 1          | radef_kMin + 1          | Fatal Error occurred |
 * | 3        | 100000     | 100000     | 200000     | radef_kInvalidParameter | radef_kNoError          | Normal operation     |
 * | 4        | 200000     | 100000     | 200000     | radef_kInternalError    | radef_kNoError          | Normal operation     |
 * | 5        | 200001     | 100000     | 200000     | radef_kMax - 1          | radef_kMax - 1          | Fatal Error occurred |
 * | 6        | 200000     | 100000     | 200000     | radef_kMax              | radef_kInvalidParameter | Fatal Error occurred |
 * | 7        | 200000     | 200000     | 199999     | radef_kMin + 1          | radef_kInvalidParameter | Fatal Error occurred |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-533} Component rasta_assert Overview
 * @verifyReq{RASW-537} Assert U32 in Range Function
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-417} Fatal Error Handling Function Structure
 * @verifyReq{RASW-416} Error Code
 * @verifyReq{RASW-483} Enum RaSta Return Code Structure
 * @verifyReq{RASW-503} Enum RaSta Return Code Usage
 */
TEST_P(AssertU32InRangeWithParam, raasTest005VerifyAssertU32InRange)
{
    // expect mocked function calls
    if (exp_error_reason != radef_kNoError) {
        // call the function and expect an assertion violation
        EXPECT_CALL(rasys_mock, rasys_FatalError(exp_error_reason)).WillOnce(Invoke(this, &raasTest::invalidArgumentException));
        EXPECT_THROW(raas_AssertU32InRange(value, min_value, max_value, set_error_reason), std::invalid_argument);
    }
    else {
        // call the function
        raas_AssertU32InRange(value, min_value, max_value, set_error_reason);
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyAssertU32InRange,
    AssertU32InRangeWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, radef_kNoError, radef_kNoError),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, radef_kNoError, radef_kNoError),
        std::make_tuple(99999, 100000, 200000, UT_RAAS_FIRST_ERROR, UT_RAAS_FIRST_ERROR),
        std::make_tuple(100000, 100000, 200000, radef_kNoError, radef_kNoError),
        std::make_tuple(200000, 100000, 200000, radef_kNoError, radef_kNoError),
        std::make_tuple(200001, 100000, 200000, UT_RAAS_LAST_ERROR, UT_RAAS_LAST_ERROR),
        std::make_tuple(200000, 100000, 200000, radef_kMax, radef_kInvalidParameter),
        std::make_tuple(200000, 200000, 199999, UT_RAAS_FIRST_ERROR, radef_kInvalidParameter)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
