/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcfg_sr_config.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version ba293ba6efefd175e9347cbdc8b80d3f122e3f0d
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Interface of RaSTA SafRetL configuration.
 *
 * This module provides the configuration data structure for the SafRetL.
 */
#ifndef SRCFG_SAFETY_RETRANSMISSION_CONFIG_H_
#define SRCFG_SAFETY_RETRANSMISSION_CONFIG_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_safety_retransmission/srcty_sr_config_types.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Configuration data of a safety and retransmission layer.
 */
extern const srcty_SafetyRetransmissionConfiguration safety_retransmission_configuration;

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRCFG_SAFETY_RETRANSMISSION_CONFIG_H_
