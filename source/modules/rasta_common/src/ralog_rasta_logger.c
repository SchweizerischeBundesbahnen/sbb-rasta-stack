/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file ralog_rasta_logger.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA debug logger.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_common/ralog_rasta_logger.h"

#if ralog_ENABLE_LOGGER != 0  // logger enabled

#include <inttypes.h>
//lint -save -e829 (stdarg.h is used only in the DEBUG build)
#include <stdarg.h>
//lint -restore
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/rasys_rasta_system_adapter.h"

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------
/**
 * @brief Maximum number of logger instances.
 *
 * Normally one logger instance is used per C file.
 */
#define MAX_NUMBER_OF_LOGGERS (50U)

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------
/**
 * @brief Struct with the data of one logger instance.
 */
typedef struct {
  ralog_LogLevel log_level;  ///< log level of this logger instance
} LoggerData;

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
/**
 * @brief Array with the data of all loggers
 */
static LoggerData loggers[MAX_NUMBER_OF_LOGGERS];

/**
 * @brief Number of used loggers
 */
static uint16_t number_of_used_loggers = 0U;

/**
 * @brief Initialization state of the module.
 *
 * True, if the module is initialized.
 */
static bool initialized = false;

/**
 * @brief Logger timestamp [ms]
 */
static uint64_t timestamp = 0U;

/**
 * @brief Last timer value [ms]
 */
static uint32_t last_timer_value = 0U;

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------
/**
 * @brief Update the logger timestamp.
 *
 * Reads rasys_GetTimerValue() and calculates the difference to the last call to increment the logger timestamp.
 */
static void UpdateTimestamp(void);

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

uint16_t ralog_InitLogger(const ralog_LogLevel log_level) {  //lint !e714 !e765 (API function to be called externally)
  raas_AssertTrue(number_of_used_loggers < MAX_NUMBER_OF_LOGGERS, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)log_level, (uint16_t)ralog_kLogLevelMin, ((uint16_t)ralog_kLogLevelMax) - 1U, radef_kInvalidParameter);

  if (!initialized) {                          // check for 1st InitLogger() call in the whole build
    last_timer_value = rasys_GetTimerValue();  // initialize timestamp time base
    initialized = true;
  }

  loggers[number_of_used_loggers].log_level = log_level;  // save the log level
  ++number_of_used_loggers;

  return number_of_used_loggers - 1U;  // return new logger ID
}

//lint -save -e714 -e765 (API function to be called externally)
void ralog_LogMessage(const uint16_t logger_id, const ralog_LogLevel log_level, const char* file_name, const char* function_name, const uint32_t line_number,
                      const char* const message, ...) {
  //lint -restore
  raas_AssertTrue(initialized, radef_kNotInitialized);
  raas_AssertTrue(logger_id < number_of_used_loggers, radef_kNotInitialized);
  raas_AssertU16InRange((uint16_t)log_level, (uint16_t)ralog_kLogLevelMin, ((uint16_t)ralog_kLogLevelMax) - 1U, radef_kInvalidParameter);

  UpdateTimestamp();

  if ((log_level <= loggers[logger_id].log_level) && (ralog_kLogLevelNone != log_level)) {
    printf("%" PRIu64 ":", timestamp);  //lint !e534 !e586 !e816 (code is used only in the DEBUG build, PRIu64 macro is expanded form gcc standard library)

    switch (log_level) {
      case ralog_kLogLevelError:
        printf("ERROR:");  //lint !e534 !e586 (code is used only in the DEBUG build)
        break;
      case ralog_kLogLevelWarn:
        printf("WARN:");  //lint !e534 !e586 (code is used only in the DEBUG build)
        break;
      case ralog_kLogLevelInfo:
        printf("INFO:");  //lint !e534 !e586 (code is used only in the DEBUG build)
        break;
      case ralog_kLogLevelDebug:
        printf("DEBUG:");  //lint !e534 !e586 (code is used only in the DEBUG build)
        break;
      case ralog_kLogLevelNone:
        // nothing to log here
        break;
      default:
        // not possible due to previous parameter range check
        break;
    }
    printf("%s:", file_name);        //lint !e534 !e586 (code is used only in the DEBUG build)
    printf("%s():", function_name);  //lint !e534 !e586 (code is used only in the DEBUG build)
    printf("%u:", line_number);      //lint !e534 !e586 (code is used only in the DEBUG build)
    va_list args;                    //lint !e586 (code is used only in the DEBUG build)
    va_start(args, message);         //lint !e586 (code is used only in the DEBUG build)
    vprintf(message, args);          //lint !e534 !e586 (code is used only in the DEBUG build)
    va_end(args);                    //lint !e586 (code is used only in the DEBUG build)
  }
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
static void UpdateTimestamp() {
  uint32_t actual_timer_value = rasys_GetTimerValue();                   // actual 32 bit timer value [ms]
  uint32_t timestamp_increment = actual_timer_value - last_timer_value;  // calculate timestamp increment [ms] with correct 32 bit overflow
  timestamp += (uint64_t)timestamp_increment;
  last_timer_value = actual_timer_value;
}

#endif  // end of logger enabled
