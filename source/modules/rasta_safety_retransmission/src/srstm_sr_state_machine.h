/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srstm_sr_state_machine.h
 *
 * @author Nicolas Andres, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version e6f89b9b2c785cdd2a74219db6d61bc80685ff37
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup sr_sm
 * @{
 *
 * @brief Interface of RaSTA SafRetL state machine module.
 *
 * This module provides all needed functionality to reflect the logic of the state machine. The state machine reacts on:
 * - occurring events
 * - function calls from the application layer
 * - notifications from the SafRetL adapter layer
 * .
 * The state machine evaluates them, initiates the necessary actions and changes the state.
 *
 * @implementsReq{RASW-559} Component sr_state_machine Overview
 * @implementsReq{RASW-518} Safety and Retransmission Layer Safety Integrity Level
 * @implementsReq{RASW-520} Error Handling
 * @implementsReq{RASW-521} Input Parameter Check
 */
#ifndef SR_SAFETY_RETRANSMISSION_STATE_MACHINE_H_
#define SR_SAFETY_RETRANSMISSION_STATE_MACHINE_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

#include "rasta_safety_retransmission/sraty_sr_api_types.h"
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
 * @brief Initialize SafRetL state machine module.
 *
 * This function is used to initialize the state machine module. It saves the passed number of configured connections. A fatal error is raised, if this
 * function is called multiple times.
 *
 * @pre The state machine module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-562} Init sr_state_machine Function
 *
 * @param [in] configured_connections Number of configured RaSTA connections. Valid range: 1 <= value <= ::RADEF_MAX_NUMBER_OF_RASTA_CONNECTIONS.
 */
void srstm_Init(const uint32_t configured_connections);

/**
 * @brief Process RaSTA connection state machine.
 *
 * This function is used to process all incoming events of the state machine and launch the needed actions. All details can be found in the image below or in
 * table 18 of the standard "Elektrische Bahn-Signalanlagen – Teil 200: Sicheres Übertragungsprotokoll RaSTA nach DIN EN 50159 (VDE0831-159); Juni 2015":
 *
 * @image html rasta_safety_stm.png "RaSTA SafRetL State Machine" width=75%
 * @image latex rasta_safety_stm.png "RaSTA SafRetL State Machine" width=15cm
 *
 * @pre The state machine module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-563} Process Connection State Machine Function
 * @implementsReq{RASW-560} sr_state_machine Events
 * @implementsReq{RASW-749} Connection State Notification Sequence
 * @implementsReq{RASW-755} Process State Machine Receive ConnReq Message Sequence
 * @implementsReq{RASW-757} Process State Machine Receive ConnResp Message Sequence
 * @implementsReq{RASW-759} Process State Machine Receive Data Message (in Sequence) Sequence
 * @implementsReq{RASW-761} Process State Machine Receive RetrReq Message (in Sequence) Sequence
 * @implementsReq{RASW-763} Process State Machine Receive RetrReq Message (not in Sequence) Sequence
 * @implementsReq{RASW-765} Send Pending Messages Sequence
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @param [in] event Received Event to process. The event has a valid range from ::srtyp_kConnEventMin <= value < ::srtyp_kConnEventMax. If the value is outside
 * this range, a ::radef_kInvalidParameter fatal error is thrown.
 * @param [in] sequence_number_in_seq true, if the received sequence number is in sequence
 * @param [in] confirmed_time_stamp_in_seq true, if the confirmed timestamp is in sequence
 */
void srstm_ProcessConnectionStateMachine(const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq,
                                         const bool confirmed_time_stamp_in_seq);

/**
 * @brief Return the state of a RaSTA connection state machine.
 *
 * This function is used to get the connection state of a dedicated RaSTA connection.
 *
 * @pre The state machine module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @implementsReq{RASW-561} Get Connection State Function
 *
 * @param [in] connection_id RaSTA connection identification. Valid range: 0 <= value < configured number of connections.
 * @return state of the RaSTA connection state machine
 */
sraty_ConnectionStates srstm_GetConnectionState(const uint32_t connection_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // SR_SAFETY_RETRANSMISSION_STATE_MACHINE_H_
