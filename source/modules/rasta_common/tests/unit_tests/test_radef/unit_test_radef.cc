/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file unit_test_radef.cc
 *
 * @author Nicola Fricker, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Unit test file for unit tests of rasta definitions.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "unit_test_radef.hh"

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test class definitons
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Test cases
// -----------------------------------------------------------------------------

/** @addtogroup common_definitions
 * @{
 */

/**
 * @test        @ID{radefTest001} Verify the rasta definition constants
 *
 * @details     Test to verify the constants for the SWuC:
 *
 * Test steps:
 * - Verify Maximum number of RaSTA connections is set to 2
 * - Verify Maximum number of application message length is set to 1055
 * - Verify Maximum number of simultaneous redundancy channels is set to 2
 * - Verify Maximum number of transport channels per redundancy channel is set to 2
 * .
 *
 * @safetyRel   Yes
 *
 * @pre         No pre-conditions for this test case.
 * @post        No post-conditions for this test case.
 *
 * @testMethod  Functional Test
 *
 * @verifyReq{RASW-525} Component rasta_definitions Overview
 */
TEST_F(radefTest, radefTest001VerifyConstants) {
    // verify values
    ASSERT_EQ(UT_RADEF_CONNECTION_MAX, RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS);
    ASSERT_EQ(UT_RADEF_MSG_LENGTH_MAX, RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE);
    ASSERT_EQ(UT_RADEF_SIM_RED_CHANNELS_MAX, RADEF_MAX_NUMBER_OF_RED_CHANNELS);
    ASSERT_EQ(UT_RADEF_TR_CHANNEL_PER_RED_CHANNEL_MAX, RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS);
}

/** @}*/

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
