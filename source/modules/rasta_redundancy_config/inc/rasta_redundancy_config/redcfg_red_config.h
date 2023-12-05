/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcfg_red_config.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version cb4315a92d98c9390dbd583ad9cdd788738cc54c
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Interface of RaSTA redundancy layer configuration.
 *
 * This module provides the configuration data structure for the RedL.
 */
#ifndef REDCFG_RED_CONFIG_H_
#define REDCFG_RED_CONFIG_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_redundancy/redcty_red_config_types.h"

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
 * @brief Configuration data of a redundancy layer.
 */
extern const redcty_RedundancyLayerConfiguration redundancy_layer_configuration;

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDCFG_RED_CONFIG_H_
