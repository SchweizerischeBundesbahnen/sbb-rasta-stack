/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file redstm_red_state_machine.c
 *
 * @author Roland Schenk, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version 6a56b6d9a6998494e007e63d764a40fdf63cbbac
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Implementation of RaSTA redundancy layer state machine.
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "redstm_red_state_machine.h"  // NOLINT(build/include_subdir)

#include <stdbool.h>
#include <stdint.h>

#include "rasta_common/raas_rasta_assert.h"
#include "rasta_common/radef_rasta_definitions.h"
#include "rasta_common/ralog_rasta_logger.h"
#include "rasta_common/rasys_rasta_system_adapter.h"
#include "redcor_red_core.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Local Macro Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Type Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------

/**
 * @brief Initialization state of the module. True, if the module is initialized.
 */
PRIVATE bool redstm_initialized = false;

/**
 * @brief Number of configured redundancy channels.
 */
PRIVATE uint32_t redstm_number_of_red_channels = 0U;

/**
 * @brief States of the redundancy channel state machines.
 */
PRIVATE redstm_RedundancyChannelStates redstm_redundancy_channel_states[RADEF_MAX_NUMBER_OF_RED_CHANNELS] = {redstm_kRedundancyChannelStateNotInitialized,
                                                                                                             redstm_kRedundancyChannelStateNotInitialized};

// -----------------------------------------------------------------------------
// Local Function Prototypes
// -----------------------------------------------------------------------------

/** @addtogroup red_sm
 * @{
 */

/**
 * @brief Process events in the closed state.
 *
 * This function processes the events in the closed state of the redundancy channel state machine, calls the appropriate functions and does the appropriate
 * state transitions.
 * This function checks at the beginning if the closed state is active, else a ::radef_kInternalError fatal error message is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification.  Valid range: 0 <= value < configured number of channels.
 * @param [in] event Event to process. All enum entries of ::redstm_RedundancyChannelEvents are valid and usable.
 */
static void ProcessStateClosedEvents(const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event);

/**
 * @brief Process events in the up state.
 *
 * This function processes the events in the up state of the redundancy channel state machine, calls the appropriate functions and does the appropriate
 * state transitions.
 * This function checks at the beginning if the up state is active, else a ::radef_kInternalError fatal error message is thrown.
 *
 * @param [in] red_channel_id Redundancy channel identification.  Valid range: 0 <= value < configured number of channels.
 * @param [in] event Event to process. All enum entries of ::redstm_RedundancyChannelEvents are valid and usable.
 */
static void ProcessStateUpEvents(const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event);

/** @}*/

// -----------------------------------------------------------------------------
// Local Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Constant Definitions
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------

void redstm_Init(const uint32_t configured_red_channels) {
  // Input parameter check
  raas_AssertTrue(!redstm_initialized, radef_kAlreadyInitialized);
  raas_AssertU32InRange(configured_red_channels, redcty_kMinNumberOfRedundancyChannels, RADEF_MAX_NUMBER_OF_RED_CHANNELS, radef_kInvalidParameter);

  redstm_number_of_red_channels = configured_red_channels;

  redstm_initialized = true;

  // Init states for all configured channels
  for (uint32_t index = 0U; index < redstm_number_of_red_channels; ++index) {
    redstm_redundancy_channel_states[index] = redstm_kRedundancyChannelStateClosed;
  }
}

void redstm_ProcessChannelStateMachine(const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event) {
  // Input parameter check
  raas_AssertTrue(redstm_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redstm_number_of_red_channels - 1U, radef_kInvalidParameter);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)redstm_kRedundancyChannelEventMin, ((uint16_t)redstm_kRedundancyChannelEventMax) - 1U,
                        radef_kInvalidParameter);

  // Process the events
  switch (redstm_redundancy_channel_states[red_channel_id]) {
    case redstm_kRedundancyChannelStateClosed:
      ProcessStateClosedEvents(red_channel_id, event);
      break;
    case redstm_kRedundancyChannelStateUp:
      ProcessStateUpEvents(red_channel_id, event);
      break;
    case redstm_kRedundancyChannelStateNotInitialized:  // fall-through
    default:
      rasys_FatalError(radef_kInternalError);
      // no coverage start
      break;
      // no coverage end
  }
}

redstm_RedundancyChannelStates redstm_GetChannelState(const uint32_t red_channel_id) {
  // Input parameter check
  raas_AssertTrue(redstm_initialized, radef_kNotInitialized);
  raas_AssertU32InRange(red_channel_id, 0U, redstm_number_of_red_channels - 1U, radef_kInvalidParameter);
  raas_AssertU16InRange((uint16_t)redstm_redundancy_channel_states[red_channel_id], (uint16_t)redstm_kRedundancyChannelStateMin,
                        ((uint16_t)redstm_kRedundancyChannelStateMax) - 1U, radef_kInternalError);

  return redstm_redundancy_channel_states[red_channel_id];
}

// -----------------------------------------------------------------------------
// Local Function Implementations
// -----------------------------------------------------------------------------

static void ProcessStateClosedEvents(const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, redstm_number_of_red_channels - 1U, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)redstm_kRedundancyChannelEventMin, ((uint16_t)redstm_kRedundancyChannelEventMax) - 1U, radef_kInternalError);
  // Make sure that we really are in closed state
  raas_AssertTrue(redstm_redundancy_channel_states[red_channel_id] == redstm_kRedundancyChannelStateClosed, radef_kInternalError);

  // Process the events
  switch (event) {
    case redstm_kRedundancyChannelEventOpen:
      redcor_InitRedundancyChannelData(red_channel_id);
      redstm_redundancy_channel_states[red_channel_id] = redstm_kRedundancyChannelStateUp;
      break;
    case redstm_kRedundancyChannelEventClose:        // fall-through
    case redstm_kRedundancyChannelEventReceiveData:  // fall-through
    case redstm_kRedundancyChannelEventSendData:     // fall-through
    case redstm_kRedundancyChannelEventDeferTimeout:
      // Nothing to do and remain in the same state
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);
      break;
      // no coverage end
  }
}

static void ProcessStateUpEvents(const uint32_t red_channel_id, const redstm_RedundancyChannelEvents event) {
  // Input parameter check
  raas_AssertU32InRange(red_channel_id, 0U, redstm_number_of_red_channels - 1U, radef_kInternalError);
  raas_AssertU16InRange((uint16_t)event, (uint16_t)redstm_kRedundancyChannelEventMin, ((uint16_t)redstm_kRedundancyChannelEventMax) - 1U, radef_kInternalError);
  // Make sure that we really are in up state
  raas_AssertTrue(redstm_redundancy_channel_states[red_channel_id] == redstm_kRedundancyChannelStateUp, radef_kInternalError);

  // Process the events
  switch (event) {
    case redstm_kRedundancyChannelEventOpen:
      // Nothing to do and remain in the same state
      break;
    case redstm_kRedundancyChannelEventClose:
      redcor_InitRedundancyChannelData(red_channel_id);  // cleanup redundancy channel data by calling the redcor_InitRedundancyChannelData() function
      redstm_redundancy_channel_states[red_channel_id] = redstm_kRedundancyChannelStateClosed;
      break;
    case redstm_kRedundancyChannelEventReceiveData:
      redcor_ProcessReceivedMessage(red_channel_id);
      // Remain in the same state
      break;
    case redstm_kRedundancyChannelEventSendData:
      redcor_SendMessage(red_channel_id);
      // Remain in the same state
      break;
    case redstm_kRedundancyChannelEventDeferTimeout:
      redcor_DeferQueueTimeout(red_channel_id);
      // Remain in the same state
      break;
      // no coverage start
    default:
      rasys_FatalError(radef_kInternalError);
      break;
      // no coverage end
  }
}
