/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srcfg_sr_config.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version ba293ba6efefd175e9347cbdc8b80d3f122e3f0d
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Definition of RaSTA SafRetL configuration.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_safety_retransmission_config/srcfg_sr_config.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rasta_safety_retransmission/srcty_sr_config_types.h"

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
const srcty_SafetyRetransmissionConfiguration safety_retransmission_configuration = {
    .rasta_network_id = 123456U,
    .t_max = 750U,
    .t_h = 300U,
    .safety_code_type = srcty_kSafetyCodeTypeLowerMd4,
    .m_w_a = 10U,
    .n_send_max = 20U,
    .n_max_packet = 1U,
    .n_diag_window = 5000U,
    .number_of_connections = 2U,
    {
        {
            .connection_id = 0U,
            .sender_id = 0x61U,
            .receiver_id = 0x62U,
        },
        {
            .connection_id = 1U,
            .sender_id = 1U,
            .receiver_id = 3U,
        },
    },
    {
        .init_a = 0x67452301U,
        .init_b = 0xEFCDAB89U,
        .init_c = 0x98BADCFEU,
        .init_d = 0x10325476U,
    },  // MD4 initial value
    {
        150U,
        300U,
        450U,
        600U,
    }  // intervals for timing diagnostics
};

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
