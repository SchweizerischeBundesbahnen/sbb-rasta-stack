/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srmd4.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for the md4 module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_srmd4.hh"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

const uint16_t example_message_length = 28;      ///< length of the example message in DIN VDE V 0831-200 Annex A

const uint8_t example_message[example_message_length] = {
    0x24, 0x00, 0x4c, 0x18, 0x3f, 0xb4, 0x96,
    0x00, 0xce, 0xca, 0x23, 0x00, 0x56, 0x44,
    0x33, 0x22, 0x66, 0x55, 0x44, 0x33, 0x57,
    0x01, 0x00, 0x00, 0xcb, 0x00, 0x00, 0x00
};      ///< example message according to DIN VDE V 0831-200 Annex A

const uint8_t example_message_md4_default[UT_SRMD4_MD4_HASH_LEN] = {
    0x83, 0xf0, 0xd0, 0x52, 0x40, 0x6b, 0xf4, 0x92,
    0xf8, 0x9f, 0x8d, 0x1e, 0x9b, 0x89, 0xc9, 0x8d
};      ///< md4 hash (default init) of example message according to DIN VDE V 0831-200 Annex A

const uint8_t example_message_md4_modified[UT_SRMD4_MD4_HASH_LEN] = {
    0x93, 0x9f, 0x1c, 0x86, 0x59, 0xcf, 0xf5, 0x03,
    0x95, 0x3c, 0x4e, 0xd3, 0x25, 0x96, 0x89, 0x5d
};      ///< md4 hash (modified init) of example message according to DIN VDE V 0831-200 Annex A

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

/**
 * @brief Test class for value-parametrized init test
 *
 * Parameter order:
 * - srcty_Md4InitValue.init_a (uint32_t)
 * - srcty_Md4InitValue.init_b (uint32_t)
 * - srcty_Md4InitValue.init_c (uint32_t)
 * - srcty_Md4InitValue.init_d (uint32_t)
 * - data size (uint16_t)
 * - data pointer (uint8_t)
 * - expected result data pointer (uint8_t)
 * - expect fatal error (bool)
 * .
 */
class Md4TestWithParam :    public srmd4Test,
                            public testing::WithParamInterface<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint16_t, uint8_t*, uint8_t*, bool>>
{
    public:
    uint32_t init_value_a =             std::get<0>(GetParam());        ///< MD4 initial value A
    uint32_t init_value_b =             std::get<1>(GetParam());        ///< MD4 initial value B
    uint32_t init_value_c =             std::get<2>(GetParam());        ///< MD4 initial value C
    uint32_t init_value_d =             std::get<3>(GetParam());        ///< MD4 initial value D
    uint16_t data_size =                std::get<4>(GetParam());        ///< data size
    uint8_t * data_ptr =                std::get<5>(GetParam());        ///< pointer to the data
    uint8_t * result_ptr =              std::get<6>(GetParam());        ///< pointer to the result data
    bool expect_fatal_error =           std::get<7>(GetParam());        ///< indicates if a fatal error is expected
};

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup sr_md4
 * @{
 */

/**
 * @test        @ID{srmd4Test001} Null pointer check
 *
 * @details     This test verifies that the fatal error function is executed whenever the
 *              CalculateMd4 function is called with a null pointer as argument.
 *
 * Test steps:
 * - call the CalculateMd4 function with invalid data pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * - call the CalculateMd4 function with invalid result pointer and verify that a fatal error with error code radef_kInvalidParameter gets thrown
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-632} Component sr_md4 Overview
 * @verifyReq{RASW-633} Calculate MD4 Function
 */
TEST_F(srmd4Test, srmd4Test001VerifyNullPointerCheck)
{
    uint8_t nbr_of_function_calls = 2;          // number of function calls

    // Test variables
    srcty_Md4InitValue init_values = {};
    uint8_t dummy_data = 0;
    srmd4_Md4 md4_result = {};

    // expect fatal error function
    EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter))
        .Times(nbr_of_function_calls)
        .WillRepeatedly(Invoke(this, &srmd4Test::invalidArgumentException));

    // call the functions
    EXPECT_THROW(srmd4_CalculateMd4(init_values, UT_SRMD4_MSG_LEN_MIN, (uint8_t*)NULL, &md4_result), std::invalid_argument);
    EXPECT_THROW(srmd4_CalculateMd4(init_values, UT_SRMD4_MSG_LEN_MIN, &dummy_data, (srmd4_Md4*)NULL), std::invalid_argument);
}

/**
 * @test        @ID{srmd4Test002} Verify the CalculateMd4 function.
 *
 * @details     This test verifies that the calculation of the MD4 hash based on given
 *              data and  initial values.
 *
 * Test steps:
 * - set the fatal error expectation for radef_kInvalidParameter based on the test parameter
 * - call the CalculateMd4 function with given test parameter
 * - verify if the MD4 hash matches the expected value
 * .
 *
 * @testParameter
 * - Init value A: Configured initial value A
 * - Init value B: Configured initial value B
 * - Init value C: Configured initial value C
 * - Init value D: Configured initial value D
 * - Data Size: size of data buffer in bytes (valid range: 28 - 1085)
 * - Data Pointer: Data pointer to buffer which contains test message
 * - Exp. Result Pointer: Expected result pointer
 * - Exp. Fatal Error: Indicates if a fatal error is expected
 * .
 * | Test Run | Input parameter                                                                                        |||||| Expected values                                                    |||
 * |----------|--------------|--------------|--------------|--------------|-----------|-------------------------------------|------------------------------|------------------|--------------------|
 * |          | Init value A | Init value B | Init value C | Init value D | Data Size | Data Pointer                        | Exp. Result Pointer          | Exp. Fatal Error | Test Result        |
 * | 0        | 0            | 0            | 0            | 0            | 28        | test_message_u8_without_safety_code | NULL                         | False            | Normal operation   |
 * | 1        | 0xffffffff   | 0            | 0            | 0            | 28        | test_message_u8_without_safety_code | NULL                         | False            | Normal operation   |
 * | 2        | 0            | 0xffffffff   | 0            | 0            | 28        | test_message_u8_without_safety_code | NULL                         | False            | Normal operation   |
 * | 3        | 0            | 0            | 0xffffffff   | 0            | 28        | test_message_u8_without_safety_code | NULL                         | False            | Normal operation   |
 * | 4        | 0            | 0            | 0            | 0xffffffff   | 28        | test_message_u8_without_safety_code | NULL                         | False            | Normal operation   |
 * | 5        | 0xffffffff   | 0xffffffff   | 0xffffffff   | 0xffffffff   | 28        | test_message_u8_without_safety_code | NULL                         | False            | Normal operation   |
 * | 6        | 0            | 0            | 0            | 0            | 27        | test_message_u8_without_safety_code | NULL                         | True             | Fatal error raised |
 * | 7        | 0            | 0            | 0            | 0            | 1086      | test_message_u8_without_safety_code | NULL                         | True             | Fatal error raised |
 * | 8        | 0x67452301   | 0xefcdab89   | 0x98badcfe   | 0x10325476   | 1085      | test_message_u8_without_safety_code | test_md4_hash                | False            | Normal operation   |
 * | 9        | 0x67452301   | 0xefcdab89   | 0x98badcfe   | 0x10325476   | 28        | example_message                     | example_message_md4_default  | False            | Normal operation   |
 * | 10       | 0xafb16782   | 0x304c59de   | 0x98badcfe   | 0x10325476   | 28        | example_message                     | example_message_md4_modified | False            | Normal operation   |
 *
 * @safetyRel   Yes
 *
 * @pre         This is a value-parametrized test case which needs instantiation with defined values.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-632} Component sr_md4 Overview
 * @verifyReq{RASW-633} Calculate MD4 Function
 * @verifyReq{RASW-634} Safety Code
 */
TEST_P(Md4TestWithParam, srmd4Test002VerifyMd4Calculation)
{
    srcty_Md4InitValue init_values = {};
    srmd4_Md4 md4_result = {};

    // get init values from test parameter
    init_values.init_a = init_value_a;
    init_values.init_b = init_value_b;
    init_values.init_c = init_value_c;
    init_values.init_d = init_value_d;

    if (expect_fatal_error) {
        // call the CalculateMd4 function and expect a fatal error
        EXPECT_CALL(rasys_mock, rasys_FatalError(radef_kInvalidParameter)).WillOnce(Invoke(this, &srmd4Test::invalidArgumentException));
        EXPECT_THROW(srmd4_CalculateMd4(init_values, data_size, data_ptr, &md4_result), std::invalid_argument);
    }
    else {
        // call the CalculateMd4 function
        EXPECT_NO_THROW(srmd4_CalculateMd4(init_values, data_size, data_ptr, &md4_result));
    }

    // verify if the calculated md4 hash matches the expectation
    if (result_ptr != NULL) {
        for (uint8_t i = 0; i < UT_SRMD4_MD4_HASH_LEN; i++) {
            EXPECT_EQ(md4_result.md4[i], result_ptr[i]);
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    verifyMd4Calculation,
    Md4TestWithParam,
    ::testing::Values(
        std::make_tuple(0, 0, 0, 0, UT_SRMD4_MSG_LEN_MIN, test_message_u8_without_safety_code, (uint8_t*)NULL, false),
        std::make_tuple(UINT32_MAX, 0, 0, 0, UT_SRMD4_MSG_LEN_MIN, test_message_u8_without_safety_code, (uint8_t*)NULL, false),
        std::make_tuple(0, UINT32_MAX, 0, 0, UT_SRMD4_MSG_LEN_MIN, test_message_u8_without_safety_code, (uint8_t*)NULL, false),
        std::make_tuple(0, 0, UINT32_MAX, 0, UT_SRMD4_MSG_LEN_MIN, test_message_u8_without_safety_code, (uint8_t*)NULL, false),
        std::make_tuple(0, 0, 0, UINT32_MAX, UT_SRMD4_MSG_LEN_MIN, test_message_u8_without_safety_code, (uint8_t*)NULL, false),
        std::make_tuple(UINT32_MAX, UINT32_MAX, UINT32_MAX, UINT32_MAX, UT_SRMD4_MSG_LEN_MIN, test_message_u8_without_safety_code, (uint8_t*)NULL, false),
        std::make_tuple(0, 0, 0, 0, (UT_SRMD4_MSG_LEN_MIN - 1), test_message_u8_without_safety_code, (uint8_t*)NULL, true),
        std::make_tuple(0, 0, 0, 0, (UT_SRMD4_MSG_LEN_MAX + 1), test_message_u8_without_safety_code, (uint8_t*)NULL, true),
        std::make_tuple(UT_SRMD4_DEF_INITVAL_A, UT_SRMD4_DEF_INITVAL_B, UT_SRMD4_DEF_INITVAL_C, UT_SRMD4_DEF_INITVAL_D, UT_SRMD4_MSG_LEN_MAX, test_message_u8_without_safety_code, test_md4_hash, false),
        std::make_tuple(UT_SRMD4_DEF_INITVAL_A, UT_SRMD4_DEF_INITVAL_B, UT_SRMD4_DEF_INITVAL_C, UT_SRMD4_DEF_INITVAL_D, example_message_length, (uint8_t*)example_message, (uint8_t*)example_message_md4_default, false),
        std::make_tuple(UT_SRMD4_MOD_INITVAL_A, UT_SRMD4_MOD_INITVAL_B, UT_SRMD4_DEF_INITVAL_C, UT_SRMD4_DEF_INITVAL_D, example_message_length, (uint8_t*)example_message, (uint8_t*)example_message_md4_modified, false)
    ));

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
