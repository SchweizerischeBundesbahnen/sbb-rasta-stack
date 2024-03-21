/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file macros.hh
 *
 * @author Patrick Haldi, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version bfa7bcef58ee9ac7504a4ae227294d26848e4b9f
 *
 * @change{-,Initial version,-,-}
 *
 * @brief some helper C macros
 */
#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_INTEGRATION_TESTS_INTEGRATION_TEST_MACROS_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_INTEGRATION_TESTS_INTEGRATION_TEST_MACROS_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#define GET_BYTE(n, b)  ((uint8_t)(((n) >> (8U * (b))) & 0xFF))
#define U16(n)  GET_BYTE(n, 0), GET_BYTE(n, 1)
#define U32(n)  GET_BYTE(n, 0), GET_BYTE(n, 1), GET_BYTE(n, 2), GET_BYTE(n, 3)

#define ARRAY_SIZE(arr)  (sizeof(arr) / sizeof(arr[0]))

#define U32_MAX ((uint32_t)-1)

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------

#endif  // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_INTEGRATION_TESTS_INTEGRATION_TEST_MACROS_HH_
