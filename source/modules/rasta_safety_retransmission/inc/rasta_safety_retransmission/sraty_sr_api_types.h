/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file sraty_sr_api_types.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_api_types
 * @{
 *
 * @brief Type definitions of RaSTA SafRetL API.
 *
 * This module defines the data structures used for the RaSTA SafRetL API interface.
 *
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-491} Enum Connection States Structure
 * @implementsReq{RASW-489} Enum Disc Reason Structure
 * @implementsReq{RASW-461} Struct Buffer Utilisation Structure
 * @implementsReq{RASW-470} Struct Connection Diagnostic Data Structure
 * @implementsReq{RASW-475} Struct Redundancy Channel Diagnostic Data Structure
 */
#ifndef SRATY_SAFETY_RETRANSMISSION_API_TYPES_H_
#define SRATY_SAFETY_RETRANSMISSION_API_TYPES_H_

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
 * @brief Enum for the state of a RaSTA connection.
 *
 * @implementsReq{RASW-491} Enum Connection States Structure
 */
//lint -esym(788, sraty_kConnectionMax) (used only for parameter range checking)
typedef enum {
  sraty_kConnectionMin = 0,             ///< Min value for connection state enum
  sraty_kConnectionNotInitialized = 0,  ///< Not initialized
  sraty_kConnectionClosed,              ///< Connection state closed
  sraty_kConnectionDown,                ///< Connection state down
  sraty_kConnectionStart,               ///< Connection state start
  sraty_kConnectionUp,                  ///< Connection state up
  sraty_kConnectionRetransRequest,      ///< Connection state retransmission request
  sraty_kConnectionRetransRunning,      ///< Connection state retransmission running
  sraty_kConnectionMax                  ///< Max value for connection state enum
} sraty_ConnectionStates;

/**
 * @brief Enum for disconnect reason.
 *
 * @implementsReq{RASW-489} Enum Disc Reason Structure
 */
//lint -esym(788, sraty_kDiscReasonMax) (used only for parameter range checking)
typedef enum {
  sraty_kDiscReasonMin = 0U,                    ///< Min value for disconnect reason enum
  sraty_kDiscReasonUserRequest = 0U,            ///< User request
  sraty_kDiscReasonNotInUse = 1U,               ///< Not in use
  sraty_kDiscReasonUnexpectedMessage = 2U,      ///< Received message type not expected for the current state
  sraty_kDiscReasonSequenceNumberError = 3U,    ///< Error in the sequence number verification during connection establishment
  sraty_kDiscReasonTimeout = 4U,                ///< Timeout for incoming messages
  sraty_kDiscReasonServiceNotAllowed = 5U,      ///< Service not allowed in this state
  sraty_kDiscReasonProtocolVersionError = 6U,   ///< Error in the protocol version
  sraty_kDiscReasonRetransmissionFailed = 7U,   ///< Retransmission failed, requested sequence number not available
  sraty_kDiscReasonProtocolSequenceError = 8U,  ///< Error in the protocol sequence
  sraty_kDiscReasonMax                          ///< Max value for disconnect reason enum
} sraty_DiscReason;

/**
 * @brief Struct for the buffer utilisation of the SafRetL buffers.
 *
 * @implementsReq{RASW-461} Struct Buffer Utilisation Structure
 */
typedef struct {
  /**
   * @brief Used send buffer entries [messages]. Valid range: 0 <= value <= ::RADEF_SEND_BUFFER_SIZE.
   *
   * @implementsReq{RASW-460} Used Send Buffer Entries
   */
  uint16_t send_buffer_used;

  /**
   * @brief Free send buffer entries [messages]. Valid range: 0 <= value <= ::RADEF_SEND_BUFFER_SIZE.
   *
   * @implementsReq{RASW-465} Free Send Buffer Entries
   */
  uint16_t send_buffer_free;

  /**
   * @brief Used receive buffer entries [messages]. Valid range: 0 <= value <= configured receive buffer size (NsendMax).
   *
   * @implementsReq{RASW-464} Used Receive Buffer Entries
   */
  uint16_t receive_buffer_used;

  /**
   * @brief Free receive buffer entries [messages]. Valid range: 0 <= value <= configured receive buffer size (NsendMax).
   *
   * @implementsReq{RASW-463} Free Receive Buffer Entries
   */
  uint16_t receive_buffer_free;
} sraty_BufferUtilisation;

/**
 * @brief Struct for the diagnostic data of a RaSTA connection.
 *
 * @implementsReq{RASW-470} Struct Connection Diagnostic Data Structure
 */
typedef struct {
  /**
   * @brief Error counter for safety code check failed [messages]. Full value range is valid and usable.
   *
   * @implementsReq{RASW-468} EC safety
   */
  uint32_t ec_safety;

  /**
   * @brief Error counter for implausible address [messages]. Full value range is valid and usable.
   *
   * @implementsReq{RASW-482} EC address
   */
  uint32_t ec_address;

  /**
   * @brief Error counter for undefined message type [messages]. Full value range is valid and usable.
   *
   * @implementsReq{RASW-481} EC type
   */
  uint32_t ec_type;

  /**
   * @brief Error counter for implausible sequence number [messages]. Full value range is valid and usable.
   *
   * @implementsReq{RASW-480} EC SN
   */
  uint32_t ec_sn;

  /**
   * @brief Error counter for implausible confirmed sequence number [messages]. Full value range is valid and usable.
   *
   * @implementsReq{RASW-479} EC CSN
   */
  uint32_t ec_csn;

  /**
   * @brief Distribution of round trip delay time [messages]. Valid range: 0 <= value <= Configured value of n_diag_window in SafRetL configuration.
   *
   * @implementsReq{RASW-478} Round Trip Delay Time Distribution
   */
  uint32_t t_rtd_distribution[RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_INTERVALS];

  /**
   * @brief Distribution of alive time [messages]. Valid range: 0 <= value <= Configured value of n_diag_window in SafRetL configuration.
   *
   * @implementsReq{RASW-477} Alive Time Distribution
   */
  uint32_t t_alive_distribution[RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_INTERVALS];
} sraty_ConnectionDiagnosticData;

/**
 * @brief Struct for the diagnostic data from a redundancy channel.
 *
 * @implementsReq{RASW-475} Struct Redundancy Channel Diagnostic Data Structure
 */
typedef struct {
  /**
   * @brief Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS.
   *
   * @implementsReq{RASW-471} Transport Channel Id
   */
  uint32_t transport_channel_id;

  /**
   * @brief Diagnosis window size [messages]. Valid range: 0 <= value <= Configured value of n_diagnosis in RedL configuration.
   *
   * @implementsReq{RASW-469} N diagnosis
   */
  uint32_t n_diagnosis;

  /**
   * @brief Nmissed [messages]. Number of messages which are not received on this transport channel within Tseq from the first reception on an other transport
   * channel. Valid range: 0 <= value <= configured value of n_diagnosis in RedL configuration.
   *
   * @implementsReq{RASW-473} N missed
   */
  uint32_t n_missed;

  /**
   * @brief Tdrift [ms]. Sum of the delays of received messages in relation to the fastest transport channel. Full value range is valid and usable.
   *
   * @implementsReq{RASW-472} T drift
   */
  uint32_t t_drift;

  /**
   * @brief Tdrift2 [ms^2]. Sum of the squares of the delays of received messages in relation to the fastest transport channel. Full value range is valid and
   * usable.
   *
   * @implementsReq{RASW-467} T drift2
   */
  uint32_t t_drift2;
} sraty_RedundancyChannelDiagnosticData;

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
#endif  // SRATY_SAFETY_RETRANSMISSION_API_TYPES_H_
