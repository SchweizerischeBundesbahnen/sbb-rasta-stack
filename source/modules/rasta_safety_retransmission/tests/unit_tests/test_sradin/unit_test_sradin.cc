/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_sradin.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 58779ba01a1eaf5fb5ee80b55900a34bb39ab06b
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,ISSUE 4976: do not make review tests fail,14.12.2022,M. Kuhn}
 *
 * @brief Unit test file for the adapter interface module of the safety and retransmission layer.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_sradin.hh"

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

/** @addtogroup sr_adapter_interface
 * @{
 */

/**
 * @test        @ID{sradinTest001} Verify the Init sr_adapter_interface function
 *
 * @details     The RaSTA stack provides the Safety and Retransmission Layer adapter interface
 *              which is only an interface definition. The implementation is not part of the RaSTA
 *              stack and must be done by the system integrator. Therefore, the init function must
 *              not be called inside the stack. This needs to be reviewed manually.
 *
 * Test steps:
 * - Analyze the code carefully and verify that the sradin_Init function is never called in the stack.
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
 * @verifyReq{RASW-648} Init sr_adapter_interface Function
 * @verifyReq{RASW-353} Initialization Function Structure
 */
TEST_F(sradinTest, sradinTest001CallInitFunction) {
    std::cout << "Is to be reviewed manually." << std::endl;
    SUCCEED();
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
