/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srdia_mock.hh
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock header file for the srdia module
 */

#ifndef SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRDIA_MOCK_HH_
#define SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRDIA_MOCK_HH_

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "gmock/gmock.h"
#include "rasta_safety_retransmission/sraty_sr_api_types.h"

// -----------------------------------------------------------------------------
// Mock class
// -----------------------------------------------------------------------------
/**
 * @brief Mock class for the rasta srdia class
 */
class srdiaMock {
private:
  static srdiaMock *instance;  ///< srdiaMock instance

public:
  /**
  * @brief srdiaMock constructor
  */
  srdiaMock();
  /**
  * @brief srdiaMock destructor
  */
  virtual ~srdiaMock();

  /**
  * @brief Mock Method object for the srdia_AreDiagnosticTimingIntervalsValid function
  */
  MOCK_METHOD(bool, srdia_AreDiagnosticTimingIntervalsValid, (const uint32_t t_max, const uint32_t * diag_timing_distr_intervals));

  /**
  * @brief Mock Method object for the srdia_Init function
  */
  MOCK_METHOD(void, srdia_Init, (const uint32_t configured_connections, const uint32_t t_max, const uint32_t n_diag_window, const uint32_t * diag_timing_distr_intervals));

  /**
  * @brief Mock Method object for the srdia_InitConnectionDiagnostics function
  */
  MOCK_METHOD(void, srdia_InitConnectionDiagnostics, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srdia_IncSafetyCodeErrorCounter function
  */
  MOCK_METHOD(void, srdia_IncSafetyCodeErrorCounter, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srdia_IncAddressErrorCounter function
  */
  MOCK_METHOD(void, srdia_IncAddressErrorCounter, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srdia_IncTypeErrorCounter function
  */
  MOCK_METHOD(void, srdia_IncTypeErrorCounter, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srdia_IncSequenceNumberErrorCounter function
  */
  MOCK_METHOD(void, srdia_IncSequenceNumberErrorCounter, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srdia_IncConfirmedSequenceNumberErrorCounter function
  */
  MOCK_METHOD(void, srdia_IncConfirmedSequenceNumberErrorCounter, (const uint32_t connection_id));

  /**
  * @brief Mock Method object for the srdia_UpdateConnectionDiagnostics function
  */
  MOCK_METHOD(void, srdia_UpdateConnectionDiagnostics, (const uint32_t connection_id, const uint32_t round_trip_delay, const uint32_t alive_time));

  /**
  * @brief Mock Method object for the srdia_SendDiagnosticNotification function
  */
  MOCK_METHOD(void, srdia_SendDiagnosticNotification, (const uint32_t connection_id));

  /**
  * @brief Get the Instance object
  *
  * @return srdiaMock*
  */
  static srdiaMock *getInstance() {
    return instance;
  }
};

#endif // SOURCE_MODULES_RASTA_SAFETY_RETRANSMISSION_TESTS_MOCKS_SRDIA_MOCK_HH_
