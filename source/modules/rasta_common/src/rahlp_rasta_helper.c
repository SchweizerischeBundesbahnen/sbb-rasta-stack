/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rahlp_rasta_helper.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA helper functions.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_common/rahlp_rasta_helper.h"

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"
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
bool rahlp_IsU16InRange(const uint16_t value, const uint16_t min_value, const uint16_t max_value) {
  // Input parameter check
  if (min_value > max_value) {
    rasys_FatalError(radef_kInvalidParameter);
  }

  bool value_in_range = true;

  if ((value < min_value) || (value > max_value)) {
    value_in_range = false;
  }

  return value_in_range;
}

bool rahlp_IsU32InRange(const uint32_t value, const uint32_t min_value, const uint32_t max_value) {
  // Input parameter check
  if (min_value > max_value) {
    rasys_FatalError(radef_kInvalidParameter);
  }

  bool value_in_range = true;

  if ((value < min_value) || (value > max_value)) {
    value_in_range = false;
  }

  return value_in_range;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
