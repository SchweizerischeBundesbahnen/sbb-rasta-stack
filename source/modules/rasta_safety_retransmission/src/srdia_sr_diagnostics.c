/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srdia_sr_diagnostics.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA SafRetL diagnostics module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srdia_sr_diagnostics.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "rasta_safety_retransmission/srnot_sr_notifications.h"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of the module.
 * true, if the module is initialized.
 */
PRIVATE bool srdia_initialized = false;

/**
 * @brief Number of configured RaSTA connections.
 */
PRIVATE uint32_t srdia_number_of_connections = 0U;

/**
 * @brief Maximal accepted age of a message [ms].
 */
PRIVATE uint32_t srdia_t_max = 0U;

/**
 * @brief Configured diagnosis window size [messages].
 */
PRIVATE uint32_t srdia_n_diag_window = 0U;

/**
 * @brief Diagnostic timing distribution intervals [ms].
 */
PRIVATE uint32_t srdia_diag_timing_distr_intervals[RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_ARRAY_SIZE];

/**
 * @brief Received buffers for all RaSTA connections.
 */
PRIVATE srdia_SrConnectionDiagnostics srdia_connection_diagnostics[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

/**
 * @brief ID of the sr_diagnostics debug logger.
 */
PRIVATE uint16_t srdia_logger_id;  //lint !e551 (static variable is used only in the DEBUG build for the debug logger)

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup sr_diagnostics
 * @{
 */

/**
 * @brief Increment the matching distribution interval for a new time in a specific distribution array.
 *
 * This function evaluates in which time interval a new time belongs and increments the matching distribution interval. T_max is splitted into 5 intervals which
 * are defined in ::srdia_diag_timing_distr_intervals. The following bins are defined:
 * - bin 1: 0 <= value <= T1
 * - bin 2: T1 < value <= T2
 * - bin 3: T2 < value <= T3
 * - bin 4: T3 < value <= T4
 * - bin 5: T4 < value <= t_max
 * .
 *
 * @implementsReq{RASW-645} Update Connection Diagnostics Function
 * @implementsReq{RASW-433} Diagnostic Timing Interval
 *
 * @param [in] distribution_array Pointer to distribution array in which the matching distribution interval is incremented. If the pointer is NULL, a
 * ::radef_kInternalError fatal error is thrown.
 * @param [in] time_value Time value to be added to diagnostic distribution. Valid range: 0 <= value <= configured t_max.
 */
static void AddTimeToTimingDistribution(uint32_t *const distribution_array, const uint32_t time_value);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

bool srdia_AreDiagnosticTimingIntervalsValid(const uint32_t t_max, const uint32_t *const diag_timing_distr_intervals) {
  // Input parameter check
  raas_AssertU32InRange(t_max, srcty_kMinTMax, srcty_kMaxTMax, radef_kInvalidParameter);
  raas_AssertNotNull(diag_timing_distr_intervals, radef_kInvalidParameter);

  bool intervals_valid = true;

  // Check if first interval is bigger then 0
  if (diag_timing_distr_intervals[0U] == 0U) {
    intervals_valid = false;
  }

  // Check ascending order of intervals T1-T4 (Last interval will be checked separately)
  for (uint16_t index = 0U; index < (RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_ARRAY_SIZE - 1U); ++index) {
    if (diag_timing_distr_intervals[index] >= diag_timing_distr_intervals[index + 1U]) {
      intervals_valid = false;
    }
  }

  // Check if last interval is smaller than t_max (intervals before are then also smaller if ascending order is respected)
  if (diag_timing_distr_intervals[RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_ARRAY_SIZE - 1U] >= t_max) {
    intervals_valid = false;
  }

  return intervals_valid;
}

void srdia_Init(const uint32_t configured_connections, const uint32_t t_max, const uint32_t n_diag_window, const uint32_t *const diag_timing_distr_intervals) {
  // Input parameter check
  raas_AssertTrue(!srdia_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange(configured_connections, srcty_kMinNumberOfRaStaConnections, RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS, radef_kInvalidParameter);
  raas_AssertU32InRange(t_max, srcty_kMinTMax, srcty_kMaxTMax, radef_kInvalidParameter);
  raas_AssertU32InRange(n_diag_window, srcty_kMinNDiagWindow, srcty_kMaxNDiagWindow, radef_kInvalidParameter);
  raas_AssertTrue(srdia_AreDiagnosticTimingIntervalsValid(t_max, diag_timing_distr_intervals), radef_kInvalidParameter);

  srdia_logger_id = ralog_INIT_LOGGER(ralog_kLogLevelNone);
  ralog_LOG_DEBUG(srdia_logger_id, "SafRetL diagnostics init ...\n");

  // Set internal parameters
  srdia_number_of_connections = configured_connections;
  srdia_t_max = t_max;
  srdia_n_diag_window = n_diag_window;
  for (uint32_t index = 0U; index < RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_ARRAY_SIZE; ++index) {
    srdia_diag_timing_distr_intervals[index] = diag_timing_distr_intervals[index];
  }

  srdia_initialized = true;

  // Init buffers for all configured connections
  for (uint32_t connection = 0U; connection < srdia_number_of_connections; ++connection) {
    srdia_InitConnectionDiagnostics(connection);
  }
}

void srdia_InitConnectionDiagnostics(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);

  ralog_LOG_DEBUG(srdia_logger_id, "Data init. Connection id: %u\n", connection_id);

  // Init diagnostic data
  srdia_connection_diagnostics[connection_id].message_counter = 0U;
  srdia_connection_diagnostics[connection_id].diagnostic_data.ec_safety = 0U;
  srdia_connection_diagnostics[connection_id].diagnostic_data.ec_address = 0U;
  srdia_connection_diagnostics[connection_id].diagnostic_data.ec_type = 0U;
  srdia_connection_diagnostics[connection_id].diagnostic_data.ec_sn = 0U;
  srdia_connection_diagnostics[connection_id].diagnostic_data.ec_csn = 0U;

  for (uint32_t index = 0U; index < RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_INTERVALS; ++index) {
    srdia_connection_diagnostics[connection_id].diagnostic_data.t_alive_distribution[index] = 0U;
    srdia_connection_diagnostics[connection_id].diagnostic_data.t_rtd_distribution[index] = 0U;
  }
}

void srdia_IncSafetyCodeErrorCounter(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);

  ++srdia_connection_diagnostics[connection_id].diagnostic_data.ec_safety;

  ralog_LOG_DEBUG(srdia_logger_id, "Connection id: %u, EC Safety: %u\n", connection_id, srdia_connection_diagnostics[connection_id].diagnostic_data.ec_safety);
}

void srdia_IncAddressErrorCounter(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);

  ++srdia_connection_diagnostics[connection_id].diagnostic_data.ec_address;

  ralog_LOG_DEBUG(srdia_logger_id, "Connection id: %u, EC Address: %u\n", connection_id,
                  srdia_connection_diagnostics[connection_id].diagnostic_data.ec_address);
}

void srdia_IncTypeErrorCounter(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);

  ++srdia_connection_diagnostics[connection_id].diagnostic_data.ec_type;

  ralog_LOG_DEBUG(srdia_logger_id, "Connection id: %u, EC Type: %u\n", connection_id, srdia_connection_diagnostics[connection_id].diagnostic_data.ec_type);
}

void srdia_IncSequenceNumberErrorCounter(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);

  ++srdia_connection_diagnostics[connection_id].diagnostic_data.ec_sn;

  ralog_LOG_DEBUG(srdia_logger_id, "Connection id: %u, EC SN: %u\n", connection_id, srdia_connection_diagnostics[connection_id].diagnostic_data.ec_sn);
}

void srdia_IncConfirmedSequenceNumberErrorCounter(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);

  ++srdia_connection_diagnostics[connection_id].diagnostic_data.ec_csn;

  ralog_LOG_DEBUG(srdia_logger_id, "Connection id: %u, EC CSN: %u\n", connection_id, srdia_connection_diagnostics[connection_id].diagnostic_data.ec_csn);
}

void srdia_UpdateConnectionDiagnostics(const uint32_t connection_id, const uint32_t round_trip_delay, const uint32_t alive_time) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);
  raas_AssertU32InRange(round_trip_delay, 0U, srdia_t_max, radef_kInvalidParameter);
  raas_AssertU32InRange(alive_time, 0U, srdia_t_max, radef_kInvalidParameter);

  // Increment distributions for t_rtd & t_alive
  AddTimeToTimingDistribution(srdia_connection_diagnostics[connection_id].diagnostic_data.t_rtd_distribution, round_trip_delay);
  AddTimeToTimingDistribution(srdia_connection_diagnostics[connection_id].diagnostic_data.t_alive_distribution, alive_time);

  ++srdia_connection_diagnostics[connection_id].message_counter;

  // Check if the diagnostic window has been exceeded to send a diagnostic notification
  if (srdia_connection_diagnostics[connection_id].message_counter >= srdia_n_diag_window) {
    ralog_LOG_DEBUG(srdia_logger_id, "Diag. window elapsed. Connection id: %u\n", connection_id);
    srdia_SendDiagnosticNotification(connection_id);  // Message counter is reset in srdia_SendDiagnosticNotification()
  }
}

void srdia_SendDiagnosticNotification(const uint32_t connection_id) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertTrue(connection_id < srdia_number_of_connections, radef_kInvalidParameter);

  srnot_SrDiagnosticNotification(connection_id, srdia_connection_diagnostics[connection_id].diagnostic_data);
  ralog_LOG_DEBUG(srdia_logger_id, "Send diag. notification. Connection id: %u\n", connection_id);

  srdia_InitConnectionDiagnostics(connection_id);  // Initialize diagnostic data of this connection after notification
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static void AddTimeToTimingDistribution(uint32_t *const distribution_array, const uint32_t time_value) {
  // Input parameter check
  raas_AssertTrue(srdia_initialized, radef_kNotInitialized);
  raas_AssertNotNull(distribution_array, radef_kInternalError);
  raas_AssertU32InRange(time_value, 0U, srdia_t_max, radef_kInternalError);

  // Check in what interval the new time corresponds
  if (time_value <= srdia_diag_timing_distr_intervals[0]) {
    ++distribution_array[0];  // Interval 1: 0 <= value <= T1
  } else if (time_value <= srdia_diag_timing_distr_intervals[1]) {
    ++distribution_array[1];  // Interval 2: T1 < value <= T2
  } else if (time_value <= srdia_diag_timing_distr_intervals[2]) {
    ++distribution_array[2];  // Interval 3: T2 < value <= T3
  } else if (time_value <= srdia_diag_timing_distr_intervals[3]) {
    ++distribution_array[3];  // Interval 4: T3 < value <= T4
  } else {
    ++distribution_array[4];  // Interval 5: T4 < value <= t_max
  }
}
