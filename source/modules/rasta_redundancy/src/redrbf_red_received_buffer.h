/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redrbf_red_received_buffer.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_receiveBuf
 * @{
 *
 * @brief Interface of RaSTA redundancy layer received messages buffer module.
 *
 * This module buffers the payload of successfully received redundancy layer messages, for the read from safety and retransmission layer. The received buffer is
 * organized as a FIFO ring buffer. One buffer entry holds a redtyp_RedundancyMessagePayload struct, which contains the payload of redundancy layer PDU message.
 */
#ifndef REDRBF_RED_RECEIVED_BUFFER_H_
#define REDRBF_RED_RECEIVED_BUFFER_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"
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
 * @brief Initialize the RedL received buffer module.
 *
 * This function is used to initialize the received buffer module. It saves the passed number of redundancy channels. For all configured channels, the
 * ::redrbf_InitBuffer function is called to properly initialize the buffer for all configured channels. A fatal error is raised, if this function is called
 * multiple times.
 *
 * @pre The received buffer module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @param [in] configured_red_channels Number of configured redundancy channels. Valid range: 1 <= value <= ::RADEF_MAX_NUMBER_OF_RED_CHANNELS.
 */
void redrbf_Init(const uint32_t configured_red_channels);

/**
 * @brief Initialize the received buffer of a dedicated redundancy channel.
 *
 * This function initializes the buffer of a given redundancy channel. It resets all properties of the buffer (read, write and used elements) and also sets the
 * message length of all elements in the buffer to 0.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void redrbf_InitBuffer(const uint32_t red_channel_id);

/**
 * @brief Add a RedL message to the received buffer of a dedicated redundancy channel. A fatal error is raised, if the buffer is full.
 *
 * When there is free space in the buffer, a RedL message is added to the buffer. If the buffer is full, a ::radef_kReceiveBufferFull fatal error message
 * is thrown. After adding the message to the buffer, the position index and buffer length are updated.
 * The message_payload->payload_size must be in the range from ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE to ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInvalidParameter fatal error message is thrown.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] message_payload Pointer to message payload structure that must be added to the buffer. If the pointer is NULL, a ::radef_kInvalidParameter fatal
 * error is thrown. For the message payload the full value range is valid and usable.
 */
void redrbf_AddToBuffer(const uint32_t red_channel_id, const redtyp_RedundancyMessagePayload* const message_payload);

/**
 * @brief Read and remove a RedL message payload from the received buffer of a dedicated redundancy channel.
 *
 * When there are messages in the buffer, the oldest SafRetL message is read from the buffer, saved into the passed structure, the position index & length are
 * updated and a ::radef_kNoError returned. If the buffer is empty, a ::radef_kNoMessageReceived is returned.
 * The message payload size read from the buffer must be in the range from ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE to ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE,
 * else a ::radef_kInternalError fatal error message is thrown.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] buffer_size Size of the external memory buffer buffer to store the read message data [bytes]. The full value range is valid and usable.
 * @param [out] message_size Pointer to size of the read message data [bytes]. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] message_buffer Pointer to external memory buffer used to store the read message data. If the pointer is NULL, a ::radef_kInvalidParameter fatal
 * error is thrown.
 * @return radef_kNoError -> Message successfully read from the buffer.
 * @return radef_kNoMessageReceived -> No received message in the buffer.
 * @return radef_kInvalidBufferSize -> External buffer size is too small for the current message.
 */
radef_RaStaReturnCode redrbf_ReadFromBuffer(const uint32_t red_channel_id, const uint16_t buffer_size, uint16_t* const message_size,
                                            uint8_t* const message_buffer);

/**
 * @brief Get the number of free buffer entries [messages].
 *
 * This function returns the amount of free entires in the received buffer of a given channel.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @return Number of free entries in the received buffer.
 */
uint16_t redrbf_GetFreeBufferEntries(const uint32_t red_channel_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDRBF_RED_RECEIVED_BUFFER_H_
