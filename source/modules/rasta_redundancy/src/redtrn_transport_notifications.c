/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redtrn_transport_notifications.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA transport layer notification functions.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "rasta_redundancy/redtrn_transport_notifications.h"

#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_redundancy/redtri_transport_interface.h"
#include "redcor_red_core.h"             // NOLINT(build/include_subdir)
#include "reddfq_red_defer_queue.h"      // NOLINT(build/include_subdir)
#include "redrbf_red_received_buffer.h"  // NOLINT(build/include_subdir)
#include "redstm_red_state_machine.h"    // NOLINT(build/include_subdir)

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
void redtrn_MessageReceivedNotification(const uint32_t transport_channel_id) {
  // Input parameter check
  raas_AssertU32InRange(transport_channel_id, 0U, (RADEF_MAX_NUMBER_OF_RED_CHANNELS * RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS) - 1U, radef_kInvalidParameter);

  uint32_t red_channel_id;
  redcor_GetAssociatedRedundancyChannel(transport_channel_id, &red_channel_id);

  redtyp_RedundancyMessage received_message;

  if (redstm_GetChannelState(red_channel_id) == redstm_kRedundancyChannelStateUp) {
    // Receive and process message only, if the redundancy channel is in state up
    const uint16_t kFreeReceivedBufferEntries = redrbf_GetFreeBufferEntries(red_channel_id);
    const uint32_t kUsedDeferQueueEntries = reddfq_GetUsedEntries(red_channel_id);
    if (kFreeReceivedBufferEntries > kUsedDeferQueueEntries) {
      if (radef_kNoError ==
          redtri_ReadMessage(transport_channel_id, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, &received_message.message_size, received_message.message)) {
        // Message received
        redcor_WriteReceivedMessageToInputBuffer(red_channel_id, transport_channel_id, &received_message);
        redstm_ProcessChannelStateMachine(red_channel_id, redstm_kRedundancyChannelEventReceiveData);
        // Clear input buffer message pending flag here, in case the state machine decided to not process the message
        redcor_ClearInputBufferMessagePendingFlag(red_channel_id);
      } else {
        // No more messages pending on this transport channel
        redcor_ClearMessagePendingFlag(red_channel_id, transport_channel_id);
      }
    } else {
      // Not enough space in the received buffer to read this message now
      redcor_SetMessagePendingFlag(red_channel_id, transport_channel_id);
    }
  } else {
    // The redundancy channel is not in state up -> read and discard message
    if (radef_kNoMessageReceived ==
        redtri_ReadMessage(transport_channel_id, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, &received_message.message_size, received_message.message)) {
      // No more messages pending on this transport channel
      redcor_ClearMessagePendingFlag(red_channel_id, transport_channel_id);
    }
  }
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
