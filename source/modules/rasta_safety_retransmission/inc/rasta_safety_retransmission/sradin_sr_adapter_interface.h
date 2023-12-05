/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file sradin_sr_adapter_interface.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_adapter_interface
 * @{
 *
 * @brief Interface of RaSTA SafRetL adapter.
 *
 * This module defines the interface functions (like init, open & close redundancy channel, send & read message) for the SafRetL adapter interface. The SafRetL
 * only defines the interface, the implementation of this adapter interface functions must be done in the SafRetL adapter.
 *
 * @remark The error handling for all function must be implemented and handled by the system integrator when developing the SafRetL adapter.
 *
 * @implementsReq{RASW-647} Component sr_adapter_interface Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 */
#ifndef SRADIN_SAFETY_RETRANSMISSION_ADAPTER_INTERFACE_H_
#define SRADIN_SAFETY_RETRANSMISSION_ADAPTER_INTERFACE_H_

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
 * @brief Initialize SafRetL adapter
 *
 * This function is used to initialize the sr_adapter_interface module. The SafRetL only defines the interface function, the implementation of this interface
 * function must be done in the SafRetL adapter.
 *
 * @implementsReq{RASW-648} Init sr_adapter_interface Function
 * @implementsReq{RASW-353} Initialization Function Structure
 */
void sradin_Init(void);

/**
 * @brief Open a redundancy channel.
 *
 * This function is used to open a specific redundancy channel of the RedL. The SafRetL only defines the interface function, the implementation of this
 * interface function must be done in the SafRetL adapter.
 *
 * @remark There is a 1:1 mapping between the connection id and redundancy channel id.
 *
 * @implementsReq{RASW-649} Open Redundancy Channel Function
 * @implementsReq{RASW-369} Open Redundancy Channel Function Structure
 * @implementsReq{RASW-367} Redundancy Channel Id
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 */
void sradin_OpenRedundancyChannel(const uint32_t redundancy_channel_id);

/**
 * @brief Close a redundancy channel.
 *
 * This function is used to close a specific redundancy channel of the RedL. The SafRetL only defines the interface function, the implementation of this
 * interface function must be done in the SafRetL adapter.
 *
 * @remark There is a 1:1 mapping between the connection id and redundancy channel id.
 *
 * @implementsReq{RASW-650} Close Redundancy Channel Function
 * @implementsReq{RASW-368} Close Redundancy Channel Function Structure
 * @implementsReq{RASW-367} Redundancy Channel Id
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 */
void sradin_CloseRedundancyChannel(const uint32_t redundancy_channel_id);

/**
 * @brief Send a message over a redundancy channel.
 *
 * This function is used to send a SafRetL message over a specific redundancy channel of the RedL. The SafRetL only defines the interface function, the
 * implementation of this interface function must be done in the SafRetL adapter.
 *
 * @remark There is a 1:1 mapping between the connection id and redundancy channel id.
 *
 * @implementsReq{RASW-651} Send Message Function
 * @implementsReq{RASW-364} Send Message Function Structure
 * @implementsReq{RASW-363} Redundancy Channel Id
 * @implementsReq{RASW-387} Message Size
 * @implementsReq{RASW-385} Message Data
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 * @param [in] message_size Size of the message data [bytes]. Valid range: ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <= value <=
 * ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE.
 * @param [in] message_data Pointer to message data array. For the message data the full value range is valid and usable.
 */
void sradin_SendMessage(const uint32_t redundancy_channel_id, const uint16_t message_size, const uint8_t* const message_data);

/**
 * @brief Read a received message from a redundancy channel.
 *
 * This function is used to read a SafRetL message from a specific redundancy channel of the RedL. The SafRetL only defines the interface function, the
 * implementation of this interface function must be done in the SafRetL adapter.
 *
 * @implementsReq{RASW-652} Read Message Function
 * @implementsReq{RASW-375} Read Message Function Structure
 * @implementsReq{RASW-381} Redundancy Channel Id
 * @implementsReq{RASW-379} Buffer Size
 * @implementsReq{RASW-372} Message Size
 * @implementsReq{RASW-371} Message Buffer
 * @implementsReq{RASW-374} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 *
 * @param [in] redundancy_channel_id Redundancy channel identification. Valid range: 0 <= value < configured number of redundancy channels.
 * @param [in] buffer_size Size of the buffer provided to parameter message_buffer [bytes]. Valid range: ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <= value <=
 * ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE. Any value in this range can be used, must be large enough to store the received message.
 * @param [out] message_size Pointer to the size of the received message data [bytes].
 * @param [out] message_buffer Pointer to a buffer for saving the received message. Enough memory to save a message with buffer_size must be allocated.
 * @return radef_kNoError -> successful operation
 * @return radef_kNoMessageReceived -> no message received (used for polling)
 */
radef_RaStaReturnCode sradin_ReadMessage(const uint32_t redundancy_channel_id, const uint16_t buffer_size, uint16_t* const message_size,
                                         uint8_t* const message_buffer);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRADIN_SAFETY_RETRANSMISSION_ADAPTER_INTERFACE_H_
