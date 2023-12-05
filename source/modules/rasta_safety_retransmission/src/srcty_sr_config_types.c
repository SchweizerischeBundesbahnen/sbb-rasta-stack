/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcty_sr_config_types.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Definition of RaSTA SafRetL configuration min./max. range constants.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_safety_retransmission/srcty_sr_config_types.h"

#include <stdint.h>

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

const uint32_t srcty_kMinNumberOfRaStaConnections = 1U;

const uint16_t srcty_kMinSrLayerPayloadDataSize = 1U;

const uint32_t srcty_kMinTMax = 750U;

const uint32_t srcty_kMaxTMax = 2000U;

const uint32_t srcty_kMinTHeartbeat = 300U;

const uint32_t srcty_kMaxTHeartbeat = 750U;

const uint16_t srcty_kMinNSendMax = 2U;

const uint16_t srcty_kMinMWA = 1U;

const uint16_t srcty_kMaxMWA = 19U;

const uint32_t srcty_kNMaxPacket = 1U;

const uint32_t srcty_kMinNDiagWindow = 100U;

const uint32_t srcty_kMaxNDiagWindow = 10000U;

const uint16_t srcty_kByteCountUint16 = 2U;

const uint16_t srcty_kByteCountUint32 = 4U;

const uint16_t srcty_kByteCountUint64 = 8U;

const uint8_t srcty_kProtocolVersionMinValue = 0x30;

const uint8_t srcty_kProtocolVersionMaxValue = 0x39;

const uint32_t srcty_kMinFreeEntriesSendBufferForRetr = 3U;

const uint32_t srcty_kMinFreeEntriesReceivedBufferForReceive = 1U;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
