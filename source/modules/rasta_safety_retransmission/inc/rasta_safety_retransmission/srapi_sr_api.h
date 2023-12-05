/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srapi_sr_api.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_api
 * @{
 *
 * @brief Interface of RaSTA SafRetL API.
 *
 * This module defines and implements the interface functions (like get initialization state, open & close connection, send & read data, get connection state)
 * for the application layer, as they act as entry point to use the RaSTA protocol stack.
 *
 * @implementsReq{RASW-543} Component sr_api Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SRAPI_SAFETY_RETRANSMISSION_API_H_
#define SRAPI_SAFETY_RETRANSMISSION_API_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "rasta_safety_retransmission/srcty_sr_config_types.h"

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
 * @brief Initialize SafRetL.
 *
 * This function is used to initialize the sr_api module. If the sr_api module is already initalized, a ::radef_kAlreadyInitialized error is returned. After
 * checking the configuration for validity, the pointer to the configuration is saved internally. If it is not valid, a radef_kInvalidConfiguration is returned.
 * The configuration is then used to initialize the state machine and the core module.
 *
 * @implementsReq{RASW-544} Init sr_api Function
 * @implementsReq{RASW-267} Initialization Function Structure
 * @implementsReq{RASW-292} Configuration SafRetL
 * @implementsReq{RASW-290} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-735} Init SafRetL Sequence
 *
 * @param [in] safety_retransmission_configuration Pointer to SafRetL configuration. More details about valid configuration can be found
 * directly in ::srcty_SafetyRetransmissionConfiguration. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @return ::radef_kNoError -> successful initialized
 * @return ::radef_kAlreadyInitialized -> module already initialized
 * @return ::radef_kInvalidConfiguration -> invalid configuration data
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode srapi_Init(const srcty_SafetyRetransmissionConfiguration *const safety_retransmission_configuration);

/**
 * @brief Get the initialization state of the SafRetL.
 *
 * This function is used to check the initialization state of the sr_api module.
 *
 * @implementsReq{RASW-545} Get Initialization State Function
 * @implementsReq{RASW-306} Get Initialization State Function Structure
 * @implementsReq{RASW-305} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 *
 * @return ::radef_kNoError -> successful initialized
 * @return ::radef_kNotInitialized -> not initialized
 */
radef_RaStaReturnCode srapi_GetInitializationState(void);

/**
 * @brief Open a RaSTA connection.
 *
 * This function is used to open a specific RaSTA connection. If the sr_api module is not initalized, a ::radef_kNotInitialized error is returned. The network
 * id is checked if it is known in the saved configuration. If it is not known, a ::radef_kInvalidParameter is returned. Afterwards the connection id is
 * obtained by calling the ::srcor_GetConnectionId function. If no valid connection is found, a ::radef_kInvalidParameter is returned. If all tests passed, the
 * ::srstm_ProcessConnectionStateMachine function is called with an ::srtyp_kConnEventOpen event. The connection id is passed to the application via the output
 * parameter.
 *
 * @implementsReq{RASW-546} Open Connection Function
 * @implementsReq{RASW-303} Open Connection Function Structure
 * @implementsReq{RASW-301} Sender Identification
 * @implementsReq{RASW-300} Receiver Identification
 * @implementsReq{RASW-324} Network Identification
 * @implementsReq{RASW-322} Connection Identification
 * @implementsReq{RASW-314} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-737} Open Connection Sequence
 *
 * @param [in] sender_id RaSTA sender identification. The full value range is valid and usable.
 * @param [in] receiver_id RaSTA receiver identification. The full value range is valid and usable.
 * @param [in] network_id RaSTA network identification. The full value range is valid and usable.
 * @param [out] connection_id Connection identification for the opened connection. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode srapi_OpenConnection(const uint32_t sender_id, const uint32_t receiver_id, const uint32_t network_id, uint32_t *const connection_id);

/**
 * @brief Close a RaSTA connection.
 *
 * This function is used to close a specific RaSTA connection. If the sr_api module is not initalized, a ::radef_kNotInitialized error is returned. If
 * initialized, the connection id is checked if it is in a valid range, otherwise a ::radef_kInvalidParameter is returned. If everything is good, the detailed
 * disconnection reason is set with the ::srcor_SetDiscDetailedReason function. Then the ::srstm_ProcessConnectionStateMachine function is called with
 * ::srtyp_kConnEventClose event to close the Rasta connection.
 *
 * @implementsReq{RASW-547} Close Connection Function
 * @implementsReq{RASW-318} Close Connection Function Structure
 * @implementsReq{RASW-316} Connection Identification
 * @implementsReq{RASW-309} Detailed Reason
 * @implementsReq{RASW-308} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-739} Close Connection Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] detailed_reason Detailed reason for disconnection. The full value range is valid and usable.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode srapi_CloseConnection(const uint32_t connection_id, const uint16_t detailed_reason);

/**
 * @brief Send a RaSTA data message.
 *
 * This function is used to send data over a specific RaSTA connection. If the sr_api module is not initalized, a ::radef_kNotInitialized error is returned. If
 * initialized, the connection id is checked if it is in a valid range, otherwise a ::radef_kInvalidParameter is returned. If the state machine is in closed
 * state, a ::radef_kInvalidOperationInCurrentState is returned, since no data can be send when the connection is closed. If everything is good, it is checked
 * if there are free entries in the send buffer with the ::srsend_GetFreeBufferEntries function. In case the buffer is full, a ::radef_kSendBufferFull error is
 * returned. If there is space, the data payload is buffered with the ::srcor_WriteMessagePayloadToTemporaryBuffer function before calling the
 * ::srstm_ProcessConnectionStateMachine function with an ::srtyp_kConnEventSendData event to process the buffered data message and send it to the connection
 * party.
 *
 * @implementsReq{RASW-548} Send Data Function
 * @implementsReq{RASW-310} Send Data Function Structure
 * @implementsReq{RASW-275} Connection Identification
 * @implementsReq{RASW-277} Message Size
 * @implementsReq{RASW-276} Message Data
 * @implementsReq{RASW-272} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-741} Send Data Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] message_size Size of the message data [bytes]. Valid range: ::srcty_kMinSrLayerPayloadDataSize <= value <=
 * ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE. If the value is outside this range, a ::radef_kInvalidMessageSize error is returned.
 * @param [in] message_data Pointer to message data array. If the pointer is NULL, a ::radef_kInvalidParameter error is returned. For the message data the
 * full value range is valid and usable.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 * @return ::radef_kInvalidMessageSize -> invalid message size
 * @return ::radef_kSendBufferFull -> send buffer full
 * @return ::radef_kInvalidOperationInCurrentState -> state machine in closed state
 */
radef_RaStaReturnCode srapi_SendData(const uint32_t connection_id, const uint16_t message_size, const uint8_t *const message_data);

/**
 * @brief Read the data of a received RaSTA message.
 *
 * This function is used to read data from a specific RaSTA connection. If the sr_api module is not initalized, a ::radef_kNotInitialized error is returned. If
 * initialized, the connection id is checked if it is in a valid range, otherwise a ::radef_kInvalidParameter is returned. If the provided buffer size is not
 * big enough for the next message from the received buffer, a ::radef_kInvalidBufferSize is returned. If everything is good, a message is read with
 * ::srrece_ReadFromBuffer. If no message is available, a ::radef_kNoMessageReceived is returned. Otherwise the received data message is copied into the
 * provided parameters (message_size & message_data).
 *
 * @implementsReq{RASW-549} Read Data Function
 * @implementsReq{RASW-274} Read Data Function Structure
 * @implementsReq{RASW-273} Connection Identification
 * @implementsReq{RASW-269} Buffer Size
 * @implementsReq{RASW-268} Message Size
 * @implementsReq{RASW-270} Message Buffer
 * @implementsReq{RASW-286} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-743} Read Data Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] buffer_size Size of the buffer provided to parameter message_buffer [bytes]. Valid range: ::srcty_kMinSrLayerPayloadDataSize <= value <=
 * ::RADEF_MAX_SR_LAYER_PAYLOAD_DATA_SIZE. Any value in this range can be used, must be large enough to store the received message.
 * @param [out] message_size Pointer to the size of the received message data [bytes]. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @param [out] message_buffer Pointer to a buffer for saving the received message. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNoMessageReceived -> no message received (used for polling)
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 * @return ::radef_kInvalidBufferSize -> invalid buffer size
 */
radef_RaStaReturnCode srapi_ReadData(const uint32_t connection_id, const uint16_t buffer_size, uint16_t *const message_size, uint8_t *const message_buffer);

/**
 * @brief Get the state of a connection.
 *
 * This function is used to get the connection state of a specific RaSTA connection. If the sr_api module is not initalized, a ::radef_kNotInitialized error is
 * returned. If initialized, the connection id is checked if it is in a valid range, otherwise a ::radef_kInvalidParameter is returned. If everything is good,
 * the connection state is obtained from the state machine with the ::srstm_GetConnectionState function call. With the ::srcor_GetBufferSizeAndUtilisation
 * further information is collected to then be returned to the caller.
 *
 * @implementsReq{RASW-550} Get Connection State Function
 * @implementsReq{RASW-288} Get Connection State Function Structure
 * @implementsReq{RASW-287} Connection Identification
 * @implementsReq{RASW-282} Connection State
 * @implementsReq{RASW-281} Buffer Utilisation
 * @implementsReq{RASW-284} Opposite Buffer Size
 * @implementsReq{RASW-283} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-745} Get Connection State Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [out] connection_state State of the connection. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @param [out] buffer_utilisation Struct with own buffer utilisation data. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @param [out] opposite_buffer_size Size of the receive buffer of the connection party. If the pointer is NULL, a ::radef_kInvalidParameter error is returned.
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 * @return ::radef_kInvalidParameter -> invalid parameter
 */
radef_RaStaReturnCode srapi_GetConnectionState(const uint32_t connection_id, sraty_ConnectionStates *const connection_state,
                                               sraty_BufferUtilisation *const buffer_utilisation, uint16_t *const opposite_buffer_size);

/**
 * @brief Check SafRetL timings.
 *
 * This function is used to check the timings of the SafRetL. If the sr_api module is not initalized, a ::radef_kNotInitialized error is
 * returned. It must be called periodically. It has 3 main uses:
 * - Received messages polling (read all available messages from the adapter layer and process them in the state machine). Read and process new messages while
 * ::srcor_IsReceivedMsgPendingAndBuffersNotFull check return true.
 * - Send pending messages (send not yet send messages from the send buffer)
 * - Check timings for message timeout & HB interval
 *   - Check with ::srcor_IsMessageTimeout if a message timeout occurred. If true, send a ::srtyp_kConnEventTimeout event to the state machine.
 *   - Check with ::srcor_IsHeartbeatInterval if the heartbeat interval elapsed and also if there are not send messages in the send buffer. If true, send a
 * ::srtyp_kConnEventSendHb event to the state machine.
 * .
 * These checks are done for all available connections.
 *
 * @implementsReq{RASW-551} Check Timings Function
 * @implementsReq{RASW-319} Check Timings Function Structure
 * @implementsReq{RASW-317} Error Code
 * @implementsReq{RASW-503} Enum RaSta Return Code Usage
 * @implementsReq{RASW-753} Check Timings Sequence
 * @implementsReq{RASW-769} Received Message Polling Sequence
 * @implementsReq{RASW-773} Check Timings Message Timeout / Heartbeat Sequence
 *
 * @remark This function must be called periodically, in a appropriate interval related to the configured timings.
 *
 * @return ::radef_kNoError -> successful operation
 * @return ::radef_kNotInitialized -> module not initialized
 */
radef_RaStaReturnCode srapi_CheckTimings(void);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SRAPI_SAFETY_RETRANSMISSION_API_H_
