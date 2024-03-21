/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srnot_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srnot module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRNOT_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRNOT_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta srnot class
 */
class srnotMock {
private:
  static srnotMock *instance;  ///< srnotMock instance

public:
  /**
  * @brief srnotMock constructor
  */
  srnotMock();
  /**
  * @brief srnotMock destructor
  */
  virtual ~srnotMock();

  /**
  * @brief Mock Method object for the srnot_MessageReceivedNotification function
  */
  MOCK_METHOD(void, srnot_MessageReceivedNotification, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srnot_ConnectionStateNotification function
  */
  MOCK_METHOD(void, srnot_ConnectionStateNotification, (const uint32_t connection_id, const sraty_ConnectionStates connection_state, const sraty_BufferUtilisation buffer_utilisation, const uint16_t opposite_buffer_size, const sraty_DiscReason disconnect_reason, const uint16_t detailed_disconnect_reason));

  /**
  * @brief Mock Method object for the srnot_SrDiagnosticNotification function
  */
  MOCK_METHOD(void, srnot_SrDiagnosticNotification, (const uint32_t connection_id, const sraty_ConnectionDiagnosticData connection_diagnostic_data));

  /**
  * @brief Mock Method object for the srnot_RedDiagnosticNotification function
  */
  MOCK_METHOD(void, srnot_RedDiagnosticNotification, (const uint32_t connection_id, const sraty_RedundancyChannelDiagnosticData redundancy_channel_diagnostic_data));

  /**
  * @brief Get the Instance object
  *
  * @return srnotMock*
  */
  static srnotMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRNOT_MOCK_HH_
