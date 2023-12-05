/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redmsg_red_messages.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_messages
 * @{
 *
 * @brief Interface of RaSTA redundancy layer messages module.
 *
 * This module provides all needed functionality for redundancy layer messages.
 * This contains the following:
 * - validate a RedL message
 * - create a new RedL message
 * - extract information form a RedL message
 */
#ifndef REDMSG_RED_MESSAGES_H_
#define REDMSG_RED_MESSAGES_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_redundancy/redcty_red_config_types.h"
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
 * @brief Initialize RedL messages module.
 *
 * This function is used to initialize the messages module. It saves the passed check code type and calls the initialization of the CRC module. A fatal error is
 * raised, if this function is called multiple times.
 *
 * @pre The messages module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @param [in] configured_check_code_type Configured check code type. All enum entries of ::redcty_CheckCodeType are valid and usable.
 */
void redmsg_Init(const redcty_CheckCodeType configured_check_code_type);

/**
 * @brief Create a new redundancy layer message and calculate the check code.
 *
 * This function creates a new redundancy layer message:
 * - Calculate and set the message length according to the payload size and the check code type
 * - Initialize the reserve data bytes to 0
 * - Set the message sequence number
 * - Copy the message payload to the message
 * - Calculate and set the check code according to the check code type
 * .
 * The message_payload->payload_size must be in the range from ::RADEF_SR_LAYER_MESSAGE_HEADER_SIZE to ::RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInvalidParameter fatal error message is thrown.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] sequence_number sequence number of the new message. The full value range is valid and usable.
 * @param [in] message_payload pinter to message payload data struct. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] redundancy_message pointer to struct containing the new created message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is
 * thrown.
 */
void redmsg_CreateMessage(const uint32_t sequence_number, const redtyp_RedundancyMessagePayload *const message_payload,
                          redtyp_RedundancyMessage *const redundancy_message);

/**
 * @brief Check the check code of a redundancy layer message.
 *
 * This function checks the validity of a provided RedL message by checking the configured check code. For the check code type ::redcty_kCheckCodeA (no check
 * code) it always returns ::radef_kNoError.
 * The redundancy_message->message_size must be in the range from ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE to ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInvalidParameter fatal error message is thrown.
 * The redundancy_message->message_size must be equal to the message size stored in the message, else a ::radef_kInvalidParameter fatal error message is thrown.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] redundancy_message pointer to message struct. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @return ::radef_kNoError -> check code is OK
 * @return ::radef_kInvalidMessageCrc -> wrong check code
 */
radef_RaStaReturnCode redmsg_CheckMessageCrc(const redtyp_RedundancyMessage *const redundancy_message);

/**
 * @brief Get the sequence number of a redundancy layer message.
 *
 * This function extracts the sequence number from the passed RedL message.
 * The redundancy_message->message_size must be in the range from ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE to ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInvalidParameter fatal error message is thrown.
 * The redundancy_message->message_size must be equal to the message size stored in the message, else a ::radef_kInvalidParameter fatal error message is thrown.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] redundancy_message pointer to struct containing the message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @return sequence number of the message
 */
uint32_t redmsg_GetMessageSequenceNumber(const redtyp_RedundancyMessage *const redundancy_message);

/**
 * @brief Get the payload of a redundancy message.
 *
 * This function extracts the message payload from the passed RedL message and sets the message_payload->payload_size.
 * The redundancy_message->message_size must be in the range from ::RADEF_MIN_RED_LAYER_PDU_MESSAGE_SIZE to ::RADEF_MAX_RED_LAYER_PDU_MESSAGE_SIZE, else a
 * ::radef_kInvalidParameter fatal error message is thrown.
 * The redundancy_message->message_size must be equal to the message size stored in the message, else a ::radef_kInvalidParameter fatal error message is thrown.
 * The internally calculated message payload size must be in the range from RADEF_SR_LAYER_MESSAGE_HEADER_SIZE to RADEF_MAX_SR_LAYER_PDU_MESSAGE_SIZE, else
 * a ::radef_kInternalError fatal error message is thrown.
 *
 * @pre The messages module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] redundancy_message pointer to struct containing the message. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [out] message_payload pointer to struct for the message payload. If the pointer is NULL, a ::radef_kInvalidParameter fatal error is thrown.
 */
void redmsg_GetMessagePayload(const redtyp_RedundancyMessage *const redundancy_message, redtyp_RedundancyMessagePayload *const message_payload);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDMSG_RED_MESSAGES_H_
