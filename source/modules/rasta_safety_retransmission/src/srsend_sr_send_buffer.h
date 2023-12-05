/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srsend_sr_send_buffer.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_sendBuf
 * @{
 *
 * @brief Interface of RaSTA SafRetL send buffer module.
 *
 * This module buffers the messages which are waiting for transmission and sent messages which could be requested for retransmission, since they are not yet
 * confirmed. The send buffer is organized as a FIFO ring buffer. One buffer entry holds a srtyp_SrMessage struct, which contains a whole SafRetL PDU message.
 *
 * @implementsReq{RASW-595} Component sr_send_buffer Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRSEND_SAFETY_RETRANSMISSION_SEND_BUFFER_H_
#define SRSEND_SAFETY_RETRANSMISSION_SEND_BUFFER_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
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
 * @brief Initialize all data of the SafRetL send buffer module.
 *
 * This function is used to initialize the send buffer module. It saves the passed number of connections. For all configured connections, the
 * ::srsend_InitBuffer function is called to properly initialize the buffer for all configured connections. A fatal error is raised, if this function is called
 * multiple times.
 *
 * @pre The send buffer module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-600} Init sr_send_buffer Function
 *
 * @param [in] configured_connections Number of configured RaSTA connections. Valid range: 1 <= value <= ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
 */
void srsend_Init(const uint32_t configured_connections);

/**
 * @brief Initialize the send buffer of a dedicated RaSTA connection.
 *
 * This function initializes the buffer of a given RaSTA connection. It resets all properties of the buffer (read, write, remove index and used entries) and
 * also sets the message length of all elements in the buffer to 0 and clears the already sent flag.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-601} Init Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 */
void srsend_InitBuffer(const uint32_t connection_id);

/**
 * @brief Add a SafRetL message to the send buffer of a dedicated RaSTA connection.
 *
 * When there is free space in the buffer, a SafRetL message is added to the buffer. If the buffer is full, a ::radef_kSendBufferFull fatal error message
 * is thrown. After adding the message to the buffer, the position index and used entires are updated.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-596} Add to Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] message Pointer to SafRetL message structure that must be added to the buffer. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is
 * thrown. For the message payload the full value range is valid and usable, the message size has a valid range of ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE <=
 * value <= ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE.
 */
void srsend_AddToBuffer(const uint32_t connection_id, const srtyp_SrMessage* const message);

/**
 * @brief Read a SafRetL message from the send buffer of a dedicated RaSTA connection.
 *
 * When there are messages in the buffer, the oldest SafRetL message is read from the buffer, saved into the passed structure, the read position pointer is
 * updated and a ::radef_kNoError returned. If the buffer is empty, a ::radef_kNoMessageToSend is returned.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-604} Read Message to Send Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [out] message Pointer to SafRetL message structure, where the message must be saved. If the pointer is NULL, a ::radef_kInvalidParameter fatal error
 * is thrown.
 * @return radef_kNoError -> message successfully read from the buffer
 * @return radef_kNoMessageToSend -> no message in the buffer
 */
radef_RaStaReturnCode srsend_ReadMessageToSend(const uint32_t connection_id, srtyp_SrMessage* const message);

/**
 * @brief Prepare send buffer for retransmission starting with a defined sequence number.
 *
 * This function prepares the send buffer for a retransmission of all messages after a given sequence number. For this, the following steps are done:
 * - remove messages which must not be retransmitted (all other than Data and RetrData)
 * - add RetrResp message to the send buffer
 * - transform requested already sent Data & RetrData messages
 * - while still not yet send RetrData is available, add it for retransmission. As soon as an other message type is added, RetrData is then forbidden
 * - add a heartbeat message after the retransmission, if no data message is pending in the buffer to indicate the end of the retransmission
 * - the remaining not yet send messages can be added to the buffer. (HB messages must be filtered from beeing send. Only RetrReq and Data are valid, other
 * message types should not be in the buffer anymore)
 * .
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 * @pre All messages before and the requested sequence number must be confirmed before calling this method.
 * @pre Sequence number for retransmission must be in the buffer, otherwise a ::radef_kInvalidSequenceNumber fatal error is thrown.
 * @pre It is assumed, that the buffer elements are in proper order with ascending sequence number and without gaps in the sequence numbers.
 *
 * @implementsReq{RASW-603} Prepare Buffer for Retransmission Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] sequence_number_for_retransmission Last good sequence number after which the retransmission must be started. The full value range is valid and
 * usable.
 * @param [in] message_header Message header data to create a new message. The message header contains also the current sequence number from which the new
 * message must continue.
 * @param [out] new_current_sequence_number New sequence number for new messages after reworking the send buffer for the retransmission. If the pointer is NULL,
 * a ::radef_kInvalidParameter fatal error is thrown.
 */
void srsend_PrepareBufferForRetr(const uint32_t connection_id, const uint32_t sequence_number_for_retransmission,
                                 const srtyp_SrMessageHeaderCreate message_header, uint32_t* const new_current_sequence_number);

/**
 * @brief Checks if a message with a specific sequence number is in the send buffer.
 *
 * This function checks if a specific sequence number is present in the send buffer of a given RaSTA connection and returns ::radef_kNoError if the number is
 * found or ::radef_kInvalidSequenceNumber if its not found.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-602} Is Sequence Number in Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] sequence_number Sequence number to check if present in send buffer. The full value range is valid and usable.
 * @return radef_kNoError -> sequence number found in the send buffer
 * @return radef_kInvalidSequenceNumber -> sequence number not found in the send buffer
 */
radef_RaStaReturnCode srsend_IsSequenceNumberInBuffer(const uint32_t connection_id, const uint32_t sequence_number);

/**
 * @brief Remove confirmed SafRetL messages from the send buffer from a defined sequence number.
 *
 * This function removes the message with a specific sequence number and all previous messages from the send buffer of a dedicated RaSTA connection. If the
 * passed sequence number or its predecessors are not found, nothing is done.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 * @pre Ascending sequence number order of buffer elements is guaranteed by ::srsend_AddToBuffer and ::srsend_PrepareBufferForRetr.
 *
 * @implementsReq{RASW-605} Remove from Buffer Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] confirmed_sequence_number Confirmed sequence number to be removed from send buffer. The full value range is valid and usable.
 */
void srsend_RemoveFromBuffer(const uint32_t connection_id, const uint32_t confirmed_sequence_number);

/**
 * @brief Get the number of free buffer entries.
 *
 * This function returns the amount of free entires in the send buffer of a given connection.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-598} Get Free Buffer Entries Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return number of free entries in the send buffer [messages]
 */
uint16_t srsend_GetFreeBufferEntries(const uint32_t connection_id);

/**
 * @brief Get the number of used buffer entries.
 *
 * This function returns the amount of used entires in the send buffer of a given connection.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-597} Get Used Buffer Entries Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return number of used entries in the send buffer [messages]
 */
uint16_t srsend_GetUsedBufferEntries(const uint32_t connection_id);

/**
 * @brief Get the number of messages to send from the send buffer.
 *
 * This function returns the number of not yet send messages in the send buffer of a given connection.
 *
 * @pre The send buffer module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-599} Get Number of Messages to Send Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return number of messages to send in the send buffer [messages]
 */
uint16_t srsend_GetNumberOfMessagesToSend(const uint32_t connection_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRSEND_SAFETY_RETRANSMISSION_SEND_BUFFER_H_
