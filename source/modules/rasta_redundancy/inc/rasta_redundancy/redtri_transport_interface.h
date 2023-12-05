/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redtri_transport_interface.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_transport_interface
 * @{
 *
 * @brief Interface of RaSTA transport layer.
 *
 * This module defines the interface functions (like init, send & read message) for the transport layer interface. The RedL only defines the interface, the
 * implementation of this interface functions must be done in the transport layer.
 *
 * @remark The error handling for all function must be implemented and handled by the system integrator when developing the transport layer.
 */
#ifndef REDTRI_TRANSPORT_INTERFACE_H_
#define REDTRI_TRANSPORT_INTERFACE_H_

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
 * @brief Initialize transport layer.
 *
 * This function is used to initialize the transport layer. The RedL only defines the interface function, the implementation of this interface function must be
 * done in the transport layer.
 */
void redtri_Init(void);

/**
 * @brief Send a RedL message over a transport channel.
 *
 * This function is used to send a RedL message over a specific transport channel of the transport layer. The RedL only defines the interface function, the
 * implementation of this interface function must be done in the transport layer.
 *
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS.
 * @param [in] message_size Size of the message data [bytes]. Valid range: ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE <= value <=
 * ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE.
 * @param [in] message_data Pointer to message data array. For the message data the full value range is valid and usable.
 */
void redtri_SendMessage(const uint32_t transport_channel_id, const uint16_t message_size, const uint8_t* const message_data);

/**
 * @brief Read a received RedL message from a transport channel.
 *
 * This function is used to read a RedL message from a specific transport channel of the transport layer. The RedL only defines the interface function, the
 * implementation of this interface function must be done in the transport layer.
 *
 * @param [in] transport_channel_id Transport channel identification. Valid range: 0 <= value < ::RADEF_MAX_NUMBER_OF_RED_CHANNELS *
 * ::RADEF_MAX_NUMBER_OF_TRANSPORT_CHANNELS.
 * @param [in] buffer_size Size of the buffer [bytes]. Valid range: ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE <= value <= ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE.
 * @param [out] message_size Pointer to the size of the received message data [bytes].
 * @param [out] message_buffer Pointer to a buffer for saving the received message. Enough memory to save a message with buffer_size must be allocated.
 * @return radef_kNoError -> successful operation
 * @return radef_kNoMessageReceived -> no message received (used for polling)
 */
radef_RaStaReturnCode redtri_ReadMessage(const uint32_t transport_channel_id, const uint16_t buffer_size, uint16_t* const message_size,
                                         uint8_t* const message_buffer);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDTRI_TRANSPORT_INTERFACE_H_
