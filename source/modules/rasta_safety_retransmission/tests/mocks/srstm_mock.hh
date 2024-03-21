/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srstm_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srstm module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRSTM_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRSTM_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"
#include "srtyp_sr_types.h"  // NOLINT(build/include_subdir)

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta srstm class
 */
class srstmMock {
private:
  static srstmMock *instance;  ///< srstmMock instance

public:
  /**
  * @brief srstmMock constructor
  */
  srstmMock();
  /**
  * @brief srstmMock destructor
  */
  virtual ~srstmMock();

  /**
  * @brief Mock Method object for the srstm_Init function
  */
  MOCK_METHOD(void, srstm_Init, (const uint32_t configured_connections));

  /**
  * @brief Mock Method object for the srstm_ProcessConnectionStateMachine function
  */
  MOCK_METHOD(void, srstm_ProcessConnectionStateMachine, (const uint32_t connection_id, const srtyp_ConnectionEvents event, const bool sequence_number_in_seq, const bool confirmed_time_stamp_in_seq));

  /**
  * @brief Mock Method object for the srstm_GetConnectionState function
  */
  MOCK_METHOD(sraty_ConnectionStates, srstm_GetConnectionState, (const uint32_t connection_id));

  /**
  * @brief Get the Instance object
  *
  * @return srstmMock*
  */
  static srstmMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRSTM_MOCK_HH_
