/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file reddfq_red_defer_queue.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_deferQueue
 * @{
 *
 * @brief Interface of RaSTA redundancy layer defer queue module.
 *
 * This module provides the RaSTA RedL defer queue functionality. It is used to store out of sequence received messages in the defer queue. If the message with
 * the missing sequence number is received or the Tseq timeout is reached, the core module delivers the messages form the defer queue to the received buffer.
 * This module provides the following functionality:
 * - Initialize defer queue of a redundancy channel
 * - Add a message to a defer queue
 * - Get a message from a defer queue
 * - Check defer queue timeout
 * - Check if the defer queue contains a message with a defined sequence number
 * - Get the minimum sequence number in the defer queue
 * - Get the number of used defer queue entries
 * .
 */
#ifndef REDDFQ_RED_DEFER_QUEUE_H_
#define REDDFQ_RED_DEFER_QUEUE_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Const Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Prototypes
// -----------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Initialization of the data of the RedL defer queue module.
 *
 * This function is used to initialize the defer queue module. It saves the passed number of redundancy channels, the defer queue size and defer queue timeout
 * time Tseq. For all configured channels, the ::reddfq_InitDeferQueue() function is called to properly initialize the defer queues for all configured channels.
 * A fatal error is raised, if this function is called multiple times.
 *
 * @pre The defer queue module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @param [in] configured_red_channels Number of configured redundancy channels. Valid range: 1 <= value <= ::RADEF_MAX_NUMBER_OF_RED_CHANNELS.
 * @param [in] configured_defer_queue_size Configured defer queue size [messages]. Valid range: ::redcty_kMinDeferQueueSize <= value <=
 * ::RADEF_MAX_DEFER_QUEUE_SIZE
 * @param [in] configured_t_seq Configured Tseq [ms]. Valid range: ::redcty_kMinTSeq <= value <= ::redcty_kMaxTSeq.
 */
void reddfq_Init(const uint32_t configured_red_channels, const uint32_t configured_defer_queue_size, const uint32_t configured_t_seq);

/**
 * @brief Initialization of the defer queue of a dedicated redundancy channel.
 *
 * This function initializes the defer queue of a given redundancy channel. It resets the number of used elements and sets the
 * message length of all elements in the defer queue to 0, to indicate that these defer queue elements are not used.
 *
 * @pre The defer queue module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void reddfq_InitDeferQueue(const uint32_t red_channel_id);

/**
 * @brief Add a redundancy layer message to the defer queue. If the queue is full, the message will be ignored.
 *
 * When there is free space in the defer queue, a RedL message with a current time stamp is added to the buffer. If the defer queue is full, the message is
 * ignored. After adding the message to the defer queue, the number of used entries is updated.
 *
 * @pre The defer queue module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] redundancy_message Pointer to the message struct to add to the defer queue. If the pointer is NULL, a ::radef_kInvalidParameter fatal
 * error is thrown. The redundancy_message->message_size must be in the range from ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE to
 * ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, else a ::radef_kInvalidParameter fatal error message is thrown.
 */
void reddfq_AddMessage(const uint32_t red_channel_id, const redtyp_RedundancyMessage* const redundancy_message);

/**
 * @brief Get and remove a redundancy layer message from the defer queue.
 *
 * When there is messages in the defer queue with the requested sequence number, it is read from the defer queue, saved into the passed structure and the number
 * of used entries is updated and the message size in the defer queue is set to 0 to indicate a free entry.
 * The message size read from the defer queue must be in the range from ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE to ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInternalError fatal error message is thrown.
 * If the message with the requested sequence number is not found in the defer queue, a ::radef_kInvalidSequenceNumber fatal error is thrown.
 *
 * @pre The defer queue module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] sequence_number Sequence number of the message to read and remove from the queue. The full value range is valid and usable.
 * @param [in] redundancy_message Pointer to RedL message structure, where the message must be saved. If the pointer is NULL, a
 * ::radef_kInvalidParameter fatal error is thrown.
 */
void reddfq_GetMessage(const uint32_t red_channel_id, const uint32_t sequence_number, redtyp_RedundancyMessage* const redundancy_message);

/**
 * @brief Check defer queue timeout on a dedicated redundancy channel.
 *
 * This function checks if a message in the defer queue fulfils the defer queue timeout criteria: rasys_GetTimerValue() - message received timestamp > Tseq
 *
 * @pre The defer queue module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @return true, if there is a defer queue timeout.
 * @return false, if there is no defer queue timeout.
 */
bool reddfq_IsTimeout(const uint32_t red_channel_id);

/**
 * @brief Checks if a message with a defined sequence number is in the defer queue.
 *
 * This function checks if a message with a defined sequence number is in the defer queue and returns true if the sequence number was found.
 *
 * @pre The defer queue module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] sequence_number Sequence number of the message to search. The full value range is valid and usable.
 * @return true, if a message with the sequence number is found.
 * @return false, if no message with the sequence number is found.
 */
bool reddfq_Contains(const uint32_t red_channel_id, const uint32_t sequence_number);

/**
 * @brief Returns the oldest sequence number found in the defer queue.
 *
 * This function searches the oldest sequence number of all messages in the defer queue and returns it. This function takes respect to uint32_t wrap around for
 * up counted sequence numbers. A sequence number is detected to be older, if the difference of two sequence numbers - 1 is < (UINT32_MAX / 2).
 *
 * @pre The defer queue module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 * @pre The defer queue module must not be empty, otherwise a ::radef_kDeferQueueEmpty fatal error is thrown.
 * @pre The number of used defer queue entries must be in the range form 1 to the configured defer queue size, otherwise a ::radef_kInternalError fatal error is
 * thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @return Oldest sequence number found in the defer queue.
 */
uint32_t reddfq_GetOldestSequenceNumber(const uint32_t red_channel_id);

/**
 * @brief Get the number of used defer queue entries.
 *
 * This function returns the number of used defer queue entries.
 *
 * @pre The defer queue module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @return Number of used defer queue entries.
 */
uint32_t reddfq_GetUsedEntries(const uint32_t red_channel_id);

/**
 * @brief Returns true, if the sequence_number_to_compare is older than sequence_number_reference.
 *
 * This function checks if sequence_number_to_compare is older than sequence_number_reference with respect to uint32_t wrap around for up counted sequence
 * numbers. The limit to detect the older value is a difference of sequence_number_reference - (sequence_number_to_compare of + 1U) < (UINT32_MAX / 2U).
 *
 * @param sequence_number_reference Sequence number reference in comparison. The full value range is valid and usable.
 * @param sequence_number_to_compare Sequence number to compare. The full value range is valid and usable.
 * @return true, if sequence_number_to_compare < sequence_number_reference
 * @return false, if sequence_number_to_compare >= sequence_number_reference
 */
bool reddfq_IsSequenceNumberOlder(const uint32_t sequence_number_to_compare, const uint32_t sequence_number_reference);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDDFQ_RED_DEFER_QUEUE_H_
