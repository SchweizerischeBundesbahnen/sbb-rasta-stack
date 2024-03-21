/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_srmd4.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the md4 module of the safety and retransmission layer.
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRMD4_UNIT_TEST_SRMD4_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRMD4_UNIT_TEST_SRMD4_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "src/srmd4_sr_md4.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
// clang-format on

using testing::StrictMock;
using testing::Invoke;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_SRMD4_MSG_LEN_MAX 1085U                  ///< maximum message length (without safety code)
#define UT_SRMD4_MSG_LEN_MIN 28U                    ///< minimum message length (header only)
#define UT_SRMD4_MD4_HASH_LEN 16U                   ///< length of the MD4 hash
#define UT_SRMD4_DEF_INITVAL_A 0x67452301           ///< default init value A according to RFC 1320
#define UT_SRMD4_DEF_INITVAL_B 0xefcdab89           ///< default init value B according to RFC 1320
#define UT_SRMD4_DEF_INITVAL_C 0x98badcfe           ///< default init value C according to RFC 1320
#define UT_SRMD4_DEF_INITVAL_D 0x10325476           ///< default init value D according to RFC 1320
#define UT_SRMD4_MOD_INITVAL_A 0xafb16782           ///< modified init value A according to example in DIN VDE V 0831-200 Annex A
#define UT_SRMD4_MOD_INITVAL_B 0x304c59de           ///< modified init value B according to example in DIN VDE V 0831-200 Annex A

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

extern uint8_t test_message_u8_without_safety_code[UT_SRMD4_MSG_LEN_MAX];
extern uint8_t test_md4_hash[UT_SRMD4_MD4_HASH_LEN];

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the md4 module
 *
 */
class srmd4Test : public ::testing::Test
{
public:
    srmd4Test()
    {

    }
    ~srmd4Test()
    {

    }

    /**
     * @brief Throw invalid argument exception
     * This function is used to throw an invalid argument exception mocked function call action.
     */
    void invalidArgumentException(){
        throw std::invalid_argument("Invalid argument!");
    }

    StrictMock<rasysMock>   rasys_mock;     ///< mock for the rasta system adapter class
};

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_UNIT_TESTS_TEST_SRMD4_UNIT_TEST_SRMD4_HH_
