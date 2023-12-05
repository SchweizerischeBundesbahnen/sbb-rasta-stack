/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddfq_red_defer_queue.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer defer queue module.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "reddfq_red_defer_queue.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "rasta_redundancy/redint_red_interface.h"
#include "redmsg_red_messages.h"  // NOLINT(build/include_subdir)
#include "redtyp_red_types.h"     // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Typedef for a defer queue entry.
 */
typedef struct {
  redtyp_RedundancyMessage message;  ///< Redundancy layer PDU message
  uint32_t received_timestamp;       ///< Message received timestamp [ms]
} DeferQueueEntry;

/**
 * @brief Typedef for a defer queue.
 */
typedef struct {
  uint32_t used_defer_queue_entries;                    ///< Number of used defer queue entries [messages]
  DeferQueueEntry entries[RADEF_MAX_DEFER_QUEUE_SIZE];  ///< Array containing all defer queue entries
} DeferQueue;

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

PRIVATE bool reddfq_initialized = false;                                   ///< Initialization state of the module. True, if the module is initialized
PRIVATE uint32_t reddfq_number_of_red_channels = 0U;                       ///< Number of configured redundancy channels
PRIVATE uint32_t reddfq_defer_queue_size = 0U;                             ///< Configured defer queue size [messages]
PRIVATE uint32_t reddfq_t_seq = 0U;                                        ///< Configured defer time [ms]
PRIVATE DeferQueue reddfq_defer_queues[RADEF_MAX_NUMBER_OF_RED_CHANNELS];  ///< Defer queues for all redundancy channels

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

void reddfq_Init(const uint32_t configured_red_channels, const uint32_t configured_defer_queue_size, const uint32_t configured_t_seq) {
  // Input parameter check
  raas_AssertTrue(!reddfq_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange(configured_red_channels, redcty_kMinNumberOfRedundancyChannels, RADEF_MAX_NUMBER_OF_RED_CHANNELS, radef_kInvalidParameter);
  raas_AssertU32InRange(configured_defer_queue_size, redcty_kMinDeferQueueSize, RADEF_MAX_DEFER_QUEUE_SIZE, radef_kInvalidParameter);
  raas_AssertU32InRange(configured_t_seq, redcty_kMinTSeq, redcty_kMaxTSeq, radef_kInvalidParameter);

  // Initialize local variable
  reddfq_number_of_red_channels = configured_red_channels;
  reddfq_defer_queue_size = configured_defer_queue_size;
  reddfq_t_seq = configured_t_seq;
  reddfq_initialized = true;

  // Init buffers for all configured channels
  for (uint32_t index = 0U; index < reddfq_number_of_red_channels; ++index) {
    reddfq_InitDeferQueue(index);
  }
}

void reddfq_InitDeferQueue(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(reddfq_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddfq_number_of_red_channels - 1U, radef_kInvalidParameter);

  // Init defer queue
  reddfq_defer_queues[red_channel_id].used_defer_queue_entries = 0U;
  for (uint32_t index = 0U; index < reddfq_defer_queue_size; ++index) {
    reddfq_defer_queues[red_channel_id].entries[index].message.message_size = 0U;  // Message size = 0 means unused defer queue entry
  }
}

void reddfq_AddMessage(const uint32_t red_channel_id, const redtyp_RedundancyMessage* const redundancy_message) {
  // Input parameter check
  raas_AssertTrue(reddfq_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddfq_number_of_red_channels - 1U, radef_kInvalidParameter);
  raas_AssertNotNull(redundancy_message, radef_kInvalidParameter);
  raas_AssertU16InRange(redundancy_message->message_size, RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInvalidParameter);

  // Check if there is free space in the defer queue, else ignore the message
  if (reddfq_defer_queues[red_channel_id].used_defer_queue_entries < reddfq_defer_queue_size) {
    bool free_entry_found = false;
    uint32_t queue_index = 0U;

    // Search free defer queue entry for the new message
    while ((queue_index < reddfq_defer_queue_size) && !free_entry_found) {
      if (reddfq_defer_queues[red_channel_id].entries[queue_index].message.message_size == 0U) {
        // Free entry found
        free_entry_found = true;

        // Copy message
        DeferQueueEntry* const free_entry_in_queue = &reddfq_defer_queues[red_channel_id].entries[queue_index];
        free_entry_in_queue->message.message_size = redundancy_message->message_size;
        for (uint16_t copy_index = 0U; copy_index < free_entry_in_queue->message.message_size; ++copy_index) {
          free_entry_in_queue->message.message[copy_index] = redundancy_message->message[copy_index];
        }

        free_entry_in_queue->received_timestamp = rasys_GetTimerValue();  // Set timestamp
        ++reddfq_defer_queues[red_channel_id].used_defer_queue_entries;
      }
      ++queue_index;
    }
    raas_AssertTrue(free_entry_found, radef_kInternalError);  // Raise a fatal error, if here no free space was found in the defer queue
  }
}

void reddfq_GetMessage(const uint32_t red_channel_id, const uint32_t sequence_number, redtyp_RedundancyMessage* const redundancy_message) {
  // Input parameter check
  raas_AssertTrue(reddfq_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddfq_number_of_red_channels - 1U, radef_kInvalidParameter);
  raas_AssertNotNull(redundancy_message, radef_kInvalidParameter);

  bool message_found = false;
  uint32_t queue_index = 0U;

  // Search message sequence number
  while ((queue_index < reddfq_defer_queue_size) && !message_found) {
    if (reddfq_defer_queues[red_channel_id].entries[queue_index].message.message_size != 0U) {  // Is this defer queue entry used?
      if (redmsg_GetMessageSequenceNumber(&reddfq_defer_queues[red_channel_id].entries[queue_index].message) == sequence_number) {
        // Message found
        message_found = true;

        // Check and copy message size
        redtyp_RedundancyMessage* const message_in_queue = &reddfq_defer_queues[red_channel_id].entries[queue_index].message;
        raas_AssertU16InRange(message_in_queue->message_size, RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE, RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, radef_kInternalError);
        redundancy_message->message_size = message_in_queue->message_size;

        // Copy message
        for (uint16_t copy_index = 0U; copy_index < redundancy_message->message_size; ++copy_index) {
          redundancy_message->message[copy_index] = message_in_queue->message[copy_index];
        }

        // "Remove" message from defer queue
        message_in_queue->message_size = 0U;
        --reddfq_defer_queues[red_channel_id].used_defer_queue_entries;
      }
    }
    ++queue_index;
  }
  raas_AssertTrue(message_found, radef_kInvalidSequenceNumber);  // Raise a fatal error, if the message was not found
}

bool reddfq_IsTimeout(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(reddfq_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddfq_number_of_red_channels - 1U, radef_kInvalidParameter);

  bool is_timeout = false;
  uint32_t index = 0U;

  // Check defer queue timeout
  while ((index < reddfq_defer_queue_size) && !is_timeout) {
    if (reddfq_defer_queues[red_channel_id].entries[index].message.message_size != 0U) {  // Is this defer queue entry used?
      if ((rasys_GetTimerValue() - reddfq_defer_queues[red_channel_id].entries[index].received_timestamp) >
          reddfq_t_seq) {  // Unsigned integer wrap around allowed in this calculation
        is_timeout = true;
      }
    }
    ++index;
  }

  return is_timeout;
}

bool reddfq_Contains(const uint32_t red_channel_id, const uint32_t sequence_number) {
  // Input parameter check
  raas_AssertTrue(reddfq_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddfq_number_of_red_channels - 1U, radef_kInvalidParameter);

  bool sequence_number_found = false;
  uint32_t index = 0U;

  // Search message sequence number
  while ((index < reddfq_defer_queue_size) && !sequence_number_found) {
    if (reddfq_defer_queues[red_channel_id].entries[index].message.message_size != 0U) {  // Is this defer queue entry used?
      if (redmsg_GetMessageSequenceNumber(&reddfq_defer_queues[red_channel_id].entries[index].message) == sequence_number) {
        sequence_number_found = true;
      }
    }
    ++index;
  }

  return sequence_number_found;
}

uint32_t reddfq_GetOldestSequenceNumber(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(reddfq_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddfq_number_of_red_channels - 1U, radef_kInvalidParameter);
  raas_AssertTrue(reddfq_defer_queues[red_channel_id].used_defer_queue_entries > 0U, radef_kDeferQueueEmpty);
  raas_AssertU32InRange(reddfq_defer_queues[red_channel_id].used_defer_queue_entries, 1U, reddfq_defer_queue_size, radef_kInternalError);

  // Search first used defer queue entry and initialize oldest_sequence_number
  uint32_t search_used_index = 0U;
  while (reddfq_defer_queues[red_channel_id].entries[search_used_index].message.message_size == 0U) {
    ++search_used_index;
    raas_AssertTrue(search_used_index < reddfq_defer_queue_size, radef_kDeferQueueEmpty);
  }
  uint32_t oldest_sequence_number = redmsg_GetMessageSequenceNumber(&reddfq_defer_queues[red_channel_id].entries[search_used_index].message);

  // Search oldest message sequence number
  for (uint32_t search_oldest_index = search_used_index + 1U; search_oldest_index < reddfq_defer_queue_size; ++search_oldest_index) {
    if (reddfq_defer_queues[red_channel_id].entries[search_oldest_index].message.message_size != 0U) {  // Is this defer queue entry used?
      const uint32_t kMessageSequenceNumber = redmsg_GetMessageSequenceNumber(&reddfq_defer_queues[red_channel_id].entries[search_oldest_index].message);
      if (reddfq_IsSequenceNumberOlder(kMessageSequenceNumber, oldest_sequence_number)) {
        oldest_sequence_number = kMessageSequenceNumber;
      }
    }
  }

  return oldest_sequence_number;
}

uint32_t reddfq_GetUsedEntries(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(reddfq_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, reddfq_number_of_red_channels - 1U, radef_kInvalidParameter);

  return reddfq_defer_queues[red_channel_id].used_defer_queue_entries;
}

bool reddfq_IsSequenceNumberOlder(const uint32_t sequence_number_to_compare, const uint32_t sequence_number_reference) {
  bool is_sequence_number_older;

  if (((sequence_number_reference - (sequence_number_to_compare + 1U)) <
       (UINT32_MAX / 2U))) {  // Unsigned integer wrap around allowed in this calculation
                              // + 1U is added to get the "older" functionality and not the "older or equal"
    is_sequence_number_older = true;
  } else {
    is_sequence_number_older = false;
  }

  return is_sequence_number_older;
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------
