/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file rasys_rasta_system_adapter.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup common_system_adapter
 * @{
 *
 * @brief Interface of the RaSTA system adapter functions.
 *
 * This module defines the interface to the necessary system functions used by the SW. This includes functionalities related to time, fatal error handling and
 * random number generation. The RaSTA common only defines the interface, the implementation of this system adapter interface functions must be done by the
 * system integrator.
 *
 * @remark The error handling for all function must be implemented and handled by the system integrator when developing the SafRetL adapter.
 *
 * @implementsReq{RASW-527} Component rasta_system_adapter Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 */
#ifndef RASYS_RASTA_SYSTEM_ADAPTER_H_
#define RASYS_RASTA_SYSTEM_ADAPTER_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
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
 * @brief Returns the actual value of a free running up counting timer.
 *
 * @implementsReq{RASW-531} Get Timer Value Function
 * @implementsReq{RASW-410} Get Timer Value Function Structure
 * @implementsReq{RASW-422} Timer Value
 *
 * @return uint32_t Time [ms]. The full range of the uint32_t type is used.
 */
uint32_t rasys_GetTimerValue(void);

/**
 * @brief Returns the granularity of the free running up counting timer.
 *
 * @implementsReq{RASW-530} Get Timer Granularity Function
 * @implementsReq{RASW-420} Get Timer Granularity Function Structure
 * @implementsReq{RASW-419} Timer Granularity
 *
 * @return uint32_t Granularity of the timer [ms].
 */
uint32_t rasys_GetTimerGranularity(void);

/**
 * @brief Returns a random generated number within the uint32_t type range.
 *
 * The value is used to randomize sequence number at startup. There is no cryptographic function which relies on that value. Therefore, a simple algorithm with
 * a different seed value at startup is sufficient.
 *
 * @implementsReq{RASW-529} Get Random Number Function
 * @implementsReq{RASW-414} Get Random Number Function Structure
 * @implementsReq{RASW-413} Random Number
 *
 * @return uint32_t Random number
 */
uint32_t rasys_GetRandomNumber(void);

/**
 * @brief Fatal error function
 *
 * This function returns the program execution to the operating system. This function is called in case of a fatal internal error. Important: This function is
 * not allowed to return.
 *
 * @implementsReq{RASW-528} Fatal Error Function
 * @implementsReq{RASW-417} Fatal Error Handling Function Structure
 * @implementsReq{RASW-416} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-520} Error Handling
 *
 * @param [in] error_reason Reason of the fatal error. Valid range: ::radef_kMin <= value < ::radef_kMax.
 */
void rasys_FatalError(const radef_RaStaReturnCode error_reason);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // RASYS_RASTA_SYSTEM_ADAPTER_H_
