/**
 * @copyright Copyright (C) 2022, SBB AG, CH-3000 Bern
 *
 * @file srdia_mock.cc
 *
 * @author TST, CSA Engineering AG, CH-4500 Solothurn
 *
 * @version fe457be274667230cc30cc95371dedfe439bab25
 *
 * @change{-,Initial version,-,-}
 *
 * @brief Mock source file for the srdia module
 */

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include "srdia_mock.hh"

// -----------------------------------------------------------------------------
// Local Variable Definitions
// -----------------------------------------------------------------------------
srdiaMock * srdiaMock::instance = NULL;

// -----------------------------------------------------------------------------
// Global Function Implementations
// -----------------------------------------------------------------------------
srdiaMock::srdiaMock() {
  instance = this;
}

srdiaMock::~srdiaMock() {
  instance = NULL;
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern "C" {

  bool srdia_AreDiagnosticTimingIntervalsValid(const uint32_t t_max, const uint32_t * diag_timing_distr_intervals){
    if(srdiaMock::getInstance() == nullptr) {
      ADD_FAILURE() << "Mock object not initialized!";
    }
    return srdiaMock::getInstance()->srdia_AreDiagnosticTimingIntervalsValid(t_max, diag_timing_distr_intervals);
  }
  void srdia_Init(const uint32_t configured_connections, const uint32_t t_max, const uint32_t n_diag_window, const uint32_t * diag_timing_distr_intervals){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_Init(configured_connections, t_max, n_diag_window, diag_timing_distr_intervals);
  }
  void srdia_InitConnectionDiagnostics(const uint32_t connection_id){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_InitConnectionDiagnostics(connection_id);
  }
  void srdia_IncSafetyCodeErrorCounter(const uint32_t connection_id){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_IncSafetyCodeErrorCounter(connection_id);
  }
  void srdia_IncAddressErrorCounter(const uint32_t connection_id){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_IncAddressErrorCounter(connection_id);
  }
  void srdia_IncTypeErrorCounter(const uint32_t connection_id){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_IncTypeErrorCounter(connection_id);
  }
  void srdia_IncSequenceNumberErrorCounter(const uint32_t connection_id){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_IncSequenceNumberErrorCounter(connection_id);
  }
  void srdia_IncConfirmedSequenceNumberErrorCounter(const uint32_t connection_id){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_IncConfirmedSequenceNumberErrorCounter(connection_id);
  }
  void srdia_UpdateConnectionDiagnostics(const uint32_t connection_id, const uint32_t round_trip_delay, const uint32_t alive_time){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_UpdateConnectionDiagnostics(connection_id, round_trip_delay, alive_time);
  }
  void srdia_SendDiagnosticNotification(const uint32_t connection_id){
    ASSERT_NE(srdiaMock::getInstance(), nullptr) << "Mock object not initialized!";
    srdiaMock::getInstance()->srdia_SendDiagnosticNotification(connection_id);
  }
}

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
