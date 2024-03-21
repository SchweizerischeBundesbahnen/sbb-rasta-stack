/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_redtri.cc
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fc82ba36ffcd09e933b8b5e8f24de7121d9b915b
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4980: Add new test redtriTest001,19.12.2022,M. Kuhn}
 *
 * @brief Unit test file for the transport interface component of the redundancy layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_redtri.hh"

using ::testing::_;

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

// -----------------------------------------------------------------------------
// Test Cases
// -----------------------------------------------------------------------------

/** @addtogroup red_transport_interface
 * @{
 */

/**
 * @test        @ID{redtriTest001} Verify the redtri_Init function
 *
 * @details     The RaSTA stack provides the Redundancy Layer transport interface
 *              which is only an interface definition. The implementation is not part of the RaSTA
 *              stack and must be done by the system integrator. Therefore, the init function must
 *              not be called inside the stack. This needs to be reviewed manually.
 *
 * Test steps:
 * - Analyze the code carefully and verify that the redtri_Init function is never called in the stack.
 * - Add a description of the review result in the test report under "Manual Test Execution".
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Review
 */
TEST_F(redtriTest, redtriTest001CallInitFunction) {
    std::cout << "Is to be reviewed manually." << std::endl;
    SUCCEED();
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
