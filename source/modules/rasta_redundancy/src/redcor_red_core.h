/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redcor_red_core.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_core
 * @{
 *
 * @brief Interface of RaSTA redundancy layer core module.
 *
 * This module provides the core functionality of the RaSTA redundancy layer. This includes:
 * - Check of redundancy layer configuration data
 * - Defer queue timeout handling and defer queue delivery to the received buffer
 * - Processing of received redundancy layer messages and forwarding to the received buffer
 * - Send redundancy layer messages to the transport channels
 * .
 */
#ifndef REDCOR_RED_CORE_H_
#define REDCOR_RED_CORE_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "rasta_redundancy/redcty_red_config_types.h"
#include "redtyp_red_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Struct for the newly received message input buffer.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  bool message_in_buffer;                   ///< flag which indicates, that a new unprocessed message is in the message input buffer
  uint32_t transport_channel_id;            ///< transport channel of message in input buffer, used for diagnostics
  redtyp_RedundancyMessage message_buffer;  ///< input buffer for newly received message
} redcor_InputBuffer;
//lint -restore

/**
 * @brief Struct for the message payload send buffer.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  bool message_in_buffer;                                  ///< flag which indicates, that a unprocessed message is in the send buffer
  redtyp_RedundancyMessagePayload message_payload_buffer;  ///< buffer for message payload to send
} redcor_SendBuffer;
//lint -restore

/**
 * @brief Struct for the process data of a redundancy channel.
 */
//lint -save -e9045 (structures are defined globally)
typedef struct {
  uint32_t seq_tx;  ///< next sequence number to be sent
  uint32_t seq_rx;  ///< next sequence number expected for the receipt

  /// true, if received data is pending on a transport channel
  bool received_data_pending[RADEF_MAX_NUMBER_OF_RED_CHANNELS * RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS];

  redcor_InputBuffer input_buffer;  ///< input buffer for newly received message
  redcor_SendBuffer send_buffer;    ///< buffer for message payload to send
} redcor_RedundancyChannelData;
//lint -restore

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
 * @brief Checks if the redundancy layer configuration is valid.
 *
 * This function checks if all elements of the redundancy layer configuration are in their valid ranges and if all configuration elements are consistent to
 * the others.
 * This function is only a wrapper function, which calls reddia_IsConfigurationValid(). For a more detailed description, see the description of
 * reddia_IsConfigurationValid().
 *
 * @param [in] redundancy_layer_configuration Pointer to redundancy layer configuration data structure. If the pointer is NULL, a ::radef_kInvalidParameter
 * fatal error is thrown.
 * @return true, if the configuration is valid.
 * @return false, if the configuration is invalid.
 */
bool redcor_IsConfigurationValid(const redcty_RedundancyLayerConfiguration *const redundancy_layer_configuration);

/**
 * @brief Initialize all data of the redundancy layer core.
 *
 * This function is used to initialize the core module. The validity of the configuration is checked by calling the redcor_IsConfigurationValid() function. If
 * the configuration is not valid a ::radef_kInvalidConfiguration fatal error is thrown. It saves the passed redundancy layer configuration. The initialization
 * of the redundancy messages module and the redundancy diagnostics module is called. Finally for all configured channels the ::redcor_InitRedundancyChannelData
 * function is called to properly initialize the data for all configured channels.
 *
 * @pre The core module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @param [in] redundancy_layer_configuration Pointer to redundancy layer configuration data structure. If the pointer is NULL, a ::radef_kInvalidParameter
 * fatal error is thrown. If the configuration is not valid a::radef_kInvalidConfiguration fatal error is thrown.
 */
void redcor_Init(const redcty_RedundancyLayerConfiguration *const redundancy_layer_configuration);

/**
 * @brief Initialize the data of a dedicated redundancy channel.
 *
 * This function initializes the data of a given redundancy channel. It resets the following properties:
 * - Sequence number receive
 * - Sequence number transmit
 * - Received data pending flags
 * - Input buffer
 * - Send buffer
 * .
 * The following initialization functions are also called here for the given redundancy channel:
 * - reddfq_InitDeferQueue()
 * - redrbf_InitBuffer()
 * - reddia_InitRedundancyChannelDiagnostics()
 * .
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void redcor_InitRedundancyChannelData(const uint32_t red_channel_id);

/**
 * @brief Handle the defer queue timeout and deliver the defer queue messages to the received buffer.
 *
 * This function handles the defer queue timeout and delivers the defer queue messages to the received buffer. First, the sequence number receive is set to the
 * oldest sequence number found in the defer queue. The messages stored in the defer queue are delivered to the received buffer, as long as there is no sequence
 * number missing. The sequence number receive is updated for each delivered message.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void redcor_DeferQueueTimeout(const uint32_t red_channel_id);

/**
 * @brief Write a received message to the input buffer.
 *
 * This function copies a received message and its message size and transport channel identification to the input buffer. The message_in_buffer flag is set. The
 * transport_channel_id is saved to be passed to the redundancy channel diagnostics.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [in] received_message Pointer to struct containing the message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * The received_message->message_size must be in the range from ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE to ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInvalidParameter fatal error message is thrown.
 */
void redcor_WriteReceivedMessageToInputBuffer(const uint32_t red_channel_id, const uint32_t transport_channel_id,
                                              const redtyp_RedundancyMessage *const received_message);

/**
 * @brief Clear input buffer message pending flag.
 *
 * This function clears the input buffer message pending flag in a given redundancy channel.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void redcor_ClearInputBufferMessagePendingFlag(const uint32_t red_channel_id);

/**
 * @brief Process a received message from the input buffer.
 *
 * This function processes a received message from the input buffer. The following tasks are done:
 * - Check if a received message is in the input buffer, else a ::radef_kNoMessageReceived fatal error is thrown.
 * - Check if the message CRC is correct, else the message is ignored
 * - Check that after the initialization only one message with sequence number 0 is processed, else ignore the message
 * - Ignore messages with Seq_pdu < Seq_rx, but update the redundancy channel diagnostics
 * - For messages with Seq_pdu = Seq_rx, add message to received buffer, increase Seq_rx, update the redundancy channel diagnostics
 * - For messages with Seq_rx < Seq_pdu <= (Seq_rx + N_defer_queue_size * 10), add message to the defer queue, if not already in the queue, and update the
 * redundancy channel diagnostics
 * - For messages with Seq_pdu > (Seq_rx + N_defer_queue_size * 10), ignore the message without update of the redundancy channel diagnostics
 * - Clear the input buffer message pending flag
 * .
 * All detailed flow chart can be found in figure 18 of the standard "Elektrische Bahn-Signalanlagen – Teil 200: Sicheres Übertragungsprotokoll RaSTA nach DIN
 * EN 50159 (VDE0831-159); Juni 2015".
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void redcor_ProcessReceivedMessage(const uint32_t red_channel_id);

/**
 * @brief Set a flag, which indicates that a received message is pending to read from the transport layer.
 *
 * This function sets the received message pending flag in a given redundancy channel for a given transport channel.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInvalidParameter fatal error is thrown.
 */
void redcor_SetMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id);

/**
 * @brief Get the received message pending flag for a dedicated transport channel.
 *
 * This function gets the received message pending flag from a given redundancy channel for a given transport channel.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInvalidParameter fatal error is thrown.
 * @return true, if a received message is pending
 * @return false, if no received message is pending
 */
bool redcor_GetMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id);

/**
 * @brief Clear the received message pending flag.
 *
 * This function clears the received message pending flag in a given redundancy channel for a given transport channel.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the
 * specified redundancy channel, a ::radef_kInvalidParameter fatal error is thrown.
 */
void redcor_ClearMessagePendingFlag(const uint32_t red_channel_id, const uint32_t transport_channel_id);

/**
 * @brief Write message payload to send buffer.
 *
 * This function copies a message to send and its message size to the send buffer of a given redundancy channel. The message_in_buffer flag is set.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 * @param [in] payload_size The payload_size must be in the range from ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE to ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInvalidParameter fatal error message is thrown.
 * @param [in] payload_data Pointer to message payload data byte array which must be written to the buffer. If the pointer is NULL, a ::radef_kInvalidParameter
 * fatal error is thrown. For the message payload data the full value range is valid and usable.
 */
void redcor_WriteMessagePayloadToSendBuffer(const uint32_t red_channel_id, const uint16_t payload_size, const uint8_t *const payload_data);

/**
 * @brief Clear send buffer message pending flag.
 *
 * This function clears the send buffer message pending flag in a given redundancy channel.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void redcor_ClearSendBufferMessagePendingFlag(const uint32_t red_channel_id);

/**
 * @brief Send a redundancy layer message from the send buffer to the transport channels.
 *
 * This function sends a redundancy layer message from the send buffer of a given redundancy channel all to the transport channels associated with this
 * redundancy channel. The sequence number transmit is increased. The message in buffer flag is cleared after sending the messages.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of channels.
 */
void redcor_SendMessage(const uint32_t red_channel_id);

/**
 * @brief Get the associated redundancy channel from a given transport channel.
 *
 * This function gets the redundancy channel, which is associated to the given transport channel.
 *
 * @pre The core module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS. If the transport channel id is out of range, or the transport channel id is not in the configuration of the a
 * redundancy channel, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] red_channel_id Pointer to associated redundancy channel identification. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is
 * thrown.
 */
void redcor_GetAssociatedRedundancyChannel(const uint32_t transport_channel_id, uint32_t *const red_channel_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDCOR_RED_CORE_H_
