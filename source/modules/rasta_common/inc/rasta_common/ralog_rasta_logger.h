/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file ralog_rasta_logger.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup common_logger
 * @{
 *
 * @brief Interface of the RaSTA debug logger.
 *
 * This module provides different logging utilities to log debug information on different log levels. The logger output is written to stdout by printf.
 * The logger module can be enabled for the DEBUG build by defining: ralog_ENABLE_LOGGER 1
 * The logger module must be disabled for the RELEASE build by defining: ralog_ENABLE_LOGGER 0
 * This removes the logger code completely form the RELEASE build by the use of macros.
 *
 * @implementsReq{RASW-540} Component rasta_logger Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 */
#ifndef RALOG_RASTA_LOGGER_H_
#define RALOG_RASTA_LOGGER_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------

#if (defined(DEBUG) && !defined(UNIT_TEST))
//lint -save -e829 (stdarg.h is used only in the DEBUG build)
#include <stdarg.h>
//lint -restore
#endif
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

#if (defined(DEBUG) && !defined(UNIT_TEST))
/**
 * @brief Global logger enable.
 *
 * ralog_ENABLE_LOGGER 1 -> logger enabled, used for DEBUG build
 */
#define ralog_ENABLE_LOGGER 1
#else
/**
 * @brief Global logger disable.
 *
 * ralog_ENABLE_LOGGER 0 -> logger disabled, used for RELEASE build, no logger code is generated
 */
#define ralog_ENABLE_LOGGER 0
#endif

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

#if ralog_ENABLE_LOGGER != 0  // logger enabled

/**
 * @brief Enum for level of the logger.
 */
//lint -esym(788, ralog_kLogLevelMax) (used only for parameter range checking)
typedef enum {
  ralog_kLogLevelMin = 0,    ///< min. log level value for range checks
  ralog_kLogLevelNone = 0,   ///< log messages disabled
  ralog_kLogLevelError = 1,  ///< log level error
  ralog_kLogLevelWarn = 2,   ///< log level warning
  ralog_kLogLevelInfo = 3,   ///< log level info
  ralog_kLogLevelDebug = 4,  ///< log level debug
  ralog_kLogLevelMax         ///< max. log level value for range checks
} ralog_LogLevel;

#endif  // logger enabled

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#if ralog_ENABLE_LOGGER != 0  // logger enabled

/**
 * @brief Initialize and configure the logger of a c source file.
 *
 * Don't call this function directly! Use the wrapper macro ralog_INIT_LOGGER(), which allows to completely remove the logger code in the RELEASE build.
 *
 * A new logger instance for a C source file is created and the logger identification of this logger is returned.
 * A fatal error with the error reason ::radef_kInternalError is raised, if the number of used loggers is MAX_NUMBER_OF_LOGGERS or greater at the start of the
 * function. The configured log level of this logger instance is stored internally. At the very first call of ralog_InitLogger() the logger timestamp is
 * initialized to 0.
 *
 * @param [in] log_level log level for this message. All enum entries of ::ralog_LogLevel are valid, for other values a fatal error with the error reason
 * ::radef_kInvalidParameter is raised.
 * @return uint16_t identification of the new logger instance
 */
uint16_t ralog_InitLogger(const ralog_LogLevel log_level);  //lint !e759 (API function to be called externally)

/**
 * @brief Log a message on a specific log level to stdout.
 *
 * Don't call this function directly! Use the wrapper macro ralog_LOG_...(), which allows to completely remove the logger code in the RELEASE build.
 *
 * The log message is generated only if the log_level is smaller or equal to the log level configured for this instance of the logger.
 * The log message consists of a timestamp [ms], log level, the calling file name, the calling function name, the source code line number and the printf
 * formatted message. The timestamp is updated by getting rasys_GetTimerValue() before creating the log message.
 *
 * @pre The logger instance with the corresponding logger ID must be initialized before, otherwise a fatal error with the error reason ::radef_kNotInitialized
 * is raised.
 *
 * @param [in] logger_id identification of the logger. The full value range is valid and usable.
 * @param [in] log_level log level for this message. All enum entries of ::ralog_LogLevel are valid, for other values a fatal error with the error reason
 * ::radef_kInvalidParameter is raised.
 * @param [in] file_name name of the source file calling ralog_LogMessage()
 * @param [in] function_name name of the function calling ralog_LogMessage()
 * @param [in] line_number line number of ralog_LogMessage() function call
 * @param [in] message message to log as printf format string
 * @param [in] ... additional parameters used in printf format string
 */
//lint -save -e759 (API function to be called externally)
void ralog_LogMessage(const uint16_t logger_id, const ralog_LogLevel log_level, const char* file_name, const char* function_name, const uint32_t line_number,
                      const char* const message, ...);
//lint -restore

/**
 * @brief Initialize and configure a logger of a c source file.
 *
 * A new logger instance for a C source file is created and the logger identification of this logger is returned.
 * A fatal error with the error reason ::radef_kInternalError is raised, if the number of used loggers is MAX_NUMBER_OF_LOGGERS or greater at the start of the
 * function. The configured log level of this logger instance is stored internally. At the very first call of ralog_INIT_LOGGER() the logger timestamp is
 * initialized to 0.
 *
 * @param [in] log_level log level for this message. All enum entries of ::ralog_LogLevel are valid, for other values a fatal error with the error reason
 * ::radef_kInvalidParameter is raised.
 * @return uint16_t identification of the new logger instance
 */
#define ralog_INIT_LOGGER(log_level) \
  ralog_InitLogger(log_level)  //lint !e9026 (function like macro is used to completely remove the logger code in the RELEASE build)

/**
 * @brief Macro to get the file name without the path (for windows systems).
 *
 * For Linux/Unix systems change to: ralog_FILENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
 */
//lint -emacro(613, ralog_FILENAME) -emacro(9016, ralog_FILENAME) -emacro(9034, ralog_FILENAME) -emacro(9050, ralog_FILENAME)
// (code is used only in the DEBUG build)
#define ralog_FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

/**
 * @brief Log an message as error to stdout.
 *
 * The log message is generated only if the log level configured for this instance of the logger is ::ralog_kLogLevelError or higher.
 * The log message consists of a timestamp [ms], log level, the calling file name, the calling function name, the source code line number and the printf
 * formatted message. The timestamp is updated by getting rasys_GetTimerValue() before creating the log message.
 *
 * @pre The logger instance with the corresponding logger ID must be initialized before, otherwise a fatal error with the error reason ::radef_kNotInitialized
 * is raised.
 *
 * @param [in] logger_id identification of the logger. The full value range is valid and usable.
 * @param [in] message message to log as printf format string
 * @param [in] ... additional parameters used in printf format string
 */
//lint -save -e9024 -e9026 (function like macro is used to completely remove the logger code in the RELEASE build, code is used only in the DEBUG build)
#define ralog_LOG_ERROR(logger_id, message, ...) ralog_LogMessage(logger_id, ralog_kLogLevelError, ralog_FILENAME, __func__, __LINE__, message, ##__VA_ARGS__)
//lint -restore

/**
 * @brief Log an message as warning to stdout.
 *
 * The log message is generated only if the log level configured for this instance of the logger is ::ralog_kLogLevelWarn or higher.
 * The log message consists of a timestamp [ms], log level, the calling file name, the calling function name, the source code line number and the printf
 * formatted message. The timestamp is updated by getting rasys_GetTimerValue() before creating the log message.
 *
 * @pre The logger instance with the corresponding logger ID must be initialized before, otherwise a fatal error with the error reason ::radef_kNotInitialized
 * is raised.
 *
 * @param [in] logger_id identification of the logger. The full value range is valid and usable.
 * @param [in] message message to log as printf format string
 * @param [in] ... additional parameters used in printf format string
 */
//lint -save -e9024 -e9026 (function like macro is used to completely remove the logger code in the RELEASE build, code is used only in the DEBUG build)
#define ralog_LOG_WARN(logger_id, message, ...) ralog_LogMessage(logger_id, ralog_kLogLevelWarn, ralog_FILENAME, __func__, __LINE__, message, ##__VA_ARGS__)
//lint -restore

/**
 * @brief Log a message as info to stdout.
 *
 * The log message is generated only if the log level configured for this instance of the logger is ::ralog_kLogLevelInfo or higher.
 * The log message consists of a timestamp [ms], log level, the calling file name, the calling function name, the source code line number and the printf
 * formatted message. The timestamp is updated by getting rasys_GetTimerValue() before creating the log message.
 *
 * @pre The logger instance with the corresponding logger ID must be initialized before, otherwise a fatal error with the error reason ::radef_kNotInitialized
 * is raised.
 *
 * @param [in] logger_id identification of the logger. The full value range is valid and usable.
 * @param [in] message message to log as printf format string
 * @param [in] ... additional parameters used in printf format string
 */
//lint -save -e9024 -e9026 (function like macro is used to completely remove the logger code in the RELEASE build, code is used only in the DEBUG build)
#define ralog_LOG_INFO(logger_id, message, ...) ralog_LogMessage(logger_id, ralog_kLogLevelInfo, ralog_FILENAME, __func__, __LINE__, message, ##__VA_ARGS__)
//lint -restore

/**
 * @brief Log a message as debug to stdout.
 *
 * The log message is generated only if the log level configured for this instance of the logger is ::ralog_kLogLevelDebug.
 * The log message consists of a timestamp [ms], log level, the calling file name, the calling function name, the source code line number and the printf
 * formatted message. The timestamp is updated by getting rasys_GetTimerValue() before creating the log message.
 *
 * @pre The logger instance with the corresponding logger ID must be initialized before, otherwise a fatal error with the error reason ::radef_kNotInitialized
 * is raised.
 *
 * @param [in] logger_id identification of the logger. The full value range is valid and usable.
 * @param [in] message message to log as printf format string
 * @param [in] ... additional parameters used in printf format string
 */
//lint -save -e9024 -e9026 (function like macro is used to completely remove the logger code in the RELEASE build, code is used only in the DEBUG build)
#define ralog_LOG_DEBUG(logger_id, message, ...) ralog_LogMessage(logger_id, ralog_kLogLevelDebug, ralog_FILENAME, __func__, __LINE__, message, ##__VA_ARGS__)
//lint -restore

#else  // logger disabled

/**
 * @brief Empty placeholder macro for ralog_INIT_LOGGER() used for RELEASE build.
 */
#define ralog_INIT_LOGGER(log_level) 0U

/**
 * @brief Empty placeholder macro for ralog_LOG_ERROR() used for RELEASE build.
 */
#define ralog_LOG_ERROR(logger_id, message, ...)

/**
 * @brief Empty placeholder macro for ralog_LOG_WARN() used for RELEASE build.
 */
#define ralog_LOG_WARN(logger_id, message, ...)

/**
 * @brief Empty placeholder macro for ralog_LOG_INFO() used for RELEASE build.
 */
#define ralog_LOG_INFO(logger_id, message, ...)

/**
 * @brief Empty placeholder macro for ralog_LOG_DEBUG() used for RELEASE build.
 */
#define ralog_LOG_DEBUG(logger_id, message, ...)

#endif  // end of logger disabled

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // RALOG_RASTA_LOGGER_H_
