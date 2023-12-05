/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redtyp_red_types.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @addtogroup red_types
 * @{
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Internal type definitions of RaSTA redundancy layer.
 *
 * This module defines the internal data types of the RaSTA RedL like the RedL PDU message and the RedL PDU message payload.
 */
#ifndef REDTYP_RED_TYPES_H_
#define REDTYP_RED_TYPES_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Typedef for a redundancy layer PDU message.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint16_t message_size;  ///< Used message size [bytes]. Valid range: ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE <= value <= ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE
  uint8_t message[RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE];  ///< Message buffer. For the message data the full value range is valid and usable.
} redtyp_RedundancyMessage;
//lint -restore

/**
 * @brief Typedef for a redundancy layer PDU message payload.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint16_t payload_size;  ///< Used payload size [bytes]. Valid range: ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <= value <= ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE.
  uint8_t payload[RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE];  ///< Payload buffer. For the payload data the full value range is valid and usable.
} redtyp_RedundancyMessagePayload;
//lint -restore

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDTYP_RED_TYPES_H_
