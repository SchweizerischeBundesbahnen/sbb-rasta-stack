/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rahlp_rasta_helper.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup common_helper
 * @{
 *
 * @brief Interface of the RaSTA helper functions.
 *
 * This module provides some helper functions for common used functionalities.
 *
 * @implementsReq{RASW-818} Component rasta_Helper Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 */
#ifndef RAHLP_RASTA_HELPER_H_
#define RAHLP_RASTA_HELPER_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

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
 * @brief Checks if a uint16_t value is in a defined range.
 *
 * This helper function checks if a uint16_t value is inside a defined range. Check condition: min_value <= value <= max_value. The minimum value must be
 * smaller or equal to the maximum value, otherwise a ::radef_kInvalidParameter fatal error is thrown.
 *
 * @implementsReq{RASW-821} Is U16 in Range Function
 * @implementsReq{RASW-521} Input Parameter Check
 *
 * @param [in] value Value to check. The full value range is valid and usable.
 * @param [in] min_value Minimum value for the check. The full value range is valid and usable.
 * @param [in] max_value Maximum value for the check. The full value range is valid and usable.
 * @return true -> successful operation, value inside the range
 * @return false -> value outside the range
 */
bool rahlp_IsU16InRange(const uint16_t value, const uint16_t min_value, const uint16_t max_value);

/**
 * @brief Checks if a uint32_t value is in a defined range.
 *
 * This helper function checks if a uint32_t value is inside a defined range. Check condition: min_value <= value <= max_value. The minimum value must be
 * smaller or equal to the maximum value, otherwise a ::radef_kInvalidParameter fatal error is thrown.
 *
 * @implementsReq{RASW-820} Is U32 in Range Function
 * @implementsReq{RASW-521} Input Parameter Check
 *
 * @param [in] value Value to check. The full value range is valid and usable.
 * @param [in] min_value Minimum value for the check. The full value range is valid and usable.
 * @param [in] max_value Maximum value for the check. The full value range is valid and usable.
 * @return true -> successful operation, value inside the range
 * @return false -> value outside the range
 */
bool rahlp_IsU32InRange(const uint32_t value, const uint32_t min_value, const uint32_t max_value);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // RAHLP_RASTA_HELPER_H_
