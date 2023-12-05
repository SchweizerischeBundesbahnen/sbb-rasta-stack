/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file sradno_sr_adapter_notifications.c
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 3d5012751f7f8e2a1c005ab71aeb88c68eaf7107
 *
 * @change{-,Initial version,-,-}
 * @change{SBB-RaSTA-083-SoftwareChangeRecord-001,BUG 4932: sr_adapter_notification module functions are not error tolerant,08.12.2022, N. Andres}
 *
 * @brief Implementation of RaSTA SafRetL adapter notifications.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_safety_retransmission/sradno_sr_adapter_notifications.h"

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"
#include "rasta_safety_retransmission/srnot_sr_notifications.h"
#include "srcor_sr_core.h"              // NOLINT(build/include_subdir)
#include "srrece_sr_received_buffer.h"  // NOLINT(build/include_subdir)
#include "srsend_sr_send_buffer.h"      // NOLINT(build/include_subdir)
#include "srstm_sr_state_machine.h"     // NOLINT(build/include_subdir)
#include "srtyp_sr_types.h"             // NOLINT(build/include_subdir)

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

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

radef_RaStaReturnCode sradno_MessageReceivedNotification(const uint32_t red_channel_id) {
  radef_RaStaReturnCode ret = radef_kNoError;

  const uint32_t kConnectionId = red_channel_id;  // 1:1 mapping between the connection id and redundancy channel id

  // Input parameter check
  if (kConnectionId < RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS) {
    srcor_SetReceivedMessagePendingFlag(kConnectionId);

    srtyp_ConnectionEvents connection_event = srtyp_kConnEventNone;
    bool sequence_number_in_seq = false;
    bool confirmed_time_stamp_in_seq = false;

    while (srcor_IsReceivedMsgPendingAndBuffersNotFull(kConnectionId)) {
      srcor_ReceiveMessage(kConnectionId, &connection_event, &sequence_number_in_seq, &confirmed_time_stamp_in_seq);

      if (connection_event != srtyp_kConnEventNone) {
        srstm_ProcessConnectionStateMachine(kConnectionId, connection_event, sequence_number_in_seq, confirmed_time_stamp_in_seq);
      }
    }
  } else {
    ret = radef_kInvalidParameter;
  }

  return ret;
}

radef_RaStaReturnCode sradno_DiagnosticNotification(const uint32_t red_channel_id, const uint32_t tr_channel_id,
                                                    const radef_TransportChannelDiagnosticData tr_channel_diagnostic_data) {
  radef_RaStaReturnCode ret = radef_kNoError;

  const uint32_t kConnectionId = red_channel_id;  // 1:1 mapping between the connection id and redundancy channel id

  // Input parameter check
  if (kConnectionId < RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS) {
    sraty_RedundancyChannelDiagnosticData red_channel_diag_data;

    // Copy and cluster diagnostic data
    red_channel_diag_data.transport_channel_id = tr_channel_id;
    red_channel_diag_data.n_diagnosis = tr_channel_diagnostic_data.n_diagnosis;
    red_channel_diag_data.n_missed = tr_channel_diagnostic_data.n_missed;
    red_channel_diag_data.t_drift = tr_channel_diagnostic_data.t_drift;
    red_channel_diag_data.t_drift2 = tr_channel_diagnostic_data.t_drift2;

    // Forward diagnostic data to application
    srnot_RedDiagnosticNotification(kConnectionId, red_channel_diag_data);
  } else {
    ret = radef_kInvalidParameter;
  }

  return ret;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
