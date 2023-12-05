/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redint_red_interface.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_api
 * @{
 *
 * @brief Interface of RaSTA redundancy layer.
 *
 * This module defines and implements the RedL interface functions:
 * - Initialize the RedL
 * - Get initialization state of the RedL
 * - Open redundancy channel
 * - Close redundancy channel
 * - Send data
 * - Read data
 * - Check timings
 * .
 */
#ifndef REDINT_RED_INTERFACE_H_
#define REDINT_RED_INTERFACE_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_redundancy/redcty_red_config_types.h"

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
 * @brief Initialize the RedL.
 *
 * This function is used to initialize the red_interface module. It checks the passed ::redcty_RedundancyLayerConfiguration configuration and if it is
 * valid, it initializes the internal data, the state machine module, the defer queue module, the received buffer module and the core module.
 *
 * @pre The RedL interface module must not be initialized, otherwise a ::radef_kAlreadyInitialized error is returned.
 *
 * @param [in] redundancy_layer_configuration Pointer to RedL configuration. More details about valid configuration can be found
 * directly in ::redcty_RedundancyLayerConfiguration. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @return ::radef_kNoError -> successful initialized
 * @return ::radef_kAlreadyInitialized -> module already initialized
 * @return ::radef_kInvalidConfiguration -> invalid configuration data
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode redint_Init(const redcty_RedundancyLayerConfiguration* const redundancy_layer_configuration);

/**
 * @brief Get the initialization state of the RedL.
 *
 * This function returns the initialization state of the RedL.
 *
 * @return ::radef_kNoError -> successful initialized
 * @return ::radef_kNotInitialized -> not initialized
 */
radef_RaStaReturnCode redint_GetInitializationState(void);

/**
 * @brief Open a given redundancy channel.
 *
 * This function initializes the internal data of a given redundancy channel and opens a redundancy channel by calling redstm_ProcessChannelStateMachine() with
 * the ::redstm_kRedundancyChannelEventOpen event.
 *
 * @pre The RedL interface module must be initialized, otherwise a ::radef_kNotInitialized error is returned.
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode redint_OpenRedundancyChannel(const uint32_t redundancy_channel_id);

/**
 * @brief Close a given redundancy channel.
 *
 * This function closes a given redundancy channel by calling redstm_ProcessChannelStateMachine() with the ::redstm_kRedundancyChannelEventClose event.
 *
 * @pre The RedL interface module must be initialized, otherwise a ::radef_kNotInitialized error is returned.
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode redint_CloseRedundancyChannel(const uint32_t redundancy_channel_id);

/**
 * @brief Send a message over a given redundancy channel.
 *
 * This function copies the given message to the send buffer of the given redundancy channel and triggers the sending of this message by calling
 * redstm_ProcessChannelStateMachine() with the ::redstm_kRedundancyChannelEventSendData event.
 *
 * @pre The RedL interface module must be initialized, otherwise a ::radef_kNotInitialized error is returned.
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 * @param [in] message_size The size of the message data [bytes] must be in the range from ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE to
 * ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, else a ::radef_kInvalidMessageSize error is returned.
 * @param [in] message_data Pointer to the message data byte array which must be send. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * For the message data the full value range is valid and usable.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 * @return ::radef_kInvalidMessageSize -> invalid message size
 * @return ::radef_kInvalidOperationInCurrentState -> sending messages is not allowed in the current redundancy channel state machine state
 */
radef_RaStaReturnCode redint_SendMessage(const uint32_t redundancy_channel_id, const uint16_t message_size, const uint8_t* const message_data);

/**
 * @brief Read a received message from a given redundancy channel.
 *
 * This function reads a received message from the received buffer of a given redundancy channel. If no message is in the received buffer, a
 * ::radef_kNoMessageReceived error is returned.
 *
 * @pre The RedL interface module must be initialized, otherwise a ::radef_kNotInitialized error is returned.
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 * @param [in] buffer_size Size of the buffer [bytes] available in the upper layer. Valid range: ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <= value <=
 * ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE. If the size of the message to read is greater then the buffer size a ::radef_kInvalidBufferSize error is returned.
 * @param [out] message_size Pointer to the size of the received message data [bytes]. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @param [out] message_buffer Pointer to a buffer for saving the received message. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNoMessageReceived -> no message received (used for polling)
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 * @return ::radef_kInvalidBufferSize -> invalid buffer size
 */
radef_RaStaReturnCode redint_ReadMessage(const uint32_t redundancy_channel_id, const uint16_t buffer_size, uint16_t* const message_size,
                                         uint8_t* const message_buffer);

/**
 * @brief Check redundancy layer timings and read pending messages form the transport channels.
 *
 * This function calls the internal function ReceivedMessagesPolling() and checks the defer queue timeout by calling reddfq_IsTimeout() and
 * redstm_ProcessChannelStateMachine() with the ::redstm_kRedundancyChannelEventDeferTimeout event, for all configured redundancy channels which are in the
 * ::redstm_kRedundancyChannelStateUp state.
 *
 * @pre The RedL interface module must be initialized, otherwise a ::radef_kNotInitialized error is returned.
 *
 * @remark This function must be called periodically, in a appropriate interval related to the configured timings.
 *
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 */
radef_RaStaReturnCode redint_CheckTimings(void);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDINT_RED_INTERFACE_H_
