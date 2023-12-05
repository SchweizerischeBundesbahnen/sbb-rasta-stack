/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srdia_sr_diagnostics.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_diagnostics
 * @{
 *
 * @brief Interface of RaSTA SafRetL diagnostics module.
 *
 * This module provides all functionalities for the SafRetL diagnostics. It handles all possible error counters and the connection diagnostics. There are error
 * counters for messages received with:
 * - faulty safety code
 * - implausible sender or receiver identification
 * - undefined message type
 * - implausible sequence number
 * - implausible confirmed sequence number
 * .
 * The connection diagnostics contain information about the distribution of messages inside the defined diagnostic window. Information is collected for the
 * round trip time and the alive time.
 *
 * @implementsReq{RASW-636} Component sr_diagnostics Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRDIA_SAFETY_RETRANSMISSION_DIAGNOSTICS_H_
#define SRDIA_SAFETY_RETRANSMISSION_DIAGNOSTICS_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "rasta_safety_retransmission/sraty_sr_api_types.h"

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Struct for the collection of diagnostic data of a RaSTA connection.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint32_t message_counter;                        ///< number of received messages
  sraty_ConnectionDiagnosticData diagnostic_data;  ///< diagnostic data struct
} srdia_SrConnectionDiagnostics;
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

/**
 * @brief Checks if the diagnostic timings distribution intervals are valid.
 *
 * Check the validity of the diagnostic timing distribution intervals. Detailed description for this check can be found in
 * srcty_SafetyRetransmissionConfiguration::diag_timing_distr_intervals.
 *
 * @implementsReq{RASW-817} Are Diagnostic Timing Intervals Valid Function
 *
 * @param [in] t_max Configured maximal accepted age of a message [ms]. Valid range: ::srcty_kMinTMax <= value <= ::srcty_kMaxTMax.
 * @param [in] diag_timing_distr_intervals Configured diagnostic timing distribution intervals [ms]. For every element full value range is valid and usable.
 * They will be checked by this function for their validity. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @return true, if the intervals are valid
 * @return false, if the intervals are invalid
 */
bool srdia_AreDiagnosticTimingIntervalsValid(const uint32_t t_max, const uint32_t* const diag_timing_distr_intervals);

/**
 * @brief Initialize SafRetL diagnostics module.
 *
 * This function is used to initialize the diagnostic data of all connections. It saves the passed maximal age of a message (t_max), diagnostic window size
 * (n_diag_window) and the diagnostic timing distribution intervals. Additionally, ::srdia_InitConnectionDiagnostics is called for all configured connections.
 *
 * @pre The diagnostics module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-642} Init sr_diagnostics Function
 *
 * @param [in] configured_connections Number of configured RaSTA connections. Valid range: 1 <= value <= ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
 * @param [in] t_max Configured maximal accepted age of a message [ms]. Valid range: ::srcty_kMinTMax <= value <= ::srcty_kMaxTMax.
 * @param [in] n_diag_window Configured diagnosis window size [messages]. Valid range: ::srcty_kMinNDiagWindow <= value <= ::srcty_kMaxNDiagWindow.
 * @param [in] diag_timing_distr_intervals Configured diagnostic timing distribution intervals [ms].
 */
void srdia_Init(const uint32_t configured_connections, const uint32_t t_max, const uint32_t n_diag_window, const uint32_t* const diag_timing_distr_intervals);

/**
 * @brief Initialize the diagnostic data of a dedicated RaSTA connection.
 *
 * This function is used to initialize the diagnostic data of a dedicated connection. This resets the message counter of ::srdia_SrConnectionDiagnostics and
 * also all members in the embedded ::sraty_ConnectionDiagnosticData structure including all error counters and the distribution arrays.
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-643} Init Connection Diagnostics Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srdia_InitConnectionDiagnostics(const uint32_t connection_id);

/**
 * @brief Increment the safety code error counter of a dedicated RaSTA connection.
 *
 * This function is used to increment the error counter for messages received with a faulty safety code of a dedicated connection.
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-639} Inc Safety Code Error Counter Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srdia_IncSafetyCodeErrorCounter(const uint32_t connection_id);

/**
 * @brief Increment the address error counter of a dedicated RaSTA connection.
 *
 * This function is used to increment the error counter for messages received with a implausible sender or receiver identification of a dedicated connection.
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-637} Inc Address Error Counter Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srdia_IncAddressErrorCounter(const uint32_t connection_id);

/**
 * @brief Increment the message type error counter of a dedicated RaSTA connection.
 *
 * This function is used to increment the error counter for messages received with undefined message type of a dedicated connection.
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-641} Inc Type Error Counter Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srdia_IncTypeErrorCounter(const uint32_t connection_id);

/**
 * @brief Increment the sequence number error counter of a dedicated RaSTA connection.
 *
 * This function is used to increment the error counter for messages received with an implausible sequence number of a dedicated connection.
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-640} Inc Sequence Number Error Counter Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srdia_IncSequenceNumberErrorCounter(const uint32_t connection_id);

/**
 * @brief Increment the confirmed sequence number error counter of a dedicated RaSTA connection.
 *
 * This function is used to increment the error counter for messages received with an implausible confirmed sequence number of a dedicated connection.
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-638} Inc Confirmed Sequence Number Error Counter Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srdia_IncConfirmedSequenceNumberErrorCounter(const uint32_t connection_id);

/**
 * @brief Update the received message timing statistics of a dedicated RaSTA connection.
 *
 * This function is used to update the received message timing statistics for a newly received time-out related message. For the passed round trip delay and
 * alive time, the corresponding timing distributions are updated according the passed value by calling ::AddTimeToTimingDistribution. Additionally,
 * the internal received messages counter is incremented and if the diagnosis window (n_diag_window) is reached (message counter >= n_diag_window), a
 * diagnostic notification is send with the current diagnostics data (::srdia_SendDiagnosticNotification).
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-645} Update Connection Diagnostics Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] round_trip_delay Round trip delay of a received message (T_rtd) [ms]. Valid range: 0 <= value <= configured t_max.
 * @param [in] alive_time Alive time of a received message (T_alive) [ms]. Valid range: 0 <= value <= configured t_max.
 */
void srdia_UpdateConnectionDiagnostics(const uint32_t connection_id, const uint32_t round_trip_delay, const uint32_t alive_time);

/**
 * @brief Send a diagnostic notification of a dedicated RaSTA connection.
 *
 * This function is used to send a diagnostic notification (::srnot_SrDiagnosticNotification) to the application layer containing the current diagnostic data
 * of a dedicated RaSTA connection. After sending the notification, the diagnostic data of the dedicated connection is reset using
 * ::srdia_InitConnectionDiagnostics.
 *
 * @pre The diagnostics module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-644} Send Diagnostic Notification Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srdia_SendDiagnosticNotification(const uint32_t connection_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRDIA_SAFETY_RETRANSMISSION_DIAGNOSTICS_H_
