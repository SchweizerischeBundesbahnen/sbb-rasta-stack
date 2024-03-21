/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_ralog.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 28c2c1fd49705792b1e28f55c490d6ae6cc6e7bc
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4976: do not make review tests fail,14.12.2022,M. Kuhn}
 *
 * @brief Unit test file for the of the logger module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_ralog.hh"

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup common_logger
 * @{
 */

/**
 * @test        @ID{ralogTest001} Verify the logger function
 *
 * @details     The logger functionality is intended to be only available in a debug build. Therefore it
 *              has to be completely omitted in a release build. This needs to be reviewed manually.
 *
 * Test steps:
 * - Analyze the code carefully and verify that the logger is only available in a debug build.
 * - Add a description of the review result in the test report under "Manual Test Execution".
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Review
 *
 * @verifyReq{RASW-540} Component rasta_logger Overview
 */
TEST_F(ralogTest, ralogTest001CallLogFunction) {
    std::cout << "Is to be reviewed manually." << std::endl;
    SUCCEED();
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
