/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file raas_rasta_assert.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA assert functions.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_common/raas_rasta_assert.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_common/rahlp_rasta_helper.h"
#include "rasta_common/rasys_rasta_system_adapter.h"

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
void raas_AssertNotNull(const void *const pointer, const radef_RaStaReturnCode error_reason) {
  // Input parameter check
  if (!rahlp_IsU32InRange((uint32_t)error_reason, (uint32_t)radef_kMin, (uint32_t)radef_kMax - 1U)) {
    rasys_FatalError(radef_kInvalidParameter);
  }

  if (pointer == NULL) {
    rasys_FatalError(error_reason);
  }
}

void raas_AssertTrue(const bool condition, const radef_RaStaReturnCode error_reason) {
  // Input parameter check
  if (!rahlp_IsU32InRange((uint32_t)error_reason, (uint32_t)radef_kMin, (uint32_t)radef_kMax - 1U)) {
    rasys_FatalError(radef_kInvalidParameter);
  }

  if (!condition) {
    rasys_FatalError(error_reason);
  }
}

void raas_AssertU8InRange(const uint8_t value, const uint8_t min_value, const uint8_t max_value, const radef_RaStaReturnCode error_reason) {
  // Input parameter check
  raas_AssertTrue(min_value <= max_value, radef_kInvalidParameter);
  if (!rahlp_IsU32InRange((uint32_t)error_reason, (uint32_t)radef_kMin, (uint32_t)radef_kMax - 1U)) {
    rasys_FatalError(radef_kInvalidParameter);
  }

  if ((value < min_value) || (value > max_value)) {
    rasys_FatalError(error_reason);
  }
}

void raas_AssertU16InRange(const uint16_t value, const uint16_t min_value, const uint16_t max_value, const radef_RaStaReturnCode error_reason) {
  // Input parameter check
  raas_AssertTrue(min_value <= max_value, radef_kInvalidParameter);
  if (!rahlp_IsU32InRange((uint32_t)error_reason, (uint32_t)radef_kMin, (uint32_t)radef_kMax - 1U)) {
    rasys_FatalError(radef_kInvalidParameter);
  }

  if ((value < min_value) || (value > max_value)) {
    rasys_FatalError(error_reason);
  }
}

void raas_AssertU32InRange(const uint32_t value, const uint32_t min_value, const uint32_t max_value, const radef_RaStaReturnCode error_reason) {
  // Input parameter check
  raas_AssertTrue(min_value <= max_value, radef_kInvalidParameter);
  if (!rahlp_IsU32InRange((uint32_t)error_reason, (uint32_t)radef_kMin, (uint32_t)radef_kMax - 1U)) {
    rasys_FatalError(radef_kInvalidParameter);
  }

  if ((value < min_value) || (value > max_value)) {
    rasys_FatalError(error_reason);
  }
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
