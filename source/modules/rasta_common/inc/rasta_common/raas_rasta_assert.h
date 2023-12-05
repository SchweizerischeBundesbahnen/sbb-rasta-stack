/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file raas_rasta_assert.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup common_assert
 * @{
 *
 * @brief Interface of the RaSTA assert functions.
 *
 * This module provides all necessary assert functions to perform checks and throw a specific fatal error if the check fails.
 *
 * @implementsReq{RASW-533} Component rasta_assert Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 */
#ifndef RAAS_RASTA_ASSERT_H_
#define RAAS_RASTA_ASSERT_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"

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
 * @brief Assert if a pointer is not NULL.
 *
 * This function checks if a pointer is not NULL. If that is not the case, the passed error code is thrown as fatal error using ::rasys_FatalError.
 *
 * @implementsReq{RASW-534} Assert not Null Function
 * @implementsReq{RASW-521} Input Parameter Check
 *
 * @param [in] pointer Pointer to check.
 * @param [in] error_reason Reason in case of error. Valid range: ::radef_kMin <= value < ::radef_kMax.
 */
void raas_AssertNotNull(const void *const pointer, const radef_RaStaReturnCode error_reason);

/**
 * @brief Assert a bool condition is true.
 *
 * This function checks if a bool condition is true. If that is not the case, the passed error code is thrown as fatal error using ::rasys_FatalError.
 *
 * @implementsReq{RASW-535} Assert True Function
 * @implementsReq{RASW-521} Input Parameter Check
 *
 * @param [in] condition Condition to check.
 * @param [in] error_reason Reason in case of error. Valid range: ::radef_kMin <= value < ::radef_kMax.
 */
void raas_AssertTrue(const bool condition, const radef_RaStaReturnCode error_reason);

/**
 * @brief Assert if a uint8_t value is in a defined range.
 *
 * This function checks if a uint8_t value is inside a defined range. Check condition: min_value <= value <= max_value. The minimum value must be
 * smaller or equal to the maximum value, otherwise a ::radef_kInvalidParameter fatal error is thrown.
 *
 * @implementsReq{RASW-538} Assert U8 in Range Function
 * @implementsReq{RASW-521} Input Parameter Check
 *
 * @param [in] value Value to check. The full value range is valid and usable.
 * @param [in] min_value Minimum value for the check. The full value range is valid and usable.
 * @param [in] max_value Maximum value for the check. The full value range is valid and usable.
 * @param [in] error_reason Reason in case of error. Valid range: ::radef_kMin <= value < ::radef_kMax.
 */
void raas_AssertU8InRange(const uint8_t value, const uint8_t min_value, const uint8_t max_value, const radef_RaStaReturnCode error_reason);

/**
 * @brief Assert if a uint16_t value is in a defined range.
 *
 * This function checks if a uint16_t value is inside a defined range. Check condition: min_value <= value <= max_value. The minimum value must be
 * smaller or equal to the maximum value, otherwise a ::radef_kInvalidParameter fatal error is thrown.
 *
 * @implementsReq{RASW-536} Assert U16 in Range Function
 * @implementsReq{RASW-521} Input Parameter Check
 *
 * @param [in] value Value to check. The full value range is valid and usable.
 * @param [in] min_value Minimum value for the check. The full value range is valid and usable.
 * @param [in] max_value Maximum value for the check. The full value range is valid and usable.
 * @param [in] error_reason Reason in case of error. Valid range: ::radef_kMin <= value < ::radef_kMax.
 */
void raas_AssertU16InRange(const uint16_t value, const uint16_t min_value, const uint16_t max_value, const radef_RaStaReturnCode error_reason);

/**
 * @brief Assert if a uint32_t value is in a defined range.
 *
 * This function checks if a uint32_t value is inside a defined range. Check condition: min_value <= value <= max_value. The minimum value must be
 * smaller or equal to the maximum value, otherwise a ::radef_kInvalidParameter fatal error is thrown.
 *
 * @implementsReq{RASW-537} Assert U32 in Range Function
 * @implementsReq{RASW-521} Input Parameter Check
 *
 * @param [in] value Value to check. The full value range is valid and usable.
 * @param [in] min_value Minimum value for the check. The full value range is valid and usable.
 * @param [in] max_value Maximum value for the check. The full value range is valid and usable.
 * @param [in] error_reason Reason in case of error. Valid range: ::radef_kMin <= value < ::radef_kMax.
 */
void raas_AssertU32InRange(const uint32_t value, const uint32_t min_value, const uint32_t max_value, const radef_RaStaReturnCode error_reason);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // RAAS_RASTA_ASSERT_H_
