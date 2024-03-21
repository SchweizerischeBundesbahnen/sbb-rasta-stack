/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redcrc.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e465d64cd5d036bb35a06ffd760b7d3b77e6e7cf
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the crc module of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redcrc.hh"

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

static const uint8_t test_message[UT_REDCRC_DATA_LEN_MAX] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
  21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
  61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
  81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
  101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120,
  121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140,
  141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
  161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180,
  181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
  201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220,
  221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
  241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 0, 1, 2, 3, 4, 5,
  6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
  26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
  46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
  66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85,
  86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105,
  106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125,
  126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
  146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165,
  166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185,
  186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205,
  206, 207, 208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225,
  226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245,
  246, 247, 248, 249, 250, 251, 252, 253, 254, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
  11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
  31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50,
  51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70,
  71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,
  91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110,
  111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130,
  131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150,
  151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170,
  171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190,
  191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210,
  211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230,
  231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
  251, 252, 253, 254, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
  36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
  76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
  96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115,
  116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
  136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155,
  156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
  176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195,
  196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, 214, 215,
  216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235,
  236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 0,
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
  21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
  61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
  81, 82, 83, 84, 85, 86, 87, 88
};      ///< test message used to generate CRC values in script which was tested against example message values from DIN VDE V 0831-200 Annex A

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
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - Configured check code type (redcty_CheckCodeType)
 * - Expect fatal error (bool)
 * .
 */
class InitTestWithParam :   public redcrcTest,
                            public testing::WithParamInterface<std::tuple<redcty_CheckCodeType, bool>>
{
  public:
  redcty_CheckCodeType configured_checkcode_type =         std::get<0>(GetParam());        ///< configured check code type
  bool expect_fatal_error =                                std::get<1>(GetParam());        ///< indicates if a fatal error is expected
};

/**
 * @brief Test class for value-parametrized calculate crc test
 *
 * Parameter order:
 * - Configured check code type (redcty_CheckCodeType)
 * - Data size (uint16_t)
 * - Data pointer (uint8_t *)
 * - Expected error (radef_RaStaReturnCode)
 * .
 */
class CalculateCrcTestWithParam:    public redcrcTest,
                                    public testing::WithParamInterface<std::tuple<redcty_CheckCodeType, uint16_t, uint8_t*, radef_RaStaReturnCode>>
{
  public:
  redcty_CheckCodeType configured_checkcode_type =      std::get<0>(GetParam());        ///< configured check code type
  uint16_t data_size =                                  std::get<1>(GetParam());        ///< data size
  uint8_t* data_pointer =                               std::get<2>(GetParam());        ///< pointer to the test data
  radef_RaStaReturnCode expected_error =                std::get<3>(GetParam());        ///< expected error return code
};

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_crc
 * @{
 */

/**
 * @test        @ID{redcrcTest001} Verify the init function.
 *
 * @details     This test verifies that the init function sets the check code type
 *              parameter correctly.
 *
 * Test steps:
 * - set the fatal error expectation based on the test parameter
 * - expect a radef_kInvalidParameter when fatal error gets thrown
 * - call the init function with given test parameter
 * - verify if the check code type is set correctly
 * - verify that the module is set to initialized state
 * .
 *
 * @testParameter
 * - Check code type: Check code type to use
 * - Expect Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter      | Expected values                        ||
 * |----------|----------------------|--------------------|--------------------|
 * |          | Check code type      | Expect Fatal Error | Test Result        |
 * | 0        | redcty_kCheckCodeA   | False              | Normal operation   |
 * | 1        | redcty_kCheckCodeB   | False              | Normal operation   |
 * | 2        | redcty_kCheckCodeC   | False              | Normal operation   |
 * | 3        | redcty_kCheckCodeD   | False              | Normal operation   |
 * | 4        | redcty_kCheckCodeE   | False              | Normal operation   |
 * | 5        | 5                    | True               | Fatal error raised |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(InitTestWithParam, redcrcTest001VerifyInitFunction)
{
  extern uint32_t redcrc_check_code_type;

  // perform test
  if(expect_fatal_error)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &redcrcTest::invalidArgumentException));
    EXPECT_THROW(redcrc_Init(configured_checkcode_type), std::invalid_argument);
  }
  else
  {
    // call the init function and verify if parameter are set correctly
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcrc_Init(configured_checkcode_type));

    EXPECT_EQ(redcrc_check_code_type, configured_checkcode_type);
    EXPECT_TRUE(redcrc_initialized);
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyInitFunction,
  InitTestWithParam,
  ::testing::Values(
    std::make_tuple(redcty_kCheckCodeA, false),
    std::make_tuple(redcty_kCheckCodeB, false),
    std::make_tuple(redcty_kCheckCodeC, false),
    std::make_tuple(redcty_kCheckCodeD, false),
    std::make_tuple(redcty_kCheckCodeE, false),
    std::make_tuple(redcty_kCheckCodeMax, true)
  ));

/**
 * @test        @ID{redcrcTest002} Verify the number of init function calls.
 *
 * @details     This test verifies that the init function can only be called once.
 *
 * Test steps:
 * - call the init function and verify that it completes successfully
 * - call the init function again and verify that a radef_kAlreadyInitialized error is raised
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redcrcTest, redcrcTest002VerifyInitFunctionCalls)
{
  // expect fatal error function
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kAlreadyInitialized)).WillOnce(Invoke(this, &redcrcTest::invalidArgumentException));

  // call the init function and verify that it completes successful
  EXPECT_NO_THROW(redcrc_Init(redcty_kCheckCodeA));

  // call the init function again and verify that an assertion violations is raised as the module is already initialized
  EXPECT_THROW(redcrc_Init(redcty_kCheckCodeA), std::invalid_argument);
}

/**
 * @test        @ID{redcrcTest003} Null pointer checks
 *
 * @details     This test verifies that the fatal error function is executed whenever the redcrc_CalculateCrc is called
 *              with a null pointer as argument.
 *
 * Test steps:
 * - init module
 * - call the redcrc_CalculateCrc function with invalid data buffer pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the redcrc_CalculateCrc function with invalid calculated crc pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   No
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_F(redcrcTest, redcrcTest003VerifyNullPointerCheck)
{
  // init module
  redcrc_Init(redcty_kCheckCodeB);

  const uint8_t nbr_of_function_calls = 2U;        // number of function calls

  // set test variables
  uint16_t data_size = UT_REDCRC_DATA_LEN_MIN;
  uint8_t data_buffer = 0U;
  uint32_t calculated_crc = 0U;

  // set expectations
  EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
    .Times(nbr_of_function_calls)
    .WillRepeatedly(Invoke(this, &redcrcTest::invalidArgumentException));

  // perform test
  EXPECT_THROW(redcrc_CalculateCrc(data_size, (uint8_t *)NULL, &calculated_crc),std::invalid_argument);
  EXPECT_THROW(redcrc_CalculateCrc(data_size, &data_buffer, (uint32_t *)NULL),std::invalid_argument);
}

/**
 * @test        @ID{redcrcTest004} Verify the redcrc_CalculateCrc function.
 *
 * @details     This test verifies the correct behaviour of the redcrc_CalculateCrc function
 *
 * Test steps:
 * - set the fatal error expectation based on the test parameter
 * - call the init function with given test parameter
 * - verify if the check code type is calculated correctly
 * .
 *
 * @testParameter
 * - Data Size: Size of data buffer in bytes (valid range: 36 - 1109)
 * - Data pointer: Pointer to data buffer containing the data for the CRC calculation
 * - Expected error code: Expected error code when calling CalculateCrc function
 * - Check code type: configured checkcode type of crc module
 * .
 * | Test Run | Input parameter         || Expected values                                                  |||
 * |----------|-----------|--------------|-------------------------|---------------------|--------------------|
 * |          | Data Size | Data pointer | Expected error code     | Check code type     | Test Result        |
 * | 0        | 1109U     | test_message | radef_kInternalError    | redcty_kCheckCodeA  | Fatal error raised |
 * | 1        | 35U       | test_message | radef_kInvalidParameter | redcty_kCheckCodeB  | Fatal error raised |
 * | 2        | 1109U     | test_message | radef_kNoError          | redcty_kCheckCodeB  | Normal operation   |
 * | 3        | 1110U     | test_message | radef_kInvalidParameter | redcty_kCheckCodeB  | Fatal error raised |
 * | 4        | 1109U     | test_message | radef_kNoError          | redcty_kCheckCodeC  | Normal operation   |
 * | 5        | 1109U     | test_message | radef_kNoError          | redcty_kCheckCodeD  | Normal operation   |
 * | 6        | 1109U     | test_message | radef_kNoError          | redcty_kCheckCodeE  | Normal operation   |
 *
 * @safetyRel   No
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 */
TEST_P(CalculateCrcTestWithParam, redcrcTest004VerifyCalculateCrcFunction)
{
  // expected CRC values of test_message message calculated from script
  // which was tested against the DIN VDE V 0831-200 Annex A example message
  // (which cant be used due its shorter than minimum data length for the CalculateCrc function)
  uint32_t expected_crc_opt_b_value = 2448192889U;
  uint32_t expected_crc_opt_c_value = 2506336787U;
  uint32_t expected_crc_opt_d_value = 19279U;
  uint32_t expected_crc_opt_e_value = 37817U;

  // test parameters
  uint32_t calculated_crc = 0U;

  // init module
  redcrc_Init(configured_checkcode_type);

  // perform test
  if(expected_error != radef_kNoError)
  {
    // call the init function and expect a fatal error
    EXPECT_CALL(rasys_mock, rasys_FatalError(expected_error)).WillOnce(Invoke(this, &redcrcTest::invalidArgumentException));
    EXPECT_THROW(redcrc_CalculateCrc(data_size, data_pointer, &calculated_crc), std::invalid_argument);
  }
  else
  {
    // call the init function and verify if parameter are set correctly
    EXPECT_CALL(rasys_mock, rasys_FatalError(_)).Times(0);
    EXPECT_NO_THROW(redcrc_CalculateCrc(data_size, data_pointer, &calculated_crc));

    // verify generated crc value
    switch (configured_checkcode_type)
    {
      case redcty_kCheckCodeB:
        EXPECT_EQ(calculated_crc, expected_crc_opt_b_value);
        break;
      case redcty_kCheckCodeC:
        EXPECT_EQ(calculated_crc, expected_crc_opt_c_value);
        break;
      case redcty_kCheckCodeD:
        EXPECT_EQ(calculated_crc, expected_crc_opt_d_value);
        break;
      case redcty_kCheckCodeE:
        EXPECT_EQ(calculated_crc, expected_crc_opt_e_value);
        break;
    }
  }
}

INSTANTIATE_TEST_SUITE_P(
  verifyCalculateCrcFunction,
  CalculateCrcTestWithParam,
  ::testing::Values(
    std::make_tuple(redcty_kCheckCodeA, UT_REDCRC_DATA_LEN_MAX, (uint8_t*)test_message, radef_kInternalError),
    std::make_tuple(redcty_kCheckCodeB, (UT_REDCRC_DATA_LEN_MIN - 1), (uint8_t*)test_message, radef_kInvalidParameter),
    std::make_tuple(redcty_kCheckCodeB, UT_REDCRC_DATA_LEN_MAX, (uint8_t*)test_message, radef_kNoError),
    std::make_tuple(redcty_kCheckCodeB, (UT_REDCRC_DATA_LEN_MAX + 1), (uint8_t*)test_message, radef_kInvalidParameter),
    std::make_tuple(redcty_kCheckCodeC, UT_REDCRC_DATA_LEN_MAX, (uint8_t*)test_message, radef_kNoError),
    std::make_tuple(redcty_kCheckCodeD, UT_REDCRC_DATA_LEN_MAX, (uint8_t*)test_message, radef_kNoError),
    std::make_tuple(redcty_kCheckCodeE, UT_REDCRC_DATA_LEN_MAX, (uint8_t*)test_message, radef_kNoError)
  ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
