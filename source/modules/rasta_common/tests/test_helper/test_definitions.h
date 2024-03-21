/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file test_definitions.h
 *
 * @author Martin Kuhn, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Definitions for unit and integration tests
 */

#ifndef SOURCE_MODULES_RASTA_COMMON_TESTS_TEST_HELPER_TEST_DEFINITIONS_H_
#define SOURCE_MODULES_RASTA_COMMON_TESTS_TEST_HELPER_TEST_DEFINITIONS_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Undefine PRIVATE macro to access local variables during unit test
 *
 */
#ifdef PRIVATE
#undef PRIVATE
#define PRIVATE
#endif

#endif  // SOURCE_MODULES_RASTA_COMMON_TESTS_TEST_HELPER_TEST_DEFINITIONS_H_
