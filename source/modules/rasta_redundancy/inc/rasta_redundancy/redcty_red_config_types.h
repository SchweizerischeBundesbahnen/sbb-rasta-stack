/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcty_red_config_types.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_config
 * @{
 *
 * @brief Type definitions of RaSTA redundancy layer configuration.
 *
 * This module defines the data types and data structures used for the RaSTA RedL configuration.
 */
#ifndef REDCTY_RED_CONFIG_TYPES_H_
#define REDCTY_RED_CONFIG_TYPES_H_

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
 * @brief Enum for the check code type of the redundancy channels.
 */
//lint -esym(788, redcty_kCheckCodeMax) (used only for parameter range checking)
typedef enum {
  redcty_kCheckCodeMin = 0,  ///< Min value for check code enum
  redcty_kCheckCodeA = 0,    ///< Check code type a) is used: No check code
  redcty_kCheckCodeB = 1,    ///< Check code type b) is used: CRC32 with polynomial 0xEE5B42FD
  redcty_kCheckCodeC = 2,    ///< Check code type c) is used: CRC32 with polynomial 0x1EDC6F41
  redcty_kCheckCodeD = 3,    ///< Check code type d) is used: CRC16 with polynomial 0x1021
  redcty_kCheckCodeE = 4,    ///< Check code type e) is used: CRC16 with polynomial 0x8005
  redcty_kCheckCodeMax       ///< Max value for check code enum
} redcty_CheckCodeType;

/**
 * @brief Struct for the configuration data of a redundancy channel.
 */
typedef struct {
  uint32_t red_channel_id;          ///< Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels
  uint32_t num_transport_channels;  ///< Number of used transport channels in this redundancy channel. Valid range: 0 <= value <
                                    ///< ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS
  uint32_t transport_channel_ids[RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS];  ///< IDs of the associated transport channels. Valid range: 0 <= value <
                                                                           ///< ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS * ::RADEF_MAX_NUMBER_OF_RED_CHANNELS
} redcty_RedundancyChannelConfiguration;

/**
 * @brief Struct for the configuration data of the redundancy layer.
 */
typedef struct {
  redcty_CheckCodeType check_code_type;  ///< Type of check code [enum].  All enum entries of ::redcty_CheckCodeType are valid and usable.
  uint32_t t_seq;        ///< Time for out of sequence message buffering (Tseq) [ms]. Valid range: ::redcty_kMinTSeq <= value <= ::redcty_kMaxTSeq
  uint32_t n_diagnosis;  ///< RedL diagnosis window size [messages]. Valid range: ::redcty_kMinRedLayerNDiagnosis <= value <= ::RADEF_MAX_RED_LAYER_N_DIAGNOSIS
  uint32_t n_defer_queue_size;  ///< Size of defer queue [messages]. Valid range: ::redcty_kMinDeferQueueSize <= value <= ::RADEF_MAX_DEFER_QUEUE_SIZE
  uint32_t number_of_redundancy_channels;  ///< Number of configured redundancy channels. Valid range: ::redcty_kMinNumberOfRedundancyChannels <= value <=
                                           ///< ::RADEF_MAX_NUMBER_OF_RED_CHANNELS
  redcty_RedundancyChannelConfiguration redundancy_channel_configurations[RADEF_MAX_NUMBER_OF_RED_CHANNELS];  ///< Redundancy channel configurations.
} redcty_RedundancyLayerConfiguration;

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

/**
 * @brief Minimum number of redundancy channels.
 */
extern const uint32_t redcty_kMinNumberOfRedundancyChannels;

/**
 * @brief Minimum number of transport channels.
 */
//lint -esym(9003, redcty_kMinNumberOfTransportChannels) (global constant definition grouped in one file)
extern const uint32_t redcty_kMinNumberOfTransportChannels;

/**
 * @brief Minimum time for out of sequence message buffering (Tseq) [ms].
 */
extern const uint32_t redcty_kMinTSeq;

/**
 * @brief Maximum time for out of sequence message buffering (Tseq) [ms].
 */
extern const uint32_t redcty_kMaxTSeq;

/**
 * @brief Minimum diagnosis window size [messages].
 */
//lint -esym(9003, redcty_kMinRedLayerNDiagnosis) (global constant definition grouped in one file)
extern const uint32_t redcty_kMinRedLayerNDiagnosis;

/**
 * @brief Minimum size of a redundancy channel defer queue [messages].
 */
extern const uint32_t redcty_kMinDeferQueueSize;

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
#endif  // REDCTY_RED_CONFIG_TYPES_H_
