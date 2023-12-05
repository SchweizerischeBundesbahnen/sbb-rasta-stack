/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcty_sr_config_types.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_config
 * @{
 *
 * @brief Type definitions of RaSTA SafRetL configuration.
 *
 * This module defines the data types and data structures used for the RaSTA SafRetL configuration.
 *
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-487} Enum Safety Code Type Structure
 * @implementsReq{RASW-423} Struct Connection Configuration Structure
 * @implementsReq{RASW-437} Struct MD4 Initial Value Structure
 * @implementsReq{RASW-427} Struct SafetyRetransmissionConfiguration Structure
 */
#ifndef SRCTY_SAFETY_RETRANSMISSION_CONFIG_TYPES_H_
#define SRCTY_SAFETY_RETRANSMISSION_CONFIG_TYPES_H_

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
 * @brief Enum for the safety code type.
 *
 * @implementsReq{RASW-487} Enum Safety Code Type Structure
 */
//lint -esym(788, srcty_kSafetyCodeTypeMax) (used only for parameter range checking)
typedef enum {
  srcty_kSafetyCodeTypeMin = 1,       ///< Min value for safety code enum
  srcty_kSafetyCodeTypeNone = 1,      ///< Safety code type 1: No safety code
  srcty_kSafetyCodeTypeLowerMd4 = 2,  ///< Safety code type 2: Lower half of MD4
  srcty_kSafetyCodeTypeFullMd4 = 3,   ///< Safety code type 3: Full MD4
  srcty_kSafetyCodeTypeMax            ///< Max value for safety code enum
} srcty_SafetyCodeType;

/**
 * @brief Struct for the configuration of a RaSTA connection.
 *
 * This struct contains the configuration of a RaSTA connection, describing the relation between sender id, receiver id & connection id. Sender id and receiver
 * id must be different from each other.
 *
 * @implementsReq{RASW-423} Struct Connection Configuration Structure
 */
typedef struct {
  /**
   * @brief Id of the connection
   *
   * Valid range: 0 <= value < srcty_SafetyRetransmissionConfiguration::number_of_connections
   *
   * @implementsReq{RASW-426} Connection Id
   */
  uint32_t connection_id;

  /**
   * @brief Id of the sender
   *
   * The full value range is valid and usable.
   *
   * @implementsReq{RASW-425} Sender Id
   */
  uint32_t sender_id;

  /**
   * @brief Id of the receiver
   *
   * The full value range is valid and usable.
   *
   * @implementsReq{RASW-435} Receiver Id
   */
  uint32_t receiver_id;
} srcty_ConnectionConfiguration;

/**
 * @brief Struct for the MD4 initial value for a RaSTA network.
 *
 * This struct contains the 4 inital values for the MD4 safety code. For every single initial value A-D, full value range is valid and usable.
 *
 * @implementsReq{RASW-437} Struct MD4 Initial Value Structure
 * @implementsReq{RASW-432} Init Value A, B, C, D
 */
typedef struct {
  uint32_t init_a;  ///< Initial value A. Full value range is valid and usable.
  uint32_t init_b;  ///< Initial value B. Full value range is valid and usable.
  uint32_t init_c;  ///< Initial value C. Full value range is valid and usable.
  uint32_t init_d;  ///< Initial value D. Full value range is valid and usable.
} srcty_Md4InitValue;

/**
 * @brief Struct for the configuration data of the SafRetL.
 *
 * @implementsReq{RASW-427} Struct SafetyRetransmissionConfiguration Structure
 */
typedef struct {
  /**
   * @brief RaSTA network id
   *
   * Represent a unique identification which represents the RaSTA network. It is not used by the SW, since only one RaSTA network is supported. The full value
   * range is valid and usable.
   *
   * @implementsReq{RASW-446} RaSTA Network Id
   */
  uint32_t rasta_network_id;

  /**
   * @brief Max. accepted age of a message [ms]
   *
   * Maximum accepted age of a message before a message timeout is triggered. Valid range of ::srcty_kMinTMax <= value <= ::srcty_kMaxTMax.
   *
   * @implementsReq{RASW-445} T max
   */
  uint32_t t_max;

  /**
   * @brief Heartbeat period [ms]
   *
   * When the heartbeat period passes without sending any new message, a new heartbeat message must be send so indicate the opposite side that the connection is
   * still alive. Valid range of ::srcty_kMinTHeartbeat <= value <= ::srcty_kMaxTHeartbeat.
   *
   * @implementsReq{RASW-444} T h
   */
  uint32_t t_h;

  /**
   * @brief Type of safety code [enum]
   *
   * Defines the used safety code type of the SafRetL. Valid range: ::srcty_kSafetyCodeTypeMin <= value < ::srcty_kSafetyCodeTypeMax
   *
   * @implementsReq{RASW-443} Safety Code Type
   */
  srcty_SafetyCodeType safety_code_type;

  /**
   * @brief Maximum number of received, unconfirmed messages [messages]
   *
   * Defines the maximum number of received and unconfirmed messages that are allowed. Valid range of ::srcty_kMinMWA <= value <= ::srcty_kMaxMWA.
   *
   * @implementsReq{RASW-442} MWA
   */
  uint16_t m_w_a;

  /**
   * @brief Maximum number of send messages without receiving a confirmation [messages]
   *
   * Defines the maximum number of messages which the communication party may send without confirmation. Valid range of ::srcty_kMinNSendMax <= value <=
   * ::RADEF_MAX_N_SEND_MAX.
   *
   * @implementsReq{RASW-441} N sendmax
   */
  uint16_t n_send_max;

  /**
   * @brief Packetization factor
   *
   * The maximal packetization factor defines how many messages from one application may at maximum be combined to form aSafRetL message. Valid range: value =
   * ::srcty_kNMaxPacket.
   *
   * @implementsReq{RASW-440} N maxPacket
   */
  uint32_t n_max_packet;

  /**
   * @brief Diagnosis window size [messages]
   *
   * Defines the size of the measurement window for the channel quality measurements. Valid range of ::srcty_kMinNDiagWindow <= value <=
   * ::srcty_kMaxNDiagWindow.
   *
   * @implementsReq{RASW-438} N diagWindow
   */
  uint32_t n_diag_window;

  /**
   * @brief Number of configured connections
   *
   * Defines the number of configured connections that are allowed. Valid range of ::srcty_kMinNumberOfRaStaConnections <= value <=
   * ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
   *
   * @implementsReq{RASW-436} Number of Connections
   */
  uint32_t number_of_connections;

  /**
   * @brief Configurations of connections
   *
   * This array of structs contains the configuration of the connections used in the SafRetL. The srcty_ConnectionConfiguration::connection_id must be in
   * ascending order starting from 0 for the entires in the array. All ranges for the individual structs are described in ::srcty_ConnectionConfiguration.
   *
   * @implementsReq{RASW-431} Connection Configurations
   */
  srcty_ConnectionConfiguration connection_configurations[RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS];

  /**
   * @brief MD4 initial value
   *
   * This struct contains the initial values for the MD4 safety code. All ranges for the individual initial values are described in ::srcty_Md4InitValue.
   *
   * @implementsReq{RASW-428} MD4 Initial Value
   */
  srcty_Md4InitValue md4_initial_value;

  /**
   * @brief Intervals for timing diagnostics [ms]
   *
   * This array defines the diagnostic timing intervals. It contains ::RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_ARRAY_SIZE timings which split the maximum
   * time T_max in ::RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_INTERVALS time intervals.
   * - each timing describes its relative starting time from 0
   * - timings must be arranged in ascending order
   * - 0 < T1 < T2 < T3 < T4 < t_max
   * .
   * Intervals are defined as follows:
   * - interval 1: 0 <= value <= T1
   * - interval 2: T1 < value <= T2
   * - interval 3: T2 < value <= T3
   * - interval 4: T3 < value <= T4
   * - interval 5: T4 < value <= t_max
   * .
   * Valid range for every element: 0 < value < srcty_SafetyRetransmissionConfiguration::t_max.
   *
   * @implementsReq{RASW-433} Diagnostic Timing Interval
   */
  uint32_t diag_timing_distr_intervals[RADEF_DIAGNOSTIC_TIMING_DISTRIBUTION_ARRAY_SIZE];
} srcty_SafetyRetransmissionConfiguration;

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Minimum number of RaSTA connections per RaSTA network.
 */
extern const uint32_t srcty_kMinNumberOfRaStaConnections;

/**
 * @brief Minimum SafRetL payload data size.
 */
extern const uint16_t srcty_kMinSrLayerPayloadDataSize;

/**
 * @brief Minimum max. accepted age of a message (Tmax) [ms].
 */
//lint -esym(9003, srcty_kMinTMax) (global constant definition grouped in one file)
extern const uint32_t srcty_kMinTMax;

/**
 * @brief Maximum max. accepted age of a message (Tmax) [ms].
 */
//lint -esym(9003, srcty_kMaxTMax) (global constant definition grouped in one file)
extern const uint32_t srcty_kMaxTMax;

/**
 * @brief Minimum heartbeat period (Th) [ms].
 */
//lint -esym(9003, srcty_kMinTHeartbeat) (global constant definition grouped in one file)
extern const uint32_t srcty_kMinTHeartbeat;

/**
 * @brief Maximum heartbeat period (Th) [ms].
 */
//lint -esym(9003, srcty_kMaxTHeartbeat) (global constant definition grouped in one file)
extern const uint32_t srcty_kMaxTHeartbeat;

/**
 * @brief Minimum receive buffer size (Nsendmax) [messages].
 */
extern const uint16_t srcty_kMinNSendMax;

/**
 * @brief Minimum max. number of received, unconfirmed messages (MWA) [messages].
 */
//lint -esym(9003, srcty_kMinMWA) (global constant definition grouped in one file)
extern const uint16_t srcty_kMinMWA;

/**
 * @brief Maximum max. number of received, unconfirmed messages (MWA) [messages].
 */
//lint -esym(9003, srcty_kMaxMWA) (global constant definition grouped in one file)
extern const uint16_t srcty_kMaxMWA;

/**
 * @brief Packetization factor (must always be 1!).
 */
//lint -esym(9003, srcty_kNMaxPacket) (global constant definition grouped in one file)
extern const uint32_t srcty_kNMaxPacket;

/**
 * @brief Minimum SafRetL diagnosis window size
 * (Ndiagwindow) [messages].
 */
//lint -esym(9003, srcty_kMinNDiagWindow) (global constant definition grouped in one file)
extern const uint32_t srcty_kMinNDiagWindow;

/**
 * @brief Maximum SafRetL diagnosis window size
 * (Ndiagwindow) [messages].
 */
//lint -esym(9003, srcty_kMaxNDiagWindow) (global constant definition grouped in one file)
extern const uint32_t srcty_kMaxNDiagWindow;

/**
 * @brief Byte count of type UInt16_t [bytes]
 *
 */
extern const uint16_t srcty_kByteCountUint16;

/**
 * @brief Byte count of type UInt32_t [bytes]
 *
 */
extern const uint16_t srcty_kByteCountUint32;

/**
 * @brief Byte count of type UInt64_t [bytes]
 *
 */
//lint -esym(9003, srcty_kByteCountUint64) (global constant definition grouped in one file)
extern const uint16_t srcty_kByteCountUint64;

/**
 * @brief Minimum ASCII character value for protocol verion
 *
 */
extern const uint8_t srcty_kProtocolVersionMinValue;

/**
 * @brief Maximum ASCII character value for protocol verion
 *
 */
extern const uint8_t srcty_kProtocolVersionMaxValue;

/**
 * @brief Minimum amount of free entries in the send buffer in case of a retransmission
 *
 * In worst case a retransmission of retransmission adds a RetrResp, HB & RetrReq to the send buffer, so there need to be at least 3 free entries to start a
 * retransmission.
 */
//lint -esym(9003, srcty_kMinFreeEntriesSendBufferForRetr) (global constant definition grouped in one file)
extern const uint32_t srcty_kMinFreeEntriesSendBufferForRetr;

/**
 * @brief Minimum amount of free entries in the received buffer in case of receiving a message
 *
 * One free entry in the received buffer is needed to receive a new message.
 */
//lint -esym(9003, srcty_kMinFreeEntriesReceivedBufferForReceive) (global constant definition grouped in one file)
extern const uint32_t srcty_kMinFreeEntriesReceivedBufferForReceive;

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
#endif  // SRCTY_SAFETY_RETRANSMISSION_CONFIG_TYPES_H_
