/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file radef_rasta_definitions.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 64779fea6efa1199e3a82cbff64181dea3877e8d
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup common_definitions
 * @{
 *
 * @brief Common definitions for the RaSTA stack implementation.
 *
 * This module defines the common definitions, types and data structures used by both RaSTA layers (SafRetL & RedL).
 *
 * @implementsReq{RASW-525} Component rasta_definitions Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 */
#ifndef RADEF_RASTA_DEFINITIONS_H_
#define RADEF_RASTA_DEFINITIONS_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

/**
 * @brief  Macro for local variables which shall be accessible during unit tests.
 */
#define PRIVATE static

// -------------------- Safety and retransmission layer ------------------------
/**
 * @brief Maximum number of RaSTA connections per RaSTA network.
 */
#define RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS (2U)

/**
 * @brief Maximum payload size of a SafRetL PDU message [Bytes].
 */
#define RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE (1055U)

/**
 * @brief Header size of a SafRetL PDU message [Bytes].
 */
#define RADEF_SR_LAYER_MESSAGE_HEADER_SIZE (28U)

/**
 * @brief Application message length size of a SafRetL PDU message [Bytes].
 *
 * Embedded length of an application message inside a SafRetL PDU message, as stated in chapter 5.5.10 of "Check code" of the standard "Elektrische
 * Bahn-Signalanlagen – Teil 200: Sicheres Übertragungsprotokoll RaSTA nach DIN EN 50159 (VDE0831-159); Juni 2015".
 */
#define RADEF_SR_LAYER_APPLICATION_MESSAGE_LENGTH_SIZE (2U)

/**
 * @brief Maximum safety code size of a SafRetL PDU message [Bytes].
 */
#define RADEF_MAX_SR_LAYER_SAFETY_CODE_SIZE (16U)

/**
 * @brief Maximum size of a SafRetL PDU message (including header and safety code) [Bytes].
 */
#define RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE                                                                                     \
  (RADEF_SR_LAYER_MESSAGE_HEADER_SIZE + RADEF_SR_LAYER_APPLICATION_MESSAGE_LENGTH_SIZE + RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE + \
   RADEF_MAX_SR_LAYER_SAFETY_CODE_SIZE)  // NOLINT(whitespace/indent)

/**
 * @brief Number of received message timing distribution diagnostic intervals.
 */
#define RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_INTERVALS (5U)

/**
 * @brief Size of timing distribution diagnostic interval array. Contains one element less then intervals since last element is set to t_max.
 */
#define RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_ARRAY_SIZE (RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_INTERVALS - 1U)

/**
 * @brief Maximum number of entires in the received buffer [messages].
 */
#define RADEF_MAX_N_SEND_MAX (20U)

/**
 * @brief Defines the number of send buffer entries [messages]
 */
#define RADEF_SEND_BUFFER_SIZE (RADEF_MAX_N_SEND_MAX)

// -------------------- Redundancy layer ---------------------------------------
/**
 * @brief Maximum number of redundancy channels.
 */
#define RADEF_MAX_NUMBER_OF_RED_CHANNELS (RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS)

/**
 * @brief Maximum number of transport channels per redundancy channel.
 */
#define RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS (2U)

/**
 * @brief Header size of a RedL PDU message [Bytes].
 */
#define RADEF_RED_LAYER_MESSAGE_HEADER_SIZE (8U)

/**
 * @brief Maximum check code size of a RedL PDU message [Bytes].
 */
#define RADEF_MAX_RED_LAYER_CHECK_CODE_SIZE (4U)

/**
 * @brief Maximum size of RedL PDU message (including RedL header, max. SafRetL PDU message size and max. check code size) [Bytes].
 */
#define RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE (RADEF_RED_LAYER_MESSAGE_HEADER_SIZE + RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE + RADEF_MAX_RED_LAYER_CHECK_CODE_SIZE)

/**
 * @brief Minimum size of RedL PDU message (including RedL header, min. SafRetL PDU message size (only SafRetL header) and min. check code size (none)) [Bytes].
 */
#define RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE (RADEF_RED_LAYER_MESSAGE_HEADER_SIZE + RADEF_SR_LAYER_MESSAGE_HEADER_SIZE)

/**
 * @brief Maximum size of a redundancy channel defer queue [messages].
 */
#define RADEF_MAX_DEFER_QUEUE_SIZE (10U)

/**
 * @brief Maximum RedL diagnosis window size (Ndiagnosis) [messages].
 */
#define RADEF_MAX_RED_LAYER_N_DIAGNOSIS (1000U)

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Enum for function return codes of the RaSTA stack.
 *
 * @implementsReq{RASW-483} Enum RaSta Return Code Structure
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 */
//lint -esym(788, radef_kMax) (used only for parameter range checking)
typedef enum {
  radef_kMin = 0,                              ///< Min value for RaSTA return code enum
  radef_kNoError = 0,                          ///< No error
  radef_kNoMessageReceived = 1,                ///< No message received
  radef_kNoMessageToSend = 2,                  ///< No message to send
  radef_kNotInitialized = 3,                   ///< Not initialized
  radef_kAlreadyInitialized = 4,               ///< Already initialized
  radef_kInvalidConfiguration = 5,             ///< Invalid configuration
  radef_kInvalidParameter = 6,                 ///< Invalid parameter
  radef_kInvalidMessageType = 7,               ///< Invalid message type
  radef_kInvalidMessageSize = 8,               ///< Invalid message size
  radef_kInvalidBufferSize = 9,                ///< Invalid buffer size
  radef_kInvalidMessageCrc = 10,               ///< Invalid message crc
  radef_kInvalidMessageMd4 = 11,               ///< Invalid message MD4
  radef_kReceiveBufferFull = 12,               ///< Receive buffer full
  radef_kDeferQueueEmpty = 13,                 ///< Defer queue empty
  radef_kSendBufferFull = 14,                  ///< Send buffer full
  radef_kInvalidSequenceNumber = 15,           ///< Invalid sequence number
  radef_kInternalError = 16,                   ///< Internal error
  radef_kInvalidOperationInCurrentState = 17,  ///< Invalid operation in the current state
  radef_kMax                                   ///< Max value for RaSTA return code enum
} radef_RaStaReturnCode;

/**
 * @brief Struct for the diagnostic data from a transport channel.
 *
 * This structure is defined in the common part because it is used by both layers. The RedL passes its diagnostic data to the SafRetL using this structure.
 *
 * @implementsReq{RASW-474} Struct Transport Channel Diagnostic Data Structure
 */
typedef struct {
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
} radef_TransportChannelDiagnosticData;

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
#endif  // RADEF_RASTA_DEFINITIONS_H_
