/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srmd4_sr_md4.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_md4
 * @{
 *
 * @brief Interface of RaSTA SafRetL MD4 module.
 *
 * This module provides the MD4 hash calculation functionality as specified in RFC 1320.
 *
 * This implementation is based on the "A portable, fast, and free implementation of the MD4 Message-Digest Algorithm (RFC 1320)" implementation of Alexander
 * Peslyak in 2001. www.openwall.info/wiki/people/solar/software/public-domain-source-code/md4
 *
 * The source code is refactored to achieve the requirements of MISRA C:2012 and EN50128 for SIL4.
 *
 * @implementsReq{RASW-632} Component sr_md4 Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRMD4_SAFETY_RETRANSMISSION_MD4_H_
#define SRMD4_SAFETY_RETRANSMISSION_MD4_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Typedef for MD4 result.
 */
typedef struct {
  uint8_t md4[RADEF_MAX_SR_LAYER_SAFETY_CODE_SIZE];  ///< Array containing the full MD4 hash
} srmd4_Md4;

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Calculate the MD4 of a data buffer.
 *
 * @implementsReq{RASW-633} Calculate MD4 Function
 * @implementsReq{RASW-634} Safety Code
 *
 * @param [in] md4_initial_value MD4 initial value. Valid values are defined in ::srcty_Md4InitValue.
 * @param [in] data_size size of data buffer [bytes]. Valid range: ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <= value <= (::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE -
 * ::RADEF_MAX_SR_LAYER_SAFETY_CODE_SIZE).
 * @param [in] data_buffer Pointer to data buffer. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] calculated_md4 Pointer to array with calculated MD4. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void srmd4_CalculateMd4(const srcty_Md4InitValue md4_initial_value, const uint16_t data_size, const uint8_t *const data_buffer,
                        srmd4_Md4 *const calculated_md4);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRMD4_SAFETY_RETRANSMISSION_MD4_H_
