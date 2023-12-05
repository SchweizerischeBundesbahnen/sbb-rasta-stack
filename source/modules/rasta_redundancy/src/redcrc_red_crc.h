/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcrc_red_crc.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_crc
 * @{
 *
 * @brief Interface of RaSTA redundancy layer CRC module.
 *
 * This module provides all functionality needed to calculate the CRC values of RedL messages according to the following check code types defined for the RaSTA
 * redundancy layer. All details can be found in the chapter 6.3.6 "Check code" of the standard "Elektrische Bahn-Signalanlagen – Teil 200: Sicheres
 * Übertragungsprotokoll RaSTA nach DIN EN 50159 (VDE0831-159); Juni 2015".
 *
 * Supported check code types:
 *
 * Check code type A: No CRC check.
 *
 * Check code type B:
 * - width = 32
 * - polynomial = 0xEE5B42FD
 * - initial_optimized = 0
 * - refin = false
 * - refout = false
 * - final_xor = 0
 * .
 * Check code type C:
 * - width = 32U
 * - polynomial = 0x1EDC6F41
 * - initial_optimized = 0xFFFFFFFF
 * - refin = true
 * - refout = true
 * - final_xor = 0xFFFFFFFF
 * .
 * Check code type D:
 * - width = 16
 * - polynomial = 0x1021
 * - initial_optimized = 0
 * - refin = true
 * - refout = true
 * - final_xor = 0
 * .
 * Check code type E:
 * - width = 16
 * - polynomial = 0x8005
 * - initial_optimized = 0
 * - refin = true
 * - refout = true
 * - final_xor = 0
 * .
 */
#ifndef REDCRC_RED_CRC_H_
#define REDCRC_RED_CRC_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Initialize the CRC module and generate the CRC lookup table according to the configured check_code_type.
 *
 * This function is used to initialize the CRC module. It saves the passed check code type. A fatal error is raised, if this function is called multiple times.
 * This function calls the internal function GenerateCrcTable(), which generates the CRC lookup table according to the configured check_code_type. For check
 * code type A (no CRC check) no CRC lookup table is generated.
 *
 * @pre The CRC module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @param [in] configured_check_code_type Configured type of CRC check code. All enum entries of ::redcty_CheckCodeType are valid and usable.
 */
void redcrc_Init(const redcty_CheckCodeType configured_check_code_type);

/**
 * @brief Calculate the defined type of CRC of a data buffer.
 *
 * This function calculates the configured type of CRC value of the data_size bytes in the data_buffer.
 * The configured check code type must be in the range: ::redcty_kCheckCodeB <= value < ::redcty_kCheckCodeMax. If the value is outside this range, a
 * ::radef_kInternalError fatal error is thrown.
 * The configured CRC width must be in the range: ::kMinWidth <= value <= ::kMaxWidth. If the value is outside this range, a ::radef_kInternalError fatal error
 * is thrown.
 *
 * @pre The CRC module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] data_size Size of data buffer [bytes]. Valid range: ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE <= value <= ::RADEF_RED_LAYER_MESSAGE_HEADER_SIZE +
 * ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE.  If the value is outside this range, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [in] data_buffer Pointer to data buffer containing the data for the CRC calculation. If the pointer is NULL, a ::radef_kInvalidParameter fatal error
 * is thrown.
 * @param [out] calculated_crc Pointer to calculated CRC. The full value range is valid and usable. If the pointer is NULL, a ::radef_kInvalidParameter fatal
 * error is thrown.
 */
void redcrc_CalculateCrc(const uint16_t data_size, const uint8_t *const data_buffer, uint32_t *const calculated_crc);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDCRC_RED_CRC_H_
