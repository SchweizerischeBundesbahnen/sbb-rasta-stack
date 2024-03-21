/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_raas.hh
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Header file for unit tests of the assert module of the common package.
 */
#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RAAS_UNIT_TEST_RAAS_HH_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RAAS_UNIT_TEST_RAAS_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

// clang-format off
#include <vector>
#include <cmath>
#include <stdexcept>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "rasta_common/raas_rasta_assert.h"
#include "rasys_mock.hh"
// clang-format on

using testing::Invoke;
using testing::StrictMock;
using testing::NaggyMock;

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

#define UT_RAAS_FIRST_ERROR (radef_RaStaReturnCode)(radef_kMin + 1)     ///< First valid error different from radef_kNoError
#define UT_RAAS_LAST_ERROR (radef_RaStaReturnCode)(radef_kMax - 1)      ///< Last valid error

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test class definiton
// -----------------------------------------------------------------------------

/**
 * @brief Test fixture class for the rasta assert module
 *
 */
class raasTest : public ::testing::Test
{
public:
    raasTest()
    {

    }
    ~raasTest()
    {

    }

    /**
    * @brief Throw invalid argument exception
    * This function is used to throw an invalid argument exception mocked function call action.
    */
    void invalidArgumentException(){
        throw std::invalid_argument("Invalid argument!");
    }

    StrictMock<rasysMock>   rasys_mock;        ///< mock for the rasta system adapter class
};

#endif  // SOURCE_MODULES_RASTA_COMMON_TESTS_UNIT_TESTS_TEST_RAAS_UNIT_TEST_RAAS_HH_
