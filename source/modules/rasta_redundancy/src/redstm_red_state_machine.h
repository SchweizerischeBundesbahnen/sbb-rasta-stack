/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redstm_red_state_machine.h
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @addtogroup red_sm
 * @{
 *
 * @brief Interface of RaSTA redundancy layer state machine module.
 *
 * This module implements the redundancy layer state machine which handles the closed and up states of the redundancy channels.
 */
#ifndef REDSTM_RED_STATE_MACHINE_H_
#define REDSTM_RED_STATE_MACHINE_H_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <stdint.h>

// -----------------------------------------------------------------------------
// Global Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Type Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Enum for the states of a redundancy channel state machine.
 */
//lint -esym(849, redstm_kRedundancyChannelStateMin) (used only for parameter range checking)
//lint -esym(788, redstm_kRedundancyChannelStateMax) (used only for parameter range checking)
typedef enum {
  redstm_kRedundancyChannelStateMin = 0U,             ///< Min value for redundancy channel state enum
  redstm_kRedundancyChannelStateNotInitialized = 0U,  ///< not initialized
  redstm_kRedundancyChannelStateClosed,               ///< redundancy channel closed
  redstm_kRedundancyChannelStateUp,                   ///< redundancy channel up
  redstm_kRedundancyChannelStateMax                   ///< Max value for redundancy channel state enum
} redstm_RedundancyChannelStates;

/**
 * @brief Enum for the events of a redundancy channel state machine.
 */
//lint -esym(849, redstm_kRedundancyChannelEventMin) (used only for parameter range checking)
//lint -esym(788, redstm_kRedundancyChannelEventMax) (used only for parameter range checking)
typedef enum {
  redstm_kRedundancyChannelEventMin = 0U,      ///< Min value for redundancy channel events enum
  redstm_kRedundancyChannelEventOpen = 0U,     ///< open redundancy channel event
  redstm_kRedundancyChannelEventClose,         ///< close redundancy channel event
  redstm_kRedundancyChannelEventReceiveData,   ///< receive data event
  redstm_kRedundancyChannelEventSendData,      ///< send data event
  redstm_kRedundancyChannelEventDeferTimeout,  ///< defer queue timeout event
  redstm_kRedundancyChannelEventMax            ///< Max value for redundancy channel events enum
} redstm_RedundancyChannelEvents;

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
 * @brief Initialize the RedL state machine module.
 *
 * This function is used to initialize the state machine module. It saves the passed number of redundancy channels. For all configured channels the state
 * machine is properly initialized. A fatal error is raised, if this function is called multiple times.
 *
 * @pre The state machine module must not be initialized, otherwise a ::radef_kAlreadyInitialized fatal error is thrown.
 *
 * @param [in] configured_red_channels Number of configured redundancy channels. Valid range: 1 <= value <= ::RADEF_MAX_NUMBER_OF_RED_CHANNELS.
 */
void redstm_Init(const uint32_t configured_red_channels);

/**
 * @brief Process redundancy channel state machine.
 *
 * This function processes the events of the redundancy channel state machine, calls the appropriate functions and does the appropriate state transitions. All
 * details can be found in the image below or in figure 14 & 15 of the standard "Elektrische Bahn-Signalanlagen – Teil 200: Sicheres Übertragungsprotokoll RaSTA
 * nach DIN EN 50159 (VDE0831-159); Juni 2015":
 *
 * @image html rasta_redundancy_stm.png "RaSTA Redundancy Channel State Machine" width=30%
 * @image latex rasta_redundancy_stm.png "RaSTA Redundancy Channel State Machine" width=10cm
 *
 * @pre The state machine module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification.  Valid range: 0 <= value < configured number of channels.
 * @param [in] event Event to process. All enum entries of ::redstm_RedundancyChannelEvents are valid and usable.
 */
void redstm_ProcessChannelStateMachine(const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event);

/**
 * @brief Return the state of a redundancy channel state machine.
 *
 * @pre The state machine module must be initialized, otherwise a ::radef_kNotInitialized fatal error is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification.  Valid range: 0 <= value < configured number of channels.
 * @return State of the redundancy channel state machine. All enum entries of ::redstm_RedundancyChannelStates are valid and usable.
 */
redstm_RedundancyChannelStates redstm_GetChannelState(const uint32_t red_channel_id);

/** @}*/

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // REDSTM_RED_STATE_MACHINE_H_
