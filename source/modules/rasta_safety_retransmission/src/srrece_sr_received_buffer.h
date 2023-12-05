/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srrece_sr_received_buffer.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_receiveBuf
 * @{
 *
 * @brief Interface of RaSTA SafRetL received buffer module.
 *
 * This module buffers the payload of successfully received SafRetL messages, for the read from the application layer. The received buffer is organized as a
 * FIFO ring buffer. One buffer entry holds a srtyp_SrMessagePayload struct, which contains the payload of SafRetL PDU message.
 *
 * @implementsReq{RASW-607} Component sr_received_buffer Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRRECE_SAFETY_RETRANSMISSION_RECEIVED_BUFFER_H_
#define SRRECE_SAFETY_RETRANSMISSION_RECEIVED_BUFFER_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

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
 * @brief Initialize the SafRetL received buffer module.
 *
 * This function is used to initialize the received buffer module. It saves the passed number of connections and the configured size of the buffer (NsendMax).
 * For all configured connections, the ::srrece_InitBuffer function is called to properly initialize the buffer for all configured connections. A fatal error is
 * raised, if this function is called multiple times.
 *
 * @pre The received buffer module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-611} Init sr_received_buffer Function
 *
 * @param [in] configured_connections Number of configured RaSTA connections. Valid range: 1 <= value <= ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
 * @param [in] configured_n_send_max Configured receive buffer size [messages]. Valid range: ::srcty_kMinNSendMax <= value <= ::RADEF_MAX_N_SEND_MAX.
 */
void srrece_Init(const uint32_t configured_connections, const uint16_t configured_n_send_max);

/**
 * @brief Initialize the received buffer of a dedicated RaSTA connection.
 *
 * This function initializes the buffer of a given RaSTA connection. It resets all properties of the buffer (read, write index and used entries) and also sets
 * the message length of all elements in the buffer to 0.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-612} Init Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srrece_InitBuffer(const uint32_t connection_id);

/**
 * @brief Add a SafRetL message to the received buffer of a dedicated RaSTA connection. A fatal error is raised, if the buffer is full.
 *
 * When there is free space in the buffer, a SafRetL message is added to the buffer. If the buffer is full, a ::radef_kReceiveBufferFull fatal error message
 * is thrown. After adding the message to the buffer, the position pointer and used entries are updated.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-608} Add to Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] message_payload Pointer to message payload structure that must be added to the buffer. If the pointer is NULL, a ::radef_kInvalidParameter fatal
 * error is thrown. For the message payload the full value range is valid and usable, the message payload size has a valid range of
 * ::srcty_kMinSrLayerPayloadDataSize <= value <= ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE.
 */
void srrece_AddToBuffer(const uint32_t connection_id, const srtyp_SrMessagePayload* const message_payload);

/**
 * @brief Read and remove a SafRetL message payload from the received buffer of a dedicated RaSTA connection.
 *
 * When there are messages in the buffer, the oldest SafRetL message is read from the buffer, saved into the passed structure, the position pointers & used
 * entries are updated and a ::radef_kNoError returned. If the buffer is empty, a ::radef_kNoMessageReceived is returned.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-613} Read from Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [out] message_payload Pointer to SafRetL message payload structure, where the message must be saved. If the pointer is NULL, a
 * ::radef_kInvalidParameter fatal error is thrown.
 * @return radef_kNoError -> Message successfully read from the buffer.
 * @return radef_kNoMessageReceived -> No message payload in the buffer.
 */
radef_RaStaReturnCode srrece_ReadFromBuffer(const uint32_t connection_id, srtyp_SrMessagePayload* const message_payload);

/**
 * @brief Get the payload size of the next message that is read from a dedicated RaSTA connection.
 *
 * This functions returns the payload size of the next message that can be read from a specific connection. If there is no message to be read, 0 will be
 * returned.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-823} Get Payload Size of Next Message To Read Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return uint32_t -> Size of the payload of the next message to read [bytes]
 */
uint32_t srrece_GetPayloadSizeOfNextMessageToRead(const uint32_t connection_id);

/**
 * @brief Get the number of free buffer entries.
 *
 * This function returns the amount of free entires in the received buffer of a given connection.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-610} Get Free Buffer Entries Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return Number of free entries in the received buffer [messages]
 */
uint16_t srrece_GetFreeBufferEntries(const uint32_t connection_id);

/**
 * @brief Get the number of used buffer entries.
 *
 * This function returns the amount of used entires in the received buffer of a given connection.
 *
 * @pre The received buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-609} Get Used Buffer Entries Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return Number of used entries in the received buffer [messages]
 */
uint16_t srrece_GetUsedBufferEntries(const uint32_t connection_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRRECE_SAFETY_RETRANSMISSION_RECEIVED_BUFFER_H_
