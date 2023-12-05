/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcty_red_config_types.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Definition of RaSTA redundancy layer configuration min./max. range constants.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_redundancy/redcty_red_config_types.h"

#include <stdint.h>

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

const uint32_t redcty_kMinNumberOfRedundancyChannels = 1U;

const uint32_t redcty_kMinNumberOfTransportChannels = 1U;

const uint32_t redcty_kMinTSeq = 50U;

const uint32_t redcty_kMaxTSeq = 500U;

const uint32_t redcty_kMinRedLayerNDiagnosis = 10U;

const uint32_t redcty_kMinDeferQueueSize = 4U;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
