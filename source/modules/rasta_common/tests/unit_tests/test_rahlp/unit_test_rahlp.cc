/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_rahlp.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the rasta helper module of the common package.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_rahlp.hh"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test Class Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized IsU16InRange test
 *
 * Parameter order:
 * - value to be checked (uint16_t)
 * - min value (uint16_t)
 * - max value (uint16_t)
 * - expected return value (bool)
 * - expect fatal error (bool)
 * .
 */
class IsU16InRangeTestWithParam : public rahlpTest,
                                  public testing::WithParamInterface<std::tuple<uint16_t, uint16_t, uint16_t, bool, bool>>
{
  public:
  uint16_t value =                            std::get<0>(GetParam());        ///< value under test
  uint16_t min_value =                        std::get<1>(GetParam());        ///< minimum allowed value
  uint16_t max_value =                        std::get<2>(GetParam());        ///< maximum allowed value
  bool exp_return_value =                     std::get<3>(GetParam());        ///< expected return value
  bool expect_fatal_error =                   std::get<4>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized IsU32InRange test
 *
 * Parameter order:
 * - value to be checked (uint32_t)
 * - min value (uint32_t)
 * - max value (uint32_t)
 * - expected return value (bool)
 * - expect fatal error (bool)
 * .
 */
class IsU32InRangeTestWithParam : public rahlpTest,
                                  public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, bool, bool>>
{
  public:
  uint32_t value =                            std::get<0>(GetParam());        ///< value under test
  uint32_t min_value =                        std::get<1>(GetParam());        ///< minimum allowed value
  uint32_t max_value =                        std::get<2>(GetParam());        ///< maximum allowed value
  bool exp_return_value =                     std::get<3>(GetParam());        ///< expected return value
  bool expect_fatal_error =                   std::get<4>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup common_helper
 * @{
 */

/**
 * @test        @ID{rahlpTest001} Verify the IsU16InRange function
 *
 * @details     This test verifies the correct behaviour of the IsU16InRange function.
 *
 * Test steps:
 * - Set expectation for the rasys_FatalError function if indicated by the test parameter
 * - If a fatal error is expected, call the IsU16InRange function with given test parameter and expect a std::invalid_argument exception.
 * - If no fatal error is expected, call the IsU16InRange function with given test parameter and verify the return value.
 * .
 *
 * @testParameter
 * - Value: Value under test
 * - Min Value: Minimum allowed value
 * - Max Value: Maximum allowed value
 * - Exp. Return Value: Return value of function call
 * - Exp. Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                |||| Expected values                        ||
 * |----------|-------|-----------|-----------|-------------------|------------------|----------------------|
 * |          | Value | Min Value | Max Value | Exp. Return Value | Exp. Fatal Error | Test Result          |
 * | 0        | 0     | 0         | 0         | true              | False            | Normal operation     |
 * | 1        | 65535 | 65535     | 65535     | true              | False            | Normal operation     |
 * | 2        | 999   | 1000      | 1005      | false             | False            | Normal operation     |
 * | 3        | 1000  | 1000      | 1005      | true              | False            | Normal operation     |
 * | 4        | 1005  | 1000      | 1005      | true              | False            | Normal operation     |
 * | 5        | 1006  | 1000      | 1005      | false             | False            | Normal operation     |
 * | 6        | 1005  | 1005      | 1004      | n/a               | True             | Fatal Error occurred |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-818} Component rasta_Helper Overview
 * @verifyReq{RASW-821} Is U16 in Range Function
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-417} Fatal Error Handling Function Structure
 * @verifyReq{RASW-416} Error Code
 */
TEST_P(IsU16InRangeTestWithParam, rahlpTest001VerifyIsU16InRange) {
  if (expect_fatal_error) {
    // call the function and expect an assertion violation
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &rahlpTest::invalidArgumentException));
    EXPECT_THROW(rahlp_IsU16InRange(value, min_value, max_value), std::invalid_argument);
  }
  else {
    // call the function and verify the return value
    EXPECT_EQ(exp_return_value, rahlp_IsU16InRange(value, min_value, max_value));
  }
}

INSTANTIATE_TEST_SUITE_P(
    verifyIsU16InRange,
    IsU16InRangeTestWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, true, false),
        std::make_tuple(UINT16_MAX, UINT16_MAX, UINT16_MAX, true, false),
        std::make_tuple(999, 1000, 1005, false, false),
        std::make_tuple(1000, 1000, 1005, true, false),
        std::make_tuple(1005, 1000, 1005, true, false),
        std::make_tuple(1006, 1000, 1005, false, false),
        std::make_tuple(1005, 1005, 1004, false, true)
    ));

/**
 * @test        @ID{rahlpTest002} Verify the IsU32InRange function
 *
 * @details     This test verifies the correct behaviour of the IsU32InRange function.
 *
 * Test steps:
 * - Set expectation for the rasys_FatalError function if indicated by the test parameter
 * - If a fatal error is expected, call the IsU32InRange function with given test parameter and expect a std::invalid_argument exception.
 * - If no fatal error is expected, call the IsU32InRange function with given test parameter and verify the return value.
 * .
 *
 * @testParameter
 * - Value: Value under test
 * - Min Value: Minimum allowed value
 * - Max Value: Maximum allowed value
 * - Exp. Return Value: Return value of function call
 * - Exp. Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                       |||| Expected values                        ||
 * |----------|------------|------------|------------|-------------------|------------------|----------------------|
 * |          | Value      | Min Value  | Max Value  | Exp. Return Value | Exp. Fatal Error | Test Result          |
 * | 0        | 0          | 0          | 0          | true              | False            | Normal operation     |
 * | 1        | 0xffffffff | 0xffffffff | 0xffffffff | true              | False            | Normal operation     |
 * | 2        | 99999      | 100000     | 200000     | false             | False            | Normal operation     |
 * | 3        | 100000     | 100000     | 200000     | true              | False            | Normal operation     |
 * | 4        | 200000     | 100000     | 200000     | true              | False            | Normal operation     |
 * | 5        | 200001     | 100000     | 200000     | false             | False            | Normal operation     |
 * | 6        | 200000     | 200000     | 199999     | n/a               | True             | Fatal Error occurred |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-818} Component rasta_Helper Overview
 * @verifyReq{RASW-820} Is U32 in Range Function
 * @verifyReq{RASW-521} Input Parameter Check
 * @verifyReq{RASW-527} Component rasta_system_adapter Overview
 * @verifyReq{RASW-528} Fatal Error Function
 * @verifyReq{RASW-417} Fatal Error Handling Function Structure
 * @verifyReq{RASW-416} Error Code
 */
TEST_P(IsU32InRangeTestWithParam, rahlpTest002VerifyIsU32InRange) {
  if (expect_fatal_error) {
    // call the function and expect an assertion violation
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &rahlpTest::invalidArgumentException));
    EXPECT_THROW(rahlp_IsU32InRange(value, min_value, max_value), std::invalid_argument);
  }
  else {
    // call the function and verify the return value
    EXPECT_EQ(exp_return_value, rahlp_IsU32InRange(value, min_value, max_value));
  }
}

INSTANTIATE_TEST_SUITE_P(
    verifyIsU32InRange,
    IsU32InRangeTestWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, true, false),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, true, false),
        std::make_tuple(99999, 100000, 200000, false, false),
        std::make_tuple(100000, 100000, 200000, true, false),
        std::make_tuple(200000, 100000, 200000, true, false),
        std::make_tuple(200001, 100000, 200000, false, false),
        std::make_tuple(200000, 200000, 199999, false, true)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
